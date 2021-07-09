#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/ModelChecker/ExecutionUnit.h>

#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>
#include <Rose/BinaryAnalysis/ModelChecker/SourceLister.h>
#include <Rose/BinaryAnalysis/ModelChecker/Tag.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Dispatcher.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics2/BaseSemantics/RiscOperators.h>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics2::BaseSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace ModelChecker {

ExecutionUnit::ExecutionUnit(const SourceLocation &sloc)
    : sourceLocation_(sloc) {}

ExecutionUnit::~ExecutionUnit() {}

Sawyer::Optional<rose_addr_t>
ExecutionUnit::address() const {
    return Sawyer::Nothing();
}

SourceLocation
ExecutionUnit::sourceLocation() const {
    // no lock necessary since data member is immutable
    return sourceLocation_;
}

bool
ExecutionUnit::containsUnknownInsn() const {
    return false;
}

Tag::Ptr
ExecutionUnit::executeInstruction(const Settings::Ptr &settings, SgAsmInstruction *insn, const BS::Dispatcher::Ptr &cpu) {
    ASSERT_not_null(settings);
    ASSERT_not_null(insn);
    ASSERT_not_null(cpu);

    Tag::Ptr retval;
    try {
        cpu->processInstruction(insn);
    } catch (const ThrownTag &e) {
        retval = e.tag;
        ASSERT_not_null(retval);
        SAWYER_MESG(mlog[DEBUG]) <<"      semantics threw a " <<retval->printableName() <<" tag\n";
        cpu->operators()->currentState(BS::StatePtr()); // to indicate that execution was interrupted
    } catch (const BS::Exception &e) {
        if (settings->ignoreSemanticFailures) {
            SAWYER_MESG(mlog[DEBUG]) <<"      semantics failed; continuing as if it were okay\n";
        } else {
            SAWYER_MESG(mlog[DEBUG]) <<"      semantics exception: " <<e.what() <<"\n";
        }
    } catch (const Rose::Exception &e) {
        SAWYER_MESG(mlog[DEBUG]) <<"      Rose::Exception: " <<e.what() <<"\n";
        cpu->operators()->currentState(BS::StatePtr()); // to indicate that execution was interrupted
    }
    return retval;
}

void
ExecutionUnit::printSource(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix) const {
    return printSource(settings, out, prefix, sourceLocation_);
}

void
ExecutionUnit::printSource(const Settings::Ptr &settings, std::ostream &out, const std::string &prefix,
                           const SourceLocation &sloc) const {
    ASSERT_not_null(settings);
    if (settings->sourceLister)
        settings->sourceLister->list(out, sloc, prefix);
}

} // namespace
} // namespace
} // namespace

#endif
