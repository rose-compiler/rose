#include "rose.h"

#include <signal.h>
#include <time.h>

// SEMANTIC_DOMAIN values
#define NULL_DOMAIN 1
#define PARTSYM_DOMAIN 2
#define SYMBOLIC_DOMAIN 3
#define INTERVAL_DOMAIN 4
#define MULTI_DOMAIN 5

// SEMANTIC_API values
#define OLD_API 1
#define NEW_API 2

#if SEMANTIC_API == OLD_API
#   include "x86InstructionSemantics.h"
    using namespace rose::BinaryAnalysis::InstructionSemantics;
#else
#   include "DispatcherX86.h"
    using namespace rose::BinaryAnalysis::InstructionSemantics2;
#endif

const RegisterDictionary *regdict = RegisterDictionary::dictionary_i386();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if SEMANTIC_DOMAIN == NULL_DOMAIN

#if SEMANTIC_API == OLD_API
#   include "NullSemantics.h"
#   define MyValueType rose::BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
#   define MyState     rose::BinaryAnalysis::InstructionSemantics::NullSemantics::State
#   define MyPolicy    rose::BinaryAnalysis::InstructionSemantics::NullSemantics::Policy<MyState, MyValueType>
#else
#   include "NullSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        return NullSemantics::RiscOperators::instance(regdict);
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == PARTSYM_DOMAIN

#if SEMANTIC_API == OLD_API
#   include "PartialSymbolicSemantics.h"
#   define MyValueType rose::BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::ValueType
#   define MyState     rose::BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::State
#   define MyPolicy    rose::BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::Policy<MyState, MyValueType>
#else
#   include "PartialSymbolicSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        return PartialSymbolicSemantics::RiscOperators::instance(regdict);
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == SYMBOLIC_DOMAIN

#if SEMANTIC_API == OLD_API
#   include "SymbolicSemantics.h"
#   define MyValueType rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::ValueType
#   define MyState     rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::State
#   define MyPolicy    rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics::Policy<MyState, MyValueType>
#else
#   include "SymbolicSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        return SymbolicSemantics::RiscOperators::instance(regdict);
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == INTERVAL_DOMAIN

#if SEMANTIC_API == OLD_API
#   include "IntervalSemantics.h"
#   define MyValueType rose::BinaryAnalysis::InstructionSemantics::IntervalSemantics::ValueType
#   define MyState     rose::BinaryAnalysis::InstructionSemantics::IntervalSemantics::State
#   define MyPolicy    rose::BinaryAnalysis::InstructionSemantics::IntervalSemantics::Policy<MyState, MyValueType>
#else
#   include "IntervalSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        return IntervalSemantics::RiscOperators::instance(regdict);
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == MULTI_DOMAIN

#if SEMANTIC_API == OLD_API
#   include "MultiSemantics.h"
#   include "PartialSymbolicSemantics.h"
#   include "SymbolicSemantics.h"
#   include "IntervalSemantics.h"
#   define MyMultiSemanticsClass MultiSemantics<                                                                               \
        PartialSymbolicSemantics::ValueType, PartialSymbolicSemantics::State, PartialSymbolicSemantics::Policy,                \
        SymbolicSemantics::ValueType, SymbolicSemantics::State, SymbolicSemantics::Policy,                                     \
        IntervalSemantics::ValueType, IntervalSemantics::State, IntervalSemantics::Policy>
#   define MyValueType MyMultiSemanticsClass::ValueType
#   define MyState MyMultiSemanticsClass::State
#   define MyPolicy MyMultiSemanticsClass::Policy<MyState, MyValueType>
#else
#   include "MultiSemantics2.h"
#   include "PartialSymbolicSemantics2.h"
#   include "SymbolicSemantics2.h"
#   include "IntervalSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        MultiSemantics::RiscOperatorsPtr ops = MultiSemantics::RiscOperators::instance(regdict);
        ops->add_subdomain(PartialSymbolicSemantics::RiscOperators::instance(regdict), "PartialSymbolic");
        ops->add_subdomain(SymbolicSemantics::RiscOperators::instance(regdict), "Symbolic");
        ops->add_subdomain(IntervalSemantics::RiscOperators::instance(regdict), "Interval");
        return ops;
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else
#error "Invalid semantic domain"
#endif

using namespace rose::BinaryAnalysis;

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

#if SEMANTIC_API == OLD_API
    MyPolicy operators;
    X86InstructionSemantics<MyPolicy, MyValueType> dispatcher(operators);
#else
    BaseSemantics::RiscOperatorsPtr operators = make_ops();
    BaseSemantics::DispatcherPtr dispatcher = DispatcherX86::instance(operators, 32);
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
#if SEMANTIC_API == OLD_API
            dispatcher.processInstruction(isSgAsmX86Instruction(insn));
            ++ninsns;
#if SEMANTIC_DOMAIN == MULTI_DOMAIN
            // multi-semantics ValueType has no is_known() or get_known(). We need to invoke it on a specific subpolicy.
            PartialSymbolicSemantics::ValueType<32> ip = operators.readRegister<32>("eip")
                                                         .get_subvalue(MyMultiSemanticsClass::SP0());
#else
            MyValueType<32> ip = operators.readRegister<32>("eip");
#endif
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

#if SEMANTIC_API == OLD_API
    MyValueType<32> eax = operators.readRegister<32>("eax");
    std::cerr <<"eax = " <<eax <<"\n";
#else
    BaseSemantics::SValuePtr eax = operators->readRegister(dispatcher->findRegister("eax"));
#if SEMANTIC_DOMAIN == MULTI_DOMAIN
    // This is entirely optional, but the output looks better if it has the names of the subdomains.
    std::cerr <<"eax = " <<(*eax + MultiSemantics::RiscOperators::promote(operators)->get_formatter()) <<"\n";
#else
    std::cerr <<"eax = " <<*eax <<"\n";
#endif
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
