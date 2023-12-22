#include <sage3basic.h>
#include <Rose/Sarif/Location.h>

#include <Rose/Affirm.h>
#include <Rose/Sarif/Exception.h>

namespace Rose {
namespace Sarif {

Location::~Location() {}

Location::Location(const SourceLocation &srcLoc, const std::string &mesg)
    : sourceLocation_(srcLoc), message_(mesg) {
    ASSERT_require(srcLoc);
}

Location::Location(const std::string &binaryArtifact, rose_addr_t addr, const std::string &mesg)
    : binaryArtifact_(binaryArtifact), address_(addr), message_(mesg) {}

Location::Ptr
Location::instance(const SourceLocation &loc, const std::string &mesg) {
    ASSERT_require(loc);
    return Ptr(new Location(loc, mesg));
}

Location::Ptr
Location::instance(const std::string &binaryArtifact, rose_addr_t addr, const std::string &mesg) {
    return Ptr(new Location(binaryArtifact, addr, mesg));
}

const SourceLocation&
Location::sourceLocation() const {
    return sourceLocation_;
}

std::pair<std::string, rose_addr_t>
Location::binaryLocation() const {
    return std::make_pair(binaryArtifact_, address_);
}

const std::string&
Location::message() const {
    return message_;
}

void
Location::message(const std::string &s) {
    if (s == message_)
        return;
    if (isFrozen())
        throw IncrementalError::frozenObject("Location");
    if (isIncremental()) {
        if (!message_.empty())
            throw IncrementalError::cannotChangeValue("Location::message");
    }

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

    if (sourceLocation_) {
        out <<firstPrefix <<"physicalLocation:\n";
        out <<pp <<"artifactLocation:\n";
        out <<ppp <<"uri: " <<StringUtility::yamlEscape(sourceLocation_.fileName().string()) <<"\n";
        out <<p <<"region:\n";
        out <<pp <<"startLine: " <<sourceLocation_.line() <<"\n";
        if (auto col = sourceLocation_.column())
            out <<pp <<"startColumn: " <<*col <<"\n";
    } else {
        out <<firstPrefix <<"physicalLocation:\n";
        out <<pp <<"artifactLocation:\n";
        out <<ppp <<"uri: " <<StringUtility::yamlEscape(binaryArtifact_) <<"\n";
        out <<p <<"region:\n";
        out <<pp <<"byteOffset: " <<StringUtility::addrToString(address_) <<"\n";
    }

    emitMessage(out, p);
}

std::string
Location::emissionPrefix() {
    return makeObjectPrefix(makeObjectPrefix(parent->emissionPrefix()));
}

} // namespace
} // namespace
