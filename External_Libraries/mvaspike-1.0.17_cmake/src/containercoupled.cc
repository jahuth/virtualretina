#include "containercoupled.h"
#include "root.h"
ContainerCoupled::~ContainerCoupled()
{
}

int ContainerCoupled::get_nb_comp(void)
{
  return components.size();
}

Devs& ContainerCoupled::get_comp(int nb)
{
  return *components[nb];
}

void ContainerCoupled::visit(void (*f)(Devs *d,int,int),int level)
{
  f(this,id,level);
  for (unsigned int i=0;i<components.size();i++)
    {
      components[i]->visit(f,level+1);
    }
}

void ContainerCoupled::local_event(int *path)
{
  components[*path]->local_event(path+1);
}

void ContainerCoupled::custom_event(int *path)
{
  components[*path]->custom_event(path+1);
}

int ContainerCoupled::add_component(Devs * comp)
{
  components.push_back(comp);
  return components.size()-1;
}

int ContainerCoupled::add_component(Devs & comp)
{
  return add_component(&comp);
}

void ContainerCoupled::init_record(BaseRecorder *r)
{
  // update rec according to default rule (devs.cc)
  Devs::init_record(r);
  // propagate the updated rec (not necessarily r)
  for (unsigned int i=0;i<components.size();i++)
    components[i]->init_record(rec);
}

int ContainerCoupled::get_local_id(int global_id)
{
  // TODO for now : using global root::paths in a tricky way
  // might pobably be somewhat improved (eg. finding patterns
  // in global_ids, ...)
  return Root::get_instance()->get_path(global_id,-1);
}
