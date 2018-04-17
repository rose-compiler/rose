// t0228b.cc

// dsw: this is the part of t0228.cc that should not be an error in
// GNU mode

void f()
{
  // this is not a legal type
  //ERROR(2): char arr3[];
}
