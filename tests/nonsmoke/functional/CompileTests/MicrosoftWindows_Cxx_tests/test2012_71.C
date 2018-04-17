// When foobar is a template it's instantiation is not located in the correct namespace.

#define DEFINE_AS_TEMPLATE 1
#define TWO_NAMESPACES 0

namespace A
   {
#if DEFINE_AS_TEMPLATE
     template<typename T> void foobar(T t);
#else
     void foobar(int t);
#endif

#if TWO_NAMESPACES
   }

namespace A
   {
#endif

  // template<typename T>
  // struct X { T t; };

#if DEFINE_AS_TEMPLATE
//     template<typename T> void foobar(T t) {};
#else
     void foobar(int t) {};
#endif
   }

// X<long> x;

#if 1
void foo()
   {
     A::foobar(1);
   }
#endif
