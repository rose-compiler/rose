
namespace internal 
   {
     template <bool>
     struct CompileAssert {};
   }

void foo() 
   {
  // Unparsed as: typedef struct internal::CompileAssert< true > [1?1 : -1]arg_count_mismatch[true?1 : -1];
  // typedef struct internal::CompileAssert< true > arg_count_mismatch[true?1 : -1];
     typedef struct internal::CompileAssert< true > arg_count_mismatch[1];
   }
