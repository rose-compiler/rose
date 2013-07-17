
template <template <template <typename V> class U> class T>
class A {
    template <template <typename W> class X>
    class foo {
      template <typename Y>
      class bar;
    };
};

template <typename T>
class B {};

template <template <typename V> class U>
class C {};

template <>
template <>
template <>
class A<C>::foo<B>::bar<int> {};
