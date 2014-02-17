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
 * \brief The main program for the tests
 *
 */


#include "flowvr/parse/ParserCommands.h"


using namespace flowvr::parse;


void usage()
{
    std::cerr << "Usage : flowvr-network  -net network_file [-o output_file] [-object] [-route] [-start]\n";
    std::cerr << "         -net  the network file to compute the commands from  \n";
    std::cerr << "         -o    output file name, STDOUT if omitted\n";
    std::cerr << "         -object : generate only the object related commands \n";
    std::cerr << "         -route  : generate only the route related  commands \n";
    std::cerr << "         -start  : generate only the starting  commands \n";
   exit(1);
}


// Return 0 if pb, 1 if ok, 2 if missing
int  checkfilename(int argc, char **argv, char * param, char ** file)
{
  int i;

  for(i = 1; (i<argc) && (strcmp(argv[i],param) != 0) ; i++);
  if ( i < argc && strcmp(argv[i],param) == 0)
    {
      i++;
      if ( (i <argc) && (argv[i][0] != '-') ){
	*file = argv[i];
	//printf("%s",*file);
      }
      else
	{
	  //printf("no");
	  return 0;
	}
    }
  else
    {
      //printf("missing");
      return 2;
    }
  //printf("ok");
  return 1;
}

int main(int argc, char **argv)
{

  char * InstGraphFile = NULL;
  char * OutputFile = NULL;
  char * temp = NULL;

  bool printObject = false;
  bool printRoute = false;
  bool printStart = false;

  if (argc < 2) usage();
  if(checkfilename(argc,argv,(char*)"-net",&InstGraphFile) != 1 ) usage();
  if(checkfilename(argc,argv,(char*)"-o",&OutputFile) == 0)  usage();
  if(checkfilename(argc,argv,(char*)"-object",&temp) == 0)  printObject = true;
  if(checkfilename(argc,argv,(char*)"-route",&temp) == 0)  printRoute = true;
  if(checkfilename(argc,argv,(char*)"-start",&temp) == 0)  printStart = true;

  if( !printObject and !printRoute and !printStart )
  {
    printObject = true;
    printRoute = true;
    printStart = true;
  }

  PassOptions(printObject,printRoute,printStart);
  ParserCommands(InstGraphFile,OutputFile);
  return 0;
}



