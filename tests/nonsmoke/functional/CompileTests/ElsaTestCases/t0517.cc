// t0517.cc
// problem with explicit instantiation forgetting default args

template <class T>
class basic_string {
public:
  void resize ();
  basic_string & erase (int p, int n = -1) {}
};

template <class T>
void basic_string <T>::resize ()
{
  this->erase (3);
}

template class basic_string < char >;
template class basic_string < int >;


// ---------------- trigger an assertion failure -------------
//ERROR(1): template <class S, class U = basic_string<S> >
//ERROR(1): class A;


// ------------ somewhat simplified ------------
template <class T>
class A {
public:
  void foo(int x, int y = -1)
    {}
};

template class A<char>;
template class A<int>;

void f(A<int> &a)
{
  a.foo(1);
}



