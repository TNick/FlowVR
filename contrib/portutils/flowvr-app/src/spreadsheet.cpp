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


/** \file spreadsheet.cpp
 * \brief  classes to handle data spreadsheets.
 */


#include <fstream>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>
#include "flowvr/app/core/traverse.h"
#include "flowvr/app/core/spreadsheet.h"
#include "flowvr/app/core/component.h"


namespace flowvr { namespace app {
    
    
    
    void CsvFile::write(const HeaderList& header, const  DataMap& data)  throw (FileException)
    {
        std::ofstream file(filename.c_str()); 
        if (!file)  throw FileException("Cannot open file", filename, __FUNCTION_NAME__);
	
	
        // Print raw  of headers 
        for (HeaderList::const_iterator it = header.begin(); it != header.end(); ++it)
            {
                file << separator << *it;
            }
        // End of line
        file << std::endl;	
	
        // Print all data raws
        for (DataMap::const_iterator rawit = data.begin() ; rawit != data.end(); ++rawit)
            {
	    
                file<< rawit->first;
                for (DataRaw::const_iterator it = rawit->second.begin(); it != rawit->second.end(); ++it)
                    {
                        if (*it > 0) 
                            file << separator << *it;
                        else
                            file << separator;
		  
                    }

                // End of line
                file << std::endl;
            }
	
    };
    
 
    void CsvFile::read(HeaderList& header, DataMap& data) const  throw (FileException, CustomException)
    {
        std::ifstream file(filename.c_str()); 
        if (!file)  throw FileException("Cannot open file", filename, __FUNCTION_NAME__);
	
	
        // a buffer to store the line read
        std::string line;
	
        if ( ! std::getline(file, line))
            throw FileException("Cannot get the first line of file", filename, __FUNCTION_NAME__);
	

        unsigned int pointer = 0;
        unsigned int rawnb = 0;
        std::string word ="";
        int localHostColNumber = -1, tmp=0;
        bool uselocalhost=false;
        bool useotherthanlocalhost=false;
	
        // Parse the first line (header line)
        while(pointer < line.size())
            {
                while( pointer < line.size() && line[pointer] != separator)
                    {
                        //remove white spaces
                        if (line[pointer] != ' ')
                            {
                                word.push_back(line[pointer]);
                            }
                        pointer++;
                    }
                if( !word.empty())
                    {
                        // warning      if id    already in
                        if (std::find(header.begin(), header.end(), word) != header.end() )  
                            {
                                // Was error before, now just a warning: may be convenient  to have several time the same name in a header
                                Msg::warning("Duplicate header element '"+ word+"' in file "+filename, __FUNCTION_NAME__);
                                // throw FileException("Duplicate header elements'"+ word+"'",filename, __FUNCTION_NAME__);
                            }
                        header.push_back(word);
                        tmp++;
                     }
                // if we have "localhost" in csv file, note the column number.
                if(word == "localhost" || word == "Localhost" || word == "127.0.0.1")
                	localHostColNumber = tmp;

                 pointer++;
                word.clear();
            }
	  
        // Parse the lines
        while (std::getline(file, line))
            {
                rawnb++;
                pointer = 0;
                bool firstWord = true; // Are we parsing the first column
                unsigned int column = 0; // Which column is being scanned?
                std::string id;
                while(line.size() > pointer)
                    {	
                        // get word
                        word.clear();
                        while(line.size() > pointer && line[pointer] != separator)
                            {
                                if (line[pointer] != ' ')
                                    word.push_back(line[pointer]);
                                pointer++;
                            }
                        pointer++;
		  

                        if (firstWord)
                            {
                                // it's the map  id 
                                if ( word.empty() )  break; // forget about this line and go to parse next line
                                id = word;

                                if(data.find(id) != data.end())
                                	throw CustomException("Component : " + id + " is specified twice in CSV file.", __FUNCTION_NAME__ );

                                // init the raw of this id with zeros.
                                data[id].insert(data[id].begin(), header.size(),0);
                                firstWord = false;
                            }
                        else
                            {
                                if (!word.empty())
                                    {
                                	// store data
                                	try{
                                		data[id].at(column) = convertTo<int>(word);

                                		if (data[id].at(column) > 0)// one component instance at least mapped
                                		{
                                			if(column == localHostColNumber-1)   // we are reading in a locahost column
                                			{
                                				// Remember that we have one component mapped on a localHost.
                                				uselocalhost=true;
                                			}
                                			else
                                			{
                                				// Remember that we have one component mapped on an host different  from  localHost.
                                				useotherthanlocalhost=true;
                                			}
                                			if(uselocalhost && useotherthanlocalhost)
                                				throw FileException("Host name 'localhost' is used in CSV file in conjunction with other host names or IP address. Replace 'localhost' by IP address or machine name to avoid problems", filename,  __FUNCTION_NAME__);
                                		}
                                	}//end try
                                       catch(const BadConversion& e)
                                            {
                                                throw FileException("Raw="+toString<unsigned int>(rawnb+1)+", col="+toString<unsigned int>(column+2)+") contains non integer value="+word,filename, __FUNCTION_NAME__);
                                                //			      std::cerr << "ERROR: Csv File "<<fileName<<", element (raw="<<rawnb+1<<", col="<<column+2<<") contains non integer value="<<word<< std::endl;
                                            }
                                        catch(const std::out_of_range& e)
                                            {
                                                throw FileException("Element (raw="+toString<unsigned int>(rawnb+1)+", col="+toString<unsigned int>(column+2)+")  out of column  (value="+word+", ignored)",filename, __FUNCTION_NAME__);
                                                //			      std::cerr << "WARNING : Csv File "<<fileName<<", element (raw="<<rawnb+1<<", col="<<column+2<<")  out of column  (value="<<word<<", ignored)"<< std::endl;
                                            }

                                    }
                                column++;
                            }
                    }
            }
    };


