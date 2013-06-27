// DQ (6/20/2013): Use of what translates into case enk_alignof in convert_expression() in EDG 4.7.

// Determine the alignment of a type at compile time.
#if defined __GNUC__
# define sa_alignof __alignof__
#elif defined __cplusplus
  template <class type> struct sa_alignof_helper { char __slot1; type __slot2; };
# define sa_alignof(type) offsetof (sa_alignof_helper<type>, __slot2)
#elif defined __hpux
  /* Work around a HP-UX 10.20 cc bug with enums constants defined as offsetof values.  */
# define sa_alignof(type) (sizeof (type) <= 4 ? 4 : 8)
#elif defined _AIX
  /* Work around an AIX 3.2.5 xlc bug with enums constants defined as offsetof values.  */
# define sa_alignof(type) (sizeof (type) <= 4 ? 4 : 8)
#else
# define sa_alignof(type) offsetof (struct { char __slot1; type __slot2; }, __slot2)
#endif

enum
   {
     sa_alignment_long = sa_alignof (long),
     sa_alignment_double = sa_alignof (double),
#if HAVE_LONG_LONG_INT
     sa_alignment_longlong = sa_alignof (long long),
#endif
     sa_alignment_longdouble = sa_alignof (long double),
     sa_alignment_max = ((sa_alignment_long - 1) | (sa_alignment_double - 1) | (sa_alignment_longdouble - 1)) + 1,
     sa_increment = ((sizeof (int) + sa_alignment_max - 1) / sa_alignment_max) * sa_alignment_max
   };

   
