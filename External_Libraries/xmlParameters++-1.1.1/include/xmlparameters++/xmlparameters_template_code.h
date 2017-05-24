


//******************************************************************************
//****** (0) Helper functions: look for an object and possibly warn     ********
//******************************************************************************


namespace find_warn
{

template<typename XMLIZED>
xmlObject_base * base(XMLIZED & object, std::string callerFunction)
{
  xmlObject_base * xmlobj = xmlObject_base::find_xmlObject ( &object );
  if(!xmlobj)
  {
    std::cerr<<"ERROR in "<<callerFunction<<" : Object of type "
              <<typeid(object).name()<<" at address "<<&object
              <<" has not been added in the XML structure yet!"<<std::endl;
    exit(0);
  }
  return xmlobj;
}


template<typename XMLIZED>
xmlObject<XMLIZED>* typed(XMLIZED & object, std::string callerFunction)
{
  xmlObject_base * xmlobjbase = find_warn::base(object,callerFunction);  
  xmlObject<XMLIZED> * xmlobj = dynamic_cast< xmlObject<XMLIZED>* > (xmlobjbase);
  if (!xmlobj)
  {
    std::cerr<<"ERROR in "<<callerFunction<<" : Object of type "
             <<typeid(object).name()<<" at address "<<&object
             <<" was already added in the XML structure, but under another type: "
             <<xmlobjbase->wrapped_typeid().name()<<" ! To my uderstanding, "
             <<"this can only happen if you used some xmlParam function on a "
             <<"recasted version of this object. Don't."<<std::endl;
    exit(0);
  }
  return xmlobj;
}


// (This one is only called by add_init functions)
template<typename XMLIZED>
std::vector<InitFunctor<XMLIZED>*> &functor_ref(XMLIZED & object, bool afterChildren)
{
  xmlObject<XMLIZED>* xmlobj = find_warn::typed(object,"xmlParam::add_init()");  
  if( afterChildren )
    return xmlobj->customInit_after;
  else
    return xmlobj->customInit_before;
}

} // namespace find_warn



//************************************************************************
//***************        (I) base class Xmlizable     ********************
//************************************************************************


template<typename XMLIZED>
void Xmlizable::add_child(XMLIZED & object , std::string name )
{
  xmlObject_base * my_wrapper = find_warn::base(*this,"Xmlizable::add_child()");
  my_wrapper->add_childSpace( new xmlObject<XMLIZED>(object,name) );
}

  
template < typename TYPE >
void Xmlizable::add_param( TYPE & newParam, std::string paramName )
{
  xmlObject_base * my_wrapper = find_warn::base(*this,"Xmlizable::add_param()");
  my_wrapper -> add_param( newParam, paramName );
}


//*************************************************************************
//**************        Other functions ...     ***************************
//*************************************************************************


template<typename XMLIZED>
void add_object(XMLIZED & object , std::string name)
{
  xmlObject<XMLIZED> * xmlobj = new xmlObject<XMLIZED>(object, name);
  xmlObject_base::static_toDelete.push_back( xmlobj );
}


template<typename XMLIZED>
void remove_object(XMLIZED & object)
{
  delete xmlObject_base::find_xmlObject ( &object );
  // CHOICE: We give no warning when asked to remove an object which is not in the structure...
}


template<typename XMLIZED>
void save(XMLIZED & object , xmlNodePtr father_node )
{
  xmlObject_base * xmlobj = find_warn::base( object,"xmlParam::save()" );
  xmlobj->save_XML (father_node) ;
}


