#include "devs.h"
#include <math.h>
#include <assert.h>
#include "error.h"
#include "root.h"
int Devs::NBDEVS=0;

Devs::Devs()
{
  nb_outport=0;
  nb_inport=0;
  id=NBDEVS;
  NBDEVS+=1;
  init_ok=false;
  rec=NULL;//new Nullrecorder();
}

void Devs::init()
{
  if (init_ok)
    fatal("Cannot init twice");
  init_ok=true;
}

void Devs::clean()
{
  if (!init_ok)
    fatal("Cannot clean() before init()");
  init_ok=false;
}

unsigned int Devs::add_inport(void)
{
  if (init_ok)
    {
      fatal("Cannot add in/out ports after init");
    }
  nb_inport++;
  return (nb_inport-1);
}

unsigned int Devs::add_outport(void)
{
 if (init_ok) 
   fatal("Cannot add in/out ports after init");
    
  nb_outport++;
  return (nb_outport-1);
}

unsigned int Devs::add_inport(unsigned int inc)
{ 
  if (init_ok) 
    fatal("Cannot add in/out ports after init");
  nb_inport+=inc;
  return (nb_inport-inc);
}

unsigned int Devs::add_outport(unsigned int inc)
{
  if (init_ok) 
    fatal("Cannot add in/out ports after init");
   
  nb_outport+=inc;
  return (nb_outport-inc);
}

void Devs::set_nb_inport(unsigned int nb)
{
  if (init_ok) 
    fatal("Cannot add in/out ports after init");
  nb_inport=nb;
}

void Devs::set_nb_outport(unsigned int nb)
{
  if (init_ok) 
    fatal("Cannot add in/out ports after init");
  nb_outport=nb;
}

unsigned int Devs::get_nb_outport(void)
{
  return nb_outport;
}

unsigned int Devs::get_nb_inport(void)
{
  return nb_inport;
}

bool Devs::is_final(void)
{
  return false;
}

bool Devs::is_population(void)
{
  return false;
}

void Devs::record(BaseRecorder *r)
{
  rec=r;
}

void Devs::init_record(BaseRecorder *r)
{
  if (rec==NULL)
    rec=r;
}

void Devs::local_event(int *path)
{
  // drop it by default
}

void Devs::custom_event(int *path)
{
  // drop it by default
}

void Devs::push_local_event(Event ev)
{
  Root::get_instance()->push_local_event(ev);
}

void Devs::record_state(void)
{
}

void Devs::visit(void (*f)(Devs *d,int,int),int level)
{
  f(this,id,level);
}

int Devs::get_nb_comp(void)
{
  return 0;
}

Devs& Devs::get_comp(int nb)
{
  return *this;
}

int Devs::get_imin(void)
{
  return 0;
}
