#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/ThreadFlow.h>

#include <Rose/Sarif/ThreadFlowLocation.h>

#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

namespace Rose {
namespace Sarif {

ThreadFlow::~ThreadFlow() {}

ThreadFlow::ThreadFlow(const std::string &message)
    : message_(message), locations(*this) {
    locations.beforeResize(boost::bind(&ThreadFlow::checkLocationsResize, this, _1, _2));
    locations.afterResize(boost::bind(&ThreadFlow::handleLocationsResize, this, _1, _2));
}

ThreadFlow::Ptr
ThreadFlow::instance(const std::string &message) {
    return Ptr(new ThreadFlow(message));
}

const std::string&
ThreadFlow::message() const {
    return message_;
}

void
ThreadFlow::message(const std::string &s) {
    if (s == message_)
        return;
    checkPropertyChange("ThreadFlow", "message", message_.empty(), {{"locations", locations.empty()}});
    message_ = s;
    if (isIncremental())
        emitMessage(incrementalStream(), emissionPrefix());
}

bool
ThreadFlow::emitMessage(std::ostream &out, const std::string &firstPrefix) {
    if (!message_.empty()) {
        out <<firstPrefix <<"message:\n";
        const std::string pp = makeObjectPrefix(firstPrefix);
        out <<pp <<"text: " <<StringUtility::yamlEscape(message_) <<"\n";
        return true;
    } else {
        return false;
    }
}

void
ThreadFlow::checkLocationsResize(int delta, const ThreadFlowLocation::Ptr &threadFlowLocation) {
    if (!threadFlowLocation)
        throw Sarif::Exception("cannot add null thread flow location to a thread flow");
    if (isIncremental() && delta < 0)
        throw IncrementalError("thread flow locations cannot be removed from a thread flow");
}

void
ThreadFlow::handleLocationsResize(int delta, const ThreadFlowLocation::Ptr &threadFlowLocation) {
    if (isIncremental()) {
        ASSERT_require(1 == delta);
        ASSERT_forbid(locations.empty());
        lock(locations.back(), "locations");
        if (locations.size() >= 2)
            locations[locations.size() - 2]->freeze();

        std::ostream &out = incrementalStream();
        const std::string p = emissionPrefix();
        if (1 == locations.size())
            out <<p <<"locations:\n";
        threadFlowLocation->emitYaml(out, makeListPrefix(p));
    }
}

void
ThreadFlow::emitYaml(std::ostream &out, const std::string &firstPrefix) {
    const bool emittedMessage = emitMessage(out, firstPrefix);

    if (!locations.empty()) {
        const std::string p = emittedMessage ? makeNextPrefix(firstPrefix) : firstPrefix;
        out <<p <<"locations:\n";
        for (auto &threadFlowLocation: locations) {
            threadFlowLocation->emitYaml(out, makeListPrefix(p));
            if (isIncremental()) {
                lock(threadFlowLocation, "locations");
                if (threadFlowLocation != locations.back())
                    threadFlowLocation->freeze();
            }
        }
    }
}

std::string
ThreadFlow::emissionPrefix() {
    return makeObjectPrefix(makeObjectPrefix(parent->emissionPrefix()));
}

} // namespace
} // namespace

#endif
