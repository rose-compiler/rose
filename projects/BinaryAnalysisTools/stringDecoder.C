#include <rose.h>

#include <ConcreteSemantics2.h>
#include <Partitioner2/Engine.h>
#include <Sawyer/CommandLine.h>
#include <TraceSemantics2.h>

using namespace Rose;
using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;
using namespace Rose::Diagnostics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

Sawyer::Message::Facility mlog;

struct Settings {
    rose_addr_t decoderVa;                              // address of the decoder function
    rose_addr_t stackVa;                                // initial stack pointer
    bool traceInsns;                                    // show instructions executed
    bool traceSemantics;                                // show semantics
    size_t insnLimit;                                   // max instructions to execute before giving up
    size_t showCall;                                    // show arguments in call (zero turns it off)
    bool synthesized;                                   // synthesize calls to the decoder
    Settings()
        : decoderVa(0), stackVa(0xbbbb0000), traceInsns(false), traceSemantics(false), insnLimit(1000000),
          showCall(0), synthesized(false) {}
};

class VirtualMachine {
    ConcreteSemantics::RiscOperatorsPtr ops_;
    BaseSemantics::DispatcherPtr cpu_;
    RegisterDescriptor regIp_, regSp_, regSs_;
    size_t wordSize_;
    rose_addr_t stackVa_, returnMarker_;
public:
    VirtualMachine(const P2::Partitioner &partitioner, const Settings &settings)
        : wordSize_(0), stackVa_(settings.stackVa), returnMarker_(0xbeef0967) {
        const RegisterDictionary *regs = partitioner.instructionProvider().registerDictionary();
        ops_ = ConcreteSemantics::RiscOperators::instance(regs);
        if (settings.traceSemantics) {
            BaseSemantics::RiscOperatorsPtr traceOps = TraceSemantics::RiscOperators::instance(ops_);
            cpu_ = partitioner.newDispatcher(traceOps);
        } else {
            cpu_ = partitioner.newDispatcher(ops_);
        }
        if (cpu_==NULL)
            throw std::runtime_error("no semantics for architecture");
    
        regIp_ = partitioner.instructionProvider().instructionPointerRegister();
        regSp_ = partitioner.instructionProvider().stackPointerRegister();
        regSs_ = partitioner.instructionProvider().stackSegmentRegister();
        wordSize_ = regIp_.nBits();
    }

    void reset(const MemoryMap::Ptr &map) {
        BOOST_FOREACH (MemoryMap::Segment &segment, map->segments())
            segment.buffer()->copyOnWrite(true);        // prevent the VM from changing the real map
        BaseSemantics::StatePtr state = ops_->currentState()->clone();
        state->clear();
        ConcreteSemantics::MemoryStatePtr memState = ConcreteSemantics::MemoryState::promote(state->memoryState());
        memState->memoryMap(map);
        ops_->currentState(state);
        BaseSemantics::SValuePtr sp = ops_->number_(wordSize_, stackVa_);
        ops_->writeRegister(regSp_, sp);
        ops_->writeMemory(regSs_, sp, ops_->number_(wordSize_, returnMarker_), ops_->boolean_(true));
    }

    MemoryMap::Ptr map() const {
        return ConcreteSemantics::MemoryState::promote(ops_->currentState()->memoryState())->memoryMap();
    }

    BaseSemantics::SValuePtr argument(size_t n) const {
        size_t bytesPerWord = wordSize_ / 8;
        rose_addr_t sp = ops_->readRegister(regSp_)->get_number();
        BaseSemantics::SValuePtr addr = ops_->number_(wordSize_,  sp + (n+1)*bytesPerWord);
        return ops_->readMemory(regSs_, addr, ops_->undefined_(wordSize_), ops_->boolean_(true));
    }

    void push(uint64_t value) {
        rose_addr_t sp = ops_->readRegister(regSp_)->get_number() - wordSize_/8;
        ops_->writeRegister(regSp_, ops_->number_(wordSize_, sp));
        writeMemory(sp, value);
    }

