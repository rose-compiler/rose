// Liao, 5/21/2009
// A test case extracted from spec cup2006's 483.xalancbmk 
// see bug 350: #include... within a namespace dec is attached to after the namespace
namespace mynamespace{
  class XMLPlatformUtils
  {
  };
}

namespace mynamespace
{
  void foo2()
  {}

// some comments here
}

namespace mynamespace
{
  void foo()
  {}

#include <LogicalPath.cpp>
}


