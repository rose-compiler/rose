// API 2 version to verify instruction semantics by running a specimen natively under a debugger and comparing the native
// execution with the simulated execution in ROSE.
#include <rose.h>

#include <AsmUnparser_compat.h>
#include <ConcreteSemantics2.h>
#include <BinaryDebugger.h>
#include <Diagnostics.h>
#include <DispatcherX86.h>
#include <Partitioner2/Engine.h>
#include <sawyer/CommandLine.h>
#include <sawyer/ProgressBar.h>
#include <TraceSemantics2.h>

namespace P2 = rose::BinaryAnalysis::Partitioner2;
using namespace rose;
using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis;
using namespace rose::BinaryAnalysis::InstructionSemantics2;

Sawyer::Message::Facility mlog;

struct Settings {
    bool traceSemantics;
    Settings()
        : traceSemantics(false) {}
};

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], Settings &settings) {
    using namespace Sawyer::CommandLine;

    Parser parser;
    parser
        .purpose("test concrete instruction semantics")
        .version(std::string(ROSE_SCM_VERSION_ID).substr(0, 8), ROSE_CONFIGURE_DATE)
        .chapter(1, "ROSE Command-line Tools")
        .doc("Synopsis", "@prop{programName} [@v{switches}] [--] @v{specimen} [@v{arguments}...]")
        .doc("Description",
             "The @v{specimen} is run as a child process under a simple debugger which single steps through each "
             "instruction.  At each step, the current instruction is disassembled in ROSE and executed in a simulated "
             "machine. The registers and memory in the virtual machine are compared with the registers and memory in "
             "the subordinate process and differences are reported.")
        .doc("Bugs",
             "Only memory that is accessed in the virtual machine is compared with memory in the subordinate process, "
             "otherwise this mechanism would be much too slow.\n\n"

             "Certain instructions cannot be tested this way, such as system calls (INT, SYSENTER). This test skips over "
             "those kinds of instructions.\n\n"

             "Certain status flags often differ between simulated and native executions because their values are "
             "indicated as undefined in the Intel reference manual, but the simulator and native hardware must choose "
             "concrete values.\n\n"

             "Repeated string instructions (e.g., REPE CMPSB) don't always have consistent status flags in the native "
             "execution until the end of the instruction.\n\n");

    SwitchGroup gen = CommandlineProcessing::genericSwitches();

    gen.insert(Switch("trace")
               .intrinsicValue(true, settings.traceSemantics)
               .doc("Trace RISC operators.  The trace is displayed only when a problem is encountered. Tracing slows "
                    "down the execution substantially even if no output is produced.  The @s{no-trace} switch disables "
                    "tracing (errors are still reported, just not accompanied by a trace).  The default is to " +
                    std::string(settings.traceSemantics ? "" : "not ") + "produce a trace."));
    gen.insert(Switch("no-trace")
               .key("trace")
               .intrinsicValue(false, settings.traceSemantics)
               .hidden(true));

    return parser.with(gen).parse(argc, argv).apply().unreachedArgs();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Semantics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef BaseSemantics::RegisterStateGeneric RegisterState;
typedef BaseSemantics::RegisterStateGenericPtr RegisterStatePtr;

typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

// A concrete semantics that reads registers and memory from a subordinate process.
class RiscOperators: public ConcreteSemantics::RiscOperators {
    BinaryDebugger &subordinate_;
protected:
    RiscOperators(const BaseSemantics::StatePtr &state, BinaryDebugger &subordinate)
        : ConcreteSemantics::RiscOperators(state, NULL), subordinate_(subordinate) {
        set_name("Verification");
    }
public:
    static RiscOperatorsPtr instance(BinaryDebugger &subordinate, const RegisterDictionary *regdict) {
        BaseSemantics::SValuePtr protoval = ConcreteSemantics::SValue::instance();
        BaseSemantics::RegisterStatePtr registers = BaseSemantics::RegisterStateGeneric::instance(protoval, regdict);
        BaseSemantics::MemoryStatePtr memory = BaseSemantics::MemoryCellList::instance(protoval, protoval);
        BaseSemantics::StatePtr state = BaseSemantics::State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state, subordinate));
    }
    
