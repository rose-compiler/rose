/*
 * This code tests a number of features of the implicit call generator.
 * It contains a tricky edge case involving nested constructors.
 */
#define CLS(A) \
struct A \
   { \
\
     bool b; \
\
     A() : b(false) \
	{ \
	} \
\
     A(bool b) : b(b) \
	{ \
	} \
\
     ~A() \
	{ \
	} \
\
     operator bool() \
	{ \
	  return true; \
	} \
\
   }

CLS(A);
CLS(B);
CLS(C);
CLS(D);
CLS(E);

int main()
   {
     int a;
     if ((A() && B(C() && D())) || E())
	{
	  return 42;
	}
   }
