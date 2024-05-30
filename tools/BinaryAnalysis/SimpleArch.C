// Shows how to define a new architecture outside of the ROSE library
#include <rose.h>                                       // must be first

#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/CallingConvention/Definition.h>
#include <Rose/BinaryAnalysis/Disassembler/Base.h>
#include <Rose/BinaryAnalysis/Disassembler/Exception.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/Engine.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Modules.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/BinaryAnalysis/RegisterDictionary.h>
#include <Rose/BinaryAnalysis/Unparser/Base.h>
#include <Rose/Diagnostics.h>

// These are part of ROSE but don't follow the usual naming conventon (yet).
#include <SageBuilderAsm.h>
#include <rose_getline.h>

// Sawyer is distributed with ROSE
#include <Sawyer/Map.h>
#include <Sawyer/Message.h>
#include <Sawyer/Parse.h>

// Boost
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/lexical_cast.hpp>

// Others
#include <iostream>
#include <regex>
#include <string>
#include <vector>

using namespace Rose::BinaryAnalysis;
using namespace Sawyer::Message::Common;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;
namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;

// These are the instructions defined for this architecture.
enum class InsnKind {
    // Arithmetic
    ADD,                                                // addition
    SUB,                                                // subtraction
    MUL,                                                // multiplication
    DIV,                                                // division

    // Bit-wise ops
    SHL,                                                // shift left
    SHR,                                                // shift right, zero-inserting
    AND,                                                // bitwise AND
    OR,                                                 // bitwise OR
    XOR,                                                // bitwise XOR
    NOT,                                                // bitwise NOT

    // Branching
    CMP,                                                // compare
    JMP,                                                // unconditional branch
    BEQ,                                                // branch if ZF set
    BLT,                                                // branch if NF set
    CALL,                                               // call subroutine
    RET,                                                // return from subroutine

    // Memory
    PUSH,                                               // push onto stack
    POP,                                                // pop from stack
    READ,                                               // read from memory
    WRITE,                                              // write to memory

    // Misc
    MOV,                                                // move data from register to register
    LOAD,                                               // load literal value into register
    HLT,                                                // halt
    UNKNOWN                                             // not a valid instruction
};

static Sawyer::Message::Facility mlog;

// Make a register dictionary that gives names to each register.
static RegisterDictionary::Ptr
makeRegisterDictionary(const Architecture::Base::ConstPtr &arch) {
    ASSERT_not_null(arch);

    auto regs = RegisterDictionary::instance(arch->name());

    // 256 16-bit general-purpose registers named "rN"
    for (size_t i = 0; i < 256; ++i)
        regs->insert("r" + boost::lexical_cast<std::string>(i), 0, i, 0, 16);

    // Some registers have special names
    regs->insert("pc", 0, 255, 0, 16);                  // same as r255
    regs->insert("sp", 0, 254, 0, 16);                  // same as r254
    regs->insert("fp", 0, 253, 0, 16);                  // same as r253
    regs->insert("lr", 0, 252, 0, 16);                  // same as r252
    regs->insert("status", 0, 251, 0, 16);              // same as r251
    regs->insert("zf", 0, 251, 0, 1);                   // zero flag: bit 0 of "status"
    regs->insert("nf", 0, 251, 1, 1);                   // negative: bit 1 of "status"

    // Some registers have special purposes
    regs->instructionPointerRegister("pc");
    regs->stackPointerRegister("sp");
    regs->stackFrameRegister("fp");
    regs->callReturnRegister("lr");

    return regs;
}

