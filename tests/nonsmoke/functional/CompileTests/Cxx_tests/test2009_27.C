#include <stdint.h>

class X
   {
     public:
          static int8_t swap_bytes(int8_t n);
// #ifndef USE_ROSE
       // DQ (8/25/2009): These are a problem for ROSE compiling ROSE
       // (error message from EDG: function "SgAsmExecutableFileFormat::swap_bytes(int8_t)" has already been declared)
          static int16_t swap_bytes(int16_t n);
          static int32_t swap_bytes(int32_t n);
          static int64_t swap_bytes(int64_t n);
// #endif
   };
