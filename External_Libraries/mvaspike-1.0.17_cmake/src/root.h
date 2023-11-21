#ifndef ROOT_H
#define ROOT_H
#include "baserecorder.h"
#include "devs.h"
#include "pq.h"

class Root
{
  // root simulator
  // its aim is to run/control the simulation of a 
  // given DEVS system
  // WARNING : singleton
 protected:
  // TODO : clean static/singleton based
  static double t;
  static class Devs * child;
  static BaseRecorder *rec;
  static class Root * me;
  static PQ pq;
  static PQ pqstom;
  //TODO : static ?
  int **paths;
  int *path_lens;

  void init_path(void);
 public:
  Root();
  virtual ~Root();
  void set_system(Devs * child);
  void set_system(Devs & child);

  virtual void init(void);
  void iter(void);
  void iter(int nb);
  void run(double t);
  double get_t(void);
  double get_next_t(void);
  void record(BaseRecorder *rec);
  void record(BaseRecorder &rec);
  int get_path(int id,int pos);
  int get_path_len(int id);
  int get_nb_comp(void);
// TODO transformer ca en protected et friend de qqchose (devs?)
  static Root * get_instance(void);
  //void push_event(int devs_id,Event *ev);
  static void push_local_event(Event ev);
  static const Event * get_local_event(void);

  static void push_custom_event(Event ev);
  static const Event * get_custom_event(void);

};

#endif
