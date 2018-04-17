/*
Hi Dan,
It seems I modified the example, so now another unparsing error occurs. 
I will send you shortly the old example, which generated an error by adding 
extra paranthesis. This is some code generating another problem:

class A
{
public:
         int f1() {}
         int f2() { pf = &A::f1; return (this->*pf)(); }
         int (A::*pf) ();
};

It gets transformed to this one via unparsing:

class A
{
   public: inline int f1()
{
  }
  inline int f2()
{
    (this) -> pf = A::f1; // !!!!! this should have been &A::f1 !!!!!
    return ((this) ->* (this) -> pf)();
  }
  int (A::*pf)();
}
;

Radu 

*/


class A
   {
     public:
          int f1() {}
          int f2() { pf = &A::f1; return (this->*pf)(); }
          int (A::*pf) ();
   };

void foo ()
   {
     void (*globalFunctionPointer)();

  // Note that both of these work the same but for member functions 
  // only the &<qualified function name> is allowed.  Thus,
  // initialization of global function pointers can use either "foo" 
  // or "&foo" while member function can only use "&A::f1" and NOT "A::f1"
     globalFunctionPointer = foo;
     globalFunctionPointer = &foo;
   }
