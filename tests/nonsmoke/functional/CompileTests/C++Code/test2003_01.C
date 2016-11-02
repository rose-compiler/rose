// This code shows an error in the unparser.
// Use of global class A in the main function is not unparsed properly 
// (unparsed code is missing "::" scope operator in the unparsed code 
// and so the wrong version of A is referenced)

class A
   {
     public:
          void foo1(int x) { int a; };
   };

int main()
   {
  // Declaration of a local class (legal C++ but unusual construction)
     class A
        {
          public:
            // Why does foo3 have to be defined???
               void foo2(){};
        };

  // Use of local class A just fine
     A a;
     a.foo2();

  // Use of global class A is not unparsed properly (unparsed code is missing "::" scope operator)
     ::A global_a;
     global_a.foo1(1);

     return 0;
   }

