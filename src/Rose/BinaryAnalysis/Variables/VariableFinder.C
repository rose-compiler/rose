#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/Variables/VariableFinder.h>

#include <Rose/As.h>
#include <Rose/Affirm.h>
#include <Rose/AST/Traversal.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/DataFlow.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/Partitioner2/Semantics.h>
#include <Rose/BitOps.h>
#include <Rose/CommandLine.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>

#include <SgAsmAarch32Instruction.h>
#include <SgAsmBinaryAdd.h>
#include <SgAsmBinaryPreupdate.h>
#include <SgAsmDirectRegisterExpression.h>
#include <SgAsmIntegerValueExpression.h>
#include <SgAsmM68kInstruction.h>
#include <SgAsmMemoryReferenceExpression.h>
#include <SgAsmPowerpcInstruction.h>
#include <SgAsmX86Instruction.h>
#include <Cxx_GrammarDowncast.h>

#include <Sawyer/ThreadWorkers.h>

using namespace Sawyer::Message::Common;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace IS = Rose::BinaryAnalysis::InstructionSemantics;

namespace Rose {
namespace BinaryAnalysis {
namespace Variables {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// LVarSemantics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace LVarSemantics {

using SValue = P2::Semantics::SValue;
using RegisterState = P2::Semantics::RegisterState;
using MemoryState = P2::Semantics::MemoryMapState;
using State = P2::Semantics::State;

using RiscOperatorsPtr = boost::shared_ptr<class RiscOperators>;


// Operations that can be applied to a single function or part of one function
class RiscOperators: public P2::Semantics::RiscOperators {
public:
    using Super = P2::Semantics::RiscOperators;
    using Ptr = RiscOperatorsPtr;

private:
    const RegisterDescriptor SP, FP;                    // stack pointer register and optional frame ptr reg
    const StackDelta::Analysis &stackDeltas_;           // stack delta information for a single function
    BS::SValue::Ptr initialSp_;                         // stack pointer at start of analysis
    StackVariable::Boundaries &boundaries_;             // variable boundaries on the stack

    // The `spOffset` is the value of the stack pointer at the start of the basic block that is to be processed
    // by this semantic state. The `fpOffset` is the value of the frame pointer (if any) at the beginning of this
    // basic block w.r.t. the value of the stack pointer at the beginning of this basic block.
    Sawyer::Optional<int64_t> spOffset_;                // first known SP value w.r.t. beginning of function
    Sawyer::Optional<int64_t> fpOffset_;                // first known FP value w.r.t. SP at same instruction


protected:
    explicit RiscOperators(const BS::State::Ptr &state, const StackDelta::Analysis &stackDeltas,
                           const RegisterDictionary::Ptr &regdict, StackVariable::Boundaries &boundaries)
        : Super(state, SmtSolverPtr()), SP(notnull(regdict)->stackPointerRegister()),
          FP(notnull(regdict)->stackFrameRegister()), stackDeltas_(stackDeltas), boundaries_(boundaries) {
        (void) SValue::promote(state->protoval());      // runtime type check
        initialSp_ = peekRegister(SP, undefined_(SP.nBits()));
    }

public:
    static Ptr instance(const P2::Partitioner::ConstPtr &partitioner, const StackDelta::Analysis &stackDeltas,
                        StackVariable::Boundaries &boundaries /*in,out*/) {
        ASSERT_not_null(partitioner);

        RegisterDictionary::Ptr regdict = partitioner->architecture()->registerDictionary();
        BS::SValue::Ptr protoval = SValue::instance();
        BS::RegisterState::Ptr registers = RegisterState::instance(protoval, regdict);
        BS::MemoryState::Ptr memory = MemoryState::instance(protoval, protoval);
        BS::State::Ptr state = State::instance(registers, memory);
        return Ptr(new RiscOperators(state, stackDeltas, regdict, boundaries));
    }

public:
    BS::RiscOperators::Ptr create(const BS::SValue::Ptr&/*protoval*/, const SmtSolverPtr& = SmtSolverPtr()) const override {
        ASSERT_not_implemented("[Robb Matzke 2019-09-16]: not needed");
    }

    BS::RiscOperators::Ptr create(const BS::State::Ptr&, const SmtSolverPtr& = SmtSolverPtr()) const override {
        ASSERT_not_implemented("[Robb Matzke 2019-09-16]: not needed");
    }

    static Ptr promote(const BS::RiscOperators::Ptr &x) {
        Ptr retval = as<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

    // Initialize the SP and/or FP as soon as we know how it relates to the beginning of the function.
    void startInstruction(SgAsmInstruction *insn) override {
        if (!spOffset_) {
            if ((spOffset_ = stackDeltas_.toInt(stackDeltas_.instructionInputStackDeltaWrtFunction(insn)))) {
                // The stack pointer offset is w.r.t. the beginning of the function.
                ASSERT_not_null(initialSp_);
                const BS::SValue::Ptr adjustedSp = add(initialSp_, number_(SP.nBits(), *spOffset_));
                writeRegister(SP, adjustedSp);
                SAWYER_MESG(mlog[DEBUG]) <<"  adjusted stack pointer at insn " <<insn->toString() <<"\n"
                                         <<"    original function stack pointer: " <<*initialSp_ <<"\n"
                                         <<"    stack ptr offset w.r.t. function: " <<*spOffset_ <<"\n"
                                         <<"    new stack pointer: " <<*adjustedSp <<"\n";
            }
        }

        if (!fpOffset_ && FP) {
            if ((fpOffset_ = stackDeltas_.toInt(stackDeltas_.instructionInputFrameDelta(insn)))) {
                // The frame pointer offset is w.r.t. the stack pointer for the same instruction
                const BS::SValue::Ptr sp = peekRegister(SP, undefined_(SP.nBits()));
                const BS::SValue::Ptr adjustedFp = add(sp, number_(FP.nBits(), *fpOffset_));
                writeRegister(FP, adjustedFp);
                SAWYER_MESG(mlog[DEBUG]) <<"  adjusted frame pointer at insn " <<insn->toString() <<"\n"
                                         <<"    old frame pointer at this insn: " <<peekRegister(FP, undefined_(FP.nBits())) <<"\n"
                                         <<"    stack pointer at this insn: " <<*sp <<"\n"
                                         <<"    frame ptr offset w.r.t. stack ptr: " <<*fpOffset_ <<"\n"
                                         <<"    new frame pointer: " <<*adjustedFp <<"\n";
            }
        }
        Super::startInstruction(insn);
    }

    BS::SValue::Ptr readMemory(const RegisterDescriptor segreg, const BS::SValue::Ptr &addr, const BS::SValue::Ptr &dflt,
                               const BS::SValue::Ptr &cond) override {
        if (SgAsmInstruction *insn = currentInstruction()) {
            SAWYER_MESG(mlog[DEBUG]) <<"  memory read by instruction " <<insn->toString() <<"\n";
            lookForVariable(addr, Access::READ);
        }
        return Super::readMemory(segreg, addr, dflt, cond);
    }

    void writeMemory(const RegisterDescriptor segreg, const BS::SValue::Ptr &addr, const BS::SValue::Ptr &data,
                     const BS::SValue::Ptr &cond) override {
        if (SgAsmInstruction *insn = currentInstruction()) {
            SAWYER_MESG(mlog[DEBUG]) <<"  memory write by instruction " <<insn->toString() <<"\n";
            lookForVariable(addr, Access::WRITE);
        }
        return Super::writeMemory(segreg, addr, data, cond);
    }

private:
    // Search the `haystack` to find a subexpression structurally equivalent to `needle` and return true iff found.
    static bool findInExpr(const SymbolicExpression::Ptr &needle, const SymbolicExpression::Ptr &haystack) {
        struct Finder: SymbolicExpression::Visitor {
            std::set<SymbolicExpression::Hash> seen;    // expressions in which we've already looked
            SymbolicExpression::Ptr needle;             // the thing we're looking for, by symbolic equivalence
            SymbolicExpression::Ptr found;              // the (first) place where we found something equivalent to the needle

            explicit Finder(const SymbolicExpression::Ptr &needle)
                : needle(needle) {}

            SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) override {
                if (!seen.insert(node->hash()).second) {
                    return SymbolicExpression::TERMINATE; // already searched here
                } else if (node->isEquivalentTo(needle)) {
                    found = node;
                    return SymbolicExpression::TERMINATE; // found, so no need to search further
                } else {
                    return SymbolicExpression::CONTINUE; // not found, so keep searching
                }
            }

            SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Ptr&) override {
                return SymbolicExpression::CONTINUE;
            }
        } finder(needle);
        haystack->depthFirstTraversal(finder);
        return finder.found != nullptr;
    }

