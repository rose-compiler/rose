#include <featureTests.h>
#ifdef ROSE_ENABLE_MODEL_CHECKER
#include <Rose/BinaryAnalysis/ModelChecker/InstructionUnit.h>

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Dispatcher.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Formatter.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/ModelChecker/SemanticCallbacks.h>
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>
#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>
#include <Rose/Sarif/Location.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/NumberToString.h>

#include <SgAsmInstruction.h>

#include <boost/format.hpp>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

InstructionUnit::InstructionUnit(SgAsmInstruction *insn, const SourceLocation &sloc)
    : ExecutionUnit(sloc), insn_(insn) {}

InstructionUnit::~InstructionUnit() {}

InstructionUnit::Ptr
InstructionUnit::instance(SgAsmInstruction *insn, const SourceLocation &sloc) {
    ASSERT_not_null(insn);
    return Ptr(new InstructionUnit(insn, sloc));
}

SgAsmInstruction*
InstructionUnit::instruction() const {
    // No lock necessary since this property is read-only
    ASSERT_not_null(insn_);
    return insn_;
}

std::string
InstructionUnit::printableName() const {
    // No lock necessary since insn_ is read-only
    ASSERT_not_null(insn_);
    return "instruction " + insn_->toString();
}

void
InstructionUnit::printSteps(const Settings::Ptr&, std::ostream &out, const std::string &prefix, size_t stepOrigin,
                            size_t maxSteps) const {
    if (maxSteps > 0) {
        if (sourceLocation())
            out <<prefix <<"from " <<sourceLocation() <<"\n";

        out <<(boost::format("%s#%-6d %s\n")
               %prefix
               %stepOrigin
               %(insn_->toString()));
    }
}

void
InstructionUnit::toYamlHeader(const Settings::Ptr&, std::ostream &out, const std::string &prefix1) const {
    out <<prefix1 <<"vertex-type: instruction\n";
    if (auto va = address()) {
        std::string prefix(prefix1.size(), ' ');
        out <<prefix <<"vertex-address: " <<StringUtility::addrToString(*va) <<"\n";
    }
}

void
InstructionUnit::toYamlSteps(const Settings::Ptr&, std::ostream &out, const std::string &prefix1, size_t /*stepOrigin*/,
                             size_t maxSteps) const {
    if (maxSteps > 0) {
        out <<prefix1 <<"instruction: " <<StringUtility::yamlEscape(insn_->toString()) <<"\n";

        if (sourceLocation()) {
            std::string prefix(prefix1.size(), ' ');
            out <<prefix <<"    source-file: " <<StringUtility::yamlEscape(sourceLocation().fileName().string()) <<"\n"
                <<prefix <<"    source-line: " <<sourceLocation().line() <<"\n";
            if (sourceLocation().column())
                out <<prefix <<"    source-column: " <<*sourceLocation().column() <<"\n";
        }
    }
}

std::vector<Sarif::Location::Ptr>
InstructionUnit::toSarif(const size_t maxSteps) const {
    std::vector<Sarif::Location::Ptr> retval;
    if (maxSteps > 0) {
        retval.push_back(Sarif::Location::instance("file:///proc/self/mem",
                                                   AddressInterval::baseSize(insn_->get_address(), insn_->get_size()),
                                                   insn_->toString()));
        if (const auto sloc = sourceLocation())
            retval.push_back(Sarif::Location::instance(sloc));
    }
    return retval;
}

size_t
InstructionUnit::nSteps() const {
    return 1;
}

Sawyer::Optional<rose_addr_t>
InstructionUnit::address() const {
    // No lock necessary since this property is read-only
    ASSERT_not_null(insn_);
    return insn_->get_address();
}

std::vector<Tag::Ptr>
InstructionUnit::execute(const Settings::Ptr &settings, const SemanticCallbacks::Ptr &semantics,
                         const BS::RiscOperators::Ptr &ops) {
    ASSERT_not_null(settings);
    ASSERT_not_null(semantics);
    ASSERT_not_null(ops);
    std::vector<Tag::Ptr> tags;
    BS::Dispatcher::Ptr cpu = semantics->createDispatcher(ops);
    BS::Formatter fmt;
    fmt.set_line_prefix("      ");

    SAWYER_MESG_FIRST(mlog[WHERE], mlog[TRACE], mlog[DEBUG]) <<"  executing " <<printableName() <<"\n";
    if (mlog[DEBUG] && settings->showInitialStates)
        mlog[DEBUG] <<"    initial state\n" <<(*ops->currentState() + fmt);

    if (Tag::Ptr tag = executeInstruction(settings, insn_, cpu, 0))
        tags.push_back(tag);
    if (!ops->currentState())
        return tags;                                    // execution failed

    if (settings->showFinalStates)
        SAWYER_MESG(mlog[DEBUG]) <<"    final state\n" <<(*ops->currentState() + fmt);
    return tags;
}

} // namespace
} // namespace
} // namespace

#endif
