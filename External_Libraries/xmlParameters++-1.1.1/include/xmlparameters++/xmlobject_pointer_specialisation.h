#ifndef XML_OBJECT_POINTER_SPEC_H
#define XML_OBJECT_POINTER_SPEC_H

#include"xmlfactory.h"

template<typename XMLIZED>
struct xmlObject<XMLIZED*> : public xmlObject_tbase<XMLIZED*>
{
  //an object factory to register different derived types for the object.
  xmlFactory<XMLIZED> * derivedFactory;

  //a "fake" (not wrapping anything) xmlObject<XMLIZED>, if the pointer is used as a simple 'container' over its base class
  xmlObject<XMLIZED> myHandler;

  //constructors and destructors:
  xmlObject(XMLIZED* & obj , std::string name = "" );
  xmlObject( std::string name = "" );
  virtual ~xmlObject();

  //overwriting some sub-routines from class xmlObject_base :
  virtual void save_Element (xmlNodePtr the_element);
  virtual std::list<xmlNodePtr> get_load_List (xmlNodePtr father_node, bool allow_multiple=false);
  virtual bool load_Element (xmlNodePtr the_element, bool warn);

  //Copy the 'other-than-wrapped-object' properties of an xmlObject<XMLIZED*> to another.
  //Useful when dealing with containers, or pointers, over class XMLIZED*
  void copy_Handler(const xmlObject<XMLIZED*> & origin);

  // What are my handled xmlnames? (registered names somewhere in my hierarcy)
  template<typename XMLIZED_2>
  bool what_xmlName( XMLIZED_2 * object , std::string & retname);
  
  //'un'-specialization of some functions:
  static const std::type_info & pointer_typeid ( XMLIZED* *obj )
  {
    return typeid(obj);
  }
  void init_Element_before()
  {
    if(this->wrapped)
    {
     for(int i=0;i<this->customInit_before.size();++i)
       (*(this->customInit_before[i]))(*(this->wrapped));
   }
  }
  void init_Element_after()
  {
    if(this->wrapped)
    {
      for(int i=0;i<this->customInit_after.size();++i)
        (*(this->customInit_after[i]))(*(this->wrapped));
    }
  }
  
private : //non copyable
  xmlObject(const xmlObject<XMLIZED*> & yo){}
};



// *******************  AND HERE COMES THE CORRESPONDING TEMPLATE CODE ! *****************
// ***************************************************************************************


// Constructors and destructor

// default: no wrapped object
template<class XMLIZED>
xmlObject<XMLIZED*>::xmlObject(std::string name ) : xmlObject_tbase<XMLIZED*>(name), derivedFactory(0), myHandler()
{
  derivedFactory=new xmlFactory<XMLIZED>();
  //myHandler=xmlObject<XMLIZED>(); // TODO: seemingly benign lign (and its three equivalents) leads to segfault !?!?
                                  // really did my best on this one, couldn't find out... luckily, I dont need it :| 
                                  // seems somehow linked to the object factory and its STL maps.
  this->regOrder=myHandler.regOrder+1;
}

// with wrapped object
template<class XMLIZED>
xmlObject<XMLIZED*>::xmlObject(XMLIZED* & obj , std::string name ) : xmlObject_tbase<XMLIZED*>(obj,name), derivedFactory(0), myHandler()
{
  derivedFactory=new xmlFactory<XMLIZED>();
  //myHandler=xmlObject<XMLIZED>(); //TODO: seemingly benign lign leads to segfault !?!?
  this->regOrder=myHandler.regOrder+1;
}

template<class XMLIZED>
xmlObject<XMLIZED*>::~xmlObject()
{
  xmlFactory<XMLIZED>::remove_occurence(derivedFactory);
  //if(wrapped)
    //std::cerr<<"REMOVED from XML structure: Object of type "<<typeid(*wrapped).name()<<" at address "<<wrapped_noType<<" corresponding to xmlObject "<<this<<" of name '"<<xmlName<<"'"<<std::endl;
}


// Saving procedure

