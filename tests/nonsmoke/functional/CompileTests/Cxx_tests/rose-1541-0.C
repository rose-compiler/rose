template<class t>
class A {
  private:
    struct B;
};

template <class T>
struct A<T>::B {};