    // Return a symbolic expression from a symbolic svalue if possible, otherwise nullptr.
    static SymbolicExpression::Ptr expr(const BS::SValue::Ptr &svalue) {
        if (const auto ssv = as<IS::SymbolicSemantics::SValue>(svalue)) {
            return ssv->get_expression();
        } else {
            return {};
        }
    }

    // Called when reading or writing to the memory state.
    void lookForVariable(const BS::SValue::Ptr &addrSVal, const Access access) {
        SgAsmInstruction *insn = currentInstruction();
        if (!insn)
            return;

        const SymbolicExpression::Ptr absAddr = SValue::promote(addrSVal)->get_expression();
        SAWYER_MESG(mlog[DEBUG]) <<"    memory address: " <<*absAddr <<"\n";

        // Compute a relative address by subtracting the initial stack pointer from when this RiscOperators object was created.
        // Equivalently, we can replace all occurrances of the original stack pointer with zero, which might be faster an more
        // easily simplified by the symbolic expression layer.
        if (findInExpr(expr(initialSp_), absAddr)) {
            const SymbolicExpression::Ptr zero = SymbolicExpression::makeIntegerConstant(initialSp_->nBits(), 0);
            const SymbolicExpression::Ptr relAddr = absAddr->substitute(expr(initialSp_), zero);
            SAWYER_MESG(mlog[DEBUG]) <<"    address w.r.t. initial stack pointer: " <<*relAddr <<"\n";
            if (const auto relAddrConcrete = relAddr->toSigned()) {
                StackVariable::insertBoundary(boundaries_, *relAddrConcrete, InstructionAccess(insn->get_address(), access));
                SAWYER_MESG(mlog[DEBUG]) <<"    concrete offset: " <<*relAddrConcrete <<"\n";
            }
        }
    }
};

} // namespace

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VariableFinder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VariableFinder::~VariableFinder() {}

VariableFinder::VariableFinder(const Settings &settings)
    : settings_(settings) {}

VariableFinder::Ptr
VariableFinder::instance(const Settings &settings) {
    return Ptr(new VariableFinder(settings));
}

Sawyer::CommandLine::SwitchGroup
VariableFinder::commandLineSwitches(Settings &settings) {
    using namespace Sawyer::CommandLine;

    SwitchGroup switches("Variable finder switches");
    switches.name("var");
    switches.doc("These switches control the analysis that finds local and/or global variables.");

    // The above switch group will be automatically suppressed in the man page until someone adds at least one switch here.

    return switches;
}

std::set<Address>
VariableFinder::findConstants(const SymbolicExpression::Ptr &expr) {
    struct: SymbolicExpression::Visitor {
        std::set<Address> constants;
        std::vector<SymbolicExpression::Ptr> path;

        SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) {
            const SymbolicExpression::Interior *parent = nullptr;
            if (!path.empty()) {
                parent = path.back()->isInteriorNodeRaw();
                ASSERT_not_null(parent);
            }
            path.push_back(node);

            // Some constants are never addresses
            if (parent && parent->getOperator() == SymbolicExpression::OP_EXTRACT &&
                (parent->child(0) == node || parent->child(1) == node))
                return SymbolicExpression::CONTINUE;          // first two args of extract are never addresses

            // If we found a constant, perhaps treat it like an address
            if (Sawyer::Optional<uint64_t> n = node->toUnsigned())
                constants.insert(*n);

            return SymbolicExpression::CONTINUE;
        }

        SymbolicExpression::VisitAction postVisit(const SymbolicExpression::Ptr&) {
            ASSERT_forbid(path.empty());
            path.pop_back();
            return SymbolicExpression::CONTINUE;
        }
    } visitor;

    ASSERT_not_null(expr);
    expr->depthFirstTraversal(visitor);
    return visitor.constants;
}

std::set<Address>
VariableFinder::findConstants(SgAsmInstruction *insn) {
    std::set<Address> constants;
    AST::Traversal::forwardPre<SgAsmIntegerValueExpression>(insn, [&constants](SgAsmIntegerValueExpression *ival) {
        constants.insert(ival->get_absoluteValue());
    });
    return constants;
}

std::set<Address>
VariableFinder::findAddressConstants(const BS::MemoryCellState::Ptr &mem) {
    std::set<SymbolicExpression::Ptr> addresses = getMemoryAddresses(mem);
    std::set<Address> retval;
    for (const SymbolicExpression::Ptr &address: addresses) {
        std::set<Address> constants = findConstants(address);
        retval.insert(constants.begin(), constants.end());
    }
    return retval;
}

bool
VariableFinder::regionContainsInstructions(const P2::Partitioner::ConstPtr &partitioner, const AddressInterval &region) {
    ASSERT_not_null(partitioner);
    return !partitioner->instructionsOverlapping(region).empty();
}

bool
VariableFinder::regionIsFullyMapped(const P2::Partitioner::ConstPtr &partitioner, const AddressInterval &region) {
    ASSERT_not_null(partitioner);
    if (MemoryMap::Ptr map = partitioner->memoryMap()) {
        return !map->findFreeSpace(1, 1, region);
    } else {
        return false;
    }
}

bool
VariableFinder::regionIsFullyReadWrite(const P2::Partitioner::ConstPtr &partitioner, const AddressInterval &region) {
    ASSERT_not_null(partitioner);
    if (MemoryMap::Ptr map = partitioner->memoryMap()) {
        AddressInterval where = map->at(region).require(MemoryMap::READ_WRITE).available();
        return (where & region) == region;
    } else {
        return false;
    }
}

void
VariableFinder::merge(AddressToAddresses &result, const AddressToAddresses &src) {
    for (const auto &node: src)
        result[node.first].insert(node.second);
}

