// Removing Deprecated Exception Specifications from C++17

struct B {
       virtual void f() noexcept;
       virtual void g();
       virtual void h() noexcept;
     };

struct D: B {
    // void f();              // ill-formed
       void g() noexcept;     // OK

   // DQ: This should work if C++17 is correctly supported.
   // void h() = delete;     // OK
     };

