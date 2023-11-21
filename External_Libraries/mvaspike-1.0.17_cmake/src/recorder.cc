#include "recorder.h"
#include "root.h"
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <cstring>
using namespace std;

Recorder::Recorder(ostream & _os) : BaseRecorder(), os(_os)
{
  os.setf(ios::showpoint);
  needf=false;
  id_mode=ID_GLOBAL;
}

Recorder::~Recorder(void)
{
  if (needf)
    os << endl;
}

void Recorder::init(void)
{
  if (needf)
    {
      needf=false;
      os << endl;
    }
  os << "INIT" << endl;
}

void Recorder::new_event(double t)
{  
  if (needf)
    {
      needf=false;
      os << endl;
    }
  os << "Event at t="<< t <<endl;
}

void Recorder::add_spike(int id)
{
  //key_actu=1;
  if (needf)
    {
      needf=false;
      os << endl;
    }
  os << " SPIKE ";
  write_id(os,id);
  os << " ";
  needf=true;
}

void Recorder::add_spike(int id, int outport)
{
  //key_actu=1;
  if (needf)
    {
      needf=false;
      os << endl;
    }
  os << " SPIKE ";
  write_id(os,id);
  os << " " << outport << " ";
  needf=true;
}

void Recorder::add_reception(int id, int inport)
{
  //key_actu=2;
  if (needf)
    {
      needf=false;
      os << endl;
    }
  os << " RECEP ";
  write_id(os,id);
  os << " " << inport << " ";
  needf=true;
}


void Recorder::add_delayed(int id)
{
  //key_actu=3;
  if (needf)
    {
      needf=false;
      os << endl;
    }
  os << " DELAY ";
  write_id(os,id);
  os << " ";
  needf=true;
}

void Recorder::add_local(int id)
{
  //key_actu=3;
  if (needf)
    {
      needf=false;
      os << endl;
    }
  os << " LOCAL ";
  write_id(os,id);
  os<< " ";
  needf=true;
}

void Recorder::add(char *s)
{
  os << s << " ";
  needf=true;
}

void Recorder::add(int i)
{
  os << i << " ";
  needf=true;
}

void Recorder::add(double i)
{
  int w=os.width();
  os << setw(5) << i << " ";
  os.width(w);
  needf=true;
}

void Recorder::set_id_mode(int mode)
{
  id_mode=mode;
}

void Recorder::write_id(ostream &os,int id)
{
  switch(id_mode)
    {
    case 0:
      os << id;
      break;
    case 1:
      for (int i=0;i<Root::get_instance()->get_path_len(id)-1;i++)
	{
	  os << Root::get_instance()->get_path(id,i);
	  os << ":";
	}
      if (Root::get_instance()->get_path_len(id)>0)
	os << Root::get_instance()->get_path(id,-1);
      break;
    case ID_MAP:
      if ((map.count(id)!=0)&&(map.find(id)->second!=NULL))
	os << map.find(id)->second;
      else
	os << id;
      break;
      // TODO local, map+path
     
    }
}

void Recorder::set_name(int id,char * s)
{
  map[id]=s; 
}

void Recorder::set_name(int id,int v)
{
  // TODO XXX Memory leak
  // TODO : keep track and free ...
  stringstream ss;
  ss << v;
  int len=ss.str().size();
  char *s=new char[len+1];
  strcpy(s,ss.str().c_str());
  map[id]=s;  
}
