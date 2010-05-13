#include <stdlib.h>
struct model ;

struct model 
{
  int i_;
  int a_[1000];
}

;

#if 0
void event0(struct model *m)
{
{
    int i = ++m -> model::i_;
    if (rand() > 1073741823) 
      if (rand() > 1073741823) 
        m -> model::i_ = (((((++m -> model::i_))?(((m -> model::i_)) && (3961)) : (((m -> model::i_)) || (3961)))));
      else {
      }
    else if (rand() > 1073741823) 
      m -> model::i_ = (((((++m -> model::i_))?(((m -> model::i_)) && (3961)) : (((m -> model::i_)) || (3961)))));
    else 
      m -> model::i_ += (((((++m -> model::i_))?(((m -> model::i_)) && (3961)) : (((m -> model::i_)) || (3961)))));
  }
}
#endif

void event0(struct model* m)
{
    //int i = 0, j = 0;
    for (int i = 0, j = 0; i < 10; ++i)
    ++m->i_;
}



