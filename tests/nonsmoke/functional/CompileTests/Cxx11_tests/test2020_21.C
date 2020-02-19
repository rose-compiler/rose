enum class A { enum0, enum1 };

constexpr A foo(int n) 
   {
     return n==0 ? A::enum0 : A::enum1; 
   }


void foobar()
   {
     struct a
        {
          int n;
          constexpr a(int i) : n(i) { }
          constexpr operator A() { return n==0 ? A::enum0 : A::enum1; }

       // Bug: unparsed as: constexpr inline operator A() const (missing explicit keyword).
          explicit operator long() { return n + 3; }
        };

     a i0 (0);

     switch (i0)
        {
          case A::enum0:
            // i0;
               break;
          case A::enum1:
            // GNU requires an expression here, but EDG does not.
               break;
        }

   }

