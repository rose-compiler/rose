// t0344.cc
// pass ptr-to-member where member is inherited


struct Base {
  void func();
  int x;
};
struct A : public Base {
};

void forall( void (A::*func)() );
//void forall( int ) ;

void another( int A::*p );


void foo()
{
  // the conversions up here require special treatment because the
  // Elsa type system puts the receiver parameter into the param list

  forall(&A::func);

  void (Base::*bptr)();
  forall(bptr);


  // the conversions below here only require that ptr-to-base-member
  // be convertible to ptr-to-derived-member (no param lists involved)

  another( &A::x );
  
  int Base::*bptr2;
  another(bptr2);
}
