#include "sensor.h"
#include "error.h"
Sensor::Sensor()
  : Neuron()
{
  nb_inport=0;
}

Sensor::~Sensor()
{
}

void Sensor::delta_ext(double t, int x)
{
  warning("Sensor::delta_ext does nothing..."); 
}
