// This a small example of the recent bug reported by Jeff, in his case it
// was for the case of a function call in the preinitialization list 
// (which is test2013_284.C and test2013_285.C).  This case is for a data
// member in the preinitialization list.  In this case the lack of name 
// qualification does not appear to be a problem, but in the case where the
// preinitialization list calls a function it is an issue.

namespace X
   {
     class A 
        {
          public:
            // int x;
               A();
        };
   }


#if 0
// The addition of this call does not confuse C++ or ROSE.
class A 
   {
     public:
          int x;
          A(int x);
   };
#endif

class B : public X::A
   {
     public:
       // int x;
       // B(int x);
          B();
   };

// This is unparsed as: B::B(int x) : A(x){}
// It is missing it's name qualification "X::" for "A", but it appears to compile fine with g++ (version 4.2) and also EDG.
// B::B (int x)
B::B ()
   : X::A()
   {
   }


