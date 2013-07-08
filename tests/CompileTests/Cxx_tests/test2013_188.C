
namespace X
   {
     class A
        {
          public:
               enum FieldType
                  {
                    ALPHA
                  };

               template <typename CType, enum FieldType DeclaredType> static inline bool ReadPrimitive(CType* value);

            // template <enum FieldType DeclaredType> class B{};
        };

     template <typename CType, enum A::FieldType DeclaredType> bool X_ReadPrimitive(CType* value);
  // A::B<A::ALPHA> b;
   }

// template <enum X::A::FieldType DeclaredType> void foobar();

void foo ()
   {
  // foobar<X::A::ALPHA>();
     X::A::ReadPrimitive<int,X::A::ALPHA>(0);

  // X::A::B<X::A::ALPHA> b;

     X::X_ReadPrimitive<int,X::A::ALPHA>(0);
   }
