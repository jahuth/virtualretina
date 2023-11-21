#include "freedcon.h"
#include <vector>
#include <algorithm>
#include <assert.h>

#include "event.h"
#include "error.h"
using namespace std;

FreeDCon::FreeDCon() : DConnections()
{
  tmp_IC=new vector<TTmpcon>;
  IC=NULL;
  from=NULL;
}

FreeDCon::~FreeDCon() {
  // TODO état...
  if (tmp_IC!=NULL)
    delete tmp_IC;
  else
	{
	  delete_IC();
	}
}

void FreeDCon::delete_IC()
{
  assert(IC!=NULL);
  for (int i=0; i<from->get_nb_comp();i++) 
    for (int j=0;j<from->get_nb_port(i);j++)
      {
		delete[] IC[i][j].tab;
      }	
  for (int i=0; i<from->get_nb_comp();i++)
    {
      if (from->get_nb_port(i)!=0) // not strickly needed if setup correctly...
		{
		  delete[] IC[i];
		}
    }
  delete[] IC;
}

void FreeDCon::connect(int comp1, int port1, int comp2, int port2,double delay)
{
  if (tmp_IC==NULL) {
    // TODO : virer, gérer l'état prep / l'état running à la place.
    tmp_IC=new vector<TTmpcon>;
  }
  tmp_IC->push_back(TTmpcon(comp1,port1,comp2,port2,delay));
}

void FreeDCon::init(IfaceBase * from,IfaceBase *to)
{
  // TODO : choose the best allocation scheme (compress or not ?)
  // default is : not compressed
  // TODO : si on peut optimiser aussi la gestion de empty()...

  this->from=from;
  // from is guarenteed to stay valid until clean() is called 

  IC=new DTargets*[from->get_nb_comp()];
  for (int i=0; i<from->get_nb_comp();i++)
    {
      if (from->get_nb_port(i)!=0)
		{
		  IC[i]=new DTargets[from->get_nb_port(i)];
		  for (int j=0;j<from->get_nb_port(i);j++)
			IC[i][j].cpt=0;
		}
      else 
		{
		  IC[i]=NULL;
		}
    }

  for (unsigned int i=0; i<tmp_IC->size(); i++)
    {
      if ((*tmp_IC)[i].from.comp>=from->get_nb_comp())
		fatal("FreeDCon : connection from an unknown component");
      if ((*tmp_IC)[i].from.port>=from->get_nb_port((*tmp_IC)[i].from.comp))
		fatal("FreeDCon : connection from an unknown port");

      IC[(*tmp_IC)[i].from.comp][(*tmp_IC)[i].from.port].cpt++;
    }
  for (int i=0; i<from->get_nb_comp();i++) {
    for (int j=0;j<from->get_nb_port(i);j++)
      {
		IC[i][j].tab=new DTarget[IC[i][j].cpt];
		IC[i][j].cpt=0;
      }
  }

  // TODO tableaux vides & exception data incorrectes
  // TODO iterator pour alléger les tmp_IC[i]
  for (unsigned int i=0; i<tmp_IC->size(); i++)
    {
      int comp=(*tmp_IC)[i].from.comp;
      int port=(*tmp_IC)[i].from.port;
      IC[comp][port].tab[IC[comp][port].cpt]=(*tmp_IC)[i].to;

      IC[comp][port].cpt++;
    }

  for (int i=0; i<from->get_nb_comp();i++) {
    for (int j=0;j<from->get_nb_port(i);j++)
      {
		stable_sort(IC[i][j].tab,IC[i][j].tab+IC[i][j].cpt);
	  }
  }
  delete tmp_IC;
  tmp_IC=NULL;
}

void FreeDCon::clean(IfaceBase * from,IfaceBase *to)
{
  assert(IC!=NULL);
  delete_IC();
  from=NULL;
  IC=NULL;
  // TODO: we should rebuild tmp_IC ! (or get  rid of clean() everywhere)
}

bool FreeDCon::has_post(int imminent,int ret)
{
  return (IC[imminent][ret].cpt!=0);
}

Event FreeDCon::post(int imminent,int ret,double t)
{
  Event ev;
  ev.data.p=(void *) (&IC[imminent][ret]);
  ev.nb=0;
  ev.t=t+IC[imminent][ret].tab[0].delay;
  ev.iterator=this;
  return ev;
}


bool FreeDCon::has_next(const Event * ev)
{
  return (((DTargets*)ev->data.p)->cpt)!=ev->nb+1;
}

Event FreeDCon::next(const Event *ev)
{
  // TODO lisiblifier :-)
  Event e=*ev;
  DTargets * tgt=(DTargets*) e.data.p;
  e.t=e.t-tgt->tab[e.nb].delay+tgt->tab[e.nb+1].delay;
  e.nb++;
  return e;
}

int FreeDCon::get_comp(const Event * ev)
{
  return ((DTargets*) ev->data.p)->tab[ev->nb].comp;
}

int FreeDCon::get_port(const Event * ev)
{
  return ((DTargets*) ev->data.p)->tab[ev->nb].port;
}
