#ifndef ROSE_checkIsCompilerGeneratedFlag_H
#define ROSE_checkIsCompilerGeneratedFlag_H

/** Checks whether appropriate nodes of an AST are marked as compiler-generated.
 *
 *  The EDG-3.x version of ROSE marks all nodes coming from rose_edg_required_macros_and_functions.h as being
 *  compiler-generated. The EDG-4.x version marks them as only frontend-specific.  This function finds all such nodes and marks
 *  them as compiler-generated.  It actually finds all nodes that are already marked as frontend-specific and makes them also
 *  compiler-generated. */
size_t checkIsCompilerGeneratedFlag(SgNode *ast);

#endif