public:
    // Reads a register from the subordinate process, unless we've already written to that register.
    virtual BaseSemantics::SValuePtr readRegister(const RegisterDescriptor &reg) ROSE_OVERRIDE {
        RegisterStatePtr regs = RegisterState::promote(get_state()->get_register_state());
        if (regs->is_partly_stored(reg))
            return ConcreteSemantics::RiscOperators::readRegister(reg);
        try {
            uint64_t value = subordinate_.readRegister(reg);
            return number_(reg.get_nbits(), value);
        } catch (const std::runtime_error &e) {
            RegisterNames rname(get_state()->get_register_state()->get_register_dictionary());
            throw BaseSemantics::Exception("cannot read register " + rname(reg) + " from subordinate process",
                                           get_insn());
        }
    }

public:
    // Reads memory from the subordinate process.
    virtual BaseSemantics::SValuePtr readMemory(const RegisterDescriptor &segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        uint8_t buf[8];
        if (dflt->get_width() > 8*sizeof(buf))
            throw BaseSemantics::Exception("readMemory width not handled: " + StringUtility::plural(dflt->get_width(), "bits"),
                                           get_insn());
        
        ASSERT_require(dflt->get_width() % 8 == 0);
        size_t nBytes = dflt->get_width() / 8;
        size_t nRead = subordinate_.readMemory(addr->get_number(), nBytes, buf);
        if (nRead < nBytes)
            throw BaseSemantics::Exception("error reading subordinate memory", get_insn());
        uint64_t value = 0;
        switch (get_state()->get_memory_state()->get_byteOrder()) {
            case ByteOrder::ORDER_MSB:
                for (size_t i=0; i<nBytes; ++i)
                    value |= (uint64_t)buf[i] << (8 * (nBytes-i) - 8);
                break;
            default:                                    // assuming little endian
                for (size_t i=0; i<nBytes; ++i)
                    value |= (uint64_t)buf[i] << (8*i);
                break;
        }
        return number_(dflt->get_width(), value);
    }

public:
    // Compare written-to simulated registers with registers in the subordinate process, reporting differences.
    bool checkRegisters(SgAsmInstruction *insn) {
        bool areSame = true;
        RegisterStatePtr regs = RegisterState::promote(get_state()->get_register_state());
        RegisterState::RegPairs cells = regs->get_stored_registers();
        RegisterNames rname(get_state()->get_register_state()->get_register_dictionary());
        BOOST_FOREACH (const RegisterState::RegPair &cell, cells) {
            uint64_t nativeValue = 0;
            try {
                nativeValue = subordinate_.readRegister(cell.desc);
            } catch (const std::runtime_error &e) {
                ::mlog[ERROR] <<"cannot read register " <<rname(cell.desc) <<" from subordinate process: " <<e.what() <<"\n";
                continue;
            }
                
            uint64_t simulatedValue = cell.value->get_number();
            if (nativeValue != simulatedValue) {
                // Avoid comparing registers whose values are indicated as "undefined" in the reference manual.
                bool dontCare = false;
                if (SgAsmX86Instruction *x86 = isSgAsmX86Instruction(insn)) {
                    switch (x86->get_kind()) {
                        case x86_bsf:
                        case x86_bsr:
                            // CF, OF, SF, AF, and PF are undefined.
                            dontCare = cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_cf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_of, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_sf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_af, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_pf, 1);
                            // Destination operand (arg 0) is undefined if source operand (arg 1) is zero
                            ASSERT_require(insn->get_operandList()->get_operands().size() == 2);
                            ASSERT_require(isSgAsmDirectRegisterExpression(insn->get_operandList()->get_operands()[0]));
                            if (cell.desc ==
                                isSgAsmDirectRegisterExpression(insn->get_operandList()->get_operands()[0])->get_descriptor())
                                dontCare = true;
                            break;
                        case x86_bt:
                            dontCare = cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_of, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_sf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_zf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_af, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_pf, 1);
                            break;
                        case x86_div:
                            dontCare = cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_cf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_of, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_sf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_zf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_af, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_pf, 1);
                            break;
                        case x86_imul:
                        case x86_mul:
                            dontCare = cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_sf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_zf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_af, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_pf, 1);
                            break;
                        case x86_rdtsc:
                            dontCare = cell.desc == RegisterDescriptor(x86_regclass_gpr, x86_gpr_ax, 0, 32) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_gpr, x86_gpr_dx, 0, 32);
                            break;
                        case x86_sar:
                        case x86_shl:
                        case x86_shr:
                        case x86_shrd:
                            // OF is undefined if shift amount != 1
                            // AF is undefined when shift amount != 0
                            // CF is undefined when the shift amoutn >= width of the destination (this seldom happens)
                            dontCare = cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_of, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_af, 1);
                            break;
                        case x86_repe_cmpsb:
                            // ZF and CF seem to have correct values only at the end of the native loop.
                            dontCare = cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_zf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_cf, 1);
                        case x86_repne_scasb:
                            // PF, SF, ZF, AF, CF seem to have correct values only at the end of the native loop.
                            dontCare = cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_pf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_sf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_zf, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_af, 1) ||
                                       cell.desc == RegisterDescriptor(x86_regclass_flags, x86_flags_status, x86_flag_cf, 1);
                            break;
                        default:
                            break;
                    }
                }
                if (!dontCare) {
                    if (areSame)
                        ::mlog[ERROR] <<"at " <<unparseInstructionWithAddress(insn) <<"\n";
                    ::mlog[ERROR] <<"values differ for register " <<rname(cell.desc)
                                  <<": simulated=" <<StringUtility::toHex2(simulatedValue, cell.desc.get_nbits())
                                  <<", native=" <<StringUtility::toHex2(nativeValue, cell.desc.get_nbits()) <<"\n";
                    areSame = false;
                }
            }
        }
        return areSame;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int
