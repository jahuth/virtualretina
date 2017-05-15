#ifndef INIT_FUNCTOR
#define INIT_FUNCTOR

// Template function "InitFunctor<XMLIZED>::create()" creates
// FUNCTORS ("generalized function") of base type InitFunctor<XMLIZED>
// that can initialize an object of type XMLIZED, after its parameters have 
// been loaded from XML.
// The goal of using a functor is to gain flexibility concerning the nature
// and signature of the underlying function: It can either be static (C-type)
// or a class member function (C++-type). Furthermore, it allows to use an 
// initialization function which takes as argument a "sibling" class of XMLIZED
// (base class or even possibly a virtual derivate) rather than XMLIZED itself.
 
// Once constructed, an InitFunctor<XMLIZED> "funk" allows to initialize any
// XMLIZED "obj", by calling "funk(obj)".

// Each xmlObject<XMLIZED> has a pointer to an InitFunctor<XMLIZED>.
// If present, it is applied AFTER the object has been loaded from XML,
// and BEFORE the object's children are in turn loaded from XML.


// The InitFunctor<XMLIZED> object :
// *******************************

// Non-template base type, for conveniency of signature in function Register().
struct InitFunctor_base  
{
  virtual ~InitFunctor_base(){}
};

template<typename XMLIZED>
struct InitFunctor : public InitFunctor_base
{ 
  virtual void operator()(XMLIZED & toInitialize)=0; // (role of the functor).
  
  // Functions to create an InitFunctor:
  // XMLIZED_b must be a *base class* of XMLIZED (including XMLIZED itself),
  // or a class virtually derived from XMLIZED (use with caution!)

  // (a) Construction from a static (C-type) function
  template<typename XMLIZED_b>
  static InitFunctor<XMLIZED>* create(void (*func)(XMLIZED_b*));

  // (b) Template construction from a class member function (C++-type)
  //     called from  particular instance of that class.
  template<typename XMLIZED_b, typename REFCLASS>
  static InitFunctor<XMLIZED>* create(void (REFCLASS::*func)(XMLIZED_b*),
                                       REFCLASS & instance);
  
  // (c) Template construction from a class member function (C++-type)
  //     of XMLIZE itself (or 'related' class XMLIZE_b)
  template<typename XMLIZED_b>
  static InitFunctor<XMLIZED>* create(void (XMLIZED_b::*func)());
  
  // (d) Template construction from a class member function (C++-type)
  //     of XMLIZE itself (or 'related' class XMLIZE_b), with an argument.
  template<typename XMLIZED_b, typename ARGCLASS>
  static InitFunctor<XMLIZED>* create(void (XMLIZED_b::*func)(ARGCLASS),
                                       ARGCLASS argument);
  
  // Duplication function (returns a new instance, with similar properties as "this")
  virtual InitFunctor<XMLIZED>* copy()=0;
};


// Now here goes the implementation
// *********************************

/* USELESS for the moment
// Casting back from InitFunctor_base to the typed version:
template<typename XMLIZED>
InitFunctor<XMLIZED>* InitFunctor<XMLIZED>::castback(InitFunctor_base *ifb)
{
  InitFunctor<XMLIZED>* ifTyped=dynamic_cast<InitFunctor<XMLIZED>*>(ifb);
  if( (bool)ifb & !(bool)ifTyped )
  {  // an initialization was specified, but with incorrect type.
    std::cerr<<"ERROR using xmlParam::InitFunctor: "
             <<"This InitFunctor does not have the correct template argument."<<std::endl;
    exit(0);
  }
  return ifTyped;
}*/

// Specialization of InitFunctor wrapping  a static (C-type) function.
template<typename XMLIZED, typename XMLIZED_b>
struct InitFunctor_stat : public InitFunctor<XMLIZED>
{
  void (*refInitFunc)(XMLIZED_b*);
  virtual void operator()(XMLIZED & toInitialize)
  { 
    XMLIZED_b *toInitCasted=dynamic_cast<XMLIZED_b*>(&toInitialize);
    if(toInitCasted)
      (*refInitFunc)(toInitCasted);
    else
      std::cerr<<"WARNING from xmlParam::InitFunctor: "
             <<"This class does not have the required virtually derived type, "
             "Cannot use initialization function."<<std::endl;
  }
  virtual InitFunctor<XMLIZED> *copy()
  {
    return this->create(refInitFunc);
  }
};