std::set<SymbolicExpression::Ptr>
VariableFinder::getMemoryAddresses(const BS::MemoryCellState::Ptr &mem) {
    struct: BS::MemoryCell::Visitor {
        std::set<SymbolicExpression::Ptr> addresses;

        void operator()(BS::MemoryCell::Ptr &cell) {
            SymbolicExpression::Ptr addr = IS::SymbolicSemantics::SValue::promote(cell->address())->get_expression();
            addresses.insert(addr);
        }
    } visitor;

    ASSERT_not_null(mem);
    mem->traverse(visitor);
    return visitor.addresses;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VariableFinder - finding stack variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

StackFrame
VariableFinder::detectFrameAttributes(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function,
                                      StackVariable::Boundaries &boundaries /*in,out*/) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    SAWYER_MESG(debug) <<"detect frame attributes for " <<function->printableName() <<"\n";

    const StackDelta::Analysis &stackDeltas = function->stackDeltaAnalysis();
    SAWYER_MESG(debug) <<"  consistent frame pointer? " <<(stackDeltas.hasConsistentFramePointer() ? "yes" : "no") <<"\n";

    SgAsmInstruction *firstInsn = partitioner->instructionProvider()[function->address()];
    const size_t bytesPerWord = partitioner->architecture()->bytesPerWord();
    StackFrame frame;

    if (isSgAsmX86Instruction(firstInsn)) {
        //-------------+-------------+---------------------------+---------------------------------------------------
        // Stack ptr   | Frame ptr   |  Stack contents           | Notes
        // word offset | word offset |                           |
        //-------------+-------------+---------------------------+---------------------------------------------------
        //
        //                           :                           :
        //                           :   (part of parent frame)  :
        //                           :                           :
        //         +1    +1          | callee's actual arguments | Variable size, 1st arg at lowest address
        //                           +---(current frame)---------+
        //          0     0          | return address            | 1 word
        //         -1    -1          | addr of parent frame      | 1 word
        //         -2    -2          | callee saved registers    | Optional, variable size, multiple of word size
        //                           :                           :
        //                           :                           :
        //
        //-------------+-------------+---------------------------+---------------------------------------------------

        frame.growthDirection = StackFrame::GROWS_DOWN;
        frame.maxOffset = bytesPerWord-1;               // top of frame
        frame.minOffset = Sawyer::Nothing();            // bottom of frame is unknown
        frame.size = Sawyer::Nothing();                 // frame size is unknown
        frame.rule = "x86: general";

        StackVariable::Boundary &returnPtr = StackVariable::insertBoundaryImplied(boundaries, 0);
        returnPtr.purpose = StackVariable::Purpose::RETURN_ADDRESS;

        StackVariable::Boundary &parentPtr = StackVariable::insertBoundaryImplied(boundaries, -bytesPerWord);
        parentPtr.purpose = StackVariable::Purpose::FRAME_POINTER;

    } else if (isSgAsmPowerpcInstruction(firstInsn)) {
        //-------------+-------------+---------------------------+---------------------------------------------------
        // Stack ptr   | Frame ptr   |  Stack contents           | Notes
        // word offset | word offset |                           |
        //-------------+-------------+---------------------------+---------------------------------------------------
        // From documentation for PPC calling conventions:
        //                           :                           :
        //                           :   (part of parent frame)  :
        //                           :                           :
        //        +1      ?          | LR saved                  |  1 word
        //         0      ?          | addr of grandparent frame |  1 word Caller's frame pointer points here
        //                           +---(current frame)---------+
        //         ?      ?          | saved FP register area    |  optional, variable size
        //         ?      ?          | saved GP register area    |  optional, multiple of 4/8 bytes
        //         ?      ?          | CR saved                  |  0 or 1 word
        //         ?      ?          | Local variables           |  optional, variable size
        //         ?      ?          | Function parameter area   |  optional, variable size for callee args not fitting in registers
        //         ?     +2          | Padding                   |  0 to 7 bytes, although I'm not sure when this is used
        //         ?     +1          | LR saved by callees       |  1 word
        //         ?      0          | addr of parent frame      |  1 word callees's frame pointer points here
        //                           +---------------------------+
        //
        // From experience with GCC PowerPC-32 big-endian:
        //                           +---(current frame)---------+
        //         -1     ?          | saved link register       |
        //          ?     ?          :                           :
        //          ?     ?          :                           :
        //          ?     0          | saved stack pointer       |
        //                           +---------------------------+
        //
        //-------------+-------------+---------------------------+---------------------------------------------------
        //
        frame.growthDirection = StackFrame::GROWS_DOWN;

        // If this function starts with "stwu r1, u32 [r1 - N]" then the frame size is N.
        if (isSgAsmPowerpcInstruction(firstInsn)->get_kind() == powerpc_stwu) {
            static RegisterDescriptor REG_R1;
            if (REG_R1.isEmpty())
                REG_R1 = partitioner->architecture()->registerDictionary()->findOrThrow("r1");
            SgAsmDirectRegisterExpression *firstRegister = isSgAsmDirectRegisterExpression(firstInsn->operand(0));
            SgAsmMemoryReferenceExpression *secondArg = isSgAsmMemoryReferenceExpression(firstInsn->operand(1));
            SgAsmBinaryAdd *memAddr = secondArg ? isSgAsmBinaryAdd(secondArg->get_address()) : NULL;
            SgAsmDirectRegisterExpression *secondRegister = memAddr ? isSgAsmDirectRegisterExpression(memAddr->get_lhs()) : NULL;
            SgAsmIntegerValueExpression *constant = memAddr ? isSgAsmIntegerValueExpression(memAddr->get_rhs()) : NULL;
            int64_t n = constant ? constant->get_signedValue() : int64_t(0);
            if (firstRegister && secondRegister && constant && n < 0 &&
                firstRegister->get_descriptor() == REG_R1 && secondRegister->get_descriptor() == REG_R1) {
                frame.size = -n;
                frame.maxOffset = -1;
                frame.minOffset = -*frame.size;
                frame.rule = "ppc: stwu r1, u32 [r1 - N]";
            }

        } else {
            frame.size = 8;
            frame.maxOffset = -1;
            frame.minOffset = -8;
            frame.rule = "ppc: general";
        }

        if (frame.minOffset) {
            // Saved link register is at the top of the frame
            StackVariable::Boundary &parentPtr = StackVariable::insertBoundaryImplied(boundaries, -bytesPerWord);
            parentPtr.purpose = StackVariable::Purpose::FRAME_POINTER;

            // Saved stack register is at the bottom of the frame
            StackVariable::Boundary &returnPtr = StackVariable::insertBoundaryImplied(boundaries, *frame.minOffset);
            returnPtr.purpose = StackVariable::Purpose::STACK_POINTER;
        }
    } else if (auto m68k = isSgAsmM68kInstruction(firstInsn)) {
        //-------------+-------------+---------------------------+---------------------------------------------------
        // Stack ptr   | Frame ptr   |  Stack contents           | Notes
        // word offset | word offset |                           |
        //-------------+-------------+---------------------------+---------------------------------------------------
        //
        //                           :                           :
        //                           :   (part of parent frame)  :
        //                           :                           :
        //                           +---(current frame)---------+
        //          0            +1  | return address            | 4 bytes
        //         -1             0  | addr of parent frame      | 4 bytes
        //                           :                           :
        //
        //-------------+-------------+---------------------------+---------------------------------------------------
        frame.growthDirection = StackFrame::GROWS_DOWN;

        // If this m68k function starts with "link.w fp, -N" then the frame size is 4 + 4 + N since the caller has pushed the
        // return address and this function pushes the old frame pointer register r6 and then reserves N additional bytes in
        // the frame.
        const RegisterDescriptor REG_FP = partitioner->architecture()->registerDictionary()->stackFrameRegister();
        if (REG_FP &&
            m68k->get_kind() == m68k_link &&
            m68k->nOperands() == 2 &&
            isSgAsmDirectRegisterExpression(m68k->operand(0)) &&
            isSgAsmDirectRegisterExpression(m68k->operand(0))->get_descriptor() == REG_FP &&
            isSgAsmIntegerValueExpression(m68k->operand(1))) {
            int64_t n = isSgAsmIntegerValueExpression(m68k->operand(1))->get_signedValue();
            ASSERT_require(n <= 0);
            frame.size = 4 /*pushed frame pointer*/ + (-n);
            frame.maxOffset = 3;
            frame.minOffset = n - 4;
            frame.rule = "m68k: link a6, N";
        } else {
            frame.maxOffset = 3;
            frame.rule = "m68k: general";
        }

        StackVariable::Boundary &parentPtr = StackVariable::insertBoundaryImplied(boundaries, -bytesPerWord);
        parentPtr.purpose = StackVariable::Purpose::FRAME_POINTER;

        StackVariable::Boundary &returnPtr = StackVariable::insertBoundaryImplied(boundaries, 0);
        returnPtr.purpose = StackVariable::Purpose::RETURN_ADDRESS;

        if (frame.minOffset && *frame.minOffset < 0) {
            StackVariable::Boundary &bottom = StackVariable::insertBoundaryImplied(boundaries, *frame.minOffset);
            bottom.purpose = StackVariable::Purpose::UNKNOWN;
        }

#ifdef ROSE_ENABLE_ASM_AARCH32
    } else if (isSgAsmAarch32Instruction(firstInsn)) {
        //-------------+-------------+---------------------------+---------------------------------------------------
        // Stack ptr   | Frame ptr   |  Stack contents           | Notes
        // word offset | word offset |                           |
        //-------------+-------------+---------------------------+---------------------------------------------------
        //
        //                           :                           :
        //                           :   (part of parent frame)  :
        //          0                :                           :
        //                           +---(current frame)---------+
        //         -1             0  | addr of parent frame      | 4 bytes
        //         -2            -1  | saved link register       | optional, 4 bytes
        //         -3            -2  | local variables           | variable size
        //                           +---------------------------+
        frame.growthDirection = StackFrame::GROWS_DOWN;

        const RegisterDescriptor REG_FP = partitioner->architecture()->registerDictionary()->findOrThrow("fp");
        const RegisterDescriptor REG_SP = partitioner->architecture()->registerDictionary()->findOrThrow("sp");
        const RegisterDescriptor REG_LR = partitioner->architecture()->registerDictionary()->findOrThrow("lr");

        // If the first three instructions are:
        //   push fp, lr
        //   add fp, sp, 4
        //   sub sp, N
        // then the frame size is N + 8 bytes
        SgAsmAarch32Instruction* insns[3] = {nullptr, nullptr, nullptr};
        insns[0] = isSgAsmAarch32Instruction(firstInsn);
        insns[1] = isSgAsmAarch32Instruction(partitioner->instructionProvider()[function->address() + 4]);
        insns[2] = isSgAsmAarch32Instruction(partitioner->instructionProvider()[function->address() + 8]);
        if (// push fp, lr
            insns[0] && insns[0]->get_kind() == Aarch32InstructionKind::ARM_INS_PUSH &&
            insns[0]->nOperands() == 2 &&
            isSgAsmDirectRegisterExpression(insns[0]->operand(0)) &&
            isSgAsmDirectRegisterExpression(insns[0]->operand(0))->get_descriptor() == REG_FP &&
            isSgAsmDirectRegisterExpression(insns[0]->operand(1)) &&
            isSgAsmDirectRegisterExpression(insns[0]->operand(1))->get_descriptor() == REG_LR &&
            // add fp, sp, 4
            insns[1] && insns[1]->get_kind() == Aarch32InstructionKind::ARM_INS_ADD &&
            insns[1]->nOperands() == 3 &&
            isSgAsmDirectRegisterExpression(insns[1]->operand(0)) &&
            isSgAsmDirectRegisterExpression(insns[1]->operand(0))->get_descriptor() == REG_FP &&
            isSgAsmDirectRegisterExpression(insns[1]->operand(1)) &&
            isSgAsmDirectRegisterExpression(insns[1]->operand(1))->get_descriptor() == REG_SP &&
            isSgAsmIntegerValueExpression(insns[1]->operand(2)) &&
            isSgAsmIntegerValueExpression(insns[1]->operand(2))->get_absoluteValue() == 4 &&
            // sub sp, sp, N
            insns[2] && insns[2]->get_kind() == Aarch32InstructionKind::ARM_INS_SUB &&
            insns[2]->nOperands() == 3 &&
            isSgAsmDirectRegisterExpression(insns[2]->operand(0)) &&
            isSgAsmDirectRegisterExpression(insns[2]->operand(0))->get_descriptor() == REG_SP &&
            isSgAsmDirectRegisterExpression(insns[2]->operand(1)) &&
            isSgAsmDirectRegisterExpression(insns[2]->operand(1))->get_descriptor() == REG_SP &&
            isSgAsmIntegerValueExpression(insns[2]->operand(2))) {
            // The frame pointer will point to one past the top (highest address) of the frame
            int64_t n = boost::numeric_cast<int64_t>(isSgAsmIntegerValueExpression(insns[2]->operand(2))->get_absoluteValue());
            frame.size = n + 8;
            frame.maxOffset = -1;
            frame.minOffset = -(n + 4);                 // entire frame except the lr_0 which appears on the stack at the fp
            // Note that the string "<saved-lr>" is important and used by initializeFrameBoundaries
            frame.rule = "aarch32: push fp, lr <saved-lr>; add fp, sp, 4; sub sp N";

        } else if (// str fp, u32 [sp (after sp -= 4)]
                   insns[0] &&
                   insns[0]->get_kind() == Aarch32InstructionKind::ARM_INS_STR &&
                   insns[0]->nOperands() == 2 &&
                   isSgAsmDirectRegisterExpression(insns[0]->operand(0)) &&
                   isSgAsmDirectRegisterExpression(insns[0]->operand(0))->get_descriptor() == REG_FP &&
                   isSgAsmMemoryReferenceExpression(insns[0]->operand(1)) &&
                   isSgAsmBinaryPreupdate(isSgAsmMemoryReferenceExpression(insns[0]->operand(1))->get_address()) &&
                   isSgAsmDirectRegisterExpression(isSgAsmBinaryPreupdate(isSgAsmMemoryReferenceExpression(insns[0]->operand(1))->get_address())->get_lhs()) &&
                   isSgAsmDirectRegisterExpression(isSgAsmBinaryPreupdate(isSgAsmMemoryReferenceExpression(insns[0]->operand(1))->get_address())->get_lhs())->get_descriptor() == REG_SP &&
                   // add fp, sp, 0
                   insns[1] &&
                   insns[1]->get_kind() == Aarch32InstructionKind::ARM_INS_ADD &&
                   insns[1]->nOperands() == 3 &&
                   isSgAsmDirectRegisterExpression(insns[1]->operand(0)) &&
                   isSgAsmDirectRegisterExpression(insns[1]->operand(0))->get_descriptor() == REG_FP &&
                   isSgAsmDirectRegisterExpression(insns[1]->operand(1)) &&
                   isSgAsmDirectRegisterExpression(insns[1]->operand(1))->get_descriptor() == REG_SP &&
                   isSgAsmIntegerValueExpression(insns[1]->operand(2)) &&
                   isSgAsmIntegerValueExpression(insns[1]->operand(2))->get_absoluteValue() == 0 &&
                   // sub sp, sp, N
                   insns[2] &&
                   insns[2]->get_kind() == Aarch32InstructionKind::ARM_INS_SUB &&
                   insns[2]->nOperands() == 3 &&
                   isSgAsmDirectRegisterExpression(insns[2]->operand(0)) &&
                   isSgAsmDirectRegisterExpression(insns[2]->operand(0))->get_descriptor() == REG_SP &&
                   isSgAsmDirectRegisterExpression(insns[2]->operand(1)) &&
                   isSgAsmDirectRegisterExpression(insns[2]->operand(1))->get_descriptor() == REG_SP &&
                   isSgAsmIntegerValueExpression(insns[2]->operand(2))) {
            int64_t n = boost::numeric_cast<int64_t>(isSgAsmIntegerValueExpression(insns[2]->operand(2))->get_absoluteValue());
            frame.size = n + 4;
            frame.maxOffset = -1;
            frame.minOffset = -(n + 4);
            frame.rule = "aarch32: str fp, u32 [sp (after sp -= 4)]; add fp, sp, 0; sub sp, sp N";

        } else {
            frame.size = 8;
            frame.maxOffset = -1;
            frame.minOffset = -8;
            frame.rule = "aarch32: general";
        }

        if (boost::contains(frame.rule, "<saved-lr>")) {
            // The "push fp, lr" pushes 8 bytes calculated from the stack pointer, so we don't want this eight
            // bytes to cross a frame variable boundary.
            StackVariable::Boundary &returnPtr = StackVariable::insertBoundaryImplied(boundaries, -4);
            returnPtr.purpose = StackVariable::Purpose::RETURN_ADDRESS;
        } else {
            StackVariable::Boundary &parentPtr = StackVariable::insertBoundaryImplied(boundaries, 0);
            parentPtr.purpose = StackVariable::Purpose::FRAME_POINTER;
        }
#endif
    }

    if (debug) {
        debug <<"  frame initialization rule: architecture \"" <<StringUtility::cEscape(partitioner->architecture()->name()) <<"\""
              <<", rule ";
        if (!frame.rule.empty()) {
            debug <<"\"" <<StringUtility::cEscape(frame.rule) <<"\"\n";
        } else {
            debug <<"unspecified\n";
        }

        switch (frame.growthDirection) {
            case StackFrame::GROWS_UP:
                debug <<"  stack direction: grows up\n";
                break;
            case StackFrame::GROWS_DOWN:
                debug <<"  stack direction: grows down\n";
                break;
        }

        debug <<"  frame boundaries w.r.t initial function SP:\n";
        debug <<"    maximum: ";
        if (frame.maxOffset) {
            debug <<*frame.maxOffset <<"\n";
        } else {
            debug <<"unknown\n";
        }
        debug <<"    mininum: ";
        if (frame.minOffset) {
            debug <<*frame.minOffset <<"\n";
        } else {
            debug <<"unknown\n";
        }

        debug <<"  frame size: ";
        if (frame.size) {
            debug <<StringUtility::plural(*frame.size, "bytes") <<"\n";
        } else {
            debug <<"unknown\n";
        }

        debug <<"  boundaries:\n";
        for (const StackVariable::Boundary &boundary: boundaries)
            StackVariable::printBoundary(debug, boundary, "    ");
    }

    return frame;
}

