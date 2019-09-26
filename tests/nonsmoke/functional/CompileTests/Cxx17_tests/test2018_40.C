// Removing Deprecated Exception Specifications from C++17

struct A {
     A(int = (A(5), 0)) noexcept;
     A(const A&) noexcept;
     A(A&&) noexcept;
     ~A();
   };
   struct B {
     B() throw();
     B(const B&) = default;  // implicit exception specification is noexcept(true)
     B(B&&, int = (throw Y(), 0)) noexcept;
     ~B() noexcept(false);
   };
   int n = 7;
   struct D : public A, public B {
     int * p = new int[n];
     // D::D() potentially-throwing, as the new operator may throw bad_alloc or bad_array_new_length
     // D::D(const D&) non-throwing
     // D::D(D&&) potentially-throwing, as the default argument for B's constructor may throw
     // D::~D() potentially-throwing
   };

