#ifndef SPIK_UNITS_H
#define SPIK_UNITS_H

#include "pulse_coupled.h"
#include "retinatools/misc_tools.h"
#include "retinatools/randomness.h"
#include "cellporthandler.h"


class CondNeuron_cpm : public Neuron, public Warray, public CellPortHandler

// A conductance-based integrate and fire neuron, with as many ionic channels as one wishes
// cpm means "constante par morceaux", which the conductances are supposed to be between events.

{
protected:

  //Evolution variables:
  double V, g_infini, I_tot, E_infini, g_leak;
  double tl;   //watch out, "l" is an "L", not a "one".

  double* inputValues;
  void record_state(void);
  bool done;

public :

  //Construction/Destruction
  CondNeuron_cpm(void);
  ~CondNeuron_cpm();

  //Spiking parameters:
  double Vreset, thres;
  double tau_refr, remaining_refr;
  double noise_refr;            //adds gaussian noise on the refractory period length (cf Keat, Reinagel, Reid, Meister 2001)

  //'Driving' functions...
  void add_inputValue(double newAddVal, int cellPort);
  void change_inputValue(double newVal, int cellPort);

  //Basic Time/Advance functions :
  double ta(void);
  void advance(double t);

  //Event Handling Functions
  int delta_int(double t);

  //Miscellaneous functions
  void init(void);
  void auto_inport(void);

  //All the "Set_X" functions
  void set_V(double Vi);
  void set_g_leak(double g);

  //All the "get_X" functions
  double get_V(void);
  double get_inputValue(int cellPort);
  double get_g_infini(void);
  double get_I_tot(void);
  double get_E_infini(void);
};




//************************ IntegratorNeuron_cpm   ****************************************************


//this is a CondNeuron_cpm designed to integrate CONTINUOUS signals, with time step "step" (possibly excitating or inhibiting conductances).
//It can be a ganglionar cell, or a simplified V1 neuron, etc.
//It is designed to be synchronised by a PulseNeuron (plugged on its port number 0);

//One remark :
// In mvaspike, a root, i.e. an actual simulator, is needed to handle time properly.
//That is why an "IntegratorNeuron_cpm" is not totally independent, in the sense that its own conductance value at present time (given through function feed_new_value) must be integrated at the right time by its "CondNeuron_cpm" part, through its inclusion in a "PulseCoupled" object.


class IntegratorNeuron_cpm : public CondNeuron_cpm

{
protected :

  double step;
  double** stockPulsedInput;
  double** stockPulsedInput_prec;

  double noiseV;
  double noiseIncr, noiseIncr_prec;  //a function of the preceding. It's the standard deviation of the little gaussian uncorrelated noise, added to the driving current at every "step" (that is why we introduce the second noise source at the IntegratorNeuron_cpm level, although intrinsically, it is rather a CondNeuron_cpm property). This noise on the driving current should result in a (quasi)gaussian CORRELATED noise on V with standard deviation being (quasi) noise_V.  noise_incr will be closer and closer to 0 as the time step gets smaller.

  static bool conductanceWarning; //static variables to print ONCE only a warning about possible negative conductances
	bool addedInCoupled;

public :

  IntegratorNeuron_cpm();
  ~IntegratorNeuron_cpm();

  void set_step(double stepp);
  void set_noiseV(double t);

  //Miscellaneous functions
  void init(void);

int pulsed_MvaspikePort;

  //Driving function :
  void feedInput(double val,int cellPort=0, bool shutUp=false);
  //and two 'old' names:
  void feedCurrent(double curr, int cellPort=0, bool shutUp=false);	//feeding current in channel 'cellPort'
  void feedCond(double cond, int cellPort, bool shutUp=false);	//feeding conductance in channel 'cellPort'

//Synchronization of the unit, by adding it inside a PulseCoupled object.
virtual void add_in_coupled(PulseCoupled & daCoupled );

protected:
  //Extension of the devs formalism :
  void delta_ext(double t, int port);
};



//**************** same as preceding, with the addition of a simple spike-frequency adaptation scheme. Each emitted spike adds a small exponential kernel to an adapting inhibitory conductance term.
//Bastardish 'incremental' object in an event-driven formalism... because we have NO explicit solution for the spike time otherwise! Use this one with a SMALL simulation time step (1 ms or so). Only for 'theoretical' experimentation (paradox, ey)

// Alternatively, you can give a try at the next object in this header (the _square version), that has an exact solution allowing to use bigger simulation time steps BUT, on the other hand, loses all the interesting properties of exponential results and thus provides poor biological results.
//Aint life a bitch.


class IntegratorNeuron_adap_exp : virtual public IntegratorNeuron_cpm
{
public :
int autoInh_CellPort;

IntegratorNeuron_adap_exp();

void set_delta_g_adap(double val);
void set_tau_g_adap(double val);
void set_E_adap(double val);

  void init(void);

protected:
  double delta_g_adap;
  double tau_g_adap, exp_decay_factor;
  //Extension of the devs formalism :
  int delta_int(double t);
  void delta_ext(double t, int port);
};



//**************** same as preceding, but here spike-triggered inhibitory conductance is a square function: this allows to still use a purely evenmential exact formalism... but on the other hand it does not have good effective properties!!
//**************** you should not use this object, however I prefer to leave its code here for the moment.


class IntegratorNeuron_adap_square : virtual public IntegratorNeuron_cpm
{
public :
int autoInh_CellPort, autoInh_MvaspikePort;

IntegratorNeuron_adap_square();

void add_in_coupled(PulseCoupled & daCoupled);

void set_delta_g_adap(double val);
void set_tau_g_adap(double val);
void set_E_adap(double val);

protected:
  double delta_g_adap, tau_g_adap;

  //Extension of the devs formalism :
  int delta_int(double t);
  void delta_ext(double t, int port);
};



#endif











