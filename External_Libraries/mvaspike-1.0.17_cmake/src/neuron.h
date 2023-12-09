#ifndef NEURON_H
#define NEURON_H
#include <vector>
#include <iostream>
#include "devs.h"

#include "event.h"

class Neuron : public Devs
{
 public:
  Neuron(void);
  virtual ~Neuron(void);
  bool is_final(void);
  int delta_int(double t);
  void delta_ext(double t, int x);
  void local_event(int * path);
  void custom_event(int *path);
  virtual void local_event(const Event &ev);
  virtual void custom_event(const Event &ev);
};

#endif
