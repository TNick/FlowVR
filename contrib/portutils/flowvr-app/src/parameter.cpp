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
 *        File: ./src/parameter.cpp                                *
 *                                                                 *
 *   Contact : Antoine Vanel                                       *
 *                                                                 *
 ******************************************************************/


#include <flowvr/xml.h>
#include <flowvr/app/core/msg.h>
#include <flowvr/app/core/parameter.h>
#include "flowvr/app/core/exception.h"
#include <algorithm>


namespace flowvr
{
  namespace app
  {

    
    Parameter::Parameter(const std::string& linetoparse,
                         const TypeFrom  setfrom_,
                         const std::string& setfromfile)
        : defaultvalue("")
        , isdefault(false)
        , isset(true)
        , setfrom(setfrom_)
        , setfromcompid(setfromfile)
    {
        std::istringstream itf(linetoparse);

        // first, get the target component, check if it's consistent.
        std::string targetcomponent;


        std::getline( itf, targetcomponent, ':' );
        
        if (itf.eof() ) 
            {
                throw CustomException("Error parsing  parameter \'"+linetoparse +"\' on command line: no ':' delimiter found in  \'"+targetcomponent+"\'", __FUNCTION_NAME__);
            }

        if (targetcomponent.empty() )
            {
                throw CustomException("Error parsing  parameter \'"+linetoparse +"\' on command line: empty component name before ':' delimiter", __FUNCTION_NAME__);
            }
            

        targetcomponent.erase (targetcomponent.find_last_not_of (" " ) + 1); //trimRight
        targetcomponent.erase (0, targetcomponent.find_first_not_of (" " )); //trimLeft
        
        //if there is space left after trim, throw an error
        if(strchr(targetcomponent.c_str(), ' ') != NULL)
            {
                throw CustomException("Error parsing  parameter \'"+linetoparse +"\' on command line: component name \'"+targetcomponent+"\' contains unexpected spaces", __FUNCTION_NAME__);
            }
        
        this->targetcomp = targetcomponent;
        


        //then check the name of the parameter.
        std::string parametername;
        
        std::getline( itf,  parametername,  '=');

        if ( itf.eof()  ) 
            {
                throw CustomException("Error parsing  parameter \'"+linetoparse +"\' on command line: no '=' delimiter found in  \'"+parametername+"\'", __FUNCTION_NAME__);
            }

        if (parametername.empty() )
            {
                throw CustomException("Error parsing  parameter \'"+linetoparse +"\' on command line: empty parameter name between  ':'  and '=' delimiters", __FUNCTION_NAME__);
            }

        parametername.erase (parametername.find_last_not_of (" " ) + 1); //trimRight
        parametername.erase (0, parametername.find_first_not_of (" " )); //trimLeft
        
        //if there is space left after trim, throw an error
        if(strchr(parametername.c_str(), ' ') != NULL)
            {
                throw CustomException("Error parsing  parameter \'"+linetoparse +"\' on command line: parameter name \'"+parametername+"\' contains unexpected spaces" , __FUNCTION_NAME__);
            }
        
        this->name = parametername;
        
        //and finally the value itself (take the value as it is  with all the spaces it contains)
        std::string value;
        std::getline( itf, value);

        if (value.empty() )
            {
                throw CustomException("Error parsing  parameter \'"+linetoparse +"\' on command line: empty parameter value after '=' delimiter", __FUNCTION_NAME__);
            }
        this->value = value;
        this->setfrom = setfrom_;
    }
          
          
    Parameter::Parameter(const CustomException& msg)
        : defaultvalue("")
        , isdefault(false)
        , isset(true)
        , setfrom(CMDLINE)
        , setfromcompid("")
    {
        std::istringstream parse((msg).what());
        std::string modName, paraName;
        
        std::getline (parse, modName, ':');
        std::getline (parse, paraName, ':');
        
        this->targetcomp = modName ;
        this->name = paraName ;
    }


    void Parameter::setValue(const Parameter& param, const std::string& setcompid)
    {
        if (isset && (value != param.value || targetcomp != param.targetcomp || setfrom != param.setfrom)  ) 
            throw CustomException("Parameter '"+name+"' already set (value="+value+"). ",__FUNCTION_NAME__ );
        isset = true;
        value = param.value;
        setfrom = param.setfrom;
        targetcomp = param.targetcomp;
        setfromcompid = setcompid;
    }

    void Parameter::setValuetoDefault( const std::string& setcompid )
    {
        if (isset && value != defaultvalue ) 
            throw CustomException("Parameter '"+name+"' already set (value="+value+"). ",__FUNCTION_NAME__ );
        isset = true;
        value = defaultvalue;
        setfrom = DEFAULT;
        targetcomp = "";
        setfromcompid = setcompid;
    }

    std::string Parameter::getStringSetFrom() const
    {
        switch (setfrom)
            {
            case CMDLINE:
                return "cmd line";
                break;
            case PARAMFILE:
                return setfromcompid;// file name stored in this attribute. 
                break;
            case ADL:
                return setfromcompid;// file name stored in this attribute. 
                break;
            case CODE:
                return "code";
                break;
            case DEFAULT:
                return "default value";
                break;
            }
    }
    

    flowvr::xml::DOMElement*  Parameter::XMLBuild(bool adl) const
    {
        
        if ( isset == false ) 
            throw CustomException("Parameter '"+name+"': no value found", __FUNCTION_NAME__);

        flowvr::xml::DOMElement *  e = new flowvr::xml::DOMElement(name);
        if (e)
            {
                if(adl)
                    e->SetAttribute("from",getStringSetFrom());

                e->LinkEndChild(new flowvr::xml::DOMText(value));
            }
        return e;
    }
    
    
} // namespace app
} // namespace flowvr


