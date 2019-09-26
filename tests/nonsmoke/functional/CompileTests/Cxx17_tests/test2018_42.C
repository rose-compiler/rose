// Removing Deprecated Exception Specifications from C++17

struct S {
     constexpr S() = default;            // ill-formed: implicit S() is not constexpr
     S(int a = 0) = default;             // ill-formed: default argument
     void operator=(const S&) = default; // ill-formed: non-matching return type
     ~S() noexcept(false)throw(int) = default;          // deleted: exception specification does not match
   private:
     int i;
     S(S&);                              // OK: private copy constructor
   };
   S::S(S&) = default;                   // OK: defines copy constructor