template<class XMLIZED>
void xmlObject<XMLIZED*>::save_Element( xmlNodePtr the_element )
{
  if(this->wrapped==0)
    std::cerr<<"ERROR in function xmlObject::save_Element() [POINTER SPECIALIZATION], at node<"
             <<the_element->name<<">. This xmlObject does not wrap any real pointer, "
             "which should never happen! Exiting."<<std::endl;
             // Should *really* never happen. If it does, segfault at next line :)
  if( *(this->wrapped) )
  {
    // Look for an already existing wrapper and destroy it (according to the law "one object<=>one wrapper")
    // Normally there should be none, unless weird desires from the user. 
    // See 'implementation issues' in the README. Think about: using this old wrapper instead?
    delete xmlObject_base::find_xmlObject(*(this->wrapped));    
    
    //that's the tough line requiring a reflexive object factory...
    std::string saveName;
    if( !(derivedFactory->GetId(*(this->wrapped),saveName)) ) 
      // Normally, this line puts the registration name in 'saveName'. Else:
    {
      std::cerr<<"WARNING in function xmlPointer::save_Element(), at node<"
               <<the_element->name<<">. The pointed object's type was not "
               "registered in the factory."<<std::endl;
      saveName="unregistered-type";
    }

    // Even if **(this->wrapped) was already wrapped, 
    // we prefer to create a new wrapper xmlObject, just for saving.
    xmlObject<XMLIZED> * tmp=  new xmlObject<XMLIZED>( **(this->wrapped), saveName );
    tmp->copy_Handler( myHandler );
    tmp->save_XML( the_element );
    // NOTE this important feature: 
    // POINTERS and CONTAINERS *do not* permanently add their 'creations' in the XML structure.
    // The created xmlObject (and its registration in the reverseMap) is directly destroyed:
    delete tmp;
  }
}


// Loading procedure: To handle polymorphism, we also rewrite 'get_load_List',
// to find all nodes corresponding to an *instantiation* of a pointed object.

template<class XMLIZED>
std::list<xmlNodePtr> xmlObject<XMLIZED*>::get_load_List( xmlNodePtr father_node, bool allow_multiple )
{
  // first find the list of elements that bear the pointer's name.
  std::list<xmlNodePtr> pointres=xmlObject_base::get_load_List(father_node,true); 
  // (allow multiple response for the moment -- the warning is handled afterwards)
  
  // then return the list of elements that bear SPECIALIZED names for pointed objects.
  std::list<xmlNodePtr> res, resinter;
  while ( pointres.size() )
  {
    resinter=xmlObject_base::get_Element_children( pointres.front() ); 
    //  => careful: if pointres=father_node, this returns ALL children of father_node.
    while ( resinter.size() )
    {
      if( derivedFactory->Find( (const char*)resinter.front()->name ) )
        res.push_back( resinter.front() );
      resinter.pop_front();
    }
    pointres.pop_front();
  }
  
  //and again a possible warning:
  if( !allow_multiple )
  {
    if( res.size()>1 )
    std::cerr<<"WARNING in function xmlObject::get_load_List [POINTER VERSION]: "
             "Node <"<<father_node->name<<"> leads to more than one possible "
             "specialization for pointer called <"<<this->xmlName<<">. Only the first "
             "specialization will be taken into account."<<std::endl;
    while(res.size()>1)
      res.pop_back();
  }
  return res;
}


template<class XMLIZED>
bool xmlObject<XMLIZED*>::load_Element( xmlNodePtr the_element, bool warn )
{ 
  //Mark this element as used
  the_element->_private=(void*)xmlObject_base::Used;
  
  // Handling past history and initializations:
  if(!this->wrapped)
    std::cerr<<"ERROR in function xmlObject::load_Element() [POINTER VERSION], at node <"
             <<this->xmlName<<">. This xmlPointer does not wrap any real pointer, which "
             "should never happen! Exiting."<<std::endl; 
             // Should *really* never happen. If it does, segfault at next line :)
  if( *(this->wrapped) )
  {
    // We do not take the risk of deleting the pointer (to avoid a 'double delete' error),
    // but we warn the user if the wrapped pointer is already non-null.
    std::cerr<<"WARNING in function xmlObject::load_Element() [POINTER VERSION], at node <"
             <<this->xmlName<<">. You are asking to load a non-null pointer. To be clean, delete "
             "manually the pointer before the call to xmlParam::load() and set it at 0. "
             "We DO NOT delete the pointer, but point it to a new object created according "
             "to the xml specifications."<<std::endl;
    // Prophylactic measure: remove possible wrappers that could have been created.
    // Normally there should be none, unless weird desires from the user.
    delete xmlObject_base::find_xmlObject(*(this->wrapped));
  }
  
  // (this name is necessarily registered, see get_load_List()...)
  std::string loadName=(const char*)the_element->name;
  const CreateStruct<XMLIZED> *cs = derivedFactory->Find( loadName );
  
  *(this->wrapped) = cs->Create();
  xmlObject<XMLIZED> * tmp =  new xmlObject<XMLIZED>( **(this->wrapped) , loadName );
  tmp->copy_Handler( myHandler );
  for(int i=0;i<cs->customInit_before().size();++i)
    tmp->customInit_before.push_back( cs->customInit_before()[i]->copy() );
  for(int i=0;i<cs->customInit_after().size();++i)
    tmp->customInit_after.push_back( cs->customInit_after()[i]->copy() );
  
  bool res=tmp->load_Element(the_element, warn);
  // NOTE this important feature: 
  // POINTERS and CONTAINERS *do not* add their 'creations' in the XML structure.
  // The created xmlObject (and its registration in the reverseMap) is directly destroyed:
  delete tmp;
  
  // Produce (and print) warning if useless remaining attributes or children elements.
  return res | xmlObject_base::check_NotUsed(the_element, warn) ;
}


