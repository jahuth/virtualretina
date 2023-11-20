#include "wstatic.h"
#include <algorithm>
using namespace std;

Wstatic::Wstatic(void)
{
  w=0;
}

void Wstatic::set_w(double weight)
{
  w=weight;
}

void Wstatic::init(int nb_weight)
{
}

double Wstatic::get_w(int port)
{
  return w;
}
