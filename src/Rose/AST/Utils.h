
#ifndef ROSE_AST_UTILS_H
#define ROSE_AST_UTILS_H

#include <map>

class SgNode;

namespace Rose {
namespace AST {

/**
 * \brief Collection of utility functions to operate on the AST.
 */
namespace Utils {

using replacement_map_t = std::map<SgNode *, SgNode *>;

//! For all nodes in the memory pool, it looks for the edges in the replacement map. If a match is found the edge is updated.
void edgePointerReplacement(replacement_map_t const &);

//! Traverse the AST `root` looking for the edges in the replacement map. If a match is found the edge is updated.
void edgePointerReplacement(SgNode * root, replacement_map_t const &);

} } }

#endif /* ROSE_AST_UTILS_H */