// Make the calling convention definitions. Our architecture has only one.
static CallingConvention::Dictionary
makeCallingConventions(const Architecture::Base::ConstPtr &arch) {
    ASSERT_not_null(arch);

    RegisterDictionary::Ptr regdict = arch->registerDictionary();
    auto cc = CallingConvention::Definition::instance("simple", "simple", arch);

    // Address locations
    cc->instructionPointerRegister(regdict->instructionPointerRegister());
    cc->returnAddressLocation(ConcreteLocation(regdict->callReturnRegister(), regdict));

    // Stack characteristics
    cc->stackPointerRegister(regdict->stackPointerRegister());
    cc->stackDirection(CallingConvention::StackDirection::GROWS_DOWN);
    cc->nonParameterStackSize(0);

    // The first eight function arguments are passed in registers 16-23
    cc->appendInputParameter(regdict->findOrThrow("r16"));
    cc->appendInputParameter(regdict->findOrThrow("r17"));
    cc->appendInputParameter(regdict->findOrThrow("r18"));
    cc->appendInputParameter(regdict->findOrThrow("r19"));
    cc->appendInputParameter(regdict->findOrThrow("r20"));
    cc->appendInputParameter(regdict->findOrThrow("r21"));
    cc->appendInputParameter(regdict->findOrThrow("r22"));
    cc->appendInputParameter(regdict->findOrThrow("r23"));

    // Arguments beyond the first eight are passed on the stack
    cc->stackParameterOrder(CallingConvention::StackParameterOrder::RIGHT_TO_LEFT);
    cc->stackCleanup(CallingConvention::StackCleanup::BY_CALLER);

    // Function return value is in r0
    cc->appendOutputParameter(regdict->findOrThrow("r0"));

    // Scratch registers that can be modified by the callee without the callee needing to restore them upon return
    cc->scratchRegisters().insert(regdict->findOrThrow("r24"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r25"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r26"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r27"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r28"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r29"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r30"));
    cc->scratchRegisters().insert(regdict->findOrThrow("r31"));
    cc->scratchRegisters().insert(regdict->instructionPointerRegister());
    cc->scratchRegisters().insert(regdict->findOrThrow("zf"));
    cc->scratchRegisters().insert(regdict->findOrThrow("nf"));

    // Callee-saved registers are everything we didn't mention above
    RegisterParts regParts = regdict->getAllParts() - cc->getUsedRegisterParts();
    std::vector<RegisterDescriptor> registers = regParts.extract(regdict);
    cc->calleeSavedRegisters().insert(registers.begin(), registers.end());

    return {cc};
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction decoder
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Decodes one instruction.
class SimpleDecoder: public Disassembler::Base {
public:
    using Ptr = Sawyer::SharedPointer<SimpleDecoder>;

protected:
    explicit SimpleDecoder(const Architecture::Base::ConstPtr &arch)
        : Disassembler::Base(arch) {}

public:
    static Ptr instance(const Architecture::Base::ConstPtr &arch) {
        return Ptr(new SimpleDecoder(arch));
    }

    Disassembler::Base::Ptr clone() const override {
        return Ptr(new SimpleDecoder(*this));
    }

    std::string mnemonic(InsnKind kind) {
        switch (kind) {
            case InsnKind::ADD: return "add";
            case InsnKind::SUB: return "sub";
            case InsnKind::MUL: return "mul";
            case InsnKind::DIV: return "div";
            case InsnKind::SHL: return "shl";
            case InsnKind::SHR: return "shr";
            case InsnKind::AND: return "and";
            case InsnKind::OR:  return "or";
            case InsnKind::XOR: return "xor";
            case InsnKind::NOT: return "not";
            case InsnKind::CMP: return "cmp";
            case InsnKind::JMP: return "jmp";
            case InsnKind::BEQ: return "beq";
            case InsnKind::BLT: return "blt";
            case InsnKind::CALL: return "call";
            case InsnKind::RET: return "ret";
            case InsnKind::PUSH: return "push";
            case InsnKind::POP: return "pop";
            case InsnKind::READ: return "read";
            case InsnKind::WRITE: return "write";
            case InsnKind::MOV: return "mov";
            case InsnKind::LOAD: return "load";
            case InsnKind::HLT: return "hlt";
            default: return "unk";
        }
    }

    SgAsmInstruction* makeInstruction(rose_addr_t addr, const std::vector<uint8_t> &bytes, InsnKind kind) {
        auto insn = new SgAsmUserInstruction(addr, architecture()->name(), mnemonic(kind), static_cast<unsigned>(kind));
        auto operands = new SgAsmOperandList;
        insn->set_operandList(operands);
        operands->set_parent(insn);
        insn->set_rawBytes(bytes);
        return insn;
    }

    SgAsmInstruction* makeInstruction(rose_addr_t addr, const std::vector<uint8_t> &bytes, InsnKind kind,
                                      SgAsmExpression *arg1) {
        auto insn = makeInstruction(addr, bytes, kind);
        ASSERT_not_null(arg1);
        insn->get_operandList()->get_operands().push_back(arg1);
        arg1->set_parent(insn->get_operandList());
        return insn;
    }

    SgAsmInstruction* makeInstruction(rose_addr_t addr, const std::vector<uint8_t> &bytes, InsnKind kind,
                                      SgAsmExpression *arg1, SgAsmExpression *arg2) {
        auto insn = makeInstruction(addr, bytes, kind, arg1);
        ASSERT_not_null(arg2);
        insn->get_operandList()->get_operands().push_back(arg2);
        arg2->set_parent(insn->get_operandList());
        return insn;
    }

    SgAsmInstruction* makeUnknownInstruction(const Disassembler::Exception &e) override {
        return makeInstruction(e.ip, e.bytes, InsnKind::UNKNOWN);
    }

    SgAsmExpression* makeRegister(uint8_t i) {
        auto expr = new SgAsmDirectRegisterExpression(RegisterDescriptor(0, i, 0, 16));
        expr->set_type(Rose::SageBuilderAsm::buildTypeU16());
        return expr;
    }

    SgAsmExpression* makeConstant(uint8_t lo, uint8_t hi) {
        uint64_t val = (static_cast<uint64_t>(lo) << 8) | static_cast<uint64_t>(hi);
        return new SgAsmIntegerValueExpression(val, Rose::SageBuilderAsm::buildTypeU16());
    }

    SgAsmInstruction* disassembleOne(const MemoryMap::Ptr &map, rose_addr_t addr, AddressSet *successors = nullptr) override {
        ASSERT_not_null(map);
        if (addr % 4 != 0)
            throw Disassembler::Exception("instruction pointer is not aligned", addr);
        if (addr > 0xfffffffc)
            throw Disassembler::Exception("instruction pointer is out of range", addr);

        std::vector<uint8_t> bytes(4, 0);
        if (map->at(addr).require(MemoryMap::EXECUTABLE).read(bytes).size() < 4)
            throw Disassembler::Exception("short read", addr);

        SgAsmInstruction *insn = nullptr;
        InsnKind kind = static_cast<InsnKind>(bytes[0]);
        switch (kind) {
            // These take two register operands
            case InsnKind::ADD:
            case InsnKind::SUB:
            case InsnKind::MUL:
            case InsnKind::DIV:
            case InsnKind::SHL:
            case InsnKind::SHR:
            case InsnKind::AND:
            case InsnKind::OR:
            case InsnKind::XOR:
            case InsnKind::CMP:
            case InsnKind::READ:
            case InsnKind::WRITE:
            case InsnKind::MOV: {
                SgAsmExpression *arg1 = makeRegister(bytes[1]);
                SgAsmExpression *arg2 = makeRegister(bytes[2]);
                insn = makeInstruction(addr, bytes, kind, arg1, arg2);
                break;
            }

            // These take one register operand
            case InsnKind::NOT:
            case InsnKind::PUSH:
            case InsnKind::POP: {
                SgAsmExpression *arg1 = makeRegister(bytes[1]);
                insn = makeInstruction(addr, bytes, kind, arg1);
                break;
            }

            // These take a 16-bit literal and a register
            case InsnKind::LOAD: {
                SgAsmExpression *arg1 = makeConstant(bytes[1], bytes[2]);
                SgAsmExpression *arg2 = makeRegister(bytes[3]);
                insn = makeInstruction(addr, bytes, kind, arg1, arg2);
                break;
            }

            // These take a 16-bit literal
            case InsnKind::BEQ:
            case InsnKind::BLT:
            case InsnKind::JMP:
            case InsnKind::CALL: {
                SgAsmExpression *arg1 = makeConstant(bytes[1], bytes[2]);
                insn = makeInstruction(addr, bytes, kind, arg1);
                break;
            }

            // These take no arguments
            case InsnKind::RET:
            case InsnKind::HLT:
                insn = makeInstruction(addr, bytes, kind);
                break;

            // Unknown instruction
            default:
                insn = makeInstruction(addr, bytes, InsnKind::UNKNOWN);
                break;
        }

        ASSERT_not_null(insn);
        if (successors) {
            bool complete = true;
            *successors |= architecture()->getSuccessors(insn, complete /*out*/);
        }
        return insn;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Unparser generates assembly listings
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SimpleUnparser: public Rose::BinaryAnalysis::Unparser::Base {
public:
    using Ptr = Sawyer::SharedPointer<SimpleUnparser>;

    struct Settings: public Rose::BinaryAnalysis::Unparser::Settings {};

private:
    Settings settings_;

protected:
    SimpleUnparser(const Architecture::Base::ConstPtr &arch, const Settings &settings)
        : Rose::BinaryAnalysis::Unparser::Base(arch), settings_(settings) {}

public:
    static Ptr instance(const Architecture::Base::ConstPtr &arch, const Settings &settings = Settings()) {
        return Ptr(new SimpleUnparser(arch, settings));
    }

    Rose::BinaryAnalysis::Unparser::Base::Ptr copy() const override {
        return instance(architecture(), settings());
    }

    const Settings& settings() const override {
        return settings_;
    }

    Settings& settings() override {
        return settings_;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Instruction semantics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SimpleDispatcher: public BS::Dispatcher {
public:
    using Super = BS::Dispatcher;
    using Ptr = boost::shared_ptr<SimpleDispatcher>;

    // Intermediate type to reduce typing in the many instruction processors
    class P: public BS::InsnProcessor {
    public:
        using D = SimpleDispatcher*;
        using Ops = BS::RiscOperators*;
        using I = SgAsmUserInstruction*;
        using A = const SgAsmExpressionPtrList&;
        virtual void p(D, Ops, I, A) = 0;

        virtual void process(const BS::Dispatcher::Ptr &dispatcher_, SgAsmInstruction *insn_) override {
            SimpleDispatcher::Ptr dispatcher = SimpleDispatcher::promote(dispatcher_);
            BS::RiscOperators::Ptr operators = dispatcher->operators();
            auto insn = isSgAsmUserInstruction(insn_);
            ASSERT_not_null(insn);
            ASSERT_require(insn == operators->currentInstruction());
            dispatcher->advanceInstructionPointer(insn);
            ASSERT_not_null(insn->get_operandList());
            SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
            operators->comment("executing instruction core");
            p(dispatcher.get(), operators.get(), insn, operands);
        }

        void assert_args(I insn, A args, size_t nargs) {
            if (args.size() != nargs) {
                std::string mesg = "instruction must have " + Rose::StringUtility::plural(nargs, "arguments");
                throw BS::Exception(mesg, insn);
            }
        }
    };

    // Rb = Ra + Rb
    struct IP_add: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = d->read(args[1]);
            auto result = ops->add(a, b);
            d->write(args[1], result);
        }
    };

    // Rb = Ra & Rb
    struct IP_and: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = d->read(args[1]);
            auto result = ops->and_(a, b);
            d->write(args[1], result);
        }
    };

    // PC = NF ? Ia : FALLTHROUGH
    struct IP_beq: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = ops->readRegister(d->REG_PC);
            auto z = ops->readRegister(d->REG_ZF);
            ops->writeRegister(d->REG_PC, ops->ite(z, a, b));
        }
    };

    // PC = NF ? Ia : FALLTHROUGH
    struct IP_blt: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = ops->readRegister(d->REG_PC);
            auto n = ops->readRegister(d->REG_NF);
            ops->writeRegister(d->REG_PC, ops->ite(n, a, b));
        }
    };

    // LR = FALLTHROUGH; PC = Ia
    struct IP_call: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 1);
            auto a = d->read(args[0]);
            ops->writeRegister(d->REG_LR, ops->readRegister(d->REG_PC));
            ops->writeRegister(d->REG_PC, a);
        }
    };

    // ZF = Ra - Rb == 0; NF = Ra < Rb
    struct IP_cmp: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = d->read(args[1]);
            auto result = ops->subtract(a, b);
            ops->writeRegister(d->REG_ZF, ops->equalToZero(result));
            ops->writeRegister(d->REG_NF, ops->extract(result, 15, 16));
        }
    };

    // Rb = Ra / Rb
    struct IP_div: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = d->read(args[1]);
            auto result = ops->unsignedDivide(a, b);
            d->write(args[1], result);
        }
    };

    // PC = Ia
    struct IP_jmp: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 1);
            auto a = d->read(args[0]);
            ops->writeRegister(d->REG_PC, a);
        }
    };

    // PC = ADDRESS_OF_HLT_INSTRUCTION
    struct IP_hlt: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 0);
            ops->writeRegister(d->REG_PC, ops->number_(16, insn->get_address()));
        }
    };

    // Rb = Ia
    struct IP_load: P {
        void p(D d, Ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0], 16);
            d->write(args[1], a);
        }
    };

    // Rb = Ra
    struct IP_mov: P {
        void p(D d, Ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            d->write(args[1], a);
        }
    };

    // Rb = Ra * Rb
    struct IP_mul: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = d->read(args[1]);
            auto result = ops->extract(ops->unsignedMultiply(a, b), 0, 16);
            d->write(args[1], result);
        }
    };

    // Rb = ~Ra
    struct IP_not: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto result = ops->invert(a);
            d->write(args[1], result);
        }
    };

    // Rb = Ra | Rb
    struct IP_or: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = d->read(args[1]);
            auto result = ops->or_(a, b);
            d->write(args[1], result);
        }
    };

    // Rb = mem[SP]; SP = SP + 4
    struct IP_pop: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 1);
            auto oldSp = ops->readRegister(d->REG_SP);
            auto newSp = ops->add(oldSp, ops->number_(16, 4));
            auto yes = ops->boolean_(true);
            auto result = ops->readMemory(RegisterDescriptor(), oldSp, ops->undefined_(16), yes);
            ops->writeRegister(d->REG_SP, newSp);
            d->write(args[0], result);
        }
    };

    // SP = SP - 4; mem[SP] = Rb
    struct IP_push: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 1);
            auto a = d->read(args[0]);
            auto oldSp = ops->readRegister(d->REG_SP);
            auto newSp = ops->subtract(oldSp, ops->number_(16, 4));
            auto yes = ops->boolean_(true);
            ops->writeRegister(d->REG_SP, newSp);
            ops->writeMemory(RegisterDescriptor(), newSp, a, yes);
        }
    };

    // Rb = mem[Ra] (4-byte read)
    struct IP_read: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto addr = d->read(args[0]);
            auto yes = ops->boolean_(true);
            auto result = ops->readMemory(RegisterDescriptor(), addr, ops->undefined_(16), yes);
            d->write(args[1], result);
        }
    };

    // PC = LR
    struct IP_ret: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 0);
            ops->writeRegister(d->REG_PC, ops->readRegister(d->REG_LR));
        }
    };

    // Rb = Ra << Rb
    struct IP_shl: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = d->read(args[1]);
            auto result = ops->shiftLeft(a, b);
            d->write(args[1], result);
        }
    };

    // Rb = Ra >> Rb  (introducing zeros)
    struct IP_shr: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = d->read(args[1]);
            auto result = ops->shiftRight(a, b);
            d->write(args[1], result);
        }
    };

    // Rb = Ra - Rb
    struct IP_sub: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = d->read(args[1]);
            auto result = ops->subtract(a, b);
            d->write(args[1], result);
        }
    };

    // mem[Rb] = Ra (4-byte write)
    struct IP_write: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto value = d->read(args[0]);
            auto addr = d->read(args[1]);
            auto yes = ops->boolean_(true);
            ops->writeMemory(RegisterDescriptor(), addr, value, ops->boolean_(true));
        }
    };

    // Rb = Ra ^ Rb
    struct IP_xor: P {
        void p(D d, Ops ops, I insn, A args) {
            assert_args(insn, args, 2);
            auto a = d->read(args[0]);
            auto b = d->read(args[1]);
            auto result = ops->xor_(a, b);
            d->write(args[1], result);
        }
    };

