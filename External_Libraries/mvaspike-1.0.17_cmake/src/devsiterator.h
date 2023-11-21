#ifndef DEVSITERATOR_H
#define DEVSITERATOR_H
#include "devs.h"
#include <stack>
#include <utility>

class DevsIterator
{
  typedef std::pair<Devs*,int> nodeptr;
 protected:
  std::stack<nodeptr> ds;
  int level;
  int nb;
 public:
  DevsIterator(Devs &start);
  Devs & operator *(void);
  bool next(void);
  int get_level(void);
  int get_nb(void);
};

#endif
