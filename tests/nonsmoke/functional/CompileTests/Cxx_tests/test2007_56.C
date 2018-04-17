#if 0
// number #1
int x;

void foo()
   {
     x = 0;
     int x;
     ::x = 42;
     x = 7;
   }
#endif

#if 0
// number #2
namespace A
   {
     extern int x;
   }

int A::x = 42;
#endif

#if 0
// number #3
namespace A
   {
     void foo();
   }

void foo();
void foobar()
   {
     foo();
     A::foo();
   }
#endif


#if 0
// number #4
void foo();
namespace A
   {
     void foo();
     void foobar()
        {
          foo();
          ::foo();
        }
   }
#endif

#if 0
// number #5
void foobar() {}
namespace A
   {
     void foobar();
   }
#endif

#if 0
// number #6
// This will be fixed by Robert
class B {};
namespace A
   {
     class B {};
     B b1;
     ::B b2;
   }
#endif

#if 0
// number #7
class B {};

int B;

class B b;
#endif


#if 0
// number #8
class A{};
namespace X
   {
  // This type hides ::A
     typedef ::A A;
     A a1;
     ::A a2;
   }
#endif

#if 0
// number #9
namespace X
   {
     class A{};
  // This type hides A (and forces type elaboration to resolve "class A"
     typedef A A;
     A a1;
     class A a2;
   }
#endif

#if 0
// number #10
class A{};
namespace X
   {
  // This type enum A hides ::A and type elaboration is NOT enough to resolve class A.
     enum A{};
     A a1;
     ::A a2;
     class ::A a3;
   }
#endif

#if 0
// number #11
namespace X
   {
     class A{};
  // This type hides A (and forces type elaboration to resolve "class A"
     enum A{};
     A a1;
     class A a2;
   }
#endif

#if 0
// number #12
namespace X
   {
     class A{};
  // This type hides A (and forces type elaboration to resolve "class A"
     int A;
  // A a1;
     class A a2;
   }
#endif

#if 0
// number #13
void foo()
   {
  // This is just for entertainment
     class A * A;
   }
#endif


#if 0
// number #14
namespace X
   {
     class X1 {};
   }

void foo( X::X1 x )
   {
   }
#endif


#if 0
// number #15

class X1 {};
namespace X
   {
     class X1 {};
     namespace Y
        {
          class X1 {};
        }
   }

void foo( X::X1 x1, X::Y::X1 x2 )
   {
   }
#endif

#if 0
// number #16

class X1 {};
namespace X
   {
     class X1 {};
     namespace X
        {
          class X1 {};

       // This will be unparsed as "void foo( X1 x1, X1 x2 )"
          void foo( X::X1 x1, X1 x2 )
             {
             }

       // EDG thinks this is different from void foo( X::X1 x1, X1 x2 ), 
       // but g++ correctly identifies that it is a redefinition.
       // void foo( ::X::X1 x1, ::X::X::X1 x2 )
          void foo( ::X::X1 x1, ::X::X::X1 x2, int x3 )
             {
             }
        }
   }

void foo( X::X1 x1, X::X::X1 x2 )
   {
   }
#endif

#if 0
// number #17

class X1 {};
class X
   {
     public:
     class X1 {};
     class X2
        {
          public:
          class X1 {};
          class X 
             {
               public:
               class X1 {};
             };
#if 1
       // This will be unparsed as "void foo( X1 x1, X1 x2 )"
          void foo( X::X1 x )
             {
             }
#endif
#if 1
       // This will be unparsed as "void foo( X1 x1, X1 x2 )"
          void foo( ::X::X1 x )
             {
             }
#endif
#if 1
       // This will be unparsed as "void foo( X1 x1, X1 x2 )"
          void foo( X::X1 x1, X1 x2 )
             {
             }
#endif
#if 1
       // EDG thinks this is different from void foo( X::X1 x1, X1 x2 ), 
       // but g++ correctly identifies that it is a redefinition.
       // void foo( ::X::X1 x1, ::X::X::X1 x2 )
          void foo( ::X::X1 x1, ::X::X::X1 x2, int x3 )
             {
             }
#endif
        };
   };

#if 1
void foo( X::X1 x1, X::X2::X1 x2 )
   {
   }
#endif

#endif


#if 0
// number #18

namespace X
   {
     class X1;
   }

class X::X1 {};
#endif

#if 0
// number #19

namespace X
   {
     class X1;
   }

using namespace X;

class X1 {};

::X1 a;

#endif

#if 0
// number #20

namespace X
   {
     class X1{};
   }

using namespace X;

class X1 {};

::X1 a;

#endif

#if 0
// number #21

namespace X
   {
     class X1{};
   }

using X::X1;

class X1 {};

::X1 a;

