#include <math.h>
#include <assert.h>
#include "dcoupled.h"
#include "event.h"
#include "iface.h"
#include "simpleiface.h"

#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;


// TODO : build tmp_con on demand and delete when merged
// TODO WARNING : connot revert/access simpledconnections after init...

DCoupled::DCoupled() :
  ContainerCoupled()
{
}

DCoupled::~DCoupled()
{

}

double DCoupled::ta(void)
{
  //double tn;
  if (imminent==EV_UNKNOWN)
    {
      // TODO : tie-breaking / select !!!
      imminent=EV_NOP;
      tn=HUGE_VAL;
      ev_type=EV_NEURONS;
      
      tn=pqf.min();
      imminent=pqf.imin();
      
      if ((!pq.empty()) && (tn>pq.top_t()))
	{
	  ev_type=EV_LINKS;
	  tn=pq.top_t();
	}
    }
  return tn;
}

int DCoupled::get_imin(void)
{
  ta();
  return imminent;
}

int DCoupled::delta_int(double t)
{
  int ret;
  if (ev_type==EV_LINKS)
    {
      const Event *ev=pq.top();
      //DConnections * con=(DConnections*)ev->iterator;
      if (rec!=NULL){
          rec->add_delayed(id);
      }
      int comp=ev->get_comp();
      int ret=EV_NOP;
      if (comp>=0)
	{
	  components[comp]->delta_ext(t,ev->get_port());
	  pqf.delta_ext(comp);
	}
      else
	if (comp==ID_ELSEWHERE)
	  ret=ev->get_port();
	
      if (ev->has_next())
	  pq.change_first(ev->next());
	
      else
	pq.pop();

      imminent=EV_UNKNOWN;
      return ret;
    }
  else 
    {
      assert(imminent!=EV_NOP);
     
      ret=components[imminent]->delta_int(t);
      pqf.delta_int(imminent);
      // TODO : NO INSTANTANEOUS INTERNAL CONNECTIONS ?
      if (ret!=EV_NOP)
	for (unsigned int  i=0;i<con.size();i++)
	  if (con[i]->has_post(imminent,ret))
	    {
	      pq.push(con[i]->post(imminent,ret,t));
	    }
      // output
      // TODO : cleanup, multi-port, etc.
      
      if (ret==EV_NOP)
	{
	  imminent=EV_UNKNOWN;
	  return EV_NOP;
	}
      for (unsigned int i=0;i<eoc.size();i++)
	{
	  if (eoc[i]->has_post(imminent,ret))
	    {
	      pq.push(eoc[i]->post(imminent,ret,t));
	    }
	}
      imminent=EV_UNKNOWN;
      return EV_NOP;
    }
}

void DCoupled::delta_ext(double t, int x)
{
  for (unsigned int  i=0;i<eic.size();i++)
    if (eic[i]->has_post(0,x))
      {
	pq.push(eic[i]->post(0,x,t));
      }
  imminent=EV_UNKNOWN;
}

void DCoupled::merge_tmp_con(void)
{
  for (unsigned int i=0;i<tmp_con.size();i++)
	{
	  //cout << " " << tmp_con[i]->src<< " " <<tmp_con[i]->src_port<< " " <<
	  //		   tmp_con[i]->dest<< " " <<tmp_con[i]->dest_port<< " " <<tmp_con[i]->delay<< endl;
	  defdcon.connect(tmp_con[i]->src,tmp_con[i]->src_port,
					tmp_con[i]->dest,tmp_con[i]->dest_port,tmp_con[i]->delay);
	}
  for (unsigned int i=0;i<tmp_eic.size();i++)
	{
	  //cout << -1<< " " <<tmp_eic[i]->src_port<< " " <<tmp_eic[i]->dest<< " " <<tmp_eic[i]->dest_port << endl;
	  defdeic.connect(0,tmp_eic[i]->src_port,
					tmp_eic[i]->dest,tmp_eic[i]->dest_port,tmp_eic[i]->delay);
	}
  for (unsigned int i=0;i<tmp_eoc.size();i++)
	defdeoc.connect(tmp_eoc[i]->src,tmp_eoc[i]->src_port,
					-1,tmp_eoc[i]->dest_port,tmp_eoc[i]->delay);
}

