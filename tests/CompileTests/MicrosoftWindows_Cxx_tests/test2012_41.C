// this test is a combination of two bug presented 
// separately in test2012_39.C and test2012_40.C.

class X
   {
     public:
          X () {}
   };

class Y
   {
   };


X
foo()
   {
     return X();
   }

Y
foobar()
   {
     return Y();
   }

