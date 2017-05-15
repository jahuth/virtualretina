#ifndef H5WRAPPER_H
#define H5WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* This file is mostly useless... I know.
Its only role is to provide an access to the hdf5 library that can be used 
WITHOUT h5cc; that way, we can compile h5wrapper using hdf5 (h5cc) tools, 
and compile e.g. mvaspike tools using mvaspike tools (any c++ compiler) ... 
I hate it, but its the simplest solution. O.R.*/

typedef struct	{
  int neuron_id;
  double spike_time;
} H5Spike;


  /* ****************************************** 
	 initialize things: Must be called first !
   ****************************************** */
  void hdf5_init(void);

  /* create an (initially empty) hdf5 file */
  int create_hdf5(const char* name);

  /* create a table in the file, fill it with spikes. 
	 Return value: hdf5 error code (negative=error). See .c for more info */	
  int save_spikes(int file_id,const char * title, const char * table_name,H5Spike * data, int datalen);

  /* close the hdf5 file */
  void close_hdf5(int file_id);

  /* open and read a file. See code for more info. */
  int load_spikes(const char * name,const char * table_name,H5Spike ** data, int *datalen);

#ifdef __cplusplus
}
#endif

#endif
