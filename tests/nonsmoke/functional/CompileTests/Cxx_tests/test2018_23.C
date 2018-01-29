typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;

int ivalue(int);
int iequals(int, int, int);



#include <stdarg.h>

struct X_
	{
	int i, j;
	// X_(int ii, int jj) : i(ii), j(jj) { }  SR00872
	int f(int n) { return i + j + n; }
	};

typedef int X_::*PMI_;

typedef int (X_::*PMF_)(int);

X_ x_ = {132, 51};

void foobar(int i, ...)
   {
     va_list ap;
     __builtin_va_start(ap,i);
      iequals(29, ivalue(__builtin_va_arg(ap,int)), ivalue(132));
   // dequals(32, dvalue(__builtin_va_arg(ap,double)), dvalue(dvalue(51)));
      iequals(35, ivalue(__builtin_va_arg(ap,X_).f(-1)), ivalue(x_.f(-1)));
      iequals(38, ivalue(x_.*__builtin_va_arg(ap,PMI_)), ivalue(51));
      iequals(41, ivalue((x_.*__builtin_va_arg(ap,PMF_))(1)), ivalue(132 + 51 + 1));
   }
#if 0
#endif

