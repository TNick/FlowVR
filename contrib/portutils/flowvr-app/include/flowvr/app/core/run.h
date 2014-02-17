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
 * File: ./include/flowvr/app/core/metaobjects/run.h               *
 *                                                                 *
 * Contacts:  Jean-Denis Lesage                                    *
 *                                                                 *
 ******************************************************************/

#include <string>
#include "flowvr/app/core/component.h"
#include "flowvr/app/core/exception.h"

#ifndef __FLOWVR_APP__RUN_H
#define __FLOWVR_APP__RUN_H

/**
 * \file run.h
 * \brief implements classes required to build a metamodule launching command
 */

namespace flowvr
{
  namespace app
  {

    /**
     * \class TokenInterface
     * \brief A run command (or launching command) is built from  tokens organized into a tree of lists.
     *  This class is the base class (interface) of all tokens.
     *  Each token have a getString() function that returns a string version of the token
     */

    class TokenInterface
    {
        public:

            /**
             * \brief constructor.
             */
            TokenInterface()
            {
            }

            /**
             * \brief copy constructor.
             */
            TokenInterface(const TokenInterface& t)
            {
            }

            /**
             * \brief clone operation for token-interfaces
             */
            virtual TokenInterface* clone() const =0;

            /**
             * \brief virtual destructor
             */
            virtual ~TokenInterface()
            {
            }

            /**
             * \brief get string version of the token for building the run command (mandatory for all tokens)
             * \return a string with the content of the token
             */
            virtual const std::string getString() const = 0;
    };

    /**
     * \class TokenString
     * \brief Token containing  a constant string
     */
    class TokenString : public TokenInterface
    {
        private:
            std::string value; ///< The value of the Token

        public:
            /**
             * \brief constructor.
             * \param value_ the string value of the Token
             */
            TokenString(const std::string& value_)
            : TokenInterface()
            , value(value_)
            {
            }

            /**
             * \brief copy constructor
             * \param t the object to copy
             */
            TokenString(const TokenString& t)
            : TokenInterface(t)
            , value(t.value)
            {
            }

            /**
             * \brief virtual copy constructor
             */

            virtual TokenInterface* clone() const
            {
                return new TokenString(*this);
            }

            /**
             * \brief virtual destructor
             */
            virtual ~TokenString()
            {
            }

            /**
             * \brief stringify the token
             * \return the string
             */
            virtual const std::string getString() const;

    };

    /**
     * \class TokenParameter
     * \brief Token linked to a component  Parameter
     */
    class TokenParameter : public TokenInterface
    {
        private:

            const Component * comp; ///< pointer to the component the parameter is read from
            std::string name; ///< parameter name

        public:

            /**
             * \brief  constructor
             * \param _comp the component having the parameter
             * \param _name the parameter name
             */
            TokenParameter(const Component& _comp, const std::string _name)
            : TokenInterface()
            , comp(&_comp)
            , name(_name)
            {
            }

            /**
             * \brief virtual destructor
             */
            virtual ~TokenParameter()
            {
            }

            /**
             * \brief copy constructor
             * \param t  the token  to copy
             */
            TokenParameter(const TokenParameter& t)
            : TokenInterface(t)
            , comp(t.comp)
            , name(t.name)
            {
            }

            virtual TokenInterface* clone() const
            {
                return new TokenParameter(*this);
            }

            /**
             * \brief stringify  the token
             * \return  a string containing the value of the parameter
             */
            virtual const std::string getString() const
            {
                return comp->getParameter<std::string>(name);
            }

    };

    /**
     * \class TokenList
     * \brief a TokenList is a list  of Tokens.
     * \warning We do not inherit from stl::list because we want a different memory allocation policy: when adding an token to the list a virtual token constructor is called.
     */
    class TokenList : public TokenInterface
    {
        private:

            std::string separator; ///<  separator string used to separate the elements of the list when calling getString()
            std::list<TokenInterface*> tokenlist; ///< the list of tokens

        public:

            /**
             * We implement here several methods coming form stl:list. We cannot inherit from stl::list instead because we need
             *  to call the virtual token  constructor when adding a token to the list
             */

