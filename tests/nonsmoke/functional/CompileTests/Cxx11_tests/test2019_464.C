
class B
   {
     public:
       // This is the function being called, but we need to 
       // distinguish it from C::foo() since both are visible.
          void foo (int x);
   };

class C : public B
   {
  // Because B::foo(int) is included in C we need to differentiate 
  // which is intended using name qualification. So we need B::foo
  // in the call from C::foo().
     public:
          void foo ();
   };

void C::foo()
   {
  // These are semanticially equivalent but using different syntax.
  // B::foo(42);
     this->B::foo(42);
   }


