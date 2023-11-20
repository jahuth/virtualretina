#ifndef FLOW_H
#define FLOW_H

// class Flow : handle compact int vectors
class Flow
{
 protected:
  int nb_channel;
  int *channel_width;
  int *accu_div;
 public:
  Flow(int nb_channel,...); // nb_channel is the dimension
  Flow(int nb_channel,int *channel_width);  // channel_width (or ...) 
                                            // is the maximum range for each
                                            // dimension
  ~Flow(void);
  int flatten(int x,...);        // compact a vector
  int flatten(int *x);
  int deflatten(int key,int nb); // retrieve the nbth component
  int get_max(void);
  int get_width(int key);
  int set(int vec,int pos,int value);  // change the component at position
                                       // pos in vector vec
};

#endif
