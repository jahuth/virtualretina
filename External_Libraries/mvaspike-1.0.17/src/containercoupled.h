#ifndef CONTAINERCOUPLED_H
#define CONTAINERCOUPLED_H
// TODO en faire un template
#include "basecoupled.h"
#include <vector>

class ContainerCoupled : public BaseCoupled
{
 protected:
  std::vector<Devs*> components;
 public:
  virtual ~ContainerCoupled();
  int get_nb_comp(void);
  Devs& get_comp(int nb);
  void visit(void (*f)(Devs *,int,int),int level);
  void local_event(int *path);
  void custom_event(int *path);
  int add_component(Devs * comp);
  int add_component(Devs & comp);  
  int get_local_id(int global_id);  
  void init_record(BaseRecorder *rec);

};

#endif
