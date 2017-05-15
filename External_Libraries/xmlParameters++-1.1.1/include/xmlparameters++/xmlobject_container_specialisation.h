#ifndef XML_OBJECT_CONTAINER_SPEC_H
#define XML_OBJECT_CONTAINER_SPEC_H


/* To avoid that this specialization be applied by the compiler whenever it
 * encounters a template class (not necessarily a container!!), all this code
 * is defined in a 'fake' object xmlObject_CV  (standing for Container Version).
 * Then, the corresponding xmlObject are explicitly specialized for 
 * vector<XMLIZED>, list<XMLIZED> and deque<XMLIZED> */

// Note: With new version of gcc, the specialization must be explicitly made
// mentionning the ALLOCATOR used. Otherwise the specialization is NOT applied.


// (Part I) - The 'intermediate' object xmlObject_CV ...
// *****************************************************

template <typename whatever> struct xmlObject_CV {};

// ...and its only useful specialization :

template<typename XMLIZED, typename ALLOC, template <typename XMLIZED, typename ALLOC> class Cont>
struct xmlObject_CV < Cont<XMLIZED, ALLOC> > : public xmlObject_tbase < Cont<XMLIZED, ALLOC> >
{
  //a "fake" xmlObject to keep trace of the common parameters shared by all elements of the container.
  xmlObject<XMLIZED> myHandler;
   
  //constructors:
  xmlObject_CV( Cont<XMLIZED, ALLOC> & obj , std::string name="" );
  xmlObject_CV( std::string name="" );

  //overwriting some sub-routines from class xmlObject_base :
  virtual void save_Element(xmlNodePtr the_element);
  virtual bool load_Element(xmlNodePtr the_element, bool warn);
    
  //Copy the 'other-than-wrapped-object' properties of an xmlObject< Cont<XMLIZED,ALLOC> > to another.
  //Useful when dealing with containers, or pointers, over class Cont<XMLIZED,ALLOC>
  void copy_Handler(const xmlObject_CV< Cont<XMLIZED, ALLOC> > & origin);
      
  // What are my handled xmlnames? (registered names somewhere in my hierarcy)
  template<typename XMLIZED_2>
  bool what_xmlName( XMLIZED_2 * object , std::string & retname);
  
  //'un'-specialization of some functions:
  static const std::type_info & pointer_typeid ( Cont<XMLIZED, ALLOC> *obj )
  {
    return typeid(obj);
  }
  void init_Element_before()
  {
    if(this->wrapped)
    {
     for(int i=0;i<this->customInit_before.size();++i)
       (*(this->customInit_before[i]))((*this->wrapped));
    }
  }
  void init_Element_after()
  {
    if(this->wrapped)
    {
      for(int i=0;i<this->customInit_after.size();++i)
        (*(this->customInit_after[i]))((*this->wrapped));
    }
  }
  
private : //non copyable
    xmlObject_CV(const xmlObject_CV< Cont<XMLIZED, ALLOC> > & yo){}
};



// (Part II) Defining the 'real' xmlObjects, for vectors, lists and deques
// ***********************************************************************

// Only need to declare the constructors again.

template < typename XMLIZED_ >
struct xmlObject < std::vector<XMLIZED_, std::allocator<XMLIZED_> > > :
public xmlObject_CV < std::vector<XMLIZED_, std::allocator<XMLIZED_> > > 
{    
  xmlObject( std::vector<XMLIZED_, std::allocator<XMLIZED_> > & obj , std::string name="" ) :
  xmlObject_CV < std::vector<XMLIZED_, std::allocator<XMLIZED_> > > (obj, name)
  {}
  
  xmlObject( std::string name="" ) :
  xmlObject_CV < std::vector<XMLIZED_, std::allocator<XMLIZED_> > > (name)
  {}
};

