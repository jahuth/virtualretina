
#ifndef XML_PARAMETERS_H
#define XML_PARAMETERS_H


// *****************************************************************************
// ***********************     xmlParameters++     *****************************
// *****************************************************************************

// This is the header you must include in all code using library xmlParameters++
// *****************************************************************************
// *****************************************************************************

#include<vector>
#include<list>
#include<deque>
#include<map>
#include<string>
#include<sstream>
#include<iostream>
#include<libxml/tree.h>      // (works provided you compile with -I path/to/libxml2_source_directory)
#include<libxml/parser.h>
#include<typeinfo>

// All functions and classes defined hereafter belong to namespace xmlParam:
namespace xmlParam
{

// We must include this header first, which handles custom initializations.
#include "initfunctor.h"

//Here are all functions you will need to use:



//***************************************************************************
//*************       (I) Base class Xmlizable         **********************
//***************************************************************************


// Whenever you wish to define how an object class loads/saves itself from an
// xml file, it must inherit from class xmlParam::Xmlizable (preferably 
// through virtual inheritance)

struct Xmlizable
{
  // The important function you must redefine in the code of each derived 
  // object, to define its associated xml structure:
  virtual void xmlize(){}

  // Sometimes useful, override these to define a built-in initialization 
  // function for the class, right after loading its parameters from XML:
  virtual void xmlinit(){}
  virtual void xmlinit_after_children(){}
  // (nota: 'custom' initializations, presented afterwards, are a heavier
  //  but more powerful alternative)
  
  
  // XML structure creation and manipulation
  //****************************************
  
  // (Generally these functions are used inside the code of function xmlize() )

  // Adding a parameter (base type or string) as xml attribute of this object:
  template<typename TYPE>
  void add_param(TYPE & newParam , std::string name);
  
  // Adding a sub-field as child xml node of this object:
  template<typename XMLIZED>
  void add_child(XMLIZED & object , std::string name );

 
  // Miscellaneous
  //**************
  
  // This function must also be redefined in the code of each derived object.
  // But its code is invariant, so you do not have to bother writing it if you
  // use the XMLIZE_MACRO instead (see Example/sample_classes_withXMLcode.h).
  virtual const std::type_info & get_pointer_typeid ()
  {
    return typeid ( this ) ;
  }

  virtual ~Xmlizable();
  
};



//***************************************************************************
//***************       (II) General functions        ***********************
//***************************************************************************

// These functions are called *outside* of xmlize() functions, typically in
// the 'main' of a program.
// 'Custom' functions can be used both in the main *and* in xmlize() functions.


// (1) XML structure creation and manipulation
// *******************************************

// Adding an object in the XML structure
template<typename XMLIZED>
void add_object (XMLIZED & object, std::string name);
 
// Removing an object from the XML structure
template<typename XMLIZED>
void remove_object (XMLIZED & object);

// Delete the whole XML structure
void delete_structure ();


// (2) Saving/Loading
// ******************

// Functions which must be used after the XML structure has been defined,
// thanks to functions xmlize() and/or custom functions.

// LOAD the xmlized architecture/parameters from an xml tree:
// (xmlNodePtr is the 'pointer to node' object of library libxml2)
template<typename XMLIZED>
void load (XMLIZED & object, xmlNodePtr father_node, bool warn_if_unused_nodes=true);

// SAVE the xmlized architecture/parameters to an xml tree:
// (xmlNodePtr is the 'pointer to node' object of library libxml2)
template<typename XMLIZED>
void save (XMLIZED & object, xmlNodePtr father_node);


// (3) Pointer and container registration
// **************************************

// Function "Register()" gives the rules to 'fill' pointers and containers, based on
// the XML file being read (including possible polymorphism for pointers).
// These functions are defined as a structure for conveniency, but called as regular
// functions (see 'USAGE' right afterwards).


// XMLIZED_d = XMLIZED class handled by the CONTAINING object, or possibly a 
//             class *virtually derived* from XMLIZED (polymorphism).
template<uint order, class XMLIZED_d>
struct Register
{
  // Total-chain version: Register a full chain of registration names for an object.
  template<class CONTAINING>
  Register( CONTAINING & object, std::list<std::string> registerNames );

