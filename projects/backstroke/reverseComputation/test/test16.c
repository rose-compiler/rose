#include <stdlib.h>
struct model ;

struct model 
{
  int m_int;
}

;

void event(struct model *m)
{
  rand() > 1073741823?m -> m_int++ : --m -> m_int;
}

