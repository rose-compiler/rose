#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/Node.h>

#include <Rose/Sarif/Log.h>
#include <fstream>

namespace Rose {
namespace Sarif {

Node::~Node() {}
Node::Node() {}

bool
Node::isIncremental() {
    if (auto log = findFirstAncestor<Log>()) {
        return log->isIncremental();
    } else {
        return false;
    }
}

std::ostream&
Node::incrementalStream() {
    if (auto log = findFirstAncestor<Log>()) {
        return log->incrementalStream();
    } else {
        static std::ofstream empty;
        return empty;
    }
}

std::string
Node::makeNextPrefix(const std::string &prefix) {
    return std::string(prefix.size(), ' ');
}

std::string
Node::makeObjectPrefix(const std::string &prefix) {
    ASSERT_require(yamlIndentAmount_ > 0);
    return makeNextPrefix(prefix) + std::string(yamlIndentAmount_, ' ');
}

std::string
Node::makeListPrefix(const std::string &prefix) {
    ASSERT_require(yamlIndentAmount_ > 0);
    return makeObjectPrefix(prefix) + "-" + std::string(yamlIndentAmount_ - 1, ' ');
}

bool
Node::isFrozen() const {
    return isFrozen_;
}

void
Node::freeze() {
    isFrozen_ = true;
}

void
Node::thaw() {
    isFrozen_ = false;
}

std::string
Node::emissionPrefix() {
    ASSERT_not_reachable("make this pure virtual before committing to Git");
}

} // namespace
} // namespace

#endif
