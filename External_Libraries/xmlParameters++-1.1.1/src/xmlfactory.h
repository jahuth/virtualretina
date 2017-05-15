#ifndef XML_FACTORY_H
#define XML_FACTORY_H


//Inspired by an object factory by Robert Geiman : http://www.gamedev.net/reference/articles/article2097.asp
//(listing 'number3')

//-Added possibilities:

//- An obligation to have only ONE std::string for each derived_type entered in the factory.
//- A reflexive map giving the std::string associated to each registered derived_type 
//  entered in the factory.
//- For the factory to fully work, the types it handles must inherit from class xmlClass.
//  This enables the xmlParameters saving function to find back the xml_name of an object 
//   when saving it, even if the object was not created by the factory.


  
template<typename XMLIZED> class xmlFactory;


// The structure for object creation, handled by the factory's registerMap:
template<typename XMLIZED>
struct CreateStruct
{

public:
  XMLIZED* Create() const
  {
    return creator();
  }
  
  std::vector<InitFunctor<XMLIZED>*> customInit_before() const
  {
    return ifb;
  }
  
  std::vector<InitFunctor<XMLIZED>*> customInit_after() const
  {
    return ifa;
  }
  
protected:
  XMLIZED* (*creator)(void); // the cool trick :|
  std::vector<InitFunctor<XMLIZED>*> ifb, ifa;
  const std::type_info *ti;
  friend class xmlFactory<XMLIZED>;
  
  // Only xmlFactory can create or destroy CreateStructs.
  CreateStruct():creator(0), ti(0) 
  {}
  // At destruction, the factory is in charge of its handled InitFunctors
  // (so, InitFunctors should not be shared between xmlObjects) :
  ~CreateStruct()
  {
    for(int i=0; i<ifb.size();++i)
      delete ifb[i];
    for(int i=0; i<ifa.size();++i)
      delete ifa[i];
  }
  
private:
  CreateStruct(CreateStruct & cs) // non-copyable 
  {}
};

  
template<typename XMLIZED>
class xmlFactory
{
protected:  
  
  // registration maps:
  std::map< std::string , CreateStruct<XMLIZED>* > registerMap;
  std::map< const std::type_info * , std::string > type_info_reverse;
  typedef typename std::map<std::string,CreateStruct<XMLIZED>*>::iterator regIter;
  
  // template creation function
  template<typename derivedCLASS>
  static XMLIZED* CreateObject()
  {
    return new derivedCLASS();
  }
  
  // Variable allowing the factory to be SHARED by different xmlObject<XMLIZED*> objects.
  int sharedBy;

public:

  // Functions allowing the factory to be SHARED by different xmlObject<XMLIZED*> objects
  
  static void assign(xmlFactory<XMLIZED> * & newFact, xmlFactory<XMLIZED> * const & oldFact)
  {
   newFact=oldFact;
    if(oldFact)
      oldFact->sharedBy += 1;
  }

  static void remove_occurence(xmlFactory<XMLIZED> * & theFact)
  {
    if(theFact)
      if(theFact->sharedBy==1) 
      {
        delete theFact;
        theFact=0;
      }
      else theFact->sharedBy-=1 ;
  }

  xmlFactory() : sharedBy(1) {}
  
  ~xmlFactory() // delete all registered CreateStructs
  {
    for(regIter iter=registerMap.begin();iter!=registerMap.end();++iter)
      delete (*iter).second;
  } 


  // Now come the real registration functions:
  
  template<typename derivedCLASS>
  bool _Register(std::string unique_id)
  {
    if (registerMap.find(unique_id) != registerMap.end())
      return false;

    registerMap[unique_id]=new CreateStruct<XMLIZED>();
    registerMap[unique_id]->creator= &CreateObject<derivedCLASS>;
    
    // We store the type info of a POINTER on the derivedCLASS, so as not to create any useless object!
    derivedCLASS * fakePoint;
    registerMap[unique_id]->ti = & typeid(fakePoint) ;
    type_info_reverse[ & typeid(fakePoint)] = unique_id ;
    return true;
  }


  bool _RegisterInit(std::string unique_id, InitFunctor<XMLIZED> *customInit, bool afterChildren)
  {
    if (registerMap.find(unique_id) == registerMap.end())
      return false;
    if (afterChildren)
      registerMap[unique_id]->ifa.push_back(customInit);
    else
      registerMap[unique_id]->ifb.push_back(customInit);
    return true;
  }


  const CreateStruct<XMLIZED>* Find(std::string unique_id)
  {
    regIter iter = registerMap.find(unique_id);
    if (iter == registerMap.end())
      return NULL;
    return ( (*iter).second );
  }
  
  
  bool GetId (XMLIZED * objAddress, std::string & namestr)
  {
    if(objAddress)
    {
      //For this function to do its job , XMLIZED must inherit from the class xmlClass !!
      const std::type_info * ti = & xmlObject<XMLIZED>::pointer_typeid( objAddress );

      typename std::map< const std::type_info * , std::string >::iterator iter = type_info_reverse.find( ti );
      if (iter != type_info_reverse.end())
      {
        namestr=((*iter).second);
        return true;
      }
    }
    return false;
  }


// Plus unused functions:  
  
  bool Unregister(std::string unique_id)
  {
    regIter iter = registerMap.find(unique_id);
    if (iter == registerMap.end())
      return NULL;
    type_info_reverse.erase( (*iter).second->ti );
    delete (*iter).second ;
    registerMap.erase( iter ); // TODO:CHECK
    return true;
  }


  XMLIZED* Create(std::string unique_id)
  {
    regIter iter = registerMap.find(unique_id);
    if (iter == registerMap.end())
      return NULL;
    return ((*iter).second->creator());
  }


  InitFunctor<XMLIZED>* GetCustomInit(std::string unique_id, bool afterChildren)
  {
    regIter iter = registerMap.find(unique_id);
    if (iter == registerMap.end())
      return NULL;
    if (afterChildren)
      return ((*iter).second->ifa);
    else
      return ((*iter).second->ifb);
  }
  
};

#endif


