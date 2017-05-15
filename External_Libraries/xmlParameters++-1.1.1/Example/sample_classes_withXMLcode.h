#ifndef SAMPLE_XML_CLASSES_H
#define SAMPLE_XML_CLASSES_H

#include<cmath>
#include<cstdlib>
#include<vector>
#include<string>
#include<cstring>
#include<sstream>
#include<iostream>

// AND THE NEW INCLUDE:
#include<xmlparameters.h> 

// A number of advanced topics are also illustrated. 
// To activate the corresponding code and observe the changes:
// #define ADVANCED_TOPICS


//****************** Here are the 'people' classes **************************
//***************************************************************************

// To be serializable to XML, a class must inherit from xmlParam::Xmlizable
class Person : virtual public xmlParam::Xmlizable
{
protected:
  // Parameters
  int age;
  std::string name, content;

  // Other variables...
  std::string presentationPhrase;

  // ...and the possibility to initialize them from the set of parameters:
  virtual void make_presentationPhrase()
  {
    std::ostringstream ost;
    ost<<"Hi, my name is "<<name<<" and I am "<<age<<" years old.";
    presentationPhrase = ost.str();
  }
  
  
public:  
  Person() : age(0) , name(""), presentationPhrase(""), content("") {}
  virtual ~Person(){} // virtual class needs virtual destructor :)

  //**********************************************
  //This is the ' XMLIZE ' part you have to add:
  const std::type_info & get_pointer_typeid ()
  {
    return typeid ( this ) ;
  }

  void xmlize()
  {
    xmlParam::Xmlizable::add_param(age,"age");
    // or more simply, given the inheritance:
    add_param(name,"name");
  }
  
  // Optionally, you can override this initialization function,
  // which is applied right after the parameters are loaded from XML.
  void xmlinit()
  {
    make_presentationPhrase();
  }
  //**********************************************
  
  
  #ifdef ADVANCED_TOPICS
  // Possible additional *custom initalization* function
  // ( see usage in Bus::xmlize() ).
  void some_custom_init(std::string *bus_number)
  {
    presentationPhrase+=" RIGHT NOW IN BUS "+*bus_number+" !?";
  }
  #endif
  
  
  // The rest of the class is unchanged...
  
  void say_Hi(void)
  {
    std::cout<<presentationPhrase<<std::endl;
  }
  
  
  void generate()
  {
    age=(int)floor((double)rand()/RAND_MAX*100);
    name="";
    int nLetters=1+(int)floor((double)rand()/RAND_MAX*10);
    for(int i=0;i<nLetters;++i)
    {
      name.push_back((char)(97+floor((double)rand()/RAND_MAX*26))); //random letter between a and z
    }
    make_presentationPhrase();
  }
  
  
  void set_content(std::string cont)
  {
    content=cont;
  }
};


class NiceGuy: public Person
{
protected:
  void make_presentationPhrase()
  {
    Person::make_presentationPhrase();
    presentationPhrase+=" How are you? Care for "+content+"?";
  }

public:
  NiceGuy() : Person() {}
  ~NiceGuy() {}

  //**********************************************
  //This is the ' XMLIZE ' part you have to add:

  //const type_info & get_pointer_typeid ()
  //{
  //   return typeid ( this ) ;
  //}

  //void xmlize()

  //or you can just write:
  XMLIZE_MACRO
  {
    Person::xmlize();
    add_param(content,"what-I-offer");
  }
  //**********************************************
};


class RudeGuy: public Person
{
protected:
  void make_presentationPhrase()
  {
    Person::make_presentationPhrase();
    presentationPhrase+=" Now, why don't you "+content+"?";
  }

public:
  RudeGuy() : Person(){}
  ~RudeGuy(){}

  // The XMLIZE macro also works as a simple declaration...
  XMLIZE_MACRO;
};

//*****************************************************
// ...in which case the corresponding definition goes:
void RudeGuy::xmlize()
{
  Person::xmlize();
  add_param(content,"my-insult");
}
//*****************************************************



//******************* Here are the 'animal' classes *********************
//***********************************************************************

struct Animal : virtual public xmlParam::Xmlizable
{
  // char arrays can also be loaded from XML. The XML load does NO MEMORY
  // ALLOCATION nor FREEING. Thus, the char arrays must have been
  //  pre-allocated with sufficient memory.
  char *my_yell; 
  Animal(const char *yell="")
  {
    my_yell=new char[300]; // pre-determine the maximum length as 300.
    strcpy(my_yell,yell);
  }
  virtual ~Animal(){}
  void say_Hi() {
    printf("%s \n",my_yell);
  }
};

struct Dog : public Animal
{
  Dog(const char *yell="") : Animal(yell) {}
  ~Dog(){}

  //**********************************************
  //This is the ' XMLIZE ' part you have to add:
  XMLIZE_MACRO
  {
    add_param(my_yell,"bark");
  }
  //**********************************************
};

