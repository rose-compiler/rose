//test input for coverage test
// This file should be sufficient large to cover all input cases.
// Liao 4/4/2017

#include <iostream>
#include <vector> 

#define SIZE 1000
int i; 

#ifdef SIZE
void foo(int i)
{

  switch (i)
  {
    case 0:
      break;
    default:
      break;

  }
}
#endif

// declare a template class
template <class T>
class pair {
    T values [2];
  public:
    pair (T first, T second)
    {
      values[0]=first; values[1]=second;
    }
   T getmax();
};

// define member function with template
// two things: 1) add template <class T> 2) add <T> after class name !!
template <class T>
T pair<T>::getmax ()
{
  T retval;
  retval = values[0]>values[1]? values[0] : values[1];
  return retval;
}

class Base {
    virtual void f(int);
};
#if 1
struct Derived : Base {
    // this member template does not override B::f
    template <class T> void f(T);
 
    // non-template member override can call the template:
    void f(int i) {
         f<>(i);
    }
};
#endif
int main()
{
  // usage
  pair<int> myobject (115, 36); 
  pair<float> myfloats (3.0, 2.18); 
  return 0;
}
