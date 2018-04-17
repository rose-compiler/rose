#if 0
template<typename _Tp>
struct _Vector_base
   {
     struct _Vector_impl
        {
          int x;
          _Vector_impl() {}
        };

     public:
//        _Vector_base() {}

     public:
          _Vector_impl _M_impl;
   };

template<typename _Tp > 
class vector : protected _Vector_base<_Tp>
   {
     public:
       // template<typename _Tp > vector();
          vector();
          vector(int size);
   };
#endif

#if 1
#include<test2012_178.h>
#else
template <bool T> struct X {};

template<>
struct X<true>
   {
  // template <typename T1, typename T2 > static void test_me(T1 x, T2 y) {};
     template <typename T1, typename T2 > static void test_me(T1 x, T2 y);
   };

template <typename T1, typename T2 > 
void X<true>::test_me(T1 x, T2 y) {}
#endif


int main()
   {
#if 0
     int size = 2;

     vector<int>   integerVector;
  // vector<float> floatVector;

  // integerVector = vector<int>(size);
#endif

     X<true>  abc_true;
     X<false> abc_false;

     return 0;
   }

