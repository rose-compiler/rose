
// This is not defined by default for GNU unless we include the system headers.
// It is defined by default for ROSE, since it is required for the Plum Hall 
// tests and I think it is required within the C++ standard.  It is OK to
// redeclare it as long as it is done consistantly!
#ifdef __LP64__
// 64 bit case
typedef long unsigned int size_t;
#else
// 32 bit case
typedef unsigned int size_t;
#endif

struct __gconv_step_data
{
  unsigned char *__outbuf;    /* Output buffer for this step.  */
  unsigned char *__outbufend; /* Address of first byte after the output
                                 buffer.  */

  /* Is this the last module in the chain.  */
  int __flags;

  /* Counter for number of invocations of the module function for this
     descriptor.  */
  int __invocation_counter;

  /* Flag whether this is an internal use of the module (in the mb*towc*
     and wc*tomb* functions) or regular with iconv(3).  */
  int __internal_use;
#if 0
  __mbstate_t *__statep;
  __mbstate_t __state;	/* This element must not be used directly by
                           any module; always use STATEP!  */

  /* Transliteration information.  */
  struct __gconv_trans_data *__trans;
#endif
};


typedef struct __gconv_info
{
  size_t __nsteps;

// This will be marked as having scope: "struct __gconv_info".  This is reasonable since 
// the declaration is never seen.  Unfortunately, as a result we need to supress the output
// of qualified names in C so that we generate legal code (regardless of the scope).
  struct __gconv_step *__steps;

// __extension__ struct __gconv_step_data __data __flexarr;
// __extension__ struct __gconv_step_data __data;
} *__gconv_t;


typedef union {
  // struct __gconv_info __cd;

  // In the code generation we will generate a name for this structure and the union, 
  // thus we have to suppress the generation of qualified names for the C code that 
  // is generated.
     struct {
          struct __gconv_info __cd;
          struct __gconv_step_data __data;
        } __combined;
   } _G_iconv_t;


