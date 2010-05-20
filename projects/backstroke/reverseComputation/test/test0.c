#include <stdlib.h>
struct model ;

struct model 
{
  int i_;
  int a_[1000];
}

;

void event0(struct model *m)
{
  int i = ++m -> i_;
  ++m -> i_;
}


void event1(struct model *m)
{
  int i = ++m -> i_;
  --m -> i_;
}


void event2(struct model *m)
{
  int i = ++m -> i_;
  m -> i_++;
}


void event3(struct model *m)
{
  int i = ++m -> i_;
  m -> i_--;
}


void event4(struct model *m)
{
  int i = ++m -> i_;
  m -> i_ = i;
}


void event5(struct model *m)
{
  int i = ++m -> i_;
  m -> i_ += i;
}


void event6(struct model *m)
{
  int i = ++m -> i_;
  (m -> i_ , i);
}


void event7(struct model *m)
{
  int i = ++m -> i_;
  rand();
}

