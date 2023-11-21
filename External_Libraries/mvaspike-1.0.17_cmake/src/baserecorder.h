#ifndef BASERECORDER_H
#define BASERECORDER_H

class BaseRecorder {
 protected:
  int nbkeys;
  int key_actu;
  bool *keyok;
 public:
  BaseRecorder(void);
  virtual ~BaseRecorder();

  virtual void init(void);          // record initial conditions

  virtual void new_event(double t); // start recording a new event
  virtual void add_spike(int id);   // a spike
  virtual void add_spike(int id, int outport);
  virtual void add_reception(int id, int inport);
  virtual void add_delayed(int id); // should use addlocal instead
  virtual void add_local(int id); 

  virtual void start_record(int key,int id);
  virtual void add(char *s);
  virtual void add(int i);
  virtual void add(double i);

  void enable_record(int key,bool t);
  bool is_enabled(int key);
};


#endif