template < typename XMLIZED_ >
struct xmlObject < std::list<XMLIZED_ , std::allocator<XMLIZED_> > > :
public xmlObject_CV < std::list<XMLIZED_, std::allocator<XMLIZED_> > > 
{
  xmlObject( std::list<XMLIZED_, std::allocator<XMLIZED_> > & obj , std::string name="" ) :
  xmlObject_CV < std::list<XMLIZED_, std::allocator<XMLIZED_> > > (obj, name)
  {}
  
  xmlObject( std::string name="" ) :
  xmlObject_CV < std::list<XMLIZED_, std::allocator<XMLIZED_> > > (name)
  {}
};

template < typename XMLIZED_ >
struct xmlObject < std::deque<XMLIZED_, std::allocator<XMLIZED_> > > :
public xmlObject_CV < std::deque<XMLIZED_, std::allocator<XMLIZED_> > > 
{
  xmlObject( std::deque<XMLIZED_, std::allocator<XMLIZED_> > & obj , std::string name="" ) :
  xmlObject_CV < std::deque<XMLIZED_, std::allocator<XMLIZED_> > > (obj, name)
  {}
  
  xmlObject( std::string name="" ) :
  xmlObject_CV < std::deque<XMLIZED_, std::allocator<XMLIZED_> > > (name)
  {}
};



// *******************  AND HERE COMES THE CORRESPONDING TEMPLATE CODE ! *****************
// ***************************************************************************************

//Constructors and destructor

// default: no wrapped object
template<typename XMLIZED, typename ALLOC, template <typename XMLIZED, typename ALLOC> class Cont>
xmlObject_CV < Cont<XMLIZED,ALLOC> >::xmlObject_CV( std::string name ) : xmlObject_tbase< Cont<XMLIZED,ALLOC> >(name), myHandler()
{
  //myHandler=xmlObject<XMLIZED>(); //TODO: seemingly benign lign leads to segfault !?!?
  this->regOrder=myHandler.regOrder+1;
}

// with wrapped object
template<typename XMLIZED, typename ALLOC, template <typename XMLIZED, typename ALLOC> class Cont>
xmlObject_CV < Cont<XMLIZED,ALLOC> >::xmlObject_CV( Cont<XMLIZED,ALLOC> & obj , std::string name ) : xmlObject_tbase< Cont<XMLIZED,ALLOC> >(obj,name), myHandler()
{
  //myHandler=xmlObject<XMLIZED>(); //TODO: seemingly benign lign leads to segfault !?!?
  this->regOrder=myHandler.regOrder+1;
}


// Saving procedure

template<typename XMLIZED, typename ALLOC, template <typename XMLIZED, typename ALLOC> class Cont>
void xmlObject_CV< Cont<XMLIZED,ALLOC> >::save_Element(xmlNodePtr the_element) 
{
  for (typename Cont<XMLIZED,ALLOC>::iterator arf=this->wrapped->begin() ; arf!=this->wrapped->end() ;arf++)
  {
    // Look for an already existing wrapper and destroy it (according to the law "one object<=>one wrapper")
    // Normally there should be none, unless weird desires from the user. 
    // See 'implementation issues' in the README. Think about: using this old wrapper instead?
    delete xmlObject_base::find_xmlObject(&(*arf));    
    
    xmlObject<XMLIZED> * tmp= new xmlObject<XMLIZED>( *arf, myHandler.xmlName );
    tmp->copy_Handler(myHandler);
    tmp->save_XML(the_element);
    // NOTE this important feature: 
    // POINTERS and CONTAINERS *do not* permanently add their 'creations' in the XML structure.
    // The created xmlObject (and its registration in the reverseMap) is directly destroyed:
    delete tmp;
  }
}

// Loading procedure

