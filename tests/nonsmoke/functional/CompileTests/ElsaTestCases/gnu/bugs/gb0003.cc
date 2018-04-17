// gb0003.cc
// applying '<' to a function

// See comments in cc_tcheck.cc (search for
// "cannot apply '<' to a function").  This is not valid C++.

int f();

int g()
{
  return f < 0;
}
