

class A {};

class B
   {
     protected:
          bool foo (A* valuePtr);
   };

class C : public B
   {
     private:
          virtual void foo ();
   };

void C::foo()
   {
     A* valuePtr;
     B::foo(valuePtr);
   }


