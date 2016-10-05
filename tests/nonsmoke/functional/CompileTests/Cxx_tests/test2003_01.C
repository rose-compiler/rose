// This code shows an error in the unparser.
// Use of global class A in the main function is not unparsed properly 
// (unparsed code is missing "::" scope operator in the unparsed code 
// and so the wrong version of A is referenced)

class A
   {
     public:
          void foo1(int x) { int a; };
   };

namespace B {
class A
   {
     public:
          void foo2(double x) { double a; };
   };
}

int main()
   {
  // Declaration of a local class (legal C++ but unusual construction)
     class A
        {
          public:
            // Why does foo3 have to be defined???  Answer: Because there is no other place where its definition could be placed structurally.
               void foo3(){};
        };

  // Use of global class A is not unparsed properly (unparsed code is missing "::" scope operator)
     ::A global_a;
     global_a.foo1(1);

  // Use of namespace class A is not unparsed properly (unparsed code is missing "B::" scope operator)
     B::A namespace_a;
     namespace_a.foo2(1.0);

  // Use of local class A just fine
     A local_a;
     local_a.foo3();

     return 0;
   }

