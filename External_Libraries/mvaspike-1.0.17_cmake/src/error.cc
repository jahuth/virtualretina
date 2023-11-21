#include "error.h"

#include <iostream>
using namespace std;
#include <stdlib.h>

void fatal(const char *s)
{
  cerr << " *** " << s << endl;
  exit(EXIT_FAILURE);
}

void warning(const char *s)
{
  cerr << " ??? " << s << endl;
  exit(EXIT_FAILURE);
}
