typedef unsigned long int size_t;

typedef union 
   {
     struct 
        {
          int extra_len;
        } formatted;
   } zip_header_t;

void foo()
   {
  // Note that this will generate an error if the full processing of include-staging/gcc_HEADERS/rose_edg_required_macros_and_functions.h is short circuited.
     __builtin_offsetof(zip_header_t, formatted.extra_len);
   }
