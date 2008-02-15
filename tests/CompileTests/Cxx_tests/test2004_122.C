namespace A
   {
     template <typename T> class B1{};
     template <typename T> class B2{};
     class C{};

      typedef int Integer;

      typedef B2<int> B_int;
      typedef B2<Integer> B_Integer;
      typedef B2<C> B_class_C;
   }

void foo1a ( A::B1<int> ab );
void foo1b ( A::B_int ab );
void foo1c ( A::B_Integer ab );
void foo1d ( A::B_class_C ab );
// void foo1e ( B::B_class_A_C ab );

// This demonstrates problem with Kull SWIG generted code
void foo2a( A::B1< A::B2< int >  >  ab);

// Global Variable Declaration 
// A::B1< A::B_int  >  global_ab;

// Unparses to: "extern void foo2b(class A::B< A::B< int >  >  ab);"
void foo2b( A::B1< A::B_int  >  ab);

#if 0
// Unparses to: "extern void foo2c(class A::B< A::B< int >  >  ab);"
void foo2c( A::B1< A::B_Integer  >  ab);

void foo2d( A::B1< A::B_class_C  >  ab);
// void foo2e( A::B< B::B_class_A_C  >  ab);

void foo3a( A::B1< A::B2< A::B2< int > >  >  ab);

// Unparses to: "extern void foo3b(class A::B< A::B< A::B< int > >  >  ab);"
void foo3b( A::B1< A::B2< A::B_int >  >  ab);

// Unparses to: "extern void foo3c(class A::B< A::B< A::B< int > >  >  ab);"
void foo3c( A::B1< A::B2< A::B_Integer >  >  ab);

// Unparses to: "extern void foo3d(class A::B< A::B< A::B< int > >  >  ab);"
void foo3d( A::B1< A::B2< A::B_class_C >  >  ab);
// void foo3e( A::B< A::B< B::B_class_A_C >  >  ab);

// This works fine
// void foo ( A::B<int> ab ) {}
#endif