main(int argc, char *argv[]) {
    Diagnostics::initialize();
    ::mlog = Sawyer::Message::Facility("tool", Diagnostics::destination);
    Diagnostics::mfacilities.insertAndAdjust(::mlog);

    // Parse command-line
    Settings settings;
    std::vector<std::string> specimen = parseCommandLine(argc, argv, settings);
    if (specimen.empty())
        throw std::runtime_error("no specimen name specified; see --help");

    // Obtain info about the specimen, including a disassembler.
    P2::Engine engine;
    engine.parse(specimen.front());
    Disassembler *disassembler = engine.obtainDisassembler();
    if (!disassembler)
        throw std::runtime_error("architecture is not supported by this tool");
    size_t addrWidth = disassembler->stackPointerRegister().get_nbits();
    const RegisterDictionary *registerDictionary = disassembler->get_registers();
    typedef Sawyer::Container::Map<rose_addr_t, SgAsmInstruction*> InstructionMap;
    InstructionMap insns;

    // Build instruction semantics framework
    BinaryDebugger debugger(specimen);
    RiscOperatorsPtr checkOps = RiscOperators::instance(debugger, registerDictionary);
    TraceSemantics::RiscOperatorsPtr traceOps = TraceSemantics::RiscOperators::instance(checkOps);
    std::ostringstream trace;
    traceOps->stream().destination(Sawyer::Message::StreamSink::instance(trace));
    BaseSemantics::DispatcherPtr cpu = DispatcherX86::instance(traceOps, addrWidth);
    if (!cpu)
        throw std::runtime_error("instruction semantics not supported for this architecture");

    // Run the specimen natively and single step through it.
    Sawyer::ProgressBar<size_t> progress(::mlog[MARCH], "executed");
    while (!debugger.isTerminated()) {
        ++progress;
        rose_addr_t ip = debugger.executionAddress();

        // Disassemble (and save) the instruction
        SgAsmInstruction *insn = NULL;
        if (!insns.getOptional(ip).assignTo(insn)) {
            try {
                uint8_t buf[16];
                size_t nAvail = debugger.readMemory(ip, sizeof buf, buf);
                if (0 == nAvail) {
                    ::mlog[WARN] <<"unable to read specimen memory at " <<StringUtility::addrToString(ip) <<"\n";
                } else if (NULL == (insn = disassembler->disassembleOne(buf, ip, nAvail, ip))) {
                    ::mlog[WARN] <<"unable to disassemble instruction at " <<StringUtility::addrToString(ip) <<"\n";
                } else if (insn->isUnknown()) {
                    ::mlog[WARN] <<"unable to disassemble instruction at " <<StringUtility::addrToString(ip) <<"\n";
                    insn = NULL;
                }
                if (insn)
                    insns.insert(ip, insn);
            } catch (const Disassembler::Exception &e) {
                ::mlog[ERROR] <<"disassembler failed at " <<StringUtility::addrToString(ip) <<"\n";
            }
        }

        // Simulate the instruction with semantics
        if (insn) {
            SAWYER_MESG(::mlog[DEBUG]) <<unparseInstructionWithAddress(insn) <<"\n";
            try {
                trace.str("");
                checkOps->get_state()->clear();
                cpu->processInstruction(insn);
            } catch (const BaseSemantics::Exception &e) {
                ::mlog[ERROR] <<e <<"\n";
            }
        }

        // Single-step the native execution and then compare written-to registers and memory for the simulated execution with
        // those same registers and memory in the native execution.
        debugger.singleStep();
        if (insn && !checkOps->checkRegisters(insn))
            std::cerr <<trace.str();
    }
}
