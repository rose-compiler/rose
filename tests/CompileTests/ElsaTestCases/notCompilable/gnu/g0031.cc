// g0031.cc
// using __attribute__ mode to select data type

// The page
//   http://gcc.gnu.org/onlinedocs/gcc-3.1/gcc/Variable-Attributes.html
// documents the notion of __attribute__ __mode__, however I cannot
// find anything that specifies what the modes themselves mean.  So
// I will simply infer their meaning from how they are used in the
// glibc headers.

// /usr/include/sys/types.h, around line 177, after macro expansion
typedef int int8_t __attribute__ ((__mode__ (  __QI__ ))) ;
typedef int int16_t __attribute__ ((__mode__ (  __HI__ ))) ;
typedef int int32_t __attribute__ ((__mode__ (  __SI__ ))) ;
typedef int int64_t __attribute__ ((__mode__ (  __DI__ ))) ;

typedef unsigned int u_int8_t __attribute__ ((__mode__ (  __QI__ ))) ;
typedef unsigned int u_int16_t __attribute__ ((__mode__ (  __HI__ ))) ;
typedef unsigned int u_int32_t __attribute__ ((__mode__ (  __SI__ ))) ;
typedef unsigned int u_int64_t __attribute__ ((__mode__ (  __DI__ ))) ;


// way to test at compile-time that two types are equal; template
// argument deduction will fail if the args are not equal; Elsa has
// __checkType, but other compilers don't, and I especially want to
// test against GCC
template <class T>
void equalTypes(T,T);

// test 'equalTypes'
void foo()
{
  int i;
  char c;

  equalTypes(i,i);
  //ERROR(1): equalTypes(i,c);
  equalTypes(c,c);
}


// compare the __mode__ types above
void bar()
{
  int8_t  i8;
  int16_t i16;
  int32_t i32;
  int64_t i64;

  u_int8_t  ui8;
  u_int16_t ui16;
  u_int32_t ui32;
  u_int64_t ui64;
  
  signed char sc;
  unsigned char uc;
  
  signed short ss;
  unsigned short us;
  
  signed int si;
  unsigned int ui;
  
  signed long long sll;
  unsigned long long ull;
  
  equalTypes(i8,  sc);
  equalTypes(i16, ss);
  equalTypes(i32, si);
  equalTypes(i64, sll);

  equalTypes(ui8,  uc);
  equalTypes(ui16, us);
  equalTypes(ui32, ui);
  equalTypes(ui64, ull);
}




