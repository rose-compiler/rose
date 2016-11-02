#if 0
// struct Type { int x; };

// The first part of the type is: "struct tag { int x; }"
// And there is no 2nd part!
// struct tag { int x; } Var;

// mixing unnamed types in typedefs with arrays
// typedef struct { int x; } ArrayType [100];

// The first part of the type is: "struct { int x; }"
// and the second part is: "[100]"
// but we have to be careful to extract the base type from the SgArrayType
struct { int x; } ArrayVar [100];
#endif

#if 0
class X { public: X(int x); };
class Y : public X 
   {
     public:
          Y() : X(42) {}
   };
#endif

#if 0
// Bug reported by Jeremiah
int x[2];
char* y = (char*)x + 1;
#endif

#if 0
namespace X
{
   int variableVar;
// enum booleanType {TRUE,FALSE};
// class classType {};
   
}

// Test qualification of names of variables defined in a namespace
int variableWithInitializer = X::variableVar;

// Test qualificiation of names of enums defined in a namespace
// X::booleanType booleanVar;

// Test qualificiation of names of enums defined in a namespace
// X::classType classVar;


// Cases which require name qualification:
//   enum values, function names, member fuction names, variable ref expressions
#endif

#if 0
template <class T> int f(T) { return 0; }

class A
   {
     static class N { } n;
     void g(int = f(n));
   };
#endif

#if 0
template <typename T> 
class Y
   {
     public:
//        typedef T scalar;
          Y()
             {
               T x;
             }
   };

Y<int> y;
void foo ()
   {
     Y<int> y;
   }

class X
   {
     void foo ()
        {
          Y<int> y;
       // int y;
        }
};
#endif

#if 0
class X
   {
     public:
          void foo1(){ ::operator new(1); }

   };
#endif

#if 0
// glob.h depends on dirent.h so once this is handled correctly the 
// problem in ROSE disappears.  However it should not be required.
// We likely need to have policy for how to handle class declarations that 
// never have a matching defining declaration.  Even if it would not link
// it should still compile!
// #include <dirent.h>
#include <glob.h>
#endif


#if 0
int f(int x);

class StringDict {
  class Iter {
  public:
    Iter(StringDict &dict) { dict.getIter(); }

    // ambiguous, so I need to check the bodies..
    int foo() { return f(3); }
  };
  Iter getIter();
};
#endif

#if 1
void foo()
   {
     const int n = 10;
     for (int i = 0, j = 1; i < n; i++)
        {
          j++;
        }
   }
#endif
