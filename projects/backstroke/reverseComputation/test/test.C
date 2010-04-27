#include "rctypes.h"
#include "stdio.h"

struct model 
{
  int m_int;
}

;

void initialize(struct model *m)
{
  m -> m_int = 0;
}


int compare(struct model *m1,struct model *m2)
{
  if (m1 -> m_int != m2 -> m_int) 
    return 1;
  return 0;
}


void event(struct model *m)
{
  //++m -> m_int;
  m->m_int = 5;
}


