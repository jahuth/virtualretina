#include "connections.h"
#include "flatten_out.h"
#include <vector>
#include <assert.h>
#include "devs.h"

using namespace std;

Flatten_Out::Flatten_Out()
{  
  maxp=0;
}


Flatten_Out::~Flatten_Out()
{
}

void Flatten_Out::init(IfaceBase * from,IfaceBase *to)
{
  assert(to->get_nb_comp()==1);
  // find max 
  maxp=0;
  for (int i=0;i<from->get_nb_comp();i++)
    maxp=(maxp>=from->get_nb_port(i))?maxp:from->get_nb_port(i);
  assert(maxp<=to->get_nb_port(0));
}

void Flatten_Out::clean(IfaceBase * from,IfaceBase *to)
{
  maxp=0;
}

bool Flatten_Out::has_post(int imminent,int ret)
{
  return true;
}

Ivent Flatten_Out::post(int imminent,int ret)
{
  Ivent ev;
  ev.nb=imminent*maxp+ret;
  return ev;
}


bool Flatten_Out::has_next(Ivent *ev)
{
  return false;
}

void Flatten_Out::next(Ivent * ev)
{
}

int Flatten_Out:: get_comp(const Ivent * ev)
{
  return 0;
}

int Flatten_Out:: get_port(const Ivent * ev)
{
  return ev->nb;
}

