// ROSE-1904: (C++11) reference to scoped enumeration must use 'enum' not 'enum class'

enum class errc 
   {
     address_family_not_supported = 97
   };

// This unparses as: "void make_error_code(enum class errc );"
// which is accepted by GNU 5.1, but it not accepted by CLANG 8.0
void make_error_code(errc);


