// Skip version 4.x gnu compilers
// #if ( __GNUC__ == 3 )

enum values { zero, nonzero };

class A
   {
     public:
#if 1
          enum values { zero, nonzero };
          enum TagType { UnknownTag=0, ZoneTag=1, FaceTag=2, EdgeTag=3, NodeTag=4 };
#endif
          template <typename T1>
          void foo() {}
   };

#if 1
template <typename T>
class X 
   {
     public:
          template <typename T1>
          void foo() {}
   };
#endif

void foo()
   {
#if 1
     X<values> x1;
     X<A::values> x2;
     X<A::values const & > x3;
     X<A::TagType const & > x4;
     x4.foo<int>();

     X<A::TagType> x5;
     x5.foo<A::TagType>();

  // These also (used to) fail!
     x5.foo<A::TagType &>();
     x5.foo<A::TagType const &>();
#endif

     A x6;
     x6.foo<values const>();
  // x6.foo<A::TagType const>();
   }

// #else
//   #warning "Not tested on gnu 4.0 or greater versions"
// #endif
