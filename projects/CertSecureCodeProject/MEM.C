/**
 * \file MEM.C
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

#include "rose.h"
#include "utilities.h"
#include <algorithm>


/**
 * Store a new value in pointers immediately after free()
 */
bool MEM01_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "free")))
		return false;

	// Figure out which variable is being freed
	const SgExpression *argExp = getFnArg(fnRef, 0);
	assert(argExp);
	const SgVarRefExp *argVar = isSgVarRefExp(argExp);
	if (!argVar)
		return false;
	const SgInitializedName *argVarName = getRefDecl(argVar);
	assert(argVarName);
	bool longlifetime = isGlobalVar(argVarName) || isStaticVar(argVarName);
	/* Block where the variable is defined */
	const SgBasicBlock* defBlock = findParentOfType(argVar->get_symbol()->get_declaration(),SgBasicBlock);

	// Pop up to the BasicBlock so that we can find the next line of code
	const SgStatement* nextStat = findInBlockByOffset(node,1);
	// block in which the free statement is enclosed
	const SgBasicBlock* block = findParentOfType(node,SgBasicBlock);
	assert(block);

	while (nextStat == NULL) {
		// If we're in a for-loop and imediately assign in the increment,
		// that is OK
		const SgForStatement *forLoop = isSgForStatement(block->get_parent());
		if (forLoop && isAssignToVar(forLoop->get_increment(), argVarName))
			return false;
		// If this block is the one in which the variable is defined, that is
		// OK
		if ((block == defBlock) && !longlifetime)
			return false;
		// Pop up to the next block
		nextStat = findInBlockByOffset(block,1);
		block = findParentOfType(block, SgBasicBlock);
		assert(block);
	}

	// Return Statements are also OK, but only for local vars
	if (isSgReturnStmt(nextStat) && (!longlifetime)) {
		return false;
	} else {
		// Assignments to the pointer are OK
		const SgExprStatement *nextExpr = isSgExprStatement(nextStat);
		if(nextExpr && isAssignToVar(nextExpr->get_expression(), argVarName))
			return false;
	}

	print_error(node, "MEM01-C", "Store a new value in pointers immediately after free()", true);

	return true;
}

/**
 * pointer to the allocated type
 *
 * \see EXP36-C which catches this
 */
bool MEM02_C( const SgNode *node ) {
	return false;
}

/**
 * Do not perform zero length allocations
 */
bool MEM04_C( const SgNode *node ) {
	const SgExpression *allocArg = removeImplicitPromotions(getAllocFunctionExpr(isSgFunctionRefExp(node)));
	if (!allocArg)
		return false;

	const SgVarRefExp *varRef = isSgVarRefExp(allocArg);
	if (!varRef)
		return false;
	const SgInitializedName *var = getRefDecl(varRef);
	const SgValueExp *val = isSgValueExp(allocArg);
	if (var) {
		const SgFunctionDefinition *fn = findParentOfType(node,SgFunctionDefinition);
		const SgVarRefExp *ref = NULL;
		/**
		 * First try, just look for a check against NULL
		 */
		FOREACH_SUBNODE(fn, nodes1, i, V_SgVarRefExp) {
			ref = isSgVarRefExp(*i);
			assert(ref);
			if ((var == getRefDecl(ref)) && isTestForNullOp(ref)) {
				return false;
			}
		}
		/**
		 * If there isn't one, maybe the progammer has assigned this var to
		 * something sane and doesn't need to check
		 */
		FOREACH_SUBNODE(fn, nodes2, i2, V_SgAssignOp) {
			const SgAssignOp *op = isSgAssignOp(*i2);
			assert(op);
			ref = isSgVarRefExp(op->get_lhs_operand());
			if (!ref || (getRefDecl(ref) != var))
				continue;
			if (!isZeroVal(removeCasts(op->get_rhs_operand())))
				return false;
		}
		const SgAssignInitializer *init = isSgAssignInitializer(var->get_initptr());
		if(init && !isZeroVal(removeCasts(init->get_operand()))) {
			return false;
		}

	} else if (val && !isZeroVal(val)) {
		/** compile time constant that is 0 */
		return false;
	} else {
		/** sizeof or something else we can't handle */
		return false;
	}

	print_error(node,"MEM04-C", "Do not perform zero length allocations", true);
	return true;
}

/**
 * Ensure that the arguments to calloc() when multiplied can be represented as
 * a size_t
 */
