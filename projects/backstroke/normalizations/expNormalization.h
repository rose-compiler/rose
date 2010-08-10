#ifndef REVERSE_COMPUTATION_NORMALIZATION
#define REVERSE_COMPUTATION_NORMALIZATION

#include <rose.h>
#include "ExtractFunctionArguments.h"

namespace backstroke_norm
{


/** Normalize an event function (main interface). */
void normalizeEvent(SgFunctionDefinition* func_def);

/** A wrapper function which takes a function declaration as parameter. */
inline void normalizeEvent(SgFunctionDeclaration* func_decl)
{
    SgFunctionDeclaration* defining_decl = isSgFunctionDeclaration(func_decl->get_definingDeclaration());
    ROSE_ASSERT(defining_decl && defining_decl->get_definition());
	ExtractFunctionArguments::NormalizeTree(defining_decl->get_definition());
    normalizeEvent(defining_decl->get_definition());
}


namespace details
{


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
void splitCommaOpExpIntoStmt(SgExpression* exp);

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
void preprocess(SgFunctionDefinition* func);

} // namespace details

} // namespace backstroke_norm

#endif
