#ifndef MODELS_H
#define MODELS_H

#include "lif.h"
#include "internal_lif.h"
#include "warray.h"
#include "wstatic.h"
#include "stdpexp.h"
#include "stdp_rbt.h"
typedef LifSkeleton<Internal_Lif,Warray> Lif;
typedef LifSkeleton<Internal_Lif,STDPExp> StdpExpLif; 
typedef LifSkeleton<Internal_Lif,STDP_RBT> StdpRBTLif; 


#endif
