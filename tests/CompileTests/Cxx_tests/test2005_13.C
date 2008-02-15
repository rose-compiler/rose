// Original code:
//      error_free &= KULL_SWIG_TypeChecker::TypeChecker< Tag::TagType const & >::registerType (baseType, "Tag::TagType const &", moduleName, fileName, "[1423]");
// Generated code:
//      error_free &= KULL_SWIG_TypeChecker::TypeChecker < const enum Tag::TagType {UnknownTag,ZoneTag,FaceTag,EdgeTag,NodeTag}& > ::registerType(baseType,std::basic_string < char , std::char_traits< char > , std::allocator< char > > (("Tag::TagType const &")),std::basic_string < char , std::char_traits< char > , std::allocator< char > > (((const char *)moduleName)),std::basic_string < char , std::char_traits< char > , std::allocator< char > > (((const char *)fileName)),std::basic_string < char , std::char_traits< char > , std::allocator< char > > (("[1423]")));

// Solution: ???
// Problem may be that enum values should not be output as template arguments


// Skip version 4.x gnu compilers
#if ( __GNUC__ == 3 )

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

#else
  #warning "Not tested on gnu 4.0 or greater versions"
#endif

