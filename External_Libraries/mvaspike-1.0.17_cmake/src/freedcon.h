#ifndef FREEDCON_H
#define FREEDCON_H
#include <vector>
#include "dconnections.h"
#include "event.h"

class FreeDCon: public DConnections {
 protected:
  class Target {
  public:
    int comp;
    int port;
  };

  class DTarget : public Target
  {
  public:
    double delay;
    bool operator < (const DTarget& t) const
    { 
      return (delay<t.delay);
    }
  };

  class TTmpcon {
  public:
    Target from;
    DTarget to;
    TTmpcon(int comp1, int port1, int comp2, int port2,double d) { 
      from.comp=comp1;
      from.port=port1;
      to.comp=comp2;
      to.port=port2;
      to.delay=d;
    }
  };

  class DTargets {
  public:
    int cpt;
    DTarget * tab;
  };

  std::vector<TTmpcon> *tmp_IC;
  DTargets **IC;
  IfaceBase *from; // remembers the component outputs we are connecting from

  // utilities
  void delete_IC(); // destroy the connection 'matrix' (IC). Used by ~FreeDCon() and clean().

 public:
  FreeDCon();
  ~FreeDCon();
  void connect(int comp1, int port1, int comp2, int port2,double delat);
  void init(IfaceBase * from,IfaceBase *to);
  void clean(IfaceBase * from,IfaceBase *to);

  bool has_post(int imminent,int ret);
  Event post(int imminent,int ret,double t);

  bool has_next(const Event * ev);
  Event next(const Event * ev);

  int get_comp(const Event * data);
  int get_port(const Event * data);
  double get_time(const Event * data);
};

#endif
