#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/ModelChecker/FailureUnit.h>

#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>
#include <Rose/BinaryAnalysis/ModelChecker/SemanticCallbacks.h>
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>
#include <boost/algorithm/string/trim.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

FailureUnit::FailureUnit(const Sawyer::Optional<rose_addr_t> &va, const SourceLocation &sloc,
                         const std::string &description, const Tag::Ptr &tag)
    : ExecutionUnit(sloc), va_(va), description_(description), tag_(tag) {}

FailureUnit::~FailureUnit() {}

FailureUnit::Ptr
FailureUnit::instance(const Sawyer::Optional<rose_addr_t> &va, const SourceLocation &sloc, const std::string &description) {
    std::string desc = boost::trim_copy(description);
    ASSERT_forbid(desc.empty());
    return Ptr(new FailureUnit(va, sloc, desc, Tag::Ptr()));
}

FailureUnit::Ptr
FailureUnit::instance(const Sawyer::Optional<rose_addr_t> &va, const SourceLocation &sloc, const std::string &description,
                      const Tag::Ptr &tag) {
    std::string desc = boost::trim_copy(description);
    ASSERT_forbid(desc.empty());
    return Ptr(new FailureUnit(va, sloc, desc, tag));
}

const std::string&
FailureUnit::description() const {
    // No lock necessary since this property is read-only
    return description_;
}

std::string
FailureUnit::printableName() const {
    // No lock necessary since va_ and description_ is read-only
    if (va_) {
        return "termination at " + StringUtility::addrToString(*va_) + ": " + description_;
    } else {
        return "termination at unknown address: " + description_;
    }
}

void
FailureUnit::printSteps(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix,
                        size_t stepOrigin, size_t maxSteps) const {
    if (maxSteps > 0) {
        if (sourceLocation())
            out <<prefix <<"from " <<sourceLocation() <<"\n";

        out <<(boost::format("%s#%-6d this step is automatic failure\n")
               %prefix
               %stepOrigin);
    }
}

void
FailureUnit::toYamlHeader(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix1) const {
    out <<prefix1 <<"vertex-type: failure\n";
    if (auto va = address()) {
        std::string prefix(prefix1.size(), ' ');
        out <<prefix <<"vertex-address: " <<StringUtility::addrToString(*va) <<"\n";
    }
}

void
FailureUnit::toYamlSteps(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix1,
                         size_t stepOrigin, size_t maxSteps) const {
    if (maxSteps > 0) {
        out <<prefix1 <<"definition: automatic-failure\n";

        if (sourceLocation()) {
            std::string prefix(prefix1.size(), ' ');
            out <<prefix <<"    source-file: " <<StringUtility::yamlEscape(sourceLocation().fileName().string()) <<"\n"
                <<prefix <<"    source-line: " <<sourceLocation().line() <<"\n";
            if (sourceLocation().column())
                out <<prefix <<"    source-column: " <<*sourceLocation().column() <<"\n";
        }
    }
}

size_t
FailureUnit::nSteps() const {
    return 1;
}

Sawyer::Optional<rose_addr_t>
FailureUnit::address() const {
    // No lock necessary since va_ is read-only
    return va_;
}

std::vector<Tag::Ptr>
FailureUnit::execute(const Settings::Ptr &settings, const SemanticCallbacks::Ptr &semantics, const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(settings);
    ASSERT_not_null(semantics);
    ASSERT_not_null(ops);
    BS::Formatter fmt;
    fmt.set_line_prefix("      ");

    SAWYER_MESG_FIRST(mlog[WHERE], mlog[TRACE], mlog[DEBUG]) <<"  executing " <<printableName() <<"\n";
    if (mlog[DEBUG] && settings->showInitialStates)
        mlog[DEBUG] <<"    initial state\n" <<(*ops->currentState() + fmt);

    ops->currentState(BS::State::Ptr());                // this is how failure is indicated
    if (tag_) {
        return {tag_};
    } else {
        return {};
    }
}

} // namespace
} // namespace
} // namespace

#endif
