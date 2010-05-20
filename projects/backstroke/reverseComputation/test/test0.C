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

    for (int i = 0; i < 10; ++i)
	for (int j = 0; j < 10; ++j)
	{
        m->i_ = m->i_+1;

    while (m->i_ > 0) m->i_ -= 10;
    do ++m->i_; while(m->i_++ < 100);
	}



    }



