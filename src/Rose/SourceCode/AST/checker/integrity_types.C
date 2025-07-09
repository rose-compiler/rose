
#include "sage3basic.h"
#include "AST_FILE_IO.h"

#include "Rose/SourceCode/AST/Checker.h"
#include "Rose/SourceCode/AST/Defect.h"

namespace Rose { namespace SourceCode { namespace AST { namespace Checker {

bool integrity_types(SgProject*) {
  auto defect_cnt = Defect::all.size();
  // TODO
  return defect_cnt == Defect::all.size();
}

} } } }

