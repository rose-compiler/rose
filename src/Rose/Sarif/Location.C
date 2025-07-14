#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <Rose/Sarif/Location.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/Sarif/Exception.h>
#include <Rose/StringUtility/Escape.h>

namespace Rose {
namespace Sarif {

void
Location::checkConsistency() const {
    // User errors
    if (sourceEnd()) {
        if (!sourceBegin())
            throw Sarif::Exception("source location begin address must be valid");
        if (sourceBegin().fileName() != sourceEnd().fileName())
            throw Sarif::Exception("source region endpoints must have the same file name");
        if (sourceBegin().line() > sourceEnd().line())
            throw Sarif::Exception("source begin line number must be less than than or equal to end line number");
        if (sourceBegin().line() == sourceEnd().line() &&
            sourceBegin().column() && sourceEnd().column() &&
            *sourceBegin().column() >= *sourceEnd().column())
            throw Sarif::Exception("source begin column number must be less than end column number on same line");
    }
}

Location::Ptr
Location::instance(const SourceLocation &loc, const std::string &mesg) {
    if (!loc)
        throw Sarif::Exception("source location begin address must be valid");
    auto self = instance();
    self->sourceBegin(loc);
    self->message(mesg);
    self->checkConsistency();
    return self;
}

Location::Ptr
Location::instance(const SourceLocation &begin, const SourceLocation &end, const std::string &mesg) {
    ASSERT_require(end);
    auto self = instance();
    self->sourceBegin(begin);
    self->sourceEnd(end);
    self->message(mesg);
    self->checkConsistency();
    return self;
}

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
Location::Ptr
Location::instance(const std::string &binaryArtifact, BinaryAnalysis::Address addr, const std::string &mesg) {
    auto self = instance();
    self->binaryArtifact(binaryArtifact);
    self->binaryAddresses(addr);
    self->message(mesg);
    return self;
}
#endif

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
Location::Ptr
Location::instance(const std::string &binaryArtifact, const BinaryAnalysis::AddressInterval &addrs, const std::string &mesg) {
    if (addrs.isEmpty())
        throw Sarif::Exception("binary region cannot be empty");
    auto self = instance();
    self->binaryArtifact(binaryArtifact);
    self->binaryAddresses(addrs);
    self->message(mesg);
    return self;
}
#endif

const SourceLocation&
Location::sourceLocation() const {
    return sourceBegin();
}

std::pair<SourceLocation, SourceLocation>
Location::sourceRegion() const {
    return std::make_pair(sourceBegin(), sourceEnd());
}

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
std::pair<std::string, BinaryAnalysis::Address>
Location::binaryLocation() const {
    if (binaryAddresses()) {
        return std::make_pair(binaryArtifact(), binaryAddresses().least());
    } else {
        return std::make_pair("", 0);
    }
}
#endif

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
std::pair<std::string, BinaryAnalysis::AddressInterval>
Location::binaryRegion() const {
    return std::make_pair(binaryArtifact(), binaryAddresses());
}
#endif

bool
Location::emit(std::ostream &out) {
    out <<"{";

    if (sourceBegin()) {
        out <<"\"physicalLocation\":{"
            <<"\"artifactLocation\":{"
            <<"\"uri\":\"" <<StringUtility::jsonEscape(sourceBegin().fileName().string()) <<"\""
            <<"}"
            <<",\"region\":{"
            <<"\"startLine\":" <<sourceBegin().line();
        if (const auto col = sourceBegin().column())
            out <<",\"startColumn\":" <<*col;
        if (sourceEnd()) {
            out <<",\"endLine\":" <<sourceEnd().line();
            if (const auto col = sourceEnd().column())
                out <<",\"endColumn\":" <<*col;
        }
        out <<"}}";

    } else {
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
        ASSERT_forbid(binaryAddresses().isEmpty());
        out <<"\"physicalLocation\":{"
            <<"\"artifactLocation\":{"
            <<"\"uri\":\"" <<StringUtility::jsonEscape(binaryArtifact()) <<"\""
            <<"}"
            <<",\"region\":{"
            <<"\"byteOffset\":" <<binaryAddresses().least();
        if (binaryAddresses() == BinaryAnalysis::AddressInterval::whole()) {
            static_assert(sizeof(BinaryAnalysis::AddressInterval::Value)==8, "binary addresses must be 8 bytes");
            out <<",\"byteLength\":18446744073709551616";
        } else {
            out <<",\"byteLength\":" <<binaryAddresses().size();
        }
        out <<"}}";
#else
        ASSERT_not_reachable("binary analysis is not supported");
#endif
    }

    if (!message().empty()) {
        out <<",\"message\":{"
            <<"\"text\":\"" <<StringUtility::jsonEscape(message()) <<"\""
            <<"}";
    }

    Node::emit(out);

    out <<"}";
    return true;
}

} // namespace
} // namespace

#endif
