#include "freecon.h"
#include <vector>
#include <algorithm>
#include "event.h"
#include "error.h"
using namespace std;

Freecon::Freecon() : Connections()
{
  tmp_IC=new vector<TTmpcon>;
  IC=NULL;
}

Freecon::~Freecon() {
  // TODO état...
  if (tmp_IC!=NULL)
    delete tmp_IC;
}

void Freecon::connect(int comp1, int port1, int comp2, int port2)
{
  if (tmp_IC==NULL) {
    // TODO : virer, gérer l'état prep / l'état running à la place.
    tmp_IC=new vector<TTmpcon>;
  }
  tmp_IC->push_back(TTmpcon(comp1,port1,comp2,port2));
}

void Freecon::init(IfaceBase * from,IfaceBase *to)
{
  // TODO : choose the best allocation scheme (compress or not ?)
  // default is : not compressed
  // TODO : si on peut optimiser aussi la gestion de empty()...

  IC=new Targets*[from->get_nb_comp()];

  for (int i=0; i<from->get_nb_comp();i++)
    {
      if (from->get_nb_port(i)!=0)
	{
	  IC[i]=new Targets[from->get_nb_port(i)];
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
	fatal("Freecon : connection from an unknown component");
      if ((*tmp_IC)[i].from.port>=from->get_nb_port((*tmp_IC)[i].from.comp))
	fatal("Freecon : connection from an unknown port");

      IC[(*tmp_IC)[i].from.comp][(*tmp_IC)[i].from.port].cpt++;
    }
  for (int i=0; i<from->get_nb_comp();i++) {
    for (int j=0;j<from->get_nb_port(i);j++)
      {
	IC[i][j].tab=new Target[IC[i][j].cpt];
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

  delete tmp_IC;
  tmp_IC=NULL;
}
void Freecon::clean(IfaceBase * from,IfaceBase *to)
{
  // TODO .. invert init
}

bool Freecon::has_post(int imminent,int ret)
{
  return (IC[imminent][ret].cpt!=0);
}

Ivent Freecon::post(int imminent,int ret)
{
  Ivent ev;
  ev.data.p=(void *) (&IC[imminent][ret]);
  ev.nb=0;
  return ev;
}


bool Freecon::has_next(Ivent * ev)
{
  return (((Targets*)ev->data.p)->cpt)!=ev->nb+1;
}

void Freecon::next(Ivent *ev)
{
  ev->nb++;
  // TODO : vérifier que ev->nb est déja mis à jour
}

int Freecon::get_comp(const Ivent * ev)
{
  return ((Targets*) ev->data.p)->tab[ev->nb].comp;
}

int Freecon::get_port(const Ivent * ev)
{
  return ((Targets*) ev->data.p)->tab[ev->nb].port;
}

int Freecon::empty(void)
{
  if (tmp_IC==NULL)
    fatal("Freecon::empty() cannot be called after init()");

  return tmp_IC->empty();
}
