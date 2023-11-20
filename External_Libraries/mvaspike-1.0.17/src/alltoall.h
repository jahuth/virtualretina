#ifndef ALLTOALL_H
#define ALLTOALL_H
#include "connections.h"
#include "devs.h"
#include "event.h"

// connect every component to a given port of every component.
class Alltoall : public Connections {
 protected:
  int nb;
  int fromport;
  int toport;
 public:
  Alltoall();
  ~Alltoall();
  void init(IfaceBase * from,IfaceBase *to);
  void clean(IfaceBase * from,IfaceBase *to);
  void set_from_port(int nb);
  void set_to_port(int nb);               // set the port we will connect to
  int get_from_port(void);
  int get_to_port(void); 
  bool has_post(int imminent,int ret);
  Ivent post(int imminent,int ret);
  bool has_next(Ivent * ev);
  void next(Ivent * ev);

  int get_comp(const Ivent * data);
  int get_port(const Ivent * data);
};


#endif
