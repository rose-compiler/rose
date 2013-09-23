
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

  // expands to:
  // (*d.operator->().operator->().operator->()).foo();
  //   D            C            B           A*
     d->foo();

     d->operator->()->foo();

     a->foo()->foo();
   }
