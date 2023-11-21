#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <vector>
using namespace std;
#include "devs.h"
#include "root.h"
#include "error.h"
#include "devsiterator.h"

// building Root as a not very classical Singleton...
// (Only one root in a program, but no private constructor
// as it leads to unreadable simulation for the user)

// TODO : template en dessous avec PQ comme <class T>
Root *Root::me=NULL;
Devs *Root::child=NULL;
BaseRecorder *Root::rec=NULL;
double Root::t=0.0;
PQ Root::pq;
PQ Root::pqstom;

Root::Root()
{
  if (me!=NULL)
    fatal("Cannot have two roots");
  me=this;
}

Root::~Root()
{
me=NULL;
}

void Root::set_system(Devs * devs)
{
  child=devs;
}

void Root::set_system(Devs & devs)
{
  child=&devs;
}

void Root::init(void)
{
  if (child!=NULL)
    {
      child->init();
      init_path();
      // child->visit(&Root::getmem);
      if (rec!=NULL)
	{
	  rec->init();
	  child->init_record(rec);
	}
      else
	child->init_record(NULL);
    }
  else {
    fatal("No DEVS system to initialize... use set_system() !");
  }
}

void Root::iter(void)
{
  t=child->ta();
  if ((pq.empty()||(t<pq.top_t())) 
      &&
      (pqstom.empty()||(t<pqstom.top_t()))) 
    {
        if (t==HUGE_VAL)
            fatal("root simulator : cannot iter(), no event to process.");
        if (rec!=NULL)
          rec->new_event(t);
        child->delta_int(t);
    }
  else 
    {
      if (pqstom.empty()|| ((!pq.empty())&&(pq.top_t()<pqstom.top_t())))
	{
	  t=pq.top_t();
	  if (rec!=NULL)
	    rec->new_event(t);
	  child->local_event(paths[pq.top_nb()]+1);
	  pq.pop();
	}
      else
	{
	  t=pqstom.top_t();
	  if (rec!=NULL)
	    rec->new_event(t);
	  child->custom_event(paths[pqstom.top_nb()]+1);
	  pqstom.pop();
	}
    }
}

void Root::iter(int nb)
{
  for (int i=0;i<nb;i++)
    iter();
}

void Root::run(double tmax)
{
  while (get_next_t()<=tmax) {
    iter();
  }
}

double Root::get_t(void)
{
  return t;
}

double Root::get_next_t(void)
{
    double lt=child->ta();
    if ((pq.empty()||(lt<pq.top_t())) && 
	(pqstom.empty()||(lt<pqstom.top_t()))) 
        return lt;
    else       
      if (pqstom.empty()|| ((!pq.empty())&&(pq.top_t()<pqstom.top_t())))
        return pq.top_t();
      else
	return pqstom.top_t();
}

void Root::record(BaseRecorder *r)
{
  // TODO assert init is not done (?)
  rec=r;
}

void Root::record(BaseRecorder &r)
{
  record(&r);
}


Root * Root::get_instance(void)
{
  return me;
}

void Root::push_local_event(Event ev)
{
  pq.push(ev);
}

void Root::push_custom_event(Event ev)
{
  pqstom.push(ev);
}

const Event * Root::get_local_event(void)
{
  return pq.top();
}

const Event * Root::get_custom_event(void)
{
  return pqstom.top();
}

void Root::init_path(void)
{

  int nb_popu=Devs::NBDEVS;
  paths=new int *[nb_popu];
  path_lens=new int[nb_popu];

  DevsIterator i(*child);
  vector<int> levels;
  int lactu=0;
  do
    {
     
      //cout << (*i).id << " " << i.get_level()<< endl;
      if (lactu<i.get_level())
	{
	  levels.push_back(i.get_nb());
	  lactu++;
	}
      else
	{
	  levels[i.get_level()-1]=i.get_nb();
	}

      path_lens[(*i).id]=i.get_level();
      paths[(*i).id] = new int[i.get_level()];
      //cout << "PATH " << (*i).id << " " <<path_lens[(*i).id]<< " - " ;
      for (int j=0;j <i.get_level();j++)
	{
	  paths[(*i).id][j]=levels[j];
	  //cout << paths[(*i).id][j] << ",";
	}
      //cout << endl;
    } while (i.next());

  return;
}

int Root::get_nb_comp(void)
{
  int nb=0;
  DevsIterator i(*child);
  do
    {
      nb+=(*i).get_nb_comp();
    } while (i.next());

  return nb+1;
}

int Root::get_path(int id,int pos)
{
  // TODO bounds checking ?
  if (pos>=0)
    return paths[id][pos];
  else
    return paths[id][path_lens[id]+pos];
}

int Root::get_path_len(int id)
{
  return path_lens[id];
}
