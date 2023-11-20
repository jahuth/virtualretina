#ifndef SIMPLEIFACE_H
#define SIMPLEIFACE_H
#include "ifacebase.h"

// homogeneous Iface (same number of ports for each component)
class SimpleIface : public IfaceBase 
{
 protected:
  int nb_comp;
  int nb_port;
 public:
  SimpleIface(int nbc,int nbp);
  virtual ~SimpleIface();

  int get_nb_comp(void);
  int get_nb_port(int comp);
};

#endif
