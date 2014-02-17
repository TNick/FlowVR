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
 *                                                                *
 *	File : flowvr/app/core/controller.h                       *
 *                                                                *
 *	Contact : Jean-Denis Lesage                               *
 *                                                                *
 ******************************************************************/

#ifndef __FLOWVR_APP_CONTROLLER_H__
#define __FLOWVR_APP_CONTROLLER_H__

#include <algorithm>
#include <list>
#include <map>
#include <string>
#include "flowvr/app/core/msg.h"
#include "flowvr/app/core/interfacecontroller.h"
#include "flowvr/app/core/component.h"


namespace flowvr { namespace app {

    /**
     * \brief standard controller
     */
    template<class Out, class In>
        class Controller : public InterfaceController
    {
            typedef Out (Component::*Fonct)(const In&); ///< pointer to function 
            Component* c; ///< Writable attribute: Component where Controller is hosted
            Fonct method; ///< Writable attribute: Method to apply on component

        public : 
            In parameter; ///< parameter to the function
            Out result; ///< result of the function

        public :
            /**
             * \brief constructor
             * \param m_ method to apply
             */
            Controller(Fonct m_,std::string name_) : c(NULL), method(m_),InterfaceController(name_) {};

            /**
             * \brief change method
             * \param m_ the method
             */
            void setMethod(Fonct m_) { method = m_; };

            /*
             * \brief change controller name
             * \param name_ the name
             */
            void setName(std::string name_) {name = name_;};

            /**
             * \brief change component
             * \param c_ the component
             */
            void setComponent(Component* c_) { c = c_; idComp = c_; };
            void setComponentClone(Component* c_) { c = c_; };

            /**
             * \brief Call function
             */
            virtual void operator()() { result = (c->*(this->method))(parameter); };

            /**
             * \brief colone controller and his parameter
             */
            InterfaceController* clone() const
            {
                Controller<Out, In>* result = new Controller<Out, In>(method,name);
                result->idComp = idComp;
                result->parameter = parameter;
                return result;
            };

            /**
             * \brief verify type equality and then function equality
             */
            bool equals(const InterfaceController* const i) const
            {
                const Controller<Out, In>* const ci = dynamic_cast<const Controller<Out, In>* const>(i);
                if (ci)
                    return ci->method == method;
                return false;
            };
    };

    template<class Out>
        class Controller<Out, void> :public InterfaceController
    {
            typedef Out (Component::*Fonct)() const;
            Component* c; ///< Writable attribute: Component where Controller is hosted
            Fonct method; ///< Writable attribute: Method to apply on component \note it is const function
			
        public :
            Out result;
		

        public :
            /**
             * \brief constructor
             * \param m_ method to apply
             */
            Controller(Fonct m_,std::string name_) : c(NULL), method(m_),InterfaceController(name_) {};


            /**
             * \brief change method
             * \param m_ the method
             */
            void setMethod(Fonct m_) { method = m_; };

            /*
             * \brief change controller name
             * \param name_ the name
             */
            void setName(std::string name_) {name = name_;};

            /**
             * \brief change component
             * \param c_ the component
             */
            void setComponent(Component* c_) { c = c_; idComp = c_; };
            void setComponentClone(Component* c_) { c = c_; };

            /**
             * \brief Call function
             * \return result of function method
             */
            virtual void operator() () { result = (c->*(this->method))(); };

            InterfaceController* clone() const
            {
                Controller<Out, void>* result = new Controller<Out, void>(method,name);
                result->idComp = idComp;
                return result;
            };

            bool equals(const InterfaceController* const i) const
            {
                const Controller<Out, void>* const ci = dynamic_cast<const Controller<Out, void>* const>(i);
                if (ci)
                    return ci->method == method;
                return false;
            };

    };


    /**
     * \class Controller 
     * \brief with one argument that can be const or not. 
     */
    template<class In>
        class Controller<void, In> : public InterfaceController
    {
            typedef void (Component::*Fonct)(const In&);
            Component* c; ///< Writable attribute: Component where Controller is hosted
            Fonct method; ///< Writable attribute: Method to apply on component

        public :
            In parameter;

        public :
            /**
             * \brief constructor
             * \param m_ method to apply
             */
            Controller(Fonct m_,std::string name_) : c(NULL), method(m_),InterfaceController(name_) {};


