#include "rosedll.h"

class SgProject;
class SgNode;

/*
 * Perform the transformation on the given project.  See implementation notes for details
 */
ROSE_DLL_API void shortCircuitingTransformation(SgProject *prj);

/*
 * Provides annotation information needed to place temporary deallocation in the correct place.
 * This annotation may be on an SgIfStmt (conjunct or disjunct of a boolean expression)
 * or an SgExprStatement (lhs of an SgCommaExp).
 */
bool IsSCGenerated(const SgNode *n);
