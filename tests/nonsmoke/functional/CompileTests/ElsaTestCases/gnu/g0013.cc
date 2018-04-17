// g0013.cc
// compound lit with type defn inside array deref
// related to d0078.cc

void f()
{   
  int *a;
  a[
    (
      (struct Foo { int x; }){ 0 }      // compound lit: {x=0}
    ).x
   ];
}





