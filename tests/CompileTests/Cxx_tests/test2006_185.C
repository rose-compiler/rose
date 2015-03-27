/*
 * This code tests a number of features of the implicit call generator.
 * It contains a tricky edge case involving nested constructors.
 */
#define CLS(NAME) \
struct NAME \
   { \
     bool b; \
     NAME() : b(false) {} \
     NAME(bool b) : b(b) {} \
     ~NAME() {} \
     operator bool() { return true; } \
   }

CLS(A);
CLS(B);
CLS(C);
CLS(D);
CLS(E);

int
main()
   {
     int a;

  // Original code is: if ( A() );
     if ( A() );

  // Original code is: if ((A() && B(C() && D())) || E())
  // Unparsed as: if ((A::A()) && (B::B((C::C()) && (D::D()))) || (E::E()))
#if (__GNUC__ == 4) && (__GNUC_MINOR__ <= 4)
  // This example fails for g++ version 4.8.1.
     if ((A() && B(C() && D())) || E())
        {
        }
#else
  // This is the only version of code that GNU g++ version 4.8.1 will accept, I think this is a bug in g++ 4.8.1.
     if ( A() && B(C() && D()) || E())
        {
        }
#endif

     return 0;
   }
