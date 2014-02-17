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
* File: include/builder.cpp                                       *
*                                                                 *
* Contacts:                                                       *
*  19/3/2004 Bruno Raffin <bruno.raffin@imag.fr>                  *
*                                                                 *
******************************************************************/
/*! \file builder.cpp
 *
 * \brief Define lasses for handling  the xml metamodule 
 *  descriptions and  metamodules (including template resolution)
 */


#include "flowvr/parse/builder.h"

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


namespace flowvr { namespace parse {


using namespace flowvr::xml;

static DOMElement* getFirstNamedChild(DOMNode* n, const char* tagname)
{
  NamedChildList l(n, tagname);
  return (DOMElement*)l.getFirst();
}

static DOMElement* getNamedChildId(DOMNode* n, const char* tagname, const char* id)
{
  NamedChildList l(n, tagname);
  for (DOMElement* c = l.getFirst(); c; c = l.getNext(c))
  {
    if (c->Attribute("id") && !strcmp(id, c->Attribute("id")))
      return (DOMElement*) c;
  }
  return NULL;
}

/*! \fn     std::String TexttoString(const char * delim);
 * \brief Return the list of nodes separated by spaces (without XML decorations)
 *  \param delim : the string inserted between each host name
 */
std::string HostList::TexttoString(const char * delim)
{
  std::string hosts="";
  std::string space="";

  for ( DOMNode * h =  nodes.getFirst(); h ; h = nodes.getNext(h))      
    {
      hosts += space + h->getTextContent();
      space = delim;
    }
  return hosts;
}

/*! \fn     std::string Nb();
 * \brief Return the number of hosts in a string
 */
std::string  HostList::Nb()
{
  std::stringstream ss;
  ss <<nodes.getLength();
  return ss.str();
}


  /*! \fn   bool   BuiltElement::AsymetricMatch(DOMElement * n)
 * \brief   Test if the current node name and if all its attributes and attribute values 
 *          are present and the same in node n (but n can have extra attributes)   
 * \param n : node to look for.
 */
bool   BuiltElement::AsymetricMatch(DOMElement * n)
{

  if ( root == NULL ||  n == NULL) return false;

  
  // tag names do not match
  if ( (root->getNodeName() == NULL && root->getNodeName() == NULL) || strcmp(root->getNodeName(),n->getNodeName()) != 0) return false;

  // Look  if attributes match
  for(DOMAttr * myat = root->FirstAttribute(); myat;  myat= myat->Next())
    {

      DOMAttr * at = n->getAttributeNode(myat->getNodeName());

      if ( at == NULL || strcmp(at->getNodeValue(),myat->getNodeValue()) != 0)
	{
	  // Does not match
	  return false;
	}
    }
  return true;
}