void
VariableFinder::removeOutliers(const StackFrame &frame, StackVariable::Boundaries &boundaries /*in,out,sorted*/) {
#ifndef NDEBUG
    for (size_t i = 1; i < boundaries.size(); ++i)
        ASSERT_require(boundaries[i-1].stackOffset < boundaries[i].stackOffset);
#endif

    if (frame.minOffset) {
        // Remove all boundaries that are below the bottom of the frame, but don't yet remove the greatest one that's below the
        // frame.
        size_t i = 0;
        while (i+1 < boundaries.size() && boundaries[i+1].stackOffset < *frame.minOffset)
            ++i;
        boundaries.erase(boundaries.begin(), boundaries.begin() + i);

        // Adjust the lowest bounded area so it starts at the start of the frame unless there's already a boundary there (in which
        // case, remove it instead of moving it).
        if (!boundaries.empty() && boundaries[0].stackOffset < *frame.minOffset) {
            if (boundaries.size() == 1 || boundaries[1].stackOffset > *frame.minOffset) {
                boundaries[0].stackOffset = *frame.minOffset;
            } else {
                boundaries.erase(boundaries.begin());
            }
        }
    }

    if (frame.maxOffset) {
        // Remove all boundaries that start above the top of the frame.
        while (!boundaries.empty() && boundaries.back().stackOffset > *frame.maxOffset)
            boundaries.pop_back();
    }
}

