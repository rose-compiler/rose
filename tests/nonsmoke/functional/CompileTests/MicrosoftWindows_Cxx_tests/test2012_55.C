class X
   {
     public:
       // test template function
//        template <typename T> void foo3(void) {}
   };


void foobar()
   {
     X x;
//     x.foo3<char>();
  // foo3();  // This is an error, since it does not contain enough information for type resolution
   }
