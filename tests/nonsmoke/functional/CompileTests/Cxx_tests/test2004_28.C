
#if 0
int volatile x;
extern const volatile int *y;
#endif

// Turn on use of restrict in EDG front-end using --edg:restrict
#ifdef __GNUC__
// for GNU g++
#define RESTRICT __restrict__
#else
// for ROSE
#define RESTRICT restrict
#endif

#if 0
// int* __restrict__ z;

int* RESTRICT a;

int a1, * const RESTRICT a2 = 0;
#endif

#if 0
void foo ( const int a )
   {
  // register int x1;
     const int x2 = 0;
  // register int b1, b2;
     int b1, b2;
   }
#endif

#if 0
// This does not work (invalid C++ code, a good thing too)
void testVarDeclarationFunctionParameter ( struct X { int x; } r )
   {

   }
#endif


#if 0
int testRegister ( register long r )
   {  /* the register keyword*/
   }
#endif


#if 1
void testRegister ( register long r )
   {  /* the register keyword*/
   }
#endif

// int* restrict abc;
// int* restrict abc;
// const int def = 0;

#if 0
struct ghi_tag
   {
     ghi_tag();
     ghi_tag(int);
     int x;
   } ABC;

ghi_tag ghi;
const ghi_tag jkl = 0;

enum enum_test
   {
     x0 = 0,
     x1 = 1
   }; // enum_var1;
#endif

#if 0
int main()
   {
     return 0;
   }
#endif
