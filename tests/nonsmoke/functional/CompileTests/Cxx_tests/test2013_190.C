namespace X
   {
     class A
        {
          public:
               enum FieldType
                  {
                    ALPHA
                  };   

               template <typename CType, enum FieldType DeclaredType>
               static inline bool ReadPrimitive(CType* value);
        };   
   }

template <enum X::A::FieldType DeclaredType>
void foobar();

void foo ()
   {
     foobar<X::A::ALPHA>();
     X::A::ReadPrimitive<int,X::A::ALPHA>(0);
   }