// Specialization of InitFunctor wrapping an external class member function.
template<typename XMLIZED, typename XMLIZED_b, typename REFCLASS>
struct InitFunctor_memb : public InitFunctor<XMLIZED>
{
  REFCLASS *refObj;
  void (REFCLASS::*refInitFunc)(XMLIZED_b*);
  virtual void operator()(XMLIZED & toInitialize)
  {
    XMLIZED_b *toInitCasted=dynamic_cast<XMLIZED_b*>(&toInitialize);
    if(toInitCasted)
      (refObj->*refInitFunc)(toInitCasted);
    else
      std::cerr<<"WARNING from xmlParam::InitFunctor: "
             <<"This class does not have the required virtually derived type, "
             "Cannot use initialization function."<<std::endl;
  }
  virtual InitFunctor<XMLIZED> *copy()
  {
    return this->create(refInitFunc,*refObj);
  }
};

// Specialization of InitFunctor wrapping a 'self' class member function.
template<typename XMLIZED, typename XMLIZED_b>
struct InitFunctor_self : public InitFunctor<XMLIZED>
{
  void (XMLIZED_b::*refInitFunc)();
  virtual void operator()(XMLIZED & toInitialize)
  {
    XMLIZED_b *toInitCasted=dynamic_cast<XMLIZED_b*>(&toInitialize);
    if(toInitCasted)
      (toInitCasted->*refInitFunc)();
    else
      std::cerr<<"WARNING from xmlParam::InitFunctor: "
             <<"This class does not have the required virtually derived type, "
             "Cannot use initialization function."<<std::endl;
  }
  virtual InitFunctor<XMLIZED> *copy()
  {
    return this->create(refInitFunc);
  }
};
  
// Specialization of InitFunctor wrapping a 'self' class member function with argument.
template<typename XMLIZED, typename XMLIZED_b, typename ARGCLASS>
struct InitFunctor_selfarg : public InitFunctor<XMLIZED>
{
  void (XMLIZED_b::*refInitFunc)(ARGCLASS);
  ARGCLASS argVal;
  virtual void operator()(XMLIZED & toInitialize)
  {
    XMLIZED_b *toInitCasted=dynamic_cast<XMLIZED_b*>(&toInitialize);
    if(toInitCasted)
      (toInitCasted->*refInitFunc)(argVal);
    else
      std::cerr<<"WARNING from xmlParam::InitFunctor: "
             <<"This class does not have the required virtually derived type, "
             "Cannot use initialization function."<<std::endl;
  }
  virtual InitFunctor<XMLIZED> *copy()
  {
    return this->create(refInitFunc,argVal);
  }
};
  
// Function "create" for static function
template<typename XMLIZED>
template<typename XMLIZED_b>
InitFunctor<XMLIZED>* InitFunctor<XMLIZED>::create( void (*func)(XMLIZED_b*) )
{
  InitFunctor_stat<XMLIZED,XMLIZED_b> *iF
    =new InitFunctor_stat<XMLIZED,XMLIZED_b>();
  iF->refInitFunc=func;
  return iF;
}

// Function "create" for external class member function
template<typename XMLIZED>
template<typename XMLIZED_b, typename REFCLASS>
InitFunctor<XMLIZED>* InitFunctor<XMLIZED>::create( void (REFCLASS::*func)(XMLIZED_b*), REFCLASS &instance )
{
  InitFunctor_memb<XMLIZED,XMLIZED_b,REFCLASS> *iF
    =new InitFunctor_memb<XMLIZED,XMLIZED_b,REFCLASS>();
  iF->refInitFunc=func;
  iF->refObj=&instance;
  return iF;
}

// Function "create" for self class member function
template<typename XMLIZED>
template<typename XMLIZED_b>
InitFunctor<XMLIZED>* InitFunctor<XMLIZED>::create( void (XMLIZED_b::*func)() )
{
  InitFunctor_self<XMLIZED,XMLIZED_b> *iF
    =new InitFunctor_self<XMLIZED,XMLIZED_b>();
  iF->refInitFunc=func;
  return iF;
}

// Function "create" for self class member function with argument
template<typename XMLIZED>
template<typename XMLIZED_b, typename ARGCLASS>
InitFunctor<XMLIZED>* InitFunctor<XMLIZED>::create( void (XMLIZED_b::*func)(ARGCLASS), ARGCLASS argument )
{
  InitFunctor_selfarg<XMLIZED,XMLIZED_b,ARGCLASS> *iF
    =new InitFunctor_selfarg<XMLIZED,XMLIZED_b,ARGCLASS>();
  iF->refInitFunc=func;
  iF->argVal=argument;
  return iF;
}

#endif
