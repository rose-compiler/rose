/**
 * \file MSC.C
 *
 * \note As written, these tests catch template declarations only if
 * instantiated.
 *
 * Copyright (c) 2007 Carnegie Mellon University.
 * All rights reserved.

 * Permission to use this software and its documentation for any purpose is
 * hereby granted, provided that the above copyright notice appear and that
 * both that copyright notice and this permission notice appear in supporting
 * documentation, and that the name of CMU not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.
 *
 * CMU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WSTRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL CMU BE
 * LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, RISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

// DQ (8/25/2009): To use pre-compiled headers "rose.h" must appears first.
#include "rose.h"

// DQ (8/25/2009): these are included by "rose.h"
// #include <list>
// #include <string>
// #include <vector>
// #include <algorithm>

#include "utilities.h"

/**
 * Strive for logical completeness
 */
bool MSC01_C( const SgNode *node ) {
	const SgSwitchStatement *swch = isSgSwitchStatement(node);
	if (!swch)
		return false;

#if 1
// DQ (8/16/2009): This needs to be fixed, the body is no longer a SgBasicBlock (bug fixed in ROSE).
//	const SgStatementPtrList &stats = swch->get_body()->get_statements();
   printf ("This needs to be fixed, the body is no longer a SgBasicBlock (bug fixed in ROSE) ...\n");
   ROSE_ASSERT(false);

   const SgStatementPtrList stats;
#else
	const SgStatementPtrList &stats = swch->get_body()->get_statements();
#endif

	if ((stats.size() > 0)
	&& isSgDefaultOptionStmt(stats.back()))
		return false;

	print_error(node, "MSC01-C", "Strive for logical completeness", true);
	return true;
}

/**
 * Avoid errors of addition 
 *
 * \note Because of a problem with the expansion of isnan/isless/etc, this
 * rule is disabled, ROSE catches most of this on it's own, so this should not
 * be a problem
 */
bool MSC03_C( const SgNode *node ) {
	const SgExprStatement *exprStmt = isSgExprStatement(node);
	if (!exprStmt) {
		/**
		 * Check for empty bodies
		 */
		const SgBasicBlock *block = NULL;
		if (isSgIfStmt(node))
			block = isSgBasicBlock(isSgIfStmt(node)->get_true_body());
		else if (isSgForStatement(node))
			block = isSgBasicBlock(isSgForStatement(node)->get_loop_body());
		else if (isSgWhileStmt(node))
			block = isSgBasicBlock(isSgWhileStmt(node)->get_body());
		else
			return false;

		if (!block
		|| !isCompilerGeneratedNode(block))
			return false;
	} else {
		/**
		 * Check for statements with no effect
		 */
		const SgBasicBlock *parent = isSgBasicBlock(exprStmt->get_parent());
		if (!parent)
			return false;

		/**
		 * Ignore the last statement in a block because it could be an implicit
		 * return value, this is GNU extension
		 */
		if ((exprStmt == isSgExprStatement(parent->get_statements().back()))
		&&  (!isSgFunctionDefinition(parent->get_parent())))
			return false;

		const SgExpression *expr = exprStmt->get_expression();
		assert(expr);

		if (isCompilerGeneratedNode(expr))
			return false;

		if(isSgFunctionCallExp(expr)
		|| isSgAssignOp(expr)
		|| isSgConditionalExp(expr)
		|| isAnyAssignOp(expr)
		|| isSgPointerDerefExp(expr)
		|| isSgPlusPlusOp(expr)
		|| isSgMinusMinusOp(expr)
		|| isSgDeleteExp(expr))
			return false;
	}

	print_error(node, "MSC03-C", "Avoid errors of addition", true);
	return true;
}

/**
 * Do not manipulate time_t typed values directly 
 */
