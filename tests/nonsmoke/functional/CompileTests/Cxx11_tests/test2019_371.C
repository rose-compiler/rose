struct S 
   {
  // static enum { No, Yes } locked;
  // static enum {} locked;
     static int foobar();
   };

// int f( decltype(S::locked) );
int f( decltype(S::foobar()) );
