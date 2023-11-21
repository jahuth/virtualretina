#ifndef WARRAY_H
#define WARRAY_H
#include <vector>
#include "weightedinput.h"
#include "baserecorder.h"
// an array of weights.

class Warray {
 protected:
    // temporary : storing declarations
  class TMP_W{
  public:
    unsigned int port;
    double w;
    TMP_W(int p,double w) { port=p; this->w=w;}
  };
  double *w;                                                       // the array
  std::vector<TMP_W> *tmp_w;               // memorize declarations

 public:
  Warray();
  ~Warray();
  void init(unsigned int len);                             // build the actual array
  void set_w(unsigned int port,double w);  // declare a weight
  int set_w(WeightedInput wi);
  double get_w(unsigned int port);               // retrieve a weight
  int guess_nb_inport(void);                          // guess a plausible lenght from declarations
  void delta_int(double t) {};
  void delta_ext(double t,int port) {};
  void record_state(BaseRecorder *) {};
};

#endif
