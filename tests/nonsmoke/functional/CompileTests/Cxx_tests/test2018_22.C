
typedef __builtin_va_list __gnuc_va_list;
typedef __gnuc_va_list va_list;

int ivalue(int);
int iequals(int, int, int);

struct X
   {
     int i, j;
     int f(int n);
   };

typedef int X::*PMI_;

typedef int (X::*PMF_)(int);

// #define XX__builtin_va_arg(v,l) (class l)(sizeof(l))
// #define __builtin_va_arg(v,l) (unsigned long)(sizeof(l))
// #define __builtin_va_arg(v,l) (l)(sizeof(l))
// #define __builtin_va_arg(v,l) ({l xxx; xxx;})
// #define __builtin_va_arg(v,__type) ({__type __typeValue; __typeValue;})

// void* rose__builtin_va_arg(const unsigned int, const unsigned int);
// #define __builtin_va_arg(v,type) rose__builtin_va_arg(v,sizeof(type))
// #define __builtin_va_arg builtin_va_arg

// Since we can't turn on the builtin function (conflicts with EDG C++11 constexpr protytypes)
// we need to define this in a way that we can recognize it and transform it back to a builtin
// function.  Alternatively, we could selectively turn on the EDG support for only a few builtin
// functions.
// #define __builtin_va_arg(v,__type) ({__type __builtin_va_arg_variable; __builtin_va_arg_variable;})

X x;

void foobar(int i, ...)
   {
     va_list ap;
     __builtin_va_start(ap,i);

 // iequals(29, ivalue(__builtin_va_arg(ap,int)), ivalue(132));
    iequals(29, ivalue(__builtin_va_arg(ap,int)), ivalue(132));

    iequals(35, ivalue(__builtin_va_arg(ap,X).f(-1)), ivalue(x.f(-1)));
   }