bool MEM07_C( const SgNode *node ) {
	const SgFunctionRefExp* fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "calloc")))
		return false;

	const SgExpression* nmembExp = getFnArg(fnRef, 0);
	const SgExpression* sizeExp = getFnArg(fnRef, 1);
	assert(nmembExp && sizeExp);

	const SgVarRefExp* nmembRef = isSgVarRefExp(nmembExp);
	size_t nmembVal;
	if (!nmembRef && !getSizetVal(nmembExp, &nmembVal))
		return false;
	const SgVarRefExp* sizeRef = isSgVarRefExp(sizeExp);
	size_t sizeVal;
	if (!sizeRef && !getSizetVal(sizeExp, &sizeVal))
		return false;

	if (nmembRef || sizeRef) {
		/* Search for the previous line, see if it is a check for overflow */
		/**
		 * \todo We need to find a more rigorous way to find checks for
		 * overflow
		 */
		const SgStatement *prevStat = findInBlockByOffset(node, -1);
		if (prevStat) {
			FOREACH_SUBNODE(prevStat, nodes, i, V_SgBinaryOp) {
				const SgBinaryOp *binOp = isSgBinaryOp(*i);
				assert(binOp);
				if (!isAnyComparisonOp(binOp))
					continue;
				const SgVarRefExp *lhs = isSgVarRefExp(binOp->get_lhs_operand());
				const SgVarRefExp *rhs = isSgVarRefExp(binOp->get_rhs_operand());
				if((lhs && nmembRef && (getRefDecl(lhs)==getRefDecl(nmembRef)))
				|| (lhs && sizeRef  && (getRefDecl(lhs)==getRefDecl(sizeRef)))
				|| (rhs && nmembRef && (getRefDecl(rhs)==getRefDecl(nmembRef)))
				|| (rhs && sizeRef  && (getRefDecl(rhs)==getRefDecl(sizeRef))))
					return false;
			}
		}
	} else {
		if (nmembVal <= (std::numeric_limits<size_t>::max() / sizeVal))
			return false;
	}

	print_error(node, "MEM07-C", "Ensure that the arguments to calloc() when multiplied can be represented as a size_t", true);
	return true;
}

/**
 * Use realloc() only to resize dynamically allocated arrays
 */
bool MEM08_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "realloc")))
		return false;

	const SgExpression *arg = removeImplicitPromotions(getFnArg(fnRef, 0));
	assert(arg);

	const SgCastExp* cast = isSgCastExp(node->get_parent()->get_parent());
	if (!cast)
		return false;

	if (cast->get_type() != arg->get_type()) {
		print_error(node, "MEM08-C", "Use realloc() only to resize dynamically allocated arrays", true);
		return true;
	}

	return false;
}

/**
 * Ensure that freed pointers are not reused
 *
 * \bug Need to check for conditional return statements
 */
bool MEM30_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "free")))
		return false;

	// Get variable as first arg
	const SgVarRefExp* ref = isSgVarRefExp( getFnArg( isSgFunctionRefExp( node), 0));
	if (ref == NULL) return false;
	const SgInitializedName* var = getRefDecl( ref);
	assert(var != NULL);

	if (NextValueReferred().next_value_referred( ref)) {
		print_error( node, "MEM30-C", "Do not access freed memory");
		return true;
	}

	return false;
}

/**
 * Free dynamically allocated memory exactly once
 *
 * If the variable is passed by reference to a function which allocates memory this could throw a false positive.
 * If the variable is freed multiple times by a single loop this could throw a false negative.
 */
bool MEM31_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "free")))
		return false;

	const SgVarRefExp* ref = isSgVarRefExp( getFnArg( isSgFunctionRefExp( node), 0));

	if (ref == NULL) return false;
	const SgInitializedName* ref_var = getRefDecl( ref);

	bool before = true;
	FOREACH_SUBNODE(findParentOfType(node, SgFunctionDefinition), nodes, i, V_SgNode) {
		if (before) {
			if (*i == node)
				before = false;
			continue;
		}

		/**
		 * Checking for return statements lowers the false positive rate by
		 * allowing conditional frees followed by returns
		 */
		if(isSgReturnStmt(*i))
			return false;

		const SgFunctionRefExp *iFn = isSgFunctionRefExp(*i);
		if(iFn && isCallOfFunctionNamed(iFn, "free")) {
			const SgVarRefExp* ref2 = isSgVarRefExp(getFnArg(iFn, 0));
			const SgInitializedName* ref2_var = getRefDecl(ref2);

			if (ref_var == ref2_var) {
				print_error(node, "MEM31-C", "Free dynamically allocated memory exactly once.");
				return true;
			}
		} else if(isSgAssignOp(*i)) {
			const SgVarRefExp *ref2 = isSgVarRefExp(isSgAssignOp(*i)->get_lhs_operand());
			const SgInitializedName* ref2_var = getRefDecl( ref2);

			if (ref_var == ref2_var)
				return false;
		}
	}

	return false;
}