StackVariables
VariableFinder::findStackVariables(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);

    // Return cached local variable information if available.
    if (function->attributeExists(ATTR_LOCAL_VARS))
        return function->getAttribute<StackVariables>(ATTR_LOCAL_VARS);

    Sawyer::Message::Stream debug(mlog[DEBUG]);
    StackVariable::Boundaries boundaries;
    const StackFrame frame = detectFrameAttributes(partitioner, function, boundaries /*in,out*/);

    // Look for stack offsets semantically. We could do a complete data-flow analysis, but it turns out that processing each basic
    // block independently is usually just as good, and much faster.
    SAWYER_MESG(debug) <<"searching for local variables semantically in " <<function->printableName() <<"\n";
    const StackDelta::Analysis &stackDeltas = function->stackDeltaAnalysis();
    if (!stackDeltas.hasResults()) {
        SAWYER_MESG(mlog[WARN]) <<"no stack delta information; variable synthesis skipped for " <<function->printableName() <<"\n";
        return {};
    }
    for (Address bblockVa: function->basicBlockAddresses()) {
        P2::BasicBlock::Ptr bb = partitioner->basicBlockExists(bblockVa);
        ASSERT_not_null(bb);
        auto ops = LVarSemantics::RiscOperators::instance(partitioner, function->stackDeltaAnalysis(), boundaries /*in,out*/);
        BS::Dispatcher::Ptr cpu = partitioner->newDispatcher(ops);
        for (SgAsmInstruction *insn: bb->instructions()) {
            try {
                cpu->processInstruction(insn);
            } catch (const BS::Exception &e) {
                if (!partitioner->architecture()->isUnknown(insn))
                    debug <<"  semantic failure for " <<insn->toString() <<": " <<e.what() <<"\n";
                break;
            } catch (...) {
                if (!partitioner->architecture()->isUnknown(insn))
                    debug <<"  semantic failure for " <<insn->toString() <<"\n";
            }
        }
    }

    // Sort and prune the boundaries so we have just those that are in the frame.
    std::sort(boundaries.begin(), boundaries.end(), [](const StackVariable::Boundary &a, const StackVariable::Boundary &b) {
        return a.stackOffset < b.stackOffset;
    });
    if (debug) {
        debug <<"  sorted boundary list:\n";
        for (const StackVariable::Boundary &boundary: boundaries)
            StackVariable::printBoundary(debug, boundary, "    ");
    }
    removeOutliers(frame, boundaries);
    if (debug) {
        debug <<"  boundary list after pruning:\n";
        for (const StackVariable::Boundary &boundary: boundaries)
            StackVariable::printBoundary(debug, boundary, "    ");
    }

    // Now that we know the boundaries between parts of the frame, create the variables that live between those boundaries.
    StackVariables lvars;
    for (size_t i = 0; i < boundaries.size(); ++i) {
        const StackVariable::Boundary &boundary = boundaries[i];

        // We know the low offset of this variable, but what is the high offset.
        int64_t maxOffset = boundary.stackOffset;
        if (i + 1 < boundaries.size()) {
            maxOffset = boundaries[i+1].stackOffset - 1;
        } else if (frame.maxOffset) {
            maxOffset = *frame.maxOffset;
        } else {
            maxOffset = (int64_t)BitOps::lowMask<uint64_t>(partitioner->architecture()->bitsPerWord() - 1);
        }

        // Create the variable
        ASSERT_require2(maxOffset >= boundary.stackOffset,
                        "maxOffset=" + boost::lexical_cast<std::string>(maxOffset) +
                        ", boundary.stackOffset=" + boost::lexical_cast<std::string>(boundary.stackOffset));
        const OffsetInterval where = OffsetInterval::hull(boundary.stackOffset, maxOffset);
        Address varMaxSize = (uint64_t)maxOffset + 1u - (uint64_t)boundary.stackOffset;
        StackVariable lvar(function, boundary.stackOffset, varMaxSize, boundary.purpose, boundary.definingInsns);
        lvar.setDefaultName();
        lvars.insert(where, lvar);
    }

    if (debug) {
        debug <<"  local variables:\n";
        print(lvars, partitioner, debug, "    ");
    }
    function->setAttribute(ATTR_LOCAL_VARS, lvars);
    return lvars;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VariableFinder - finding global variables
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class FollowNamedEdges: public P2::DataFlow::InterproceduralPredicate {
    std::regex functionNameRe;

public:
    explicit FollowNamedEdges(const std::string &re)
        : functionNameRe(re) {}

public:
    virtual bool operator()(const P2::ControlFlowGraph&, const P2::ControlFlowGraph::ConstEdgeIterator &edge, size_t /*depth*/) {
        const P2::FunctionSet &functions = edge->target()->value().owningFunctions();
        for (const P2::Function::Ptr &function: functions.values()) {
            const std::string name = function->name();
            if (std::regex_match(name, functionNameRe))
                return true;
        }
        return false;
    }
};

