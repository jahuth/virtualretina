#include <stdlib.h>
#include <stdio.h>
#include "h5wrapper.h"

int main()
{
  H5Spike *spikes;
  int nbspikes;
  int status;
  int i;
  hdf5_init();
  status=load_spikes("test.hdf5","Spikes",&spikes, &nbspikes);
  if (status<0)
	{
	  printf("error - load_spikes returned %d\n",status);
	  exit(EXIT_FAILURE);
	}
  printf ("%d spikes were read. First 10 (or less) are:\n",nbspikes);
  for (i=0;i< (nbspikes<10?nbspikes:10) ; i++)
	{
	  printf("%d %f\n",spikes[i].neuron_id,spikes[i].spike_time);
	}
  free(spikes);
  return EXIT_SUCCESS;
}
