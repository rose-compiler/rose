#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/Location.h>

#include <Rose/Affirm.h>
#include <Rose/Sarif/Exception.h>

namespace Rose {
namespace Sarif {

Location::~Location() {}

Location::Location(const SourceLocation &begin, const SourceLocation &end, const std::string &mesg)
    : sourceBegin_(begin), sourceEnd_(end), message_(mesg) {
    if (!begin)
        throw Sarif::Exception("source location begin address must be valid");
    if (end) {
        if (begin.fileName() != end.fileName())
            throw Sarif::Exception("source region endpoints must have the same file name");
        if (begin.line() > end.line())
            throw Sarif::Exception("source begin line number must be less than than or equal to end line number");
        if (begin.line() == end.line() && begin.column() && end.column() && *begin.column() >= *end.column())
            throw Sarif::Exception("source begin column number must be less than end column number on same line");
    }
}

Location::Location(const std::string &binaryArtifact, const BinaryAnalysis::AddressInterval &addrs, const std::string &mesg)
    : binaryArtifact_(binaryArtifact), binaryRegion_(addrs), message_(mesg) {
    if (addrs.isEmpty())
        throw Sarif::Exception("binary region cannot be empty");
}

Location::Ptr
Location::instance(const SourceLocation &loc, const std::string &mesg) {
    return Ptr(new Location(loc, SourceLocation(), mesg));
}

Location::Ptr
Location::instance(const SourceLocation &begin, const SourceLocation &end, const std::string &mesg) {
    ASSERT_require(end);
    return Ptr(new Location(begin, end, mesg));
}

Location::Ptr
Location::instance(const std::string &binaryArtifact, rose_addr_t addr, const std::string &mesg) {
    return Ptr(new Location(binaryArtifact, addr, mesg));
}

Location::Ptr
Location::instance(const std::string &binaryArtifact, const BinaryAnalysis::AddressInterval &addrs, const std::string &mesg) {
    return Ptr(new Location(binaryArtifact, addrs, mesg));
}

const SourceLocation&
Location::sourceLocation() const {
    return sourceBegin_;
}

std::pair<SourceLocation, SourceLocation>
Location::sourceRegion() const {
    return std::make_pair(sourceBegin_, sourceEnd_);
}

std::pair<std::string, rose_addr_t>
Location::binaryLocation() const {
    if (binaryRegion_) {
        return std::make_pair(binaryArtifact_, binaryRegion_.least());
    } else {
        return std::make_pair("", 0);
    }
}

std::pair<std::string, BinaryAnalysis::AddressInterval>
Location::binaryRegion() const {
    return std::make_pair(binaryArtifact_, binaryRegion_);
}

const std::string&
Location::message() const {
    return message_;
}

void
Location::message(const std::string &s) {
    if (s == message_)
        return;
    checkPropertyChange("Location", "message", message_.empty(), {});
    message_ = s;
    if (isIncremental())
        emitMessage(incrementalStream(), emissionPrefix());
}

void
Location::emitMessage(std::ostream &out, const std::string &firstPrefix) {
    if (!message_.empty()) {
        out <<firstPrefix <<"message:\n";
        out <<makeObjectPrefix(firstPrefix) <<"text: " <<StringUtility::yamlEscape(message_) <<"\n";
    }
}

void
Location::emitYaml(std::ostream &out, const std::string &firstPrefix) {
    const std::string p = makeNextPrefix(firstPrefix);
    const std::string pp = makeObjectPrefix(p);
    const std::string ppp = makeObjectPrefix(pp);

    if (sourceBegin_) {
        out <<firstPrefix <<"physicalLocation:\n";
        out <<pp <<"artifactLocation:\n";
        out <<ppp <<"uri: " <<StringUtility::yamlEscape(sourceBegin_.fileName().string()) <<"\n";
        out <<pp <<"region:\n";
        out <<ppp <<"startLine: " <<sourceBegin_.line() <<"\n";
        if (const auto col = sourceBegin_.column())
            out <<ppp <<"startColumn: " <<*col <<"\n";
        if (sourceEnd_) {
            out <<ppp <<"endLine: " <<sourceEnd_.line() <<"\n";
            if (const auto col = sourceEnd_.column())
                out <<ppp <<"endColumn: " <<*col <<"\n";
        }

    } else {
        ASSERT_forbid(binaryRegion_.isEmpty());
        out <<firstPrefix <<"physicalLocation:\n";
        out <<pp <<"artifactLocation:\n";
        out <<ppp <<"uri: " <<StringUtility::yamlEscape(binaryArtifact_) <<"\n";
        out <<pp <<"region:\n";
        out <<ppp <<"byteOffset: " <<StringUtility::addrToString(binaryRegion_.least()) <<"\n";
        if (binaryRegion_ == BinaryAnalysis::AddressInterval::whole()) {
            out <<ppp <<"# WARNING: byteLength has been reduced by one to work around SARIF design flaw\n";
            out <<ppp <<"byteLength: " <<StringUtility::addrToString(binaryRegion_.greatest()) <<"\n";
        } else {
            out <<ppp <<"byteLength: " <<StringUtility::addrToString(binaryRegion_.size()) <<"\n";
        }
    }

    emitMessage(out, p);
}

std::string
Location::emissionPrefix() {
    return makeObjectPrefix(makeObjectPrefix(parent->emissionPrefix()));
}

} // namespace
} // namespace

#endif
