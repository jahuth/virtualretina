#include <math.h>
#include "pqf.h"
#include "devs.h"
#include <vector>

using namespace std;


PQF::PQF()
{
  components=NULL;
  size=0;
  tmin=-1;
}

PQF::~PQF(void)
{

}

void PQF::init(vector<Devs*> *comp)
{
  components=comp;
  size=comp->size();
}

double PQF::min(void)
{
  if (tmin==-1) {
    tmin=HUGE_VAL;
    ind=-1;
    for (int i=0;i<size; i++) 
      {
	double ts=(*components)[i]->ta();
	if (ts<tmin) {
	  tmin=ts;
	  ind=i;
	}
      }
  }     
  return tmin;
}

int PQF::imin(void)
{
  // TODO : attention, vérifier que min a été appelé (en mode debug)
  return ind;
}

void PQF::delta_ext(int comp)
{
  double ts=(*components)[comp]->ta();
 
  if (tmin!=-1) { 
    if (ts<tmin) {
      tmin=ts;
      ind=comp;
    } else if (ind==comp) {
      tmin=-1;
    }
  }
}

void PQF::delta_int(int comp)
{
  tmin=-1;
}

inline int PQF::get_size(void)
{
  return size;
}
