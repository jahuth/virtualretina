#ifndef PULSE_COUPLED_H
#define PULSE_COUPLED_H

#include <mvaspike.h>

//***********************    PulseNeuron   ******************************
// a regular pulsar at every time "t*step". 
// It is needed to synchronize an IntegratorNeuron_cpm (see spik_units.h)

class PulseNeuron: public Neuron

{
protected:
  double tl;
  double step;
  double remain_time_to_step;

public :

  //Construction/Destruction
  PulseNeuron(double stepp);
  ~PulseNeuron();

  //Event Handling Functions
  int delta_int(double t);
  void delta_ext(double t, int port);

  //Miscellaneous
  void init(void);
  void auto_inport(void);
  double ta(void);
  void advance(double t);
};


//***********************      PulseCoupled       *************************
// a simple DCoupled object, with added synchronization 
// properties thanks to its PulseNeuron.

class PulseCoupled : public DCoupled
{
	public:
	PulseNeuron daPulse;
	PulseCoupled(double step) : DCoupled() , daPulse(step)
	{
	  this->add_component(daPulse);
	}
};
//That's all folks!

#endif











