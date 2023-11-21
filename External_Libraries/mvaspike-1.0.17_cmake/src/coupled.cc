#include <math.h>
#include <assert.h>
#include "coupled.h"
#include "event.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include "iface.h"

using namespace std;

Coupled::Coupled() :
  ContainerCoupled()
{
  tn=-1;
  imminent=EV_UNKNOWN;
}

Coupled::~Coupled()
{

}

void Coupled::add_con(Connections *con)
{
  this->con.push_back(con);
}

void Coupled::add_eic(Connections *con)
{
  this->eic.push_back(con);
}

void Coupled::add_eoc(Connections *con)
{
  this->eoc.push_back(con);
}

void Coupled::add_con(Connections &con)
{
  this->con.push_back(&con);
}

void Coupled::add_eic(Connections &con)
{
  this->eic.push_back(&con);
}

void Coupled::add_eoc(Connections &con)
{
  this->eoc.push_back(&con);
}


double Coupled::ta(void)
{
  if (imminent!=EV_UNKNOWN)
    return tn;

  // TODO : tie-breaking / select !!!
  imminent=EV_NOP;
  tn=HUGE_VAL;
  
  for (unsigned int i=0;i<components.size();i++)
    {
      double lt=components[i]->ta();
      if (lt<tn)
	{
	  tn=lt; 
	  imminent=i;
	}
    }
  return tn;
}

int Coupled::get_imin(void)
{
  ta();
  return imminent;
}

int Coupled::delta_int(double t)
{
  int ret;
  
  assert(imminent!=EV_NOP);

  ret=components[imminent]->delta_int(t);


  // internal connections
  if (ret!=EV_NOP) 
    for (unsigned int i=0;i<con.size();i++)
      if (con[i]->has_post(imminent,ret))
	{
	  Ivent ev=con[i]->post(imminent,ret);
	  components[con[i]->get_comp(&ev)]->delta_ext(t,con[i]->get_port(&ev));
	  while (con[i]->has_next(&ev)) {
	    con[i]->next(&ev);
	    components[con[i]->get_comp(&ev)]->delta_ext(t,con[i]->get_port(&ev));
	  }
	}

  // output

  if (ret==EV_NOP)
    {
      imminent=EV_UNKNOWN;
      return EV_NOP;
    }

  for (unsigned int i=0;i<eoc.size();i++)
    {
      if (eoc[i]->has_post(imminent,ret))
        {
		  Ivent ev=eoc[i]->post(imminent,ret);
		  // WARNING returning the first one
		  // (only one output/event) might change in the future
		  imminent=EV_UNKNOWN;
		  return eoc[i]->get_port(&ev);
        }
    }
  imminent=EV_UNKNOWN;
  return EV_NOP;
}

void Coupled::delta_ext(double t, int x)
{
  imminent=EV_UNKNOWN;
  for (unsigned int  i=0;i<eic.size();i++)
    if (eic[i]->has_post(0,x))
      {
	Ivent ev=eic[i]->post(0,x);
	components[eic[i]->get_comp(&ev)]->delta_ext(t,eic[i]->get_port(&ev));
	while (eic[i]->has_next(&ev)) {
	  eic[i]->next(&ev);
	  components[eic[i]->get_comp(&ev)]->delta_ext(t,eic[i]->get_port(&ev));
	}
      }
}

void Coupled::init()
{
  if (!defcon.empty())
    con.push_back(&defcon);
  if (!defeic.empty())
    eic.push_back(&defeic);
  if (!defeoc.empty())
    eoc.push_back(&defeoc);

  for (unsigned int i=0; i<components.size(); i++)
    {
      components[i]->init();
    }

  Iface *i=new Iface;
  i->copy_out(&components);
  outface=i;
  i=new Iface;
  i->copy_in(&components);
  inface=i;

  eocface=new Iface(1,nb_outport);
  eicface=new Iface(1,nb_inport);

  for (unsigned int i=0;i<con.size();i++)
    con[i]->init(outface,inface);

  for (unsigned int i=0;i<eoc.size();i++)
    eoc[i]->init(outface,eocface);

  for (unsigned int i=0;i<eic.size();i++)
    eic[i]->init(eicface,inface);
}

void Coupled::clean()
{
  Devs::clean();

  for (unsigned int i=0; i<components.size(); i++)
    {
      components[i]->clean();
    }
  
  for (unsigned int i=0;i<con.size();i++)
    con[i]->clean(outface,inface);

  for (unsigned int i=0;i<eoc.size();i++)
    eoc[i]->clean(outface,eocface);

  for (unsigned int i=0;i<eic.size();i++)
    eic[i]->clean(eicface,inface);

  delete outface;
  delete inface;
  delete eocface;
  delete eicface;

}

void Coupled::connect(int c1,int p1,int c2,int p2)
{
    defcon.connect(c1,p1,c2,p2);
}

void  Coupled::connect_in(int p1,int c2,int p2)
{
    defeic.connect(0,p1,c2,p2);
}

void  Coupled::connect_out(int c1,int p1,int p2)
{
    defeoc.connect(c1,p1,0,p2);
}