  // Single-name version: Register variants for the last registration step in the chain.
  template<class CONTAINING>
  Register( CONTAINING & object, std::string registerName );
};


// Create a list of strings from a series of string arguments (up to 6 levels of registration)
//   ==> To be used as second argument to the Total-chain version of Register
std::list<std::string> names(std::string str1);
std::list<std::string> names(std::string str1, std::string str2);
std::list<std::string> names(std::string str1, std::string str2, std::string str3);
std::list<std::string> names(std::string str1, std::string str2, std::string str3, std::string str4);
std::list<std::string> names(std::string str1, std::string str2, std::string str3, std::string str4, std::string str5);
std::list<std::string> names(std::string str1, std::string str2, std::string str3, std::string str4, std::string str5, std::string str6);


// ********** USAGE (see Example/README for real illustrations) :

// 'order' is the DEPTH of the registration in the structure. In the total-chain version,
// the list registerNames MUST have exactly 'order' elements. The list can be directly 
// constructed with helper function xmlParam::names().

// To register Toto* a	:	        xmlParam::Register<1,Toto>(a, "name_of_the_toto")
// If "Tata" derives from "Toto",	xmlParam::Register<1,Tata>(a, "name_of_the_tata") also works,
//    and it is possible to register *both* classes Toto and Tata on pointer "a" (polymorphism).

// To register vector<Toto> b	:   xmlParam::Register<1,Toto>(b, "name_of_a_toto")
// To register Toto** c	:       	xmlParam::Register<2,Toto>(c, xmlParam::names("name_of_the_ptr" , "name_of_a_toto") )
//    and then possibly	:       	xmlParam::Register<2,Tata>(c, "name_of_a_tata") )
// To register vector<Toto*> *d	: xmlParam::Register<3,Tata>(d, xmlParam::names("name_of_the_vector" , "name_of_a_ptr" , "name_of_a_tata") )
//    and then possibly :         xmlParam::Register<3,Toto>(d, "name_of_a_toto")


// (4) Retrieving XML names
// ************************

// Retrieve the xml name associated to an object.
// These functions are not necessary to the main purpose of the library (loading/saving from xml).
// See README (section "retrieving an object's xml name") for details.

template<typename XMLIZED>
std::string get_xmlName (XMLIZED & object);

template<typename XMLIZED, typename XMLIZEDref>
std::string handled_xmlName (XMLIZED & object, XMLIZEDref & refobject );

template<typename XMLIZED>
std::string last_xmlName (XMLIZED & object);


// (5) 'Custom' functions
// **********************

// The equivalent of 'Xmlizable' member functions add_child() and add_param(),
// to apply when fatherObject is a POINTER or CONTAINER (meaning that it does not
// derive from Xmlizable, but rather 'points' to Xmlizable objects).
// In 'normal' conditions, you need not use these functions.

template<typename XMLIZED_1, typename XMLIZED_2>
void add_child_custom(XMLIZED_1 & childObject , std::string name, XMLIZED_2 & fatherObject);

template < typename TYPE , typename XMLIZED >
void add_param_custom(TYPE & newParam, std::string paramName, XMLIZED &fatherObject);


// Add custom initialization function(s) to an object, to be applied after its 
// parameters have been loaded from XML. You can attach as many initializations
// functions as you want, to any object.
// It is often simpler to override the functions Xmlizable::xml_init_before/after().
// Type XMLIZED_b = XMLIZED, or base class of XMLIZED, such that 'func(&object)' is a valid
//                  call to initialize 'object' after its parameters are loaded from XML.
// See initfunctor.h for more details.

// (a) Initialize using a static (C-type) function:
template<typename XMLIZED, typename XMLIZED_b>
void add_init (XMLIZED & object, void (*func)(XMLIZED_b*), bool afterChildren=false);

// (b) Initialize using a class member function from a particular instance of that class (C++):
template<typename XMLIZED, typename XMLIZED_b, typename REFCLASS>
void add_init (XMLIZED & object, void (REFCLASS::*func)(XMLIZED_b*), REFCLASS & instance, bool afterChildren=false);

// (c) Initialize using a class member function from class XMLIZED_b itself:
template<typename XMLIZED, typename XMLIZED_b>
void add_init (XMLIZED & object, void (XMLIZED_b::*func)(), bool afterChildren=false);

// (d) Initialize using a class member function from class XMLIZED_b itself, with an argument:
template<typename XMLIZED, typename XMLIZED_b, typename ARGCLASS>
void add_init (XMLIZED & object, void (XMLIZED_b::*func)(ARGCLASS), ARGCLASS argument, bool afterChildren=false);


// **Register** initialization functions, for objects that *will* be created from
// the XML load. It must be called after the string "registerName" has been registered 
// thanks to function "Register()".
// XMLIZED_b = XMLIZED class handled by the CONTAINING object, or a 'related' class 
//             (base class, virtually derived class) such that function "func" can be
//             called on the XMLIZED object created after the XML load.

template<uint order>
struct Register_init
{
  // (a) Initialize using a static (C-type) function:
  template<typename CONTAINING, typename XMLIZED_b>
  Register_init(CONTAINING & object, std::string registerName, 
                 void (*func)(XMLIZED_b*), bool afterChildren=false);

