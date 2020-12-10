/* For each function (SgAsmFunction) process each instruction (SgAsmInstruction) through the instruction semantics
 * layer using the FindConstantsPolicy. Output consists of each instruction followed by the registers and memory locations
 * with constant or pseudo-constant values. */
#include "conditionalDisable.h"
#ifdef ROSE_BINARY_TEST_DISABLED
#include <iostream>
int main() { std::cout <<"disabled for " <<ROSE_BINARY_TEST_DISABLED <<"\n"; return 1; }
#else

#define __STDC_FORMAT_MACROS
#include "rose.h"
#include <set>
#include <inttypes.h>
#include <Sawyer/IntervalSet.h>

// SEMANTIC_DOMAIN values
#define NULL_DOMAIN 1
#define PARTSYM_DOMAIN 2
#define SYMBOLIC_DOMAIN 3
#define INTERVAL_DOMAIN 4
#define MULTI_DOMAIN 5

// SMT_SOLVER values
#define NO_SOLVER 0
#define YICES_LIB 1
#define YICES_EXE 2

#include "DispatcherX86.h"
#include "TestSemantics2.h"
using namespace Rose::BinaryAnalysis::InstructionSemantics2;

#if !defined(SMT_SOLVER) || SMT_SOLVER == NO_SOLVER
    #include "BinarySmtSolver.h"
    Rose::BinaryAnalysis::SmtSolverPtr make_solver() { return Rose::BinaryAnalysis::SmtSolverPtr(); }
#elif SMT_SOLVER == YICES_LIB
    #include "BinaryYicesSolver.h"
    Rose::BinaryAnalysis::SmtSolverPtr make_solver() {
        return Rose::BinaryAnalysis::YicesSolver::instance(Rose::BinaryAnalysis::SmtSolver::LM_LIBRARY);
    }
#elif SMT_SOLVER == YICES_EXE
    #include "BinaryYicesSolver.h"
    Rose::BinaryAnalysis::SmtSolverPtr make_solver() {
        return Rose::BinaryAnalysis::YicesSolver::instance(Rose::BinaryAnalysis::SmtSolver::LM_EXECUTABLE);
    }
#elif SMT_SOLVER == Z3_LIB
    #include "BinaryZ3Solver.h"
    Rose::BinaryAnalysis::SmtSolverPtr make_solver() {
        return Rose::BinaryAnalysis::Z3Solver::instance(Rose::BinaryAnalysis::SmtSolver::LM_LIBRARY);
    }
#elif SMT_SOLVER == Z3_EXE
    #include "BinaryZ3Solver.h"
    Rose::BinaryAnalysis::SmtSolverPtr make_solver() {
        return Rose::BinaryAnalysis::Z3Solver::instance(Rose::BinaryAnalysis::SmtSolver::LM_EXECUTABLE);
    }
#else
#   error "invalid value for SMT_SOLVER"
#endif

const Rose::BinaryAnalysis::RegisterDictionary *regdict = Rose::BinaryAnalysis::RegisterDictionary::dictionary_pentium4();

#include "TraceSemantics2.h"

// defaults for command-line switches
static bool do_trace = false;
static bool do_test_subst = false;
static bool do_usedef = true;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if SEMANTIC_DOMAIN == NULL_DOMAIN

#   include "NullSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        BaseSemantics::RiscOperatorsPtr retval = NullSemantics::RiscOperators::instance(regdict);
        TestSemantics<
            NullSemantics::SValuePtr, NullSemantics::RegisterStatePtr, NullSemantics::MemoryStatePtr,
            BaseSemantics::StatePtr, NullSemantics::RiscOperatorsPtr> tester;
        tester.test(retval);
        return retval;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif  SEMANTIC_DOMAIN == PARTSYM_DOMAIN
#   include "PartialSymbolicSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        BaseSemantics::RiscOperatorsPtr retval = PartialSymbolicSemantics::RiscOperators::instance(regdict);
        TestSemantics<PartialSymbolicSemantics::SValuePtr, BaseSemantics::RegisterStateGenericPtr,
                      BaseSemantics::MemoryCellListPtr, BaseSemantics::StatePtr,
                      PartialSymbolicSemantics::RiscOperatorsPtr> tester;
        tester.test(retval);
        return retval;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif  SEMANTIC_DOMAIN == SYMBOLIC_DOMAIN

#   include "SymbolicSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        SymbolicSemantics::RiscOperatorsPtr retval = SymbolicSemantics::RiscOperators::instance(regdict);
        retval->computingDefiners(do_usedef ? SymbolicSemantics::TRACK_ALL_DEFINERS : SymbolicSemantics::TRACK_NO_DEFINERS);
        TestSemantics<SymbolicSemantics::SValuePtr, BaseSemantics::RegisterStateGenericPtr,
                      SymbolicSemantics::MemoryStatePtr, BaseSemantics::StatePtr,
                      SymbolicSemantics::RiscOperatorsPtr> tester;
        tester.test(retval);
        return retval;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == INTERVAL_DOMAIN
