//#include <stdlib.h>
#include <utility>
struct model ;

struct point
{
    int x, y;
  std::pair<int, int> pr;
};

bool operator ==(const point& p1, const point& p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

bool operator !=(const point& p1, const point& p2)
{
    return !(p1 == p2);
}

struct model 
{
  int i_;
  int j_;
  float f_;
  int a_[1000];
  point p;
}

;

namespace ns
{
    int ns_i;
}

// Swap
void event0(struct model* m)
{
    (m->i_ += 3) += 3;
    //(++m->i_)++;
    m->i_++;
    //(++m->i_) += 10; 
    //m->i_ += ++++m->j_;
    //m->i_ += (m->j_ += 10);
    //++(m->i_ = 10);
    int t = m->i_;
    m->i_ = m->j_;
    m->j_ = t;
    ns::ns_i = 10;
}

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


#if 0
void event1(struct model* m)
{
#if 0
    int i = 10;
    int j = 20;
    m->i_ += i;
    m->i_ -= j;
#endif
    //m->i_ += m->i_;
#if 0
    if (int i = 2)
        m->i_ += i;
#endif
    int j = 4;
    while (int i = j--)
        m->i_ += i;
    for (int i = 10; int j = i--;)
        m->i_ ++;

#if 1
    j = 2;
    int i = j % 2;
    //switch (int i = j%2)
    switch (i)
    {
        case 0:
            m->i_ += i++;
            m->i_ -= i++;
            break;
        case 1:
            m->i_ -= i;
            break;
        default:
            break;
    }
#endif
    
#if 0
    m->i_ -= m->i_;
    m->i_ = -m->i_;
    m->i_ = i - m->i_;
    m->i_ = i + m->i_;
    m->i_ = m->i_ - i;
    m->i_ = m->i_ + i;
#endif
}


#if 0
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

    int i = 10;
    m->i_ += i;
    


    ++m->f_;
    while (m->i_  > 0) m->i_ -= 10;

#if 1
    //m->i_ = 3;
    i = 1;
    //int j;

    for(int i = 1, j = 0; j < 5; ++j)
    {
	//for (i = 0; i < 2; ++i)
	++m->i_;
    }




    for (int i = 0; i < 10; ++i)
    switch(i)
    {
#if 1
	int k;
	//default:
	case 0:
	    ++m->i_;
	    break;
#if 0
	    --m->i_;
	    m->i_ *= 5;
#endif
	case 1:
	    m->i_ += 3;
	    break;
#endif
	case 2:
	case 3:
#if 0
	    m->i_++ ;
	    m->i_ += 2;
	    break;
#endif	    
	    //break;
    }
#endif



}
#endif


void event2(struct model* m)
{
    int i = 2, j = 3, k = 4;

    m->i_ = -m->i_;

    m->i_ = ++i + m->i_;
    m->i_ = ++i + j + m->i_;

    m->i_ = i + j + m->i_;
    m->i_ = i + m->i_ + j;
    m->i_ = i + (m->i_ + j);
    m->i_ = i + j - m->i_;
    m->i_ = i - (j + m->i_);
    m->i_ = (i - (j - (k - m->i_)));

    //m->i_ = i - 2 * m->i_ - m->i_;
    //m->i_ = i - 2 * m->i_;

    m->i_ += ++i / (j = 2);
#if 0
    m->i_ += push(++i / (push(i), i = 1));
    m->i_ -= (i = pop(), i--, pop());
#endif
}

#endif
