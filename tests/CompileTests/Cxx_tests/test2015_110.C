// DQ (11/2/2015): This is a copy of test2014_124.C (with added comments specific to a bug fix for for loop test unparsing).

class A
   {
     public:
       // A () {}
          A (int) {}
          operator bool () { return false; }
   };

void foo (int x)
   {
  // Original code: for (; A x22222222222222 = 0;)
  // Unparsed as: for (; class A x22222222222222(0);)
     for (; A x22222222222222 = 0;)
        x++;
   }
