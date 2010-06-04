#include <stdlib.h>
struct model ;

struct model 
{
  int i_;
  float f_;
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
#if 0
    for (int i = 0; i < 10; ++i)
	for (int j = 0; j < 10; ++j)
	{
        m->i_ = m->i_+1;


    while (--m->i_ > 0) m->i_ -= 10;



    do ++m->i_; while(m->i_++ < 100);
	}
#endif
    ++m->f_;
    while ((m->i_  = 5)> 0) m->i_ -= 10;

#if 0
    //m->i_ = 3;
    int i = 1;
    int j;

    for(int i = 1, j = 0; j < 5; ++j)
    {
	//for (i = 0; i < 2; ++i)
	++m->i_;
    }




    switch(i)
    {
	int j;
	case 0:
	    ++m->i_;
	    break;
	    --m->i_;
	    m->i_ *= 5;
	case 1:
	    {
	    m->i_ *= 3;
	    break;
	    }
	case 2:
	    m->i_++ ;
	    m->i_ *= 2;
	    break;
	    
	default:
	    break;
    }
#endif



}