#   include "IntervalSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        BaseSemantics::RiscOperatorsPtr retval = IntervalSemantics::RiscOperators::instance(regdict);
        TestSemantics<IntervalSemantics::SValuePtr, BaseSemantics::RegisterStateGenericPtr,
                      IntervalSemantics::MemoryStatePtr, BaseSemantics::StatePtr,
                      IntervalSemantics::RiscOperatorsPtr> tester;
        tester.test(retval);
        return retval;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == MULTI_DOMAIN
#   include "MultiSemantics2.h"
#   include "PartialSymbolicSemantics2.h"
#   include "SymbolicSemantics2.h"
#   include "IntervalSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        MultiSemantics::RiscOperatorsPtr ops = MultiSemantics::RiscOperators::instance(regdict);
        PartialSymbolicSemantics::RiscOperatorsPtr s1 = PartialSymbolicSemantics::RiscOperators::instance(regdict);
        SymbolicSemantics::RiscOperatorsPtr s2 = SymbolicSemantics::RiscOperators::instance(regdict);
        s2->set_compute_usedef();
        IntervalSemantics::RiscOperatorsPtr s3 = IntervalSemantics::RiscOperators::instance(regdict);
        ops->add_subdomain(s1, "PartialSymbolic");
        ops->add_subdomain(s2, "Symbolic");
        ops->add_subdomain(s3, "Interval");
        return ops;
    }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else
#error "invalid value for SEMANTIC_DOMAIN"
#endif

/* Analyze a single interpretation a block at a time */
static void
analyze_interp(SgAsmInterpretation *interp)
{
    /* Get the set of all instructions except instructions that are part of left-over blocks. */
    struct AllInstructions: public SgSimpleProcessing, public std::map<rose_addr_t, SgAsmX86Instruction*> {
        void visit(SgNode *node) {
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(node);
            SgAsmFunction *func = SageInterface::getEnclosingNode<SgAsmFunction>(insn);
            if (func && 0==(func->get_reason() & SgAsmFunction::FUNC_LEFTOVERS))
                insert(std::make_pair(insn->get_address(), insn));
        }
    } insns;
    insns.traverse(interp, postorder);

    while (!insns.empty()) {
        std::cout <<"=====================================================================================\n"
                  <<"=== Starting a new basic block                                                    ===\n"
                  <<"=====================================================================================\n";
        AllInstructions::iterator si = insns.begin();
        SgAsmX86Instruction *insn = si->second;
        insns.erase(si);

        BaseSemantics::RiscOperatorsPtr operators = make_ops();
        BaseSemantics::Formatter formatter;
        formatter.set_suppress_initial_values();
        formatter.set_show_latest_writers(do_usedef);
        BaseSemantics::DispatcherPtr dispatcher;
        if (do_trace) {
            // Enable RiscOperators tracing, but turn off a bunch of info that makes comparisons with a known good answer
            // difficult.
            Sawyer::Message::PrefixPtr prefix = Sawyer::Message::Prefix::instance();
            prefix->showProgramName(false);
            prefix->showThreadId(false);
            prefix->showElapsedTime(false);
            prefix->showFacilityName(Sawyer::Message::Prefix::NEVER);
            prefix->showImportance(false);
            Sawyer::Message::UnformattedSinkPtr sink = Sawyer::Message::StreamSink::instance(std::cout);
            sink->prefix(prefix);
            sink->defaultPropertiesNS().useColor = false;
            TraceSemantics::RiscOperatorsPtr trace = TraceSemantics::RiscOperators::instance(operators);
            trace->stream().destination(sink);
            trace->stream().enable();
            dispatcher = DispatcherX86::instance(trace, 32);
        } else {
            dispatcher = DispatcherX86::instance(operators, 32);
        }
        operators->solver(make_solver());

        // The fpstatus_top register must have a concrete value if we'll use the x86 floating-point stack (e.g., st(0))
        if (const Rose::BinaryAnalysis::RegisterDescriptor REG_FPSTATUS_TOP = regdict->findOrThrow("fpstatus_top")) {
            BaseSemantics::SValuePtr st_top = operators->number_(REG_FPSTATUS_TOP.nBits(), 0);
            operators->writeRegister(REG_FPSTATUS_TOP, st_top);
        }

#if SEMANTIC_DOMAIN == SYMBOLIC_DOMAIN
        BaseSemantics::SValuePtr orig_esp;
        if (do_test_subst) {
            // Only request the orig_esp if we're going to use it later because it causes an esp value to be instantiated
            // in the state, which is printed in the output, and thus changes the answer.
            BaseSemantics::RegisterStateGeneric::promote(operators->currentState()->registerState())->initialize_large();
            orig_esp = operators->readRegister(regdict->findOrThrow("esp"));
            std::cout <<"Original state:\n" <<*operators;
        }
#endif

        /* Perform semantic analysis for each instruction in this block. The block ends when we no longer know the value of
         * the instruction pointer or the instruction pointer refers to an instruction that doesn't exist or which has already
         * been processed. */
        while (1) {
            /* Analyze current instruction */
            std::cout <<"\n" <<unparseInstructionWithAddress(insn) <<"\n";
            try {
                dispatcher->processInstruction(insn);
                std::cout <<(*operators + formatter);
            } catch (const BaseSemantics::Exception &e) {
                std::cout <<e <<"\n";
            }

            /* Never follow CALL instructions */
            if (insn->get_kind()==Rose::BinaryAnalysis::x86_call ||
                insn->get_kind()==Rose::BinaryAnalysis::x86_farcall)
                break;

            /* Get next instruction of this block */
            BaseSemantics::SValuePtr ip = operators->readRegister(dispatcher->findRegister("eip"));
            if (!ip->is_number())
                break;
            rose_addr_t next_addr = ip->get_number();
            si = insns.find(next_addr);
            if (si==insns.end()) break;
            insn = si->second;
            insns.erase(si);
        }

        // Test substitution on the symbolic state.
#if SEMANTIC_DOMAIN == SYMBOLIC_DOMAIN
        if (do_test_subst) {
            SymbolicSemantics::SValuePtr from = SymbolicSemantics::SValue::promote(orig_esp);
            BaseSemantics::SValuePtr newvar = operators->undefined_(32);
            newvar->set_comment("frame_pointer");
            SymbolicSemantics::SValuePtr to =
                SymbolicSemantics::SValue::promote(operators->add(newvar, operators->number_(32, 4)));
            std::cout <<"Substituting from " <<*from <<" to " <<*to <<"\n";
            SymbolicSemantics::RiscOperators::promote(operators)->substitute(from, to);
            std::cout <<"Substituted state:\n" <<(*operators+formatter);
        }
#endif
    }
}

