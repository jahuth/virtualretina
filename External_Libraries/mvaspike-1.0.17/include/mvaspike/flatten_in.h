#ifndef FLATTEN_IN_H
#define FLATTEN_IN_H
#include "connections.h"
#include "devs.h"
#include "event.h"
#include <vector>

class Flatten_In : public Connections {
  // map a single component, with many inports,
  // from some components with some ports
 protected:
  int maxp;
 
 public:
  Flatten_In();
  ~Flatten_In();
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