            typedef std::list<TokenInterface*>::iterator iterator; ///< list iterator

            typedef std::list<TokenInterface*>::const_iterator
                const_iterator; ///< const list iterator

            bool empty() const
            {
                return tokenlist.empty();
            }///< true if list is empty

            iterator begin()
            {
                return tokenlist.begin();
            }///< return iterator on the first list element

            const_iterator begin() const
            {
                return tokenlist.begin();
            }///< return const iterator on the first list  element

            iterator end()
            {
                return tokenlist.end();
            }///< return iterator on the last list element


            const_iterator end() const
            {
                return tokenlist.end();
            } ///< return const iterator on the last list element

            TokenInterface* front()
            {
                return tokenlist.front(); ///< return pointer to the first list element
            }

            const TokenInterface* front() const
            {
                return tokenlist.front(); ///< return const pointer to the first list element
            }

            TokenInterface* back()
            {
                return tokenlist.back(); ///< return pointer to the last  list element
            }

            const TokenInterface* back() const
            {
                return tokenlist.back(); ///< return const pointer to the last  list element
            }

            /**
             * \brief  insert copy of token at begining  of the list
             * \param _t token to copy and add
             */
            void push_front(const TokenInterface& _t)
            {
                TokenInterface * t = _t.clone(); ///< call virtual copy constructor
                tokenlist.push_front(t);
            }

            /**
             * \brief  remove  first list element
             */
            void pop_front()
            {
                delete front();
                tokenlist.pop_front();
            }

            /**
             * \brief  append copy of token at end of the list
             * \param _t token to copy and add
             */
            void push_back(const TokenInterface& _t)
            {
                TokenInterface * t = _t.clone();
                tokenlist.push_back(t);
            }

            /**
             * \brief  remove  last  list element
             */
            void pop_back()
            {
                delete back();
                tokenlist.pop_back();
            }

            /**
             * \brief constructor.
             * \param _sep  the optional separator (default ' ')
             */
            TokenList(const std::string& _sep = " ") :
                separator(_sep)
            {
            }

            /**
             * \brief copy constructor
             * \param t the object to copy
             */
            TokenList(const TokenList& t);

            /**
             * \brief virtual destructor
             */
            virtual ~TokenList();

            /**
             * \brief virtual copy constructor (mandatory for all tokens).
             */
            virtual TokenInterface* clone() const
            {
                return new TokenList(*this);
            }

            /**
             * \brief return the separator
             */
            const std::string getSeparator() const
            {
                return separator;
            }

            /**
             * \brief stringify the token
             * \return a string  resulting from the concatenation of the strings returned by  getString()
             *  on each element. Substrings are separated by \param separator
             */
            virtual const std::string getString() const;

            /**
             * \brief  build a new list from an initial  list and  a token
             * \param script the initial TokenList
             * \param token to add to the resulting list
             * \return the TokenList with the token  added
             */
            friend TokenList operator+(const TokenList& script,
                                       const TokenInterface& token);

            /**
             * \brief operator+=  for appending a token at the end of the list (equivalent to script.push_back(token) )
             * \param script the token list
             * \param token the token to add
             * \return true
             */
            friend bool operator+=(TokenList& script,
                                   const TokenInterface& token);

            /**
             * \brief concate two token lists
             * \param script1 the first list
             * \param script2 the second list
             * \return script1 + script2
             */
            friend TokenList operator+(const TokenList& script1,
                                       const TokenList& script2);

            /**
             * \brief operator+= for 2 token lists
             * \param script1 the first token list
             * \param script2 the second  token list
             * \return true
             */
            friend bool operator+=(TokenList& script1,
                                   const TokenList& script2);

    };

    /**
     * \class TokenHostList
     * \brief  a token  related to the lists of hosts of a given component. getString() returns the sequence of  host names separated by a given separator.
     */
    class TokenHostList : public TokenInterface
    {
        private:
            const Component * comp; ///< the component the host list is read from
            std::string separator; ///< the separator inserted between hosts names when getString() is called


        public:

