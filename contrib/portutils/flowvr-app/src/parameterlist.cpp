/******* COPYRIGHT ************************************************
 *                                                                 *
 *                             FlowVR                              *
 *                                                                 *
 *-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
 * INRIA and                                                       *
 * Laboratoire d'Informatique Fondamentale d'Orleans               *
 * (FRE 2490). ALL RIGHTS RESERVED.                                *
 *                                                                 *
 *   The source code is  covered by different licences,            *
 *   mainly  GNU LGPL and GNU GPL. Please refer to the             *
 *   copyright.txt file in each subdirectory.                      *
 *                                                                 *
 *-----------------------------------------------------------------*
 *                                                                 *
 *  Original Contributors:                                         *
 *    Jeremie Allard,                                              *
 *    Ronan Gaugne,                                                *
 *    Valerie Gouranton,                                           *
 *    Loick Lecointre,                                             *
 *    Sebastien Limet,                                             *
 *    Bruno Raffin,                                                *
 *    Sophie Robert,                                               *
 *    Emmanuel Melin.                                              *
 *                                                                 *
 *******************************************************************
 *                                                                 *
 *        File: ./src/parameterlist.cpp                            *
 *                                                                 *
 *   Contact : Antoine Vanel                                       *
 *                                                                 *
 ******************************************************************/



#include <queue>
#include <fstream>
#include <iostream>
//#include <sstream>
#include <string>
#include <algorithm>
#include "flowvr/app/core/msg.h"
#include <flowvr/app/core/parameterlist.h>


namespace flowvr
{
	namespace app
	{

    ParameterList::const_iterator ParameterList::getParam(const std::string& name,
    		                                              const std::string& targetcomp,
    		                                              const Parameter::TypeFrom setfrom)  const
    {

        for(ParameterList::const_iterator it = begin(); it!=end(); ++it)
		{
			if(it->getName() == name && it->getTargetComp() == targetcomp && it->getSetFrom() == setfrom )
				return it;
		}

        return end();
    }


    ParameterList::iterator ParameterList::getParam(const ParameterList::iterator beginsearch,
    		                                        const std::string& name,
    		                                        const std::string& targetcomp,
    		                                        const Parameter::TypeFrom setfrom)
    {
        for(ParameterList::iterator it = beginsearch; it!=end(); ++it)
		{
			if(it->getName() == name && it->getTargetComp() == targetcomp && it->getSetFrom() == setfrom )
				return it;
		}
        return end();
    }



    ParameterList::iterator ParameterList::getParam(const std::string& name)
    {
        for(ParameterList::iterator it = begin(); it!=end(); ++it)
		{
			if(it->getName() == name )
				return it;
		}
        return end();
    }


    xml::DOMElement*  ParameterList::XMLBuild(bool adl) const
    {
        xml::DOMElement* paramlist = new xml::DOMElement("parameters");

        if( !empty() )
        {
            for(ParameterList::const_iterator it=begin(); it!=end(); ++it)
            {
                paramlist->LinkEndChild(XMLBuild(adl));
            }
        }
        return paramlist;
    }


    bool ParameterList::eraseDuplicates(const Parameter& param)
    {
        // Check if same parameter already there
        ParameterList::iterator it = getParam(begin(),param.getName(),param.getTargetComp(),param.getSetFrom());
        bool iserased = false;

        while (it != end() )
		{
			// found duplicate: erase it
			it = erase(it);

			if (param.getName() != "info") // silent for info parameter
				iserased = true;

			it = getParam(begin(), param.getName(),param.getTargetComp(),param.getSetFrom());
		}
        return iserased;
    }

    void ParameterList::push_fromParameterList(const ParameterList& paramlist)
    {
        for(ParameterList::const_iterator it = paramlist.begin() ; it != paramlist.end() ; ++it)
		{
			// Check if same parameter already there and erase it if so
			eraseDuplicates(*it);
			// add parameter
			push_back(*it);
		}
    }



    void ParameterList::push_fromCmdLine(const std::string &parameter_string)
    {

        if (parameter_string != "")
		{
			// parse command line
			std::istringstream iss(parameter_string);
			std::string mot;
			std::list<std::string> cutParamString;

                        // split between different parameters (comma used as separator)
                        while (std::getline(iss, mot, ','))
                            {
                                if ( !mot.empty() ) cutParamString.push_back(mot);
                            }

                        // process each parameter
			for (std::list<std::string>::const_iterator it = cutParamString.begin(); it
					!= cutParamString.end(); ++it)
			{

                            // Throw exceptions if something wrong
                            Parameter param(*it, Parameter::CMDLINE, "");


                            // Check if same parameter already there
                            if (eraseDuplicates(param))
				{
                                    Msg::warning(
                                                 "Parameter set from command line "
                                                 + param.getTargetComp() + ":" + param.getName()
                                                 + "=" + param.getValue()
                                                 + " erases previous parameter value set from command line",
                                                 __FUNCTION_NAME__);
				}
                            
                            // add parameter
                            push_back(param);
			}

		}
    }