template<typename XMLIZED>
void load(XMLIZED & object , xmlNodePtr father_node, bool warn_if_unused_nodes )
{
  xmlObject_base * xmlobj = find_warn::base( object,"xmlParam::load()" );
  // We copy the whole structure under father_node, so as to be able to "play"
  // on it (set "_private" pointers to detect unused nodes, see xmlobject_base.h) 
  xmlNodePtr father_copy =xmlCopyNode(father_node,1); // (libxml2 function)
  xmlObject_base::set_NotUsed(father_copy) ; // set all "_private" pointers to xmlObject_base::NotUsed
  bool res=xmlobj->load_XML (father_copy, warn_if_unused_nodes) ;
  if(res && warn_if_unused_nodes)
    std::cerr<<"These warnings probably mean that you have spelling errors in your XML file, "
               "or that your XML file is not up-to-date with the programs."<<std::endl;
  xmlFreeNode(father_copy); // TODO:Check
}

 
// Custom functions... almost identical to the Xmlizable versions.

template < typename TYPE , typename XMLIZED >
void add_param_custom(TYPE & newParam, std::string paramName, XMLIZED &fatherObject)
{
  xmlObject_base * father = find_warn::base( fatherObject,"xmlParam::add_param_custom()" );
  father->add_param(newParam, paramName);
}


template<typename XMLIZED_1, typename XMLIZED_2>
void add_child_custom(XMLIZED_1 & childObject, std::string name, XMLIZED_2 & fatherObject)
{
  xmlObject_base * father = find_warn::base( fatherObject,"xmlParam::add_child_custom()" );
  father->add_childSpace( new xmlObject<XMLIZED_1>(childObject,name) );
}


// The three add-init functions:

// (a) Initialize using a static (C-type) function:
template<typename XMLIZED, typename XMLIZED_b>
void add_init (XMLIZED & object, void (*func)(XMLIZED_b*), bool afterChildren)
{
  find_warn::functor_ref(object,afterChildren).push_back(InitFunctor<XMLIZED>::create(func));
}

// (b) Initialize using a class member function from a particular instance of that class (C++):
template<typename XMLIZED, typename XMLIZED_b, typename REFCLASS>
void add_init (XMLIZED & object, void (REFCLASS::*func)(XMLIZED_b*), REFCLASS &instance, bool afterChildren)
{
  find_warn::functor_ref(object,afterChildren).push_back(InitFunctor<XMLIZED>::create(func,instance));
}

// (c) Initialize using a class member function from class XMLIZED_b itself:
template<typename XMLIZED, typename XMLIZED_b>
void add_init (XMLIZED & object, void (XMLIZED_b::*func)(), bool afterChildren)
{
  find_warn::functor_ref(object,afterChildren).push_back(InitFunctor<XMLIZED>::create(func));
}

// (d) Initialize using a class member function from class XMLIZED_b itself:
template<typename XMLIZED, typename XMLIZED_b, typename ARGCLASS>
void add_init (XMLIZED & object, void (XMLIZED_b::*func)(ARGCLASS), ARGCLASS argument, bool afterChildren)
{
  find_warn::functor_ref(object,afterChildren).push_back(InitFunctor<XMLIZED>::create(func,argument));
}


// ************** The three xmlName-related functions

// (1) The easy one
template<typename XMLIZED>
std::string get_xmlName(XMLIZED & object)
{
  xmlObject_base * xmlobj = find_warn::base( object,"xmlParam::get_xmlName()" );
  return xmlobj->xmlName ;
}

// (2) The incomplete one ( so much trouble for so little :D )
template<typename XMLIZED, typename XMLIZEDref>
std::string handled_xmlName (XMLIZED & object, XMLIZEDref & refobject )
{
  std::string retname="";
  xmlObject<XMLIZEDref> * xmlobj = find_warn::typed(refobject,"xmlParam::handled_xmlName()");
  if( !(XMLname<XMLIZEDref,XMLIZED>::findname(&object,xmlobj,retname)) ) // normally, this line puts the registration name in 'retname'. Else:
    std::cerr<<"ERROR in xmlParam::handled_xmlName() : 'object' of type "
             <<typeid(object).name()<<" at address "<<&object
             <<" is NOT HANDLED by object 'refobject'."
             <<" Did you call correctly function xmlParam::Register<> on 'refobject'?"<<std::endl;
  return retname;
}

