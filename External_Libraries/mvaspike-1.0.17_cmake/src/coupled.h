#ifndef COUPLED_H
#define COUPLED_H	
#include "containercoupled.h"
#include "freecon.h"
#include <vector>

#include "connections.h"

class Coupled: public ContainerCoupled {
protected:
  int imminent;
  double tn;
  // connections
  std::vector<Connections *> con;
  // outputs
  std::vector<Connections *> eoc;
  // inputs
  std::vector<Connections *> eic;
  // Ifaces
  IfaceBase * inface;
  IfaceBase *outface;
  IfaceBase *eicface;
  IfaceBase *eocface;
  // default connections : convenient, little overhead -> keep it
  Freecon defcon;
  Freecon defeic;
  Freecon defeoc;
public:
  Coupled();
  virtual ~Coupled();
  void init();
  void clean();

  double ta(void);
  int get_imin(void);

  int delta_int(double t);
  void delta_ext(double t,int x);

  void add_con(Connections *con);
  void add_eic(Connections *con);
  void add_eoc(Connections *con);
  void add_con(Connections &con);
  void add_eic(Connections &con);
  void add_eoc(Connections &con);
  void connect(int c1,int p1,int c2,int p2);
  void connect_in(int p1,int c2,int p2);
  void connect_out(int c1,int p1,int p2);
};
#endif
