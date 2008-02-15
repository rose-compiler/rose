// This is the (wrongly) elaborated version of d0051.cc; that is this
// code is wrong.  G++ complains correctly, but elsa still doesn't
// find the error that is exhibited here.
struct A {};
struct B {
  A
  //ERROR(1): const
    x;
  B &operator =(B const &other) {
    x = other.x;
    return *this;
  }
};
