#include "simpleiface.h"

SimpleIface::SimpleIface(int nbc,int nbp)
{
  nb_comp=nbc;
  nb_port=nbp;
}

SimpleIface::~SimpleIface()
{
}

int SimpleIface::get_nb_comp(void)
{
  return nb_comp;
}

int SimpleIface::get_nb_port(int comp)
{
  return nb_port;
}