protected:
    const RegisterDescriptor REG_PC, REG_SP, REG_LR, REG_ZF, REG_NF;

protected:
    explicit SimpleDispatcher(const Architecture::Base::ConstPtr &arch, const BS::RiscOperators::Ptr &ops)
        : Super(arch, ops),
          REG_PC(arch->registerDictionary()->instructionPointerRegister()),
          REG_SP(arch->registerDictionary()->stackPointerRegister()),
          REG_LR(arch->registerDictionary()->callReturnRegister()),
          REG_ZF(arch->registerDictionary()->findOrThrow("zf")),
          REG_NF(arch->registerDictionary()->findOrThrow("nf")) {

        if (BS::State::Ptr state = currentState()) {
            if (BS::MemoryState::Ptr memory = state->memoryState()) {
                if (ByteOrder::ORDER_UNSPECIFIED == memory->get_byteOrder()) {
                    memory->set_byteOrder(arch->byteOrder());
                } else if (memory->get_byteOrder() != arch->byteOrder()) {
                    mlog[WARN] <<"semantic state memory byte order conflicts with architecture byte order\n";
                }
            }
        }

        iprocSet(static_cast<int>(InsnKind::ADD), new IP_add);
        iprocSet(static_cast<int>(InsnKind::AND), new IP_and);
        iprocSet(static_cast<int>(InsnKind::BEQ), new IP_beq);
        iprocSet(static_cast<int>(InsnKind::BLT), new IP_blt);
        iprocSet(static_cast<int>(InsnKind::CALL), new IP_call);
        iprocSet(static_cast<int>(InsnKind::CMP), new IP_cmp);
        iprocSet(static_cast<int>(InsnKind::DIV), new IP_div);
        iprocSet(static_cast<int>(InsnKind::JMP), new IP_jmp);
        iprocSet(static_cast<int>(InsnKind::HLT), new IP_hlt);
        iprocSet(static_cast<int>(InsnKind::LOAD), new IP_load);
        iprocSet(static_cast<int>(InsnKind::MOV), new IP_mov);
        iprocSet(static_cast<int>(InsnKind::MUL), new IP_mul);
        iprocSet(static_cast<int>(InsnKind::NOT), new IP_not);
        iprocSet(static_cast<int>(InsnKind::OR), new IP_or);
        iprocSet(static_cast<int>(InsnKind::POP), new IP_pop);
        iprocSet(static_cast<int>(InsnKind::PUSH), new IP_push);
        iprocSet(static_cast<int>(InsnKind::READ), new IP_read);
        iprocSet(static_cast<int>(InsnKind::RET), new IP_ret);
        iprocSet(static_cast<int>(InsnKind::SHL), new IP_shl);
        iprocSet(static_cast<int>(InsnKind::SHR), new IP_shr);
        iprocSet(static_cast<int>(InsnKind::SUB), new IP_sub);
        iprocSet(static_cast<int>(InsnKind::WRITE), new IP_write);
        iprocSet(static_cast<int>(InsnKind::XOR), new IP_xor);
    }

