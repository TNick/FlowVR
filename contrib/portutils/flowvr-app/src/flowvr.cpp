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
 *        File: ./src.flowvr.cpp                                   *
 *                                                                 *
 *	Contact : Jean-Denis Lesage                               *
 *                                                                 *
 ******************************************************************/


/*! \mainpage
The goal of the FlowVR library is to provide users with the necessary tools to develop and
run high performance interactive applications on PC clusters and Grids. The main target
applications include virtual reality and scientific visualization. FlowVR enforces a modular
programming that leverages software engineering issues while enabling high performance
executions on distributed and parallel architectures.
 */




#include <cctype>
#include <algorithm>
#include <unistd.h>
#include <sys/stat.h>
#include <flowvr/parse/ParserCommands.h>
#include <flowvr/telnet.h>
#include <flowvr/utils/filepath.h>
#include <flowvr/utils/cmdline.h>
#include <string>
#include <iostream>
#include "flowvr/app/core/class.h"
#include "flowvr/app/core/genclass.h"
#include "flowvr/app/core/arch.h"
#include "flowvr/app/core/spreadsheet.h"
#include "flowvr/app/core/traverse.h"
#include "flowvr/app/core/msg.h"
#include "flowvr/app/core/controller.h"
#include "flowvr/app/core/parameter.h"
#include "flowvr/app/core/parameterlist.h"
#include "flowvr/app/core/applicationTracer.h"

using namespace flowvr::app;
using namespace flowvr::xml;




#define EXECUTE (1 << 0) 
#define SETHOSTS (1 << 1)
#define RESOLVEMSGTYPES (1 << 2)
#define SETCONNECTION (1 << 3)
#define XMLBUILD  (1 << 4)
#define RUNBUILD  (1 << 5)

