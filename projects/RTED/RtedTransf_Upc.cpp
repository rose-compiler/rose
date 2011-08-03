

#include "RtedTransformation.h"

namespace SI = SageInterface;
namespace SB = SageBuilder;


void RtedTransformation::transformUpcBlockingOps(SgStatement* const stmt)
{
  // before:
  //         some_upc_blocking_operation();
  //
  // after:
  //         rted_UpcExitWorkzone();  // releases heap lock (reader)
  //         some_upc_blocking_operation();
  //         rted_UpcEnterWorkzone(); // acquires heap lock (reader)

  ROSE_ASSERT(withupc);

  static const std::string exitwz_msg("RS: UpcExitWorkzone()");
  static const std::string enterwz_msg("RS: UpcEnterWorkzone()");

  ROSE_ASSERT( stmt && symbols.roseUpcEnterWorkzone && symbols.roseUpcExitWorkzone );

  requiresParentIsBasicBlock(*stmt);

  insertCheck(ilBefore, stmt, symbols.roseUpcExitWorkzone,  SB::buildExprListExp(), exitwz_msg);
  insertCheck(ilAfter,  stmt, symbols.roseUpcEnterWorkzone, SB::buildExprListExp(), enterwz_msg);
}

void RtedTransformation::transformPtrDerefs(SharedPtrDerefContainer::value_type ptrderef)
{
    ROSE_ASSERT(ptrderef);

    SgExpression* operand = ptrderef->get_operand();
    ROSE_ASSERT ( isUpcSharedPointer(operand->get_type()) );

    SgStatement*   stmt = getSurroundingStatement(ptrderef);
    SgExprListExp* lock_args   = SB::buildExprListExp();
    SgExprListExp* unlock_args = SB::buildExprListExp();

    SI::appendExpression(lock_args,   SI::deepCopy(operand));
    SI::appendExpression(unlock_args, SI::deepCopy(operand));

    insertCheck(ilBefore, stmt, symbols.roseUpcEnterSharedPtr, lock_args);
    insertCheck(ilAfter,  stmt, symbols.roseUpcExitSharedPtr,  unlock_args);
}



#if NOT_YET_IMPLEMENTED

static const bool keep_old_tree = false;
static const bool delete_old_tree = true;

/// \brief   replace old with repl, and delete subtree rooted in old.
/// \return  old's operand
static
SgExpression& extractOperand(SgUnaryOp& old)
{
  SgExpression* op = old.get_operand();
  SgExpression* emptydummy = SB::buildIntVal(0);

  SI::replaceExpression(op, emptydummy, keep_old_tree);

  op->set_parent(NULL);
  return *op;
}

// this code sketches the correct implementation (enclosing all unsafe operations
//   in safe wrappers)

void RtedTransformation::transformPtrDerefs(PtrDerefContainer::value_type data)
{
  const SgSourceFile& srcfile = *data.first;
  SgPointerDerefExp*  deref = data.second;

  // lookup generated function
  SgExpression&       operand = extractOperand(*deref);
  SgPointerType*      ptrtype = isSgPointerType(operand.get_type());
  ROSE_ASSERT(ptrtype);
/*
  SgSymbol&           sym = lookup_safeRead(srcfile, *ptrtype);
  SgExprListExp&      args = *SB::buildExprListExp();
  SgFunctionCallExp&  call = *SB::buildFunctionCallExp(&sym, &args);

  SI::replaceExpression(deref, call, delete_old_tree);
  SI::appendExpression(args, operand);
*/
}


SgSymbol& RtedTransformation::lookup_safeRead(const SgSourceFile& srcfile, const SgPointerType& type)
{
  f = srcfilefuns.find(srcfile);
  ROSE_ASSERT(f != srcfilefuns.end());

  p = f.find(type);
  ROSE_ASSERT(p != f.end());

  return *p;
}

void RtedTransformation::make_safeRead(const SgSourceFile& srcfile, const SgPointerType& type)
{
  f = srcfilefuns[srcfile];
  p = f.find[type];

  if (p == NULL)
  {

  }

  return *p;
}
#endif /* NOT_YET_IMPLEMENTED */
