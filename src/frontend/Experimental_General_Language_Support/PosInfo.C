#include "sage3basic.h"
#include "PosInfo.h"

namespace Rose {
namespace builder {

PosInfo::PosInfo(SgLocatedNode* fromNode) {
  ASSERT_not_null(fromNode);
  ASSERT_not_null(fromNode->get_startOfConstruct());
  ASSERT_not_null(fromNode->get_endOfConstruct());

  startLine_ = fromNode->get_startOfConstruct()->get_line();
  startCol_  = fromNode->get_startOfConstruct()->get_col();
  endLine_   = fromNode->get_endOfConstruct()->get_line();
  endCol_    = fromNode->get_endOfConstruct()->get_col();
 }
 
} // namespace builder
} // namespace Rose
 
