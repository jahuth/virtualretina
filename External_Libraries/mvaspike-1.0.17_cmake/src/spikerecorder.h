#ifndef SPIKERECORDER_H
#define SPIKERECORDER_H
#include <iostream>
#include "recorder.h"

class SpikeRecorder : public Recorder
{
 public:
  SpikeRecorder(void);
  ~SpikeRecorder(void);
  void init(void);

  void new_event(double t);
  void add_spike(int id);
  void add_spike(int id, int outport);
  void add_reception(int id, int inport);
  void add_delayed(int id);
  void add(int);
  void add(char *);
  void add(double);
};


#endif
