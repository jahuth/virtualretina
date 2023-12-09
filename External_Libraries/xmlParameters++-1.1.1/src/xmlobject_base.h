#ifndef XML_OBJECT_BASE_H
#define XML_OBJECT_BASE_H

// The base xml-handling object. It will work by 'wrapping' XMLIZED objects, through
// template specializations of the form xmlObject<XMLIZED>

// This object is associated to its own xml node if its xmlName!="". 
// Otherwise it is associated to its father node.

struct xmlObject_base
{

public:

  //***************  NAMES, AND HANDLING OF RECURSIVE INCLUSIONS :

  std::string xmlName;
  // xmlObjects corresponding to *user-defined* (i.e., with functions add_child() or add_child_custom(),
  // but NOT those *created* via POINTERS or CONTAINERS) children objects:
  std::list<xmlObject_base*> childSpace;
  void add_childSpace( xmlObject_base * newChild );
  void* wrapped_noType;	        // (void*) address of the future wrapped object (in xmlObject< whatever > )
  virtual const std::type_info & wrapped_typeid() = 0; // typeid of the future wrapped object (in xmlObject< whatever > )
  xmlObject_base* fatherSpace;  // Never used for the moment, but well, who knows...
  // Used for pointers and containers : how 'deep' is the recursion
	// of containers and pointers before actually reaching an Xmlizable ?
	// Thanks to a recursion, this number is set directly at construction of the xmlObject:
  uint regOrder;
    
  //****************  CONSTRUCTORS, DESTRUCTORS :

  xmlObject_base();
  xmlObject_base( std::string name );
  virtual ~xmlObject_base();


  //****************  LOADING, SAVING :

  //The loading and saving paradigms:
  void save_XML(xmlNodePtr father_node);
  bool load_XML(xmlNodePtr father_node, bool warn); // (return=true if unused nodes, even if warn=false)
    
  //sub-routines, used by the two preceding functions:

  xmlNodePtr make_child_Element (xmlNodePtr father_node);
  virtual void save_Element(xmlNodePtr the_element);
  
  virtual std::list<xmlNodePtr> get_load_List (xmlNodePtr father_node, bool allow_multiple=false);
  virtual bool load_Element(xmlNodePtr the_element, bool warn); // (return=true if unused nodes, even if warn=false)
  bool check_NotUsed(xmlNodePtr the_element, bool warn);
  // possible custom initialization (see xmlobject.h)
  virtual void init_Element_before()=0;
  virtual void init_Element_after()=0;


  //**************** BASE TYPE PARAMETERS, HANDLED AS ATTRIBUTES OF THE XML NODE:

  struct paramHandler_base
  {
    paramHandler_base(){}
    virtual ~paramHandler_base(){}
    virtual void save_Attributes(xmlNodePtr the_element) = 0;
    virtual void load_Attributes(xmlNodePtr the_element) = 0;
  };

  template<typename TYPE>
  struct paramHandler : public paramHandler_base
  {
    std::map<std::string , TYPE *> registery;
    paramHandler(){}
    ~paramHandler(){}
    void save_Attributes(xmlNodePtr the_element);
    void load_Attributes(xmlNodePtr the_element);
  };

  typedef std::map< const std::type_info * , paramHandler_base* > TypeMap;
  TypeMap my_paramHandlers;

  template<typename TYPE>
  void add_param(TYPE & newParam, std::string paramName);

  //**************** STATIC FUNCTIONS AND VARIABLES:

  // The very important reverseMap, useful for all reverse accesses
  // from objects to wrapping xmlObjects...
  static std::map< void* , xmlObject_base* > reverseMap;
  
  // ...and the method(s) to find an object in the reverseMap.
  // It is template only because it calls template function find_address (see below)
  template<typename XMLIZED>
  static xmlObject_base* find_xmlObject(XMLIZED* object);
  
  // Additional libxml2-related helper functions
  static std::list<xmlNodePtr> get_Element_children(const xmlNodePtr father_node, std::string imposedName="");
  template<typename TYPE>
  static void set_Element_attribute(TYPE *typ, std::istringstream &ist); 
  static void set_NotUsed(xmlNodePtr the_node);

  // Template functions to retrieve the 'fundamental' address of an object, that is:
  //  -the address of its 'Xmlizable' part, if it is a 'regular' xmlized object
  //  -its (uniquely defined) address if it is a 'containing' xmlized object
  //     (i.e., a pointer or stl container)
  template<typename XMLIZED>
  static void* ref_address(XMLIZED* object);
  template<typename XMLIZED>
  static void* ref_address(XMLIZED** object);
  template<typename XMLIZED>
  static void* ref_address(std::vector <XMLIZED>* object);
  template<typename XMLIZED>
  static void* ref_address(std::list <XMLIZED>* object);
  template<typename XMLIZED>
  static void* ref_address(std::deque <XMLIZED>* object);
  
  // Knowing if an xmlNode/xmlAttr was used during the load. 
  // This functionality uses the 'void* _private' pointer of xmlNode and xmlAttr,
  // casted to a boolean pointer.
  // We allow ourselves to modify the xml tree during the load, because we
  // first *make a copy* of it (see function xmlParam::load() )
  static bool *Used, *NotUsed;
  