void DCoupled::init()
{
  merge_tmp_con();
  con.push_back(&defdcon);
  eic.push_back(&defdeic);
  eoc.push_back(&defdeoc);
  for (unsigned int i=0; i<components.size(); i++)
    {
      components[i]->init();
    }
  
  Iface* i=new Iface(); 
  i->copy_out(&components);
  outface=i;
  
  i= new Iface();
  i->copy_in(&components);
  inface=i;
  
  eocface=new SimpleIface(1,nb_outport);
  eicface=new SimpleIface(1,nb_inport);
  
  for (unsigned int i=0;i<con.size();i++)
    con[i]->init(outface,inface);
  
  for (unsigned int i=0;i<eoc.size();i++)
    eoc[i]->init(outface,eocface);

  for (unsigned int i=0;i<eic.size();i++)
    eic[i]->init(eicface,inface);
  pqf.init(&components);
  imminent=EV_UNKNOWN;
}

void DCoupled::clean()
{
  Devs::clean();

  for (unsigned int i=0; i<components.size(); i++)
    {
      components[i]->clean();
    }
  
  for (unsigned int i=0;i<con.size();i++)
    con[i]->clean(outface,inface);

  for (unsigned int i=0;i<eoc.size();i++)
    eoc[i]->clean(outface,eocface);

  for (unsigned int i=0;i<eic.size();i++)
    eic[i]->clean(eicface,inface);

  delete outface;
  delete inface;
  delete eocface;
  delete eicface;

}
void DCoupled::connect(int c1,int p1,int c2,int p2,double d)
{  
  defdcon.connect(c1,p1,c2,p2,d);
}

void DCoupled::connect_in(int p1,int c2,int p2,double d)
{
  defdeic.connect(0,p1,c2,p2,d);
}

void DCoupled::connect_out(int c1,int p1,int p2,double d)
{
  defdeoc.connect(c1,p1,-1,p2,d);
}

int DCoupled::add_con(DConnections *con)
{
  this->con.push_back(con);
  return this->con.size()-1;
}

int DCoupled::add_con(DConnections &con)
{
  this->con.push_back(&con);
  return this->con.size()-1;
}


int DCoupled::add_eic(DConnections *con)
{
  this->eic.push_back(con);
  return this->eic.size()-1;
}

int DCoupled::add_eic(DConnections &con)
{
  this->eic.push_back(&con);
  return this->eic.size()-1;
}

int DCoupled::add_eoc(DConnections *con)
{
  this->eoc.push_back(con);
  return this->eic.size()-1;
}

int DCoupled::add_eoc(DConnections &con)
{
  this->eoc.push_back(&con);
  return this->eic.size()-1;
}

// these have been added to help txtmvaspike...
int DCoupled::connect(SimpleDConnection *s)
{
  tmp_con.push_back(s);	
  return tmp_con.size()-1;
}

int DCoupled::connect_in(SimpleDConnection *s)
{
  tmp_eic.push_back(s);	
  return tmp_eic.size()-1;
}

int DCoupled::connect_out(SimpleDConnection *s)
{
  tmp_eoc.push_back(s);	
  return tmp_eoc.size()-1;
}

void DCoupled::local_event(int *path)
{
  imminent=EV_UNKNOWN;
  ContainerCoupled::local_event(path);
  pqf.delta_ext(*path);
}

void DCoupled::custom_event(int *path)
{
  imminent=EV_UNKNOWN;
  ContainerCoupled::custom_event(path);
  pqf.delta_ext(*path);
}
