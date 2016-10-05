#if 0
// This demonstrates a scope problem for x
class X
   {
     public:
          int foo() 
            {
              return x;
            }

          int x;
   };
#endif

#if 1
class A
   {
     public:
          static int foo()
            {
              return x;
            }

          static int x;
   };

int A::x = 0;
#endif

#if 0
class Y
   {
     public:
          static int x;
     class X
        {
          public:
               static int x;
        };
   };

int X::x = 0;
int Y::x = 0;
int Y::X::x = 0;
#endif
