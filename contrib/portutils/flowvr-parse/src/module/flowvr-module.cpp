/******* COPYRIGHT ************************************************
*                                                                 *
*                             FlowVR                              *
*                       Configuration Parser                      *
*                                                                 *
*-----------------------------------------------------------------*
* COPYRIGHT (C) 2003-2011                by                       *
* INRIA and                                                       *
* Laboratoire d'Informatique Fondamentale d'Orleans               *
* (FRE 2490). ALL RIGHTS RESERVED.                                *
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
* File: src/main.cpp                                              *
*                                                                 *
* Contacts:                                                       *
*  12/10/2003 Sophie Robert <Sophie.Robert@lifo.univ-orleans.fr>  *
*                                                                 *
******************************************************************/
/*! \file main.cpp 
 *
 * \brief The main program for the flowvr-run command
 *
 */

#include "flowvr/parse/builder.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>


/*! \fn void usage()
 *  \brief Usage for flowvr-run 
*/

using namespace flowvr::parse;

void usage()
{
   std::cerr << "Usage : flowvr-module -mml file [-d dir:dir:...] [-desc  file1 file2 ...]  [-ro outfile] [-mo outfile] [-run] [-modlist] [-dtd uri]\n";
   std::cerr << "        -mml : a metamodule list file\n";
   std::cerr << "        -d   : search metamodule descriptions within  these directories\n"; 
   std::cerr <<"                (using the file names given in the .mml file) \n";
   std::cerr << "        -desc: the  files where to find the metamodule descriptions \n";
   std::cerr << "               (dont use the names given in the .mml file) \n";
   std::cerr << "        -ro  : output file to store  the run commands, use stdout if omitted \n";
   std::cerr << "        -mo  : output file to store  the module list, use stdout if omitted\n";
   std::cerr << "        -run : compute the  run  commands \n";
   std::cerr << "        -mod : compute the partial  module list\n";
   std::cerr << "        -dtd : overwrite the default dtd URI included in the output file of the module list \n";
   std::cerr << "      WARNING: metamodule descriptions  are searched in the following order:\n"; 
   std::cerr << "               1) the -desc files\n";
   std::cerr << "               2) the -d directories (using the file names given in the .mml file)\n";
   std::cerr << "               3) the directories given by the $FLOWVR_DESC_PATH (using the file names given in the .mml file)\n";
   exit(1);
}


/*! \fn std::vector<char *> getfilelist(int argc, char **argv, char * param)
 *  \brief Extract the list of files coresponding to the param option 
 *  \param int argc: classical C argc parameter
 *  \param char **argv: classical C argv parameter
 *  \param char * param:  the option to check files for ("-mml" for instance)
 *  \return a vector of filenames
*/
std::vector<const char *> getfilelist(int argc,const  char **argv,const  char * param)
{
  int i;
  std::vector<const char *> filelist;
  
  for(i = 1; (i<argc) && (strcmp(argv[i],param) != 0) ; i++); 
  if ( i < argc && strcmp(argv[i],param) == 0) 
    {
      i++;
      for (;(i <argc) && (argv[i][0] != '-');i++){
	filelist.push_back(argv[i]);
      }
    }
  return filelist;

}


/*! \fn bool  checkarg(int argc, char **argv, char * param)
 *  \brief Check for the presence of the param option in the input argument
 *  \param int argc: classical C argc parameter
 *  \param char **argv: classical C argv parameter
 *  \param char * param:  the option to check files for ("-desc" for instance)
 *  \return a boolean
*/
bool checkarg(int argc, const char **argv, const char * param)
{
  int i;
  
  for(i = 1; (i<argc) && (strcmp(argv[i],param) != 0) ; i++); 
  if ( i < argc && strcmp(argv[i],param) == 0) 
    {
      return true;
    }
  return false;

}

/*! \fn int main(int argc, char **argv)
 *  \brief The main function fr flowvr-run
*/

int main(int argc, const char **argv)
{

  std::vector<const char *> metaDescFileList;
  std::vector<const char *> metaModuleFileList;
  std::vector<const char *> runOutFileList;
  std::vector<const char *> modOutFileList;
  std::vector<const char *> uriList;

  std::string resultingPath;
  flowvr::utils::FilePath dirList("share/flowvr/modules", "FLOWVR_DESC_PATH", resultingPath );
  std::cout << "FilePath resolved: [ " << resultingPath << "] for FLOWVR_DESC_PATH" << std::endl;

  if (argc < 3 )  usage();

  // this one is mandatory
  metaModuleFileList = getfilelist(argc,argv,"-mml");
  if ( metaModuleFileList.size() != 1) usage();


  if (checkarg(argc,argv,"-desc"))
    {
      metaDescFileList = getfilelist(argc,argv,"-desc");
      if (metaDescFileList.size()<=0 ) usage();
    }

  if (checkarg(argc,argv,"-d"))  
    {
      std::vector<const char*> l = getfilelist(argc,argv,"-d");
      for (unsigned int i=0;i<l.size();i++)
    	dirList.addPath(l[i]);
    }
  
  if (checkarg(argc,argv,"-ro"))  
    {
      runOutFileList = getfilelist(argc,argv,"-ro");
      if ( runOutFileList.size() != 1) usage(); 
    }

  if (checkarg(argc,argv,"-mo"))  
    {
      modOutFileList = getfilelist(argc,argv,"-mo");
      if ( modOutFileList.size() != 1) usage(); 
    }

  if (checkarg(argc,argv,"-dtd"))  
    {
      uriList = getfilelist(argc,argv,"-dtd");
      if ( uriList.size() != 1) usage(); 
    }

  // To keep white spaces
  TiXmlBase::SetCondenseWhiteSpace(false);


  MetaModuleListBuilder  builder(*metaModuleFileList.begin(), metaDescFileList,
				 dirList);

  builder.Init();

  if (checkarg(argc,argv,"-run"))
  {
    builder.BuildRun();

    if (!runOutFileList.empty()) 
    {
      std::ofstream f(* runOutFileList.begin());
      builder.PrintRunHeader(&f,(uriList.empty()  ? "" : *uriList.begin()));
      builder.PrintRun(&f);
      builder.PrintRunFooter(&f);
      f.close();
    }
    else
    {
      builder.PrintRunHeader(& std::cout,(uriList.empty()  ? "" : *uriList.begin()));
      builder.PrintRun(& std::cout);
      builder.PrintRunFooter(& std::cout);
    }
  }

  if (checkarg(argc,argv,"-mod"))
  {
    builder.BuildModuleList();
    
    if (!modOutFileList.empty()) 
    {
      std::ofstream f(* modOutFileList.begin());
      builder.PrintModHeader(&f,(uriList.empty()  ? "" : *uriList.begin()));
      builder.PrintModuleList(&f);
      //f.close();
      //builder.ExecScript(*modOutFileList.begin());
      //f.open(* modOutFileList.begin(),std::ios_base::app);
      builder.PrintModFooter(&f);
      f.close();
    }
    else
    {
      builder.PrintModHeader(& std::cout,(uriList.empty()  ? "" : *uriList.begin()));
      builder.PrintModuleList(& std::cout);
      //builder.ExecScript(NULL);
      builder.PrintModFooter(& std::cout);
    }
  }

  return 0;
}


