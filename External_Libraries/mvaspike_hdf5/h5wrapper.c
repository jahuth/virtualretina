#include "h5wrapper.h"
#include "H5TA.h"
#include <stdlib.h>

static const char * field_names[2] = {"neuron_id","spike_time"};
static const size_t dst_size=sizeof( H5Spike );
static const size_t dst_offset[2]={HOFFSET(H5Spike,neuron_id), HOFFSET(H5Spike,spike_time)};
static const size_t dst_sizes[2]={sizeof(int),sizeof(double)};
static hid_t field_type[2];


/* hdf5_init() must be called prior to any other function in this file ! */
void hdf5_init(void)
{
  field_type[0]=H5T_NATIVE_INT;
  field_type[1]=H5T_NATIVE_DOUBLE;
}

/* Open (create) a HDF5 file, named 'name', using the (currently incomplete!) fa7 convention */
int create_hdf5(const char* name)
{  
  hid_t file_id;	
  file_id=H5Fcreate(name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
  // TODO: use a local typedef to handle return type/ convert to/from hid_t
  return file_id;
}

/* Create a table in one go. 'file_id' must have been opened by create_hdf5; 
   'title' and 'table_name' are currently user-defined, it might change in the future
   (use "Spikes" for both in doubt; add comments to 'title' if needed)
   'data' is an  array of H5Spike of size 'datalen'
 */
int save_spikes(int file_id,const char * title, const char * table_name,H5Spike * data, int datalen)
{
  hsize_t chunk_size=100000;             
  int *fill_data=NULL;               
  int compress=1;
  herr_t status;
  status=H5TBmake_table(title,file_id,table_name,(hsize_t)2,(hsize_t)datalen,dst_size, 
						field_names, dst_offset, field_type,
						chunk_size, fill_data, compress, data);
  return status;
}

/* close a hdf5 file created by create_hdf5() */
void close_hdf5(int file_id)
{
  H5Fclose(file_id);
}


/* read a dataset from table 'table_name' in file 'name'.
   The results ('*datalen' spikes) is returned in '*data'.
   data should be freed by the user when not needed anymore UNLESS
   load_spikes returns an error (return value<0); in that case, data is undefined
   and must not be used, read, touched, looked at, freed, mentioned, sold, bought, not even in your dreams.
   So is *datalen in that case.
*/
int load_spikes(const char * name,const char * table_name,H5Spike ** data, int *datalen)
{
  hid_t file_id;
  herr_t status;
  file_id=H5Fopen(name,H5F_ACC_RDONLY,H5P_DEFAULT);
  hsize_t nfields_out;
  hsize_t nrecords_out;
  status=H5TBget_table_info(file_id,table_name,&nfields_out,&nrecords_out);
  if (status<0)
	return status;
  *data=malloc(nrecords_out*sizeof(H5Spike));
  *datalen=nrecords_out;
  status=H5TBread_table(file_id,table_name,dst_size,dst_offset,dst_sizes,*data);
  if (status<0)
	{
	  free(*data);
	}
  H5Fclose( file_id );
  return status;
}
