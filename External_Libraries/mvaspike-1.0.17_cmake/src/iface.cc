#include <vector>
#include <assert.h>
#include "devs.h"
using namespace std;
#include "iface.h"

Iface::Iface()
{
  nb_comp=0;
  port=NULL;
}

Iface::Iface(int nbc,int nbp)
{
  nb_comp=nbc;
  port=new int[nbc];
  for (int i=0;i<nbc;i++)
    port[i]=nbp;
}

Iface::~Iface()
{
  if (nb_comp!=0)
    delete[] port;
}

void Iface::copy_out(vector<Devs*>* v)
{
  assert(port== NULL);
  nb_comp=v->size();
  port=new int[nb_comp];  
  for (int i=0;i<nb_comp;i++)
    port[i]=(*v)[i]->get_nb_outport();
  
}

void Iface::copy_in(vector<Devs*>* v)
{
  assert(port==NULL);
  nb_comp=v->size();
  port=new int[nb_comp];
  for (int i=0;i<nb_comp;i++)
    port[i]=(*v)[i]->get_nb_inport();
  
}

int Iface::get_nb_comp(void)
{
  return nb_comp;
}

int Iface::get_nb_port(int comp)
{
  return port[comp];
}
