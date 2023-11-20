#include "spikerecorder.h"
#include <iostream>
#include <iomanip>
#include "error.h"
#include "root.h"

using namespace std;

SpikeRecorder::SpikeRecorder(void)
    : Recorder()
{
}

SpikeRecorder::~SpikeRecorder(void)
{

}


void SpikeRecorder::new_event(double t)
{
//    cout  << t << " ";
}

void SpikeRecorder::add_spike(int id)
{
  // TODO : remplacer par event_actu que new_event pourrait
  // memoriser... ca revient presque au meme (avec 8 octets de plus)
  // mais c'est plus lisible...
  write_id(os,id);
  os << " " << Root::get_instance()->get_t()<<endl;
}

void SpikeRecorder::add_spike(int id, int outport)
{
  write_id(os,id);
  os << " " << outport<< " " << Root::get_instance()->get_t()<<endl;
}

void SpikeRecorder::add_reception(int id, int inport)
{
    // NOP
}

void SpikeRecorder::add_delayed(int id)
{
    // cout << "D";
}

void SpikeRecorder::add(char *s)
{
}
void SpikeRecorder::add(int i)
{
}

void SpikeRecorder::add(double i)
{
}

void SpikeRecorder::init(void)
{
}
