#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#include "rose.h"
#include "Disassembler.h"

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

#include "DispatcherX86.h"

using namespace Rose::BinaryAnalysis;
using namespace Rose::BinaryAnalysis::InstructionSemantics2;

const RegisterDictionary *regdict = RegisterDictionary::dictionary_i386();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if SEMANTIC_DOMAIN == NULL_DOMAIN

#   include "NullSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        return NullSemantics::RiscOperators::instance(regdict);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == PARTSYM_DOMAIN

#   include "PartialSymbolicSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        return PartialSymbolicSemantics::RiscOperators::instance(regdict);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == SYMBOLIC_DOMAIN

#   include "SymbolicSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        return SymbolicSemantics::RiscOperators::instance(regdict);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == INTERVAL_DOMAIN

#   include "IntervalSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        return IntervalSemantics::RiscOperators::instance(regdict);
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == MULTI_DOMAIN

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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else
#error "Invalid semantic domain"
#endif

using namespace Rose::BinaryAnalysis;

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

    BaseSemantics::RiscOperatorsPtr operators = make_ops();
    BaseSemantics::DispatcherPtr dispatcher = DispatcherX86::instance(operators, 32);

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
            dispatcher->processInstruction(insn);
            ++ninsns;
            BaseSemantics::SValuePtr ip = operators->readRegister(dispatcher->findRegister("eip"));
            if (!ip->is_number())
                break;
            va = ip->get_number();
            if (had_alarm)
                break;
        }
    }

    BaseSemantics::SValuePtr eax = operators->readRegister(dispatcher->findRegister("eax"));
#if SEMANTIC_DOMAIN == MULTI_DOMAIN
    // This is entirely optional, but the output looks better if it has the names of the subdomains.
    std::cerr <<"eax = " <<(*eax + MultiSemantics::RiscOperators::promote(operators)->get_formatter()) <<"\n";
#else
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

#endif
