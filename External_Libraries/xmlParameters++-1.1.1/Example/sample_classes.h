#ifndef SAMPLE_XML_CLASSES_H
#define SAMPLE_XML_CLASSES_H

#include<cmath>
#include<cstdlib>
#include<vector>
#include<string>
#include<cstring>
#include<sstream>
#include<iostream>


//****************** Here are the 'people' classes **************************
//***************************************************************************

class Person
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
};



//******************* Here are the 'animal' classes *********************
//***********************************************************************

struct Animal
{
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
};

// Note that Cat is a VIRTUAL Animal.
struct Cat : virtual public Animal
{
  Cat(const char *yell="") : Animal(yell) {}
  ~Cat(){}
};

  
//********************** And here is the bus ! **************************
//***********************************************************************

struct Bus
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
  ~Bus(){}
  
  
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
    driver=0;
    for(uint i=0;i<people.size(); ++i)
      delete people[i];
    people.clear();
    if(animal_trailer)
    {
      for(uint i=0;i<animal_trailer->size(); ++i)
        delete (*animal_trailer)[i];
      animal_trailer->clear();
      delete animal_trailer;
      animal_trailer=0;
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