// For x86 ELF executables, global variables live in a particular section, such as .bss. The compiler typically generates code that
// computes the global variable address as an offset from the function's own address. It does this by calling __x86.get_pc_thunk.ax
// (or similar) which returns the address of the instruction following the call, and then adding a constant to the returned
// value. This computes the starting address of the section (base address), to which additional offsets are added to get to the
// various global variables.  Since the base address is computed in one basic block and used in subsequent basic blocks, we use a
// data flow analysis to propagate the base address to the other blocks.
struct FindGlobalVariableVasMethod1Worker {
    P2::Partitioner::ConstPtr partitioner;
    Sawyer::ProgressBar<size_t> &progress;
    VariableFinder &self;

    // The following data members are synchronized with this mutex
    SAWYER_THREAD_TRAITS::Mutex mutex_;
    AddressToAddresses result;

    explicit FindGlobalVariableVasMethod1Worker(const P2::Partitioner::ConstPtr &partitioner, Sawyer::ProgressBar<size_t> &progress,
                                                VariableFinder &self)
        : partitioner(partitioner), progress(progress), self(self) {
        ASSERT_not_null(partitioner);
    }

    void operator()(size_t /*workId*/, const P2::Function::Ptr &function) {
        ++progress;
        using Df = P2::DataFlow::Engine;
        using Cfg = Df::Cfg;

        Sawyer::Message::Stream debug(mlog[DEBUG]);
        SAWYER_MESG(debug) <<"  analyzing " <<function->printableName() <<"\n";

        // Create the data-flow CFG
        P2::ControlFlowGraph::ConstVertexIterator cfgRoot = partitioner->findPlaceholder(function->address());
        ASSERT_require(cfgRoot != partitioner->cfg().vertices().end());
        FollowNamedEdges predicate(".+\\.get_pc_thunk\\..+");
        Cfg cfg = P2::DataFlow::buildDfCfg(partitioner, partitioner->cfg(), cfgRoot, predicate);

        // Create the data-flow transfer and merge functors
        BS::Dispatcher::Ptr cpu = partitioner->newDispatcher(partitioner->newOperators());
        Df::TransferFunction xfer(cpu);
        xfer.ignoringSemanticFailures(true);
        Df::MergeFunction merger(cpu);

        // Create the initial state, and cause it to use a simple merge function that doesn't account for aliasing
        BS::State::Ptr initialState = xfer.initialState();
        initialState->memoryState()->merger(BS::Merger::instance());
        initialState->memoryState()->merger()->memoryAddressesMayAlias(false);

        // Run the dataflow with a low number of iterations
        Df df(cfg, xfer, merger);
        df.insertStartingVertex(0, initialState);

        Sawyer::Stopwatch timer;
        const size_t maxSteps = cfg.nVertices() * 3 + 1; // arbitrary, but positive
        for (size_t i = 0; i < maxSteps; ++i) {
            if (!df.runOneIteration())
                break;
            if (timer.report() > self.settings().gvarMethod1MaxTimePerFunction.count()) {
                mlog[WARN] <<"max time exceeded for gvar method 1 for " <<function->printableName()
                           <<(boost::format(" (%d of %d steps; %1.0f%%)") % i % maxSteps % (100.0 * i / maxSteps)) <<"\n";
                break;
            }
        }

        // Find all constants that appear in memory address expressions and organize them according to what instruction(s) wrote to
        // that address. We could organize by memory reads also, but alas, the memory state does not track readers--only writers.
        using Map = Sawyer::Container::IntervalSetMap<AddressInterval, Sawyer::Container::Set<Address>>;
        struct Accumulator: BS::MemoryCell::Visitor {
            VariableFinder &variableFinder;
            Sawyer::Message::Stream &debug;
            Map map;
            Accumulator(VariableFinder &variableFinder, Sawyer::Message::Stream &debug)
                : variableFinder(variableFinder), debug(debug) {}
            void operator()(BS::MemoryCell::Ptr &cell) override {
                SymbolicExpression::Ptr address = IS::SymbolicSemantics::SValue::promote(cell->address())->get_expression();
                if (debug) {
                    debug <<"      memory address: " <<*address <<"\n"
                          <<"        writers = {";
                    for (Address va: cell->getWriters().values())
                        debug <<" " <<StringUtility::addrToString(va);
                    debug <<" }\n";
                }
                std::set<Address> constants = variableFinder.findConstants(address);
                for (Address c: constants) {
                    SAWYER_MESG(debug) <<"        constant = " <<StringUtility::addrToString(c) <<"\n";
                    map.insert(c, cell->getWriters());
                }
            }
        };

        Accumulator accumulator(self, debug);
        for (size_t i = 0; i < cfg.nVertices(); ++i) {
            SAWYER_MESG(debug) <<"    final state for vertex " <<i <<": " <<cfg.findVertex(i)->value().toString() <<"\n";
            if (auto state = df.getFinalState(i)) {
                SAWYER_MESG(debug) <<(*state+"      ") <<"\n";
                auto mem = BS::MemoryCellState::promote(state->memoryState());
                mem->traverse(accumulator);
            }
        }

        // Compute the result for this single function
        AddressToAddresses functionResult;
        SAWYER_MESG(debug) <<"  summary for " <<function->printableName() <<":\n";
        for (const auto &node: accumulator.map.nodes()) {
            if (debug) {
                debug <<"    variable va: " <<StringUtility::addrToString(node.key()) <<"; insns:";
                for (Address va: node.value().values())
                    debug <<" " <<StringUtility::addrToString(va);
                debug <<"\n";
            }

            if (self.regionContainsInstructions(partitioner, node.key())) {
                SAWYER_MESG(debug) <<"      skipped because region overlaps with instructions\n";
                continue;
            }
            if (!self.regionIsFullyMapped(partitioner, node.key())) {
                SAWYER_MESG(debug) <<"      skipped because sub-region is not fully mapped\n";
                continue;
            }
            if (!self.regionIsFullyReadWrite(partitioner, node.key())) {
                SAWYER_MESG(debug) <<"      skipped because sub-region is not fully read+write\n";
                continue;
            }

            functionResult[node.key().least()].insert(node.value());
        }

        // Merge this function's result into the global result
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        for (const auto &node: functionResult)
            result[node.first] |= node.second;
    }
};

