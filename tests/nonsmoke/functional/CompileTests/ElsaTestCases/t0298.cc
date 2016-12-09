// t0298.cc
// pointers to members

// variation of 8.3.3.2.cc

class X {
public:
  void f(int);
  int a;
};
class Y;

int X::* pmi = &X::a;
void (X::* pmf)(int) = &X::f;
double X::* pmd;
char Y::* pmc;

void foo()
{
  X obj;

  obj.*pmi = 7;        // assign 7 to an integer
                       // member of obj
  (obj.*pmf)(7);       // call a function member of obj
                       // with the argument 7

  X *ptr;
  (ptr->*pmf)(7);      // call a function member of *ptr
                       // with the argument 7
                       
  //ERROR(1): (obj->*pmf)(7);      // wrong, LHS of ->* must be pointer
}
