#include "rose.h"

#include <signal.h>
#include <time.h>

#if USE_TEMPLATES
#   include "x86InstructionSemantics.h"
    using namespace BinaryAnalysis::InstructionSemantics;
#else
#   include "DispatcherX86.h"
    using namespace BinaryAnalysis::InstructionSemantics2;
#endif



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 3==POLICY_SELECTOR          // Partial symbolic semantics

#if USE_TEMPLATES
#   include "PartialSymbolicSemantics.h"
#   define MyValueType BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::ValueType
#   define MyState     BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::State
#   define MyPolicy    BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::Policy
#else
#   include "PartialSymbolicSemantics2.h"
    typedef PartialSymbolicSemantics::RiscOperators RiscOperators;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif 4==POLICY_SELECTOR        // Symbolic semantics

#if USE_TEMPLATES
#   include "SymbolicSemantics.h"
#   define MyValueType BinaryAnalysis::InstructionSemantics::SymbolicSemantics::ValueType
#   define MyState     BinaryAnalysis::InstructionSemantics::SymbolicSemantics::State
#   define MyPolicy    BinaryAnalysis::InstructionSemantics::SymbolicSemantics::Policy
#else
#   include "SymbolicSemantics2.h"
    typedef SymbolicSemantics::RiscOperators RiscOperators;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif 5==POLICY_SELECTOR        // Null semantics

#if USE_TEMPLATES
#   include "NullSemantics.h"
#   define MyValueType BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
#   define MyState     BinaryAnalysis::InstructionSemantics::NullSemantics::State
#   define MyPolicy    BinaryAnalysis::InstructionSemantics::NullSemantics::Policy
#else
#   include "NullSemantics2.h"
    typedef NullSemantics::RiscOperators RiscOperators;
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif 7==POLICY_SELECTOR        // Interval semantics

#if USE_TEMPLATES
#   include "IntervalSemantics.h"
#   define MyValueType BinaryAnalysis::InstructionSemantics::IntervalSemantics::ValueType
#   define MyState     BinaryAnalysis::InstructionSemantics::IntervalSemantics::State
#   define MyPolicy    BinaryAnalysis::InstructionSemantics::IntervalSemantics::Policy
#else
#   include "IntervalSemantics2.h"
    typedef IntervalSemantics::RiscOperators RiscOperators;
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else
#error "Invalid policy selector"
#endif

static const unsigned timeout = 60;      // approximate maximum time for test to run.
static volatile int had_alarm = 0;

void
alarm_handler(int)
{
    had_alarm = 1;
}


// Get the set of all instructions except instructions that are part of left-over blocks
struct AllInstructions: public SgSimpleProcessing {
protected:
    Disassembler::InstructionMap insns;
public:
    AllInstructions(SgNode *ast) {
        traverse(ast, preorder);
    }
    SgAsmInstruction *fetch(rose_addr_t va) const {
        Disassembler::InstructionMap::const_iterator found = insns.find(va);
        return found==insns.end() ? NULL : found->second;
    }
protected:
    void visit(SgNode *node) {
        if (SgAsmInstruction *insn = isSgAsmInstruction(node)) {
            SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(insn);
            if (func && 0==(func->get_reason() & SgAsmFunction::FUNC_LEFTOVERS))
                insns.insert(std::make_pair(insn->get_address(), insn));
        }
    }
};


int
main(int argc, char *argv[])
{
    SgProject *project = frontend(argc, argv);
    SgAsmInterpretation *interp = SageInterface::querySubTree<SgAsmInterpretation>(project).back();
    AllInstructions insns(interp);
    SgAsmGenericHeader *header = interp->get_headers()->get_headers().front();
    rose_addr_t start_va = header->get_base_va() + header->get_entry_rva();

#if USE_TEMPLATES
    MyPolicy<MyState, MyValueType> operators;
    X86InstructionSemantics<MyPolicy<MyState, MyValueType>, MyValueType> dispatcher(operators);
#else
    BaseSemantics::RiscOperatorsPtr operators = RiscOperators::instance();
    BaseSemantics::DispatcherPtr dispatcher = DispatcherX86::instance(operators);
#endif

    struct sigaction sa;
    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);
    alarm(timeout);
    struct timeval start_time;
    std::cout <<"test starting...\n";
    gettimeofday(&start_time, NULL);

    size_t ninsns = 0;
    while (!had_alarm) {
        rose_addr_t va = start_va;
        while (SgAsmInstruction *insn = insns.fetch(va)) {
            //std::cerr <<unparseInstructionWithAddress(insn) <<"\n";
#if USE_TEMPLATES
            dispatcher.processInstruction(isSgAsmx86Instruction(insn));
            ++ninsns;
            MyValueType<32> ip = operators.readRegister<32>("eip");
            if (!ip.is_known())
                break;
            va = ip.known_value();
#else
            dispatcher->processInstruction(insn);
            ++ninsns;
            BaseSemantics::SValuePtr ip = operators->readRegister(dispatcher->findRegister("eip"));
            if (!ip->is_number())
                break;
            va = ip->get_number();
#endif
            if (had_alarm)
                break;
        }
    }

#if USE_TEMPLATES
    MyValueType<32> eax = operators.readRegister<32>("eax");
    std::cerr <<"eax = " <<eax <<"\n";
#else
    BaseSemantics::SValuePtr eax = operators->readRegister(dispatcher->findRegister("eax"));
    std::cerr <<"eax = " <<*eax <<"\n";
#endif

    struct timeval stop_time;
    gettimeofday(&stop_time, NULL);
    double elapsed = ((double)stop_time.tv_sec-start_time.tv_sec) + 1e-6*((double)stop_time.tv_usec-start_time.tv_usec);
    if (elapsed < timeout/4.0)
        std::cout <<"warning: test did not run for a sufficiently long time; output may contain a high degree of error.\n";
    std::cout <<"number of instructions:  " <<ninsns <<"\n"
              <<"elapsed time:            " <<elapsed <<" seconds\n"
              <<"semantic execution rate: " <<(ninsns/elapsed) <<" instructions/second\n";
    return 0;
}
