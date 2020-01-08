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
          explicit operator long() { return n + 3; }
        };

     a i0 (0);
     a i1 (1);

     switch (i0)
        {
          case A::enum0:
               i0;
               break;
          case A::enum1:
            // GNU requires an expression here, but EDG does not.
               break;
        }

     switch (i1)
        {
          case A::enum1:
               i1;
               break;
          case A::enum0:
            // GNU requires an expression here, but EDG does not.
               break;
        }

     switch (i0)
        {
          case foo(0):
               i0;
               break;
          case foo(1):
            // GNU requires an expression here, but EDG does not.
               break;
        }

     switch (i1)
        {
          case foo(1):
               i1;
               break;
          case foo(0):
            // GNU requires an expression here, but EDG does not.
               break;
        }
   }

