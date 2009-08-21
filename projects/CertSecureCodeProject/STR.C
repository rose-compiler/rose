/**
 * \file STR.C
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

/**
 * Use plain char for characters in the basic character set 
 */
bool STR04_C( const SgNode *node ) {
	const SgInitializedName *var = isSgInitializedName(node);
	if (!var)
		return false;
	const SgAssignInitializer *init = isSgAssignInitializer(var->get_initializer());
	if (!init)
		return false;
	if (!isSgTypeString(init->get_type()))
		return false;
	if (isSgTypeChar(var->get_type()->findBaseType())
	||  isSgTypeWchar(var->get_type()->findBaseType()))
		return false;

	/**
	 * \bug Rose is missing const dereference
	 */
	if (isTypeWcharT(const_cast<SgType *>(var->get_type())->dereference()))
		return false;

	print_error(node, "STR04-C", "Use plain char for characters in the basic character set", true);
	return true;
}

/**
 * Use pointers to const when referring to string literals
 */
bool STR05_C( const SgNode *node ) {
	const SgInitializedName *var = isSgInitializedName(node);
	if(!var)
		return false;
	const SgAssignInitializer *init = isSgAssignInitializer(var->get_initializer());
	if (!init)
		return false;
	if (!isSgTypeString(init->get_type()))
		return false;
	if (!isSgPointerType(var->get_type()))
		return false;

	print_error(node, "STR05-C", "Use pointers to const when referring to string literals", true);
	return true;
}

/**
 * Do not assume that strtok() leaves the parse string unchanged 
 */
bool STR06_C( const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if(!(fnRef && isCallOfFunctionNamed(fnRef, "strtok")))
		return false;

	const SgVarRefExp* str = isSgVarRefExp(removeImplicitPromotions(getFnArg(fnRef, 0)));

	/* Probably a call to strtok(0,...) */
	if (!str)
		return false;

	bool before = true;

	FOREACH_SUBNODE(str->get_symbol()->get_scope(), nodes, i, V_SgVarRefExp) {
		const SgVarRefExp *iVar = isSgVarRefExp(*i);
		assert(iVar);

		/* Ignore nodes before fnRef */
		if (before) {
			if (iVar == str)
				before = false;
			continue;
		}

		/* Ignore other variables */
		if (getRefDecl(iVar) != getRefDecl(str))
			continue;

		/* If we wrote to the string, we're done */
		if (varWrittenTo(iVar))
			return false;

		const SgFunctionCallExp* iFn = findParentOfType(iVar, SgFunctionCallExp);
		if (iFn) {
			const SgFunctionRefExp *iRef = isSgFunctionRefExp(iFn->get_function());
			assert(iRef);
			if (isCallOfFunctionNamed(iRef, "free"))
				return false;
			if (isCallOfFunctionNamed(iRef, "memcpy")
			||  isCallOfFunctionNamed(iRef, "strcpy")
			||  isCallOfFunctionNamed(iRef, "strncpy")) {
				const SgVarRefExp* arg = isSgVarRefExp(getFnArg(iRef, 0));
				assert(arg);
				if (getRefDecl(arg) == getRefDecl(str))
					return false;
			}
		}

		print_error(iVar, "STR06-C", "Do not assume that strtok() leaves the parse string unchanged", true);
		return true;
	}

	return false;
}

/**
 * Do not attempt to modify string literals
 */
bool STR30_C(const SgNode *node ) {
	const SgInitializedName *varName = isSgInitializedName(node);
	if (!varName)
		return false;

	/* Find out if varName is initialized with a string literal */
	const SgAssignInitializer *init = isSgAssignInitializer(varName->get_initializer());
	if (!init)
		return false;

	if (!isSgTypeString(init->get_type()))
		return false;

	FOREACH_SUBNODE(varName->get_scope(), nodes, i, V_SgVarRefExp) {
		const SgVarRefExp *iVar = isSgVarRefExp(*i);
		assert(iVar);

		/* For some reason the compiler generates spurious accesses ... */
		if (isCompilerGeneratedNode(iVar))
			continue;

		/* Make sure we are dealing with the correct variable here */
		if (getRefDecl(iVar) != varName)
			continue;

		/* if the variable got written to, it probably no longer points to the
		 * string */
		if (varWrittenTo(iVar))
			return false;

		/* is it getting derefenced? */
		const SgNode *parent = iVar->get_parent();
		if (!(isSgPntrArrRefExp(parent)
		    ||isSgPointerDerefExp(parent))) {
			continue;
		}

		/* And is the derefence getting written to? */
		if (varWrittenTo(parent)) {
			print_error(iVar, "STR30-C", "Do not attempt to modify string literals");
			return true;
		}
	}
	return false;
}

