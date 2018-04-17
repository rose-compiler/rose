class X
   {
     public:
#if 1
          X () {}
#endif
   };


X
foo()
   {
     return X();
   }

