#ifndef PQF_H
#define PQF_H
#include <vector>
#include "devs.h"

class PQF {
protected:
  int size;
  int ind;
  double tmin;
  std::vector<Devs*> *components;
public:
  PQF();
  virtual ~PQF(void);
  virtual void delta_ext(int comp);
  virtual void delta_int(int comp);
  virtual double min(void);
  virtual int imin(void);

  virtual void init(std::vector<Devs*> *comp);
  int get_size(void);

};
#endif