    void writeMemory(rose_addr_t va, uint64_t value) {
        BaseSemantics::SValuePtr vaExpr = ops_->number_(wordSize_, va);
        BaseSemantics::SValuePtr valueExpr = ops_->number_(wordSize_, value);
        ops_->writeMemory(RegisterDescriptor(), vaExpr, valueExpr, ops_->boolean_(true));
    }

    uint64_t readMemory(rose_addr_t va) const {
        MemoryMap::Ptr map = ConcreteSemantics::MemoryState::promote(ops_->currentState()->memoryState())->memoryMap();
        uint8_t buf[16];
        memset(buf, 0, sizeof buf);
        size_t nBytes = wordSize_ / 8;
        ASSERT_require(sizeof(buf) >= nBytes);
        map->at(va).limit(nBytes).read(buf);
        uint64_t retval = 0;
        for (size_t i=0; i<nBytes; ++i)
            retval |= buf[i] << (8*i);                  // FIXME[Robb P. Matzke 2015-05-19]: this is only little endian
        return retval;
    }
    
    std::string readString(rose_addr_t va) const {
        MemoryMap::Ptr map = ConcreteSemantics::MemoryState::promote(ops_->currentState()->memoryState())->memoryMap();
        return map->readString(va, 256 /*arbitrary*/);
    }

    void setIp(rose_addr_t ip) {
        ops_->writeRegister(regIp_, ops_->number_(wordSize_, ip));
    }

    rose_addr_t run(const P2::Partitioner &partitioner, const Settings &settings,
                    const std::set<rose_addr_t> &breakpoints = std::set<rose_addr_t>()) {
        for (size_t nInsns=0; nInsns<settings.insnLimit; ++nInsns) {
            rose_addr_t ip = ops_->readRegister(regIp_)->get_number();
            if (ip == returnMarker_ || (nInsns>0 && breakpoints.find(ip)!=breakpoints.end()))
                return ip;
            SgAsmInstruction *insn = partitioner.instructionProvider()[ip];
            if (!insn)
                throw std::runtime_error("no instruction at " + StringUtility::addrToString(ip));
            if (settings.traceInsns && ::mlog[TRACE])
                ::mlog[TRACE] <<partitioner.unparse(insn) <<"\n";
            cpu_->processInstruction(insn);
        }
        throw std::runtime_error("execution limit exceeded ("+StringUtility::plural(settings.insnLimit, "instructions")+")");
    }

    RegisterDescriptor regSs() const { return regSs_; }
    rose_addr_t returnMarker() const { return returnMarker_; }
    size_t wordSize() const { return wordSize_; }
};

