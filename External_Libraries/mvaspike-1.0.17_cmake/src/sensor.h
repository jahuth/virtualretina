#ifndef SENSOR_H
#define SENSOR_H

#include "neuron.h"

// A sensor is a neuron that has no spiking input.

class Sensor : public Neuron 
{
 public:
  Sensor();
  ~Sensor();   
  void delta_ext(double t, int x);
};
	     
#endif
