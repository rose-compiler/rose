#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/ThreadFlowLocation.h>

#include <Rose/Sarif/Location.h>

#include <boost/bind/bind.hpp>

using namespace boost::placeholders;

namespace Rose {
namespace Sarif {

ThreadFlowLocation::~ThreadFlowLocation() {}

ThreadFlowLocation::ThreadFlowLocation(const std::string &message, const Location::Ptr &location)
    : message_(message), location(*this, location) {
    this->location.beforeChange(boost::bind(&ThreadFlowLocation::checkLocationChange, this, _1, _2));
    this->location.afterChange(boost::bind(&ThreadFlowLocation::handleLocationChange, this, _1, _2));
}

ThreadFlowLocation::Ptr
ThreadFlowLocation::instance(const std::string &message) {
    return Ptr(new ThreadFlowLocation(message, Location::Ptr()));
}

ThreadFlowLocation::Ptr
ThreadFlowLocation::instance(const Location::Ptr &location) {
    return Ptr(new ThreadFlowLocation("", location));
}

ThreadFlowLocation::Ptr
ThreadFlowLocation::instance(const std::string &message, const Location::Ptr &location) {
    return Ptr(new ThreadFlowLocation(message, location));
}

const std::string&
ThreadFlowLocation::message() const {
    return message_;
}

void
ThreadFlowLocation::message(const std::string &s) {
    if (s == message_)
        return;
    checkPropertyChange("ThreadFlowLocation", "message", message_.empty(), {{"location", !location}});
    message_ = s;
    if (isIncremental())
        emitMessage(incrementalStream(), emissionPrefix());
}

bool
ThreadFlowLocation::emitMessage(std::ostream &out, const std::string &firstPrefix) {
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
ThreadFlowLocation::checkLocationChange(const Location::Ptr &oldLocation, const Location::Ptr&) {
    checkPropertyChange("ThreadFlowLocation", "location", !oldLocation, {});
}

void
ThreadFlowLocation::handleLocationChange(const Location::Ptr&, const Location::Ptr &newLocation) {
    if (isIncremental() && newLocation)
        newLocation->emitYaml(incrementalStream(), emissionPrefix());
}

void
ThreadFlowLocation::emitYaml(std::ostream &out, const std::string &firstPrefix) {
    if (message_.empty() && !location)
        throw Sarif::Exception("ThreadFlowLocation must have a message, a location, or both");

    const bool emittedMessage = emitMessage(out, firstPrefix);
    if (location) {
        const std::string p = emittedMessage ? makeNextPrefix(firstPrefix) : firstPrefix;
        location->emitYaml(out, p);
    }
}

std::string
ThreadFlowLocation::emissionPrefix() {
    return makeObjectPrefix(makeObjectPrefix(parent->emissionPrefix()));
}

} // namespace
} // namespace

#endif
