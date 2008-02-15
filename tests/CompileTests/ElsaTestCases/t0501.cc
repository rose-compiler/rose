// t0501.cc
// nested class befriends containing template

template <class T>
struct A {
  struct B {
    friend class A<T>;
  };
};
