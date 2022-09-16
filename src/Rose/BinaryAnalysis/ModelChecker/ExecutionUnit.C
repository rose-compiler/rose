#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <sage3basic.h>
#include <Rose/BinaryAnalysis/ModelChecker/ExecutionUnit.h>

#include <Rose/BinaryAnalysis/ModelChecker/ErrorTag.h>
#include <Rose/BinaryAnalysis/ModelChecker/Settings.h>
#include <Rose/BinaryAnalysis/ModelChecker/SourceLister.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Dispatcher.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/Exception.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/RiscOperators.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/State.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

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
ExecutionUnit::executeInstruction(const Settings::Ptr &settings, SgAsmInstruction *insn, const BS::Dispatcher::Ptr &cpu,
                                  size_t nodeStep) {
    ASSERT_not_null(settings);
    ASSERT_not_null(insn);
    ASSERT_not_null(cpu);

    // For x86, an addressing mode of FP + (R * C1) + C2 (where FP is the frame pointer register, R is some other register and
    // C1 and C2 are constants) should be evaluated as if it where FP + C2 + (R * C1). This gives the model checker a chance to
    // recognize that FP + C2 is the address of the variable that was intended to be accessed before we add the offset for the
    // index to check for an out-of-bounds access.
    if (auto x86 = isSgAsmX86Instruction(insn)) {
        for (size_t i = 0; i < x86->nOperands(); ++i) {
            if (auto mre = isSgAsmMemoryReferenceExpression(x86->operand(i))) {
                if (auto add1 = isSgAsmBinaryAdd(mre->get_address())) {
                    if (auto add2 = isSgAsmBinaryAdd(add1->get_lhs())) {
                        if (auto fp = isSgAsmDirectRegisterExpression(add2->get_lhs())) {
                            if (auto mul = isSgAsmBinaryMultiply(add2->get_rhs())) {
                                if (auto r = isSgAsmDirectRegisterExpression(mul->get_lhs())) {
                                    if (auto c1 = isSgAsmIntegerValueExpression(mul->get_rhs())) {
                                        if (auto c2 = isSgAsmIntegerValueExpression(add1->get_rhs())) {
                                            const RegisterDescriptor REG_FP =
                                                cpu->registerDictionary()->findLargestRegister(x86_regclass_gpr, x86_gpr_bp);
                                            ASSERT_require(REG_FP);
                                            if (fp->get_descriptor() == REG_FP) {
                                                // Pattern found. Swap some arguments
                                                add1->set_rhs(mul);
                                                mul->set_parent(add1);

                                                add2->set_rhs(c2);
                                                c2->set_parent(add2);

                                                SAWYER_MESG(mlog[DEBUG]) <<"      rearranged argument " <<i <<"\n";
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    Tag::Ptr retval;
    try {
        cpu->processInstruction(insn);
    } catch (const ThrownTag &e) {
        retval = e.tag;
        ASSERT_not_null(retval);
        SAWYER_MESG(mlog[DEBUG]) <<"      semantics threw a " <<retval->printableName() <<" tag\n";
        cpu->operators()->currentState(BS::State::Ptr()); // to indicate that execution was interrupted
    } catch (const BS::Exception &e) {
        if (settings->ignoreSemanticFailures) {
            SAWYER_MESG(mlog[DEBUG]) <<"      semantics failed; continuing as if it were okay\n";

            // Update the instrucition pointer if the exception occurred before that could happen. Assume that we would just
            // execute instruction that follows the failed instruction in memory.
            const RegisterDescriptor IP = cpu->instructionPointerRegister();
            const rose_addr_t curVa = insn->get_address();
            if (cpu->operators()->peekRegister(IP)->toUnsigned().orElse(curVa) == curVa) {
                BS::SValue::Ptr nextVa = cpu->operators()->number_(IP.nBits(), curVa + insn->get_size());
                cpu->operators()->writeRegister(cpu->instructionPointerRegister(), nextVa);
            }

        } else {
            SAWYER_MESG(mlog[DEBUG]) <<"      semantics exception: " <<e.what() <<"\n";
            retval = ErrorTag::instance(nodeStep, "semantic-failure", e.what(), insn);
            cpu->operators()->currentState(BS::State::Ptr()); // to indicate that execution was interrupted
        }
    } catch (const Rose::Exception &e) {
        SAWYER_MESG(mlog[DEBUG]) <<"      Rose::Exception: " <<e.what() <<"\n";
        cpu->operators()->currentState(BS::State::Ptr()); // to indicate that execution was interrupted
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
