#include <stdlib.h>
struct model ;

struct model 
{
  int m_int;
}

;

void event(struct model *m)
{
  ++m -> m_int?m -> m_int++ : --m -> m_int;
}

