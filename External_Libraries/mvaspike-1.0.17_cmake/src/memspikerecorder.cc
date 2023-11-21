#include "memspikerecorder.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include "error.h"
#include "root.h"

using namespace std;

MemSpikeRecorder::MemSpikeRecorder(void)
    : SpikeRecorder()
{
}

MemSpikeRecorder::~MemSpikeRecorder(void)
{
}

void MemSpikeRecorder::add_spike(int id)
{
  spikes.push_back(_Rec(Root::get_instance()->get_t(),id));
}

void MemSpikeRecorder::add_spike(int id, int outport)
{
  spikes.push_back(_Rec(Root::get_instance()->get_t(),id));
}


int MemSpikeRecorder::get_nb_spikes(void)
{
  return spikes.size();
}

double MemSpikeRecorder::get_spike_t(int nb)
{
  return spikes[nb].first;
}

int MemSpikeRecorder::get_spike_id(int nb)
{
  return spikes[nb].second;
}

int MemSpikeRecorder::get_last_id()
{
  if (spikes.size()==0)
    return -1;
  else
    return spikes[spikes.size()-1].second;
}

double MemSpikeRecorder::get_last_t()
{
  if (spikes.size()==0)
    return -1.0;
  else
    return spikes[spikes.size()-1].first;
}

void MemSpikeRecorder::save(char * filename)
{
  ofstream f(filename);
  for (unsigned int i=0;i<spikes.size();i++)
    {
      write_id(f,spikes[i].second);
      f << " " << spikes[i].first << endl;
    }
}

vector<double>* MemSpikeRecorder::get_all_t(int nb)
{
  // Warning : it is the responsability of the caller to delete
  // the returned vector. Use a copy instead ?
  vector<double> *v=new vector<double>;
  for (int i=0;i<spikes.size();i++)
    if (spikes[i].second==nb)
      v->push_back(spikes[i].first);
  return v;
}
