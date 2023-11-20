#ifndef LIF_H
#define LIF_H

#include "neuron.h"
#include "warray.h"
#include "internal_lif.h"

template <class Internal,class W>
class LifSkeleton :
  public Neuron,
  public Internal,
  public W
{
 protected:
  void record_state(void);
 public:
  LifSkeleton(void);
  ~LifSkeleton(void);
  double ta(void);
  int delta_int(double t);
  void delta_ext(double t,int port);
  void init(void);
  void auto_inport(void);
};

template <class Internal,class W>
LifSkeleton<Internal,W>::LifSkeleton(void):
  Neuron(),Internal(),W()
{
}

template <class Internal,class W>
LifSkeleton<Internal,W>::~LifSkeleton(void)
{
}

template <class Internal,class W>
double LifSkeleton<Internal,W>::ta(void)
{
  return Internal::ta();
}

template <class Internal,class W>
int LifSkeleton<Internal,W>::delta_int(double t)
{
  W::delta_int(t);
  Internal::reset(t);
  Neuron::delta_int(t);
  return 0;
}

template <class Internal,class W>
void LifSkeleton<Internal,W>::delta_ext(double t,int port)
{
  W::delta_ext(t,port);
  Internal::dirac(t,W::get_w(port));
  Neuron::delta_ext(t,port);
}

template <class Internal,class W>
void LifSkeleton<Internal,W>::init()
{
  Internal::init();
  W::init(nb_inport);
  Neuron::init();
}

template <class Internal,class W>
void LifSkeleton<Internal,W>::record_state(void)
{
  Internal::record_state(rec);
  W::record_state(rec);
}

template <class Internal,class W>
void LifSkeleton<Internal,W>::auto_inport(void)
{
  set_nb_inport(W::guess_nb_inport());
}

#endif
