// #include <stdio.h>

struct X;

struct X
   {
     int y;
   };


#if 0
// This is a typedef to a function pointer
typedef void (*function_pointer_type) (struct X  *);

struct Y
   {
     function_pointer_type function_pointer;
   };

#endif

#if 0
// This is a typedef to a function pointer
typedef int (*__gconv_trans_fct) (struct __gconv_step *,struct __gconv_step_data *, void *, __const unsigned char *, __const unsigned char **, __const unsigned char *, unsigned char **, size_t *);
#endif

#if 0
// Typedef with multiple types declared (each is an typedef of int to x,y,z)
// typedef int x,y,z; is equivalent to:
// typedef int x;
// typedef int y;
// typedef int z;
typedef int x,y,z;
#endif

#if 0
// typedef int (*__gconv_trans_fct) ();

struct X;

struct X
   {
  // __gconv_trans_fct __trans_fct;
     int y;
   };

#endif
