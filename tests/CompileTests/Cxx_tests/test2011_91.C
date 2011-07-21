class foo
   {
     public:
        int bar_variable;
   };

template < int foo :: * M > void test ( ) { }

void foobar()
   {
  // Unparses as: "test < &bar_variable > ();"
     test < &foo::bar_variable > ();
   }
