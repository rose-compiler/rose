enum class E { E0, E1 };

constexpr E f_(int n) { return n==0 ? E::E0 : E::E1; }


void foobar()
   {
     struct a
        {
          int n;
          constexpr a(int i) : n(i) { }
          constexpr operator E() { return n==0 ? E::E0 : E::E1; }
          explicit operator long();
        };

     a i0 (0);
     a i1 (1);

     switch (i0)
        {
          case E::E0:
               break;
          case E::E1:
        }

     switch (i1)
        {
          case E::E1:
               break;
          case E::E0:
        }
	
     switch (i0)
        {
          case f_(0):
               break;
          case f_(1):
        }

     switch (i1)
        {
          case f_(1):
               break;
          case f_(0):
        }
   }
