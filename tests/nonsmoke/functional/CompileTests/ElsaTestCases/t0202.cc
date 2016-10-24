// t0202.cc
// member function of a template class accepting a ptr-to-member argument

template <class T>
class C {
public:
  void foo(  int (T::*mf)()  )
  {  }
};