    void ParameterList::push_fromParamFile(const std::string& parameter_file)
    {
		if( !parameter_file.empty() )
		{
			std::ifstream fichier_tmp(parameter_file.c_str());
			if (fichier_tmp)
			{
				Msg::warning("Load Parameter File: " + parameter_file,__FUNCTION_NAME__);

				std::string line_tmp;
				char comment;
				while (std::getline(fichier_tmp, line_tmp))
				{
					if( line_tmp.empty() )
						continue;

					strncpy(&comment, line_tmp.c_str(), sizeof(char));
					if (comment != '#')
					{
						Parameter param(line_tmp, Parameter::PARAMFILE, parameter_file);

						// Check if same parameter already there
						if (eraseDuplicates(param))
						{
							Msg::warning(
									"Parameter setfrom command parameter file ["
											+ parameter_file + std::string("] ")
											+ (param.getTargetComp().empty() ? "<NO-TARGET-COMP>" : param.getTargetComp()) + ":"
											+ (param.getName().empty() ? "<NO-PARAM-NAME>":param.getName()) + "="
											+ (param.getValue().empty() ? "<NO-PARAM-VALUE>":param.getValue())
											+ std::string(" erases previous parameter value set from parameter file")
											+ std::string("\n\tline: ")
											+ line_tmp,
									__FUNCTION_NAME__);
						}
						// add parameter
						push_back(param);
					}
				}
			}
			else
			{
				Msg::error("Unable  to load parameter file: " + parameter_file, __FUNCTION_NAME__);
			}
		}
    }



    void ParameterList::push_fromAdl(const std::string& adlfile)
    {
        flowvr::xml::DOMDocument adldescr(adlfile);
		const flowvr::xml::DOMElement* comp;
		std::queue<const flowvr::xml::DOMElement*> pile;
		const flowvr::xml::DOMElement* paramelem;

		if (!adldescr.LoadFile())
		{
			Msg::debug("Disable parameter  extraction from " + adlfile
					+ " (file not found)", __FUNCTION_NAME__);
			return;
		}

		Msg::debug("Read parameter values from adl file '" + adlfile + "'", __FUNCTION_NAME__);

		// root element is the root component
		pile.push(adldescr.RootElement());

		while (!pile.empty())
		{
			comp = pile.front();
			pile.pop();

			const flowvr::xml::DOMElement * elemSon = comp->FirstChildElement("parameters");
			if (elemSon)
			{
				if (elemSon->FirstChildElement())
				{
					paramelem = elemSon->FirstChildElement();
					while (paramelem)
					{
						if (comp->Attribute("id") && paramelem->Value()
								&& paramelem->GetText())
						{
							// Store file name in the setfromcompid argument.
							Parameter param(paramelem->Value(),
									paramelem->GetText(), Parameter::ADL,
									adldescr.Value(), comp->Attribute("id"));

							// Check if same parameter already there
							if (eraseDuplicates(param))
							{
								Msg::warning(
										"Parameter set from adl file "
												+ param.getTargetComp() + ":"
												+ param.getName() + "="
												+ param.getValue()
												+ " erases previous parameter value set from adl file",
										__FUNCTION_NAME__);
							}
							push_back(param); // insert param
						}
						paramelem = paramelem->NextSiblingElement();
					}// end while
				}//endif
			}//endif


			for (comp = comp->FirstChildElement("component"); comp; comp = comp->NextSiblingElement())
			{
				pile.push(comp);
			}
		}//end while
    }

    bool ParameterList::push_fromStdIn(const std::list<CustomException> listerror)
    {
		bool setparam = false;
		// Ask parameter and add a loop
		if (!listerror.empty())
		{
			std::list<CustomException>::const_iterator it;
			for (it = listerror.begin(); it != listerror.end(); ++it)
			{
				if (it->getType() == 2)
				{
					setparam = true;
					std::cout << " PARAMETER NEEDED " << std::endl;
					std::cout << "*" << (*it).what() << std::endl;
					Parameter tmpParam(*it);
					std::cout << "please enter parameter value : " << std::endl;
					std::string tmp;
					std::cin >> tmp;
					int c;
					while ((c = getchar()) != '\n' & c != EOF)
						;
					tmpParam.setValue(tmp, "");
					// Check if same parameter already there
					if (eraseDuplicates(tmpParam))
					{
						Msg::warning("Parameter setfrom stdin"
								+ tmpParam.getTargetComp() + ":"
								+ tmpParam.getName() + "=" + tmpParam.getValue()
								+ " erases previous parameter value",
								__FUNCTION_NAME__);
					}
					push_front(tmpParam);
				}
			}
		}
		return setparam;
    }

  } // namespace app
} // close namespace flowvr
