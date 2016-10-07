// t0506.cc
// ordinary template function returning DQT

template <class T> 
struct A {
  typedef int INT;
          
  template <class U>
  struct B {
    typedef float FLOAT;
  };
};

template <class T>
typename A<T>::INT f(A<T> x, int y)
{ return 3; }

template <class T>
typename A<T>::template B<T*>::FLOAT g(T t)
{ return 3.4; }

int main() {
  A<int> x;
  f(x, 3);

  g(3);
}
