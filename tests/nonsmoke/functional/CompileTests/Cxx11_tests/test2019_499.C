
// This corresponds to the assertion failing at line 238 of the unparseCxx_types.C file.
// This is tied for the 4th most numerous issue in testing (causing failure in 2 files).

class A {};

namespace N
   {
     namespace P
        {
          enum Type
             {
               P1
             };
       }

     namespace Q
        {
          enum Type
             {
               Q1,Q2
             };
       }

     namespace R
        {
          enum Type
             {
               R1
             };
       }
   }

enum E { E1 };

static const int MINIMUM_VALUE = 42;

class X_input
   {
     protected:
          X_input(const char* name, N::P::Type s1, N::Q::Type s2, int s3, int s4, E s5, int s6 = MINIMUM_VALUE, N::R::Type s7 = N::R::R1, N::Q::Type s8 = N::Q::Q1, int s9 = 0 );
   };

class X : public X_input
   {
     public: 
          X();

     protected:
       // virtual bool foo();
          inline void foo (A a);
   };

namespace Ba_xyz
   {
     enum Type
        {
          BIT_RESULTS
        };
   }


#define DEFAULT_VALUE 42

template <class T>
void ba_foobar(void(T::*functionPtr)(A), Ba_xyz::Type def, T* hij, int value = DEFAULT_VALUE) 
   {
   }

using namespace N;

X::X() : X_input("XXX", P::P1, Q::Q2, 42, 42, E1)
   {
     ba_foobar(&X::foo, Ba_xyz::BIT_RESULTS, this);
   }
