// The source position information of the end of the last SgNamespaceDefinition is wrong.
// it is the same as the starting position (and the starting position is correct).
// This translates to a bug in how the comment will be located relative to the end of the scope.

namespace mynamespace
{
  int y;
}
namespace mynamespace
{
  int x;
  //#include <LogicalPath.cpp>
}
