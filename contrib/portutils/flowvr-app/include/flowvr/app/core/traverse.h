/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                     Application Library                         *
 *                                                                 *
 *-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
 * INRIA                                                           *
 * ALL RIGHTS RESERVED.	                                          *
 *                                                                 *
 * This source is covered by the GNU LGPL, please refer to the     *
 * COPYING file for further information.                           *
 *                                                                 *
 *-----------------------------------------------------------------*
 *                                                                 *
 *  Original Contributors:                                         *
 *    Jeremie Allard,                                              *
 *    Thomas Arcila,                                               *
 *    Jean-Denis Lesage.                                           *	
 *    Clement Menier,                                              *
 *    Bruno Raffin                                                 *
 *                                                                 *
 *******************************************************************
 *                                                                 *
 *	File : flowvr/app/core/traverse.h                          *
 *                                                                 *
 *	Contact : Jean-Denis Lesage                                *
 *                                                                 *
 ******************************************************************/
#ifndef __FLOWVR_APP__TRAVERSE_H
#define __FLOWVR_APP__TRAVERSE_H

#include "flowvr/app/core/component.h"
#include "flowvr/app/core/controller.h"
#include "flowvr/app/core/host.h"
#include <map>
#include <list>

namespace flowvr { namespace app {

    /**
     * \file traverse.h
     * \brief  Define different type of traverse
     * The follwing types of traverses are available: 
     *  - a fixed point traverse for controller without input and output put 
     *  - one pass traverses for controllers somes with input and/or ouput
     */
	
	

///////////////////////////////////////////////////////////////////
//
//  One pass  traverses for controllers with inpout and/or ouputs
// 
///////////////////////////////////////////////////////////////////
    /**
     * \brief the result structure. Use by default the += operator.
     */
    template<typename Out>
        struct TraverseResult
    {
            Out result;
            Component* c;

            TraverseResult() : result(), c(NULL) {};
            TraverseResult(const Out& init) : result(init), c(NULL) {};

            void operator() (const TraverseResult<Out>& r) 
            { 
                result += r.result;
            };
    };

    /**
     * \brief the result structure with pointer. Use by default the += operator. It must be a non-sense in common case. You should redefine yours.
     */
    template<typename Out>
        struct TraverseResult<Out*>
    {
            Out* result;
            Component* c;

            TraverseResult() : result(NULL), c(NULL) {};
            TraverseResult(Out* init) : result(init), c(NULL) {};
            //		~TraverseResult() { delete result;};

            void operator() (const TraverseResult<Out*>& r) 
            { 
                if (result)
                    result += r.result;
                else
                    result = r.result;
            };
    };

    /**
     * \brief the result structure for the  map to Hosts.
     */
    template <>
        struct TraverseResult<HostMap >
    {
            HostMap result;	///< result map
            Component* c;	///< current component

            TraverseResult() : result(), c(NULL) {};
            TraverseResult(const HostMap init) : result(init), c(NULL) {};

            void operator() (const TraverseResult<HostMap>& r)
            { 
                result.insert(r.result.begin(), r.result.end());
            };

    };

    /**
     * \brief the result structure for XML objects. It makes the union between two XML objects.
     */
    template <>
        struct TraverseResult<xml::DOMElement*>
    {
            xml::DOMElement* result;	///< pointer to the result
            Component* c;	///< current component

            /**
             * \brief standard constructor
             */
            TraverseResult() : result(new xml::DOMElement("root")), c(NULL) {};
            /**
             * \brief constructor with a XML
             * \param init pointer to the first XML object
             */
            TraverseResult(xml::DOMElement* init)
            : result(new xml::DOMElement("network"))
            , c(NULL)
            {
                if (init)
                    result->LinkEndChild(init);
            }

            TraverseResult( const TraverseResult &other )
            : result( new xml::DOMElement( *other.result ) )
            , c(other.c)
            {

            }

            void operator=(const TraverseResult &other )
            {
            	result = new xml::DOMElement( *other.result );
            	c = other.c;
            }


//            virtual ~TraverseResult()
//            {
//            	delete result;
//            }

            /**
             * \brief make the union
             * \param r the new XML object
             */
            void operator() (const TraverseResult<xml::DOMElement*>& r) 
            {
                if (r.result->LastChild())
                    {
                        for (xml::DOMElement* it = (xml::DOMElement*) r.result->FirstChild(); it != (xml::DOMElement*) r.result->LastChild(); it = (xml::DOMElement*) r.result->IterateChildren(it))
                            {
                                result->LinkEndChild(it->Clone());
                            }
                        if (r.result->LastChild())
                            result->LinkEndChild((xml::DOMElement*) r.result->LastChild()->Clone());
                    }
            }
    };

