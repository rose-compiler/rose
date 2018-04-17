// t0308.cc
// 5.16 para 2

struct A {};

void g();
void h();
void j(A &a);
void jc(A const &a);

void f(int x, int y)
{
  int *ptr;
  int arr[3];
  A a;
  void (*funcptr)();

  switch (x) {
    case 1:
      // both void
      return y? g() : h();

    case 11:
      // error: void vs. non-void
      //ERROR(1): return y? g() : 3;
      //ERROR(2): return y? 3 : g();

    case 2:
      // second is throw
      x = (y? throw 2 : 3);
      break;

    case 3:
      // third is throw
      x = (y? 2 : throw 3);
      break;

    case 4:
      // both throw
      return (y? throw 2 : throw 3);

    case 5:
      // lvalue-to-rvalue
      jc(y? throw 2 : a);
      //ERROR(3): j(y? throw 2 : a);    // rvalue after conversion
      break;

    case 6:
      // array-to-pointer (actually hard to tell, b/c the conversion
      // could happen in the assignment...)
      ptr = (y? throw 2 : arr);
      break;

    case 7:
      // function-to-pointer (again, could occur in assignment)
      funcptr = (y? throw 2 : h);
      break;
  }
}



