// This test is from ElsaTestCases/std/13.1a.cc
class X
   {
     int* h() restrict;                // Error if "int* h();" is declared
  // int* h() restrict { return 0L; }; // Error if "int* h();" is declared
#if 0
     int* h() const restrict;          // Error if "int* h() const;" is declared
     int* h() const volatile restrict; // Error if "int* h() const volatile;" is declared
     int* h() volatile restrict;       // Error if "int* h() volatile;" is declared
#endif
   };
