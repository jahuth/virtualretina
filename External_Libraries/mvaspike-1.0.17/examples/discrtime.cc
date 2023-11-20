#include <iostream>
using namespace std;

// the main API header file
#include <mvaspike.h>

class SampledInput
{
public:
  double getI(double t, int idx)
  { 
    return t*2; // dumb I(t) 
  }
};

class MyExtendedLif : public Lif
{
protected:
  SampledInput *input;
  int my_index;
public:
  MyExtendedLif(SampledInput *si) : Lif(), input(si)
  {
  }
  int delta_int(double t)
  {
    cout << "spike at t="<<t<<", I is "<< I << endl;
    return Lif::delta_int(t);
  }
  void custom_event(const Event& ev)
  {
    cout << "custom event received ";
    double now=ev.t;
    cout << "t=" << now << endl;
    double newI=input->getI(now,my_index);
    set_I(newI,now);
  }
};

int main()
{
  Root root;
  SampledInput si;
  MyExtendedLif n(&si);

  root.set_system(n);
  n.set_I(0.0);
  root.init();
  double t=0;
  double DT=1.0;
  while (t<5)
    {
      root.run(t+DT);
      root.push_custom_event(Event(n.id,t+DT));
      t+=DT;
    }
  return 0;
}