// Note that Cat is a VIRTUAL Animal.
// (this led to errors when xmlizing, in version 1.0)
struct Cat : virtual public Animal
{
  Cat(const char *yell="") : Animal(yell) {}
  ~Cat(){}

  //**********************************************
  //This is the ' XMLIZE ' part you have to add:
  XMLIZE_MACRO
  {
    add_param(my_yell,"mew");
  }
  //**********************************************
};


  
//********************** And here is the bus ! **************************
//***********************************************************************

struct Bus : virtual public xmlParam::Xmlizable
{
  int capacity;
  std::string reg_number;
  
  NiceGuy myself; // Of course!
  Person *driver; // driver can be there... or not.
  std::vector<Person*> people;
  // an animal trailer can be there... or not.
  std::vector<Animal*> *animal_trailer;

  
public:
  Bus() : capacity(4), driver(0), animal_trailer(0) {}
  // Make sure that all uninitialized pointers loadable from XML are explicitly set at 0. 
  // Otherwise, you will get warnings.
  ~Bus(){}

  //**********************************************
  //This is the ' XMLIZE ' part you have to add:
  XMLIZE_MACRO
  {
    add_param(capacity,"capacity");
    add_param(reg_number,"registration-number");

    xmlParam::Xmlizable::add_child(myself, "me");

    add_child(driver, "driver");
    xmlParam::Register<1,RudeGuy>(driver,"unpolite");
    xmlParam::Register<1,NiceGuy>(driver,"polite");

    add_child(people, "people");
    xmlParam::Register<2,Person>(people, xmlParam::names("","shy-guy") );
    xmlParam::Register<2,NiceGuy>(people,"nice-guy");
    xmlParam::Register<2,RudeGuy>(people,"rude-guy");

    add_child(animal_trailer, "");
    xmlParam::Register<3,Dog>(animal_trailer,xmlParam::names("animal-trailer","","dog") );
    xmlParam::Register<3,Cat>(animal_trailer, "cat" );
    
    #ifdef ADVANCED_TOPICS
    // Adding supplementary 'custom' initializations to an object.
    // Custom functions are applied after the corresponding 'xmlinit_xxx()' function.
    // See xmlparameters.h for more details.
    xmlParam::add_init(myself,&Person::some_custom_init,&(this->reg_number));
    xmlParam::Register_init<2>(people,"rude-guy",&Person::some_custom_init,&(this->reg_number));
    #endif
  }
  //**********************************************


  // The rest of the code is unchanged:
  
  void everybody_say_Hi(void)
  {
    std::cout<<"-myself : "<<std::endl;
    myself.say_Hi();
    if(driver)
    {
      std::cout<<"-driver : "<<std::endl;
      driver->say_Hi();
    }
    for(uint i=0;i<people.size();++i)
    {
      std::cout<<"-passenger number "<<i+1<<" : "<<std::endl;
      people[i]->say_Hi();
    }
    if(driver)
      if((int)(1+people.size()) > capacity)
        std::cout<<"-driver : "<<std::endl<<"There are too many of you. Rude guys, get off!!!"<<std::endl;

    int count=0;
    if(animal_trailer)
      for(uint i=0;i<animal_trailer->size();++i)
      {
        std::cout<<"-animal number "<<++count<<" : "<<std::endl;
        (*animal_trailer)[i]->say_Hi();
      }
  }

  void empty()
  {
    delete driver;
    driver=0; // all uninitialized pointers loadable from XML must be set at 0. 
    for(uint i=0;i<people.size(); ++i)
      delete people[i];
    people.clear();
    if(animal_trailer)
    {
      for(uint i=0;i<animal_trailer->size(); ++i)
        delete (*animal_trailer)[i];
      animal_trailer->clear();
      delete animal_trailer;
      animal_trailer=0; // idem
    }
  }

  void generate()
  {
    // myself
    myself.set_content("a little shot of vodka");
    myself.generate();

    // driver: sometimes here, sometimes not
    if((double)rand()/RAND_MAX<0.5)
    {
      driver = new RudeGuy();
      driver->set_content("pay me your ticket, you little swine");
      driver->generate();
    }
    else driver=0;

    // other passengers: nice guys, rude guys, and shy guys
    int nPeople=(int)floor((double)rand()/RAND_MAX*6);
    for(int i=0;i<nPeople;++i)
    {
      double rnd=(double)rand()/RAND_MAX;
      if(rnd<0.33)
        people.push_back(new Person());
      else if(rnd<0.66)
      {
        people.push_back(new NiceGuy());
        people.back()->set_content("some appetizers with the vodka");
      }
      else
      {
        people.push_back(new RudeGuy());
        people.back()->set_content("take that glass of vodka");
      }
      people[i]->generate();
    }

    // animals : 0 (no trailer) or 2.
    if((double)rand()/RAND_MAX<0.5)
    {
      animal_trailer= new std::vector<Animal*>;
      animal_trailer->push_back(new Dog("RwaAAF! (vodka!)"));
      animal_trailer->push_back(new Cat("Sniff... (pityful humans...)"));
    }
  }
};

#endif