#endif

#if 0
// number #22

namespace X
   {
     class X1;
   }

using namespace X;

class X1 {};
class X::X1 {};

// This fails to generate the name qualification because the hidden list does not
// record the global scope X1 ("::X1") as hiding the namespace X class X1 ("X::X1").
// ::X1 a;

X::X1 b;

#endif

#if 0
// number #23

namespace X
   {
     class X1{};
   }

void foo()
   {
  // using namespace X;
     using X::X1;
     
     {
        
     class X1 {};

     X1 a1;
     X::X1 a2;
     }
     
   }
#endif

#if 0
// number #24

namespace X
   {
     class X1{};
   }
namespace Y
   {
     using namespace X;
     class X1{};
   }

void foo()
   {
  // using namespace X;
  // using X::X1;
     using namespace X;
     
     {
        
//     class X1 {};

     Y::X1 a1;
     X::X1 a2;
     }
     
   }
#endif


#if 0
// number #25
typedef struct Array_Domain_c array_domain;
typedef struct Array_Domain_c array_domain;
#endif

#if 0
// number #26

// typedef struct

struct X
   {
  // int __count;
  // union Y
     struct Y
        {
          int __wch;
          char __wchb[4];
        } __value;
   } __mbstate_t;
#endif

#if 0
// number #27
namespace std
{
  template<typename T> class A{};
}

std::A<int> a1;

void foo()
   {
     std::A<float> a2;
   }
#endif


#if 0
// number #28
class X
   {
     public:
       // Notice that we can initialize static constants within the class!
          static const int    maxIntValue    = 3;
          static const long   maxLongValue   = 3;
          static const char   maxCharValue   = 'Z';

       // Not allowed for float, double, or pointers (interesting!)
       // static const float  maxFloatValue  = 3.0;
       // static const double maxDoubleValue = 3.0;
       // static const size_t maxSizeTValue = 3;
       // static const int   *maxIntPointerValue = 0L;

       // types of data members that can't be initalized in-class
          static const double pi;
          const int tuncatedPi;
          const double e;

       // this is the only way to initalize a non-static const variable (integer or double)
          X(): tuncatedPi(3), e(2.71) {}
   };

// This is the only way to initialize a static const data member which is non-integer based
const double X::pi = 3.14;
#endif

#if 0
// number #29
#define STORAGE
class Y
   {
     public:
       // This is allowed by GNU but not by EDG
       // static const double pi = 3.141592653589793238462643383279; // Pi to 30 places

       // This is allowed by EDG, but not by g++ (g++ needs constant to be static)
       // const double pi = 3.141592653589793238462643383279; // Pi to 30 places

       // Code that will compile with EDG
       // const double pi = 3.141592653589793238462643383279; // Pi to 30 places
       // Code that we should generate so that we can compile with g++
       // static const double pi = 3.141592653589793238462643383279; // Pi to 30 places

          int findme;

          static const double static_pi;
          STORAGE const double pi = 3.141592653589793238462643383279; // Pi to 30 places
   };

const double Y::static_pi = 3.14;
#endif



#if 0
// number #30

#define DEMONSTRATE_BUG 1

int x;

class X
   {
     public:
          int y;

          static 
          bool _S_initialized()
             { 
#if DEMONSTRATE_BUG
            // A variable declaration will be created for the first occurance of 
            // _S_ios_base_init but it will not be connected to the AST!
               return ::X::_S_ios_base_init;
#else
               return true;
#endif
             }

          int z;

     private:
       // We need to have the previously used variable declaration be used here instead 
       // of a new one being created.  Either that or force the previous initialized name
       // (called prev_decl_item in Sage III)  to point to the correct declaration (this one).
          static int _S_ios_base_init;
       // int _S_ios_base_init;
   };

#if 1
// DQ (1/7/2007): added initalization of static variable so test name qualification.
int X::_S_ios_base_init = 0;
#endif
#endif



#if 0
// number #31

#define ALTERNATE_FIX 0

class GenericGridData
   {
     public:
          int computedGeometry;
   };

class GenericGrid
   {
     public:
#if !ALTERNATE_FIX
          void computedGeometry()
             { rcData->computedGeometry = 0; }
#endif

          typedef GenericGridData RCData;
       // Note that currently the unparse reports that the parent of rcData
          RCData* rcData;

#if ALTERNATE_FIX
          void computedGeometry()
             { rcData->computedGeometry = 0; }
#endif
   };
#endif


#if 0
// number #32

// class vector{};

class myVector
   {
     public:
       // class vector{};

       // This references a class that is not defined or declared elsewhere.
       // We build the declaration but we don't build a symbol (since we 
       // would not know what scope to associate it with).  By default we
       // define the scope of the SgClassDeclaration for class vector to be
       // SgGlobal. After checking it appears that EDG is assigning it to 
       // global scope (or it defaults to global scope, I think).
          myVector operator= (const class vector &x) const;
   };

