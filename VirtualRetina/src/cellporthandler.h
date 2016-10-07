#ifndef CELLPORTHANDLER_H
#define CELLPORTHANDLER_H

#include "instantsynapse.h"

class CellPortHandler
{
protected:

  vector<InstantSynapse*> inputSynapses;
  vector<double*> inputNernst;
  int lastPort;

public:

  CellPortHandler()
  {
    inputSynapses.push_back(new InstantSynapse);
    inputNernst.push_back(0);
    lastPort=0;
  }


  virtual ~CellPortHandler(){}


  virtual int new_currentInput(InstantSynapse *syn=0)
  {
    if(syn==0)
      inputSynapses.push_back(new InstantSynapse);
    else
      inputSynapses.push_back(syn);
    inputNernst.push_back(0);
    lastPort++;
    return lastPort;
  }



  virtual int new_ionicChannel(double valNernst, InstantSynapse *syn=0)
  {
    if(syn==0)
      inputSynapses.push_back(new InstantSynapse);
    else
      inputSynapses.push_back(syn);
    inputNernst.push_back(new double(valNernst));
    lastPort++;
    return lastPort;
  }

  InstantSynapse& inputSynapse(int port=0)
  {
    if(port>lastPort)
      cerr<<"Error in CellPortHandler::inputSynapse(port): Asking for a synaptic port that does not exist."<<endl;
    return *(inputSynapses[port]);
  }

};


#endif

















