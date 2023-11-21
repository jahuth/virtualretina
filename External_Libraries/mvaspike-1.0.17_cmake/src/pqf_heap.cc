#include <math.h>
#include "pqf_heap.h"
using namespace std;

void PQF_heap::init(vector<Devs*> *comp)
{
  PQF::init(comp);

  tab=new PEvent[size];
  index=new int[size];

  for (int i=0;i<size;i++)
    {
      tab[i].time=HUGE_VAL;
      tab[i].comp=i;
      index[i]=i;
    }

  for (int i=0;i<size;i++)
    {
      change(i,(*comp)[i]->ta());
    }
}

PQF_heap::PQF_heap()
{
  tab=NULL;
  index=NULL;
}


PQF_heap::~PQF_heap(void)
{
  if (tab!=NULL)
    delete[] tab;
  if (index!=NULL)
    delete[] index;
}

void PQF_heap::change(int ind,double t)
{
  int i;
  int n=index[ind];

  if (tab[(n-1)/2].time>t) {
    i=n;
    while ((i>0) && (tab[(i-1)/2].time>t)){
      tab[i].time=tab[(i-1)/2].time;
      tab[i].comp=tab[(i-1)/2].comp;
      index[tab[i].comp]=i;
      i=(i-1)/2;
    }
    tab[i].time=t;
    tab[i].comp=ind;
    index[ind]=i;
  } else {
     int j=n;
     i=n*2+1;
     while (i<size) {
       if ((i<size-1) && ( tab[i+1].time < tab[i].time)) i++;
       if (t<tab[i].time) break;
       tab[j].comp=tab[i].comp;
       tab[j].time=tab[i].time;
       index[tab[j].comp]=j;
       j=i;
       i=i*2+1;
     }
     tab[j].comp=ind;
     tab[j].time=t;
     index[ind]=j;
  }
}

double PQF_heap::min(void)
{
  return tab[0].time;
}

int PQF_heap::imin(void)

{
  return tab[0].comp;
}

void PQF_heap::delta_ext(int comp)
{
  double ts=(*components)[comp]->ta();
  change(comp,ts);
}


void PQF_heap::delta_int(int comp)
{
 
  // TODO : attention, vérifier que min a été appelé (en mode debug)
  change(tab[0].comp,(*components)[tab[0].comp]->ta());
}