//The copy_Handler function in this case:

template<class XMLIZED>
void xmlObject<XMLIZED*>::copy_Handler(const xmlObject<XMLIZED*> & origin)
{
  // duplicated InitFunctors, but shared factory :oP
  for(int i=0;i<origin.customInit_before.size();++i)
    this->customInit_before.push_back( origin.customInit_before[i]->copy() );
  for(int i=0;i<origin.customInit_after.size();++i)
    this->customInit_after.push_back( origin.customInit_after[i]->copy() );
  xmlFactory<XMLIZED>::remove_occurence(derivedFactory);
  xmlFactory<XMLIZED>::assign(derivedFactory , origin.derivedFactory);
  myHandler.copy_Handler( origin.myHandler );
  myHandler.xmlName = origin.myHandler.xmlName;
}



// ***********    SPECIALIZATIONS FOR FUNCTION XMLreg IN THE POINTER CASE   ******
// *******************************************************************************

//*************
// (1) When order > 1, just pass the registration of derivedCLASS to my handler !
//*************

template < uint order , typename XMLIZED , typename derivedCLASS >
struct XMLreg <order , XMLIZED* , derivedCLASS >
{
  static bool total_registration ( std::list<std::string> registerNames , xmlObject<XMLIZED*> * xmlobj )
  {
    if( registerNames.size() != order )
      std::cerr<<"ERROR in function XMLreg <"<<order<<" , XMLIZED * , derivedCLASS >::total_registration "
                 "(list<string> registerNames , xmlObject<XMLIZED*> * xmlobj) : the list registerNames "
                 "must have the same size as the value of 'order' "<<std::endl;
    //auto-registration of the type of the handler (that is, XMLIZED) :
    XMLreg <1 , XMLIZED* , XMLIZED >::last_step_registration ( registerNames.front() , xmlobj );
    //passing the registration of type <derivedCLASS> to the xmlObject<XMLIZED> myHandler of xmlobj:
    registerNames.pop_front();
    return XMLreg <order-1 , XMLIZED , derivedCLASS>::total_registration ( registerNames, &(xmlobj->myHandler) );
  }

  static bool last_step_registration ( std::string registerName , xmlObject<XMLIZED*> * xmlobj )
  {
    //only passing the registration of type <derivedCLASS> to the xmlObject<XMLIZED> myHandler of xmlobj:
    return XMLreg <order-1 , XMLIZED , derivedCLASS>::last_step_registration ( registerName, &(xmlobj->myHandler) );
  }                     
};

//*************
// (2)   For order 1, OUR derivedFactory is concerned with the type derivedCLASS :
//*************

template < typename XMLIZED , typename derivedCLASS >
struct XMLreg <1 , XMLIZED* , derivedCLASS >
{
  static bool total_registration ( std::list<std::string> registerNames , xmlObject<XMLIZED*> * xmlobj )
  {
    if( registerNames.size() != 1 )
      std::cerr<<"ERROR in function XMLreg <1 , XMLIZED * , derivedCLASS >::total_registration "
                 "(list<string> registerNames , xmlObject<XMLIZED*> * xmlobj) : the list "
                 "'registerNames' must have the same size as the value of 'order' "<<std::endl;
    return xmlobj->derivedFactory->template _Register<derivedCLASS>( registerNames.front() );
  }

  static bool last_step_registration ( std::string registerName , xmlObject<XMLIZED*> * xmlobj )
  {
    return xmlobj->derivedFactory->template _Register<derivedCLASS>( registerName );
  }
};


// *******    SPECIALIZATIONS FOR FUNCTION XMLreginit IN THE POINTER CASE   ******
// *******************************************************************************

// (same principle)

//*************
// (1)  When order > 1 , just pass the registration of baseCLASS to my handler !
//*************

