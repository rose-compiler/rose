
#if 1
// has_explicit_enum_base == true && is_scoped_enum == true
enum class Enum2 : unsigned int 
#else
// has_explicit_enum_base == false && is_scoped_enum == false
enum Enum2 
#endif
   {
     Val1, 
     Val2
   };
