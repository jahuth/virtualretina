#ifndef IFACE_H
#define IFACE_H
#include "ifacebase.h"
#include "devs.h"
#include <vector>

class Iface : public IfaceBase 
{
 protected:
  int nb_comp;
  int  *port;
 public:
  Iface();  
  Iface(int nbc,int nbp);
  virtual ~Iface();

  void copy_out(std::vector<Devs*>* v);
  void copy_in(std::vector<Devs*>* v);

  int get_nb_comp(void);
  int get_nb_port(int comp);
};

#endif
