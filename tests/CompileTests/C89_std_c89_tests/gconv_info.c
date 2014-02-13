#if 0 /* JJW 5-22-2008 commented out because this is not portable to Mac */

// define __flexarr

#include <features.h>
#define __need_mbstate_t
#include <wchar.h>
#define __need_size_t
#define __need_wchar_t
#include <stddef.h>

// define __flexarr	[1]

#ifndef __mbstate_t_defined
typedef struct {} __mbstate_t;
#endif

/* Forward declarations.  */
struct __gconv_step;
struct __gconv_step_data;
struct __gconv_loaded_object;
struct __gconv_trans_data;


/* Type of a conversion function.  */
typedef int (*__gconv_fct) (struct __gconv_step *, struct __gconv_step_data *,
			    __const unsigned char **, __const unsigned char *,
			    unsigned char **, size_t *, int, int);

/* Type of a specialized conversion function for a single byte to INTERNAL.  */
typedef wint_t (*__gconv_btowc_fct) (struct __gconv_step *, unsigned char);

/* Constructor and destructor for local data for conversion step.  */
typedef int (*__gconv_init_fct) (struct __gconv_step *);
typedef void (*__gconv_end_fct) (struct __gconv_step *);


/* Type of a transliteration/transscription function.  */
typedef int (*__gconv_trans_fct) (struct __gconv_step *,
				  struct __gconv_step_data *, void *,
				  __const unsigned char *,
				  __const unsigned char **,
				  __const unsigned char *, unsigned char **,
				  size_t *);

/* Function to call to provide transliteration module with context.  */
typedef int (*__gconv_trans_context_fct) (void *, __const unsigned char *,
					  __const unsigned char *,
					  unsigned char *, unsigned char *);

/* Function to query module about supported encoded character sets.  */
typedef int (*__gconv_trans_query_fct) (__const char *, __const char ***,
					size_t *);

/* Constructor and destructor for local data for transliteration.  */
typedef int (*__gconv_trans_init_fct) (void **, const char *);
typedef void (*__gconv_trans_end_fct) (void *);

struct __gconv_trans_data
{
  /* Transliteration/Transscription function.  */
  __gconv_trans_fct __trans_fct;
  __gconv_trans_context_fct __trans_context_fct;
  __gconv_trans_end_fct __trans_end_fct;
  void *__data;
  struct __gconv_trans_data *__next;
};


/* Description of a conversion step.  */
struct __gconv_step
{
  struct __gconv_loaded_object *__shlib_handle;
  __const char *__modname;

  int __counter;

  char *__from_name;
  char *__to_name;

  __gconv_fct __fct;
  __gconv_btowc_fct __btowc_fct;
  __gconv_init_fct __init_fct;
  __gconv_end_fct __end_fct;

  /* Information about the number of bytes needed or produced in this
     step.  This helps optimizing the buffer sizes.  */
  int __min_needed_from;
  int __max_needed_from;
  int __min_needed_to;
  int __max_needed_to;

  /* Flag whether this is a stateful encoding or not.  */
  int __stateful;

  void *__data;		/* Pointer to step-local data.  */
};

/* Additional data for steps in use of conversion descriptor.  This is
   allocated by the `init' function.  */
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

  __mbstate_t *__statep;
  __mbstate_t __state;	/* This element must not be used directly by
                           any module; always use STATEP!  */

  /* Transliteration information.  */
  struct __gconv_trans_data *__trans;
};

// #define __flexarr

/* Combine conversion step description with data.  */
typedef struct __gconv_info
{
  size_t __nsteps;
  struct __gconv_step *__steps;
//  __extension__ struct __gconv_step_data __data __flexarr;
  __extension__ struct __gconv_step_data __data __flexarr;
} *__gconv_t;


typedef union
{
  struct __gconv_info __cd;
  struct
  {
    struct __gconv_info __cd;
    struct __gconv_step_data __data;
  } __combined;
} _G_iconv_t;

#endif
