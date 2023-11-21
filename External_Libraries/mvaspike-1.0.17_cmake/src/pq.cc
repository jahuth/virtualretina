#include "pq.h"
#include <stdlib.h>

PQ::PQ()
{
  chunk=1024; // TODO guess ? double ?
  len=0;
  maxlen=chunk;
  tab=(Event*)malloc(sizeof(Event)*chunk);
}

PQ::~PQ(){
  free(tab);
}

bool PQ::empty(void)
{
  return (len==0);
}

const Event * PQ::top(void)
{
  return &tab[0];
}

int PQ::top_nb(void)
{
  return tab[0].nb;
}

double PQ::top_t(void)
{
  return tab[0].t;
}

void * PQ::top_data(void)
{
  return tab[0].data.p;
}

void PQ::change_first(Event ev)
{
  // TODO : optimiser, c'est fait pour ça...
  //Event ev=tab[0];
  pop();
  push(ev);
}

void PQ::pop()
{  
  int i,j;
  len--;
  Event e=tab[len];

  j=0;
  i=1;
  while (i<len) {
    if ((i<len-1) && ( tab[i+1].t < tab[i].t)) i++;
    if (e.t<tab[i].t) break;
    tab[j]=tab[i];
    j=i;
    i=i*2+1;
  }
  tab[j]=e;
}

void PQ::push(Event ev)
{
  int i;
  if (len==maxlen) {
    maxlen+=chunk;
    tab=(Event*)realloc(tab,sizeof(Event)*maxlen);
  }
  i=len;
  while ((i>0) && (tab[(i-1)/2].t>ev.t)){
    tab[i]=tab[(i-1)/2];
    i=(i-1)/2;
  }
  tab[i]=ev;
  len++;
}

int PQ::get_size(void)
{
  return len;
}
