#ifndef PQF_HEAP_H
#define PQF_HEAP_H
#include <vector>
#include "pqf.h"

class PQF_heap : public PQF {
  class PEvent {
  public:
    double time;
    int comp;
  }; /* possible event */
  void change(int ind,double t);
  void set_change(int ind);
  bool get_change(void);

protected:
  PEvent * tab;
  int * index;
public:
  PQF_heap();
  ~PQF_heap(void);
  
  void delta_ext(int comp);
  void delta_int(int comp);
  double min(void);
  int imin(void);
  void init(std::vector<Devs*> *comp);
};

#endif
