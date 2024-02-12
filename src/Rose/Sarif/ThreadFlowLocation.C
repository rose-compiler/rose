#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/ThreadFlowLocation.h>

#include <Rose/Sarif/Exception.h>
#include <Rose/Sarif/Location.h>

namespace Rose {
namespace Sarif {

ThreadFlowLocation::Ptr
ThreadFlowLocation::instance(const std::string &message) {
    auto self = Ptr(new ThreadFlowLocation);
    self->message(message);
    return self;
}

ThreadFlowLocation::Ptr
ThreadFlowLocation::instance(const Location::Ptr &location) {
    auto self = Ptr(new ThreadFlowLocation);
    self->location(location);
    return self;
}

ThreadFlowLocation::Ptr
ThreadFlowLocation::instance(const std::string &message, const Location::Ptr &location) {
    auto self = Ptr(new ThreadFlowLocation);
    self->message(message);
    self->location(location);
    return self;
}

bool
ThreadFlowLocation::emit(std::ostream &out) {
    if (message().empty() && !location()) {
        throw Sarif::Exception("ThreadFlowLocation must have a message, a location, or both");
    } else {
        std::string sep;
        out <<"{";

        if (!message().empty()) {
            out <<sep <<"\"message\":{"
                <<"\"text\":\"" <<StringUtility::jsonEscape(message()) <<"\""
                <<"}";
            sep = ",";
        }

        if (location()) {
            out <<sep <<"\"location\":";
            location()->emit(out);
            sep = ",";
        }

        out <<"}";
        return true;
    }
}

} // namespace
} // namespace

#endif
