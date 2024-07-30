
#include "sage3basic.h"
#include "AST_FILE_IO.h"

#include "Rose/AST/Checker.h"
#include "Rose/AST/Defect.h"

namespace Rose { namespace AST { namespace Defects {

std::set<defect_t<Kind::any> *> defect_t<Kind::any>::all;

defect_t<Kind::any>::defect_t(Kind kind_) : kind(kind_) {}
defect_t<Kind::any>::~defect_t() {}

void defect_t<Kind::any>::clear() {
  for (auto d: all) delete d;
  all.clear();
}

void defect_t<Kind::any>::display(std::ostream &) {
  // TODO
}

} } }

namespace Rose { namespace AST { namespace Checker {

bool all(SgProject* project) {
  return integrity(project) && consistency(project);
}

bool integrity(SgProject* project) {
  bool res = integrity_edges(project);
  if (!res) return false;
  res &= integrity_declarations(project);
  res &= integrity_symbols(project);
  res &= integrity_types(project);
  return res;
}

bool consistency(SgProject*) {
  return true; // TODO
}

} } }

