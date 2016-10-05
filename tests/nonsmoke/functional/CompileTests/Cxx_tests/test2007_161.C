#define USE_NAMESPACE 0

#if USE_NAMESPACE
namespace A {
#endif
template <typename T>
class X
   {
     public:
         ~X(){};
   };
#if USE_NAMESPACE
X<int> x;
 }
#endif


#if USE_NAMESPACE
A::X<int> x;
#else
X<int> x;
#endif
