#ifndef MEMSPIKERECORDER_H
#define MEMSPIKERECORDER_H
#include <iostream>
#include <utility>
#include <vector>
#include "spikerecorder.h"

class MemSpikeRecorder : public SpikeRecorder
{
 protected:
  typedef std::pair<double,int> _Rec;
  std::vector<_Rec> spikes;
 public:
  MemSpikeRecorder(void);
  // MemSpikeRecorder(int);
  ~MemSpikeRecorder(void);
    
  void add_spike(int id);
  void add_spike(int id, int outport);

  int get_nb_spikes(void);
  double get_spike_t(int nb);
  int get_spike_id(int nb);
  int get_last_id();
  double get_last_t();
  void save(char * filename);
  std::vector<double>* get_all_t(int nb);
};


#endif