    void SpreadSheet::importFromFile(CsvFile csvfile)
    {
    	try{
        csvfile.read(this->header,this->data);
    	}
    	catch (CustomException& e){
    		throw e;
    	}

        sourcefile= csvfile.getFileName(); 
    };
    
  
    void SpreadSheet::exportToFile(CsvFile csvfile) const
    {
        csvfile.write(this->header,this->data);
    };
    
    void SpreadSheet::checkApp(const Composite* app) const
    {
        if (!app)
            throw NullPointerException("application id=", app->getFullId(), __FUNCTION_NAME__);
        // could be extended to test id prefix and detect ids in the spreadsheet that are not part of the application.
    };


    void SpreadSheetHosts::importFromApp(Composite* app)
    {

    	if (!app)
            throw NullPointerException("application id=", app->getFullId(), __FUNCTION_NAME__);

        /// Set a traverse (one pass)  to call the method getHostMap (goal: to get a map  of all hosts used by each component)


        Controller< HostMap, void > cHostMap(& Component::getHostMap,"getHostMap");
        Traverse< HostMap, void > tHostMap;

        HostMap hostmap= tHostMap(app, &cHostMap).result; /// call traverse on getHostMap
	
        //Clear spreadsheet
        header.clear(); 
        data.clear();
	
        // Iterate through the mapList and create lines of the tab
        for (HostMap::const_iterator it = hostmap.begin(); it != hostmap.end(); ++it)
        {
        	// Add Hosts
        	for (HostList::const_iterator itHost = it->second.begin(); itHost != it->second.end(); ++itHost)
        	{
        		int index = header.index((*itHost).getName());
        		if (index < 0)  // found new host. Update header
        		{
        			// insert missing host
        			header.push_back((*itHost).getName());
        		}
        	}
        }


        // Iterate through the mapList and create data lines 
        for (HostMap::const_iterator it = hostmap.begin() ; it != hostmap.end(); ++it)
            {
                // Allocate the vector (fill with zeros)
                if ( data.find(it->first) !=  data.end())   
                    throw CustomException("Object id="+it->first+" already in spreadsheet", __FUNCTION_NAME__);

                data[it->first].assign(header.size(),0);
                // Add Hosts
                for (HostList::const_iterator itHost = it->second.begin(); itHost != it->second.end(); ++itHost)
                {
                	try{
                		data[it->first].at(header.index((*itHost).getName()))++;
                	} catch(const std::out_of_range& e)
                	{
                		throw CustomException("Host "+(*itHost).getName()+" found in application but not in spreadsheet header", __FUNCTION_NAME__);
                	}
                }
            }

    };


    void  SpreadSheetHosts::importFromApp(const std::list<Component*> failedcomponents)
    {
        // Iterate through the mapList and create lines of the tab
        for (std::list<Component*>::const_iterator it = failedcomponents.begin() ; it != failedcomponents.end(); ++it)
            {
                data[(*it)->getFullId()].assign(header.size(),0);
            }
    };


    HostMap SpreadSheetHosts::exportToHostMap()
    {
      
        HostMap hostmap;
      
        // Turn the  Data map into a HotMap. It will be used  latter to  applied the data  to the application (through a traverse)
        for (DataMap::iterator rawit = data.begin(); rawit != data.end(); rawit++)
            {
                // iterate on each host
                unsigned int i = 0; // host index
                for ( DataRaw::iterator it = rawit->second.begin(); it != rawit->second.end(); it++ )
                    {
                		Host h(header[i],sourcefile);
                        // add in the list *it  occurrences of the host
                        for ( int l =0; l < *it; l++ )
                            {
                                hostmap[rawit->first].push_back(h);
                            }
                        i++;
                    }
            }
        return hostmap;
    };


    void SpreadSheetHosts::checkArch(const Architecture* arch) const  throw (CustomException)
    {
        const Machine* m = NULL;
        if (!arch)
            throw NullPointerException("architecture", "", __FUNCTION_NAME__);
        for (HeaderList::const_iterator it = header.begin(); it != header.end(); ++it)
            {
                try{
                    m = &(arch->findMachine(*it));
                }
                catch( const CustomException& e)
                    {
                        Msg::warning("In architecture file " + arch->getFileName() + ", a exception was raised : ", __FUNCTION_NAME__);
                        Msg::warningException(e);
                    }
            }
    };

};};
