#ifndef FLATTEN_OUT_H
#define FLATTEN_OUT_H
#include "connections.h"
#include "devs.h"
#include "event.h"
#include <vector>

class Flatten_Out : public Connections {
  // map a set of components, with any number of outport,
  // to a single component with many ports
 protected:
  int maxp;
 
 public:
  Flatten_Out();
  ~Flatten_Out();
  void init(IfaceBase * from,IfaceBase *to);
  void clean(IfaceBase * from,IfaceBase *to);
  bool has_post(int imminent,int ret);
  Ivent post(int imminent,int ret);
  bool has_next(Ivent * ev);
  void next(Ivent * ev);

  int get_comp(const Ivent * data);
  int get_port(const Ivent * data);
};

#endif
