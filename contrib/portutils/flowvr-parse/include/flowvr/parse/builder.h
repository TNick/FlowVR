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
* File: include/builder.h                                         *
*                                                                 *
* Contacts:                                                       *
*  19/3/2004 Bruno Raffin <bruno.raffin@imag.fr>                  *
*                                                                 *
******************************************************************/
/*! \file builder.h
 *
 * \brief Define lasses for handling  the xml metamodule 
 *  descriptions and  metamodules (including template resolution)
 */

#ifndef FLOWVR_PARSE_BUILDER_H
#define FLOWVR_PARSE_BUILDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sstream> 
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <fcntl.h>


#include <flowvr/xml.h>
#include <flowvr/utils/filepath.h>


using namespace flowvr::xml;

namespace flowvr
{

namespace parse
{

// Class to traverse the  direct children of a given node selecting only the one named tagname
class NamedChildList
{
 public:

/*! \fn     NamedChildList(DOMNode* n, const char * tagname) 
 * \param n: the parent node
 * \param tagname: look for  the children   named tagname.
 */
  
  NamedChildList(DOMNode* n, const char * tagname) 
    : parent(n), name(tagname), length(-1)
  {
  }


  
/*! \fn     DOMNode* getFirst()
 * \brief return the first named children node
 */

  DOMElement* getFirst()
  {
    if (parent == NULL) return NULL;

    DOMNode * n  = parent->getFirstChild();
    while (n!=NULL && !(n->getNodeType() == DOMNode::ELEMENT_NODE && strcmp(name,n->getNodeName()) == 0) )
    {
      n = n->getNextSibling();
    }
    return (DOMElement*)n;
  }

/*! \fn     DOMNode* getNext(DOMNde * n)
 * \brief return the next named children node after n
 * \param n: the current named children node
 */

  DOMElement* getNext(DOMNode * n)
  {
    if (n != NULL)
      {
	do
	  {
	    n = n->getNextSibling();
	  }
	while (n!=NULL && !(n->getNodeType() == DOMNode::ELEMENT_NODE &&  strcmp(name,n->getNodeName()) == 0));
      }
    return (DOMElement*)n;
  }


/*! \fn      int getLength()
 * \brief return the number of named children nodes
 */
  int getLength()
  {

    if (length<0)
    {
      DOMNode *n = getFirst();
      length = 0;

      while (n!=NULL)
      {
	n = getNext(n);
	++length;
      }
    }
    return length;
  }

 protected:
 DOMNode * parent;
 const char *  name;
 int length;

};


/// Class to build and work on a list of DOM Nodes with tag "host"
class HostList
{
 public:

  NamedChildList nodes;

/*! \fn   HostList(DOMNode * n)
 * \brief Constructor return a  list of nodes of tag "host"
 * \param DomNode * n: the host list is build from the children of the node n
 */
  HostList(DOMNode * n) : nodes(n,"host")
  {
  }

  int getLength() { return nodes.getLength(); }

/*! \fn     std::String TexttoString();
 * \brief Return the list of nodes separated by spaces (without XML decorations)
 * \param delim : the string inserted between each host
 */
  std::string TexttoString(const char * delim);

/*! \fn     std::string Nb();
 * \brief Return the number of hosts in a string
 */
  std::string  Nb();

};

/// Class with various methods to resolve the <template>  child nodes 
class BuiltElement
{
public : 

  DOMElement* root;
  BuiltElement(DOMNode* n)
  : root((DOMElement*)n)
  {
  }