            /**
             * \brief constructor
             * \param  _comp the component the host list is related to
             * \param _sep optional  separator  (default ' ')
             */
            TokenHostList(const Component* _comp,
                          const std::string& _sep=" ") :
                comp(_comp), separator(_sep)
            {
            }

            /**
             * \brief virtual destructor
             */
            virtual ~TokenHostList()
            {
            }

            /**
             * \brief copy constructor
             * \param the object to copy
             */
            TokenHostList(const TokenHostList& t)
            : TokenInterface(t)
            , comp(t.comp)
            , separator(t.separator)
            {
            }

            /**
             * \brief virtual copy constructor
             */
            virtual TokenInterface* clone() const
            {
                return new TokenHostList(*this);
            }

            /**
             * \brief stringify  token
             * \return  a string containing the list of hosts separated  by \param separator
             */
            virtual const std::string getString() const;

            /**
             *\brief change component used to find hosts
             *\param comp_ the new component
             */
            void changeComp(Component* comp_) { comp = comp_; }
    };

    /**
     * \class TokenHostListSize
     * \brief  a token  related to the lists of hosts of a given compoeent. getString() returns the host list size
     */
    class TokenHostListSize : public TokenInterface
    {
        private:
            const Component * comp; ///< the component the host list size is computed from

        public:

            /**
             * \brief Constructor
             * \param _comp the component givieng the host list
             */

            TokenHostListSize(const Component* _comp)
            : TokenInterface()
            , comp(_comp)
            {
            }

            /**
             * \brief virtual destructor
             */
            virtual ~TokenHostListSize()
            {
            }

            /**
             * \brief copy constructor
             * \param the object to copy
             */
            TokenHostListSize(const TokenHostListSize& t)
            : TokenInterface(t)
            , comp(t.comp)
            {
            }

            /**
             * \brief virtual copy constructor
             */
            virtual TokenInterface* clone() const
            {
                return new TokenHostListSize(*this);
            }

            /**
             * \brief stringify  token
             * \return the host list size
             */
            virtual const std::string getString() const;

    };

    /**
     * \class TokenFunction
     * \brief This token applies a function to an object
     * \note template Function: The function to apply.
     *
     * The function class must
     *  #- provide a function with the signature: const std::string apply(const Component*)
     *  #- be constructible without arguments (default constructor)
     *
     *  @see TokenFunction::getString()
     */
    template <class Function> class TokenFunction : public TokenInterface
    {
        private:
            const Component * comp; ///< the object the function will be applied to

        public:
            /**
             * \brief Constructor
             * \param comp_ the component   where the method will be applied
             */
            TokenFunction(const Component* comp_) :
                comp(comp_)
            {
            }

            /**
             * \brief copy constructor
             * \param t the object to copy
             */
            TokenFunction(const TokenFunction& t) :
                comp(t.comp)
            {
            }

            /**
             * \brief virtual copy constructor
             */
            virtual TokenInterface* clone() const
            {
                return new TokenFunction<Function>(*this);
            }

            /**
             * \brief virtual destructor
             */
            virtual ~TokenFunction()
            {
            }

            /**
             * \brief stringify the token
             * \return a string containing the result of the function call
             */
            virtual const std::string getString() const;

    };

    /**
     * \interface FunctionInterface
     * \brief interface for all functions applied to a component by a TokenFunction.
     */
    class FunctionInterface
    {
        public:
            /**
             * \brief the constructor
             */
            FunctionInterface()
            {
            }

            /**
             * \brief virtual destructor
             */
            virtual ~FunctionInterface()
            {
            }

            /**
             * \brief This is the  function  called by the TokenFunction class
             * \param obj the component
             * \return a string with the result of the function
             */
            virtual const std::string apply(const Component* obj) = 0;
    };


    // Implementation of template functions
    template <class Function> const std::string TokenFunction<Function>::getString() const
    {
        if (!comp)
            throw NullPointerException("component","", __FUNCTION_NAME__);
        const Component* obj = dynamic_cast<const Primitive*>(comp);
        Component* theComponent;

        Function f;
        return f.apply(theComponent);

    }

