#include "devsiterator.h"
using namespace std;

DevsIterator::DevsIterator(Devs &start)
{
  ds.push(nodeptr(&start,start.get_nb_comp()));
  level=1;
  nb=0;
}

Devs & DevsIterator::operator *(void)
{
  return *ds.top().first;
}

int DevsIterator::get_nb(void)
{
  return nb;
}

bool DevsIterator::next(void)
{
  int r=true;
  if (ds.top().second==0)
    {
      level--;
      ds.pop();
        if (ds.empty())
	return false;
      r=next();
    }
  else 
    {
      ds.top().second--;
      nb=ds.top().second;
      Devs *top=ds.top().first;
      ds.push(nodeptr(&top->get_comp(nb),top->get_comp(nb).get_nb_comp()));
      level++;
    }
  return r;
}

int DevsIterator::get_level(void)
{
  return level;
}
