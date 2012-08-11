template <bool T> struct X {};

template<>
struct X<true>
   {
  // template <typename T1, typename T2 > static void test_me(T1 x, T2 y) {};
     template <typename T1, typename T2 > static void test_me(T1 x, T2 y);
   };

template <typename T1, typename T2 > 
void X<true>::test_me(T1 x, T2 y) {}