template < uint order , typename XMLIZED , typename baseCLASS >
struct XMLreginit <order , XMLIZED* , baseCLASS >
{
  static bool stat( std::string registerName, xmlObject<XMLIZED*> * xmlobj,
                    void (*func)(baseCLASS*), bool afterChildren )
  {
    return XMLreginit<order-1, XMLIZED, baseCLASS>::stat( registerName, &(xmlobj->myHandler), func, afterChildren);
  }
                        
  template<typename REFCLASS>
  static bool memb( std::string registerName, xmlObject<XMLIZED*> * xmlobj,
                    void (REFCLASS::*func)(baseCLASS*), REFCLASS &instance, bool afterChildren )
  {
    return XMLreginit<order-1, XMLIZED, baseCLASS>::memb( registerName, &(xmlobj->myHandler), func, instance, afterChildren);
  }
                        
  static bool self( std::string registerName, xmlObject<XMLIZED*> * xmlobj,
                    void (baseCLASS::*func)(), bool afterChildren )
  {
    return XMLreginit<order-1, XMLIZED, baseCLASS>::self( registerName, &(xmlobj->myHandler), func, afterChildren);
  }
  
  template<typename ARGCLASS>
  static bool selfarg( std::string registerName, xmlObject<XMLIZED*> * xmlobj,
                    void (baseCLASS::*func)(ARGCLASS), ARGCLASS argument, bool afterChildren )
  {
    return XMLreginit<order-1, XMLIZED, baseCLASS>::selfarg( registerName, &(xmlobj->myHandler), func, argument, afterChildren);
  }
};

//*************
// (2)   For order 1, OUR derivedFactory is concerned with the type baseCLASS=XMLIZED_b :
//*************

template <typename XMLIZED , typename XMLIZED_b >
struct XMLreginit <1 , XMLIZED* , XMLIZED_b >
{
  static bool stat( std::string registerName, xmlObject<XMLIZED*> * xmlobj,
                    void (*func)(XMLIZED_b*), bool afterChildren )
  {
    InitFunctor<XMLIZED> *iF = InitFunctor<XMLIZED>::create(func);
    return xmlobj->derivedFactory->_RegisterInit( registerName, iF, afterChildren );
  }
                        
  template<typename REFCLASS>
  static bool memb( std::string registerName, xmlObject<XMLIZED*> * xmlobj,
                    void (REFCLASS::*func)(XMLIZED_b*), REFCLASS &instance, bool afterChildren )
  {
    InitFunctor<XMLIZED> *iF = InitFunctor<XMLIZED>::create(func,instance);
    return xmlobj->derivedFactory->_RegisterInit( registerName, iF, afterChildren );
  }
  
  static bool self( std::string registerName, xmlObject<XMLIZED*> * xmlobj,
                    void (XMLIZED_b::*func)(), bool afterChildren )
  {
    InitFunctor<XMLIZED> *iF = InitFunctor<XMLIZED>::create(func);
    return xmlobj->derivedFactory->_RegisterInit( registerName, iF, afterChildren );
  }
  
  template<typename ARGCLASS>
  static bool selfarg( std::string registerName, xmlObject<XMLIZED*> * xmlobj,
                    void (XMLIZED_b::*func)(ARGCLASS), ARGCLASS argument, bool afterChildren )
  {
    InitFunctor<XMLIZED> *iF = InitFunctor<XMLIZED>::create(func,argument);
    return xmlobj->derivedFactory->_RegisterInit( registerName, iF, afterChildren );
  }
  
};


// *******    SPECIALIZATIONS FOR FUNCTION XMLname IN THE POINTER CASE   ******
// ****************************************************************************

// What are my handled xmlnames?

// (1) General behavior is passing to my handler...
template < typename XMLIZED , typename XMLIZED_2 >
struct XMLname<XMLIZED*,XMLIZED_2>
{
  static bool findname( XMLIZED_2 * object , xmlObject<XMLIZED*> * xmlobj , std::string & retname)
  {
    return XMLname<XMLIZED,XMLIZED_2>::findname(object,&(xmlobj->myHandler),retname);
  }
};

// (2) ...except if XMLIZED_2 = XMLIZED, my pointed type. So: (yet another) TEMPLATE SPECIALIZATION !
template < typename XMLIZED >
struct XMLname<XMLIZED*,XMLIZED>
{
  static bool findname( XMLIZED * object , xmlObject<XMLIZED*> * xmlobj , std::string & retname)
  {
    return xmlobj->derivedFactory->GetId(object,retname); // if true, then retname is filled with correct registration name.
  }
};

#endif