    /**
     * \brief the result structure for void returning methods. Make nothing, only for interface compatibility.
     */
    template<>
        struct TraverseResult<void>
    {
            Component* c;

            TraverseResult() : c(NULL) {};
    };


    /**
     * \brief one pass  traverse with basic controller (ie In->Out). The parameter is the same for all components
     */
    template<typename Out, typename In>
        struct Traverse
    {
            /**
             * \brief the part of algorithm who make the recursivity.
             */
            struct FunctorTraverse
            {
                    TraverseResult<Out> result;	///< result
                    Controller<Out, In>* control;	///< the controller
                    In parameter;	///< the parameter to set

                    /**
                     * \brief constructor
                     * \param control_ the controller
                     * \param parameter_ the parameter to set
                     */
                    FunctorTraverse(Controller<Out, In>* control_, const In& parameter_) : result(), control(control_), parameter(parameter_) {};

                    /**
                     * \brief the recursive part of the algorithm
                     */
                    void operator() (Component* c)
                    {
                        Traverse<Out, In> t;
                        result(t(c, control,parameter));
                    };
            };

            /**
             * \brief main algorithm
             * \param comp the root of the components tree
             * \param control the controller to apply
             * \param parameter the parameter to set
             * \returns structure with the result
             */
            TraverseResult<Out> operator() (Component* comp, Controller<Out, In>* control, const In& parameter)
            {


                FunctorTraverse resultAcc(control, parameter);
                control->parameter = parameter;
                comp->applyController(control);
                resultAcc.result(control->result);
                // test if component is composite
                Composite* compComposite = dynamic_cast<Composite*>(comp);
                if (compComposite)
                    {
                        // traverse through children
                        resultAcc = std::for_each(compComposite->getComponentBegin(), compComposite->getComponentEnd(), resultAcc);
                    }
                return resultAcc.result;
            };
    };
	
    /**
     * \brief one pass  traverse with function controller (ie void->Out)
     */
    template<typename Out>
        struct Traverse<Out, void>
    {
            bool ignore;  ///< if true exceptions are catch and directly printed as error messages. This way the traverses is executed on all components, producing an ouput. If false the traverse stops when the first exception occurs.
            bool erroroccured; ///< used only if ignore=true. In this case it  becomes true if at least one expcetion was thrown (but caught an printed as an error messgage). Enable to know that erorrs occured. 
			
            /**
             * \brief default behaviour : don't ignore error
             */
            Traverse<Out,void>() : ignore(false),erroroccured(false) {};
			
            /**
             * \brief the part of algorithm who make the recursivity
             */
            struct FunctorTraverse
            {
                    bool ignore ;
                    bool erroroccured; ///< turned on if at least one error occured

                    TraverseResult<Out> result;	///< result of the subtree
                    Controller<Out, void>* control;	///< the controller
                    /**
                     * \brief constructor
                     * \param control_ the controller
                     */
                    FunctorTraverse(Controller<Out, void>* control_) : ignore(false), result(), control(control_),erroroccured(false) {};

                    /**
                     * \brief the recursive part of the algorithm
                     */
                    void operator() (Component* c)
                    {
                        Traverse<Out, void> t;
                        t.ignore = ignore;
                        result(t(c, control));
                        if (t.erroroccured) erroroccured=true;
                    };
            };

            /**
             * \brief main algorithm
             * \param comp the root of the components tree
             * \param control the controller to apply
             * \returns a structure with the result
             */
            TraverseResult<Out> operator() (Component* comp, Controller<Out, void>* control)
            {

                // test if component is composite
                FunctorTraverse resultAcc(control);
                resultAcc.ignore = ignore;
                try
                    {
                        comp->applyController(control);
                        resultAcc.result(control->result);
                    }
                catch(const CustomException& e)
                    {
                        if (ignore)
                            {
                                // Just print error messages and keep on moving.
                                Msg::errorException(e);
                                erroroccured = true;
                            }
                        else
                            {
                                throw e;
                            }
                    }
                Composite* compComposite = dynamic_cast<Composite*>(comp);
                if (compComposite)
                    {
                        // traverse through children
                        resultAcc = std::for_each(compComposite->getComponentBegin(), compComposite->getComponentEnd(), resultAcc);
                        if (resultAcc.erroroccured) erroroccured=true;
                    }

                return resultAcc.result;
            };
    };


    /**
     * \brief one pass traverse dedicated to @see HierarchyXMLBuild() controller
     */
    struct TraverseXMLHierarchy
    {
            Component* comp; ///< root of traverse algorithm
			
            /**
             * \brief pass comp_ as a parameter
             * \param comp_ the root 
             */
            TraverseXMLHierarchy(Component* comp_) : comp(comp_) {}
			
