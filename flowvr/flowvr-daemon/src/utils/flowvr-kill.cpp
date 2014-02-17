/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                     Daemon and Base Plugins                     *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA and                                                       *
* Laboratoire d'Informatique Fondamentale d'Orleans               *
* (FRE 2490) ALL RIGHTS RESERVED.                                 *
*                                                                 *
* This source is covered by the GNU GPL, please refer to the      *
* COPYING file for further information.                           *
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
* File: flowvr-suite/flowvr/flowvrd/src/utils/flowvr-kill.cpp     *
*                                                                 *
* Contacts: antoine.vanel@imag.fr                                 *
*                                                                 *
*                                                                 *
******************************************************************/

#include <string>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include "flowvr/module.h"
#include "flowvr/utils/cmdline.h"
#include "flowvr/xml.h"


typedef flowvr::xml::DOMElement XML;

int main(int argc, char** argv) {
	
	std::string hosts_list;
	std::string hostFileName;
	bool dist=true;
	bool no_clean = false;
	//bool rm = true;

	
	std::string description = "usage: flowvr-kill [-F textfile] [-L \"hosts ...\"] ";
	flowvr::utils::CmdLine cmdline(description.c_str());
	 
	//declaration des options   
	cmdline.opt<std::string>("hostfile", 'F', "read the specified [hostfile.txt] which specify the hosts to be mop up" ,&hostFileName, false);
	cmdline.opt<std::string>("list", 'L', "explicit list of the hosts to be clean, separated by spaces, between quotes" ,&hosts_list, false);
 
	
	//cmdline.opt<bool>("distant", 'd', "launch kill on local processes only", &dist); 	
	cmdline.opt<bool>("noclean", 'n', "don't delete the log files", &no_clean); 
	
	//parse the command line 			
	if ( ! cmdline.parse(argc, argv) ) return 1;
	
		if(no_clean){
                    //		std::cout << "don't delete the log files" << std::endl;
                }
	
	//
	if(hostFileName != ""){
		std::cout << "kill all zombie processes on hosts listed in : "<< hostFileName << std::endl;
		// open the hostFile :
	 	std::ifstream fichier_tmp(hostFileName.c_str());
		 if ( fichier_tmp ) {
		 	        std::string line_tmp; 
        			while ( std::getline( fichier_tmp, line_tmp ) )
        			{
        				std::cout << " -  " << line_tmp << "\n" << std::endl;
        				std::string system = "flowvr-run-ssh " + line_tmp + " flowvr-kill" ;
        				if(no_clean) system += " -c"; 
        				int getReturnValue = std::system(system.c_str());
        			}
		 }
		 else { std::cout << "can not open the specified file..." << std::endl;}		
	}
	
	else if (hosts_list != ""){
		std::cout << "Kill all zombie processes on hosts : " << hosts_list << std::endl;
		
				std::istringstream iss(hosts_list);
				//iss = hosts_list;
			    std::string mach;
			    while ( std::getline( iss, mach, ' ' ) )
			    {
			    	std::string system =  "flowvr-run-ssh " + mach + " flowvr-kill";
			    	if(no_clean) {system += " -c";}
			    	//std::cout << system
			    	int getReturnValue = std::system(system.c_str());
			  		std::cout << "flowvr-kill launched on host : "<< mach << std::endl;
			    }
	}
	
	else if(dist){
			// form the working path
			std::string cmd_one, full_cmd;
			cmd_one += getenv("HOME");
			cmd_one += "/.flowvr/";

			// get the host name to launch kill command only on local processes			
			char host[32]; 
			gethostname(host, 32);
			
		     full_cmd += "ls " + cmd_one + "*.log | grep " + host + " > " + cmd_one + "workfile";
                     std::cout << "Kill all processes logged in "<<cmd_one<<"*_PID.log files"<<std::endl;
			// write all the log files in a temp file
			int getReturnValue = std::system(full_cmd.c_str());
			
			full_cmd = cmd_one + "workfile";
			// open it
			 std::ifstream fichier_tmp( full_cmd.c_str() );
			 if ( fichier_tmp ) 
		    {
		        std::string line_tmp; 
		        while ( std::getline( fichier_tmp, line_tmp ) )
		        {
		            //std::cout << line_tmp << std::endl;
		            std::ifstream fichier_log(line_tmp.c_str());
		            if (fichier_log){
		            	std::string line_log, cmdLine;

		            	std::getline( fichier_log, line_log ); //first line contains PID
		            	std::getline( fichier_log, cmdLine);   //second one contains cmdline

		            	std::string test = "ps -p " + line_log + " > /dev/null";
		            	int ret = std::system(test.c_str());

		            	// check command line of the concerned pid
		            	// to be sure we are about to kill the right process.
		            	std::string process_name;
		            	std::string getName = "/proc/" + line_log + "/cmdline";
		            	std::ifstream proc_file(getName.c_str());
		            	std::getline( proc_file, process_name);

		            	if(ret == 0){ // if process exists
		            		if(strcmp(process_name.c_str(), cmdLine.c_str())==0)
		            			// if the name of the process is still coherent
								{
									std::string cmd = "kill -9 " + line_log;
									int getReturnValue = std::system(cmd.c_str());
								}
		            		else
		            			std::cout << "ignoring pid " << line_log << ". Process ID has been reused since the log file has been created" << std::endl;

		            	}
		            }            
		        }
		    }
		    fichier_tmp.close();
		    
			if(!no_clean)
				{
					full_cmd = "rm " + cmd_one + "workfile > /dev/null";
					int getReturnValue = std::system( full_cmd.c_str() );

					if( std::system (std::string("ls " + cmd_one + "*_PID.log | grep " + host + " > /dev/null").c_str()) == 0){
						full_cmd = "rm `ls " + cmd_one + "*_PID.log | grep " + host + "` > /dev/null";
						getReturnValue = std::system( full_cmd.c_str() );
					}
				}
	}
	

	
	
	return 0;
}
