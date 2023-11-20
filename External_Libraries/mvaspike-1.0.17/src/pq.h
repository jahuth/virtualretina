#ifndef PQ_H
#define PQ_H
#include "event.h"

class PQ {
  // good old priority queue (heap)
 protected:
  int len;
  int maxlen;
  int chunk;
  Event *tab;
 public:
  PQ(void);
  virtual ~PQ(void);
  virtual bool empty(void);
  virtual double top_t(void);
  virtual void * top_data(void);
  virtual int top_nb(void);
  virtual const Event * top(void);
  virtual void change_first(Event ev);
  virtual void pop(void);
  virtual void push(Event ev);
  virtual int get_size(void);
};

#endif
