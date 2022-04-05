
#include "sage3basic.h"
#include "AST_FILE_IO.h"

#include "Rose/AST/checker.h"
#include "Rose/AST/defect.h"

namespace Rose { namespace AST { namespace Checker {

bool integrity_symbols(SgProject * project) {
  auto defect_cnt = Defect::all.size();
  // TODO
  return defect_cnt == Defect::all.size();
}

} } }