  operator DOMElement*() { return root; }

protected :


/*! \fn     void   FindReplaceTplate(DOMElement *n)
 * \brief   Find  all descendent <template>  and, when  possible,  
 *           resolve them with the children  <template> of n 
 * \param n : the element where to look for templates to use  for the  resolution
 */
  void   FindReplaceTplate(DOMElement *n);



/*! \fn  vooid     FindReplaceHost(DOMElement * n)
 * \brief   Find  all <host> descendent and replace them with the first host
 *          from the children of n that match  (same tagname, same attributes 
 *          and attribute values for all defined attributes)
 * \param n : the element where to look for hosts to use  for the  resolution
 */
 void   FindReplaceHost(DOMElement * n);



/*! \fn       void   FindReplaceHostNb(DOMElement *n);
 * \brief   Find  all <nb> descendent  and replace them with the number 
 *          of hosts computed from the <host> children of node n
 * \param n : the element where to look for hosts to use  for the  resolution
 */
 void   FindReplaceHostNb(DOMElement * n);

/*! \fn        void  FindReplaceMachineFile(DOMElement *n);
 * \brief   Find  all <machinefile> descendent and replace
 *            them with the machinefile 
 * \param n : the element where to look for hosts to use  for the  resolution
 * \param mmid : meta module id

 */
 void  FindReplaceMachineFile(DOMElement *n, const std::string& mid );

/*! \fn       void   FindReplaceHostList(DOMElement *n);
 * \brief   Find  all <machinelist> or <hostlist> children and replace
 *            them with the list of hosts  computed from the children <hosts> of n
 * \param n : the element where to look for hosts to use  for the  resolution

 */
 void  FindReplaceHostList(DOMElement *n);


/*! \fn       void  FindReplaceMetaModId(DOMElement *n, const char * mid) 
 * \brief   Find  all <moduleid> children and replace
 *          them with a text node giving the module id
 * \param mid : meta module id
 */
 void  FindReplaceMetaModId(DOMElement * n , const std::string& mid);

 public: 


/*! \fn       void NodeToAttr(const char * tagname, const char * attrname )
 * \brief    Look for a child  with tag name tagname. If more than one available error. 
 *           If not a pure text node error.
 *           Otherwise create a new attribute for the current node with name  attrname 
 *           and value  the text content of this child
 * \param tagname : the child node name to look for 
 * \param attrname : attribute name
 */

 void NodeToAttr(const char * tagname,const char * attrname, const char * def=NULL);



/*! \fn     void Solve(DOMElement  * n, const char * mmid)
 * \brief Solve all templates of the current node using the  templates from  node n 
 * \param DomElement  * n: the node where to find the data to solve the templates
 * \param mmid : meta module id
 */


  void Solve(DOMElement  * n, const std::string& mmid)
  {
    FindReplaceTplate(n);
    FindReplaceHost(n);
    FindReplaceHostNb(n);
    FindReplaceHostList(n);
    FindReplaceMachineFile(n,mmid);
    FindReplaceMetaModId(n,mmid);
  }


/*! \fn     bool SolveDefault()
 * \brief Solve all templates of the current node using the templates default values  
 *        Return flase if one template not solve (template without default value)
 */
  bool SolveDefault();


  /*! \fn   bool   BuiltElement::AsymetricMatch(DOMElement * n)
 * \brief   Test if the current node name and if all its attributes and attribute values 
 *          are present and the same in node n (but n can have extra attributes)   
 * \param n : node to look for.
 */
 bool   AsymetricMatch(DOMElement * n);


  /*! \fn   bool   BuiltElement::Match(DOMElement * n)
 * \brief   Test if the current node and n  match (same tagname, same attributes 
 *          and attribute values for all defined attributes)
 * \param n : node to look for.
 */
 bool   Match(DOMElement * n);



/*! \fn   DOMNode * FindMatchingChild(DOMNode * n, const char * tagName)
 * \brief   Find  the first  child  of n matching the current node (same tagname, same attributes 
 *          and attribute values for all defined attributes), return null if no 
 *          match found
 * \param n : node to look for.
 * \return  a pointer to the matching node or NULL
 */
  DOMNode *  FindMatchingChild(DOMElement * n);

  /*! \fn   DOMNode * FindAsymetricMatchingChild(DOMNode * n, const char * tagName)
 * \brief   Find  the child  of n matching the current node. CHeck for an exact mathc first, 
 *          next for a asymetric match (same tagname, same attributes 
 *          and attribute values for all  attributes of the current node). Return null if no 
 *          match found or if more than one eligible  candidate. 
 * \param n : node to look for.
 * \return  a pointer to the matching node or NULL
 */

