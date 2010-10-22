#ifndef REVERSE_COMPUTATION_NORMALIZATION
#define REVERSE_COMPUTATION_NORMALIZATION

#include <rose.h>
#include "ExtractFunctionArguments.h"

namespace BackstrokeNorm
{

namespace BackstrokeNormUtility
{
	/** Preprocess + normalize all expressions + postprocess. */
	void normalize(SgNode* node);

	/** Normalize all expressions in the given AST node. */
	void normalizeExpressions(SgNode* node);
	
	/** Transform a modifying expression into several ones contained in a comma
	 operator expression. The transformations are:

		a = b  ==>  a = b, a
		--a  ==>  --a, a
		a--  ==>  t = a, --a, t
		a && b  ==>  t = a, t && t = b, t
		a || b  ==>  t = a, t || t = b, t
	*/
	void getAndReplaceModifyingExpression(SgExpression*& exp);

	/** Split a comma expression into several statements. */
	void turnCommaOpExpIntoStmt(SgExpression* exp);

	/** Turn a conditional expression into if statements. */
	void turnConditionalExpIntoStmt(SgExpression* exp);

	/** Propagate comma operation expressions, e.g. (a, b) + c ==> a, (b + c). */
	SgExpression* propagateCommaOpExp(SgExpression* exp);

	/** Propagate conditional operation expressions,
	   e.g. (a ? b : c) = d ==> a ? (b = d) : (c = d). */
	SgExpression* propagateConditionalExp(SgExpression* exp);

	inline SgExpression* propagateCommaOpAndConditionalExp(SgExpression* exp)
	{
		exp = propagateCommaOpExp(exp);
		return propagateConditionalExp(exp);
	}

	/** Preprocess the code to be normalized. */
	void preprocess(SgNode* node);

	//! Tell if the given node is inside of a sizeof operator.
	bool isInSizeOfOp(SgNode* node);

} // namespace BackstrokeNormUtility


inline void normalizeEvent(SgFunctionDefinition* func_def)
{
	BackstrokeNormUtility::normalize(func_def->get_body());
}

/** A wrapper function which takes a function declaration as parameter. */
inline void normalizeEvent(SgFunctionDeclaration* func_decl)
{
    SgFunctionDeclaration* defining_decl = isSgFunctionDeclaration(func_decl->get_definingDeclaration());
    ROSE_ASSERT(defining_decl && defining_decl->get_definition());
	ExtractFunctionArguments::NormalizeTree(defining_decl->get_definition());
    normalizeEvent(defining_decl->get_definition());
}


} // namespace BackstrokeNorm

#endif
