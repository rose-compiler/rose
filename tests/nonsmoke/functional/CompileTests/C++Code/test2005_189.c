// This test code demonstrates the handling of the "flexible array data members"
// this is a construct such as "struct tag { int f[] } type;" The use of this 
// type in another struct is not allowed in strict c99 mode, but is allowed
// in the default C mode (used when the file extension is ".c").  

// Details of C, ANSI C, C99, ANSI C99, C++ modes are in the ROSE User Manual appendix.

// The trick to making this code compile is to not use the -rose:C_only 
// or -rose:C99_only flags.  These commandline flags trigger the strict ANSI modes
// of C and C99, respectively.  Within the strict C or C99 modes the 
// structure containing the flexible array can not be use as a data member.
// See EDG manual "C99 Features Available in Other Modes" for more details.

// #undef __flexarr
// #define __flexarr	[1]
// #define __flexarr

#if 0
// This code appears in /usr/include/cdef.h and resets the
// value of "__flexarr" to be "[]", since __GNUC_PREREQ (2,97)
// is true (as part of g++ support in ROSE via EDG).  We can't
// control the use of the /usr/include/cdef.h header file since
// it is not a part of the gnu header files that we copy and 
// modify slightly to make work with EDG as part of ROSE.

/* Support for flexible arrays. */
#if __GNUC_PREREQ (2,97)
/* GCC 2.97 supports C99 flexible array members.  */
#warning __GNUC_PREREQ (2,97) is TRUE
# define __flexarr	[]
#else
#warning __GNUC_PREREQ (2,97) is FALSE
# ifdef __GNUC__
#  define __flexarr	[0]
# else
#  if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#   define __flexarr	[]
#  else
/* Some other non-C99 compiler.  Approximate with [1].  */
#   define __flexarr	[1]
#  endif
# endif
#endif

#endif


class XX { int x; };

struct Atag
   {
     int x;
   } Atype;

struct Xtag
   {
  // Original problem data member when compiling with -rose:C_only (use .c file and don't use -rose:C_only)
  // __extension__ struct __gconv_step_data1 __data __flexarr;
     __extension__ struct Atag __data [];
   } *Xtype;

struct
   {
     struct Xtag my__cd;
     int x;
   } newType;



