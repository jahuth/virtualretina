#ifndef RECFILTER_H
#define RECFILTER_H

#include "baserecfilter.h"
#include "cellporthandler.h"

//This class performs recursive filtering on a 1d temporal input.

class RecFilter: public BaseRecFilter
{
protected:

  double** last_inputs;
  double** last_values;
  double initial_input_value;
  virtual void allocateValues(void);
  
public:
  RecFilter( double stepp=1 );
  RecFilter( const RecFilter& yo );
  RecFilter( const BaseRecFilter& yo );
  virtual ~RecFilter(void);
  RecFilter& operator=(const BaseRecFilter& yo);
  
  RecFilter& set_initial_input_value (double val);
  
  //DRIVING FUNCTIONS:

  //adds new_input to the inputs of the map for the next time step.
  virtual void feedInput( double new_input );

  //proceeds to temporal integration of the map's new input.
  virtual void tempStep(void);

  //ACCESS FUNCTION:
  double read(int n=0) ;
  
};

#endif

