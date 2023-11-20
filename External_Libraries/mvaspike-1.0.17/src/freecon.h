#ifndef FREECON_H
#define FREECON_H
#include <vector>
#include "connections.h"
#include "event.h"

class Freecon: public Connections {
 protected:
  class Target {
  public:
    int comp;
    int port;
  };

  class TTmpcon {
  public:
    Target from;
    Target to;
    TTmpcon(int comp1, int port1, int comp2, int port2) { 
      from.comp=comp1;
      from.port=port1;
      to.comp=comp2;
      to.port=port2;
    }
  };

  class Targets {
  public:
    int cpt;
    Target * tab;
  };

  std::vector<TTmpcon> *tmp_IC;
  Targets **IC;

 public:
  Freecon();
  ~Freecon();
  void connect(int comp1, int port1, int comp2, int port2);
  void init(IfaceBase * from,IfaceBase *to);
  void clean(IfaceBase * from,IfaceBase *to);

  bool has_post(int imminent,int ret);
  Ivent post(int imminent,int ret);

  bool has_next(Ivent * ev);
  void next(Ivent * ev);

  int get_comp(const Ivent * data);
  int get_port(const Ivent * data);
  int empty(void);
};

#endif
