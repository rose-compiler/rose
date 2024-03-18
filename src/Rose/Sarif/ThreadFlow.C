#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/ThreadFlow.h>

#include <Rose/Sarif/ThreadFlowLocation.h>
#include <Rose/StringUtility/Escape.h>
#include <ROSE_UNUSED.h>

namespace Rose {
namespace Sarif {

ThreadFlow::Ptr
ThreadFlow::instance(const std::string &message) {
    auto self = instance();
    self->message(message);
    return self;
}

bool
ThreadFlow::emit(std::ostream &out) {
    if (message().empty() && locations().empty()) {
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

        if (!locations().empty()) {
            out <<sep1 <<"\"locations\":[";
            std::string sep2;
            for (auto &threadFlowLocation: locations()) {
                out <<sep2;
                threadFlowLocation->emit(out);
                sep2 = ",";
            }
            out <<"]";
            sep1 = ",";
        }

        out <<"}";
        return true;
    }
}

} // namespace
} // namespace

#endif
