

#include<list>
#include<string>
#include<cstring>
#include<iostream>
#include <libxml/tree.h>      //Works provided you compile with -I path/to/libxml2_source_directory
#include <libxml/parser.h>
#include<vector>
#include<map>
#include<deque>
#include<sstream>
#include<typeinfo>

namespace xmlParam
{

// must declare type Xmlizable (used by function find_address)
struct Xmlizable;

#include "xmlobject_base.h"

xmlObject_base::xmlObject_base() : fatherSpace(0), wrapped_noType(0), regOrder(0)
{}

xmlObject_base::xmlObject_base(std::string name):  xmlName(name) , fatherSpace(0), wrapped_noType(0), regOrder(0)
{}


// Destructor (really important)
xmlObject_base::~xmlObject_base()
{
  // Destroy parameter registeries
  for(TypeMap::iterator it = my_paramHandlers.begin() ; it!=my_paramHandlers.end() ; ++it)
    delete (*it).second;
  // Destroy my entry in the reverseMap
  if(wrapped_noType)
  {
    std::map<void *,xmlObject_base *>::iterator iter = xmlObject_base::reverseMap.find( wrapped_noType );
    if(iter!=xmlObject_base::reverseMap.end())
      xmlObject_base::reverseMap.erase(iter);
  }
  // Destroy children xmlObjects
  while (childSpace.size())
  {
    delete childSpace.front();
    childSpace.pop_front();
  }
  // We *do not* delete the entry in lastNames !
  // lastNames just grows and grows until we call delete_structure().
}


void xmlObject_base::add_childSpace( xmlObject_base * newChild )
{
  childSpace.push_back(newChild);
  newChild->fatherSpace=this;
}


//Non-virtual saving/loading functions :

bool xmlObject_base::load_XML( xmlNodePtr father_node, bool warn )
{
  bool res=false; // To check for unused stuff.
  
  // Basic functionality
  std::list<xmlNodePtr> relevEl = get_load_List ( father_node, false );
  if ( relevEl.size() )
    res=load_Element(relevEl.front(), warn);
  
  // If the object is a pointer or container (meaning regOrder>0), we also
  // perform an "xmlObject_base"-type load_XML, just in case other childSpaces, 
  // or parameters, have also been attached to the object (odd, but who knows!?)
  // (Note: important to do it *after*, to avoid erroneous warnings)
  if( regOrder>0 )
  {
    relevEl = xmlObject_base::get_load_List ( father_node, false );
    if ( relevEl.size() )
      res=res | xmlObject_base::load_Element(relevEl.front(), warn);
  }
  
  return res;
}


void xmlObject_base::save_XML(xmlNodePtr father_node)
{
  xmlNodePtr saveHere=make_child_Element( father_node );
  save_Element( saveHere );
  if( regOrder>0 ) // specialization (see previous comments in load_XML)
    xmlObject_base::save_Element( saveHere );
}



//The sub-routines (three of whom are virtual functions)


xmlNodePtr xmlObject_base::make_child_Element (xmlNodePtr father_node)
{
  xmlNodePtr res=father_node;
  if( xmlName !="" )
    res=xmlNewChild(father_node,NULL,(const xmlChar*)xmlName.c_str(),NULL);
  return res;
}


void xmlObject_base::save_Element(xmlNodePtr the_element)
{
  //Saving all registered parameters
  for(TypeMap::iterator it = my_paramHandlers.begin() ; it!=my_paramHandlers.end() ; ++it)
    (*it).second->save_Attributes(the_element);

  //Saving all children xmlObject_base:
  for(uint i=0;i<childSpace.size();++i)
  {
    childSpace.front()->save_XML(the_element);
    childSpace.push_back(childSpace.front());
    childSpace.pop_front();
  }
}

// if allow_multiple=false, print an warning if the returned list
// has multiple elements. (allow_multiple=true only for containers
// which can handle a succession of loadable elements).
std::list<xmlNodePtr> xmlObject_base::get_load_List (xmlNodePtr father_node, bool allow_multiple )
{
  std::list<xmlNodePtr> res;
  if( xmlName !="") 
    res= get_Element_children(father_node, xmlName);
  else
    res.push_back(father_node);
    
  if( !allow_multiple )
  {
    if( res.size()>1 )
    std::cerr<<"WARNING in function xmlObject_base::get_load_List: node <"
             <<father_node->name<<"> has more than one child element called <"
             <<xmlName<<">. Only the first node will be taken into account."
             <<std::endl;
    while(res.size()>1)
      res.pop_back();
  }
  return res;
}


bool xmlObject_base::load_Element(xmlNodePtr the_element, bool warn)
{
  //Mark this element as used
  the_element->_private=(void*)Used;
  
  //Loading all registered parameters
  for(TypeMap::iterator it = my_paramHandlers.begin() ; it!=my_paramHandlers.end() ; ++it)
    (*it).second->load_Attributes(the_element);

  // First possible initialization of the created element (using InitFunctors)
  init_Element_before();
  
  //Recursively loading children xmlObject_base objects
  bool childWarning=false;
  for(uint i=0;i<childSpace.size();++i)
  {
    childWarning=childWarning | 
      childSpace.front()->load_XML(the_element,warn);
    childSpace.push_back(childSpace.front());
    childSpace.pop_front();
  }
  
  // Second possible initialization of the created element (using InitFunctors)
  init_Element_after();
  
  // Produce (and print) warning if useless remaining attributes or children elements.
  bool NotUsedWarning=check_NotUsed(the_element,warn) ;
  return  NotUsedWarning | childWarning ;
}


// The 'unused' warning function
bool xmlObject_base::check_NotUsed(xmlNodePtr the_element, bool warn)
{
  bool attrWarning=false, elemWarning=false;
  // Only warn if xmlName!="" (else, let my parent do it).
  if ( xmlName!="" )
  {
    // check remaining attributes
    for ( xmlAttrPtr att=the_element->properties; att!=NULL; att=att->next )
    {
      if( att->_private==(void*)NotUsed )
      {
        if(warn && !attrWarning)
          std::cerr<<"WARNING in xmlObject_base::load_Element() at node  <"
                   <<xmlName<<">. You are using the following USELESS attributes:"
                   <<std::endl;
        attrWarning=true;
        if(warn)
        {
          xmlChar* value = xmlNodeListGetString(att->doc, att->children, 1);
          std::cerr<<att->name<<"=\""<<value<<"\""<<std::endl;
          xmlFree(value);
        }
      }
    }
    // check remaining children elements
    std::list<xmlNodePtr> children=get_Element_children(the_element);
    while( children.size() )
    {
      if( children.front()->_private==(void*)NotUsed )
      {
        if(warn && !elemWarning)
          std::cerr<<"WARNING in xmlObject_base::load_Element() at node  <"
                   <<xmlName<<">. You are using the following USELESS child elements:"
                   <<std::endl;
        elemWarning=true;
        if(warn)
          std::cerr<<"<"<<children.front()->name<<"/>"<<std::endl;
      }
      children.pop_front();
    }
  }
  
  return attrWarning | elemWarning;
}


// ****************** STATIC FUNCTIONS ************************


//  ADDITIONAL libxml2-related helper functions.

std::list<xmlNodePtr> xmlObject_base::get_Element_children(const xmlNodePtr father_node, std::string imposedName)
{
  const xmlChar *cname=(const xmlChar*)imposedName.c_str();
  std::list<xmlNodePtr> res;
  for (xmlNodePtr cur=father_node->children; cur!=NULL; cur=cur->next)
    if ( cur->type==XML_ELEMENT_NODE && ( !xmlStrcmp(cur->name,cname)||imposedName=="" ))
        res.push_back(cur);
  return res;
}

// Recursively set all elements and attributes inside the tree of 'the_node'
// to _private=xmlObject_base::NotUsed
void xmlObject_base::set_NotUsed(xmlNodePtr the_node)
{
  the_node->_private=(bool*)NotUsed;
  
  for(xmlAttrPtr att=the_node->properties; att!=NULL; att=att->next)
    att->_private=(bool*)NotUsed;
  
  std::list<xmlNodePtr> children=get_Element_children(the_node);
  while( children.size() )
  {
    set_NotUsed( children.front() );
    children.pop_front();
  }  
}

// Template specialization of set_Element_attribute for TYPE=string
template<>
void xmlObject_base::set_Element_attribute(std::string *typ, std::istringstream &ist)
{
  *typ=ist.str();
}

// Template specialization of set_Element_attribute for TYPE=char*
// WARNING : *typ is neither freed nor allocated : just overwritten.
template<>
void xmlObject_base::set_Element_attribute(char* *typ, std::istringstream &ist)
{
  strcpy(*typ,ist.str().c_str());
}
  
//...and of course the static variables initialization:

std::list<xmlObject_base*> xmlObject_base::static_toDelete = std::list<xmlObject_base*>();

std::map< void * , xmlObject_base *> xmlObject_base::reverseMap = std::map< void * , xmlObject_base *>();

std::map< void * , std::string > xmlObject_base::lastNames = std::map< void * , std::string>();

bool* xmlObject_base::Used=new bool(true);
bool* xmlObject_base::NotUsed=new bool(false);

} //namespace xmlParam


