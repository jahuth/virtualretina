#ifndef SPIKETRAINS_H
#define SPIKETRAINS_H
#include <iostream>
#include "devs.h"
class SpikeTrains : public Devs
{
 protected:
  class OutSpike {
  public:
    OutSpike(void) : t(-1), nb(-1) {};
    OutSpike(double _t,int _nb): t(_t), nb(_nb) {};
    
    double t;
    int nb;
  };
  static bool greater(const OutSpike& o1,const OutSpike &o2)
    { 
      return (o1.t>o2.t);
    }
  OutSpike tn;
  virtual OutSpike advance(void)=0; // global time !
 public:
  SpikeTrains();
  virtual ~SpikeTrains();
  double ta(void);
  int delta_int(double t);
  void delta_ext(double t, int x);
  int get_imin(void);
};

#endif
