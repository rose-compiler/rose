// t0481.cc
// invoke dtor of template class w/o naming its template args

template <class T>
struct A {
};

void foo()
{
  A<int> *a = 0;
  a->~A();
  (*a).~A();    // equivalent
}
