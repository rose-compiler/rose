#include <stdlib.h>
struct model ;

struct model 
{
  int m_int;
}

;

void event(struct model *m)
{
  m -> m_int ^= 3034;
}

