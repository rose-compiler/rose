#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/CodeFlow.h>

#include <Rose/Sarif/ThreadFlow.h>
#include <ROSE_UNUSED.h>

#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

namespace Rose {
namespace Sarif {

CodeFlow::~CodeFlow() {}

CodeFlow::CodeFlow(const std::string &message)
    : message_(message), threadFlows(*this) {
    threadFlows.beforeResize(boost::bind(&CodeFlow::checkThreadFlowsResize, this, _1, _2));
    threadFlows.afterResize(boost::bind(&CodeFlow::handleThreadFlowsResize, this, _1, _2));
}

CodeFlow::Ptr
CodeFlow::instance(const std::string &message) {
    return Ptr(new CodeFlow(message));
}

const std::string&
CodeFlow::message() const {
    return message_;
}

void
CodeFlow::message(const std::string &s) {
    if (s == message_)
        return;
    checkPropertyChange("CodeFlow", "message", message_.empty(), {{"threadFlows", threadFlows.empty()}});
    message_ = s;
    if (isIncremental())
        emitMessage(incrementalStream(), emissionPrefix());
}

bool
CodeFlow::emitMessage(std::ostream &out, const std::string &firstPrefix) {
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
CodeFlow::checkThreadFlowsResize(int delta, const ThreadFlow::Ptr &threadFlow) {
    if (!threadFlow)
        throw Sarif::Exception("cannot add null thread flow to a result");
    if (isIncremental() && delta < 0)
        throw IncrementalError("thread flows cannot be removed from a code flow");
}

void
CodeFlow::handleThreadFlowsResize(int delta, const ThreadFlow::Ptr &threadFlow) {
    if (isIncremental()) {
        ROSE_UNUSED(delta);
        ASSERT_require(1 == delta);
        ASSERT_forbid(threadFlows.empty());
        lock(threadFlows.back(), "threadFlows");
        if (threadFlows.size() >= 2)
            threadFlows[threadFlows.size() - 2]->freeze();

        std::ostream &out = incrementalStream();
        const std::string p = emissionPrefix();
        if (1 == threadFlows.size())
            out <<p <<"threadFlows:\n";
        threadFlow->emitYaml(out, makeListPrefix(p));
    }
}

void
CodeFlow::emitYaml(std::ostream &out, const std::string &firstPrefix) {
    const bool emittedMessage = emitMessage(out, firstPrefix);

    if (!threadFlows.empty()) {
        const std::string p = emittedMessage ? makeNextPrefix(firstPrefix) : firstPrefix;
        out <<p <<"threadFlows:\n";
        for (auto &threadFlow: threadFlows) {
            threadFlow->emitYaml(out, makeListPrefix(p));
            if (isIncremental()) {
                lock(threadFlow, "threadFlows");
                if (threadFlow != threadFlows.back())
                    threadFlow->freeze();
            }
        }
    }
}

std::string
CodeFlow::emissionPrefix() {
    return makeObjectPrefix(makeObjectPrefix(parent->emissionPrefix()));
}

} // namespace
} // namespace

#endif
