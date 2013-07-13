
template <template <typename U> class T>
class A {
    template <typename V>
    void foo(/*typename T<V>::type_t & arg*/);
};

template <typename U>
class B {};

/*
template <>
class B<int> {
  public:
    struct type_t {};
};
*/

template <>
template <>
void A<B>::foo<int>(/*B<int>::type_t & arg*/) {}