template<typename XMLIZED, typename ALLOC, template <typename XMLIZED, typename ALLOC> class Cont>
bool xmlObject_CV< Cont<XMLIZED,ALLOC> >::load_Element(xmlNodePtr the_element, bool warn)
{
  //Mark this element as used
  the_element->_private=(void*)xmlObject_base::Used;
  
  // Handling past history and initializations:
  // Always empty the container before loading it
  if(this->wrapped->size())
  {
    std::cerr<<"WARNING in function xmlObject::load_Element() [CONTAINER VERSION], at node <"
             <<this->xmlName<<">. You are asking to load a non-empty container. To remove this "
             "warning, empty your container manually before loading it from xml. Old content "
             "is destroyed, new content is created according to the xml specifications."
             <<std::endl;
    // Prophylactic measure: remove possible wrappers that could have been created.
    // Normally there should be none, unless weird desires from the user.
    for (typename Cont<XMLIZED,ALLOC>::iterator arf=this->wrapped->begin() ; arf!=this->wrapped->end() ;arf++)
      delete xmlObject_base::find_xmlObject(&(*arf));
    this->wrapped->empty();
  }
  
  // Creating the contained elements , and loading them from xml (from the LIST, this time !)
  std::list<xmlNodePtr> find_contained = myHandler.get_load_List( the_element, true ); // Allow multiple list!
  
  bool res=false;
  while( find_contained.size() )
  {
    this->wrapped->push_back( XMLIZED() );	//the container...
    xmlObject<XMLIZED> * tmp =  new xmlObject<XMLIZED>( this->wrapped->back(), myHandler.xmlName );
    tmp->copy_Handler(myHandler);
    tmp->xmlName = myHandler.xmlName;
    res= res | tmp->load_Element( find_contained.front(), warn) ;
    // NOTE this important feature: 
    // POINTERS and CONTAINERS *do not* permanently add their 'creations' in the XML structure.
    // The created xmlObject (and its registration in the reverseMap) is directly destroyed:
    delete tmp;
    find_contained.pop_front();
  }

  // Produce (and print) warning if useless remaining attributes or children elements.
  return res | xmlObject_base::check_NotUsed(the_element, warn) ;
}


template<typename XMLIZED, typename ALLOC, template <typename XMLIZED, typename ALLOC> class Cont>
void xmlObject_CV< Cont<XMLIZED,ALLOC> >::copy_Handler(const xmlObject_CV< Cont<XMLIZED,ALLOC> > & origin)
{
  for(int i=0;i<origin.customInit_before.size();++i)
    this->customInit_before.push_back( origin.customInit_before[i]->copy() );
  for(int i=0;i<origin.customInit_after.size();++i)
    this->customInit_after.push_back( origin.customInit_after[i]->copy() );
  myHandler.copy_Handler( origin.myHandler );
  myHandler.xmlName = origin.myHandler.xmlName;
}



// ***********    SPECIALIZATIONS FOR FUNCTION XMLreg IN THE CONTAINER CASE   ******
// *********************************************************************************

//*************
// (1) When order > 1 , just pass the registration of derivedCLASS to my handler !
//*************

template < uint order , typename XMLIZED , typename ALLOC, typename derivedCLASS, template<typename XMLIZED, typename ALLOC> class Cont >
struct XMLreg <order , Cont<XMLIZED,ALLOC> , derivedCLASS > 
{    
  static bool total_registration ( std::list<std::string> registerNames , xmlObject_CV< Cont<XMLIZED,ALLOC> > * xmlobj )
  {
    if( registerNames.size() != order )
      std::cerr<<"ERROR in function XMLreg <"<<order<<", Cont<XMLIZED,ALLOC>, derivedCLASS >::total_registration(): "
                "the list registerNames must have the same size as the value of 'order' "<<std::endl;
    //auto-registration of the type of the handler (that is, XMLIZED) :
    XMLreg <1 , Cont<XMLIZED,ALLOC> , XMLIZED >::last_step_registration( registerNames.front(), xmlobj );
    //passing the registration of type <derivedCLASS> to the xmlObject<XMLIZED> myHandler of xmlobj:
    registerNames.pop_front();
    return XMLreg <order-1 , XMLIZED , derivedCLASS>::total_registration( registerNames, &(xmlobj->myHandler) );
  }
    
  static bool last_step_registration ( std::string registerName , xmlObject_CV< Cont<XMLIZED,ALLOC> > * xmlobj )
  {
    //only passing the registration of type <derivedCLASS> to the xmlObject<XMLIZED> myHandler of xmlobj:
    return XMLreg <order-1 , XMLIZED , derivedCLASS>::last_step_registration( registerName, &(xmlobj->myHandler) );
  }  
};

