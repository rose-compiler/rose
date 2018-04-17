
namespace X
   {
     enum FieldType
        {
          ALPHA
        };

     template <typename CType, enum FieldType DeclaredType> void ReadPrimitive(CType* value);
   }

void foo ()
   {
     X::ReadPrimitive<int,X::ALPHA>(0);
   }