// (3) The dangerous one
template<typename XMLIZED>
std::string last_xmlName(XMLIZED & object)
{
  void* refadd = xmlObject_base::ref_address( &object );
  std::map<void*, std::string>::iterator iter = xmlObject_base::lastNames.find( refadd );
  if (iter != xmlObject_base::lastNames.end())
    return ((*iter).second);
  else
    std::cerr<<"ERROR in xmlParam::last_xmlName() : Object of type "
    <<typeid(object).name()<<" at address "<<refadd
    <<" has not been given a name by the XML structure yet!"<<std::endl;
  return "";
}



//***************************************************************************
//**************      (III)   Registration        ***************************
//***************************************************************************
 
 
// Total-chain version
// ********************

template<uint order, class XMLIZED_d>
template<class CONTAINING>
Register<order,XMLIZED_d>::Register( CONTAINING & object, std::list<std::string> registerNames )
{
  xmlObject<CONTAINING> *xmlobj = find_warn::typed(object,"xmlParam::Register()");
  XMLreg <order, CONTAINING, XMLIZED_d>::total_registration ( registerNames , xmlobj );
}


// Single-name version
//********************

template<uint order, class XMLIZED_d>
template<class CONTAINING>
Register<order,XMLIZED_d>::Register( CONTAINING & object, std::string registerName )
{
  xmlObject<CONTAINING> *xmlobj = find_warn::typed(object,"xmlParam::Register()");
  XMLreg <order, CONTAINING, XMLIZED_d>::last_step_registration ( registerName , xmlobj );
}

// (a) Register a static (C-type) initialization function
//*******************************************************

template<uint order>
template<typename CONTAINING, typename XMLIZED_b>
Register_init<order>::Register_init (CONTAINING & object, std::string registerName, 
                                     void (*func)(XMLIZED_b*), bool afterChildren)
{
  xmlObject<CONTAINING> *xmlobj = find_warn::typed(object,"xmlParam::RegisterInit()");
  XMLreginit<order, CONTAINING, XMLIZED_b>::stat(registerName, xmlobj, func, afterChildren );
}

// (b) Register a class member (C++) initialization function
//**********************************************************

template<uint order>
template<typename CONTAINING, typename XMLIZED_b, typename REFCLASS>
Register_init<order>::Register_init (CONTAINING & object, std::string registerName,
                                     void (REFCLASS::*func)(XMLIZED_b*), REFCLASS &instance, bool afterChildren)
{
  xmlObject<CONTAINING> *xmlobj=find_warn::typed(object,"xmlParam::RegisterInit()");
  XMLreginit<order, CONTAINING, XMLIZED_b>::memb( registerName, xmlobj, func, instance, afterChildren );
}

// (c) Register a class member (C++) self-initialization function
//***************************************************************

template<uint order>
template<typename CONTAINING, typename XMLIZED_b>
Register_init<order>::Register_init (CONTAINING & object, std::string registerName,
                                     void (XMLIZED_b::*func)(), bool afterChildren)
{
  xmlObject<CONTAINING> *xmlobj=find_warn::typed(object,"xmlParam::RegisterInit()");
  XMLreginit<order, CONTAINING, XMLIZED_b>::self( registerName, xmlobj, func, afterChildren );
}

// (d) Register a class member (C++) self-initialization function with argument
//*****************************************************************************

template<uint order>
template<typename CONTAINING, typename XMLIZED_b, typename ARGCLASS>
Register_init<order>::Register_init (CONTAINING & object, std::string registerName,
                                     void (XMLIZED_b::*func)(ARGCLASS), ARGCLASS argument, bool afterChildren)
{
  xmlObject<CONTAINING> *xmlobj=find_warn::typed(object,"xmlParam::RegisterInit()");
  XMLreginit<order, CONTAINING, XMLIZED_b>::selfarg( registerName, xmlobj, func, argument, afterChildren );
}

