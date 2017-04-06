//test input for coverage test
// This file should be sufficient large to cover all input cases.
// Liao 4/4/2017

#include <iostream>
#include <vector> 
#include <cassert>

using namespace std;
#define SIZE 1000
int i; 

void foo();
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
class mypair {
    T values [2];
  public:
    mypair (T first, T second)
    {
      values[0]=first; values[1]=second;
    }
   T getmax();
};

// define member function with template
// two things: 1) add template <class T> 2) add <T> after class name !!
template <class T>
T mypair<T>::getmax ()
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


// operator overload
class Integer
{
  public:
    int i;
    Integer () {i=0;}
    Integer (int n):i(n) {}

   //prefix: ++a
  // a is increased first, then used, still the same a, allow modifying
  Integer & operator++ ()
  {
    i++;
    return *this; // by reference, still the value can be used, confusion
  }
  
  
  //suffix: a++,
  // old temp a is used, then increased by 1 
  // must return by value for the old value, 
  // must have const return to catch error like (a++)=c
  //Integer operator++ (int)
  const Integer operator++ (int)
  {
    Integer old(this->i);
    i++;
    return old;
  }


  // assignment operator
  Integer&  operator= (int value)
  {
    this->i = value;
    return *this;
  }

  // must has const input to help catch error like (a++)=c
  Integer&  operator= (const Integer& other)
  {
    if (this == & other) // not = !!
      return *this;
    this->i = other.i;
    return *this;
  }

  bool operator == (int y)
  {
    return (i == y);
  }

};

void test_splitVariableDeclaration()
{
  int i=100;
}

void test_setOperand()
{
  int j=12345;

}

void test_collectVariableReferencesInArrayTypes()
{
  int total_gpudevs = 0;
  int gpu_selection[total_gpudevs];
  
  test_splitVariableDeclaration();
}

void test_removeConsecutiveLabels ()
{
  int a = 10;

Loop1:
LOOP:
  do {
    if( a == 15) {
      a = a + 1;
      goto LOOP;
    }
    a++;
  }
  while( a < 20 );
}

int main(int argc, char* argv[])
{
  // usage
  mypair<int> myobject (115, 36); 
  mypair<float> myfloats (3.0, 2.18); 

  Integer a, b, c;
  a = 1; b =2 ; c =3;
  (++a) = c;
  assert (a == 3);
  //(a++) = c; // compilation error: trying to modify const
  cout<<a.i<<endl;

  if (argc >1)
  {
    cout<<"argc >1"<<endl;
  }

  return 0;
}
