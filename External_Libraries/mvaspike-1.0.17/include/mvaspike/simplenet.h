#ifndef SIMPLENET_H
#define SIMPLENET_H
#include "dcoupled.h"
#include "freedcon.h"
#include "neuron.h"
#include "root.h"

template <class TNeuron>
class SimpleNet : public Root
{
  protected:
    TNeuron *neurons;
    FreeDCon f;
    DCoupled dc;
    int nb;
  public:
    SimpleNet(int nb);
    ~SimpleNet(void);
    void init(void);
    void connect(int src,int dest,int dest_port,double delay);
    TNeuron* at(int id);

    TNeuron& operator[](int id)
        {
            return neurons[id];
        }
};
template <class TNeuron>
SimpleNet<TNeuron>::SimpleNet(int nb)
    : Root()
{
    this->nb=nb;
    neurons=new TNeuron[nb];
    for (int i=0;i<nb;i++)
        dc.add_component(neurons+i);
}

template <class TNeuron>
SimpleNet<TNeuron>::~SimpleNet(void)
{
    delete[] neurons;
}


template <class TNeuron>
void SimpleNet<TNeuron>::init(void)
{
    set_system(&dc);
    dc.add_con(&f);
    Root::init();
}

template <class TNeuron>
void SimpleNet<TNeuron>::connect(int src,int dest,int dest_port,double delay)
{
    f.connect(src,0,dest,dest_port,delay);
}

template <class TNeuron>
TNeuron* SimpleNet<TNeuron>::at(int id)
{
    return neurons+id;
}

#endif
