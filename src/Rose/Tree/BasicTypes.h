#ifndef ROSE_Tree_BasicTypes_H
#define ROSE_Tree_BasicTypes_H

#include <memory>

namespace Rose {
namespace Tree {

class InsertionError;

class Base;
using BasePtr = std::shared_ptr<Base>;                  /**< Shared-ownership pointer for @ref Base. */

class CycleError;

template<class T> class Edge;

class Exception;

class ReverseEdge;

} // namespace
} // namespace
#endif
