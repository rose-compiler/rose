#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/Exception.h>

namespace Rose {
namespace Sarif {

IncrementalError
IncrementalError::cannotChangeValue(const std::string &what) {
    return IncrementalError("cannot change " + what + " after emitting a prior value");
}

IncrementalError
IncrementalError::cannotSetAfter(const std::string &what, const std::string &after) {
    return IncrementalError("cannot set " + what + " after emitting " + after);
}

IncrementalError
IncrementalError::frozenObject(const std::string &what) {
    return IncrementalError("cannot change " + what + " after emitting subsequent data");
}

IncrementalError
IncrementalError::notAttached(const std::string &what) {
    return IncrementalError(what + " is not attached to the log");
}

} // namespace
} // namespace

#endif
