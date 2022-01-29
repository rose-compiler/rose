#include "sage3basic.h"
#include "PosInfo.h"

namespace Rose {
namespace builder {

PosInfo::PosInfo(SgLocatedNode* fromNode) {
  ROSE_ASSERT(fromNode != NULL);
  ROSE_ASSERT(fromNode->get_startOfConstruct() != NULL);
  ROSE_ASSERT(fromNode->get_endOfConstruct()   != NULL);

  startLine_ = fromNode->get_startOfConstruct()->get_line();
  startCol_  = fromNode->get_startOfConstruct()->get_col();
  endLine_   = fromNode->get_endOfConstruct()->get_line();
  endCol_    = fromNode->get_endOfConstruct()->get_col();
 }
 
} // namespace builder
} // namespace Rose
 
