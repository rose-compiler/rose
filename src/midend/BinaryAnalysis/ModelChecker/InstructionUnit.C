#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <BinaryAnalysis/ModelChecker/InstructionUnit.h>

#include <BinaryAnalysis/ModelChecker/Semantics.h>
#include <BinaryAnalysis/ModelChecker/Settings.h>
#include <BinaryAnalysis/ModelChecker/Tag.h>
#include <boost/format.hpp>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

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
InstructionUnit::printSteps(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix,
                            size_t stepOrigin, size_t maxSteps) const {
    if (maxSteps > 0) {
        if (sourceLocation())
            out <<prefix <<"from " <<sourceLocation() <<"\n";

        out <<(boost::format("%s#%-6d %s\n")
               %prefix
               %stepOrigin
               %(insn_->toString()));
    }
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
InstructionUnit::execute(const Settings::Ptr &settings, const Semantics::Ptr &semantics, const BS::RiscOperatorsPtr &ops) {
    ASSERT_not_null(settings);
    ASSERT_not_null(semantics);
    ASSERT_not_null(ops);
    std::vector<Tag::Ptr> tags;
    BS::DispatcherPtr cpu = semantics->createDispatcher(ops);
    BS::Formatter fmt;
    fmt.set_line_prefix("      ");

    SAWYER_MESG_FIRST(mlog[WHERE], mlog[TRACE], mlog[DEBUG]) <<"  executing " <<printableName() <<"\n";
    if (mlog[DEBUG] && settings->showInitialStates)
        mlog[DEBUG] <<"    initial state\n" <<(*ops->currentState() + fmt);

    if (Tag::Ptr tag = executeInstruction(settings, insn_, cpu))
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