  /*! \fn   bool   BuiltElement::Match(DOMElement * n)
 * \brief   Test if the current node and n  match (same tagname, same attributes 
 *          and attribute values for all defined attributes)
 * \param n : node to look for.
 */
bool   BuiltElement::Match(DOMElement * n)
{

  if ( root == NULL ||  n == NULL) return false;

  
  // tag names do not match
  if ( (root->getNodeName() == NULL && root->getNodeName() == NULL) || strcmp(root->getNodeName(),n->getNodeName()) != 0) return false;


  if ( ! AsymetricMatch(n)) return false;
  if ( ! BuiltElement(n).AsymetricMatch(root)) return false;
  
  return true;
}






/*! \fn     void   FindReplaceTplate(DOMElement *n)
 * \brief   Find  all children <template>  and, when  possible,  
 *           resolve them with the children  <template> of n 
 * \param n : the element where to look for templates to use  for the  resolution
 */
void   BuiltElement::FindReplaceTplate(DOMElement *n)
{
  
  if (root ==NULL || n == NULL) return;

  TiXmlNamedTagList * tplateList  = new TiXmlNamedTagList(root,"template");

  for (DOMNode * tplate =  tplateList->getFirst(); tplate ; )
    {
      DOMNode *  match = BuiltElement(tplate).FindMatchingChild(n);

	  DOMNode * tmp = tplate;
	  tplate = tplateList->getNext(tplate);

      if (match != NULL)
	{
	  std::string deflt= match->getTextContent();
	  if ( deflt.length() > 0)
	    {
	      tmp->getParent()->ReplaceChild(tmp, DOMText(match->getTextContent()));//ReplaceChild only work on children nodes !!
	    }
	}
    }
}



/*! \fn  vooid     FindReplaceHost(DOMElement * n)
 * \brief   Find  all <host> descendent and replace them with the first host
 *          from the children of n that match  (same tagname, same attributes 
 *          and attribute values for all defined attributes)
 * \param n : the element where to look for hosts to use  for the  resolution
 */
void   BuiltElement::FindReplaceHost(DOMElement * n)
{

  if (root == NULL  || n == NULL) return;

  TiXmlNamedTagList* hList = new TiXmlNamedTagList(root,"host");
  
  for (DOMNode * h =  hList->getFirst(); h ;)
    {
      DOMNode *  match = BuiltElement(h).FindMatchingChild(n);
      
      DOMNode * tmp = h;
      h = hList->getNext(h);
      
      if (match != NULL)
	{
	  std::string deflt= match->getTextContent();
	  if ( deflt.length() > 0)
	    {
	      tmp->getParent()->ReplaceChild(tmp, DOMText(match->getTextContent()));//ReplaceChild only work on children nodes !!
	    }
	}
    }
}


/*! \fn       void   FindReplaceHostNb(HostList * hostList);
 * \brief   Find  all <nb> descendent and replace them with the number 
 *          of hosts computed from the <host> children of node n
 * \param n : the element where to look for hosts to use  for the  resolution
 */
void   BuiltElement::FindReplaceHostNb(DOMElement * n)
{
  if (root == NULL  || n == NULL) return;

  DOMNodeList* nbList = root->getElementsByTagName("nb");
  HostList hostList(n);

  if ( hostList.getLength() < 1) return;
  
  for (DOMNode * nb =  nbList->getFirst(); nb ; )
  {
    DOMNode * tmp = nb;
    nb = nbList->getNext(nb);
    tmp->getParent()->ReplaceChild(tmp, DOMText(hostList.Nb()));
  }
  delete nbList;
}

/*! \fn       void FindReplaceHostList(HostList * hostList);
 * \brief   Find  all <machinelist> or <hostlist> children and replace
 *            them with the list of hosts  computed from the children <hosts> of n
 * \param n : the element where to look for hosts to use  for the  resolution
 */
void  BuiltElement::FindReplaceHostList(DOMElement *n) 
{
  char * tplatename[]= {(char*) "hostlist",(char*) "machinelist"};

  if (root == NULL || n == NULL) return;

  HostList hostList(n);
  
  if ( hostList.getLength() < 1) return;

  DOMText  text(hostList.TexttoString(" "));

  for (int i =0; i<2; i++)
  {
    TiXmlNamedTagList hostlistList(root,tplatename[i]);
        
    for (DOMNode * hl =  hostlistList.getFirst(); hl ;)
    {
      DOMNode * tmp = hl;
      hl = hostlistList.getNext(hl);
      tmp->getParent()->ReplaceChild(tmp, text);
    }
  }
}


/*! \fn        void  FindReplaceMachineFile(DOMElement *n, const char * mid);
 * \brief   Find  all <machinefile> children and replace
 *            them with the machinefile 
 * \param n : the element where to look for hosts to use  for the  resolution
 * \param mid : the meta module id 
 */
 void  BuiltElement::FindReplaceMachineFile(DOMElement *n, const std::string& mid)
{
  if (root == NULL || n == NULL) return;

  HostList hostList(n);

  TiXmlNamedTagList idList(root,"machinefile");
  
  if ( idList.getLength() < 1 || hostList.getLength() < 1) return;

  std::string fname(mid);
  fname += ".machine";
  
  std::ofstream f(fname.c_str());  
  if ( !f.is_open())
  {
    std::cerr << "Fatal Error : cannot open "<<fname<< std::endl;
    exit(1);
  }
  f << hostList.TexttoString("\n");
  f.close();
  
  for (DOMNode * id =  idList.getFirst(); id; )
  {
    DOMNode * tmp = id;
    id = idList.getNext(id);
    tmp->getParent()->ReplaceChild(tmp, DOMText(fname));
  }
}


/*! \fn       void  FindReplaceMetaModId(DOMElement *n, const char * mid )
 * \brief   Find  all <moduleid> children and replace
 *          them with a text node giving the module id
 */
void  BuiltElement::FindReplaceMetaModId(DOMElement * n , const std::string& mid )
{
  if (root == NULL || n == NULL) return;

  if ( !mid.empty() )
  {
    DOMNodeList* idList = root->getElementsByTagName("metamoduleid");
    for (DOMNode * id =  idList->getFirst(); id; )
    {
      DOMNode * tmp = id;
      id = idList->getNext(id);
      tmp->getParent()->ReplaceChild(tmp, DOMText(mid));
    }
    delete idList;
  }
}


/*! \fn       void NodeToAttr(const char * tagname, const char * attrname )
 * \brief    Look for a child  with tag name tagname. If more than one available error. 
 *           If not a pure text node error.
 *           Otherwise create a new attribute for the current node with name  attrname 
 *           and value  the text content of root child
 * \param tagname : the child node name to look for 
 * \param attrname : attribute name
 */

void BuiltElement::NodeToAttr(const char * tagname, const char * attrname, const char* def)
{
  if (root == NULL) 
  {
    std::cerr << "Fatal Error : Null node !" << std::endl;
    exit(1);
  }

  TiXmlNamedTagList* nList = new TiXmlNamedTagList(root,tagname);
  
  if( nList->getLength() > 1)
  {
    std::cerr <<"Fatal error: more than one node <"<<tagname<<"> in node "<<*root<< std::endl;
    exit(1);
  }
  
  if( nList->getLength() <  1)
  {
    if (root->Attribute(attrname)) return;
    else if (def != NULL)
    {
      root->SetAttribute(attrname,def);
    }
    else
    {
      std::cerr <<"Fatal error: no  node <"<<tagname<<"> in node "<<*root<< std::endl;
      exit(1);
    }
  }
  else
  {
    if (root->Attribute(attrname))
    {
      std::cerr << "Warning: erasing attribute "<<attrname<<" with content of tag "<<tagname<<" in "<<*root<<std::endl;
    }
    root->SetAttribute(attrname,nList->getFirst()->getTextContent());
    root->RemoveChild(nList->getFirst());
  }
}
  
  
  /*! \fn   DOMNode * FindMatchingChild(DOMNode * n, const char * tagName)
 * \brief   Find  the first  child  of n matching the current node (same tagname, same attributes 
 *          and attribute values for all defined attributes), return null if no 
 *          match found
 * \param n : node to look for.
 * \return  a pointer to the matching node or NULL
 */
DOMNode *  BuiltElement::FindMatchingChild(DOMElement * n)
{
  
  if (root == NULL || n == NULL) return NULL;

  NamedChildList * nodeList = new NamedChildList(n,root->getNodeName());
  
  for (DOMNode * node =  nodeList->getFirst(); node ; node = nodeList->getNext(node))
    {
      if (Match((DOMElement *) node)) return node;
    }

  return NULL;
}

