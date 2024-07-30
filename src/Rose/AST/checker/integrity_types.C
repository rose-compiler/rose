
#include "sage3basic.h"
#include "AST_FILE_IO.h"

#include "Rose/AST/Checker.h"
#include "Rose/AST/Defect.h"

namespace Rose { namespace AST { namespace Checker {

bool integrity_types(SgProject*) {
  auto defect_cnt = Defect::all.size();
  // TODO
  return defect_cnt == Defect::all.size();
}

} } }