// A test for empty intersection of symbols from all scopes would help
// avoid this sort of bug (but it would be expensive if not done well).

// If this declaration is present then a SgClassSymbol is generated
// in global scope AND it is referenced in the sysmbol table for the
// class definition for class myVector!  This is likely anohter bug!
// class vector{};
#endif


#if 0
// number #33

#if 0
class myNumberType
   {
     public:
         void foobar();
   };

typedef ::myNumberType myNumberType;
typedef myNumberType numberType;
#else
typedef float numberType;
#endif

class Y
   {
     public:
          typedef int numberType;
   };

class Z : public Y
   {
     public:
       // Example of where "::" is significant, without it numberType 
       // will be an "int" instead of a "myNumberType"
          typedef ::numberType numberType;
   };
#endif



#if 0
// number #34

typedef enum enumType1{};

class Array
   {
     public:
       // Error: This becomes a SgEnumDeclaration in the AST 
       // Is this a typedef which has no name?
#if 1
       // Within this scope ::enumType1 is hidden (should be in EDG's list of hidden names)
          typedef enum enumType1{};
#endif
//        Array(enumType1 x);
          Array(::enumType1 x);
   };
#endif



#if 0
// number #35

// This example shows how integer declarations can hide enum fields.
// This is currently a bug in the hidden list computation.

enum enum1 { zeroA };
void foo(enum1);
// int zeroA;

void foo(int);

void foo()
   {
     int zeroA;

     foo();
     foo(::zeroA);
   }
#endif



#if 0
// number #36

// Example of unnamed namespace.
// Use of "_1" used to unparse to boost::lambda::::_1

namespace boost {
namespace lambda {

#if 0
namespace X {
  // These are constants types and need to be initialised
  typedef int Integer;
  int _1;
} // unnamed
#endif
   
namespace {
  // These are constants types and need to be initialised
  typedef int Integer;
  int _1;
} // unnamed
   
} // lambda
} // boost


int _1;

void foo()
   {
     _1 = 1;
//   boost::lambda::X::_1 = 1;
     boost::lambda::_1 = 1;

     int x;
//   boost::lambda::X::Integer y1;
     boost::lambda::Integer y2;
   }
#endif


#if 0
// number #37

// Example from Rich:

/* ====================================================
 * This example shows the need for name qualification
 * for 'friend' function declarations. In ROSE 0.8.9a,
 * class N::A's 'friend' declaration for 'bar' loses
 * its global qualifier ('::') when unparsed.
 * ==================================================== */

// Forward declaration of the true friend.
void bar();

// ====================================================
namespace N
{
  // This 'bar' is not the true friend.
  void bar ();

  // Class declaring true friend.
  class A
   {
     public:
          friend void ::bar(); // '::' must be preserved.
   };
}
#endif


#if 0
// number #38

// Example from Rich:

/* ====================================================
 * This example shows the need for name qualification
 * for 'friend' function declarations. In ROSE 0.8.9a,
 * class N::A's 'friend' declaration for 'bar' loses
 * its global qualifier ('::') when unparsed.
 * ==================================================== */

// Forward declaration of the true friend.
namespace N { class A; }
void bar (const N::A&);

// ====================================================
namespace N
{
  // This 'bar' is not the true friend.
  void bar (const A&);

  // Class declaring true friend.
  class A
  {
  public:
    A () { ::bar (*this); }
    friend void ::bar (const A&); // '::' must be preserved.

  private:
    void for_true_friends_only (void) const;
  };
}

// ====================================================
// This 'bar' is the true friend.
void bar (const N::A& x)
{
  x.for_true_friends_only ();
}

#endif


#if 0
// number #39

typedef float numberType;

class Y
   {
     public:
          typedef int numberType;
   };

class Z : public Y
   {
     public:
          void foo() {}

         ::numberType foo2 ();
   };

#endif

#if 0
// number #40

// This test code demonstrates the requirements for "::" (global qualification).

class myNumberType
   {
     public:
         void foobar();
   };

typedef ::myNumberType myNumberType;

typedef myNumberType numberType;

class Y
   {
     public:
          typedef int numberType;
   };

