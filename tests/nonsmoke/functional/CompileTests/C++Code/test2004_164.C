// Example demonstrating bug in ROSE (likely in unparsing of AST)
// Original code (aaa,bbb,ccc are types):
//      result = (arg1)->operator *((aaa::bbb<aaa::XY >::ccc const &)*arg2);
// Unparses as:
//      result=*arg1;

// the following code demonstrated what i think is the same problem but in a form easier
// to fix than in the form it appears within Kull.

class A
   {
     public:
          A & operator*(A* ptr);
          void foo();
   };

void foo()
   {
     A a1;
     A *a2 = &a1;
     A *a3 = &a1;

  // Original code: (a3->operator*(a2)).foo();
  // ERROR Unparsed code: (*a3).foo();
     (a3->operator*(a2)).foo();

#if 0
  // Alternative formulation using overloaded operator syntax (with same result)
     ((*a3) * a2).foo();
#endif
   }

