namespace foo
   {
     class bar_class{};
   };

// template < int foo :: * M > void test ( ) { }
template < typename  T > class test {};

void foobar()
   {
  // Unparses as: "test < &bar_variable > ();"
  // test < &foo::bar_variable > ();

     test < foo::bar_class > A;
   }