  // where we stock all xmlObjects which will have to be deleted manually by the user
  static std::list<xmlObject_base*> static_toDelete;
  static void static_Delete();
  
  // where we stock all xml names associated (even temporarily) to objects
  // this is only added for user conveniency (not used by core functionalities of the library)
  static std::map< void* , std::string > lastNames;
  
private:  //non copyable
  xmlObject_base(const xmlObject_base & yo){}

};
  

// *********  AND HERE COMES THE CORRESPONDING TEMPLATE CODE ! **************
// **************************************************************************


// *********** Class xmlObject_base::paramHandler<TYPE>


template<typename TYPE>
void xmlObject_base::paramHandler<TYPE>::save_Attributes(xmlNodePtr the_element)
{
  for(typename std::map<std::string,TYPE*>::iterator it=registery.begin(); it!=registery.end(); ++it)
  {
    std::ostringstream ost;
    ost<<*(it->second);
    xmlSetProp(the_element,(const xmlChar*)it->first.c_str(),(const xmlChar*)ost.str().c_str());
  }
}


template<typename TYPE>
void xmlObject_base::paramHandler<TYPE>::load_Attributes ( xmlNodePtr the_element )
{
  for(typename std::map<std::string,TYPE*>::iterator it=registery.begin(); it!=registery.end(); ++it)
    for( xmlAttrPtr att=the_element->properties; att!=NULL; att=att->next )
      if( !xmlStrcmp( att->name, (const xmlChar*)it->first.c_str() ))
      {
        std::istringstream ist;
        xmlChar* value = xmlNodeListGetString( att->doc, att->children, 1);
        ist.str((const char*)value);
        set_Element_attribute<TYPE>(it->second,ist);
        xmlFree(value);
        // mark att as used, so as to check for useless attributes.
        att->_private=(void*)Used;
      }
}

       
// ************ Class xmlObject_base


template<typename TYPE>
void xmlObject_base::add_param(TYPE & newParam, std::string paramName)
{
  //creating my_handler_of_TYPE if it does not exist yet...
  if (my_paramHandlers.find( & typeid(newParam) ) == my_paramHandlers.end())
    my_paramHandlers[& typeid(newParam)] = new paramHandler<TYPE>();

  //adding newParam in my_handler_of_TYPE
  paramHandler<TYPE> * my_handler_of_TYPE = dynamic_cast< paramHandler<TYPE>* > (my_paramHandlers[& typeid(newParam)]);
  if ( my_handler_of_TYPE->registery.find(paramName) != my_handler_of_TYPE->registery.end() )
  {
    std::cerr<<"WARNING in xmlObject_base::add_param() : Trying to store "
    "an already created parameter name : "<<paramName
    <<". But... OK! Old value indicated by the name is now forgotten."<<std::endl;
  }
  my_handler_of_TYPE->registery[paramName] = &newParam;
}


// find_xmlObject : static template function.
template<class XMLIZED>
xmlObject_base* xmlObject_base::find_xmlObject(XMLIZED* object)
{
  void* refadd=xmlObject_base::ref_address(object);
  std::map<void*, xmlObject_base* >::iterator iter = xmlObject_base::reverseMap.find(refadd);
  if (iter != xmlObject_base::reverseMap.end())
    return ((*iter).second);
  return 0;
}


// ref_address : static template function.
// Different specialization for 'regular' and 'containing' objects

template<class XMLIZED>
void* xmlObject_base::ref_address(XMLIZED* object)
{
  // In this case, we always go back to the "Xmlizable" part of the object.
  //   ==> necessary for objects with multiple and virtual inheritances.
  return (void*) static_cast<Xmlizable*>(object);
  // make sure that "object" is xmlized, i.e., inherits from Xmlizable.
  // else, compile-time error.
}

// un-specializing this function for "containing" objects:

// (1) pointer over an XMLIZED type
template<class XMLIZED>
void* xmlObject_base::ref_address(XMLIZED** object)
{
  return (void*) object;
}
// (2a) stl vector over an XMLIZED type
template<class XMLIZED>
void* xmlObject_base::ref_address(std::vector<XMLIZED>* object)
{
  return (void*) object;
}
// (2b) stl list over an XMLIZED type
template<class XMLIZED>
void* xmlObject_base::ref_address(std::list<XMLIZED>* object)
{
  return (void*) object;
}
// (2c) stl deque over an XMLIZED type
template<class XMLIZED>
void* xmlObject_base::ref_address(std::deque<XMLIZED>* object)
{
  return (void*) object;
}


// set_Element_attribute : static template function
// This template function exists only because of its different specialization 
// when TYPE=string (check it in xmlobject_base.cc) 
template<typename TYPE>
void xmlObject_base::set_Element_attribute(TYPE *typ, std::istringstream &ist)
{
  ist>>*typ;
}

// The specialization when TYPE=string: we must DECLARE it here, to be sure that
// the specialization is applied when using the SHARED library libxmlaparameters++.so
// (whereas the STATIC library works well even without this declaration)
template<>
void xmlObject_base::set_Element_attribute(std::string *typ, std::istringstream &ist); 

// The specialization when TYPE=char*: idem ( and not well tested :( )
template<>
void xmlObject_base::set_Element_attribute(char* *typ, std::istringstream &ist); 


#endif

