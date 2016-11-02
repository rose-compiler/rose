#if 0
class A
   {
     public:
         int foo() 
            {
              return A_variable;
            }

         int A_variable;
   };

// This causes an error as it should!
// extern int A::A_variable;

// This causes an error as it should!
// class X;
// int X::X_static_variable = 0;

class X
   {
     public:
         static int foo() 
            {
              return X_static_variable;
            }

         static int X_static_variable;
   };

int X::X_static_variable = 0;
#endif

#if 0
int f(int a, ...);
int f(...);
#endif


#if 0
int a;
extern int a;
extern int y;
extern int y;

// int z;
#endif



#if 1
template <class T> 
class X
   {
     public:
#if 0
         void foo(void);
         void foobar ( void (X::*memberFunctionPointer) (void) )
            {
              foobar(foo);
            }
#endif
         void foobar ( void (*globalFunctionPointer) (void) )
            {
              void foo(void);
              foobar(foo);
            }
   };

void foo(void)
   {
     X<int> Xint;
     Xint.foobar(foo);
   }
#endif
