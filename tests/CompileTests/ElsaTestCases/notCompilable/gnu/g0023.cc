// g0023.cc
// g++-2 lets code refer to bad_alloc w/o std:: prefix

namespace std {
  class bad_alloc {};
}

int *foo()
{
  try {
    return new int;
  }
  catch (std::bad_alloc) {      // standard
    return 0;
  }
}

int *foo2()
{
  try {
    return new int;
  }
  catch (bad_alloc) {           // nonstandard
    return 0;
  }
}

int *foo3()
{
  try {
    return new int;
  }
  catch (class bad_alloc) {     // nonstandard, with elaborated type spec
    return 0;
  }
}