/**
 * Ensure that string storage is sufficient for chars & terminator
 *
 * This just ensures that strcpy is copying into a pointer
 */
bool STR31_C(const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "strcpy")))
		return false;

	const SgVarRefExp* ref = isSgVarRefExp( getFnArg( isSgFunctionRefExp(node), 0));
 	// strcpy() not copying into simple var
	if (ref == NULL)
		return false;
	if (!isSgArrayType(getRefDecl(ref)->get_type()))
		return false;
	if (isSgArrayType(getFnArg(isSgFunctionRefExp(node), 1)->get_type()))
		return false;

	print_error( node, "STR31-C", "String copy destination must contain sufficient storage");
	return true;
}

/**
 * Null-terminate byte strings as required
 *
 * \note This only works on character arrays that don't get initialized
 * It also only examines strncpy; it doesn't handle realloc or
 * other string-copying functions.
 */
bool STR32_C(const SgNode *node ) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!(fnRef && isCallOfFunctionNamed(fnRef, "strncpy")))
		return false;
	const SgExpression *dstExp = removeImplicitPromotions(getFnArg(fnRef,0));
	const SgExpression *srcExp = removeImplicitPromotions(getFnArg(fnRef,1));
	const SgExpression *lenExp = getFnArg(fnRef,2);
	assert(dstExp && srcExp && lenExp);

	const SgVarRefExp *srcRef = isSgVarRefExp(srcExp);
	if(!srcRef) {
		const SgCastExp *srcCast = isSgCastExp(srcExp);
		if(!srcCast)
			return false;
		srcRef = isSgVarRefExp(srcCast->get_operand());
		if(!srcRef)
			return false;
	}

	const SgArrayType *arrT = isSgArrayType(srcExp->get_type());
	const SgVarRefExp *dstRef = isSgVarRefExp(dstExp);
	if (!arrT || !dstRef)
		return false;
	size_t len;
	/* Some machines evaluate to unsigned int, others to unsigned long */
	if (!getSizetVal(lenExp,&len))
		return false;
	const SgValueExp *srcVal = isSgValueExp(arrT->get_index());
	if (!srcVal) // VLA or some such...
		return false;
	const SgUnsignedLongVal *srcValInt = isSgUnsignedLongVal(srcVal);
	if (!srcValInt)
		return  false;
	size_t src_size = srcValInt->get_value();

	if (src_size >= len) {
		do {
			// check for null termination, violation if not present

			// first, find the parent block and the next expression after the
			// strncpy()
			const SgStatement * nextStat = findInBlockByOffset(node, 1);
			// if all went well, it should be an expression
			if(!nextStat)
				break;
			const SgExprStatement *nextExpr = isSgExprStatement(nextStat);
			if(!nextExpr)
				break;
			// To comply with the rule, it must be an assignment...
			const SgAssignOp *assignOp = isSgAssignOp(nextExpr->get_expression());
			if (!assignOp)
				break;
			// ... to an array ...
			const SgPntrArrRefExp *arrRef = isSgPntrArrRefExp(assignOp->get_lhs_operand());
			if (!arrRef)
				break;
			// ... that is the same as dstRef ...
			const SgVarRefExp *varRef = isSgVarRefExp(arrRef->get_lhs_operand());
			assert(varRef);
			if (getRefDecl(varRef) != getRefDecl(dstRef))
				break;
			// ... and must have an index ...
			size_t dst_idx;
			if (!getSizetVal(arrRef->get_rhs_operand(), &dst_idx))
				break;
			// ... that is equal to len - 1
			if (len > 0 && dst_idx == len - 1) {
				return false;
			}
		} while(0);
		print_error(node, "STR32-C", "Null-terminate byte strings as required");
		return true;
	} else {
		return false;
	}
}

