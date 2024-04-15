#include <featureTests.h>
#ifdef ROSE_ENABLE_CONCOLIC_TESTING
#include <Rose/BinaryAnalysis/Concolic/ExecutionLocation.h>

#include <boost/lexical_cast.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Concolic {

ExecutionLocation
ExecutionLocation::nextPrimary() const {
    return ExecutionLocation(primary_ + 1, 0, When::POST);
}

ExecutionLocation
ExecutionLocation::nextSecondary(When when) const {
    return ExecutionLocation(primary_, secondary_ + 1, when);
}

bool
ExecutionLocation::operator<(const ExecutionLocation &other) const {
    if (this->primary_ != other.primary_)
        return this->primary_ < other.primary_;
    if (this->when_ != other.when_)
        return this->when_ < other.when_;
    return this->secondary_ < other.secondary_;
}

// class method
bool
ExecutionLocation::isSorted(const ExecutionLocation &a, const ExecutionLocation &b) {
    return a < b;
}

void
ExecutionLocation::print(std::ostream &out) const {
    switch (when_) {
        case When::PRE:
            out <<"pre";
            break;
        case When::POST:
            out <<"post";
            break;
    }
    out <<"." <<primary_ <<"." <<secondary_;
}

std::string
ExecutionLocation::toString() const {
    return boost::lexical_cast<std::string>(*this);
}

std::ostream&
operator<<(std::ostream &out, const ExecutionLocation &x) {
    x.print(out);
    return out;
}

} // namespace
} // namespace
} // namespace

#endif
