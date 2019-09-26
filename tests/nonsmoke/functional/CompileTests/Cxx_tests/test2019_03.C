class X 
   {
     public:
          X () {}
   };

// This tests the constructor initializer that is used to wrap hte aggregate initializer.
X *x3 = new X[2];

