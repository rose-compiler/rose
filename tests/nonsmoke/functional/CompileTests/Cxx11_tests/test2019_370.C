struct S 
   {
  // static enum { No, Yes } locked;
     static enum {} locked;
   };

int f( decltype(S::locked) );