            /**
             * \brief change method
             * \param m_ the method
             */
            void setMethod(Fonct m_) { method = m_; };

            /*
             * \brief change controller name
             * \param name_ the name
             */
            void setName(std::string name_) {name = name_;};


            /**
             * \brief change component
             * \param c_ the component
             */
            void setComponent(Component* c_) { c = c_; idComp = c_; };
            void setComponentClone(Component* c_) { c = c_; };

            /**
             * \brief Call function
             */
            virtual void operator() () { (c->*(this->method))(parameter); };

            InterfaceController* clone() const
            {
                Controller<void, In>* result = new Controller<void, In>(method,name);
                result->idComp = idComp;
                result->parameter = parameter;
                return result;
            };

            bool equals(const InterfaceController* const i) const
            {
                const Controller<void, In>* const ci = dynamic_cast<const Controller<void, In>* const>(i);
                if (ci)
                    return ci->method == method;
                return false;
            };

    };


    template<>
        class Controller<void, void> : public InterfaceController
    {
        protected :
            typedef void (Component::*Fonct)();
            Component* c; ///< Writable attribute: Component where Controller is hosted
            Fonct method; ///< Writable attribute: Method to apply on component


        public :

            /**
             * \brief constructor
             * \param m_ method to apply
             */
            Controller(Fonct m_,std::string name) : c(NULL), method(m_),InterfaceController(name) {};

            /**
             * \brief change method
             * \param m_ the method
             */
            void setMethod(Fonct m_) { method = m_; };

            /*
             * \brief change controller name
             * \param name_ the name
             */
            void setName(std::string name_) {name = name_;};

            /**
             * \brief change component
             * \param c_ the component
             */
            void setComponent(Component* c_) { c = c_;idComp = c_;  };
            void setComponentClone(Component* c_) { c = c_; };


            /**
             * \brief Call function
             */
            virtual void operator() ()  
            {

                (c->*(this->method))();
                isModified = true;

            };

            InterfaceController* clone() const
            {
                Controller<void, void>* result = new Controller<void, void>(method,name);
                result->idComp = idComp;
                return result;
            };


            bool equals(const InterfaceController* const i) const
            {
                const Controller<void, void>* const ci = dynamic_cast<const Controller<void, void>* const>(i);
                if (ci)
                    return ci->method == method;
                return false;
            };

    };

    /**
     * \brief this controller used a map as a parameter. See sethost from spreadsheet for example
     */
    template<typename In>
        class ControllerWithMap : public InterfaceController
    {
            typedef void (Component::*Fonct)(const In&);
            Component* c; ///< Writable attribute: Component where Controller is hosted
            Fonct method; ///< Writable attribute: Method to apply on component

        public :
            std::map<std::string, In>* parameter;

        public :
            /**
             * \brief constructor
             * \param m_ method to apply
             */
            ControllerWithMap(Fonct m_,std::string name) : c(NULL), method(m_), parameter(NULL),InterfaceController(name) {};


            /**
             * \brief change method
             * \param m_ the method
             */
            void setMethod(Fonct m_) { method = m_; };

            /*
             * \brief change controller name
             * \param name_ the name
             */
            void setName(std::string name_) {name = name_;};


            /**
             * \brief copy map 
             */
            void setMap(std::map<std::string, In> *map) { parameter = map; }


            /**
             * \brief change component
             * \param c_ the component
             */
            void setComponent(Component* c_) { c = c_; idComp = c_; };
            void setComponentClone(Component* c_) { c = c_; };

            /**
             * \brief Call function
             */
            virtual void operator() () 
            {
                if(parameter->count(c->getFullId()) != 0) 
                    (c->*(this->method))((*parameter)[c->getFullId()]); 
            };

            InterfaceController* clone() const
            {
                ControllerWithMap<In>* result = new ControllerWithMap<In>(method,name);
                result->idComp = idComp;
                result->setMap(parameter);
                return result;
            };

            bool equals(const InterfaceController* const i) const
            {
                const ControllerWithMap<In>* const ci = dynamic_cast<const ControllerWithMap<In>* const>(i);
                if (ci)
                    return ci->method == method;
                return false;
            };


    };





}; };

#endif //__COMMAND
