enum class EnumClass1 
   {
     enumValue1 = 1
   };

EnumClass1 func1() 
   {
  // Note that name qualification is required here because enumValue1 
  // is not visible in global scope for a "enum class".
     return EnumClass1::enumValue1;
   }

// EnumClass1 x = EnumClass1::enumValue1;
