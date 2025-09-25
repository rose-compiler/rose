
#include <sage3basic.h>
#include <Rose/Source/Analysis/Visitor.h>

namespace Rose {
namespace Source {
namespace Analysis {

Visitor::Visitor(
  std::string const & name,
  std::string const & version
) :
  Analyzer(name, version),
  ROSE_VisitTraversal()
{}

int Visitor::apply() {
  traverseMemoryPool();
  return 0;
}

} } }
