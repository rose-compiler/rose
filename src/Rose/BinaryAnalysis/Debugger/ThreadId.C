#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Debugger/ThreadId.h>

#include <boost/lexical_cast.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Debugger {

ThreadId
ThreadId::unspecified() {
    return ThreadId();
}

void
ThreadId::print(std::ostream &out) const {
    out <<id_;
}

std::string
ThreadId::toString() const {
    return boost::lexical_cast<std::string>(*this);
}

std::ostream&
operator<<(std::ostream &out, const ThreadId tid) {
    tid.print(out);
    return out;
}

} // namespace
} // namespace
} // namespace

#endif
