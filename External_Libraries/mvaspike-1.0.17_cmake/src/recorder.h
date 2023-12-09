#ifndef RECORDER_H
#define RECORDER_H
#include "baserecorder.h"
#include <iostream>

#ifndef HAVE_HASH_MAP
#include <map>
typedef std::map<int,const char *> DictMap;
#else
#include <hash_map>
typedef std::hash_map<int,const char *> DictMap;
#endif

class Recorder : public BaseRecorder
{
 protected:
  bool needf;
  std::ostream &os;
  int id_mode;
  DictMap map;
  void write_id(std::ostream &os,int id);
 public:
  // TODO ENUM
  static const int ID_GLOBAL=0;
  static const int ID_PATH=1;
  static const int ID_MAP=2;

  Recorder(std::ostream &os=std::cout);
  virtual ~Recorder();

  virtual void init(void);          // record initial conditions

  virtual void new_event(double t); // start recording a new event
  virtual void add_spike(int id);   // a spike
  virtual void add_spike(int id, int outport);
  virtual void add_reception(int id, int inport);
  virtual void add_delayed(int id); // should use addlocal instead
  virtual void add_local(int id); 

  virtual void add(char *s);
  virtual void add(int i);
  virtual void add(double i);
  void set_id_mode(int mode);
  void set_name(int id,char * s);  
  void set_name(int id,int v);

};


#endif
