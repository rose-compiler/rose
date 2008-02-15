// t0412.cc
// dependent resolution in an E_constructor

struct A {
  A(int);
  A(int,int);
};

template <class T>
void bar(T &t)
{
  0, A(t);          // dependent
}

void foo()
{ 
  int x;
  bar(x);
}
