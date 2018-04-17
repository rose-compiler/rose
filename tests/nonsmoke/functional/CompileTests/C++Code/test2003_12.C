
#if 1
#include "test2003_12.h"
#else
typedef enum {
  A_INT,
  B_INT
} EnumDataType;

typedef struct
   {
     EnumDataType x;
   } X_Type;
#endif

void foo ()
   {
     X_Type i = { B_INT };
   }

