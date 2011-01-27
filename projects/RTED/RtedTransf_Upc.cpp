

#include "RtedTransformation.h"

namespace SI = SageInterface;
namespace SB = SageBuilder;


/// \brief  converts the parent to a basic block (unless it already is one)
/// \return a SgBasicBlock object
static
SgBasicBlock& convertParentToBasicBlock(SgStatement& stmt)
{
  SgLocatedNode* block = SI::ensureBasicBlockAsParent(&stmt);
  SgBasicBlock*  res = isSgBasicBlock(block);

  ROSE_ASSERT(res);
  return *res;
}

void RtedTransformation::transformUpcBarriers(SgUpcBarrierStatement* const stmt)
{
  static const std::string prmsg("RS: processMsg()");

  ROSE_ASSERT( stmt );

  convertParentToBasicBlock(*stmt);

  // create the call nodes
  ROSE_ASSERT(symbols.roseProcessMsg);

  SgExprListExp&         arglist = *SB::buildExprListExp();
  SgExprStatement&       processMsgStmt = *insertCheck(ilBefore, stmt, symbols.roseProcessMsg, &arglist, prmsg);

  SgUpcBarrierStatement& xtraBarrier = *buildUpcBarrierStatement();
  /* SgExprStatement&    xtraBarrierStmt = */
  SI::insertStatementBefore(&processMsgStmt, &xtraBarrier);
}
