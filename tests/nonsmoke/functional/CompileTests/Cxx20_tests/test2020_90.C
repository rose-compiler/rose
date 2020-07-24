template<class T           >  void g(T, T = T());  // #1
template<class T, class... U> void g(T, U ...);    // #2

void h() 
   {
  // DQ (7/21/2020): Failing case should not be tested
  // g(42);  // error: ambiguous
   }

