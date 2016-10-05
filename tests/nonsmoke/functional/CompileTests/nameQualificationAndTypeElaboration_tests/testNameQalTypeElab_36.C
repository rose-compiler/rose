// number #36

// Example of unnamed namespace.
// Use of "_1" used to unparse to boost::lambda::::_1

namespace boost {
namespace lambda {

#if 0
namespace X {
  // These are constants types and need to be initialised
  typedef int Integer;
  int _1;
} // unnamed
#endif
   
namespace {
  // These are constants types and need to be initialised
  typedef int Integer;
  int _1;
} // unnamed
   
} // lambda
} // boost


int _1;

void foo()
   {
     _1 = 1;
//   boost::lambda::X::_1 = 1;
     boost::lambda::_1 = 1;

     int x;
//   boost::lambda::X::Integer y1;
     boost::lambda::Integer y2;
   }
