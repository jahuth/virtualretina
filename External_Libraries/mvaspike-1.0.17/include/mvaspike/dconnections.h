#ifndef DCONNECTIONS_H
#define DCONNECTIONS_H
#include <vector>
#include "devs.h"
#include "event.h"
#include "ifacebase.h"
#include "eventiterator.h"
class DConnections : public EventIterator {
 public:
  DConnections();
  virtual ~DConnections();
  virtual void init(IfaceBase * from,IfaceBase *to)=0;
  virtual void clean(IfaceBase * from,IfaceBase *to)=0;
  virtual bool has_post(int imminent,int ret)=0;
  virtual Event post(int imminent,int ret,double t)=0;
};

#endif
