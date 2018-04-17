template <typename T>
class A_T
   {
     public:
          long foo (T x) { return (long)x; }
   };

void foo (int x)
   {
  // for (A_T<bool> x2; x2.foo(false); x++)
     for (A_T<bool> x2; x2.foo(false);)
        {
        }
   }

