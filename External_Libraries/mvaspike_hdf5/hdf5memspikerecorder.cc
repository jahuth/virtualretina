#include "h5wrapper.h"
#include "hdf5memspikerecorder.h"
#include <mvaspike.h>

#ifdef MVASPIKE2
using namespace mvaspike;
#endif
#include <iostream>
using namespace std;

HDF5MemSpikeRecorder::HDF5MemSpikeRecorder() : MemSpikeRecorder()
{
  hdf5_init();
}

void HDF5MemSpikeRecorder::save_to_hdf5(const char * filename)
{
  int fid=create_hdf5(filename);
  int len=spikes.size();
  // TODO: we can be more conservative (memory-wise) if using other h5 functions...
  // saving chunks of data etc.
  //cout << "mem cpy starting" <<endl;

  H5Spike *h5spikes=new H5Spike[len];
  for (unsigned int i=0;i<len;i++)
	{
	  h5spikes[i].neuron_id=get_spike_id(i);
	  h5spikes[i].spike_time=get_spike_t(i);
	}
  //cout << "save starting" <<endl;
  int err=save_spikes(fid,"Spikes","Spikes",h5spikes,len);
  //cout << "save finished" << endl;
  delete[] h5spikes;
  if (err<0)
	fatal("Cannot save spikes to HDF5 file...");
  close_hdf5(fid);
}