/**
 * Check if there is a gets or if sscanf/scanf has a "%s"
 */
bool STR35_C(const SgNode *node) {
	const SgFunctionRefExp *fnRef = isSgFunctionRefExp(node);
	if (!fnRef)
		return false;
	if(isCallOfFunctionNamed(fnRef, "gets")) {
		print_error(node, "STR35-C", "Do not copy data from an unbounded source to a fixed-length array");
		return true;
	}

	int argNum;
	if((argNum = getScanfFormatString(fnRef)) == -1) {
		return false;
	}
	const SgExpression *frmt = removeImplicitPromotions(getFnArg(fnRef,argNum));
	assert(frmt);
	const SgStringVal *frmt_s = isSgStringVal( frmt);
	if (frmt_s == NULL) {
		return false;
	}
	std::string s = frmt_s->get_value();
	if(strstr(s.c_str(), "%s") != NULL) {
		print_error(node, "STR35-C", "Do not copy data from an unbounded source to a fixed-length array");
		return true;
	}

	return false;
}

/**
 * Do not specify the dimension of a character array initialized with a string
 * literal
 */
bool STR36_C(const SgNode *node) {
	const SgVariableDeclaration *varDecl = isSgVariableDeclaration(node);
	if (!varDecl)
		return false;

	FOREACH_INITNAME(varDecl->get_variables(), i) {
		assert(*i);
		const SgArrayType *varType = isSgArrayType((*i)->get_type());
		if (!varType)
			continue;
		if (!isAnyCharType(varType->get_base_type()))
			continue;
		const SgAssignInitializer *varInitializer = isSgAssignInitializer((*i)->get_initializer());
		if (!varInitializer)
			continue;
		if (varType->get_index()) {
			print_error(*i, "STR36-C", "Do not specify the dimension of a character array initialized with a string literal");
		return true;
		}
	}

	return false;
}

/**
 * STR37-C. Arguments to character handling functions must be representable as
 * an unsigned char
 *
 * \note The code here should work if these functions were not implemented as
 * macros :(
 */
bool STR37_C(const SgNode *node) {
	const SgFunctionRefExp *fn = isSgFunctionRefExp(node);
	if (!fn)
		return false;

	if (!(isCallOfFunctionNamed(fn, "isalnum")
		||isCallOfFunctionNamed(fn, "isalpha")
		||isCallOfFunctionNamed(fn, "isascii")
		||isCallOfFunctionNamed(fn, "isblank")
		||isCallOfFunctionNamed(fn, "iscntrl")
		||isCallOfFunctionNamed(fn, "isdigit")
		||isCallOfFunctionNamed(fn, "isgraph")
		||isCallOfFunctionNamed(fn, "islower")
		||isCallOfFunctionNamed(fn, "isprint")
		||isCallOfFunctionNamed(fn, "ispunct")
		||isCallOfFunctionNamed(fn, "isspace")
		||isCallOfFunctionNamed(fn, "isupper")
		||isCallOfFunctionNamed(fn, "isxdigit")
		||isCallOfFunctionNamed(fn, "toascii")
		||isCallOfFunctionNamed(fn, "toupper")
		||isCallOfFunctionNamed(fn, "tolower"))) {
		return false;
	}

	const SgExpression *arg = removeImplicitPromotions(getFnArg(fn,0));

	if (isSgTypeUnsignedChar(arg->get_type()))
		return false;

	print_error(node,"STR37-C", "Arguments to character handling functions must be representable as an unsigned char");
	return true;
}

bool STR(const SgNode *node) {
  bool violation = false;
  violation |= STR04_C(node);
  violation |= STR05_C(node);
  violation |= STR06_C(node);
  violation |= STR30_C(node);
  violation |= STR31_C(node);
  violation |= STR32_C(node);
  violation |= STR35_C(node);
  violation |= STR36_C(node);
//  violation |= STR37_C(node);
  return violation;
}
