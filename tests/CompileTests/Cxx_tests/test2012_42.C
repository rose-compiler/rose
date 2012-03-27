class X
   {
     public:
      // X (int i, double j = 42.5);
         X (int i, char* j = "XXX");
   };


X foo()
   {
     return X(1);
   }

X foobar()
   {
     return 1;
   }

