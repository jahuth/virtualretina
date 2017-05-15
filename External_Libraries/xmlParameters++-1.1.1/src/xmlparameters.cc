#include "xmlparameters.h"

namespace xmlParam
{

// *******************  Xmlizable *******************

Xmlizable::~Xmlizable()
{
  // remove this wrapper of mine, if any.
  delete xmlObject_base::find_xmlObject(this); // my xmlClass ref_address is 'this', by construction :-)
  // remove my entry in the lastNames map.
  std::map<void *, std::string >::iterator iter = xmlObject_base::lastNames.find(this); // idem
  if(iter!=xmlObject_base::lastNames.end())
    xmlObject_base::lastNames.erase(iter);
  /* This minimizes the presence of obsolete addresses in the reverseMap and in lastNames 
  (corresponding to an object which has then been deleted). However, to avoid *all*
  obsolete addresses, one should also replace xmlized pointers by a class "xmlParam::ptr",
  and xmlized containers by classes "xmlParam::vector", "xmlParam::list" and "xmlParam::deque",
  all also inheriting from Xmlizable (TODO ? or not :-) )
  If this is done one day, replace call to 'this' by a more explicit
  call to 'xmlObject_base::ref_address(this)'
  */
}

// *************** other functions ********************

void delete_structure()
{
  while(xmlObject_base::static_toDelete.size())
  {
    delete xmlObject_base::static_toDelete.front();
    xmlObject_base::static_toDelete.pop_front();
  }
  xmlObject_base::lastNames.clear();
}


// create a list<string> from a list of arguments.
// tremendous.


std::list<std::string> names(std::string str1)
{std::list<std::string> ls; ls.push_back(str1); return ls;}

std::list<std::string> names(std::string str1, std::string str2)
{std::list<std::string> ls=names(str1); ls.push_back(str2); return ls;}

std::list<std::string> names(std::string str1, std::string str2, std::string str3)
{std::list<std::string> ls=names(str1,str2); ls.push_back(str3); return ls;}

std::list<std::string> names(std::string str1, std::string str2, std::string str3, std::string str4)
{std::list<std::string> ls=names(str1,str2,str3); ls.push_back(str4); return ls;}

std::list<std::string> names(std::string str1, std::string str2, std::string str3, std::string str4, std::string str5)
{std::list<std::string> ls=names(str1,str2,str3,str4); ls.push_back(str5); return ls;}

std::list<std::string> names(std::string str1, std::string str2, std::string str3, std::string str4, std::string str5, std::string str6)
{std::list<std::string> ls=names(str1,str2,str3,str4,str5); ls.push_back(str6); return ls;}


} // namespace xmlParam

