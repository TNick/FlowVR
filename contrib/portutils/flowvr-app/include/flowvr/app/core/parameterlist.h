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
 *        File: ./include/flowvr/app/core/parameterlist.h          *
 *                                                                 *
 *   Contact : Antoine Vanel                                       *
 *                                                                 *
 ******************************************************************/



#ifndef __FLOWVR_APP_PARAMETERLIST_H
#define __FLOWVR_APP_PARAMETERLIST_H

#include <list>
#include <string>
#include <flowvr/xml.h>
#include <flowvr/app/core/parameter.h>


namespace flowvr { namespace app{

    /**
     * \class ParameterList
     * \brief  list of parameters
     */
    class ParameterList : public  std::list<Parameter>
    {
            
        public:

            /**
             * \brief default constructor
             */
            ParameterList()
            : std::list<Parameter>()
            {

            }


            /**
             * \brief getParam method searches  a given parameter and return an iterator
             * \param name parameter name
             * \param targetcomp parameter target component
             * \param setfrom  parameter setfrom attribute
             * \return iterator to the found parameter or iterator to end() if not found
             *
             */

            ParameterList::const_iterator getParam(const std::string& name,
            		                               const std::string& targetcomp,
            		                               const Parameter::TypeFrom setfrom) const;

            /**
             * \brief getParam method searches  a given parameter and return an iterator
             * \param beginsearch  the search starts from  this iterator position
             * \param name parameter name
             * \param targetcomp parameter target component
             * \param setfrom  parameter setfrom attribute
             * \return iterator to the found parameter or iterator to end() if not found
             *
             */

            ParameterList::iterator getParam(const ParameterList::iterator beginsearch,
            		                         const std::string& name,
            		                         const std::string& targetcomp,
            		                         const Parameter::TypeFrom setfrom);
           
            /**
             * \brief getParam method searches  a given parameter and return an iterator
             * \param name parameter name
             * \return iterator to the found parameter or iterator to end() if not found
             */

            ParameterList::iterator getParam(const std::string& name);

            /**
             * \brief XMLBuild Method.
             * \brief Build a list of parameters inside  <parameter> markup
             * \param adl :  if adl is true, add attribute "from"
             */
            xml::DOMElement*  XMLBuild(bool adl=true) const;


            /**
             * \brief eraseDuplicates suppress existing occurences of param (with different value) from the parameter list.
             * \param param the param to look for 
             * \return true if duplicate parameter found and erased
             */
            bool  eraseDuplicates(const Parameter& param);
                                        

            /**
             * \brief push_fromParameterList  add parameters  read from an other list of parameters. Remove duplicates
             * \param paramlist is the  list parameters are added from 
             */
            void push_fromParameterList(const ParameterList&  paramlist);


            /**
             * \brief push_fromCmdLine add parameters  read from command line. Remove duplicates
             * \param parameter_string string extracted from the command line
             */

            void push_fromCmdLine(const std::string &parameter_string);
            
            /**
             * \brief push_fromParamFile add parameters  read from the ADL XML file. Parameter inserted only if not already present.
             * \param head  pointeur to the DOM object of the ADL file
             */
            void push_fromParamFile(const std::string& parameter_file);
                
            /**
             * \brief push_fromAdl add parameters  read from the ADL XML file. Parameter inserted only if not already present.
             * \param adlfile adl file name
             */
            void push_fromAdl(const std::string& adlfile);


            /**
             * \brief push_fromStdIn add parameters  read from the standard input. Ask values for parameters with no values.

             * \param errorlist the list of exceptions related to missing parameter values
             */
            bool push_fromStdIn(const std::list<CustomException> listerror);

    };
    
	} // namespace app
} // namespace flowvr

#endif //__PARAMETER__
