// t0556.cc
// problem with static data member template with elaborated class type

struct B {};

template <class T>
struct A {
  static int a;
  
  static B b1;
  static struct B b2;
  static B b3;
};

// leading 'struct' causes problems for Elsa

template <class T>
struct B A<T>::b1;

template <class T>
struct B A<T>::b2;

template <class T>
B A<T>::b3;
