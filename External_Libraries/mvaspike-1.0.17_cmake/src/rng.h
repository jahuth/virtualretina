#ifndef RNG_H
#define RNG_H

class RNG {
 protected:
  unsigned int randmax;
  unsigned int seed;
  int flag;
  double sg;
 public:
  RNG();
  virtual ~RNG(){}
  virtual void srand(unsigned int st)=0; // int only ? or unsigned, long long ... ?
  virtual unsigned int calcrand(unsigned int s)=0;
  virtual unsigned int rand(void)=0;        // local rand()
  virtual unsigned int get_nth_rand(unsigned int n)=0;
  double rnd(double max);     // return 0<=rnd<=max
  double rnd_poisson(double lambda); // Poisson distribution (freq.=lambda)
  int randint(int start,int end);
  double gauss(void);
};

// MISC utils
int powmod(int a,int r,int n);

#endif
