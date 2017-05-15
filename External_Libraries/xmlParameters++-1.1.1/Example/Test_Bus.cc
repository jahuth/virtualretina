

// This code, and the corresponding class definitions in "sample_classes_withXMLcode.h",
// illustrate the basic usage of xmlParameters++.
// Some 'advanced topics' are also illustrated through bits of commented code.
// You can uncomment those bits and see their effects when re-compiling the program.

// ACTION OF THE PROGRAM:
// 1) Load the content of a C++ object 'Bus' from the XML file 'bus-definition.xml'
// 2) Check that the 'Bus' object has been well defined, by printing its content.
// 3) Save the 'Bus' to an output XML file 'reconstructed-bus-file.xml'

// Launching "Test_Bus -g" (for 'generate') randomly regenerates the bus between 
// the original load (stage 1) and the printing of its content (stage 2).
// This allows to verify that the 'save_XML' function works even without having
// created the object with load_XML.


#include"sample_classes_withXMLcode.h" // The 'XML-modified' classes used by this program.
#include<cstring>

using namespace std;

int main(int argc, char **argv)
{

  //******************* The -g option:
  bool generate=false;
  for (int k=0; k<argc; k++)
    if(!strcmp(argv[k],"-g"))
      generate=true;


  //******************* Bus creation :
  Bus theBus;		
  xmlParam::add_object(theBus, "bus");

  #ifdef ADVANCED_TOPICS
  // Custom creation/modification of the XML structure.
  // Any parameter can be added anywhere in the xml structure, including
  // on 'pointer' nodes. Example:
  int a=5;	
  xmlParam::add_param_custom(a, "value-of-a", theBus.driver); 
  // (See the effect in "reconstructed-bus-file.xml")
  #endif


  //******************* (1) Bus loading procedure :

  // Open and parse the XML file with library libxml2
  xmlDocPtr doc;
  xmlNodePtr root_node;
  doc=xmlParseFile("bus_definition.xml");
  if (doc == NULL ) {
    fprintf(stderr,"Document not parsed successfully. \n");
    return 1;
  }
  root_node=xmlDocGetRootElement(doc);
  if (root_node == NULL) {
    fprintf(stderr,"empty document\n");
    xmlFreeDoc(doc);
    return 2;
  }

  // LOAD the bus from the XML document with xmlParameters++
  cout<<endl<<"Loading the bus content from file 'bus_definition.xml'"<<endl;
  
  #ifdef ADVANCED_TOPICS
  // You can remove the warnings associated to unused nodes during the load:
  xmlParam::load (theBus,root_node,false);
  #else
  // DEFAULT LOAD : (produces warnings)
  xmlParam::load (theBus,root_node);
  #endif
  
  #ifdef ADVANCED_TOPICS
  // Accessing the XML names that were used.
  // Note the difference between 
  // *user-included* variables ( easy access with 'get_xmlName' )
  // *loaded* variables (  - harder access with 'handled_xmlNames'
  //                       - error-prone access with 'last_xmlNames' )
  cout<<endl<<"Some xmlNames encountered during loading:"<<endl;
  cout<< xmlParam::get_xmlName(theBus.driver) <<endl;
  cout<< xmlParam::handled_xmlName(*(theBus.people[0]),theBus.people)<<endl;
  cout<< xmlParam::last_xmlName(*(theBus.animal_trailer->at(0)))<<endl;
  #endif
  
  if(generate)
  {
    cout<<endl<<"Randomly re-generating the bus."<<endl;
    // Then we forget about the bus we had just loaded, and generate a new one.
    // Note the added difficulty for the forthcoming 'xmlParam::save' procedure :
    // Through the previous call to function 'xmlParam::load', we have already
    // done some "work" on our underlying xmlParameters objects, but that "work"
    // is mostly irrelevant now that the Bus has been re-generated from scratch.
    srand( (unsigned)time( NULL ) );
    theBus.empty();
    theBus.generate();
  }
  
  
  //********************* (2) Bus action:
  
  cout<<endl<<"Everybody in the bus, say hi:"<<endl;
  theBus.everybody_say_Hi();


  //********************* (3) Bus saving procedure :

  // free the old libxml2 tree structure and create a new one
  xmlFreeDoc(doc); 
  doc = xmlNewDoc((const xmlChar*)"1.0");
  root_node = xmlNewNode(NULL, (const xmlChar*)"bus-description-file");
  xmlDocSetRootElement(doc, root_node);

  // Save to the XML document using xmlParameters++
  cout<<endl<<"Saving the bus content to file 'reconstructed_bus_file.xml'"<<endl;
  xmlParam::save (theBus,root_node);
  
  // Save the XML document to file using libxml2
  xmlSaveFormatFile("reconstructed_bus_file.xml",doc,1);

  // You must manually destroy the xml structure when you are through with it:
  // (rather useless in this case, since at the end of the program)
  theBus.empty();
  xmlFreeDoc(doc); // free the libxml2 structure
  xmlParam::delete_structure(); // free the xmlParameters structure

  return 0;
}


