// t0454.cc
// variety of template argument syntaxes

template <int n>
struct A {};
  
int f();
  
struct B {
  int b;
};

void foo()
{
  B b;
  int x;
  int *p;

  //ERROR(1):   A< f() > a1;        // error: not const
  //ERROR(2):   A< b.b > a2;        // error: not const
                A< -1 > a3;
  //ERROR(3):   A< x++ > a4;        // error: not const (side effect)
                A< 1+2 > a5;
  //ERROR(4):   A< 1>2 > a6;        // error: unparenthesized '>'
  //ERROR(5):   A< &x > a7;         // error: type mismatch
  //ERROR(6):   A< *p > a8;         // error: not const
                A< 1?2:3 > a9;
  //ERROR(7):   A< x=3 > a10;       // error: not const (side effect)
  //ERROR(8):   A< delete p > a11;  // error: not const (side effect)
  //ERROR(9):   A< throw x > a12;   // error: not const (side effect)
}


// EOF