    /**
     * \class CmdLine
     * \brief A command follows the classical syntax: "command [options list] [argument list]"
     */
    class CmdLine : public TokenList
    {
        private:
            TokenList * optlist; ///< the option list
            TokenList * arglist; ///< the argument list

        public:

            /**
             * \brief Constructor
             * \param name the commande name
             */

            CmdLine(const std::string& name);

            /**
             * \brief virtual destructor
             */
            virtual ~CmdLine()
            {
            }

            /**
             * \brief copy constructor
             * \param t the object to copy
             */
            CmdLine(const CmdLine& t);

            /**
             * \brief  virtual copy constructor
             */
            virtual TokenInterface* clone() const
            {
                return new CmdLine(*this);
            }
            ;

            /**
             * \brief  add an option
             * \param opt the token  for the option
             */
            void addOption(const TokenInterface& opt)
            {
                optlist->push_back(opt);
            }

            /**
             * \brief  add an option limited to a single string
             * \param opt  the  option string
             */
            void addOption(const std::string& opt)
            {
                addOption(TokenString(opt));
            }

            /**
             * \brief  add an argument
             * \param arg the token  for the argument
             */
            void addArg(const TokenInterface& arg)
            {
                arglist->push_back(arg);
            }

            /**
             * \brief  add an argument  limited to a single string
             * \param arg  the  argument string
             */
            void addArg(const std::string& arg)
            {
                addArg(TokenString(arg));
            }

            /**
             * \brief this method change host list Token. It changes the component used to find hosts
             * \param comp : the component
             */
            void changeHostComp(Component* comp);

//            template<class T>
//            T *clone() const
//            {
//            	return new T(*this);
//            }

            virtual bool setParallel() { return false; }
            virtual bool isParallel() const { return false; }
    };

    /**
     * \class FlowvrRunSSH
     * \brief the commande line for  the flowvr-run-ssh launcher
     */
    class FlowvrRunSSH : public CmdLine
    {
        public:

            /**
             * \class constructor
             * \param comp the component the flowvr-run-ssh commande is related to (required to get the host list)
             */
            FlowvrRunSSH(const Component * comp);

            /**
             * \class constructor
             * \param comp the component the flowvr-run-ssh commande is related to (required to get the host list)
             * \param host_ a list of hosts (don't look for it)
             */
            FlowvrRunSSH(const Component* comp, const std::string& host_)
            : CmdLine("flowvr-run-ssh")
            , m_bParallel(false)
            {
                addArg("'"+host_+"'");
            }

            /**
             * \brief copy constructor
             * \param t the object to copy
             */
            FlowvrRunSSH(const FlowvrRunSSH& t)
            : CmdLine(t)
            , m_bParallel(t.m_bParallel)
            {
            }

            /**
             * \brief  virtual copy constructor
             */
            virtual TokenInterface* clone() const
            {
                return new FlowvrRunSSH(*this);
            }

            /**
             * \brief virtual destructor
             */
            virtual ~FlowvrRunSSH()
            {
            }

            /**
             * \brief run the command in background
             */
            void setBackground()
            {
                addOption("-b");
            }

						/**
						 * \brief run the command in a grid context using the jobfile to ssh in an existing job
						 * \param jobfile path to the jobfile
						 */
						void setGrid(const std::string& jobfile)
						{
							addOption("-g " + jobfile);
						}


            /**
             * \brief changes to path before executing the command
             * \param path the execution path
             */
            void setExecPath(const std::string& path)
            {
                addOption("-d " + path);
            }

            /**
             * \brief set flowvr-run-ssh in verbose mode
             */
            void setVerbose()
            {
                addOption("-v");
            }

            /**
             * \brief set flowvr-run-ssh in parallel mode
             */
            bool setParallel()
            {
            	if( m_bParallel )
            		return true;

                addOption("-p");
                m_bParallel = true;

                return true;
            }

            /**
             * \brief set an environment variable  in the context of the   command execution
             * \param var the variable name
             * \param value the variable value
             */
            void addEnvVar(const std::string var, const std::string value)
            {
                addOption("-e "+var+" "+value);
            }

