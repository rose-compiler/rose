// t0409.cc
// use of overloaded template member operator= in extern "C" function

template <class T>
struct S {
  void operator=(T *);
  void operator=(S&);
};

extern "C" 
void foo()
{        
  S<int> s;
  
  int *p;
  s = p;

  s = 0;
}
