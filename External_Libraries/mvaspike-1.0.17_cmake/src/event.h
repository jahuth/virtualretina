#ifndef EVENT_H
#define EVENT_H

// TODO enum 
#define EV_NOP (-1)
#define EV_INT (-2)
#define EV_EXT (-3)
#define EV_OUT (-4)
#define EV_UNKNOWN (-5)

#define ID_ELSEWHERE (-1)
#define ID_NOWHERE (-2)

#include <stdlib.h>
typedef union {
  void * p;
  int i;
} U_pi;

// immediate event, or iterating event, as you like.
class Ivent{
 public:
  int nb;
  U_pi data;
  class EventIterator * iterator;
};

class Event :public Ivent{
 public:
  Event(void);
  Event(double t);
  Event(int nb,double t);
  Event(Ivent iv,double t);
  Event(int nb, void * data,double t,class EventIterator *ei);
  Event(int nb, int data,double t,class EventIterator *ei);
  bool has_next(void) const;
  Event next(void) const;
  int get_comp(void) const;
  int get_port(void) const;

  double t;
  // TODO : operators ? cf. how we use sort...
  static bool greater(const Event& o1,const Event &o2)
  { 
    return (o1.t>o2.t);
  }
  static bool lower(const Event& o1,const Event &o2)
  {
    return (o1.t<o2.t);
  }
  friend bool operator<(const Event& left,const Event& right)
  {
    return left.t<right.t;
  }
};
#endif
