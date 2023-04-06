#ifndef ROSE_Tree_Exception_H
#define ROSE_Tree_Exception_H
#include <Rose/Tree/BasicTypes.h>

#include <Rose/Exception.h>

#include <string>

namespace Rose {
namespace Tree {

/** Base class for errors related to the AST. */
class Exception: public Rose::Exception {
public:
    /** Vertex that caused the error. */
    BasePtr vertex;

    /** Construct a new error with the specified message and the causing vertex. */
    Exception(const std::string &mesg, const BasePtr&);
    ~Exception();
};

/** Error when attaching a vertex to a tree and the vertex is already attached somewhere else.
 *
 *  If the operation were allowed to continue without throwing an exception, the data structure would no longer be a tree. */
class InsertionError: public Exception {
public:
    /** Construct a new error with the vertex that caused the error. */
    explicit InsertionError(const BasePtr&);
    ~InsertionError();
};

/** Error when attaching a vertex to a tree would cause a cycle.
 *
 *  If the operation were allowed to continue without throwing an exception, the data structure would no longer be a tree. */
class CycleError: public Exception {
public:
    /** Construct a new error with the node that caused the error. */
    explicit CycleError(const BasePtr&);
    ~CycleError();
};

} // namespace
} // namespace
#endif
