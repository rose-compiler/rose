
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

// The bug in ROSE is that only a single "template <>" is unparsed, instead of a pair of them.
template <>
template <>
void A<B>::foo<int>(/*B<int>::type_t & arg*/) {}