  DOMNode *  FindAsymetricMatchingChild(DOMElement * n);

/*! \fn   DOMNode * FindMatchingNode(DOMNode * n)
 * \brief   Find  the first  descendent node    of n (all the sub tree is explored)
 *          matching the current node (same tagname, same attributes 
 *          and attribute values for all defined attributes), return null if no 
 *          match found
 * \param n : parent node wherer to   look for matching descendent
 * \return  a pointer to the matching node or NULL
 */

  DOMNode * FindMatchingNode(DOMNode * n);



};




/// the Class MetaModule: data  and mehods to compute the run commands and metamodule lists.
class MetaModule
{
 protected:
  DOMElement  * metaModDesc; //< pointer to a <metamoduledescription>
  DOMElement  * metaMod; //< pointer to a <metamodule>
  DOMElement  * run;//< run to  build from the  <metamodule> and <metamoduledescription>
  DOMElement  * moduleList; //< module list to  build from the  <metamodule> and <metamoduledescription>
  std::string  metaModId; //< the meta  module id 
  std::string  metaModDescFile; //< the file name that contain the  meta module description
 public: 

/*! \fn     MetaModule(DOMNode * m)
 * \brief   Constructor that set the metamodule pointeur 
 */
  MetaModule(DOMElement * m): metaMod(m){}

/*! \fn       void setMetaMod(DOMElement * m)
 */

  void setMetaMod(DOMElement * m)
    {
      metaMod = m;
    };

/*! \fn       DOMElement * getMetaMod()
 */
  DOMElement * getMetaMod()
    {
      return metaMod; 
    };



/*! \fn       void setMetaModDesc(DOMElement * m)
 */

  void setMetaModDesc(DOMElement * m)
    {
      metaModDesc = m;
    };

/*! \fn       DOMElement * getMetaModDesc()
 */
  DOMElement * getMetaModDesc()
    {
      return metaModDesc; 
    };

/*! \fn         void setRun(DOMElement * r)
 */
  void setRun(DOMElement * r)
    {
      run = r;
    };

/*! \fn         DOMElement *  getRun()
 */

  DOMElement *  getRun()
    {
      return run;
    };


/*! \fn  void setModuleList(DOMElement * m)
 */

  void setModuleList(DOMElement * m)
    {
      moduleList = m;
    };

/*! \fn  void getModuleList()
 */
  DOMElement *  getModuleList()
    {
      return  moduleList;
    };

/*! \fn   const std::string&  getMetaModId()
 */
  const std::string&  getMetaModId()
    {
      return  metaModId;
    };

/*! \fn  std::string& setMetaModDescFile()
 */
  void  setMetaModDescFile(const std::string& f)
    {
      metaModDescFile = f;
    };


/*! \fn  std::string& getMetaModDescFile()
 */
  std::string&  getMetaModDescFile()
    {
      return  metaModDescFile;
    };


/*! \fn      void BuildMetaModuleId()
 * \brief Compute the module id 
 */
  void BuildMetaModuleId()
    {

      if (metaMod == NULL)
	{
	  std::cerr << "Fatal Error : no meta module  found " << std::endl;
	  exit(1);
	}

      DOMAttr * at = metaMod->getAttributeNode("id");
      if ( at == NULL)
	{
	  std::cerr << "Fatal Error : no id attribute found for" << metaMod << std::endl;
	  exit(1);
	}
      metaModId = at->getNodeValue();
    };





/*! \fn    DOMElement *  Build(const char * tagName)
 * \brief Select the child Element tagName from the metamodule, 
 *        Make a copy.
 * \param tagName: Tag name ("run" or "modulelist")
 */
  DOMElement *  Build(const char * tagName);

/*! \fn    void Solve(DOMElement * n, const char * tagName)
 * \brief Solve the templates using the metamoduledescription   data
 * \param tagName: Tag name ("run" or "modulelist")
 */
  void  Solve(DOMElement * n, const char * tagName);

/*! \fn      void BuildRun()
 * \brief Compute the run command 
 */
  void BuildRun();

/*! \fn      void BuildModuleList()
 * \brief Compute the module list
 */
  void BuildModuleList();

  
/*! \fn       void PrintRun(std::ostream & o)
 * \brief Print the run command to the out stream o 
 */
 void PrintRun(std::ostream * o)
    {

      if (run == NULL ) return;

      // Suppress all attributes "id" of the run tag 
      for(DOMAttr * at = run->FirstAttribute(); at;)
	{
	  DOMAttr * tmp = at;
	  at= at->Next();// move next before to remove the node
	  if (tmp->getNodeName() == "id")
	    {
	      run->RemoveAttribute(tmp->getNodeName());
	    }
	}


      // Insert  attribute "metamoduleid" to the  run tag 
      run->SetAttribute("metamoduleid",metaModId);


      // ask for text content to have entities written in a readable manner ("<" and not "&lt;")
      //(* o)<<run->getTextContent()<<std::endl;
      (* o)<<*run<<std::endl;
      
    }


/*! \fn       void PrintModuleList(std::ostream & o)
 * \brief Print the module list to  the out stream o 
 */
  void PrintModuleList(std::ostream * o)
    {
      
      if (moduleList == NULL ) return;
      
      TiXmlNamedTagList* mList = new TiXmlNamedTagList(moduleList,"module");

      for (DOMNode * m =  mList->getFirst(); m ; m = mList->getNext(m))
	{
	  (* o)<<*m<<std::endl;
	}
    }
};



/// The Class MetaModuleListBuilder compute all run commands and module lists from a  given application graph 
class MetaModuleListBuilder
{
protected: 
  flowvr::utils::FilePath dirMetaModDesc; // the directory list to use as prefix for metamod desc file search
  DOMDocument * applGraphDoc; // The application graph document
  std::vector<DOMDocument *> metaModDescDocList; // The list of metamoduledesc files loaded
  std::vector<MetaModule *> metaModList; // The  metamodules to treat



/*! \fn   DOMDocument * LoadFile(const  char * file)
 * \brief Load  and parse file. exit if problem
 * \param file: a  file name
 * \return: a pointer to a DOMDocument. Exit on error.
 */

