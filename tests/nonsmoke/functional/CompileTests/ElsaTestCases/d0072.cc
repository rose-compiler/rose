// d0072.cc

template <class T> 
const T & m(const T & a, const T & b)
{}

template <class S> 
struct R {
};

R<short> &e(R<short> &I)  
{}

const short *&e(const short *&)  
{}

void q() 
{
  const short *iter;
  m(iter, e(iter));
}