//*************
// (2)  For order 1, we can only give a registration name to the contained objects, of type XMLIZED.
//	Thus, if derivedCLASS != XMLIZED , we do not define the function => compilation error.
//*************

template < typename XMLIZED, typename ALLOC, typename derivedCLASS, template<typename XMLIZED, typename ALLOC> class Cont >
struct XMLreg <1 , Cont<XMLIZED,ALLOC> , derivedCLASS > ;


template < typename XMLIZED, typename ALLOC, template<typename XMLIZED, typename ALLOC> class Cont >
struct XMLreg <1 , Cont<XMLIZED,ALLOC> , XMLIZED >
{    
  static bool total_registration ( std::list<std::string> registerNames , xmlObject_CV< Cont<XMLIZED,ALLOC> > * xmlobj )
  {
    if( registerNames.size() != 1 )
      std::cerr<<"ERROR in function XMLreg <1, Cont<XMLIZED,ALLOC>, XMLIZED >::total_registration (): "
                 "the list registerNames must have the same size as 'order' (that is, 1) !"<<std::endl;
    if(xmlobj->myHandler.xmlName !="")
    {
      std::cerr<<"ERROR in function XMLreg <1, Cont<XMLIZED,ALLOC>, XMLIZED >::total_registration (): "
                 "the name for the contained object has already been registered once ! "
                 "Ancient registration name is kept."<<std::endl;
      return false;
    }
    xmlobj->myHandler.xmlName = registerNames.front();
    return true;
  }
    
  static bool last_step_registration ( std::string registerName , xmlObject_CV< Cont<XMLIZED,ALLOC> > * xmlobj )
  {
    if(xmlobj->myHandler.xmlName !="")
    {
      std::cerr<<"ERROR in function XMLreg <1, Cont<XMLIZED,ALLOC>, XMLIZED >::last_step_registration (): "
                 "the name for the contained object has already been registered once ! "
                 "Ancient registration name is kept."<<std::endl;
      return false;
    }
    xmlobj->myHandler.xmlName = registerName;
    return true;    
  }  
};


// ******   SPECIALIZATIONS FOR FUNCTION XMLreginit IN THE CONTAINER CASE   ******
// *******************************************************************************

// (same principle)

//*************
// (1) When order > 1 , just pass the registration of "func" to my handler !
//*************

template < uint order, typename XMLIZED, typename ALLOC, typename baseCLASS, template<typename XMLIZED, typename ALLOC> class Cont >
struct XMLreginit <order, Cont<XMLIZED,ALLOC> , baseCLASS >
{
  static bool stat( std::string registerName, xmlObject<Cont<XMLIZED,ALLOC> > * xmlobj,
                    void (*func)(baseCLASS*), bool afterChildren )
  {
    return XMLreginit<order-1, XMLIZED, baseCLASS>::stat( registerName, &(xmlobj->myHandler), func, afterChildren);
  }
                        
  template<typename REFCLASS>
  static bool memb( std::string registerName, xmlObject<Cont<XMLIZED,ALLOC> > * xmlobj,
                    void (REFCLASS::*func)(baseCLASS*), REFCLASS &instance, bool afterChildren )
  {
    return XMLreginit<order-1, XMLIZED, baseCLASS>::memb( registerName, &(xmlobj->myHandler), func, instance, afterChildren);
  }
                       
  static bool self( std::string registerName, xmlObject<Cont<XMLIZED,ALLOC> > * xmlobj,
                    void (baseCLASS::*func)(), bool afterChildren )
  {
    return XMLreginit<order-1, XMLIZED, baseCLASS>::self( registerName, &(xmlobj->myHandler), func, afterChildren);
  }
  
