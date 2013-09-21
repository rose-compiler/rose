void foo (int x)
   {
#if 0
     class A
        {
          public:
             A () {}
             int foo (int x) { return x; }

             A (int) {}
             operator bool () { return false; }
        } x1;
#endif
     try
        {
          class AB
             {
               public:
                    int foo (int x) { if (!x) throw -1; return x; }
             } x2;
          x2.foo (0);
        }
#if 1
     catch (float)
        {
        }
#endif
#if 0
     catch (const A& x)
        {
        }
#endif
#if 1
     catch (int)
        {
          class AC
             {
               public:
                    int foo (int x) { throw x; }
             } x_AC;
        }
#endif
   }