  DOMDocument * LoadFile(const std::string& file);


/*! \fn   bool CheckFile(const  char * file)
 * \brief Check for a file existence 
 * \param file: a  file name
 */

  bool  CheckFile(const std::string& file)
    {

      if (file.empty()) return false;

      // Could have been done with something like statfs, 
      //but it is less portable 
      
      std::ifstream f(file.c_str());
      if (f.is_open())
	{
	  f.close();
	  return true;
	}
      return false;
    }

/*! \fn   const char * FindFile(const char *  file)
 * \brief Find where  file is located using dirMetaModDesc 
 * \param file: a  file name
 * \return dir+file if found NULL otherwise
 */

  const std::string FindFile(const std::string& file);
  
public:
  

/*! \fn   MetaModuleListBuilder(const char *  agf,  std::vector<char *> f)
 * \brief  Load the application graph file and the list of metamoduledesc files f
 * \param agf: application graph file name
 * \param f: list of metamoduledesc file names
 * \param dir: directory list (dir1:dri2:...) whee  metamoduledesc file can be found
 */
  MetaModuleListBuilder(const std::string& agf,  std::vector<const char *> f,const flowvr::utils::FilePath& dir):dirMetaModDesc(dir)
    {

      applGraphDoc = LoadFile(agf);


      for (std::vector<const char *>::iterator fi = f.begin(); fi != f.end(); ++fi) 
	{
	  metaModDescDocList.push_back(LoadFile(*fi));
	}
    }



/*! \fn     ~MetaModuleListBuilder()
 */
  ~MetaModuleListBuilder()
    {
      for (std::vector<MetaModule *>::iterator mi = metaModList.begin(); mi != metaModList.end(); ++mi) 
	{
	  delete(*mi);
	}
      for (std::vector<DOMDocument *>::iterator di = metaModDescDocList.begin(); di != metaModDescDocList.end(); ++di) 
	{
	  delete(*di);
	}
    }





/*! \fn     void   Init()
 * \brief  Find a matching metamoduledescription  for each metamodule from the application graph 
 */
  void   Init();


/*! \fn    bool FindMatchingMetaModDesc(MetaModule * mm)
 * \brief Find for each metamodule the matching metamoduledescription using first
 *        the metaModDocList (files loaded from the command line), next 
 *        loading the file name given in the formal metamoduledecsription list of the metamodule
 * \param mm: the MetaModule  we want to find a matching metamoduledescirption for 
 */
  bool  FindMatchingMetaModDesc(MetaModule * mm);


/*! \fn    DOMNode * FindInMetaModDocList(DOMNode * mm)
 * \brief Find for each metamodule the matching metamoduledescription looking 
 *        in the metaModDocList (files loaded from the command line)
 * \param mm: the MetaModule  we want to find a matching metamoduledescirption for 
 */
  DOMNode * FindInMetaModDocList(DOMNode * mm);


/*! \fn    DOMNode * MetaModuleListBuilder::FindInMetaModDescFile(DOMNode * mm)
 * \brief Find for each metamodule the matching metamoduledescription 
 *        loading  the file name given in the formal metamoduledecsription list of the metamodul
 * \param mm: the MetaModule  we want to find a matching metamoduledescirption for 
 */
  DOMNode * FindInMetaModDescFile(DOMNode * mm);




/*! \fn     void   BuildRun()
 * \brief  Build the run command for each metamodule
 */
  void BuildRun()
    {
      for (std::vector<MetaModule *>::iterator mi = metaModList.begin(); mi != metaModList.end(); ++mi) 
	{
	  (*mi)->BuildRun();
	}
    }


/*! \fn     void   BuildModuleList()
 * \brief  Build the module list or each metamodule
 */
  void BuildModuleList()
    {
      for (std::vector<MetaModule *>::iterator mi = metaModList.begin(); mi != metaModList.end(); ++mi) 
	{
	  (*mi)->BuildModuleList();
	}
    }


/*! \fn       void PrintRun(std::ostream & o)
 * \brief Print the run command to the out stream o 
 */
  void PrintRun(std::ostream * o)
    {
      
      for (std::vector<MetaModule *>::iterator mi = metaModList.begin(); mi != metaModList.end(); ++mi) 
	{
	  (*mi)->PrintRun(o);
	}

    }
  

/*! \fn     void   PrintModHeader(std::ostream * o,const char * dtd)
 * \brief  Print header for the module list output
 */
  void   PrintModHeader(std::ostream * o, const char * dtd)
  {
    std::string add = "http://flowvr.sf.net/flowvr-parse/dtd/network.dtd";
    if ( strlen(dtd) > 0) add =dtd;
    *o <<"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"<<std::endl;
    *o <<"<!DOCTYPE network SYSTEM \""<<add<<"\">"<<std::endl;
    *o <<"<network>"<<std::endl;
  }


/*! \fn     void   PrintModFooter(std::ostream * o)
 * \brief  Print header for the module list output
 */
  void   PrintModFooter(std::ostream * o)
  {
    *o <<"</network>"<<std::endl;
  }


/*! \fn     void   PrintRunHeader(std::ostream * o,const char * dtd)
 * \brief  Print header for the module list output
 */
  void   PrintRunHeader(std::ostream * o, const char * dtd)
  {
    std::string add = "http://flowvr.sf.net/flowvr-parse/dtd/commands.dtd";
    if ( strlen(dtd) > 0) add =dtd;
    *o <<"<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>"<<std::endl;
    *o <<"<!DOCTYPE commands SYSTEM \""<<add<<"\">"<<std::endl;
    *o <<"<commands>"<<std::endl;
  }


/*! \fn     void   PrintRunFooter(std::ostream * o)
 * \brief  Print header for the module list output
 */
  void   PrintRunFooter(std::ostream * o)
  {
    *o <<"</commands>"<<std::endl;
  }



/*! \fn     void   PrintModuleList()
 * \brief  Print a module list  for each  each metamodule
 */
  void PrintModuleList(std::ostream * o)
    {

      for (std::vector<MetaModule *>::iterator mi = metaModList.begin(); mi != metaModList.end(); ++mi) 
	{
	  (*mi)->PrintModuleList(o);
	}
    }
};

};}; // end namespace
#endif
