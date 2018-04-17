// t0473.cc
// variant of t0243.cc where bad_alloc is in std::

namespace std {
  class bad_alloc {};
}

int *foo()
{
  try {
    return new int;
  }
  catch (std::bad_alloc) {
    return 0;
  }
}
