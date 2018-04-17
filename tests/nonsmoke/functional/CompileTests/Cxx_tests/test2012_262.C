// This test is from ElsaTestCases/std/13.1a.cc
class X
   {
     static void f();
  // ERROR(1): void f();                      // ill-formed
  // ERROR(2): void f() const;                // ill-formed
  // ERROR(3): void f() const volatile;       // ill-formed
     void g();
     void g() const;                // OK: no static g
     void g() const volatile;       // OK: no static g
     void g() volatile;       // OK: no static g

  // DQ (12/11/2012): Test variations with restrict keyword.
  // int* h();
  // int* h() const;
  // int* h() const volatile;
  // int* h() volatile;
     int* h() restrict;                // Error if "int* h();" is declared
     int* h() const restrict;          // Error if "int* h() const;" is declared
     int* h() const volatile restrict; // Error if "int* h() const volatile;" is declared
     int* h() volatile restrict;       // Error if "int* h() volatile;" is declared
   };

