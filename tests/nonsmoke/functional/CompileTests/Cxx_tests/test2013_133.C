

class X {};

void foo ( X & a, const X & b );
void foo ( X & a, const X & b );

#if 0
void foo ( X & a,const X & b = X() );
#else
void foo ( X & a,
           const X & b = X() );
#endif

void foo ( X & a, const X & b );
void foo ( X & a, const X & b );

void foobar()
   {
     X x;
#if 1
     foo(x);
#else
     foo(x, X());
#endif
   }