/**
 * Use the correct syntax for flexible array members
 */
bool MEM33_C( const SgNode *node ) {
	const SgClassDefinition* def = isSgClassDefinition(node);
	if (!def)
		return false;

	const SgVariableDeclaration* varDecl = isSgVariableDeclaration(def->get_members().back());
	/**
	 * Disabling assertion due to C++ code
	 */
	if (!varDecl)
		return false;
//	assert(varDecl);

	if (varDecl->get_variables().size() != 1)
		return false;

	const SgInitializedName *varName = varDecl->get_variables().front();
	assert(varName);

	const SgArrayType *arrT = isSgArrayType(varName->get_type());
	if (!arrT)
		return false;

	const SgValueExp* arrSize = isSgValueExp(arrT->get_index());
	if (!arrSize)
		return false;

	if (isVal(arrSize,0) || isVal(arrSize,1)) {
		print_error(varDecl, "MEM33-C", "Use the correct syntax for flexible array members");
		return true;
	}
	return false;
}

/**
 * Only free memory allocated dynamically
 */
bool MEM34_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!fnRef)
		return false;
	if (!(isCallOfFunctionNamed(fnRef, "free")
		||isCallOfFunctionNamed(fnRef, "realloc")))
		return false;

	const SgVarRefExp *varRef = isSgVarRefExp(removeImplicitPromotions(getFnArg(fnRef,0)));
	if (!varRef)
		return false;
	const SgInitializedName *var = getRefDecl(varRef);
	assert(var);
	/**
	 * It's much too hard to analyze these kinds of variables
	 */
	if (isGlobalVar(var) || isStaticVar(var))
		return false;

	/**
	 * Ignore arguments to a function
	 */
	const SgFunctionDefinition *parent = findParentOfType(node, SgFunctionDefinition);
	FOREACH_INITNAME(parent->get_declaration()->get_args(), p) {
		if(var == *p)
			return false;
	}

	assert(parent);
	Rose_STL_Container<SgNode *> nodes = NodeQuery::querySubTree( const_cast<SgFunctionDefinition*>(parent), V_SgNode );
	Rose_STL_Container<SgNode *>::iterator i = nodes.begin();
	while(fnRef != isSgFunctionRefExp(*i)) {
		assert(i != nodes.end());
		i++;
	}
	const SgNode *block = NULL;
	do {
		if (block) {
			if (block == *i)
				block = NULL;
			continue;
		} else if (isSgReturnStmt(*i)) {
			block = findParentOfType(*i, SgBasicBlock);
		}
		const SgFunctionRefExp *iFn = isSgFunctionRefExp(*i);
		const SgVarRefExp *iVar = isSgVarRefExp(*i);
		if (iFn) {
			if (!(isCallOfFunctionNamed(iFn, "malloc")
				||isCallOfFunctionNamed(iFn, "calloc")
				||isCallOfFunctionNamed(iFn, "realpath")
				||isCallOfFunctionNamed(iFn, "strdup")
				||isCallOfFunctionNamed(iFn, "realloc")))
				continue;
			if (var == getVarAssignedTo(iFn, NULL))
				return false;
		} else if (iVar && (getRefDecl(iVar) == var)) {
			if (varWrittenTo(iVar))
				break;
		}
	} while ((i--) != nodes.begin());

	print_error(node, "MEM34-C", "Only free memory allocated dynamically");
	return true;
}

bool MEM(const SgNode *node) {
  bool violation = false;
  violation |= MEM01_C(node);
  violation |= MEM02_C(node);
  violation |= MEM04_C(node);
  violation |= MEM07_C(node);
  violation |= MEM08_C(node);
  violation |= MEM30_C(node);
  violation |= MEM31_C(node);
  violation |= MEM33_C(node);
  violation |= MEM34_C(node);
  return violation;
}
