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
 *       File : flowvr/core/spreadsheet.h                     *
 *                                                                 *
 *                                                                 *
 *       Contact : Burno Raffin                                    *
 *                                                                 *
 ******************************************************************/


/** \file spreadsheet.h
 * \brief  classes to handle data spreadsheets. Should not be used directly
 *
 * A spreadsheet is either used  to get data from an application and have it formated in a convenient way, 
 *  or to used data from a file and apply it to an application.
 */    


#ifndef __FLOWVR_APP_SPREADSHEET__
#define __FLOWVR_APP_SPREADSHEET__

#include <vector>
#include <map>
#include "flowvr/app/core/arch.h"
#include "flowvr/app/core/host.h"
#include "flowvr/app/core/component.h"


namespace flowvr { namespace app {

    /// \brief Utility definition. Used for the spreadhseet data structure
    typedef  std::vector<int> DataRaw;
    typedef  std::map<std::string, DataRaw> DataMap;



    /** \class HeaderList  
     * \brief Utility class to store the  spreadsheet header list (list of host names for instance).
     * A spreadsheet is composed of a header (first row)
     */
    class  HeaderList: public std::vector<std::string>
    {
        public : 

            /** \brief Constructor
             */
            HeaderList(): std::vector<std::string>(){};

            /** \brief  Return the position (index) of the element in the list or -1 if element not found. 
             * \param s  The string to search
             */
            unsigned int index(const std::string s)
            {
                unsigned int i;
                for ( i = 0;  i < this->size() && this->at(i) !=s ; i++);
                if ( i< this->size() ) return i; 
                return -1; 
            };
    };


    /** \class  CsvFile 
     * \brief A  csv file consisting of one header row, one object column and integers numbers defining the number of objects for the corresponding header data.
     */
    class CsvFile
    {
        private:
            std::string filename;
            char separator;

        public:

            /** \brief constructor
             * \param filename_ The name of the csv file
             * \param separator_ optional argument to change the separator
             */

            CsvFile(const std::string filename_, const  char separator_=','): filename(filename_),separator(separator_){};


            /** \brief get file name
             * \return file name 
             */
            const std::string& getFileName() const {return filename;};

            /** \brief write to a csv file. 
             * \param  header the list of headers (first raw of the spreadsheet
             * \param  data  the map giving for each component the elements attached to it (the other raws of the spreadsheet)
             */
            void write(const HeaderList& header, const  DataMap& data)  throw (FileException);



            /** \brief read from  a csv file. 
             * \param  header the list of headers (first raw of the spreadsheet)
             * \param  data  the map giving for each component the elements attached to it (the other raws of the spreadsheet)
             */
            void read(HeaderList& header, DataMap& data)  const throw (FileException, CustomException);

    };

    /** \class SpreadSheet   
     * \brief Enable to build a spreadsheet, export or import it to a cvsfile or to an application
     */
    class SpreadSheet
    {
        protected:
            HeaderList  header;  ///< the spreadsheet header 
            DataMap  data; ///< the data (key map correspond to the object)
            std::string sourcefile; ///< the name of the file the spreadsheet data are read from 


        public:

            /** \brief constructor
             */
            SpreadSheet():sourcefile(""){};


            /** \brief get name of file the spreadsheet has been read from
             * \return file name or empty string if spreadsheet not built from file
             */
            const std::string& getSourceFile() const {return sourcefile;};

            /** \brief Import data from a CSV file
             * \param csvfile the CSV file
             */
            void importFromFile(CsvFile csvfile);

            /** \brief Export  data to a CSV file
             * \param csvfile the CSV file
             */
            void exportToFile(CsvFile csvfile) const;

            /** \brief Check the spreadsheet data is conform to the application
             * \param app The application 
             */
            void checkApp(const Composite* app) const;

    };



    /** \class SpreadSheetHosts 
     * \brief Specialized spreadsheet for host mapping
     */

    class SpreadSheetHosts : public SpreadSheet
    {

        public : 

            /** \brief constructor
             */
            SpreadSheetHosts():SpreadSheet(){};


            /** \brief  fill the data structure with the data read from the application
             * \param app The application  data are read from
             * 
             */
            void importFromApp(Composite* app);

            /** \brief  fill the data structure from a  list of components
             * \param failedcomponents The list of components   data are read from
             */
             void importFromApp(const std::list<Component*> failedcomponents);

            /** \brief  export  spreadsheet data to a map of hosts.
             * \return a map of hosts
             *
             * Data structure better adapted to get the host list during component mapping
             */
             HostMap exportToHostMap();

            /** \brief  Test spreadsheet integrity against the architecture data.
             * \param arch The description of the target architecture
             * \throw CustomException if something wrong
             * 
             * Basically check that each host mentioned into the spreadsheet is present in the architecture
             */
            void checkArch(const Architecture* arch) const throw (CustomException);


    };

};};

#endif