  /*! \fn   DOMNode * FindAsymetricMatchingChild(DOMNode * n, const char * tagName)
 * \brief   Find  the child  of n matching the current node. CHeck for an exact mathc first, 
 *          next for a asymetric match (same tagname, same attributes 
 *          and attribute values for all  attributes of the current node). Return null if no 
 *          match found or if more than one eligible  candidate. 
 * \param n : node to look for.
 * \return  a pointer to the matching node or NULL
 */
DOMNode *  BuiltElement::FindAsymetricMatchingChild(DOMElement * n)
{
  
  if (root == NULL || n == NULL) return NULL;

  //Check if we can find an exact match first 
  DOMNode * node = FindMatchingChild(n);
  if (node != NULL) return node;


  NamedChildList * nodeList = new NamedChildList(n,root->getNodeName());

  bool unique = false;
  for (DOMNode * nit=  nodeList->getFirst(); nit ; nit = nodeList->getNext(nit))
    {
      if (AsymetricMatch((DOMElement *) nit)) 
	{
	  if (unique == false) 
	    {
	      node = nit;
	      unique = true;
	      continue;
	    }
	  // we found a second match 
	  if (unique == true) return NULL;
	}
    }
  if (unique) return node;
  return NULL;
}

/*! \fn   DOMNode * FindMatchingNode(DOMNode * n)
 * \brief   Find  the first  descendent node    of n (all the sub tree is explored)
 *          matching the current node (same tagname, same attributes 
 *          and attribute values for all defined attributes), return null if no 
 *          match found
 * \param n : parent node wherer to   look for matching descendent
 * \return  a pointer to the matching node or NULL
 */

DOMNode * BuiltElement::FindMatchingNode(DOMNode * n)
{

  if (n == NULL) return NULL;

  TiXmlNamedTagList* nList = new TiXmlNamedTagList(n,root->getNodeName());  
    
  for (DOMNode * nl =  nList->getFirst(); nl ; nl = nList->getNext(nl))
    {
      if (Match( (DOMElement *) nl))
	{
#ifdef DEBUGPARSE
	  std::cerr <<"Found "<< *nl<< "matching" << *root << std::endl;
#endif 
	  return nl;
	}
    }
  return NULL;
}


/*! \fn     void SolveDefault()
 * \brief Solve all templates of the current node using the templates default values  
 *        Return  false if found one template wihout default value
 */
bool  BuiltElement::SolveDefault()
{

  if (root == NULL) return true;

  char * tplatename[]=  {(char*) "moduleid",(char*) "nb",(char*) "hostlist",(char*) "host",(char*) "machinelist",(char*) "template",(char*) "machinefile"};
  bool ok = true;
  
  for (int i =0; i<7; i++)
    {
      TiXmlNamedTagList* tplateList = new TiXmlNamedTagList(root,tplatename[i]);

      for (DOMNode * l =  tplateList->getFirst(); l ;)
	{
	  DOMNode * tmp =l; 
	  l = tplateList->getNext(l); // we  must move to the next node  before to  deleted  it
	  std::string deflt= tmp->getTextContent();

	  if ( deflt.length() <=0)
	    {
	      std::cerr <<"   Template Not Solved: " <<*tmp <<"in node "<<*tmp->getParent()<< std::endl;
	      ok = false;
	    }
	  else
	    {
	      tmp->getParent()->ReplaceChild(tmp, DOMText(deflt));
	    }
	}
    }
  return (ok ? true : false);
}






/*! \fn    DOMElement *  Build(const char * tagName)
 * \brief Select the child Element tagName from the metamodule, 
 *        Make a copy.
 * \param tagName: Tag name ("run" or "modulelist")
 */
DOMElement *  MetaModule::Build(const char * tagName)
{

  TiXmlNamedTagList* nodeList = new TiXmlNamedTagList(metaMod,tagName);
  if (nodeList->getLength() <= 0)
  {
    std::cerr << "WARNING: no <"<<tagName<<"> found in" << getMetaModId() << std::endl;
    metaMod->LinkEndChild(new DOMElement(tagName));
    delete nodeList;
    nodeList = new TiXmlNamedTagList(metaMod,tagName);
  }
  if (nodeList->getLength() > 1)
  {
    std::cerr << "Fatal Error : more than one <"<<tagName<<"> found in" <<* metaMod << std::endl;
    exit(1);
  }

  DOMNode * mmd = BuiltElement(nodeList->getFirst()).FindAsymetricMatchingChild(metaModDesc);
  if ( mmd == NULL)
  {
    std::cerr << "Fatal Error : no or several candidates  <"<<tagName<<"> found in "<< *metaModDesc  << std::endl;
    exit(1);
  }
  return (DOMElement*)mmd->Clone();
}


/*! \fn    void Solve(DOMElement * n, const char * tagName)
 * \brief Solve the templates using the metamoduledescription   data
 * \param tagName: Tag name ("run" or "modulelist")
 */
void MetaModule::Solve(DOMElement * n, const char * tagName)
{
  TiXmlNamedTagList* nodeList = new TiXmlNamedTagList(metaMod,tagName);
  BuiltElement tmpNode(n);

  // template resolution is done in three steps. 
  // First we try to solve templates using the data included in the current node (run or modulelist node)
  // Next, we solve the remaining templates using the data included in the parent node (metamodule)
  // Next, we solve the remaining templates using their default value when available
  
  tmpNode.Solve((DOMElement *) nodeList->getFirst(),metaModId.c_str());
  tmpNode.Solve(metaMod,metaModId.c_str());
  tmpNode.Solve((DOMElement *) metaMod->getParent(),metaModId.c_str());
  if(!tmpNode.SolveDefault())
  {
    std::cerr << "Fatal Error : Templates not solved (see above)" << std::endl;
    exit(1);
  }

#ifdef DEBUGPARSE
  std::cerr <<"Clone with template solved "<<* tmpNode <<std::endl;
#endif
}


/*! \fn      void BuildRun()
 * \brief Compute the run command 
 */
void MetaModule::BuildRun()
{
  run = Build("run");
  if (run == NULL ) return;

  Solve(run,"run");
  
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

  
  // Identify the directory where the file containing the meta module description is located
  std::string::size_type l = getMetaModDescFile().rfind("/");
  std::string dir =".";
  if (l != std::string::npos){
    dir = getMetaModDescFile().substr(0,l);
  }
  
  //Compute the absolute path
  if ( dir[0] != '/'){
    const char  * pwd = getcwd(NULL,0);
    dir.insert(0,"/");
    dir.insert(0,pwd);
  }
  
  // Insert  attribute "execdir" to the  run tag 
  run->SetAttribute("execdir",dir);
  
  
  // Insert  attribute "metamoduleid" to the  run tag 
  run->SetAttribute("metamoduleid",metaModId);
  
  
}



/*! \fn  void execute(const std::string& command, std::string& result)
 * \brief Execute command and redirect its standard output in result string (append mode)
 * \param command: the command to execute
 * \param file: the file that  that will store the result
 * \param id: the metamodule id to initialize the variable FLOWVR_MODNAME
 * \param shell: the shell that shoudl be used to execute the command
 * \param execdir: change to dir before to execute the command
 */
void execute(const std::string& command, std::string& result, const std::string& id, const std::string& shell,const std::string&  execdir)
{
  int fds[2];
  pid_t   pid;
  if (pipe(fds))
  {
    std::cerr << "pipe failed."<<std::endl;
    return;
  }
  pid = fork ();
  if (pid < 0)
  {
    std::cerr << "fork failed."<<std::endl;
    return;
  }
  else if (pid == 0)
  { // child process
    close(fds[0]);
    if (!id.empty())
      setenv("FLOWVR_MODNAME",id.c_str(),1);
    //// Remove standard input
    //dup2(open("/dev/null",O_RDONLY),0);
    dup2(fds[1],1);
    int retdir = chdir(execdir.c_str());
    if (retdir != 0) std::cerr << "ERROR: chdir("<<execdir<<") returned "<<retdir<<std::endl;
    int retexec = execlp(shell.c_str(),shell.c_str(),"-c",command.c_str(),NULL);
    std::cerr << "ERROR: execlp("<<shell<<" -c "<<command<<") returned "<<retexec<<std::endl;
    exit(1);
  }
  else
  { // parent process
    char buf[256];
    int n;
    close(fds[1]);
    while ((n=read(fds[0], buf, sizeof(buf)))>0)
    {
      result.append(buf,buf+n);
    }
    close(fds[0]);
    waitpid(pid,NULL,0);
  }
};


/*! \fn      void BuildModuleList()
 * \brief Compute the module list
 */
void MetaModule::BuildModuleList()
{
  moduleList = Build("modulelist");

  // First execute the scripts
  {
    // Identify the directory where the file containing the meta module description is located
    std::string::size_type l = getMetaModDescFile().rfind("/");
    std::string dir =".";
    if (l != std::string::npos){
      dir = getMetaModDescFile().substr(0,l);
    }

    TiXmlNamedTagList sList(moduleList,"script");

    for (DOMNode * s =  sList.getFirst(); s ; s = sList.getFirst())
    {
      // Solve local templates
      Solve((DOMElement*)s,"modulelist");
      const char * shell = ((DOMElement *)s)->Attribute("shell");
      if (shell==NULL || shell[0]=='\0') shell="sh";
      std::string result;
      execute(s->getTextContent(),result,getMetaModId(),shell,dir);
      // Parse the result and insert it into moduleList
      flowvr::xml::DOMParser parser;
      if (parser.parseString(result.c_str()))
      {
	std::cerr << "Fatal Error: Script "<<s<<" produced bad XML"<<std::endl;
	exit(1);
      }
      for (DOMNode * c = parser.getDocument()->getFirstChild(); c; c = c->getNextSibling())
      {
	moduleList->InsertBeforeChild(s, *c);
      }
      // Erase the script element
      moduleList->RemoveChild(s);
    }
  }

  // Insert missing tags
  {  
    DOMNodeList* mList = moduleList->getElementsByTagName("module");
    for (DOMNode* m = mList->getFirst(); m; m = mList->getNext(m))
    {
      // Check for missing host
      if (! ((DOMElement*)m)->Attribute("host") && getFirstNamedChild(m,"hostname")==NULL)
      {
	DOMElement* h = new DOMElement("hostname");
	h->LinkEndChild(new DOMElement("hostlist"));
	m->LinkEndChild(h);
      }
      // Check for missing endIt / beginIt
      DOMNode* input = getFirstNamedChild(m,"input");
      if (input == NULL)
      {
	input = new DOMElement("input");
	m->LinkEndChild(input);
      }
      if (! getNamedChildId(input,"port","beginIt"))
      {
	DOMElement* p = new DOMElement("port");
	p->SetAttribute("id","beginIt");
	p->LinkEndChild(new DOMElement("datatype"));
	input->LinkEndChild(p);
      }

      DOMNode* output = getFirstNamedChild(m,"output");
      if (output == NULL)
      {
	output = new DOMElement("output");
	m->LinkEndChild(output);
      }
      if (! getNamedChildId(output,"port","endIt"))
      {
	DOMElement* p = new DOMElement("port");
	p->SetAttribute("id","endIt");
	p->LinkEndChild(new DOMElement("datatype"));
	output->LinkEndChild(p);
      }
      // Check for missing datatype
      DOMNodeList* pList = ((DOMElement*)m)->getElementsByTagName("port");
      for (DOMNode* p = pList->getFirst(); p; p = pList->getNext(p))
      {
	if (! getFirstNamedChild(p, "datatype"))
	{
	  p->LinkEndChild(new DOMElement("datatype"));
	}
      }
      delete pList;
    }
    delete mList;
  }

  // Solve the remaining templates
  Solve(moduleList,"modulelist");

  // Transform special tags into attributes
  {
    TiXmlNamedTagList mList(moduleList,"module");
    for (DOMNode * m =  mList.getFirst(); m ; m = mList.getNext(m))
    {
      BuiltElement(m).NodeToAttr("modulename","id",metaModId.c_str());
      BuiltElement(m).NodeToAttr("hostname","host");
    }
  }
};


/*! \fn   DOMDocument * LoadFile(const  char * file)
 * \brief Load  and parse file. exit if problem
 * \param file: a  file name
 * \return: a pointer to a DOMDocument. Exit on error.
 */

DOMDocument * MetaModuleListBuilder::LoadFile(const std::string& file)
{
  if ( file.empty() ) return NULL;
  
  if ( !CheckFile(file))
  {
    std::cerr << "Fatal Error : cannot load "<<file<< std::endl;
    exit(1);
  }
  
#ifdef DEBUGPARSE
  std::cerr<<"Loading file "<<file<<std::endl;
#endif
  
  flowvr::xml::DOMParser* p = new flowvr::xml::DOMParser();
  if (p->parse(file.c_str()) != 0)
  {
    std::cerr << "Fatal Error : cannot parse "<<file<< std::endl;
    exit(1);
  }
  return p->getDocument();
}



/*! \fn   const char * FindFile(const char *  file)
 * \brief Find where  file is located using dirMetaModDesc 
 * \param file: a  file name
 * \return dir+file if found NULL otherwise
 */

const std::string MetaModuleListBuilder::FindFile(const std::string& file)
    {
      std::string path(file);

      if (dirMetaModDesc.findFile(path)) return path;

      // Try to add a desc directory...
      std::string::size_type pos = file.rfind("/");

      if (pos != std::string::npos)
        path = file.substr(0, pos) + "/desc" + file.substr(pos);
      else
        path = file;
      if (!dirMetaModDesc.findFile(path))
	path.clear();
      return path;
    }

/*! \fn    bool FindMatchingMetaModDesc(MetaModule * mm)
 * \brief Find for each metamodule the matching metamoduledescription  looking first
 *        in the metaModDocList (files loaded from the command line), next 
 *        loading  the file name given in the formal metamoduledecsription list of the metamodul
 * \param mm: the MetaModule  we want to find a matching metamoduledescirption for 
 */

bool MetaModuleListBuilder::FindMatchingMetaModDesc(MetaModule * mm)
{
  TiXmlNamedTagList* mmdList = new TiXmlNamedTagList(mm->getMetaMod(),"metamoduledescription");
      
  if ( mmdList->getLength()  <= 0)
    {
      std::cerr << "Fatal Error : no <metamoduledescription> found in "<< * mm->getMetaMod()  << std::endl;
      exit(1);
	}
  if ( mmdList->getLength()  > 1)
    {
      std::cerr << "Fatal Error : more than one  <metamoduledescription> found in "<< * mm->getMetaMod()  << std::endl;
      exit(1);
    }

  DOMElement * mmd = (DOMElement *) FindInMetaModDocList(mmdList->getFirst());
  if (mmd == NULL) {
    mmd = (DOMElement *) FindInMetaModDescFile(mmdList->getFirst());
  }

  if ( mmd != NULL){ 
    mm->setMetaModDesc(mmd);
    mm->setMetaModDescFile(mmd->GetDocument()->Value());
    return true;
  }

  return false;
}



DOMNode * MetaModuleListBuilder::FindInMetaModDocList(DOMNode * mmd)
{

  if (mmd == NULL ) return NULL;

  // Look inside the meta module description document already loaded
  for (std::vector<DOMDocument *>::iterator di = metaModDescDocList.begin(); di != metaModDescDocList.end(); ++di) 
    { 
      
      DOMNode * tmp = BuiltElement(mmd).FindMatchingNode(*di);
      
      if( tmp != NULL) return tmp;
   }
  return NULL;
}


/*! \fn    DOMNode * MetaModuleListBuilder::FindInMetaModDescFile(DOMNode * mm)
 * \brief Find for each metamodule the matching metamoduledescription 
 *        loading  the file name given in the formal metamoduledecsription list of the metamodul
 * \param mm: the MetaModule  we want to find a matching metamoduledescirption for 
 */

DOMNode * MetaModuleListBuilder::FindInMetaModDescFile(DOMNode * mm)
{

  if (mm == NULL ) return NULL;    

  DOMDocument * d  = LoadFile(FindFile(mm->getTextContent()).c_str());
  if ( d != NULL)
    {
      return BuiltElement(mm).FindMatchingNode(d);
    }
  return NULL;
}


  
/*! \fn     void   Init()
 * \brief  Find a matching metamoduledescription  for each metamodule from the .mml  file 
 */
void   MetaModuleListBuilder::Init()
{
  if (applGraphDoc == NULL)
  {
    std::cerr << "Fatal Error : no .mml  file"  << std::endl;
    exit(1);
  }

  TiXmlNamedTagList mmList(applGraphDoc,"metamodule");  
  // Initialise the list of MetaModule with the metamodule tags founds in the .mml file
  for (DOMNode * mm =  mmList.getFirst(); mm ; mm = mmList.getNext(mm))
  {
    metaModList.push_back(new MetaModule((DOMElement *)mm));
#ifdef DEBUGPARSE
    std::cerr <<"Found metamodule"<< *mm<<std::endl;
#endif
  }
  
  //  Find a matching metamoduledescription  and build the meta module id
  for (std::vector<MetaModule *>::iterator mi = metaModList.begin(); mi != metaModList.end(); ++mi) 
  {
    // build the meta module id
    (*mi)->BuildMetaModuleId();
    if ( !FindMatchingMetaModDesc(* mi)) 
    {
      std::cerr << "Fatal Error : No actual <metamoduledescription> found for "<< *(*mi)->getMetaMod()<< std::endl;
      exit(1);
    }
  }
}

};};
