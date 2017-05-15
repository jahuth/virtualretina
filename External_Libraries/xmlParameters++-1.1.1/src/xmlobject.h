#ifndef XML_OBJECT_H
#define XML_OBJECT_H

// The 'basic' template specialization of class xmlObject.
// Can wrap any c++ object of type XMLIZED which inherits from class Xmlizable.
// Further template specializations of xmlObject (in separate headers) allow to wrap 
// *pointers* and *STL containers* (and then, any combination thereof) over objects
// which inherit from Xmlizable.

// In this header and in the following, all code starts being totally template.


// (1) Intermediate class 'xmlObject_tbase' ('template base')
// ************************************************************

// contains the template code common to all specializations of xmlObject.


template<typename XMLIZED>
struct xmlObject_tbase : public xmlObject_base
{
  XMLIZED* wrapped;

  // Custom initialization of the handled object: Two sets of possible functors,
  // called after the XML load of the object's parameters, and respectively
  // *before* and *after* its *childrens*' XML load.
  std::vector<InitFunctor<XMLIZED>*> customInit_before, customInit_after;
  
  // default constructor: no wrapped object
  xmlObject_tbase( std::string name = "") : xmlObject_base(name) , wrapped(0) 
  {}
  
  // constructor with wrapped object
  xmlObject_tbase( XMLIZED & obj , std::string name="" ) : xmlObject_base(name) , wrapped(&obj)
  {
    // wrapped_noType is always the 'reference' address of the object
    wrapped_noType = xmlObject_base::ref_address( &obj ) ;
    xmlObject_base::reverseMap [ wrapped_noType ] = this ;
    xmlObject_base::lastNames [ wrapped_noType ] = name;
    //std::cerr<<"ADDED in the XML structure: Object of type "<<typeid(obj).name()<<" at address "<<wrapped_noType<<" corresponding to xmlObject "<<this<<" of name '"<<name<<"'"<<std::endl;
  }

  virtual ~xmlObject_tbase()
  {
    // always destroy my InitFunctors ! (so, InitFunctors should never be shared by different xmlObjects, but copied)
    for(int i=0;i<this->customInit_before.size();++i)
      delete customInit_before[i];
    for(int i=0;i<this->customInit_after.size();++i)
      delete customInit_after[i];
    //if(wrapped)
    //  std::cerr<<"REMOVED from XML structure: Object of type "<<typeid(*wrapped).name()<<" at address "<<wrapped_noType<<" corresponding to xmlObject "<<this<<" of name '"<<xmlName<<"'"<<std::endl;
  }

  // This function is only used for information purposes, in xmlParam::find_warn::typed()
  const std::type_info & wrapped_typeid()
  {
    return typeid(wrapped); // this is always "XMLIZED" !
  }
};



// (2) Basic specialization 'xmlObject<XMLIZED>'
// *********************************************


template<typename XMLIZED>
struct xmlObject : public xmlObject_tbase<XMLIZED>
{
  // constructor without wrapped object.
  xmlObject( std::string name="" ) : xmlObject_tbase<XMLIZED>(name)
  {}
  
  // constructor with wrapped object.
  // THIS IS THE ONLY PLACE WHERE FUNCTION xmlize() IS CALLED ! But it is crucial.
  xmlObject( XMLIZED & obj, std::string name="" ) : xmlObject_tbase<XMLIZED>(obj,name)
  {
    obj.xmlize() ;
  }

  // This is the only place where functions 'xmlinit_xxx()' are called.
  void init_Element_before()
  {
    if(this->wrapped)
    {
      this->wrapped->xmlinit();
      for(int i=0;i<this->customInit_before.size();++i)
        (*(this->customInit_before[i]))(*(this->wrapped));
    }
  }
  void init_Element_after()
  {
    if(this->wrapped)
    {
      this->wrapped->xmlinit_after_children();
      for(int i=0;i<this->customInit_after.size();++i)
        (*(this->customInit_after[i]))(*(this->wrapped));
    }
  }
  
  //Copy the 'other-than-wrapped object' properties of an xmlObject<XMLIZED> to another.
  //Useful when dealing with containers, or pointers, over class XMLIZED
  void copy_Handler(const xmlObject<XMLIZED> & origin)
  {
    for(int i=0;i<origin.customInit_before.size();++i)
      this->customInit_before.push_back( origin.customInit_before[i]->copy() );
    for(int i=0;i<origin.customInit_after.size();++i)
      this->customInit_after.push_back( origin.customInit_after[i]->copy() );
  }

  //This function is used to find the eventual derived type of an XMLIZED*, which can be something else than XMLIZED.
  //It will be different for the pointer and container specializations of xmlObject.
  //Note that it implies, that XMLIZED must necessarily inherit from class Xmlizable.
  static const std::type_info & pointer_typeid ( XMLIZED *obj )
  {
    return obj->get_pointer_typeid();
  }
  
private : //non copyable
  xmlObject(const xmlObject<XMLIZED> & yo){}
};


// REGISTRATION FUNCTIONS :

//We have to make them external functions of fake structures 'XMLreg', to allow partial template specialization.

// There are two possible registration functions ( in this case) :
//   (1) Recursive registration with intermediate registrations (for contained objects, pointer names...)
//   (2) Recursive registration WITHOUT intermediate registrations (preceding function must ALREADY have been used),

//NOTA: there is no code by default: compile error, except for the useful specialisations, to come in the headers for
// xmlObject<XMLIZED*> and for xmlObject< stl_container<XMLIZED> >

template<uint order, typename XMLIZED, typename derivedCLASS >
struct XMLreg
{
  static bool total_registration ( std::list<std::string> registerNames , xmlObject<XMLIZED> * xmlobj );
  static bool last_step_registration ( std::string registerName , xmlObject<XMLIZED> * xmlobj );
};

// Similarly for registration of initialization functions, to be used only *after* XMLreg

template<uint order, typename XMLIZED, typename baseCLASS >
struct XMLreginit
{
  static bool stat( std::string registerName, xmlObject<XMLIZED> * xmlobj,
                    void (*func)(baseCLASS*), bool afterChildren );
                    
  template<typename REFCLASS>
  static bool memb( std::string registerName, xmlObject<XMLIZED> * xmlobj,
                    void (REFCLASS::*func)(baseCLASS*), REFCLASS &instance, bool afterChildren );
                    
  static bool self( std::string registerName, xmlObject<XMLIZED> * xmlobj,
                    void (baseCLASS::*func)(), bool afterChildren );
                    
  template<typename ARGCLASS>
  static bool selfarg( std::string registerName, xmlObject<XMLIZED> * xmlobj,
                    void (baseCLASS::*func)(ARGCLASS), ARGCLASS argument, bool afterChildren );
};

// XMLNAME FUNCTION :

// 'Does this object have a name somewhere in my container hierarchy ?'

// Basic xmlObjects always respond 'NO' !!
// Pointer and container specializations return 'true' if object's type is handled
// somewhere in their hierarchy, and they put the corresponding name in 'retname'.

// Definition, for 'normal' xmlObjects
template<typename XMLIZED , typename XMLIZED_2>
struct XMLname
{
  static bool findname( XMLIZED_2 * object , xmlObject<XMLIZED> * xmlobj , std::string & retname)
  {
    return false;
  }
};


#endif