static std::vector<std::string>
parseCommandLine(int argc, char *argv[], P2::Engine &engine, Settings &settings) {
    using namespace Sawyer::CommandLine;
    std::string purpose = "decode encoded strings";
    std::string description =
        "Demonstrates the use of ROSE instruction semantics and ability to start execution an an arbitrary address and "
        "machine state. The @s{decoder} switch is required--it is the entry address of a string decoding function. This "
        "analysis finds all statically-detected calls to that function, obtains three arguments from the call's basic block, "
        "and calls the function.  The third argument is used as the address of a buffer where the decoded string is stored, "
        "and the string will be printed as the result.\n\n"
        "This tool can also run in a mode where the calls are synthesized by varying the first of three arguments.";

    Parser parser = engine.commandLineParser(purpose, description);
    parser.doc("Bugs", "z",
               "Being a demo, this tool is not very flexible when it comes to how the decrypted string is located or "
               "what argument values are used in the synthesis mode.");

    SwitchGroup sg("Tool-specific switches");
    sg.name("tool");

    sg.insert(Switch("decoder")
              .argument("address", nonNegativeIntegerParser(settings.decoderVa))
              .doc("Virtual address of the string decoding function."));

    sg.insert(Switch("stack")
              .argument("address", nonNegativeIntegerParser(settings.stackVa))
              .doc("Initial value for the stack pointer.  The default is " +
                   StringUtility::addrToString(settings.stackVa) + "."));

    sg.insert(Switch("trace-insns")
              .intrinsicValue(true, settings.traceInsns)
              .doc("Cause instructions to be printed to standard error as they are executed. The @s{no-trace-insns} switch "
                   "turns tracing off. The default is to " + std::string(settings.traceInsns?"":"not ") +
                   "show tracing."));
    sg.insert(Switch("no-trace-insns")
              .key("trace-insns")
              .intrinsicValue(false, settings.traceInsns)
              .hidden(true));

    sg.insert(Switch("trace-semantics")
              .intrinsicValue(true, settings.traceSemantics)
              .doc("Cause instruction semantics (the RISC-like operations) to be printed to standard error as they are "
                   "executed. The @s{no-trace-semantics} switch turns tracing off. The default is to " +
                   std::string(settings.traceSemantics?"":"not ") + "show tracing."));
    sg.insert(Switch("no-trace-semantics")
              .key("trace-semantics")
              .intrinsicValue(false, settings.traceSemantics)
              .hidden(true));

    sg.insert(Switch("insn-limit")
              .argument("n", nonNegativeIntegerParser(settings.insnLimit))
              .doc("Maximum number of instructions to execute per decoder call before giving up. The default is " +
                   StringUtility::plural(settings.insnLimit, "instructions") + "."));

    sg.insert(Switch("show-call")
              .argument("n", nonNegativeIntegerParser(settings.showCall))
              .doc("Show calls to the decryption function along with their arguments.  The @v{n} specifies how many arguments "
                   "(each being the natural length of a word) to display. If @v{n} is zero then call information is not "
                   "displayed.  The default is " + StringUtility::plural(settings.showCall, "arguments") + "."));

    sg.insert(Switch("synthesized")
              .intrinsicValue(true, settings.synthesized)
              .doc("Synthesize calls from scratch instead of looking for existing calls."));

    return parser.with(sg).parse(argc, argv).apply().unreachedArgs();
}

static std::string
arguments(const VirtualMachine &vm, size_t nArgs) {
    std::ostringstream ss;
    ss <<"(";
    for (size_t i=0; i<nArgs; ++i) {
        BaseSemantics::SValuePtr arg = vm.argument(i);
        ss <<(i?", ":"") <<"arg_" <<i <<" = " <<*arg;
        if (vm.map()->at(arg->get_number()).exists()) {
            if (uint64_t deref = vm.readMemory(arg->get_number()))
                ss <<" [deref=" <<StringUtility::toHex2(deref, vm.wordSize()) <<"]";
        }
    }
    ss <<")";
    return ss.str();
}

