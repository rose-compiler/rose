// t0434.cc
// some DQTs

template <class S, class T>
class A {
public:
  typedef S char_type;
  typedef T int_type;

  int      foo1 (char_type c);
  int_type foo2 (char      c);
  int_type foo3 (char_type c);
};

//ERROR(1): template <class S, class T>
//ERROR(1): int A<S,T>::dorf(char_type c)  {}

template <class S, class T>
                 int       A<S,T>::foo1(char_type c)  {}

template <class S, class T>
typename A<S,T>::int_type  A<S,T>::foo2(char      c)  {}

template <class S, class T>
typename A<S,T>::int_type  A<S,T>::foo3(char_type c)  {}