AddressToAddresses
VariableFinder::findGlobalVariableVasMethod1(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    AddressToAddresses retval;
    SAWYER_MESG(mlog[DEBUG]) <<"looking for global variable addresses (method 1: data-flow)\n";

    // It doesn't matter what order we process these functions because the worker is intraprocedural. Therefore, we don't need to
    // add any edges to this graph. However, we'll sort the vertices from small to large, and since the workInParallel runs them
    // in the opposite order, this results in the best overall parallel performance.
    Sawyer::Container::Graph<P2::Function::Ptr> g;
    std::vector<P2::Function::Ptr> functions = partitioner->functions();
    std::sort(functions.begin(), functions.end(), [](const P2::Function::Ptr &a, const P2::Function::Ptr &b) {
        return a->nBasicBlocks() < b->nBasicBlocks();
    });
    for (const P2::Function::Ptr &function: functions)
        g.insertVertex(function);

    // Process each function and accumulate the results in the worker.result data member
    Sawyer::ProgressBar<size_t> progress(g.nVertices(), mlog[MARCH], "gvars method 1");
    progress.suffix(" functions");
    const size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    FindGlobalVariableVasMethod1Worker worker(partitioner, progress, *this);
    Sawyer::workInParallel(g, nThreads, std::ref(worker));
    return worker.result;
}

struct FindGlobalVariableVasMethod2Worker {
    P2::Partitioner::ConstPtr partitioner;
    Sawyer::ProgressBar<size_t> &progress;
    VariableFinder &self;

    // The following data members are synchronized with this mutex
    SAWYER_THREAD_TRAITS::Mutex mutex_;
    AddressToAddresses result;

    FindGlobalVariableVasMethod2Worker(const P2::Partitioner::ConstPtr &partitioner, Sawyer::ProgressBar<size_t> &progress,
                                       VariableFinder &self)
        : partitioner(partitioner), progress(progress), self(self) {}

    void operator()(size_t /*workId*/, const P2::BasicBlock::Ptr &bb) {
        Sawyer::Message::Stream debug(mlog[DEBUG]);
        ++progress;
        AddressToAddresses partialResult;

        IS::SymbolicSemantics::RiscOperators::Ptr ops =
            IS::SymbolicSemantics::RiscOperators::instanceFromRegisters(partitioner->architecture()->registerDictionary());
        BS::Dispatcher::Ptr cpu = partitioner->newDispatcher(ops);
        ASSERT_not_null(cpu);

        for (SgAsmInstruction *insn: bb->instructions()) {
            SAWYER_MESG(debug) <<"  " <<insn->toString() <<"\n";
            ops->currentState()->clear();
            try {
                cpu->processInstruction(insn);
            } catch (...) {
                if (!partitioner->architecture()->isUnknown(insn))
                    SAWYER_MESG(mlog[WARN]) <<"semantics failed for " <<insn->toString() <<"\n";
            }

            // Find all constants that appear in memory address expressions.
            BS::MemoryCellState::Ptr mem = BS::MemoryCellState::promote(cpu->currentState()->memoryState());
            std::set<Address> constants = self.findAddressConstants(mem);

            // Compute the contiguous regions formed by those constants. E.g., an instruction that reads four bytes of memory
            // might have four consecutive constants.
            AddressIntervalSet regions;
            for (Address c: constants) {
                if (self.regionContainsInstructions(partitioner, c))
                    continue;                           // not a variable pointer if it points to an instruction
                if (!self.regionIsFullyMapped(partitioner, c))
                    continue;                           // global variables always have storage
                if (!self.regionIsFullyReadWrite(partitioner, c))
                    continue;
                regions.insert(c);
            }

            // Save only the lowest constant in each contiguous region.
            for (const AddressInterval &interval: regions.intervals()) {
                SAWYER_MESG(debug) <<"      " <<StringUtility::addrToString(interval.least()) <<"\n";
                partialResult[interval.least()].insert(insn->get_address());
            }
        }

        // Save the partial results in the worker
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        for (const auto &node: partialResult)
            result[node.first] |= node.second;
    }
};

AddressToAddresses
VariableFinder::findGlobalVariableVasMethod2(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    AddressToAddresses retval;
    SAWYER_MESG(mlog[DEBUG]) <<"looking for global variable addresses (method 2: insn memory address constants)\n";

    // Get all basic blocks and sort them by increasing number of instructions. This results in best parallelism since
    // workInParallel will run them in the opposite order.
    std::vector<P2::BasicBlock::Ptr> bblocks = partitioner->basicBlocks();
    std::sort(bblocks.begin(), bblocks.end(), [](const P2::BasicBlock::Ptr &a, const P2::BasicBlock::Ptr &b) {
        return a->nInstructions() < b->nInstructions();
    });

    // Basic blocks can be processed in any order, so create a work graph with no edges.
    Sawyer::Container::Graph<P2::BasicBlock::Ptr> g;
    for (const P2::BasicBlock::Ptr &b: bblocks)
        g.insertVertex(b);

    // Analyze basic blocks in parallel
    Sawyer::ProgressBar<size_t> progress(partitioner->cfg().nVertices(), mlog[MARCH], "gvars method 3");
    progress.suffix(" bblocks");
    FindGlobalVariableVasMethod2Worker worker(partitioner, progress, *this);
    const size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    Sawyer::workInParallel(g, nThreads, std::ref(worker));
    return worker.result;
}

