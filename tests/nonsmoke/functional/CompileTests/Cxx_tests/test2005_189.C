// DQ (8/20/2006): I don't know why I have this comment here, I now thinik 
// it is incorrect, this is only allowed in C++ and not in C or C99.

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

struct XX { int x; };

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