/* Analyze only interpretations that point only to 32-bit x86 instructions. */
class AnalyzeX86Functions: public SgSimpleProcessing {
public:
    size_t ninterps;
    AnalyzeX86Functions(): ninterps(0) {}
    void visit(SgNode* node) {
        SgAsmInterpretation *interp = isSgAsmInterpretation(node);
        if (interp) {
            const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
            bool only_x86 = true;
            for (size_t i=0; i<headers.size() && only_x86; ++i)
                only_x86 = 4==headers[i]->get_word_size();
            if (only_x86) {
                ++ninterps;
                analyze_interp(interp);
            }
        }
    }
};

int main(int argc, char *argv[]) {
    ROSE_INITIALIZE;

    std::vector<std::string> args(argv, argv+argc);
    for (size_t argno=1; argno<args.size(); ++argno) {
        if (0==args[argno].compare("--trace")) {
            do_trace = true;
            args[argno] = "";
        } else if (0==args[argno].compare("--no-trace")) {
            do_trace = false;
            args[argno] = "";
        } else if (0==args[argno].compare("--test-subst")) {
            do_test_subst = true;
            args[argno] = "";
        } else if (0==args[argno].compare("--no-test-subst")) {
            do_test_subst = false;
            args[argno] = "";
        } else if (0==args[argno].compare("--usedef")) {
            do_usedef = true;
            args[argno] = "";
        } else if (0==args[argno].compare("--no-usedef")) {
            do_usedef = false;
            args[argno] = "";
        } else if (0==args[argno].compare("--debug-solver")) {
            Rose::Diagnostics::mfacilities.control("Rose::BinaryAnalysis::SmtSolver(all)");
        }
    }
    args.erase(std::remove(args.begin(), args.end(), ""), args.end());

    SgProject *project = frontend(argc, argv);
    AnalyzeX86Functions analysis;
    analysis.traverse(project, postorder);
    if (0==analysis.ninterps) {
        std::cout <<"file(s) didn't have any 32-bit x86 headers.\n";
    } else {
        std::cout <<"analyzed headers: " <<analysis.ninterps<< "\n";
    }

    std::ostream &info = std::cerr; // do not include in answer because objects vary in size per architecture
    info <<"Before vacuum...\n";
    BaseSemantics::SValue::poolAllocator().showInfo(info);
    BaseSemantics::SValue::poolAllocator().vacuum();
    info <<"After vacuum...\n";
    BaseSemantics::SValue::poolAllocator().showInfo(info);

    return 0;
}

#endif
