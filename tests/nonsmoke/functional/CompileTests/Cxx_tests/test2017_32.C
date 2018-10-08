namespace std
{
  typedef long unsigned int size_t;
  typedef long int ptrdiff_t;
}

namespace std
{
  inline namespace __cxx11 __attribute__((__abi_tag__ ("cxx11"))) { }
}

namespace __gnu_cxx
{
  inline namespace __cxx11 __attribute__((__abi_tag__ ("cxx11"))) { }
}

// # 40 "/nfs/casc/overture/ROSE/opt/rhel7/x86_64/gcc/5.1.0/mpc/1.0/mpfr/3.1.2/gmp/5.1.2/lib/gcc/x86_64-unknown-linux-gnu/5.1.0/include/stdarg.h" 3 4
// typedef __builtin_va_list __gnuc_va_list;
// typedef __gnuc_va_list va_list;

typedef __builtin_va_list va_list;

namespace std
{
  using ::va_list;
}

void foobar ( std::va_list args)
   {
     std::va_list args_copy;

     __builtin_va_copy(args_copy,args);
   }
