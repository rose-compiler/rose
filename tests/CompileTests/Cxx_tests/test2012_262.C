// This test is from ElsaTestCases/std/13.1a.cc
class X
   {
     static void f();
  // ERROR(1): void f();                      // ill-formed
  // ERROR(2): void f() const;                // ill-formed
  // ERROR(3): void f() const volatile;       // ill-formed
     void g();
     void g() const;                // OK: no static g
  // void g() const volatile;       // OK: no static g
     void g() volatile;       // OK: no static g

  // DQ (12/7/2012): I think this is also worth testing.
  // void g() restrict;
   };

