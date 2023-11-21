#include "connections.h"
#include "alltoall.h"
#include <vector>
#include "devs.h"

using namespace std;

Alltoall::Alltoall()
  : Connections()
{  
  nb=0;
  fromport=0;
  toport=0;
}


Alltoall::~Alltoall()
{
}

void Alltoall::init(IfaceBase * from,IfaceBase *to)
{
  nb=to->get_nb_comp();
}

void Alltoall::clean(IfaceBase * from,IfaceBase *to)
{
  nb=0;
}

bool Alltoall::has_post(int imminent,int ret)
{
  return (ret==fromport);
}

Ivent Alltoall::post(int imminent,int ret)
{
  Ivent ev;
  ev.nb=0;
  return ev;
}


bool Alltoall::has_next(Ivent *ev)
{
  if (ev->nb<nb-1) {
    return true;
  } else return false;
}

void Alltoall::next(Ivent * ev)
{
  ev->nb+=1;
}

int Alltoall:: get_comp(const Ivent * ev)
{
  return ev->nb;
}

int Alltoall:: get_port(const Ivent * data)
{
  return toport;
}

int Alltoall:: get_from_port(void)
{
  return fromport;
}
int Alltoall:: get_to_port(void)
{
  return toport;
}

void Alltoall::set_from_port(int nb)
{
  fromport=nb;
}

void Alltoall::set_to_port(int nb)
{
  toport=nb;
}