static void
processExistingCalls(const P2::Partitioner &partitioner, const Settings &settings) {
    P2::ControlFlowGraph::ConstVertexIterator decoderVertex = partitioner.findPlaceholder(settings.decoderVa);
    ASSERT_require(partitioner.cfg().isValidVertex(decoderVertex));

    VirtualMachine vm(partitioner, settings);

    // Find all calls to the decoder function
    BOOST_FOREACH (const P2::ControlFlowGraph::Edge &edge, decoderVertex->inEdges()) {
        if (edge.value().type() != P2::E_FUNCTION_CALL)
            continue;
        const P2::ControlFlowGraph::ConstVertexIterator caller = edge.source();
        if (caller->value().type() != P2::V_BASIC_BLOCK || caller->value().bblock()==NULL)
            continue;
        ::mlog[TRACE] <<"decoder called at " <<partitioner.edgeName(edge) <<"\n";

        // Reset the virtual machine
        vm.reset(partitioner.memoryMap());
        vm.setIp(caller->value().address());

        // Decoder return addresses
        std::set<rose_addr_t> breakpoints;
        P2::CfgConstEdgeSet callReturnEdges = P2::findCallReturnEdges(caller);
        BOOST_FOREACH (const P2::ControlFlowGraph::ConstEdgeIterator &callret, callReturnEdges.values()) {
            const P2::ControlFlowGraph::ConstVertexIterator returnVertex = callret->target();
            if (returnVertex->value().type() == P2::V_BASIC_BLOCK)
                breakpoints.insert(returnVertex->value().address());
        }

        // Execute until the return address
        rose_addr_t resultVa = 0;
        std::string stringId;
        breakpoints.insert(settings.decoderVa);
        while (1) {
            rose_addr_t ip = 0;
            try {
                ip = vm.run(partitioner, settings, breakpoints);
            } catch (const std::runtime_error &e) {
                ::mlog[WARN] <<StringUtility::addrToString(ip) <<": " <<e.what() <<"\n";
                break;
            }

            if (ip == settings.decoderVa) {
                // When entering the decoder, save [esp+0xc] since this is the address of the decoded string.
                breakpoints.erase(settings.decoderVa);
                if (settings.showCall)
                    std::cout <<"(" <<partitioner.edgeName(edge) <<")" <<arguments(vm, settings.showCall) <<"\n";
                stringId = "string-" + StringUtility::numberToString(vm.argument(0)->get_number());
                resultVa = vm.argument(2)->get_number();
            } else {
                // When leaving the decoder, print the decoded string
                std::string str = vm.readString(resultVa);
                std::cout <<stringId <<"\t\"" <<StringUtility::cEscape(str) <<"\"\n";
                break;
            }
        }
    }
}

static void
processSynthesizedCalls(const P2::Partitioner &partitioner, const Settings &settings) {
    const rose_addr_t lengthVa = settings.stackVa + 0x1000; // arbitrary address (at least 4 bytes of space)
    const rose_addr_t resultVa = settings.stackVa + 0x1010; // arbitrary address (at least 104 bytes of space)
    VirtualMachine vm(partitioner, settings);           // virtual machine using concrete semantics
    for (size_t strId=1; strId<=0x7a; ++strId) {
        vm.reset(partitioner.memoryMap());              // reset virtual machine to initial conditions
        vm.writeMemory(lengthVa, 0x68);                 // limited length
        vm.push(resultVa);                              // arg #3 is the address of the decoded string's buffer
        vm.push(lengthVa);                              // arg #2 is the returned length of the decoded string
        vm.push(strId);                                 // arg #1 is the string id number in [1 .. 0x7a]
        vm.push(vm.returnMarker());                     // to know when to stop
        vm.setIp(settings.decoderVa);                   // starting postion
        if (settings.showCall)
            std::cout <<"(*" <<StringUtility::addrToString(settings.decoderVa) <<")" <<arguments(vm, settings.showCall) <<"\n";
        try {
            vm.run(partitioner, settings);              // run until returnMarker is executed
        } catch (const std::runtime_error &e) {
            ::mlog[WARN] <<e.what() <<"\n";
            continue;
        }
        std::string str = vm.readString(resultVa);      // read the NUL-terminated string
        std::cout <<"string-" <<std::hex <<strId <<std::dec <<"\t\"" <<StringUtility::cEscape(str) <<"\"\n";
    }
}


int
main(int argc, char *argv[]) {
    ROSE_INITIALIZE;
    Diagnostics::initAndRegister(&::mlog, "tool");

    Settings settings;
    P2::Engine engine;
    engine.doingPostAnalysis(false);                    // not needed by this tool
    std::vector<std::string> specimens = parseCommandLine(argc, argv, engine, settings);
    P2::Partitioner partitioner = engine.partition(specimens);
    if (settings.traceInsns || settings.traceSemantics)
        ::mlog[TRACE].enable();

    // Find the string decoder.
    if (!partitioner.functionExists(settings.decoderVa)) {
        ::mlog[FATAL] <<"cannot find decoder function at " <<StringUtility::addrToString(settings.decoderVa) <<"\n";
        exit(1);
    }

    if (settings.synthesized) {
        processSynthesizedCalls(partitioner, settings);
    } else {
        processExistingCalls(partitioner, settings);
    }
}
