#ifndef DEVS_H
#define DEVS_H
#include "baserecorder.h"
#include "event.h"

class Devs
{
  // basic Discrete Event System Object
  // defines ta,delta_int,delta_ext
  // defines useful functions to describes input/output ports
  // defines useful functions to walk through the DEVS tree
  // VIRTUAL !

protected:
  unsigned int nb_outport;	
  unsigned int nb_inport;
  bool init_ok;
  BaseRecorder *rec;  
  virtual void record_state(void);
public:
  static int NBDEVS;
  int id;

  Devs();				      
  virtual ~Devs(){}
  virtual void init(void);    
  virtual void init_record(BaseRecorder *rec); 
  virtual void clean(void);

  virtual int delta_int(double t)=0;
  virtual void delta_ext(double t, int x)=0;
  virtual double ta(void)=0;
  virtual void local_event(int *path);
  virtual void custom_event(int *path);

  virtual int get_imin(void);
  virtual void record(BaseRecorder *rec);

  virtual bool is_final(void);
  virtual bool is_population(void);

  unsigned int add_inport(void);
  unsigned int add_outport(void);
  unsigned int add_inport(unsigned int inc);
  unsigned int add_outport(unsigned int inc);

  void set_nb_inport(unsigned int nb);
  void set_nb_outport(unsigned int nb);

  unsigned int get_nb_outport(void);
  unsigned int get_nb_inport(void);
  void push_local_event(Event ev);
  
  virtual void visit(void (*f)(Devs *,int,int),int level);

  virtual int get_nb_comp(void);
  virtual Devs& get_comp(int nb);

  friend class Root;
};

#endif
