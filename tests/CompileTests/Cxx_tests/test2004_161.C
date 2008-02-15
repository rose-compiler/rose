// The error is in the handling of a vacuous pseudo destructor call from a pointer
// This seems to happen in many files within Kull project!  It appears to not generally
// be a problem sense the difficult code is not unparsed in the current source file.
// So all places where this occurs are not kept from compiling properly.
// It would be a problem if we were to unparse all the headers, I think.
// Though since we have specified the problem function as inlined, why is it not
// a problem within Kull?

// Here is a great explaination of "Pseudo Destructors"

// Fundamental types have a pseudo destructor. A pseudo destructor is a
// syntactic construct whose sole purpose is to satisfy the need of
// generic algorithms and containers. It is a no-operation code and has
// no real effect on its object. For example:
 
// typedef int N;
// int main()
//    {
//      N i = 0;
//      i.N::~N(); // pseudo destructor invocation
//      i = 1;     // i  was not affected by the invocation of the pseudo
//      return 0;
//    }

// In the statement numbered 1, the pseudo destructor of the nonclass
// type N is explicitly invoked. Pseudo destructors enable you to write
// generic code that doesn't have to know the exact type of every
// argument. A good example is a generic container that can store
// elements of user-defined types as well as fundamental types.

// Template function to destroy the object pointed to by a pointer type.
// the keyword "inline" is required to force the template function be 
// processed as a definition (without "inline" it is processed as a 
// function prototype)!

// Problem Code from: 
//      filename = /usr/casc/overture/dquinlan/ROSE/LINUX-3.3.2/g++_HEADERS/hdrs1/bits/stl_construct.h
//      line     = 136  column = 22 
template <class _Tp>
inline void _Destroy (_Tp* __pointer)
   { __pointer->~_Tp(); }

// We need the complete type since the inlined function will be output and -> is called!
class A
   {
     public:
          A();
         ~A();
   };

void foo()
   {
     A *a1;
  // This line works fine!
     _Destroy<A>(a1);
     A **a2 = &a1;

#if 0
 // This line fails since I don't have a name for the "A*" type 
 // unless I generate an alias via a compiler generated typedef
 // However, generating a typedef does not seem to preserve the 
 // source-to-source behavior.
     _Destroy<A*>(a2);
#endif
   }

