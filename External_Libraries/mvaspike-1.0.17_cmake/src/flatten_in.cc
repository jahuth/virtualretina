#include "connections.h"
#include "flatten_in.h"
#include <vector>
#include <assert.h>
#include "devs.h"

using namespace std;

Flatten_In::Flatten_In()
{  
  maxp=0;
}


Flatten_In::~Flatten_In()
{
}

void Flatten_In::init(IfaceBase * from,IfaceBase *to)
{
  assert(from->get_nb_comp()==1);
  maxp=0;
  for (int i=0;i<to->get_nb_comp();i++)
    maxp=(maxp>=to->get_nb_port(i))?maxp:to->get_nb_port(i);
  assert(maxp<=from->get_nb_port(0));
}

void Flatten_In::clean(IfaceBase * from,IfaceBase *to)
{
  maxp=0;
}

bool Flatten_In::has_post(int imminent,int ret)
{
  return true;
}

Ivent Flatten_In::post(int imminent,int ret)
{
  Ivent ev;
  ev.nb=ret;
  return ev;
}


bool Flatten_In::has_next(Ivent *ev)
{
  return false;
}

void Flatten_In::next(Ivent * ev)
{
}

int Flatten_In:: get_comp(const Ivent * ev)
{
  return ev->nb/maxp;
}

int Flatten_In:: get_port(const Ivent * ev)
{
  return ev->nb%maxp;
}

