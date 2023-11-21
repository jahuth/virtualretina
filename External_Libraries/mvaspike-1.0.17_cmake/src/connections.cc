#include "connections.h"

Connections::Connections() {}

Connections::~Connections() {}

int Connections::get_nb_edges(IfaceBase * from)
{
  // TODO : template pour parcours réutilisable ...
  int r=0;
  for (int i=0; i< from->get_nb_comp(); i++)
    for (int j=0; j < from->get_nb_port(i); j++)
      {
	if (has_post(i,j))
	  {
	    Ivent iv=post(i,j);
	    r++;
	    while (has_next(&iv))
	      {
		next(&iv);
		r++;
	      }

	  }
      }
  return r;
}