bool MSC05_C( const SgNode *node ) {
	if (isAnyBinArithOp(node)
	  ||isAnyBinArithAssignOp(node)
	  ||isAnyBinBitOp(node)
	  ||isAnyBinBitAssignOp(node)
	  ||isAnyRelationalOp(node)) {
		const SgBinaryOp *binOp = isSgBinaryOp(node);
		assert(binOp);
		const SgType *lhsT = binOp->get_lhs_operand()->get_type();
		const SgType *rhsT = binOp->get_rhs_operand()->get_type();
		if (!(isTypeTimeT(lhsT) || isTypeTimeT(rhsT)))
			return false;
	} else if(isSgBitComplementOp(node)
	  ||isSgMinusMinusOp(node)
	  ||isSgNotOp(node)
	  ||isSgPlusPlusOp(node)
	  ||isSgUnaryAddOp(node)
	  ||isSgMinusOp(node)) {
		const SgUnaryOp *op = isSgUnaryOp(node);
		assert(op);
		const SgType *opT = op->get_operand()->get_type();
		if (!isTypeTimeT(opT))
			return false;
	} else {
		return false;
	}

	print_error(node, "MSC05-C", "Do not manipulate time_t typed values directly", true);
	return true;
}

/**
 * Detect and remove unused values
 *
 * \bug Disabled until a better algorithm can be found
 */
bool MSC13_C( const SgNode *node ) {
	const SgInitializedName *var = isSgInitializedName(node);
	if (!var)
		return false;
	bool unused = false;
	bool violation = false;
	if (var->get_initializer())
		unused = true;
	const SgNode * prev = var;
	FOREACH_SUBNODE(var->get_scope(), nodes, i, V_SgVarRefExp) {
		const SgVarRefExp *iVar = isSgVarRefExp(*i);
		if (!iVar || (getRefDecl(iVar) != var))
			continue;
		if (varWrittenTo(iVar) && (!findParentOfType(iVar, SgPointerDerefExp))) {
			if (unused) {
				print_error(prev, "MSC13-C", "Detect and remove unused values", true);
				violation = true;
			} else {
				unused = true;
				prev = iVar;
			}
		} else {
			unused = false;
		}
	}
	return violation;
}

/**
 * Do not use rand()
 */
bool MSC30_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "rand")))
		return false;
	print_error( node, "MSC30-C", "Do not use rand()");
	return true;
}

/**
 * Ensure that return values are compared against the proper type
 */
bool MSC31_C( const SgNode *node ) {
	const SgBinaryOp *op = isSgBinaryOp(node);
	if (!op || isCompilerGeneratedNode(node))
		return false;
	if (!(isSgEqualityOp(op) || isSgNotEqualOp(op)))
		return false;
	const SgExpression *lhs = op->get_lhs_operand();
	const SgValueExp *val = NULL;
	intmax_t n;
	while((val = isSgValueExp(lhs)) != NULL) {
		if (!getIntegerVal(val, &n) || (n >= 0))
			return false;
		if (val->get_originalExpressionTree())
			lhs = removeImplicitPromotions(val->get_originalExpressionTree());
		else
			break;
	}
	const SgExpression *rhs = op->get_rhs_operand();
	while((val = isSgValueExp(rhs)) != NULL) {
		if (!getIntegerVal(val, &n) || (n >= 0))
			return false;
		if (val->get_originalExpressionTree())
			rhs = removeImplicitPromotions(val->get_originalExpressionTree());
		else
			break;
	}
	assert(lhs && rhs);
	const SgType *lhsType = stripModifiers(lhs->get_type());
	const SgType *rhsType = stripModifiers(rhs->get_type());
	assert(lhsType && rhsType);
	/**
	 * \todo We should not be using unparseToString, there should be a better
	 * way to get the name of a typedef type
	 */
	std::string lhsName = lhsType->unparseToString();
	std::string rhsName = rhsType->unparseToString();
	if ((lhsName == "time_t" || lhsName == "size_t")
	 && (lhsName != rhsName)) {
		print_error(node, "MSC31-C", "Ensure that return values are compared against the proper type");
		return true;
	}
	return false;
}

bool MSC(const SgNode *node) {
  bool violation = false;

#if 0
// DQ (8/16/2009): This function has a non-portable requirement of ROSE.
  violation |= MSC01_C(node);
#endif

  violation |= MSC03_C(node);
  violation |= MSC05_C(node);
//  violation |= MSC13_C(node);
  violation |= MSC30_C(node);
  violation |= MSC31_C(node);
  return violation;
}
