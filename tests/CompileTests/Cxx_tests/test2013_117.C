
struct A
   {
  // void foo(); 
     A foo(); 
     A *operator->();
   };

struct B { A* operator->(); };

struct C { B & operator->(); };

struct D { C operator->(); };

int main() 
   {
     D d;
     A a;

  // These all expand semantically to be:
  // (*d.operator->().operator->().operator->()).foo();
  //   D            C            B           A*
     (*d.operator->().operator->().operator->()).foo();
     d->foo();
     d->operator->()->foo();

  // This is semantically a bit different:
     a->foo()->foo();
   }
