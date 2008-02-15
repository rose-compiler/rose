struct A {};

void f() {
 
  struct {
    struct A z;
  } *p;

  p->z;

  (int) & (((struct Foo {struct A y;}*)0)->y);

  (int) & (((struct {struct A y;}*)0)->y);
}
