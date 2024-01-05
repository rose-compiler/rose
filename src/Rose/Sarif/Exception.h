#ifndef ROSE_Sarif_Exception_H
#define ROSE_Sarif_Exception_H

#include <Rose/Exception.h>

namespace Rose {
namespace Sarif {

/** Base class for SARIF exceptions. */
class Exception: public Rose::Exception {
public:
    /** Construct an exception with a message. */
    Exception(const std::string &mesg)
        : Rose::Exception(mesg) {}

    ~Exception() throw() {}
};

/** Exception thrown for incremental log errors. */
class IncrementalError: public Exception {
public:
    /** Construct an exception with a message. */
    IncrementalError(const std::string &mesg)
        : Exception(mesg) {}

    ~IncrementalError() throw() {}

    /** Unable to modify an existing value already emitted. */
    static IncrementalError cannotChangeValue(const std::string &what);

    /** Cannot change a value after emitting something else. */
    static IncrementalError cannotSetAfter(const std::string &what, const std::string &alreadyEmitted);

    /** Cannot change anything about an object because a subsequent object has been emitted. */
    static IncrementalError frozenObject(const std::string &what);

    /** The specified thing is not attached to the same log. */
    static IncrementalError notAttached(const std::string &what);
};

} // namespace
} // namespace

#endif
