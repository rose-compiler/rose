// Example of unnamed namespace.
// Use of "_1" used to unparse to boost::lambda::::_1

namespace boost {
namespace lambda {


namespace {
  // These are constants types and need to be initialised
// typedef int Integer;
  int _1;
} // unnamed
   
} // lambda
} // boost


int _1;

void foo()
   {
     _1 = 1;
     boost::lambda::_1 = 1;

  // int x;
  // boost::lambda::Integer y;
   }
