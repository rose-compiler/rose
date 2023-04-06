#ifndef ROSE_Tree_H
#define ROSE_Tree_H

#include <Rose/Tree/Base.h>
#include <Rose/Tree/BasicTypes.h>
#include <Rose/Tree/Exception.h>
#include <Rose/Tree/List.h>

namespace Rose {

/** Tree utilities.
 *
 *  This namespace holds types and functions for working with tree data structures with the following properties:
 *
 *  @li Each vertex of the tree is allocated on the stack and is reference counted.
 *
 *  @li Each vertex has data members, some of which are pointers to other vertices.
 *
 *  @li Some of the vertex pointers are edges in the tree, while others are not. An edge has two end-points: the parent and the
 *  child.
 *
 *  @li Each vertex has a parent pointer that points to the parent vertex in the tree if there is a tree edge from the parent
 *  to the child. This parent pointer is read-only and adjusted automatically when a vertex is inserted or erased from a tree.
 *
 *  @li In order for the tree to be well formed, a vertex can point to at most one parent.
 *
 *  @li Edges that form the tree can be traversed in a forward or reverse direction. Forward edges go from parent to child, and
 *  reverse edges go from child to parent.
 *
 *  @li Tree vertex types are polymorphic. */
namespace Tree {}

} // namespace
#endif