  template<typename ARGCLASS>
  static bool selfarg( std::string registerName, xmlObject<Cont<XMLIZED,ALLOC> > * xmlobj,
                    void (baseCLASS::*func)(ARGCLASS), ARGCLASS argument, bool afterChildren )
  {
    return XMLreginit<order-1, XMLIZED, baseCLASS>::selfarg( registerName, &(xmlobj->myHandler), func, argument, afterChildren);
  }
};

//*************
// (2)  For order 1, "func" directly provides customInits to myHandler (with baseCLASS=XMLIZED_b)
//*************

template < typename XMLIZED, typename ALLOC, typename XMLIZED_b, template<typename XMLIZED, typename ALLOC> class Cont >
struct XMLreginit <1 , Cont<XMLIZED,ALLOC> , XMLIZED_b >
{
  static bool stat( std::string registerName, xmlObject<Cont<XMLIZED,ALLOC> > * xmlobj,
                    void (*func)(XMLIZED_b*), bool afterChildren )
  {
    if(afterChildren)
      xmlobj->myHandler.customInit_after.push_back(InitFunctor<XMLIZED>::create(func));
    else
      xmlobj->myHandler.customInit_before.push_back(InitFunctor<XMLIZED>::create(func));
    return true;
  }
                        
  template<typename REFCLASS>
  static bool memb( std::string registerName, xmlObject<Cont<XMLIZED,ALLOC> > * xmlobj,
                    void (REFCLASS::*func)(XMLIZED_b*), REFCLASS &instance, bool afterChildren )
  {
    if(afterChildren)
      xmlobj->myHandler.customInit_after.push_back(InitFunctor<XMLIZED>::create(func,instance));
    else
      xmlobj->myHandler.customInit_before.push_back(InitFunctor<XMLIZED>::create(func,instance));
    return true;
  }
  
  static bool self( std::string registerName, xmlObject<Cont<XMLIZED,ALLOC> > * xmlobj,
                    void (XMLIZED_b::*func)(), bool afterChildren )
  {
    if(afterChildren)
      xmlobj->myHandler.customInit_after.push_back(InitFunctor<XMLIZED>::create(func));
    else
      xmlobj->myHandler.customInit_before.push_back(InitFunctor<XMLIZED>::create(func));
    return true;
  }
                       
  template<typename ARGCLASS>
  static bool selfarg( std::string registerName, xmlObject<Cont<XMLIZED,ALLOC> > * xmlobj,
                       void (XMLIZED_b::*func)(ARGCLASS), ARGCLASS argument, bool afterChildren )
  {
    if(afterChildren)
      xmlobj->myHandler.customInit_after.push_back(InitFunctor<XMLIZED>::create(func,argument));
    else
      xmlobj->myHandler.customInit_before.push_back(InitFunctor<XMLIZED>::create(func,argument));
    return true;
  }
};


// *******    SPECIALIZATIONS FOR FUNCTION XMLname IN THE CONTAINER CASE   ******
// ******************************************************************************

// What are my handled xmlnames?

// (1) General behavior is passing to my handler...
template < typename XMLIZED, typename ALLOC, typename XMLIZED_2, template<typename XMLIZED, typename ALLOC> class Cont >
struct XMLname< Cont<XMLIZED,ALLOC>, XMLIZED_2 >
{
  static bool findname( XMLIZED_2 * object , xmlObject_CV< Cont<XMLIZED,ALLOC> > * xmlobj , std::string & retname )
  {
    return XMLname<XMLIZED,XMLIZED_2>::findname(object,&(xmlobj->myHandler),retname);
  }
};

// (2) ...except if XMLIZED_2 = XMLIZED, my contained type. So: (yet another) TEMPLATE SPECIALIZATION !
template < typename XMLIZED, typename ALLOC, template<typename XMLIZED, typename ALLOC> class Cont >
struct XMLname< Cont<XMLIZED,ALLOC>, XMLIZED >
{
  static bool findname( XMLIZED * object , xmlObject_CV< Cont<XMLIZED,ALLOC> > * xmlobj , std::string & retname )
  {
    retname=xmlobj->myHandler.xmlName;
    return true;
  }
};



#endif

