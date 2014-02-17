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
 * File: ./src/run.cpp                                             *
 *                                                                 *
 * Contacts:  Bruno Raffin                                         *
 *                                                                 *
 ******************************************************************/

/**
 * \file run.cpp
 * \brief class for defining and processing run commands  (each metamodule must specify a run command, i.e. how to launch it). 
 */

#include "flowvr/app/core/run.h"

namespace flowvr { namespace app {


    const std::string TokenString::getString() const
    {
        if(!value.empty())
            {
                return value;
            }
        return "";
    };



    TokenList::TokenList(const TokenList& t): separator(t.separator)
    {
        // reallocate the tokens   the list point to (make a real copy of all tokens)
        for(const_iterator it = t.begin(); it != t.end(); ++it)
		{
			push_back(**it);// clone **it and insert into the list
			                // (push_back() method of this class uses the clone-API the TokenInterface)
		}
    }
	
    TokenList::~TokenList()
    {
        for(iterator it = begin(); it != end(); it++)
		{
			delete *it;
		}
    }
	
    const std::string TokenList::getString() const
    {
        std::string result;
	  
        for(const_iterator it = begin(); it != end();)
            {
                result += (*it)->getString();
                if ( (++it) != end()) result+=separator; 
            };
        return result;
    }



    
    const std::string  TokenHostList::getString() const 
    {
        if (!comp)
        	throw NullPointerException("Component","", __FUNCTION_NAME__);
	    
        std::string result;

        // Get the host list directly from HostList

        if(comp->hostList.empty())
        	throw CustomException("Component " + comp->getFullId() + ": empty host list. Unable to build host list for run command (at least one host expected)", __FUNCTION_NAME__);

        for ( HostList::const_iterator it = comp->hostList.begin(); it != comp->hostList.end(); ++it)
            {
                if (!result.empty()) result+=separator;
                result += (*it).getName();
            }

        return result;
    }
	
    const std::string TokenHostListSize::getString() const 
    {
        if (!comp) throw NullPointerException("Component ", "", __FUNCTION_NAME__);
	    
        int result = 0;
        const Composite * metamod = dynamic_cast<const Composite*>(comp);
	    
        if(  ! metamod )
            return "1";
	   
        // comp is a composite (hopefully a metamodule)

        for (Composite::ConstpComponentIterator it = metamod->getComponentBegin(); it != metamod->getComponentEnd();++it)
            {               
                result += (*it)->hostList.size();
            }
        return toString<int>(result);
    }
	
	


    TokenList operator+(const TokenList& script,  const TokenInterface& token)
    {
        TokenList result(script);
        result.push_back(token);
        return result;
    }

    bool operator+=(TokenList& script,  const TokenInterface&  token)
    {
        script.push_back(token);
        return true;
    }

    TokenList operator+(const TokenList& script1, const TokenList& script2)
    {
        TokenList result(script1);
        for (TokenList::const_iterator it = script2.begin(); it != script2.end(); ++it)
            {
                result.push_back(**it);
            }
        return result;
    }




    bool operator+=(TokenList& script1, const TokenList& script2)
    {
        for(TokenList::const_iterator it = script2.begin(); it != script2.end(); ++it)
            {
                script1.push_back(**it);
            }
        return true;
    }


	
    CmdLine::CmdLine(const std::string& name): TokenList(" "),optlist(NULL),arglist(NULL)
    {
        push_front(TokenString(name));
        push_back(TokenList(" "));
        optlist = static_cast<TokenList*>(back());
        push_back(TokenList(" "));
        arglist = static_cast<TokenList*>(back());
    }


    CmdLine::CmdLine(const CmdLine& t) 
    {
        for (TokenList::const_iterator it = t.begin(); it != t.end(); it++)
            {
                push_back(**it);
                if ( *it == t.optlist) optlist = static_cast<TokenList*>(back());
                if ( *it == t.arglist) arglist = static_cast<TokenList*>(back());
            }

    }

    void CmdLine::changeHostComp(Component* comp)
    {
        for(TokenList::iterator it = arglist->begin(); it != arglist->end(); ++it)
            {
                TokenHostList* t = dynamic_cast<TokenHostList*>(*it);
                if(t)
                    {
                        t->changeComp(comp);	
                    }
            }

        for(TokenList::iterator it = optlist->begin(); it != optlist->end(); ++it)
            {
                TokenHostList* t = dynamic_cast<TokenHostList*>(*it);
                if(t)
                    {
                        t->changeComp(comp);	
                    }
            }
    }

    FlowvrRunSSH::FlowvrRunSSH(const Component * comp)
    : CmdLine("flowvr-run-ssh")
    , m_bParallel(false)
    {
        addArg(TokenString("'"));
        addArg(TokenHostList(comp," "));
        addArg(TokenString("'"));
    }

    OpenMPIRun::OpenMPIRun(const Component * comp): CmdLine("mpirun")
    {
        addOption("-x FLOWVR_MODNAME");
        addOption("-x FLOWVR_PARENT");
        addArg(TokenString("--host "));
        addArg(TokenHostList(comp,",")); //host list  separator=","
        addArg(TokenString(" "));
		
    }

	        
}; };
