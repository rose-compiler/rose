#include <stdlib.h>
struct model ;

struct model 
{
  int m_int;
  int m_int_array[1000];
}

;

void event0(struct model *m)
{
#if 0
    for (int i = 0; i < 10; ++i)
	++m->m_int;

    int i = 0;
    for (; i < 10; ++i)
	++m->m_int;

    int j = 10;
    for (int i = 0; i < j; ++i)
	++m->m_int;

    //for (int i = 0, j = 10; i < j; ++i)
	++m->m_int;


    for (; m->m_int < 100;)
	m->m_int += 10;
#endif

    for (; m->m_int++ < 10000000; ++m->m_int);
}