class Z : public Y
   {
     public:
       // Such typedefs are significant (but force 
       // "typedef ::numberType numberType;" to be an error)
       // typedef numberType numberType;

       // Example of where "::" is significant, without it numberType 
       // will be an "int" instead of a "myNumberType"
          typedef ::numberType numberType;

       // Note also that if Z were not derived from Y then 
          typedef numberType numberType;
       // could have been used to refer to myNumberType

          void foo()
             {
            // Unparsed as Z::numberType numberValue, which is a problem since the typedef base type is not unparsed as "::numberType"
               numberType numberValue;

           // function "foobar()" not defined for "int"
               numberValue.foobar();
             }

         ::numberType foo ( ::numberType numberValue );
#if 1
         ::numberType foo ( numberType numberValue, int x );

      // The code generation will output a redundant "::" (global qualifier) becuase EDG things it is required.
         ::numberType foo ( Y::numberType numberValue, int x );

      // The global qualifier for the function parameter is marked as required, but is not REALLY needed.
         ::numberType foo ( ::Y::numberType numberValue );
#endif
   };

#endif


#if 0
// number #41

class B
   {
     public:
     class C
        {
        };
   };

class C
   {
   };

class A
   {
     public:
     class B
        {
          public:
          class A
             {
             };

          class C
             {
               public:
                 // A* a1_ptr;
                    A* a1_ptr;
                 // B* b1_ptr;
                    B* b1_ptr;
                 // C* c1_ptr;
                    C* c1_ptr;
                 // ::A* a2_ptr;
                    ::A* a2_ptr;
                 // B::A* a3_ptr;
                    B::A* a3_ptr;
                 // ::B* b_ptr;
                    ::B* b_ptr;
                 // ::C* c2_ptr;
                    ::C* c2_ptr;
                 // ::B::C* bc1_ptr;
                    ::B::C* bc1_ptr;
                 // B::C* bc2_ptr;
                    B::C* bc2_ptr;
             };
        };
   };

#endif

#if 0
// number #42

class A
   {
     public:
          void foo();
   };

class B : public A
   {
     public:
          void foo();
   };

void foobar()
   {
     A a;
     B b;

  // foobar();
     foobar();
  // a.foo();
     a.foo();
  // b.foo();
     b.foo();
  // b.B::foo();
     b.B::foo();
  // b.A::foo();
     b.A::foo();
   }

#endif

#if 0
// number #43

class A
   {
     public:
          void foo();
          A operator=(A a);
          A operator+(A a);
   };

class B : public A
   {
     public:
          void foo();
          A operator=(A a);
          A operator+(A a);
   };

void foobar()
   {
     A a;
     B b;

  // foobar();
     foobar();
  // a.foo();
     a.foo();
  // b.foo();
     b.foo();
  // b.B::foo();
     b.B::foo();
  // b.A::foo();
     b.A::foo();

  // a = a + a;
     a = a + a;
  // b = b + b;
     b = b + b;

  // Calling base class operator=  "b.A::operator=(b);"
     b.A::operator=(b);

   }

#endif



#if 0
// number #44

void foo0()
   {
     void foobar();
     foobar();
   }

namespace X {

void foo1()
   {
     void foobar();
     foobar();
   }

void foobar(){}

void foo2()
   {
     foobar();
     X::foobar();
   }
   
}

void foobar(){}

// namespace Y {
//    void foobar(){}
// }

namespace X {
// using namespace Y;

void foo3()
   {
  // This will resolve to X::foobar();
  // foobar();

     ::foobar();

     X::foobar();
   }
}


using namespace X;
// using namespace Y;

void foo4()
   {
   // This call to foobar requires qualification
   // foobar();
     ::foobar();
     X::foobar();
  // Y::foobar();
   }

#endif

#if 0
// number #45

void foo0()
   {
     void foobar();
     foobar();
   }

namespace X {

void foobar(){}

}

void foobar(){}

using namespace X;
// using namespace Y;

void foo4()
   {
   // This call to foobar requires qualification
   // foobar();
     ::foobar();
     X::foobar();
   }

#endif


#if 0
// Just testing stuff!
namespace A
   {
     int a;
     void foo();
   }

namespace A
   {
     int b;
     void foo();
   }

namespace A
   {
     int c;
     void foo() {};
   }

namespace A
   {
     int d;
     void foo();
   }
#endif

#if 0
// Just testing stuff!
namespace A
   {
     int a;
     template<typename T> class X {};
     X<int> A_x;
   }

A::X<int> global_y;

namespace B
   {
     int b;
     A::X<int> A_z;
   }
#endif





#if 1
template <typename T>
class X
   {
     public:
          friend X<T> & operator+( X<T> & i, X<T> & j)
             {
               return i;
             }
   };

#if 1
template <typename T>
T & operator+( T & ii, T & jj)
   {
     return ii;
   }
#endif

// template X<int> & operator+( X<int> & ii, X<int> & jj);

#if 1
int main()
   {
     X<int> y,z;

  // Error used to be unparses as: "x = (+< X< int > > (y,z));"
     y + z;

     return 0;
   }
#endif
#endif
