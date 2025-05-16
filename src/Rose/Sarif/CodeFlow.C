#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/CodeFlow.h>

#include <Rose/Sarif/ThreadFlow.h>
#include <Rose/StringUtility/Escape.h>
#include <ROSE_UNUSED.h>

namespace Rose {
namespace Sarif {

CodeFlow::Ptr
CodeFlow::instance(const std::string &message) {
    auto self = instance();
    self->message(message);
    return self;
}

bool
CodeFlow::emit(std::ostream &out) {
    if (message().empty() && threadFlows().empty()) {
        return false;

    } else {
        std::string sep1;
        out <<"{";

        if (!message().empty()) {
            out <<sep1 <<"\"message\":{"
                <<"\"text\":\"" <<StringUtility::jsonEscape(message()) <<"\""
                <<"}";
            sep1 = ",";
        }

        if (!threadFlows().empty()) {
            out <<sep1 <<"\"threadFlows\":[";
            std::string sep2;
            for (auto &threadFlow: threadFlows()) {
                out <<sep2;
                threadFlow->emit(out);
                sep2 = ",";
            }
            out <<"]";
        }

        Node::emit(out);

        out <<"}";
        return true;
    }
}

} // namespace
} // namespace

#endif
