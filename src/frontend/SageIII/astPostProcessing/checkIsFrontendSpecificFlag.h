#ifndef ROSE_checkFrontendSpecificFlag_H
#define ROSE_checkFrontendSpecificFlag_H

/** Checks whether appropriate nodes of an AST are marked as front-end specific.
 *
 *  A node is frontend-specific if it was parsed from the "rose_edg_required_macros_and_functions.h" or if it has an ancestor
 *  in the AST that is frontend-specific.   All violations are fixed in place.  Returns the number of violations found/fixed. */
size_t checkIsFrontendSpecificFlag(SgNode *ast);


#endif