struct FindGlobalVariableVasMethod3Worker {
    P2::Partitioner::ConstPtr partitioner;
    Sawyer::ProgressBar<size_t> &progress;
    VariableFinder &self;

    // The following data members are synchronized with this mutex
    SAWYER_THREAD_TRAITS::Mutex mutex_;
    AddressToAddresses result;

    FindGlobalVariableVasMethod3Worker(const P2::Partitioner::ConstPtr &partitioner, Sawyer::ProgressBar<size_t> &progress,
                                       VariableFinder &self)
        : partitioner(partitioner), progress(progress), self(self) {}

    void operator()(size_t /*workId*/, const P2::BasicBlock::Ptr &bb) {
        Sawyer::Message::Stream debug(mlog[DEBUG]);
        ++progress;
        AddressToAddresses partialResult;

        for (SgAsmInstruction *insn: bb->instructions()) {
            SAWYER_MESG(debug) <<"  " <<insn->toString() <<"\n";

            // Find constants in the instruction AST
            std::set<Address> constants = self.findConstants(insn);
            AddressIntervalSet regions;
            for (Address c: constants) {
                if (self.regionContainsInstructions(partitioner, c))
                    continue;                           // not a variable pointer if it points to an instruction
                if (!self.regionIsFullyMapped(partitioner, c))
                    continue;                           // global variables always have storage
                if (!self.regionIsFullyReadWrite(partitioner, c))
                    continue;
                regions.insert(c);
            }

            // Save only the lowest constant in each contiguous region.
            for (const AddressInterval &interval: regions.intervals()) {
                SAWYER_MESG(debug) <<"      " <<StringUtility::addrToString(interval.least()) <<"\n";
                partialResult[interval.least()].insert(insn->get_address());
            }
        }

        // Save the partial results in the worker
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex_);
        for (const auto &node: partialResult)
            result[node.first] |= node.second;
    }
};

AddressToAddresses
VariableFinder::findGlobalVariableVasMethod3(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    AddressToAddresses retval;
    SAWYER_MESG(mlog[DEBUG]) <<"looking for global variable addresses (method 2: insn immediate values)\n";

    // Get all basic blocks and sort them by increasing number of instructions. This results in best parallelism since the
    // workInParallel will run them in the opposite order.
    std::vector<P2::BasicBlock::Ptr> bblocks = partitioner->basicBlocks();
    std::sort(bblocks.begin(), bblocks.end(), [](const P2::BasicBlock::Ptr &a, const P2::BasicBlock::Ptr &b) {
        return a->nInstructions() < b->nInstructions();
    });

    // Basic blocks can be processed in any order, so create a work graph with no edges.
    Sawyer::Container::Graph<P2::BasicBlock::Ptr> g;
    for (const P2::BasicBlock::Ptr &b: bblocks)
        g.insertVertex(b);

    // Analyze basic blocks in parallel
    Sawyer::ProgressBar<size_t> progress(partitioner->cfg().nVertices(), mlog[MARCH], "gvars method 3");
    progress.suffix(" bblocks");
    FindGlobalVariableVasMethod3Worker worker(partitioner, progress, *this);
    const size_t nThreads = Rose::CommandLine::genericSwitchArgs.threads;
    Sawyer::workInParallel(g, nThreads, std::ref(worker));
    return worker.result;
}

AddressToAddresses
VariableFinder::findGlobalVariableVas(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);
    Sawyer::Message::Stream debug(mlog[DEBUG]);
    Sawyer::Message::Stream info(mlog[INFO]);
    info <<"Finding global variable addresses";
    Sawyer::Stopwatch timer;

    AddressToAddresses retval;
    merge(retval, findGlobalVariableVasMethod1(partitioner));
    merge(retval, findGlobalVariableVasMethod2(partitioner));
    merge(retval, findGlobalVariableVasMethod3(partitioner));

    info <<"; took " <<timer <<"\n";
    return retval;
}

GlobalVariables
VariableFinder::findGlobalVariables(const P2::Partitioner::ConstPtr &partitioner) {
    ASSERT_not_null(partitioner);

    // Return cached global variable information
    if (partitioner->attributeExists(ATTR_GLOBAL_VARS))
        return partitioner->getAttribute<GlobalVariables>(ATTR_GLOBAL_VARS);

    // First, find all the global variables assume they're as large as possible but no global variables overlap.
    typedef Sawyer::Container::IntervalMap<AddressInterval /*occupiedVas*/, Address /*startingVa*/> GVars;
    GVars gvars;
    AddressToAddresses globalVariableVas = findGlobalVariableVas(partitioner);
    static size_t wordSize = partitioner->architecture()->bitsPerWord();
    for (AddressToAddresses::const_iterator iter = globalVariableVas.begin(); iter != globalVariableVas.end(); ++iter) {

        // This variable cannot overlap with the next variable
        AddressToAddresses::const_iterator next = iter; ++next;
        Address lastVa = next == globalVariableVas.end() ? BitOps::lowMask<Address>(wordSize) : next->first - 1;

        // The variable will not extend beyond one segment of the memory map
        MemoryMap::ConstNodeIterator node = partitioner->memoryMap()->at(iter->first).findNode();
        ASSERT_require2(node != partitioner->memoryMap()->nodes().end(), StringUtility::addrToString(iter->first));
        lastVa = std::min(lastVa, node->key().greatest());

        gvars.insert(AddressInterval::hull(iter->first, lastVa), iter->first);
    }

    // Then remove code areas from the global variable location map
    for (const P2::ControlFlowGraph::Vertex &vertex: partitioner->cfg().vertices()) {
        if (vertex.value().type() == P2::V_BASIC_BLOCK) {
            for (SgAsmInstruction *insn: vertex.value().bblock()->instructions())
                gvars.erase(AddressInterval::baseSize(insn->get_address(), insn->get_size()));
        }
    }

    // Finally build the return value
    GlobalVariables retval;
    for (const GVars::Node &node: gvars.nodes()) {
        if (node.key().least() == node.value()) {
            std::vector<InstructionAccess> insns;
            insns.reserve(globalVariableVas[node.value()].size());
            for (const Address insnAddr: globalVariableVas[node.value()].values())
                insns.push_back(InstructionAccess(insnAddr, AccessFlags()));
            retval.insert(node.key(), GlobalVariable(node.key().least(), node.key().size(), insns));
        }
    }

    const_cast<P2::Partitioner*>(partitioner.getRawPointer())->setAttribute(ATTR_GLOBAL_VARS, retval);
    return retval;
}


} // namespace
} // namespace
} // namespace

#endif
