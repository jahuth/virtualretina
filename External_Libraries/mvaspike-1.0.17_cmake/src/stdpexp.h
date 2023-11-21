#ifndef STDPEXP_H
#define STDPEXP_H

#include "warray.h"
#include "baserecorder.h"
#include "lif.h"
#include <vector>
// an array of dynamic weights.
// using asymetrical exponential weight update function
class STDPExp : public Warray
{
  class TmpLearn
    {
    public:
      int port;
      bool flag;
      TmpLearn(int p,bool f) : port(p), flag(f) {}
    };
 protected:
  bool * stdpon;
  double lastf;
  double * lastr;
  double * accu_prepost;
  double accu_postpre;
  double W_prepost;
  double W_postpre;
  double Tau_prepost;
  double Tau_postpre;
  double boundm;
  double boundp;
  int wlen;
  std::vector<TmpLearn> tmp_learn;
 public:
  STDPExp(void);
  ~STDPExp(void);
  void delta_int(double t);
  void delta_ext(double t,int port);
  void init(int len);
  void record_state(BaseRecorder *rec);
  void set_bounds(double m,double p);
  void learn(int port,bool f);
  void set_tau(double prepost,double postpre);
  void set_dw(double wprepost,double wpostpre);
};


#endif
