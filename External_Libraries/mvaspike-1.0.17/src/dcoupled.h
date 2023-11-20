#ifndef DCOUPLED_H
#define DCOUPLED_H	
#include "containercoupled.h"
#include "freedcon.h"
#include "freecon.h"
#include <vector>
#include "pq.h"
#include "connections.h"
#include "dconnections.h"
#include "pqf_heap.h"
#include "simpledconnection.h"
// Coupled DEVS with delayed connections

class DCoupled: public ContainerCoupled {
 private:
  static const int EV_NEURONS=0;
  static const int EV_LINKS=1;
 protected:
  int imminent;
  int ev_type;
  PQ pq;
  // delayed connections
  std::vector<DConnections *> con;
  // outputs
  std::vector<DConnections *> eoc;
  // inputs
  std::vector<DConnections *> eic;
  // Ifaces
  IfaceBase * inface;
  IfaceBase *outface;
  IfaceBase *eicface;
  IfaceBase *eocface;
  PQF_heap pqf;
  // default Dconnections : convenient, little overhead -> keep it
  FreeDCon defdcon;
  FreeDCon defdeic;
  FreeDCon defdeoc;
  // temporary simpledconnections
  std::vector<SimpleDConnection*> tmp_con;
  std::vector<SimpleDConnection*> tmp_eic;
  std::vector<SimpleDConnection*> tmp_eoc;

  double tn;

  void merge_tmp_con(void);

public:
  DCoupled();
  virtual ~DCoupled();
  void init();
  void clean();

  double ta(void);  
  int get_imin(void);

  int delta_int(double t);
  void delta_ext(double t,int x);

  int add_con(DConnections *con);
  int add_con(DConnections &con);
  int add_eic(DConnections *con);
  int add_eic(DConnections &con);
  int add_eoc(DConnections *con);
  int add_eoc(DConnections &con);
  void connect(int c1,int p1,int c2,int p2,double delay);
  void connect_in(int p1,int c2,int p2,double delay);
  void connect_out(int c1,int p1,int p2,double delay);
  int connect(SimpleDConnection *s);
  int connect_in(SimpleDConnection *s);
  int connect_out(SimpleDConnection *s);
  void local_event(int *path);
  void custom_event(int *path);
};
#endif