            /**
             * \brief redirect display to disp for the command
             * \param disp display address
             */
						void openDisplay(const std::string& disp)
						{
							addEnvVar("DISPLAY", disp);
						}

						/**
             * \brief configure ssh to do X11 forwarding
             */
						void setX11Forwarding()
						{
							addOption("-X ");
            }


            /**
             * \brief propagate   environment variable  to the  command execution context
             * \param var the variable name
             */
            void addPropagEnvVar(const std::string var)
            {
                addOption("-x "+var);
            }

            /**
             * \brief add  the command  to the flowvr-run-ssh argument list
             * \param cmd the command
             */
            void addCommand(const CmdLine& cmd)
            {
                addArg(cmd);
            }

            virtual bool isParallel() const { return m_bParallel; }
        private:
            bool m_bParallel;

    };

    /**
     * \class OpenMPIRun
     * \brief  commande line for  the openmpi  launcher (just work for launching an MPI application consisting of 1 binary file).
     */

    class OpenMPIRun : public CmdLine
    {
        public:

            /**
             * \brief constructor
             * \param the component the launcher is related to
             */
            OpenMPIRun(const Component * comp);

            /**
             * \brief copy constructor
             * \param t the object to copy
             */
            OpenMPIRun(const OpenMPIRun& t) :
                CmdLine(t)
            {
            }

            /**
             * \brief virtual copy constructor
             */
            virtual TokenInterface* clone() const
            {
                return new OpenMPIRun(*this);
            }

            /**
             * \brief destructor
             */
            virtual ~OpenMPIRun()
            {
            }

            /**
             * \brief set flowvr-run-ssh in verbose mode
             */
            void setVerbose()
            {
                addOption("-v");
            }

            /**
             * \brief add  the command  to the flowvr-run-ssh argument list
             * \param cmd the command
             */
            void addCommand(const CmdLine& cmd)
            {
                addArg(cmd);
            }

            virtual bool setParallel() { return true; }
            virtual bool isParallel() const { return true; }

    };

    /**
     * \class ForRun
     * \brief create a list of command line using a for loop.
     *        Parameter can be set through sorted list.
     *        The template is the type of launcher (flowvr-run-ssh, mpirun...)
     */
    class ForRunFlowvrRunSSH : public CmdLine
    {
        private:

            std::list<FlowvrRunSSH*> listCmd;

        public:
            ForRunFlowvrRunSSH() :
                CmdLine(""), listCmd()
            {
            }

            ForRunFlowvrRunSSH(const ForRunFlowvrRunSSH& f) :
                CmdLine(f), listCmd()
            {
                listCmd.clear();
                for(std::list<FlowvrRunSSH*>::const_iterator it = f.listCmd.begin(); it != f.listCmd.end(); ++it)
                       listCmd.push_back((FlowvrRunSSH*) (*it)->clone());
            }

            virtual ~ForRunFlowvrRunSSH()
            {
                while (!listCmd.empty())
				{
					FlowvrRunSSH* it = listCmd.back();
					delete it;
					listCmd.pop_back();
				}
            }

            /**
             * \brief virtual copy constructor
             */
            virtual TokenInterface* clone() const
            {
                ForRunFlowvrRunSSH* result = new ForRunFlowvrRunSSH;
                for(std::list<FlowvrRunSSH*>::const_iterator it = listCmd.begin(); it != listCmd.end(); ++it)
                        result->addCmd((FlowvrRunSSH*) (*it)->clone());
                return result;
            }

            virtual const std::string getString() const
            {
                std::string result;
                for(std::list<FlowvrRunSSH*>::const_iterator it = listCmd.begin(); it != listCmd.end(); ++it)
                {
                        if(it != listCmd.begin())
                            result += "; ";
                        result += (*it)->getString();
                }
                return result;
            }

            FlowvrRunSSH getlistCmd()
			{
            	return *(listCmd.back());
			}

            void addCmd(FlowvrRunSSH* r)
            {
                listCmd.push_back((FlowvrRunSSH*) (r->clone()));
            }

    };

}
}
#endif //__FLOWVR_APP__RUN_H
