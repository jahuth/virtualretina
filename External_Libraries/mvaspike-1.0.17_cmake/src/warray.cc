#include "warray.h"
#include "error.h"
#include <algorithm>
#include <iostream>
using namespace std;

Warray::Warray(void)
{
  w=NULL;
  tmp_w=new vector<TMP_W>;
}

Warray::~Warray(void)
{
  if (w!=NULL) delete[] w;
  else delete tmp_w;
}
void Warray::init(unsigned int len)
{

  w=new double[len];
  fill_n(w,len,0.0);
  for (unsigned int i=0; i<tmp_w->size();i++)
    {
      unsigned int port=(*tmp_w)[i].port;
      if (port>=len)
	fatal("Warray::init : cannot initialize an undeclared port");
      w[port]=(*tmp_w)[i].w;
    }
  delete tmp_w;

}

void Warray::set_w(unsigned int port,double w)
{
  if (this->w!=NULL)
    this->w[port]=w;
  else
    tmp_w->push_back(TMP_W(port,w));
}

// TODO use WeightedInput internally
int Warray::set_w(WeightedInput wi)
{
  tmp_w->push_back(TMP_W(wi.port,wi.weight));
  return wi.port;
}

double Warray::get_w(unsigned int port)
{
    // should work before and after init.
    // TODO : bound checking (first case)
  if (w!=NULL)
    return w[port];
  else
    {
      double r=0;
      bool found=false;
      for (unsigned int i=0;i<tmp_w->size();i++)
	{
	  if ((*tmp_w)[i].port==port)
		{
		  r=(*tmp_w)[i].w;
		  found=true;
		}
	}
      if (!found)
		fatal("Warray::get_w() : weight not set");
      return r;
    }
}

int Warray::guess_nb_inport(void)
{
  unsigned int max=0;
  for (unsigned int i=0;i<tmp_w->size();i++)
    {
      if ((*tmp_w)[i].port>max)
	max=(*tmp_w)[i].port;
    }
  return max+1;
}
