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
 *        File: ./include/flowvr/app/core/parameter.h              *
 *                                                                 *
 *   Contact : Antoine Vanel                                       *
 *                                                                 *
 ******************************************************************/



#ifndef __FLOWVR_APP_PARAMETER_H
#define __FLOWVR_APP_PARAMETER_H

#include <flowvr/xml.h>
#include <flowvr/app/core/exception.h>


namespace flowvr { namespace app{

    

    /**
     * \class Parameter
     * \brief parameters are (name,values) pairs that each component can declare.  Values can be set through different ways (from ascendant component or commande line, or configuration file).
     */

    class Parameter{

            
            /**
             * \enum TypeFrom
             * \brief This type is used to know how a parameter has been 'set'
             */
        public: enum TypeFrom {CMDLINE, PARAMFILE, ADL, CODE, DEFAULT, NONE};
            
            
        private:
            std::string name; ////< parameter name
            std::string defaultvalue; ////< default value
            bool isdefault; ////< true if has devault value
            std::string value; ////<  final  value (once set)
            bool isset; ////< true if value is set
            TypeFrom setfrom; ///< how the parameter has been set
            std::string setfromcompid; // the component id that set this parameter (the one that called the setparameter) or the file name the parameter was set from
            std::string targetcomp; ///< id of a target child component  the parameter set will affect.

        public:



            /**
             * \brief default constructor
             */

            Parameter(): name(""),
                         defaultvalue(""),
                         isdefault(false),
                         value(""),
                         isset(false),
                         setfrom(NONE),
                         setfromcompid(""),
                         targetcomp(""){}


            /**
             * \brief copy constructor
             * \param x the parameter to copy
             */
            Parameter(const Parameter& x): name(x.name),
                                           defaultvalue(x.defaultvalue),
                                           isdefault(x.isdefault),
                                           value(x.value),
                                           isset(x.isset),
                                           setfrom(x.setfrom),
                                           setfromcompid(x.setfromcompid),
                                           targetcomp(x.targetcomp){}


            /**
             * \brief constructor for adding a parameter
             * \param name_  parameter name
             * \param isdefault_  optional parameter. True is has default value
             * \param defaultvalue_ default value
             */
            Parameter(const std::string& name_, const  bool  isdefault_ = false, const std::string& defaultvalue_ ="")
            : name(name_)
			, defaultvalue(defaultvalue_)
			, isdefault(isdefault_)
			, value("")
			, isset(false)
			, setfrom(DEFAULT)
			, setfromcompid("")
			, targetcomp("")
            {}
            

            /**
             * \brief constructor for setting a parameter
             * \param name_  parameter name
             * \param value_   parameter value
             * \param setfrom_  the way the parameter is set
             * \param setfromcompid_  the id the parameter is set from
             * \param targetcomp_  optional target child id
             */
            Parameter(const std::string& name_,
            		  const std::string& value_,
            		  const  TypeFrom  setfrom_,
            		  const std::string& setfromcompid_,
            		  const std::string& targetcomp_ = "" )
            : name(name_)
			, defaultvalue("")
			, isdefault(false)
			, value(value_)
			, isset(true)
			, setfrom(setfrom_)\
			, setfromcompid(setfromcompid_)
			, targetcomp(targetcomp_)
			{}



            /**
             * \brief constructor for setting a parameter from a "comp:param=value" line 
             * \param linetoparse the line to be parsed
             * \param setfrom_   the way the parameter is set (should be CMDLINE, PARAMFILE or  ADL)
             * \param setfromfile the name of the file the parameter comes from (empty if CMDLINE)
             */
            Parameter(const std::string& linetoparse, const TypeFrom  setfrom_, const std::string& setfromfile);


            /**
             * \Brief set parameter from an exception. Used for interactive parameter setting
             * \param msg the caught exception the parameter to set is extracted from
             */
            Parameter(const CustomException& msg);


            /**
             * \brief return the parameter's name
             */
            std::string getName() const
            {
                return name ;
            }
            

            /**
             * \brief return true if parameter's value has been set, false otherwise
             */
            bool isSet() const
            {
                return isset;
            }


            /**
             * \brief return the parameter's value
             */
            std::string getValue() const
            {
                return value ;
            }

            /**
             * \brief set value,isset,setfrom and targetcomp of added parameter from a set parameter
             * \param param  the parameter data are extracted from to update the current parameter state
             * \param setcompid the full id of the component that sets this parameter value
             * \throw CustomException if parameter already set but with different values.
             */
            void setValue(const Parameter& param, const std::string& setcompid);


            /**
             * \brief set value of  added parameter to its default value.
             * \param setcompid the full id of the component that sets this parameter value
             * \throw CustomException if parameter already set but with different values.
             */
            void setValuetoDefault(const std::string& setcompid);




            /**
             * \brief return the component id that sets the parameter
             */
            std::string getSetFromComp() const
            {
                return setfromcompid ;
            }


            /**
             * \brief return the way the parameter was set
             */
            TypeFrom getSetFrom() const
            {
                return setfrom ;
            }


            /**
             * \brief return the way the parameter was set in a string
             */
            std::string getStringSetFrom() const;


            /**
             * C\brief return the name of the target component 
             */
            std::string getTargetComp() const
            {
                return targetcomp ;
            }
    

            /**
             * \brief return true is parameter has default value
             */
            bool isDefaultSet() const
            {
                return isdefault ;
            }

            /**
             * \brief return the parameter default value
             */
            std::string getDefault() const
            {
                return defaultvalue ;
            }



            /**
             * \brief XMLBuild Method.
             * \brief dump parameters in .adl.xml or .net.xml file
             * \param adl :  if adl is true, add attribute "from"
             */

            flowvr::xml::DOMElement*  XMLBuild(bool adl=true) const;
    };

  } // namespace app
} // namespace flowvr

#endif //__PARAMETER__
