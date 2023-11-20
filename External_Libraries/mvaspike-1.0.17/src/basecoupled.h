#ifndef BASECOUPLED_H
#define BASECOUPLED_H
#include "devs.h"
class BaseCoupled : public Devs
{
 public:
  virtual ~BaseCoupled(){}
  virtual int get_nb_comp(void)=0;
  virtual Devs& get_comp(int nb)=0;
  // TODO operator [] ?
};

#endif