public:
    static Ptr instance(const Architecture::Base::ConstPtr &arch, const BS::RiscOperators::Ptr &ops) {
        return Ptr(new SimpleDispatcher(arch, ops));
    }

    Super::Ptr create(const BS::RiscOperators::Ptr &ops) const override {
        return instance(architecture(), ops);
    }

    static Ptr promote(const BS::Dispatcher::Ptr &d) {
        Ptr retval = boost::dynamic_pointer_cast<SimpleDispatcher>(d);
        ASSERT_not_null(retval);
        return retval;
    }

    int iprocKey(SgAsmInstruction *insn) const override {
        ASSERT_not_null(insn);
        return insn->get_anyKind();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Main architecture class describes all the architecture-specific things.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SimpleArch: public Architecture::Base {
public:
    // Reference counting pointers
    using Ptr = std::shared_ptr<SimpleArch>;
    using ConstPtr = std::shared_ptr<const SimpleArch>;

protected:
    SimpleArch()                                        // use `instance` instead
        : Architecture::Base("simple", 2, ByteOrder::EL) {}

public:
    // Allocating constructor
    static Ptr instance() {
        return Ptr(new SimpleArch);
    }

    // Registers defined. We could have initialized this in the constructor, but the usual practice is to build the register
    // dictionary when it is first needed.
    RegisterDictionary::Ptr registerDictionary() const override {
        static SAWYER_THREAD_TRAITS::Mutex mutex;
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
        if (!registerDictionary_.isCached())
            registerDictionary_ = makeRegisterDictionary(shared_from_this());
        return registerDictionary_.get();
    }

    // Calling convention definitions. We could have initialized this in the constructor, but the usual practice is to build the
    // definitions when they're first needed.
    const CallingConvention::Dictionary& callingConventions() const override {
        static SAWYER_THREAD_TRAITS::Mutex mutex;
        SAWYER_THREAD_TRAITS::LockGuard lock(mutex);
        if (!callingConventions_.isCached())
            callingConventions_ = makeCallingConventions(shared_from_this());
        return callingConventions_.get();
    }

    // Return a new instruction decoder
    Disassembler::Base::Ptr newInstructionDecoder() const override {
        return SimpleDecoder::instance(shared_from_this());
    }

    // Return a new unparser
    Rose::BinaryAnalysis::Unparser::Base::Ptr newUnparser() const override {
        return SimpleUnparser::instance(shared_from_this());
    }

    // Return new instruction dispatcher
    BS::Dispatcher::Ptr newInstructionDispatcher(const BS::RiscOperators::Ptr &ops) const override {
        ASSERT_not_null(ops);
        return SimpleDispatcher::instance(shared_from_this(), ops);
    }

    // All instructions are encoded in exactly four bytes
    Sawyer::Container::Interval<size_t> bytesPerInstruction() const override {
        return 4;
    }

    // All instructions are aligned on 4-byte boundaries
    Alignment instructionAlignment() const override {
        return Alignment(4, bitsPerWord());
    }

    // Single-line instruction descriptions
    std::string instructionDescription(const SgAsmInstruction *insn_) const override {
        auto insn = isSgAsmUserInstruction(insn_);
        ASSERT_not_null(insn);
        auto kind = static_cast<InsnKind>(insn->get_kind());
        switch (kind) {
            case InsnKind::ADD: return "add";
            case InsnKind::SUB: return "subtract";
            case InsnKind::MUL: return "multiply";
            case InsnKind::DIV: return "divide";
            case InsnKind::SHL: return "shift left";
            case InsnKind::SHR: return "shift right, zero-inserting";
            case InsnKind::AND: return "bitwise AND";
            case InsnKind::OR:  return "bitwise OR";
            case InsnKind::XOR: return "bitwise XOR";
            case InsnKind::NOT: return "bitwise NOT";
            case InsnKind::CMP: return "compare";
            case InsnKind::JMP: return "branch unconditionally";
            case InsnKind::BEQ: return "branch if ZF set";
            case InsnKind::BLT: return "branch if NF set";
            case InsnKind::CALL: return "call subroutine";
            case InsnKind::RET: return "return from subroutine";
            case InsnKind::PUSH: return "push onto stack";
            case InsnKind::POP: return "pop from stack";
            case InsnKind::READ: return "read from memory";
            case InsnKind::WRITE: return "write to memory";
            case InsnKind::MOV: return "move data";
            case InsnKind::LOAD: return "load literal";
            case InsnKind::HLT: return "halt";
            case InsnKind::UNKNOWN: return "invalid instruction";
        }
        ASSERT_not_reachable("unhandled instruction");
    }

    // Predicate for unknown instruction
    bool isUnknown(const SgAsmInstruction *insn_) const override {
        auto insn = isSgAsmUserInstruction(insn_);
        ASSERT_not_null(insn);
        return static_cast<InsnKind>(insn->get_kind()) == InsnKind::UNKNOWN;
    }

    // Instructions that might not fall through
    bool terminatesBasicBlock(SgAsmInstruction *insn_) const override {
        auto insn = isSgAsmUserInstruction(insn_);
        ASSERT_not_null(insn);
        switch (static_cast<InsnKind>(insn->get_kind())) {
            case InsnKind::JMP:
            case InsnKind::BEQ:
            case InsnKind::BLT:
            case InsnKind::CALL:
            case InsnKind::RET:
            case InsnKind::HLT:
                return true;
            default:
                return false;
        }
    }

    // Is instruction sequence a function call?
    bool isFunctionCallFast(const std::vector<SgAsmInstruction*> &insns, rose_addr_t *target, rose_addr_t *ret) const override {
        if (insns.empty())
            return false;
        auto last = isSgAsmUserInstruction(insns.back());
        ASSERT_not_null(last);
        if (static_cast<InsnKind>(last->get_kind()) == InsnKind::CALL) {
            if (target && last->nOperands() >= 1) {
                if (auto ival = isSgAsmIntegerValueExpression(last->operand(0)))
                    *target = ival->get_absoluteValue();
            }
            if (ret)
                *ret = last->get_address() + last->get_size();
            return true;
        } else {
            return false;
        }
    }

    // Is instruction sequence a function return?
    bool isFunctionReturnFast(const std::vector<SgAsmInstruction*> &insns) const override {
        if (insns.empty())
            return false;
        auto last = isSgAsmUserInstruction(insns.back());
        ASSERT_not_null(last);
        return static_cast<InsnKind>(last->get_kind()) == InsnKind::RET;
    }

    // Static successors for an instruction
    AddressSet getSuccessors(SgAsmInstruction *insn_, bool &complete) const override {
        auto insn = isSgAsmUserInstruction(insn_);
        ASSERT_not_null(insn);
        AddressSet retval;

        switch (static_cast<InsnKind>(insn->get_kind())) {
            case InsnKind::BEQ:
            case InsnKind::BLT:
                retval.insert(insn->get_address() + insn->get_size());
                [[fallthrough]];

            case InsnKind::JMP:
            case InsnKind::CALL:
                if (auto ival = isSgAsmIntegerValueExpression(insn->operand(0))) {
                    retval.insert(ival->get_absoluteValue());
                    complete = true;
                } else {
                    complete = false;
                }
                break;

            case InsnKind::RET:
                complete = false;
                break;

            case InsnKind::HLT:
                complete = true;
                break;

            default:
                complete = true;
                retval.insert(insn->get_address() + insn->get_size());
        }
        return retval;
    }

    // Compiler-generated function prologue
    std::vector<P2::FunctionPrologueMatcher::Ptr> functionPrologueMatchers(const P2::Engine::Ptr&) const override {

        // Matches:
        //    PUSH FP           -- save the frame pointer
        //    PUSH LR           -- save the link register
        //    MOV SP, FP        -- point to the new call frame
        class Match: public P2::FunctionPrologueMatcher {
        protected:
            P2::Function::Ptr function_;

            Match() {}

        public:
            static Ptr instance() {
                return Ptr(new Match);
            }

            std::vector<P2::Function::Ptr> functions() const override {
                ASSERT_not_null(function_);
                return std::vector<P2::Function::Ptr>{function_};
            }

            bool match(const P2::Partitioner::ConstPtr &partitioner, const rose_addr_t startAddr) override {
                ASSERT_not_null(partitioner);
                const RegisterDescriptor SP = partitioner->architecture()->registerDictionary()->stackPointerRegister();
                const RegisterDescriptor FP = partitioner->architecture()->registerDictionary()->stackFrameRegister();
                const RegisterDescriptor LR = partitioner->architecture()->registerDictionary()->callReturnRegister();
                rose_addr_t addr = startAddr;
                {
                    auto insn = isSgAsmUserInstruction(partitioner->discoverInstruction(addr));
                    if (!insn || insn->nOperands() != 1 || (InsnKind)insn->get_kind() != InsnKind::PUSH)
                        return false;
                    auto arg = isSgAsmDirectRegisterExpression(insn->operand(0));
                    if (!arg || arg->get_descriptor() != FP)
                        return false;
                    addr += insn->get_size();
                }
                {
                    auto insn = isSgAsmUserInstruction(partitioner->discoverInstruction(addr));
                    if (!insn || insn->nOperands() != 1 || (InsnKind)insn->get_kind() != InsnKind::PUSH)
                        return false;
                    auto arg = isSgAsmDirectRegisterExpression(insn->operand(0));
                    if (!arg || arg->get_descriptor() != LR)
                        return false;
                    addr += insn->get_size();
                }
                {
                    auto insn = isSgAsmUserInstruction(partitioner->discoverInstruction(addr));
                    if (!insn || insn->nOperands() != 2 || (InsnKind)insn->get_kind() != InsnKind::MOV)
                        return false;
                    auto arg0 = isSgAsmDirectRegisterExpression(insn->operand(0));
                    if (!arg0 || arg0->get_descriptor() != SP)
                        return false;
                    auto arg1 = isSgAsmDirectRegisterExpression(insn->operand(1));
                    if (!arg1 || arg1->get_descriptor() != FP)
                        return false;
                }
                function_ = P2::Function::instance(startAddr, SgAsmFunction::FUNC_PATTERN);
                function_->reasonComment("standard prologue");
                return true;
            }
        };

        return {Match::instance()};
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Assembler
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class SimpleAssembler {
    struct Label {
        uint16_t address = 0;
        unsigned lineNumber = 0;

        Label(uint16_t address, unsigned lineNumber)
            : address(address), lineNumber(lineNumber) {}
    };

    struct Statement {
        std::vector<std::string> tokens;
        uint16_t address = 0;
        unsigned lineNumber = 0;

        Statement(const std::vector<std::string> &tokens, uint16_t address, unsigned lineNumber)
            : tokens(tokens), address(address), lineNumber(lineNumber) {}
    };

    Architecture::Base::ConstPtr arch_;
    std::istream &in_;
    std::ostream &out_;
    unsigned lineNumber_ = 0;                           // current line number
    uint16_t point_ = 0;                                // current address
    std::map<std::string, Label> labels_;
    std::list<Statement> statements_;

public:
    SimpleAssembler(std::istream &in, std::ostream &out)
        : arch_(SimpleArch::instance()), in_(in), out_(out) {
        ASSERT_not_null(arch_);
    }

private:
    void error(const std::string &mesg) const {
        mlog[ERROR] <<"stdin:" <<lineNumber_ <<": error: " <<mesg <<"\n";
    }

    void emit(InsnKind kind, uint8_t a1 = 0, uint8_t a2 = 0, uint8_t a3 = 0) const {
        out_.put(static_cast<uint8_t>(kind)).put(a1).put(a2).put(a3);
    }

    void emit(InsnKind kind, const std::pair<uint8_t, uint8_t> &p, uint8_t a) const {
        emit(kind, p.first, p.second, a);
    }

    void emit(InsnKind kind, const std::pair<uint8_t, uint8_t> &p) const {
        emit(kind, p.first, p.second, 0);
    }

    template<class T>
    std::string toString(T t) const {
        return boost::lexical_cast<std::string>(t);
    }

    uint8_t reg(const std::string &name) const {
        if (auto r = arch_->registerDictionary()->find(name)) {
            return r.minorNumber();
        } else {
            error("invalid register \"" + Rose::StringUtility::cEscape(name) + "\"");
            return 0;
        }
    }

    std::pair<uint8_t, uint8_t> imm(const std::string &name) const {
        if (auto val = Sawyer::parse<uint16_t>(name)) {
            return {*val >> 8, *val & 0xff};
        } else {
            error("invalid numeric literal \"" + Rose::StringUtility::cEscape(name) + "\"");
            return {0, 0};
        }
    }

    // Parse a single statement and add it to the statement list
    void parseStatement(std::string line) {
        std::smatch found;

        // Optional label
        std::regex labelRe("[ \\t]*([_a-zA_Z][_a-zA_Z0-9]*):(.*)", std::regex_constants::extended);
        if (std::regex_match(line, found, labelRe)) {
            const std::string name = found.str(1);
            line = found.str(2);
            const auto label = labels_.insert(std::make_pair(name, Label(point_, lineNumber_)));
            mlog[DEBUG] <<"label \"" <<name <<"\" = " <<Rose::StringUtility::addrToString(point_, 16) <<"\n";
            if (!label.second)
                error("label \"" + name + "\" already defined at line " + toString(label.first->second.lineNumber));
        }

        // Remove comments
        line = line.substr(0, line.find(';'));
        boost::trim(line);
        if (line.empty())
            return;

        // Some special directives
        std::regex setPcRe("\\.pc[ \\t]*=[ \\t*](.*)");
        if (std::regex_match(line, found, setPcRe)) {
            const std::string addrStr = found.str(1);
            if (auto address = Sawyer::parse<uint16_t>(addrStr)) {
                point_ = *address;
            } else {
                error("invalid numeric literal \"" + Rose::StringUtility::cEscape(addrStr) + "\"");
            }
            return;
        }

        // Split line into tokens
        std::vector<std::string> tokens;
        boost::split_regex(tokens, line, boost::regex("[ \\t]+"));
        if (tokens.empty())
            return;

        // Remove commas from the end of each token operand but the last one.
        for (size_t i = 1; i + 1 < tokens.size(); ++i) {
            if (boost::ends_with(tokens[i], ",")) {
                boost::erase_tail(tokens[i], 1);
                boost::trim_right(tokens[i]);
            } else {
                error("comma expected after operand #" + toString(i) + ": " + tokens[i]);
            }
        }

        // Expand special values
        for (std::string &token: tokens) {
            if ("$pc" == token)
                token = toString(point_);
        }

        statements_.push_back(Statement(tokens, point_, lineNumber_));
        point_ += 4;                                    // every instruction is 4 bytes
    }

    // Read and parse all input lines
    void parseInput() {
        point_ = 0;
        while (in_) {
            ++lineNumber_;
            parseStatement(rose_getline(in_));
        }
    }

    // Emit code for one statement
    void emitStatement(Statement &stmt) {
        lineNumber_ = stmt.lineNumber;

        // Expand labels
        for (std::string &token: stmt.tokens) {
            if (boost::starts_with(token, "$") && token.size() > 1) {
                const std::string name = token.substr(1);
                const auto label = labels_.find(name);
                if (label == labels_.end()) {
                    error("label \"" + name + "\" not defined");
                    token = "0";
                } else {
                    token = toString(label->second.address);
                }
            }
        }

        // Emit code
        const std::vector<std::string> &t = stmt.tokens;
        ASSERT_forbid(t.empty());
        if ("add" == t[0]) {                            // Rb = Ra + Rb
            emit(InsnKind::ADD, reg(t[1]), reg(t[2]));
        } else if ("and" == t[0]) {                     // Rb = Ra & Rb
            emit(InsnKind::AND, reg(t[1]), reg(t[2]));
        } else if ("beq" == t[0]) {                     // PC = ZF ? Ia : FALLTHROUGH
            emit(InsnKind::BEQ, imm(t[1]));
        } else if ("blt" == t[0]) {                     // PC = NF ? Ia : FALLTHROUGH
            emit(InsnKind::BLT, imm(t[1]));
        } else if ("call" == t[0]) {                    // LR = PC; PC = Ia
            emit(InsnKind::CALL, imm(t[1]));
        } else if ("cmp" == t[0]) {                     // ZF = a - b == 0; NF = a < b
            emit(InsnKind::CMP, reg(t[1]), reg(t[2]));
        } else if ("div" == t[0]) {                     // Rb = Ra / Rb (integer division)
            emit(InsnKind::DIV, reg(t[1]), reg(t[2]));
        } else if ("hlt" == t[0]) {                     // PC = CURRENT (infinite loop)
            emit(InsnKind::HLT);
        } else if ("jmp" == t[0]) {                     // PC = Ia
            emit(InsnKind::JMP, imm(t[1]));
        } else if ("load" == t[0]) {                    // Rb = Ia
            emit(InsnKind::LOAD, imm(t[1]), reg(t[2]));
        } else if ("mov" == t[0]) {                     // Rb = Ra
            emit(InsnKind::MOV, reg(t[1]), reg(t[2]));
        } else if ("mul" == t[0]) {                     // Rb = Ra % Rb
            emit(InsnKind::MUL, reg(t[1]), reg(t[2]));
        } else if ("not" == t[0]) {                     // Rb = ~Ra
            emit(InsnKind::NOT, reg(t[1]), reg(t[2]));
        } else if ("or" == t[0]) {                      // Rb = Ra | Rb
            emit(InsnKind::OR, reg(t[1]), reg(t[2]));
        } else if ("pop" == t[0]) {                     // Ra = mem[SP]; SP = SP + 4
            emit(InsnKind::POP, reg(t[1]));
        } else if ("push" == t[0]) {                    // SP = SP - 4; mem[SP] = Ra
            emit(InsnKind::PUSH, reg(t[1]));
        } else if ("read" == t[0]) {                    // Rb = mem[Ra]
            emit(InsnKind::READ, reg(t[1]), reg(t[2]));
        } else if ("ret" == t[0]) {                     // PC = LR
            emit(InsnKind::RET);
        } else if ("shl" == t[0]) {                     // Rb = Ra << Rb
            emit(InsnKind::SHL, reg(t[1]), reg(t[2]));
        } else if ("shr" == t[0]) {                     // Rb = Ra >> Rb
            emit(InsnKind::SHR, reg(t[1]), reg(t[2]));
        } else if ("sub" == t[0]) {                     // Rb = Ra - Rb
            emit(InsnKind::SUB, reg(t[1]), reg(t[2]));
        } else if ("write" == t[0]) {                   // mem[Rb] = Ra
            emit(InsnKind::WRITE, reg(t[1]), reg(t[2]));
        } else if ("xor" == t[0]) {                     // Rb = Ra ^ Rb
            emit(InsnKind::XOR, reg(t[1]), reg(t[2]));
        } else {
            error("invalid opcode \"" + Rose::StringUtility::cEscape(t[0]) + "\"");
            emit(InsnKind::HLT);
        }
    }

public:
    void assemble() {
        parseInput();
        for (Statement &stmt: statements_)
            emitStatement(stmt);
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Functions
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// Assembles code from standard input and writes it to standard output
void
assemble() {
    Rose::Diagnostics::initAndRegister(&mlog, "SimpleArchitecture");
    mlog.comment("demonstrating a simple architecture");
    mlog[DEBUG].enable();
    SimpleAssembler(std::cin, std::cout).assemble();
}

// Register architecture with ROSE
extern "C" {
void registerArchitectures() {
    Rose::Diagnostics::initAndRegister(&mlog, "SimpleArchitecture");
    mlog.comment("demonstrating a simple architecture");
    Architecture::registerDefinition(SimpleArch::instance());
}
}
