
#if 0
double __builtin_nan(const char);
double __builtin_nan_XXX(const char);

const char xxx;

char* a;

const char* b;
#endif

typedef double __v4df __attribute__ ((__vector_size__ (32)));


typedef float __v8sf __attribute__ ((__vector_size__ (32)));
typedef long long __v4di __attribute__ ((__vector_size__ (32)));
typedef unsigned long long __v4du __attribute__ ((__vector_size__ (32)));
typedef int __v8si __attribute__ ((__vector_size__ (32)));
typedef unsigned int __v8su __attribute__ ((__vector_size__ (32)));
typedef short __v16hi __attribute__ ((__vector_size__ (32)));
typedef unsigned short __v16hu __attribute__ ((__vector_size__ (32)));
typedef char __v32qi __attribute__ ((__vector_size__ (32)));
typedef unsigned char __v32qu __attribute__ ((__vector_size__ (32)));

/* The Intel API is flexible enough that we must allow aliasing with other
   vector types, and their scalar components.  */
typedef float __m256 __attribute__ ((__vector_size__ (32), __may_alias__));
typedef long long __m256i __attribute__ ((__vector_size__ (32), __may_alias__));
typedef double __m256d __attribute__ ((__vector_size__ (32), __may_alias__));

__m256d __builtin_ia32_addsubpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_addsubps256 (__v8sf __A, __v8sf __B);
__m256d __builtin_ia32_andpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_andps256 (__v8sf __A, __v8sf __B);
__m256d __builtin_ia32_andnpd256 (__v4df __A, __v4df __B);
__m256 __builtin_ia32_andnps256 (__v8sf __A, __v8sf __B);

void __builtin_ia32_maskloadpd256 (const __v4df *__P, __v4di __M);

typedef double __v4df __attribute__ ((__vector_size__ (32)));
