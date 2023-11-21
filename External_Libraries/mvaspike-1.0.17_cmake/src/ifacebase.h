#ifndef IFACEBASE_H
#define IFACEBASE_H

// interface definition for Iface
class IfaceBase {
 public:
  virtual ~IfaceBase(){}
  virtual int get_nb_comp(void)=0;
  virtual int get_nb_port(int comp)=0;
};

#endif
