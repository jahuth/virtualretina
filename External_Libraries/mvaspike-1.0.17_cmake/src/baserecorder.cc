#include "baserecorder.h"
#include <iomanip>
#include <algorithm>
using namespace std;

BaseRecorder::BaseRecorder(void)
{
  nbkeys=4;
  keyok= new bool[nbkeys];
  fill_n(keyok,nbkeys,true);
  key_actu=-1;
}

BaseRecorder::~BaseRecorder(void)
{
  delete[] keyok;
}

void BaseRecorder::init(void)
{
}

void BaseRecorder::new_event(double t)
{  
}

void BaseRecorder::add_spike(int id)
{
  key_actu=1;
}

void BaseRecorder::add_spike(int id, int outport)
{
  key_actu=1;
}

void BaseRecorder::add_reception(int id, int inport)
{
  key_actu=2;
}


void BaseRecorder::add_delayed(int id)
{
  key_actu=3;
}

void BaseRecorder::add_local(int id)
{
  key_actu=3;
}

void BaseRecorder::add(char *s)
{
}

void BaseRecorder::add(int i)
{
}

void BaseRecorder::add(double i)
{
}

void BaseRecorder::start_record(int key,int id)
{
  key_actu=key;
  add(key);
  add(id);
}

void BaseRecorder::enable_record(int key,bool t)
{
  keyok[key]=t;
}

bool BaseRecorder::is_enabled(int key)
{
  return keyok[key];
}