            /**
             * \brief compute hierarchical xml description
             * \return the fractal-like ADL of the component
             */
            xml::DOMElement* operator() ()
            {
                xml::DOMElement* result =  comp->HierarchyXMLBuild();
                Composite* composite = dynamic_cast<Composite*>(comp);
                if(composite != NULL)
                    {
                        for(Composite::ConstpComponentIterator it = composite->getComponentBegin(); it != composite->getComponentEnd(); ++it)
                            {
                                TraverseXMLHierarchy t(*it);
                                xml::DOMElement* resultChild = t();
                                result->LinkEndChild(resultChild);
                            }
                    }
                return result;
            }
    };


///////////////////////////////////////////////////////////////////
//
//  One pass  and fixed point  traverses for controllers without  inpout and output
// 
///////////////////////////////////////////////////////////////////


		
    /**
     * \brief One pass traverse for controller without inpout or output arguments. 
     *  It applies the controller to each component from top to bottom of the component hierarchy.
     *  Once a controller call throws an exception the execution stops. 
     *  There is no retry like with the @see TraversePointFixe traverse
     *  It should thus be used only for controllers that can be applied in any order on the component hierarchy. 
     *  Otherwise use the @see TraversePointFixe.
     *
     *  This traverse is  cheaper regarding the number of operations than a fixed point traverse.
     */  
    struct TraverseOnePass
    {
            std::list<Component*> toDoComponent; ///< Components to be traversed
            InterfaceController* control; ///< Controller to apply

            /**
             * \brief constructor
             * \param comp pointer to component root. This is the starting point of the traverse.
             * \param control_ the controller to apply
             */
            TraverseOnePass(Component* comp, InterfaceController* control_) :
                toDoComponent(1, comp), control(control_)
            {
            }

            /**
             * \brief this method launch the traverse.
             * \param comp this component is the starting point of the traverse. if NULL then use toDoComponent as starting points.
             */
            void operator()(Component* comp = NULL)
            {
                if (comp != NULL)
                    {
                        toDoComponent.clear();
                        toDoComponent.push_back(comp);
                    }

                while (!toDoComponent.empty())
                    {
                        // FIFO strategy
                        Component* current = toDoComponent.front();
                        toDoComponent.pop_front();
                        current->applyController(control);
                        if (dynamic_cast<Composite*>(current)) // add child objects to the todo list.
                            toDoComponent.insert(
                                                 toDoComponent.end(),
                                                 ((Composite*)current)->getComponentBegin(),
                                                 ((Composite*)current)->getComponentEnd());
                    }
            }

    };

    /**
     * \brief Fixed point Traverse applying  a controller without inpout or output.
     * This traverse tries to apply the controller on all available components. The traverse stops when the controller
     * has been successfully applied on all components or if it cannot apply the controller on any component left. 
     * The exceptions raised on the remaining failing components are accumulated in an error list that can be further
     *  displayed to the user.  The reason for unsucessfull traverse can be errors: 
     *  - local to a component
     *  - due to cyclic dependencies between a sub-set of components (example: a controller on component A needs to get some
     * information about an other component B but this data is only available   once the controller  is applied to B,
     * but again this controller onf B needs data available once A has executed its controller).
     *
     * @see flowvr.comp.cpp for examples of use of traverses (used for the @see execute() controller for instance)
     */
 
   struct TraversePointFixe
    {
            std::list<CustomException> listError; ///< the list of exceptions raised during the last loop
            std::list<Component*> toDoComponent;	///< to do components
            std::list<Component*> doneComponent;	///< list of components that have been processed with success
            InterfaceController* control; ///< control used in this traverse
            unsigned int countTraverse; ///< the number of loops made by the traverse algorithm
            bool isModified;	///< A boolean that signals if the controller has been successfully applied on one component during the last traverse
            bool silent; ///< boolean set to on for less outputs.
            bool addTodoComp; /// < boolean set to false if traverse does not add new component to its todo list (done externaly),  true if traverse add children of successfuly processed comps.

            /**
             * \brief Initialization of listErrors and toDoComponents. Add children of successfully processed components to the toDoComponent list
             * \param comp pointer to the component root. It is the common ancestor of all components traversed. 
             * \param control_ pointer to the controller used throught the traverse
             */
            TraversePointFixe(Component* comp, InterfaceController* control_) : listError(), toDoComponent(1, comp), doneComponent(), control(control_), countTraverse(0), isModified(false), silent(false),addTodoComp(true) {};
			

            /**
             * \brief Constructor with an empty todoComponent list (expected to be set externaly). Also does not add any new component to the toDoComponent
             * \param control_ pointer to the controller used throught the traverse
             */
            TraversePointFixe(InterfaceController* control_) : listError(), toDoComponent(), doneComponent(), control(control_), countTraverse(0), isModified(false), silent(false),addTodoComp(false) {};
			

