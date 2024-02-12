#include <featureTests.h>
#ifdef ROSE_ENABLE_SARIF
#include <sage3basic.h>
#include <Rose/Sarif/Location.h>

#include <Rose/Affirm.h>
#include <Rose/BinaryAnalysis/AddressInterval.h>
#include <Rose/Sarif/Exception.h>

namespace Rose {
namespace Sarif {

void
Location::checkConsistency() const {
    // User errors
    if (sourceEnd_) {
        if (!sourceBegin_)
            throw Sarif::Exception("source location begin address must be valid");
        if (sourceBegin_.fileName() != sourceEnd_.fileName())
            throw Sarif::Exception("source region endpoints must have the same file name");
        if (sourceBegin_.line() > sourceEnd_.line())
            throw Sarif::Exception("source begin line number must be less than than or equal to end line number");
        if (sourceBegin_.line() == sourceEnd_.line() &&
            sourceBegin_.column() && sourceEnd_.column() && *sourceBegin_.column() >= *sourceEnd_.column())
            throw Sarif::Exception("source begin column number must be less than end column number on same line");
    }
}

Location::Ptr
Location::instance(const SourceLocation &loc, const std::string &mesg) {
    if (!loc)
        throw Sarif::Exception("source location begin address must be valid");
    auto self = instance();
    self->sourceBegin_ = loc;
    self->message(mesg);
    self->checkConsistency();
    return self;
}

Location::Ptr
Location::instance(const SourceLocation &begin, const SourceLocation &end, const std::string &mesg) {
    ASSERT_require(end);
    auto self = instance();
    self->sourceBegin_ = begin;
    self->sourceEnd_ = end;
    self->message(mesg);
    self->checkConsistency();
    return self;
}

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
Location::Ptr
Location::instance(const std::string &binaryArtifact, rose_addr_t addr, const std::string &mesg) {
    auto self = instance();
    self->binaryArtifact_ = binaryArtifact;
    self->binaryRegion_ = addr;
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
    self->binaryArtifact_ = binaryArtifact;
    self->binaryRegion_ = addrs;
    self->message(mesg);
    return self;
}
#endif

const SourceLocation&
Location::sourceLocation() const {
    return sourceBegin_;
}

std::pair<SourceLocation, SourceLocation>
Location::sourceRegion() const {
    return std::make_pair(sourceBegin_, sourceEnd_);
}

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
std::pair<std::string, rose_addr_t>
Location::binaryLocation() const {
    if (binaryRegion_) {
        return std::make_pair(binaryArtifact_, binaryRegion_.least());
    } else {
        return std::make_pair("", 0);
    }
}
#endif

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
std::pair<std::string, BinaryAnalysis::AddressInterval>
Location::binaryRegion() const {
    return std::make_pair(binaryArtifact_, binaryRegion_);
}
#endif

bool
Location::emit(std::ostream &out) {
    out <<"{";

    if (sourceBegin_) {
        out <<"\"physicalLocation\":{"
            <<"\"artifactLocation\":{"
            <<"\"uri\":\"" <<StringUtility::jsonEscape(sourceBegin_.fileName().string()) <<"\""
            <<"}"
            <<",\"region\":{"
            <<"\"startLine\":" <<sourceBegin_.line();
        if (const auto col = sourceBegin_.column())
            out <<",\"startColumn\":" <<*col;
        if (sourceEnd_) {
            out <<",\"endLine\":" <<sourceEnd_.line();
            if (const auto col = sourceEnd_.column())
                out <<",\"endColumn\":" <<*col;
        }
        out <<"}}";

    } else {
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
        ASSERT_forbid(binaryRegion_.isEmpty());
        out <<"\"physicalLocation\":{"
            <<"\"artifactLocation\":{"
            <<"\"uri\":\"" <<StringUtility::jsonEscape(binaryArtifact_) <<"\""
            <<"}"
            <<",\"region\":{"
            <<"\"byteOffset\":" <<binaryRegion_.least();
        if (binaryRegion_ == BinaryAnalysis::AddressInterval::whole()) {
            static_assert(sizeof(BinaryAnalysis::AddressInterval::Value)==8);
            out <<",\"byteLength\":18446744073709551616";
        } else {
            out <<",\"byteLength\":" <<binaryRegion_.size();
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

    out <<"}";
    return true;
}

} // namespace
} // namespace

#endif
