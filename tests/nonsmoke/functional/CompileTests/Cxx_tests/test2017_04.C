// Need example of template dependent array bound that is not a constant.

#if 1
void foobar()
   {
     int array[] = { 2, 3 };
   }
#endif

#if 0
template<typename F1, typename... Fs>
void wait_for_all(F1& f1, Fs&... fs)
   {
  // bool dummy[] = { (f1.wait(), true), (fs.wait(), true)... };
     bool dummy[] = { (f1.wait(), true), (fs.wait(), true)... };

  // prevent unused parameter warning
     (void) dummy;
   }
#endif

#if 0
template <int... T> 
void foo ()
   {
     int array[] = { T... };
   };
#endif

#if 0
void foobar()
   {
  // Test using a template that is an expression.
     const int N = 7;
     foo<N>();
   }
#endif

