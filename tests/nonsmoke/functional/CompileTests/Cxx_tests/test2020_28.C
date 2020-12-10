extern "C"
   {
     class X
        {
       // BUG: this is output as: "public: extern "C" { friend int foo(class X_ &); }"
       // friend functions are marked as 0 which corresponds to public, because there 
       // is not concept of default in EDG.  We have to account for this in how we set 
       // it in ROSE.
          friend int foo();

          int ijk;
        };
   }