  // (b) Initialize using a class member function from a particular instance of that class (C++):
  template<typename CONTAINING, typename XMLIZED_b, typename REFCLASS>
  Register_init(CONTAINING & object, std::string registerName,
                 void (REFCLASS::*func)(XMLIZED_b*), REFCLASS & instance, bool afterChildren=false);

  // (c) Initialize using a class member function from class XMLIZED_b itself:
  template<typename CONTAINING, typename XMLIZED_b>
  Register_init(CONTAINING & object, std::string registerName,
                 void (XMLIZED_b::*func)(), bool afterChildren=false);
                 
  // (d) Initialize using a class member function from class XMLIZED_b itself, with an argument:
  template<typename CONTAINING, typename XMLIZED_b, typename ARGCLASS>
  Register_init(CONTAINING & object, std::string registerName,
                 void (XMLIZED_b::*func)(ARGCLASS), ARGCLASS argument, bool afterChildren=false);
};



//***************************************************************************
//*****************       (III) A useful macro        ***********************
//***************************************************************************

// Can be written in class headers, instead of the *declaration* of function xmlize.
// GOAL: To regroup the declaration of function xmlize, and the invariant definition 
// of function get_pointer_type_info (see (I) xmlParam::Xmlizable ).

#define XMLIZE_MACRO		\
const std::type_info & get_pointer_typeid ()	\
{		\
return typeid (this) ;		\
}	\
\
void xmlize()

// USAGE (see Example/README for real illustrations) :

/* (a) Instead of the declaration/definition of function xmlize() :

class Toto : virtual public xmlParam::Xmlizable
{
  ...
  XMLIZE_MACRO
  {
	  ...all xmlize code...
  }
};

OR (b) Instead of the simple declaration of function xmlize()

class Toto : virtual public xmlParam::Xmlizable
{
  ...
  XMLIZE_MACRO;
};

void Toto::xmlize()
{
	...all xmlize code...
}
*/



//*********************************************************************
//*********************************************************************
//**************     Now here comes the code!       *******************
//*********************************************************************
//*********************************************************************

#include"xmlobject_base.h"
#include"xmlobject.h"
// Template specializations of xmlObject for pointers, and for stl containers:
#include"xmlobject_pointer_specialisation.h"
#include"xmlobject_container_specialisation.h"

//the code for classes defined in this header:
#include"xmlparameters_template_code.h"	

} // namespace xmlParam

#endif

