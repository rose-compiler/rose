// This code tests the unparsing of typedefs of member function pointers

/*
// original code:
typedef void (A::*PointerToMemberFunctionType)();
// unparsed code:
typedef A (A::*PointerToMemberFunctionType);
 */

class A {};
// typedef float MyType;
typedef void (*MyPointerToFunctionType)();
typedef void (A::*MyPointerToMemberFunctionType)();

// PointerToMemberFunctionType X = 0;

#if 0
// Checking effect of <TABS> on column information in AST
int main ()
   {
     int a,b,c=2;
     b = 1;
     a = b + c;

		int d;
                return 0;
   }
#endif
