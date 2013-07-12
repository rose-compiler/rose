namespace X
   {
     class A
        {
          public:
               enum FieldType
                  {
                    ALPHA
                  };   

               template <typename CType, enum FieldType DeclaredType> static bool static_ReadPrimitive(CType* value);
               template <typename CType, enum FieldType DeclaredType> bool ReadPrimitive(CType* value);
        };   
   }

template <enum X::A::FieldType DeclaredType> void foobar();

void foo ()
   {
  // Test for enum template argument name qualification on non-member function.
     foobar<X::A::ALPHA>();

  // Test for enum template argument name qualification on static member function.
     X::A::static_ReadPrimitive<int,X::A::ALPHA>(0);

  // Test for enum template argument name qualification on member function.
     X::A* a;
     a->ReadPrimitive<int,X::A::ALPHA>(0);
   }
