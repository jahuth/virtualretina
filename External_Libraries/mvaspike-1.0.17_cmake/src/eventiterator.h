#ifndef EVENTITERATOR_H
#define EVENTITERATOR_H
#include "event.h"
class EventIterator {
 public:
  virtual ~EventIterator(){}
  virtual bool has_next(const Event * ev)=0;
  virtual Event next(const Event * ev)=0;
  virtual int get_comp(const Event * data)=0;
  virtual int get_port(const Event * data)=0;
};

#endif
