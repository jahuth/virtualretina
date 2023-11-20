#include "flow.h"
#include <stdarg.h>


// TODO factoriser constructeurs
// TODO save one int for accu_ (w[0] never used)

Flow::Flow(int nb_channel,...)
{
  this->nb_channel=nb_channel;  
  channel_width=new int[nb_channel];
  accu_div=new int[nb_channel];
  va_list va;
  va_start(va,nb_channel);
  for (int i=0;i<nb_channel;i++)
    {
      channel_width[i]=va_arg(va,int);
    }
  accu_div[nb_channel-1]=1;
  for(int i=nb_channel-2;i>=0;i--)
    {
      accu_div[i]=accu_div[i+1]*channel_width[i+1];
    }
  va_end(va);
}

Flow::Flow(int nb_channel,int * cw)
{
  this->nb_channel=nb_channel;
  channel_width=new int[nb_channel];
  accu_div=new int[nb_channel];
  for (int i=0;i<nb_channel;i++)
    {
      channel_width[i]=cw[i];
    }
  accu_div[nb_channel-1]=1;
  for(int i=nb_channel-2;i>=0;i--)
    {
      accu_div[i]=accu_div[i+1]*channel_width[i+1];
    }
}

Flow::~Flow(void)
{
  delete[] channel_width;
  delete[] accu_div;
}

int Flow::flatten(int *x)
{
  int r=x[0];
  for(int i=1;i<nb_channel;i++)
    {
      r=r*channel_width[i]+x[i];
    }
  return r;
}

int Flow::flatten(int x,...)
{
  if (nb_channel==1)
    return x;
  else {
    int r=x;
    va_list va;
    va_start(va,x);
    for(int i=1;i<nb_channel;i++)
      {
	r=r*channel_width[i]+va_arg(va,int);
      }
    va_end(va);
    return r;
  }
}

int Flow::deflatten(int key,int nb)
// TODO change name : get/at/etc is better
{
  if (nb_channel==1)
    return key;
  else {
    int r=key;
    r=r/accu_div[nb];

    if (nb==0)
      return r;
    else
      return r%channel_width[nb];
  }
}

int Flow::get_width(int key)
{
  return channel_width[key];
}

int Flow::get_max(void)
{
  return  accu_div[0]*channel_width[0];
}

int Flow::set(int vec,int pos,int value)
{
  if (pos==0)
    return value*accu_div[0]+vec%accu_div[0];
  else
    return vec-vec%accu_div[pos-1]+value*accu_div[pos]+vec%accu_div[pos];
}
