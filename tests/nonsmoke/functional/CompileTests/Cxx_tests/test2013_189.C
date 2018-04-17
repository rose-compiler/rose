namespace X
   {
     class A
        {
          public:
               enum FieldType
                  {
                    ALPHA
                  };   

            // template <typename CType, enum FieldType DeclaredType> static inline bool ReadPrimitive(CType* value);
               template <typename CType, enum FieldType DeclaredType> static void ReadPrimitive(CType* value);
        };

     template <typename CType, enum A::FieldType DeclaredType> void X_ReadPrimitive(CType* value);
   }

void foo ()
   {
     X::A::ReadPrimitive<int,X::A::ALPHA>(0);
     X::X_ReadPrimitive<int,X::A::ALPHA>(0);
   }