            /**
             * \brief method that launches the traverse algorithm
             * \param comp components where the traverse is launched. If NULL then traverse used the toDoComponents list as starting points.
             */
            void operator() (Component* comp = NULL)
            {
                Component* current;
                Component * statePrevious;
                bool failed;
                isModified = false; // true it at least one component of the application successfully execute (*control())
                countTraverse = 0;
                std::list<Component*> toDoComponentNext;
                if (comp != NULL)
                    {
                        toDoComponent.clear();
                        toDoComponent.push_back(comp);
                    }
                doneComponent.clear();

                bool modified; 
                do
                    {
                        // Start a try on all toDoComponents
                        ++countTraverse;
                        listError.clear();
                        modified = false;// true if  at least one component of toDoComponent succesfully execute (* control)()
                        while (!toDoComponent.empty())
                            {
                                // FIFO strategy
                                current = toDoComponent.front();
                                toDoComponent.pop_front();
                                failed = false;

                                // create clone of current component
                                statePrevious = current->cloneForTraverse();
                                
                                /*
                                 //Used by the cloning method reexecuting traverses (very slow)
                                try 
                                    {
                                        statePrevious = current->createUnfoldsInstance(); // create a copy component and apply all previsously applied controllers.
                                    }
                                catch( const CustomException& e)
                                    {
                                        // Error when clongin this component
                                        Msg::debug("TRAVERSE ERROR ("+control->name+", it="
                                                   +toString<int>(countTraverse)+", comps todo="+toString<int>(toDoComponent.size()+toDoComponentNext.size())
                                                   +"): error while cloning component id = " + current->getFullId()
                                                   +".\n\tError was  in function "+ e.functionName+"\n\t"
                                                   +e.what(), __FUNCTION_NAME__);
                                    }
                                */


                                try
                                    {
                                        // We make a try on a cloned component
                                        statePrevious->applyController(control, current);
                                    }
                                catch (const CustomException& e)
                                    {
                                        // There is a failure with this component... register error and try this component next turn
                                        failed = true;
                                        toDoComponentNext.push_back(current);
                                        listError.push_back(e);
                                        if ( !silent)
                                            Msg::debug("TRAVERSE TEST ("+control->name+", it="+toString<int>(countTraverse)+", comps todo="+toString<int>(toDoComponent.size()+toDoComponentNext.size())+"): failed to apply "+control->name+" on component id = " + current->getFullId()+". Try again later.\n\tError was  in function "+ e.functionName+"\n\t"+e.what(), __FUNCTION_NAME__);
                                    }
                                // OK with this controller 
                                // recycle memory of the clone
                                delete statePrevious;
                                if (!failed)
                                    {
                                        // apply controller on the genuine component
                                        try
                                            {
                                                Msg::debug("TRAVERSE APPLY ("+control->name+", it="+toString<int>(countTraverse)+", comps todo="+toString<int>(toDoComponent.size()+toDoComponentNext.size())+"): apply "+control->name+"  on component id = " + current->getFullId(), __FUNCTION_NAME__);
                                                current->applyController(control);
                                                modified = true; // Control did not returned  exception. We consider the state of the component changed
                                                isModified |= control->isModified; 
                                                doneComponent.push_back(current);
                                            }
                                        catch(const CustomException& e)
                                            {
                                                // Should no enter in this section.... because this controller has already suceeded with the clone...
                                                // This section is for defensive-programming
                                                Msg::debug("TRAVERSE ERROR ("+control->name+", it="+toString<int>(countTraverse)+", comps todo="+toString<int>(toDoComponent.size()+toDoComponentNext.size())+"): Unexpected error when  executing "
                                                           +control->name+" on component id = " + current->getFullId()
                                                           +"\n\tError was  in function "+ e.functionName+"\n\t"+e.what(), __FUNCTION_NAME__);
                                                // So there is a big issue with the traverse... send the exception to user...
                                                throw e;
                                            }
                                        if ( addTodoComp && dynamic_cast<Composite*>(current)) // add child objects to the todo list.
                                            {
                                                //Msg::debug("TRAVERSE ("+control->name+"): add children of " + current->getFullId(), __FUNCTION_NAME__);
                                                toDoComponent.insert(toDoComponent.end(), ((Composite*)current)->getComponentBegin(), ((Composite*)current)->getComponentEnd());
                                            }
                                    }
                            }
                        // Go for next turn...
                        toDoComponent.swap(toDoComponentNext);
                    }while (modified && !toDoComponent.empty());
            }

    };



}; };

#endif
