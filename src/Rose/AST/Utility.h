#ifndef ROSE_AST_Utility_H
#define ROSE_AST_Utility_H
#include <RoseFirst.h>

#include <iostream>
#include <map>
#include <ostream>

class SgNode;

namespace Rose {
namespace AST {

/** Collection of utility functions to operate on the AST. */
namespace Utility {

using replacement_map_t = std::map<SgNode *, SgNode *>;

//! For all nodes in the memory pool, it looks for the edges in the replacement map. If a match is found the edge is updated.
void edgePointerReplacement(replacement_map_t const &);

//! Traverse the AST `root` looking for the edges in the replacement map. If a match is found the edge is updated.
void edgePointerReplacement(SgNode * root, replacement_map_t const &);


/** Check that all parent pointers in the specified subtree are correct.
 *
 *  This function does a traversal of the tree starting at the specified root and checks that each child points to the
 *  correct parent. If not, then an error message is printed and the function returns false. If you do not want error
 *  messages, then pass a closed output stream.
 *
 *  Safety: This function is not thread safe. */
bool checkParentPointers(SgNode *root, std::ostream &out = std::cerr);

/** Set parent pointers based on child pointers in a subtree.
 *
 *  Traverses a subtree, and makes sure that each child points to its parent. No diagnostics are emitted; if you want diagnostics
 *  then you should call @ref checkParentPointers instead. I.e., checking and repairing are two separate operations.
 *
 *  Caveats:
 *
 *  @li If @p root is null, then this function is a no-op.
 *
 *  @li The parent pointer for the node pointed to by @p root is not adjusted.
 *
 *  @li Parent pointers in other subtrees that might be reachable from non-tree pointers that cross from this subtree to another
 *  are not adjusted.
 *
 *  Safety: This function is not thread safe. */
void repairParentPointers(SgNode *root);

} } }

#endif /* ROSE_AST_Utility_H */
