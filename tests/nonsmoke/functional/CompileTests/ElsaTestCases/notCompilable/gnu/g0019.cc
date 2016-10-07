// g0019.cc
// min and max operators

void foo()
{
  int i,j,k;

  i = j <? k;
  i = j >? k;

  double a,b,c;

  a = b <? c;
  a = b >? c;
}


// little trick to get compiler to tell me the 
// value of a compile-time constant
template <int x>
class A {};

// tell me the value in an error report
void tellme(int dummy);
void tellme(int *dummy);

// require the value to be 0
void zero(int dummy);
void zero(A<0> a);

// require it to be 1
void one(int dummy);
void one(A<1> a);

// 5
void five(int dummy);
void five(A<5> a);



void bar()
{
  // what is the precedence?

  // lower than '*'
  //   if '*' is higher, this is 1     (yes)
  //   if '<?' is higher, this is 5
  A<( 5 * 1 <? 1 )> a2;
  one(a2);

  // lower than '+'
  //   if '+' is higher, this is 1     (yes)
  //   if '<?' is higher, this is 6
  A<( 5 + 1 <? 1 )> a1;
  one(a1);

  // lower than '<<'
  //   if '<<' is higher, this is 1    (yes)
  //   if '<?' is higher, this is 8
  A<( 4 << 1 <? 1 )> a5;
  one(a5);

  
  // it seems that '<?' and '>?' have equal precedence, with
  // left associativity

  // (      )                 5        (yes)
  A<( 4 <? 1 >? 5 )> a6;
  //      (      )            4
  five(a6);

  //      (      )            4
  A<( 4 >? 7 <? 1 )> a7;
  // (      )                 1        (yes)
  one(a7);


  // gcc-3: same precedence as '<' and '>', left associating with them
  // gcc-2: between "</>" and "<</>>"
  // Elsa implements the gcc-3 rules ...

  // (      )                 0        (yes)
  A<( -1 < 1 <? 0 )> a8;
  //      (      )            1
  zero(a8);

  // (       )                1        (yes)
  A<( -1 <? 1 < 0 )> a4;
  //       (     )           -1        (actually, gcc-2 is here...)
  one(a4);


  // higher than '||'
  //   if '||' is higher, this is 0
  //   if '<?' is higher, this is 1    (yes)
  A<( 5 || 0 <? 0 )> a3;
  one(a3);
  
  
  // wrong types
  //ERROR(1): 3 <? a3;
  //ERROR(2): a3 >? 3;
}