int main(int argc, char** argv)
{
    const std::string compclass="[CompClass]";
    const std::string compclasslower="[compclass]";

    std::string description = "usage: flowvr [options] "+ compclasslower;

    flowvr::utils::CmdLine cmdline(description.c_str());

    bool onLocalHost = false;
    bool netStop = false;
    bool cmdObject = false;
    bool cmdRoute = false;
    bool cmdStart = false;
    bool launch = false;
    bool interactiveStart = false;
    bool launchwocompilation = false;
    bool trace = false;
	
    bool justListComponents = false;

    std::string  prefixFile = compclasslower;


    std::string archFile = ( getenv("FLOWVR_ARCH_FILE") == NULL ? "": getenv("FLOWVR_ARCH_FILE"));
    std::string adlFile = compclasslower+".adl.in.xml";
    std::string complib="";
    std::string compid=compclasslower;
    std::string parameter_string = "";
    std::string parameter_file = "";


    /// CSV spreadsheet (host mapping) related variables
    SpreadSheetHosts spreadsheetin;
    SpreadSheetHosts spreadsheetout;
    SpreadSheetHosts spreadsheeterr;
    std::string  csvFile = compclasslower+".csv";


    // Extra options (in addition to flowvrd/src/utils/telnet.cpp options)
    cmdline.opt<bool>("localhost", 'L', "map all objects on localhost", &onLocalHost);
    cmdline.opt<bool>("net", 'n', "stop compilation process after net.xml and run.xml generation", &netStop);
    cmdline.opt<bool>("object", 'o', "generate only objects related commands in cmd.xml", &cmdObject);
    cmdline.opt<bool>("route", 'r', "generate only route related commands in cmd.xml", &cmdRoute);
    cmdline.opt<bool>("start", 'R', "generate only starting commands in cmd.xml", &cmdStart);
    cmdline.opt<bool>("launch", 'x', "launch application", &launch);
    cmdline.opt<bool>("launchwocompilation", 'X', "launch application with former .cmd.xml and .run.xml files", &launchwocompilation);
    cmdline.opt<bool>("trace", 't', "enable trace capture",&trace);
    //    cmdline.opt<bool>("interactiveStart", 'I', "Start application in interactive mode (commands must be loaded manually", &interactiveStart);
    cmdline.opt<std::string>("prefix", 'p', "prefix for output files",&prefixFile,false);
    cmdline.opt<std::string>("arch", 'a', "architecture file",&archFile,false);
    cmdline.opt<std::string>("hostmap", 'H', "Spreadsheet file for host mapping (csv format)",&csvFile,false);
    cmdline.opt<std::string>("adlfile", 'd', "input adl file",&adlFile,false);
    cmdline.opt<std::string>("parameter", 'P', "parameter values (syntace: -Pcompfullid:paramname=value,comp2:paramname='a string with spaces' ...)", &parameter_string, false);
    cmdline.opt<std::string>("parameterFile", 'Q', "parameter value file", &parameter_file, false);
    cmdline.opt<std::string>("complib", 'c', "full name of the library file containing the component to load",&complib,false);
    cmdline.opt<std::string>("compid", 'i', "set id of loaded root component",&compid,false);
    cmdline.opt<bool>("list", 'S', "just list all loadable components contained in library file.", &justListComponents);

    // Parse commande line
    // Exit if error or --help option
    bool error = false;
    if ( !cmdline.parse(argc, argv, &error ) )
    {
    	std::cout << cmdline.help() << std::endl;
    	if(error)
    		return 1;
    	return 0;
    }

    // check if -P is used more than once.
    flowvr::utils::BaseOption* Poption = cmdline.getOpt("parameter");
    if(Poption != NULL && Poption->count > 1)
    	Msg::error("-P option can only be used once (use ',' to separate different parameter settings)",__FUNCTION_NAME__);

    // Increase the verbosity level by the number of occurences of the verbose option
    flowvr::utils::BaseOption*  verb = cmdline.getOpt("verbose");

    if ( verb  != NULL)
        Msg::verboseLevel+= verb->count;

    // component name  (also the component class name)
    std::string compname;

    if (!justListComponents)
    {
        // if justListComponents, the name of main component is useless
        if  ( cmdline.args.size() !=  1 || cmdline.args[0].empty() )
        {
            Msg::error("One and only one Component Class Name needed  as input argument",__FUNCTION_NAME__);
            return 1;
        }
        // component name  (also the component class name)
        compname = cmdline.args[0];

    }

    // We actually always use the lower case version of the component name. 
    // So we are case insensitive regarding the component name 
    // The dynamic component loader (@class GenClass) also register the component name using its lower case name.
    std::string compnamelower = compname;
    std::transform(compnamelower.begin(), compnamelower.end(),compnamelower.begin(),::tolower);

    // Restore  option values to default value  if not overwritten
    if (prefixFile == compclasslower)
        prefixFile=compnamelower;
    if (csvFile == compclasslower+".csv")
        csvFile=compnamelower+".csv";
    if (compid == compclasslower)
        compid=compnamelower;

    // Set name of csv output files
    std::string csvout = prefixFile+".out.csv";
    std::string csverr = prefixFile+".err.csv";


    if (onLocalHost)
        Msg::debug("--localhost option enabled: all components mapped onto localhost",__FUNCTION_NAME__);
    else
        Msg::debug("Use spreadsheet file: "+csvFile,__FUNCTION_NAME__);


    // if complib empty use  component name to deduct the lib name that contains this component
    if (complib.empty())
    {
        // lowercase all letters for the lib name
        complib=compnamelower;

        // To be improved by pushing all these platform dependent issues at the cmake level
#ifdef __APPLE__

        complib="lib"+complib+".comp.dylib";
#else

        complib="lib"+complib+".comp.so";
#endif

    }

#ifdef __APPLE__
    static flowvr::utils::FilePath libpath("DYLD_LIBRARY_PATH");
#else

    static flowvr::utils::FilePath libpath("LD_LIBRARY_PATH");
#endif

    // Dynamic component loading
    Composite* app;


    if(launchwocompilation) {

      app = new Composite("Dummy toplevel composite"); 

    } else {
      
    
        // Check we  find  the library
        if (!libpath.findFile(complib))
        {
            Msg::error("Cannot find "+complib,__FUNCTION_NAME__);
            return 0;
        }
        Msg::debug("Found "+complib,__FUNCTION_NAME__);
    
        if (justListComponents)
        {
            Class::listComponents(complib);
            return 0;
        }
    
        try
        {
            Class* myClass = Class::find(compnamelower,complib);// load component library
            if(myClass != NULL)
            {
            	Msg::debug("Create instance of component "+compnamelower+" with id="+compid,__FUNCTION_NAME__);
            	app = dynamic_cast<Composite*>(myClass->create(compid));// create instance of component
            }
    
        }
        catch (const CustomException& e)
        {
            Msg::errorException(e);
            return 0;
        }
    
    }


    // Set Architecture  File
    bool foundArchFile = false;
    if (! archFile.empty() )
    {
        struct stat s;
        if (!stat(archFile.c_str(),&s))
        {
            // File found
            Msg::debug("Use architecture file: "+archFile,__FUNCTION_NAME__);
            foundArchFile = true;

            app->addParameter("archFile");
            app->setParameter("archFile", archFile);

        }
    }

    if ( !foundArchFile )
    {
        Msg::debug("No architecture file found. Disable architecture check",__FUNCTION_NAME__);
    }

    


    // initialize the list of external parameters (set from command line or files)
    app->paramListSetExternal = new ParameterList;

    // load parameters from command line
    try
        {
            app->paramListSetExternal->push_fromCmdLine(parameter_string);
        }
    catch (const CustomException &e)
        {
            Msg::errorException(e);
            return -5;
        }
        
    // load parameters from parameter file
    app->paramListSetExternal->push_fromParamFile(parameter_file);


    // if no adl file provided try default name
    if (adlFile == compclasslower+".adl.in.xml")
    {
        adlFile = compnamelower+".adl.in.xml";
    }
    // extra parameters from adl file
    app->paramListSetExternal->push_fromAdl(adlFile);


    Architecture arch;
    if (foundArchFile &&   !onLocalHost)
    {
        try
        {
            arch.importArchitecture(archFile);
        }
        catch(const FileException& e)
        {
            Msg::warningException(e);
        }
    }

    /// Read the spreadsheet for host mapping data

    if (!onLocalHost && !launchwocompilation)
    {

		try
		{
			spreadsheetin.importFromFile(CsvFile(csvFile));
		}
		catch(const CustomException& e)
		{
			Msg::error("FATAL ERROR during CSV parsing \n\tError is:",__FUNCTION_NAME__);
			Msg::errorException(e);
			return -5;
		}

		try
		{
			/// check spreadsheet content against architecture description
			if (foundArchFile)
				spreadsheetin.checkArch(&arch);
        }
        catch(const FileException& f)
        {
            Msg::warningException(f);
        }
    }


    ///  Extract data from CSV spreadsheet and instanciate component static member to point to it:
    HostMap hostmap = spreadsheetin.exportToHostMap();
    if(onLocalHost)
        // if the application is mapped on localhost, we don't need informations from CSV
    	Component::csvHostMap = NULL;
    else
    	Component::csvHostMap = &hostmap;


    /// Set traverse (fixe point mode)  for calling the execute method
    Controller<void, void> controlExecute(&Component::execute,"execute");
    TraversePointFixe treeExecute(app, &controlExecute);


    /// Set  traverse (fixe point mode)  for calling the setHosts method
    Controller<void, void> controlSetHosts(&Component::setHostsMainController,"setHosts");
    TraversePointFixe treeSetHosts(&controlSetHosts); // Passive traverse (does not add new components to its todo list externaly set).

    if(onLocalHost){
		controlSetHosts.setMethod(&Component::setLocalHost); //use a specific functor for mapping on localhost.
		controlSetHosts.setName("setLocalHost");
    }

    unsigned char pipelineFailed = 0; /// array of bits. bit i => bool not(stage i succeeded) ?


    Msg::debug("Execute and SetHosts stages starting:", __FUNCTION_NAME__);

    if(!launchwocompilation)
    {
        // We interleave a host mapping and execute as one may need the results of the other to progress. 
        do
            {
                try{
                    treeExecute(); /// call traverse on execute method
                }
                catch( CustomException& e )
                    {
                        Msg::error("FATAL ERROR during "+controlExecute.name+ "- Exit\n\tError is:",__FUNCTION_NAME__);
                        Msg::errorException(e);
                        return -5;
                    }
                
                // Get the components that successfully applied execute and insert them in the list of setHost.
                treeSetHosts.toDoComponent.splice(treeSetHosts.toDoComponent.begin(),treeExecute.doneComponent);
                try{
                    treeSetHosts();//Call traverse on setHost method
                }
                catch( CustomException& e )
                    {
                        Msg::error("FATAL ERROR during "+controlSetHosts.name+ "- Exit\n\tError is:",__FUNCTION_NAME__);
                        Msg::errorException(e);
                        return -5;
                    }
                
            }
        while(treeExecute.isModified || treeSetHosts.isModified );
        
        
        // if error in execute => create a csv file with actual application state
        if (!treeExecute.listError.empty())
        {
            spreadsheetout.importFromApp(app);
            spreadsheetout.exportToFile(CsvFile(csvout));
            if (foundArchFile)
                spreadsheetout.checkArch(&arch); /// check  spreadsheet content  read from application against architecture description
            spreadsheeterr.importFromApp(treeExecute.toDoComponent);
            spreadsheeterr.exportToFile(CsvFile(csverr));

            std::cerr << "TRAVERSE ERRORS ("<<controlExecute.name<<"):  reached fixed point with the following errors:"<<std::endl<<std::endl;
            for (std::list<CustomException>::const_iterator it = treeExecute.listError.begin(); it != treeExecute.listError.end(); ++it)
            {
                Msg::errorException(*it);
            }

            Msg::error("Execute stage FAILED.", __FUNCTION_NAME__);

            pipelineFailed |= EXECUTE; ///  set execute bit to  true
        }

        // if error in sethosts => create a csv file with actual application state
        if (!treeSetHosts.listError.empty())
            {
                std::cerr << "TRAVERSE ERRORS ("<<controlSetHosts.name<<"):  reached fixed point with the following errors:"<<std::endl<<std::endl;
                for (std::list<CustomException>::const_iterator it = treeSetHosts.listError.begin(); it != treeSetHosts.listError.end(); ++it)
                    {
                        Msg::errorException(*it);
                    }
                spreadsheetout.importFromApp(app);
                spreadsheetout.exportToFile(CsvFile(csvout));
                if (foundArchFile)
                    spreadsheetout.checkArch(&arch); /// check  spreadsheet content  read from application against architecture description
                spreadsheeterr.importFromApp(treeExecute.toDoComponent);
                spreadsheeterr.exportToFile(CsvFile(csverr));
                Msg::error("SetHosts stage FAILED.", __FUNCTION_NAME__);
                pipelineFailed |= SETHOSTS; /// sethosts bit to true
            }


        if (  (pipelineFailed & EXECUTE) == 0 || (pipelineFailed & SETHOSTS) == 0)
                	Msg::debug("Execute and SetHosts stages done.", __FUNCTION_NAME__);

        Msg::debug("resolveMsgTypes stage starting:", __FUNCTION_NAME__);


        /// Set traverse (fixed point mode) for calling the resolveMsgTypes
        Controller<void, void> resolveMsgTypesController(&Component::resolveMsgTypes,"resolveMsgTypes");
        TraversePointFixe treeResolveMsgTypes(app, &resolveMsgTypesController);
        
        try
            {
                treeResolveMsgTypes();//call controller        
            }
        catch( CustomException& e )
            {
            Msg::error("FATAL ERROR during "+resolveMsgTypesController.name+ "- Exit\n\tError is:",__FUNCTION_NAME__);
                Msg::errorException(e);
                return -5;
            }


        // if error in resolveMsgTypes
        if (!treeResolveMsgTypes.listError.empty())
            {
                pipelineFailed |=  RESOLVEMSGTYPES;
                std::cerr << "TRAVERSE ERRORS ("<<resolveMsgTypesController.name<<"):  reached fixed point with the following errors:"<<std::endl<<std::endl;
                for (std::list<CustomException>::const_iterator it = treeResolveMsgTypes.listError.begin(); it != treeResolveMsgTypes.listError.end(); ++it)
                    {
                        Msg::errorException(*it);
                    }
                Msg::error("resolveMsgTypes stage  FAILED", __FUNCTION_NAME__);
            }
        else
        	{
        		Msg::debug("resolveMsgTypes stage done.", __FUNCTION_NAME__);
        	}

        
        Msg::debug("setConnections stage starting:", __FUNCTION_NAME__);
        
        /// Set traverse (one pass) for calling the setConnections
        Controller<void, void> setConnectionsController(&Component::setConnections,"setConnections");
        TraverseOnePass  treeSetConnections(app, &setConnectionsController);

        try
            {
                treeSetConnections();//call setConnection controller        
            }
        catch( CustomException& e )
            {
                Msg::errorException(e);
                Msg::error("setConnections stage FAILED.", __FUNCTION_NAME__);
                pipelineFailed |= SETCONNECTION;
            }
         
        if ( (pipelineFailed & SETCONNECTION ) == 0 )
        	Msg::debug("setConnections stage done.", __FUNCTION_NAME__);
    
    }//end lauchwocompilation

    //create adl.out file (preserve adl.xml from changes)
    // Traverse (one pass)
    TraverseXMLHierarchy tHierarchy(app);
    std::string outAdlFile = prefixFile + ".adl.out.xml";
    DOMDocument* fAdl = new DOMDocument(outAdlFile);
    if(!launchwocompilation)
    {
        // Generate the adl.xmlfile
        DOMNode* eAdl = tHierarchy();
        fAdl->LinkEndChild(eAdl);
        fAdl->SaveFile();
    }

    if(!trace)
      delete fAdl;


    // Set Traverse (one pass) to call the XMLBuild method (to build the .net.xml file)
    Controller<DOMElement*, void> controlNet(&Component::XMLBuild,"XMLBuild");
    Traverse<DOMElement*, void> treeNet;
    treeNet.ignore = true;// Don't throw exceptions, but rather directly print warning messages (.net.xml file generated but can be inconsistent)

    std::string netFile = prefixFile+".net.xml";
    DOMDocument* fNet = new DOMDocument(netFile);
    if(!launchwocompilation)
    {


        Msg::debug("XMLBuild stage starting:", __FUNCTION_NAME__);

        // generate the net.xml
        try
        {
            DOMNode* eNet = treeNet(app, &controlNet).result; /// Call traverse on XMLBuild
            eNet->SetValue("network");

            fNet->LinkEndChild(eNet);
            fNet->SaveFile();
        }
        catch (CustomException& e)
        {
            // Don't expect to catch anything as the ignore option is on 
            Msg::errorException(e);
            Msg::error("XMLBuild  stage FAILED.", __FUNCTION_NAME__);
            pipelineFailed |= XMLBUILD; /// bit xmlbuild set to true

        }

        if (treeNet.ignore && treeNet.erroroccured)
            {
                Msg::error("XMLBuild stage: "+netFile+" generated but content may be incomplete as errors occurred", __FUNCTION_NAME__);
                pipelineFailed |= XMLBUILD; /// bit xmlbuild set to true
            }
        

        if ( (pipelineFailed & XMLBUILD) == 0)
        	Msg::debug("XMLBuild stage done.", __FUNCTION_NAME__);
    }



    /// Set traverse (one pass) to call the XMLrunBuild method
    Controller<DOMElement*, void> controlRun(&Component::XMLRunBuild,"XMLRunBuild");
    Traverse<DOMElement*, void> treeRun;
    DOMNode* eRun;

    if(!launchwocompilation)
    {
    	 Msg::debug("RunBuild stage starting:", __FUNCTION_NAME__);

        // Compile Run.xml
        try
        {
            eRun = treeRun(app, &controlRun).result; /// Call the traverse on XMLRunBuild method
        }
        catch(const CustomException& e)
        {
            Msg::errorException(e);
            Msg::error("RunBuild  stage FAILED.", __FUNCTION_NAME__);
            pipelineFailed |= RUNBUILD; /// runbuild bit set to true
            return -1;
        }
        eRun->SetValue("commands");
    }

    std::string runFile = prefixFile+".run.xml";
    DOMDocument* fRun = new  DOMDocument(runFile);
    if(!launchwocompilation)
        {
            fRun->LinkEndChild(eRun);
            fRun->SaveFile();
        }
    else
        {
            // Otherwise Process only the run.xml
            if(!fRun->LoadFile())
                {
                    Msg::error(prefixFile+".run.xml cannot be loaded. Please restart flowvr without the -X option", __FUNCTION_NAME__);
                    return 1;
                }
        }

    if ( (pipelineFailed & RUNBUILD) == 0)
    	Msg::debug("RunBuild stage done.", __FUNCTION_NAME__);


    // Write actual application host mapping state to spreadsheet for control
    spreadsheetout.importFromApp(app);
    spreadsheetout.exportToFile(CsvFile(csvout));
    if (foundArchFile)
        spreadsheetout.checkArch(&arch);


    if (pipelineFailed != 0)
    {
        std::string summary("----------------------------------------------------------------\n");
        summary += ("\tSummary\n");
        summary += ("\t----------------------------------------------------------------\n");
        if ( (pipelineFailed & EXECUTE) != 0 )
        {
            summary += ("\tExecute Stage : FAILED\n");
        }
        else
        {
            summary += ("\tExecute Stage : OK\n");
        }
        if ( (pipelineFailed & SETHOSTS) != 0 )
        {
            summary += ("\tSetHosts Stage : FAILED\n");
        }
        else
        {
            summary += ("\tSetHosts Stage : OK\n");
        }

        if ( (pipelineFailed & RESOLVEMSGTYPES) != 0 )
        {
            summary += ("\tresolveMsgTypes Stage : FAILED\n");
        }
        else
        {
            summary += ("\tresolveMsgTypes Stage : OK\n");
        }
        if ( (pipelineFailed & SETCONNECTION) != 0 )
        {
            summary += ("\tsetConnections Stage : FAILED\n");
        }
        else
        {
            summary += ("\tSetConnections Stage : OK\n");
        }

      
        if (  (pipelineFailed & XMLBUILD) != 0 )
        {
            if (treeNet.ignore && treeNet.erroroccured)
                summary += ("\tXMLBuild Stage : FAILED  but generated  "+netFile+" anyway \n");
            else
                summary += ("\tXMLBuild Stage : FAILED\n");
        }
        else
        {
            summary += ("\tXMLBuild Stage : OK\n");
        }
    if (  (pipelineFailed & RUNBUILD) != 0 )
        {
            summary += ("\tRunBuild Stage : FAILED\n");
        }
        else
        {
            summary += ("\tRunBuild Stage : OK\n");
        }

        summary += ("\t----------------------------------------------------------------\n");
        summary += ("\tOutput files ares:\n");
        summary += ("\t\t"+outAdlFile+"\n");
        summary += ("\t\t"+netFile+"\n");
        summary += ("\t\t"+csvout+"\n");
        summary += ("\t\t"+csverr+"\n");
        summary += ("\t\t"+runFile+"\n");
        summary += ("\t----------------------------------------------------------------\n");
        Msg::error(summary, __FUNCTION_NAME__);

        delete app;
        return 1;
    }


   
    //  If trace capture activate change the network accordingly
    if(trace)
	{
    	app->addParameter<std::string>("TraceResultPrefix", "/tmp/");

		/* inspect app to add objects needed to trace application here*/
		appTracer::applicationTracer* tracer = new appTracer::applicationTracer(prefixFile);

		std::list<std::string> MaListe;
		int inspectAppResult = tracer->inspectApp(app, MaListe);

		if(app->getParameter<std::string>("TraceResultPrefix") == "/tmp/")
			Msg::debug("Default prefix used for trace files : '/tmp/'. If you want to change it, override [appName]:TraceResultPrefix parameter with -P option", __FUNCTION_NAME__);
		if(tracer->addLoggersRunXML(*fRun, MaListe, app->getParameter<std::string>("TraceResultPrefix")))
			Msg::error("Cannot add loggers in run file", __FUNCTION_NAME__);

		if(tracer->addLoggersNetXML(*fNet, MaListe))
			Msg::error("Cannot add loggers in net file", __FUNCTION_NAME__);

		if(tracer->addLoggersADLXml(*fAdl, MaListe))
			Msg::error("Cannot add loggers in adl file", __FUNCTION_NAME__);

		tracer->createIntermediateFiles(netFile, app->getParameter<std::string>("TraceResultPrefix"));

		Msg::info("Ready to trace application : use 'trace' and 'notrace' commands in flowvr console", __FUNCTION_NAME__);
		delete fAdl;
    }
    // Final output
    Msg::info("Successful application processing.\n\tResults written to "+runFile+", "+netFile+", "+outAdlFile+", "+csvout,__FUNCTION_NAME__);

    if (netStop)
    {
        delete app;
        delete fNet;
        delete fRun;
        return 1;
    }

    if( !cmdObject and !cmdRoute and !cmdStart )
    {
        cmdObject = true;
        cmdRoute = true;
        cmdStart = true;
    }

    flowvr::parse::PassOptions(cmdObject,cmdRoute,cmdStart);
    std::string cmdFile = prefixFile+".cmd.xml";
    DOMDocument* fCmd = new DOMDocument(cmdFile);
    if(!launchwocompilation)
    {
        // Generate Cmd
        flowvr::parse::ParserCommands(fNet,fCmd);
        fCmd->SaveFile();
    }
    else
    {
        // Otherwise process cmd.xml file only
        if(!fCmd->LoadFile())
        {
            Msg::error(prefixFile+".cmd.xml cannot be loaded. Please restart flowvr without the -X option", __FUNCTION_NAME__);
            return 1;
        }
    }

    // Final output
    Msg::info("Successful application processing.\n\tResults written to "+runFile+", "+netFile+", "+outAdlFile+", "+csvout,__FUNCTION_NAME__);
    Msg::info("Successful generation of command file for daemon.\n\tResult written to "+cmdFile,__FUNCTION_NAME__);
    if (!launch && !launchwocompilation)
    {
        delete app;
        delete fNet;
        delete fCmd;
        delete fRun;
        return 0;
    }

    Msg::info("Launching application",__FUNCTION_NAME__);
    BasicController myControl;
    if (!myControl.init(prefixFile))
    {
        Msg::error("Cannot launch the application \n\tMake sure a flowvr daemon is  running \n\tLaunching aborted",__FUNCTION_NAME__);
        delete app;
        delete fNet;
        delete fCmd;
        delete fRun;
        return 1;
    }

    std::string parent = myControl.getParent();

    Msg::info("controller namespace: "+parent,__FUNCTION_NAME__);
    if (setenv("FLOWVR_PARENT", parent.c_str(), 1))
    {
        Msg::error("setting FLOWVR_PARENT environment variable failed.",__FUNCTION_NAME__);
    }

    {
        char buffer[1024];
        if (getcwd(buffer, sizeof(buffer))==NULL)
        {
            Msg::error("getcwd failed.",__FUNCTION_NAME__);
        }
        else if (setenv("FLOWVR_PWD",buffer,1))
        {
            Msg::error("setting FLOWVR_PWD environment variable failed.",__FUNCTION_NAME__);
        }
    }

    myControl.start();


    // register all modules that are expected to connect (to sync the .run.xml with the .cmd.xml)
    {
      std::set<std::string> & allExpectedModules = myControl.allExpectedModules; 
                  
      for(DOMElement* cmd = fCmd->RootElement()->FirstChildElement(); 
          cmd; 
          cmd = cmd->NextSiblingElement()) {
        TiXmlElement *elt = cmd->ToElement();
        if(strcmp(cmd->Value(),            "addobject"               ) == 0 && elt && 
           strcmp(elt->Attribute("class"), "flowvr.plugins.Regulator") == 0) {
          allExpectedModules.insert(cmd->Attribute("id"));
        }
      }
    }    

    Msg::info("Processing "+runFile,__FUNCTION_NAME__);
    DOMElement* root = fRun->RootElement();
    DOMElement* cmd = root->FirstChildElement();
    while (cmd!=NULL)
    {
        if (!myControl.ok())
        {
            std::string cmds="";
            Msg::error("Cannot send command: \n\t"+  (cmds<<*cmd)+"\n\tLaunching aborted",__FUNCTION_NAME__);

            delete app;
            delete fNet;
            delete fRun;
            delete fCmd;
            return 1;
        }
        myControl.processCommand(cmd);
        cmd = cmd->NextSiblingElement();
    }

    Msg::info("Processing "+cmdFile,__FUNCTION_NAME__);


    root = fCmd->RootElement();
    cmd = root->FirstChildElement();
    while (cmd!=NULL)
    {
        if (!myControl.ok())
        {
            std::string cmds="";
            Msg::error("Cannot send command: \n\t" + (cmds<<*cmd)+"\n\tLaunching aborted",__FUNCTION_NAME__);

            delete app;
            delete fNet;
            delete fRun;
            delete fCmd;
            return 1;
        }
        myControl.processCommand(cmd);
        cmd = cmd->NextSiblingElement();
    }

    while (myControl.ok())
    {
        std::cout << ">"; //prompt
        std::string command = BasicController::getcmdline();
        if ( !command.empty())  
            myControl.processCommand(command, true);
    }


    myControl.wait();

    std::cout << "Done!" << std::endl;
    delete fNet;
    delete fRun;
    delete fCmd;
    delete app->paramListSetExternal;
    delete app; // need virtual destructor
    return 0;
};


