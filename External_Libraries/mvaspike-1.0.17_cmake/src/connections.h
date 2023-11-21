#ifndef CONNECTIONS_H
#define CONNECTIONS_H
#include <vector>
#include "devs.h"
#include "event.h"
#include "ifacebase.h"
class Connections {
 public:
  Connections();
  virtual ~Connections();
  virtual void init(IfaceBase * from,IfaceBase *to)=0;
  virtual void clean(IfaceBase * from,IfaceBase *to)=0;
  virtual bool has_post(int imminent,int ret)=0;

  virtual Ivent post(int imminent,int ret)=0;

  virtual bool has_next(Ivent * ev)=0;
  virtual void next(Ivent * ev)=0;

  virtual int get_comp(const Ivent * data)=0;
  virtual int get_port(const Ivent * data)=0;
  virtual int get_nb_edges(IfaceBase * from);
};

#endif
