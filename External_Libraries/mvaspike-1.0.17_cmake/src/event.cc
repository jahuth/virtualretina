#include "eventiterator.h"

Event::Event(void)
{
  nb=0;
  data.p=NULL;
  t=0;
  iterator=NULL;
}


Event::Event(double t)
{
  nb=0;
  data.p=NULL;
  this->t=t;
  iterator=NULL;
}

Event::Event(int nb,double t)
{
  this->nb=nb;
  data.p=NULL;
  this->t=t;
  iterator=NULL;
}

Event::Event(Ivent iv,double t)
{
  this->nb=iv.nb;
  this->data=iv.data;
  this->t=t;
  this->iterator=iv.iterator;
}

Event::Event(int nb, void * data,double t,EventIterator *ei)
{
  this->nb=nb;
  this->data.p=data;
  this->t=t;
  this->iterator=ei;
}

Event::Event(int nb, int data,double t,EventIterator *ei)
{
  this->nb=nb;
  this->data.i=data;
  this->t=t;
  this->iterator=ei;
}

bool Event::has_next(void) const
{
  return iterator->has_next(this);
}

Event Event::next(void) const
{
  return iterator->next(this);
}

int Event::get_comp(void) const
{
  return iterator->get_comp(this);
}

int Event::get_port(void) const
{
  return iterator->get_port(this);
}
