/* For each function (SgAsmFunction) process each instruction (SgAsmInstruction) through the instruction semantics
 * layer using the FindConstantsPolicy. Output consists of each instruction followed by the registers and memory locations
 * with constant or pseudo-constant values. */

#define __STDC_FORMAT_MACROS
#include "rose.h"
#include <set>
#include <inttypes.h>

// SEMANTIC_DOMAIN values
#define NULL_DOMAIN 1
#define PARTSYM_DOMAIN 2
#define SYMBOLIC_DOMAIN 3
#define INTERVAL_DOMAIN 4
#define MULTI_DOMAIN 5
#define FINDCONST_DOMAIN 6
#define FINDCONSTABI_DOMAIN 7

// SEMANTIC_API values
#define OLD_API 1
#define NEW_API 2

// SMT_SOLVER values
#define NO_SOLVER 0
#define YICES_LIB 1
#define YICES_EXE 2

#if !defined(SEMANTIC_API)
#   error "SEMANTIC_API must be defined on the compiler command line"
#elif SEMANTIC_API == OLD_API
#   include "x86InstructionSemantics.h"
    using namespace BinaryAnalysis::InstructionSemantics;
#elif SEMANTIC_API == NEW_API
#   include "DispatcherX86.h"
    using namespace BinaryAnalysis::InstructionSemantics2;
#else
#   error "invalid value for SEMANTIC_API"
#endif

#if !defined(SMT_SOLVER) || SMT_SOLVER == NO_SOLVER
#   include "SMTSolver.h"
    SMTSolver *make_solver() { return NULL; }
#elif SMT_SOLVER == YICES_LIB
#   include "YicesSolver.h"
    SMTSolver *make_solver() {
        YicesSolver *solver = new YicesSolver;
        solver->set_linkage(YicesSolver::LM_LIBRARY);
        return solver;
    }
#elif SMT_SOLVER == YICES_EXE
#   include "YicesSolver.h"
    SMTSolver *make_solver() {
        YicesSolver *solver = new YicesSolver;
        solver->set_linkage(YicesSolver::LM_EXECUTABLE);
        return solver;
    }
#else
#   error "invalid value for SMT_SOLVER"
#endif

const RegisterDictionary *regdict = RegisterDictionary::dictionary_i386();

#ifdef TRACE
#include "TraceSemantics2.h"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if SEMANTIC_DOMAIN == NULL_DOMAIN

#if SEMANTIC_API == OLD_API
#   include "NullSemantics.h"
#   define MyValueType BinaryAnalysis::InstructionSemantics::NullSemantics::ValueType
#   define MyState     BinaryAnalysis::InstructionSemantics::NullSemantics::State
#   define MyPolicy    BinaryAnalysis::InstructionSemantics::NullSemantics::Policy<MyState, MyValueType>
#else
#   include "NullSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        return NullSemantics::RiscOperators::instance(regdict);
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif  SEMANTIC_DOMAIN == PARTSYM_DOMAIN
#if SEMANTIC_API == OLD_API
#   include "PartialSymbolicSemantics.h"
#   define MyValueType BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::ValueType
#   define MyState     BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::State
#   define MyPolicy    BinaryAnalysis::InstructionSemantics::PartialSymbolicSemantics::Policy<MyState, MyValueType>
#else
#   include "PartialSymbolicSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        return PartialSymbolicSemantics::RiscOperators::instance(regdict);
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif  SEMANTIC_DOMAIN == SYMBOLIC_DOMAIN

#if SEMANTIC_API == OLD_API
#   include "SymbolicSemantics.h"
#   define MyValueType BinaryAnalysis::InstructionSemantics::SymbolicSemantics::ValueType
#   define MyState     BinaryAnalysis::InstructionSemantics::SymbolicSemantics::State
#   define MyPolicy    BinaryAnalysis::InstructionSemantics::SymbolicSemantics::Policy<MyState, MyValueType>
#else
#   include "SymbolicSemantics2.h"
    static BaseSemantics::RiscOperatorsPtr make_ops() {
        SymbolicSemantics::RiscOperatorsPtr retval = SymbolicSemantics::RiscOperators::instance(regdict);
#ifndef TRACE // usedef AND tracing is a little much for the eyes
        retval->set_compute_usedef();
#endif
        return retval;
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == INTERVAL_DOMAIN
#if SEMANTIC_API == OLD_API
#   include "IntervalSemantics.h"
#   define MyValueType BinaryAnalysis::InstructionSemantics::IntervalSemantics::ValueType
#   define MyState     BinaryAnalysis::InstructionSemantics::IntervalSemantics::State
#   define MyPolicy    BinaryAnalysis::InstructionSemantics::IntervalSemantics::Policy<MyState, MyValueType>
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
        PartialSymbolicSemantics::RiscOperatorsPtr s1 = PartialSymbolicSemantics::RiscOperators::instance(regdict);
        SymbolicSemantics::RiscOperatorsPtr s2 = SymbolicSemantics::RiscOperators::instance(regdict);
        s2->set_compute_usedef();
        IntervalSemantics::RiscOperatorsPtr s3 = IntervalSemantics::RiscOperators::instance(regdict);
        ops->add_subdomain(s1, "PartialSymbolic");
        ops->add_subdomain(s2, "Symbolic");
        ops->add_subdomain(s3, "Interval");
        return ops;
    }
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif SEMANTIC_DOMAIN == FINDCONST_DOMAIN

#if SEMANTIC_API == OLD_API
#   include "findConstants.h"
#   define MyValueType XVariablePtr
    struct MyPolicy: public FindConstantsPolicy {
        void startInstruction(SgAsmInstruction *insn) {
            addr = insn->get_address();
            newIp = number<32>(addr);
            if (rsets.find(addr)==rsets.end())
                rsets[addr].setToBottom();
            cur_state = rsets[addr];
            currentInstruction = isSgAsmx86Instruction(insn);
        }
        void print(std::ostream &out) {
            out <<cur_state <<"    ip = " <<newIp <<"\n";
        }
    };

#else
#   error "FindConstantsPolicy has no counterpart in the new InstructionSemantics2 API"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#elif  SEMANTIC_DOMAIN == FINDCONSTABI_DOMAIN

#if SEMANTIC_API == OLD_API
#   include "findConstants.h"
#   define MyValueType XVariablePtr
    struct MyPolicy: public FindConstantsABIPolicy {
        void startInstruction(SgAsmInstruction *insn) {
            addr = insn->get_address();
            newIp = number<32>(addr);
            if (rsets.find(addr)==rsets.end())
                rsets[addr].setToBottom();
            cur_state = rsets[addr];
            currentInstruction = isSgAsmx86Instruction(insn);
        }
        void print(std::ostream &out) {
            out <<cur_state <<"    ip = " <<newIp <<"\n";
        }
    };

#else
#   error "FindConstantsABIPolicy has no counterpart in the new InstructionSematnics2 API"
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#else
#error "invalid value for SEMANTIC_DOMAIN"
#endif

#if SEMANTIC_API == NEW_API
// Show the register state for BaseSemantics::RegisterStateGeneric in the same format as for RegisterStateX86. This is
// for comparison of the two register states when verifying results.  It's also close to the format used by the old binary
// semantics API.
void
show_state(const BaseSemantics::RiscOperatorsPtr &ops)
{
#if SEMANTIC_DOMAIN == MULTI_DOMAIN
    std::cout <<*ops;
    return;
#endif

    struct ShowReg {
        BaseSemantics::RiscOperatorsPtr ops;
        std::ostream &o;
        std::string prefix;

        ShowReg(const BaseSemantics::RiscOperatorsPtr &ops, std::ostream &o, const std::string &prefix)
            : ops(ops), o(o), prefix(prefix) {}

        void operator()(const char *name, const char *abbr=NULL) {
            const RegisterDictionary *regdict = ops->get_state()->get_register_state()->get_register_dictionary();
            const RegisterDescriptor *desc = regdict->lookup(name);
            assert(desc);
            (*this)(*desc, abbr?abbr:name);
        }
        void operator()(const RegisterDescriptor &desc, const char *abbr) {
            BaseSemantics::RegisterStatePtr regstate = ops->get_state()->get_register_state();
            FormatRestorer fmt(o);
            o <<prefix <<std::setw(8) <<std::left <<abbr <<"= { ";
            fmt.restore();
            BaseSemantics::SValuePtr val = regstate->readRegister(desc, ops.get());
            o <<*val <<" }\n";
        }
        void operator()(unsigned majr, unsigned minr, unsigned offset, unsigned nbits, const char *abbr) {
            (*this)(RegisterDescriptor(majr, minr, offset, nbits), abbr);
        }
    } show(ops, std::cout, "    ");

    std::cout <<"registers:\n";
    show("eax",         "ax");
    show("ecx",         "cx");
    show("edx",         "dx");
    show("ebx",         "bx");
    show("esp",         "sp");
    show("ebp",         "bp");
    show("esi",         "si");
    show("edi",         "di");
    show("es");
    show("cs");
    show("ss");
    show("ds");
    show("fs");
    show("gs");
    show("cf");
    show(x86_regclass_flags, 0, 1, 1, "?1");
    show("pf");
    show(x86_regclass_flags, 0, 3, 1, "?3");
    show("af");
    show(x86_regclass_flags, 0, 5, 1, "?5");
    show("zf");
    show("sf");
    show("tf");
    show("if");
    show("df");
    show("of");
    show(x86_regclass_flags, 0, 12, 1, "iopl0");
    show(x86_regclass_flags, 0, 13, 1, "iopl1");
    show("nt");
    show(x86_regclass_flags, 0, 15, 1, "?15");
    show("rf");
    show("vm");
    show(x86_regclass_flags, 0, 18, 1, "ac");
    show(x86_regclass_flags, 0, 19, 1, "vif");
    show(x86_regclass_flags, 0, 20, 1, "vip");
    show(x86_regclass_flags, 0, 21, 1, "id");
    show(x86_regclass_flags, 0, 22, 1, "?22");
    show(x86_regclass_flags, 0, 23, 1, "?23");
    show(x86_regclass_flags, 0, 24, 1, "?24");
    show(x86_regclass_flags, 0, 25, 1, "?25");
    show(x86_regclass_flags, 0, 26, 1, "?26");
    show(x86_regclass_flags, 0, 27, 1, "?27");
    show(x86_regclass_flags, 0, 28, 1, "?28");
    show(x86_regclass_flags, 0, 29, 1, "?29");
    show(x86_regclass_flags, 0, 30, 1, "?30");
    show(x86_regclass_flags, 0, 31, 1, "?31");
    show("eip", "ip");

    std::cout <<"memory:\n";
    ops->get_state()->print_memory(std::cout, "    ");
}
#endif


/* Analyze a single interpretation a block at a time */
static void
analyze_interp(SgAsmInterpretation *interp)
{
    /* Get the set of all instructions except instructions that are part of left-over blocks. */
    struct AllInstructions: public SgSimpleProcessing, public std::map<rose_addr_t, SgAsmx86Instruction*> {
        void visit(SgNode *node) {
            SgAsmx86Instruction *insn = isSgAsmx86Instruction(node);
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
        SgAsmx86Instruction *insn = si->second;
        insns.erase(si);

#if SEMANTIC_API == NEW_API
        typedef BaseSemantics::DispatcherPtr MyDispatcher;
        BaseSemantics::RiscOperatorsPtr operators = make_ops();
#ifdef TRACE
        TraceSemantics::RiscOperatorsPtr trace = TraceSemantics::RiscOperators::instance(operators);
        trace->set_stream(stdout);
        MyDispatcher dispatcher = DispatcherX86::instance(trace);
#else
        MyDispatcher dispatcher = DispatcherX86::instance(operators);
#endif
        operators->set_solver(make_solver());
#else   // OLD_API
        typedef X86InstructionSemantics<MyPolicy, MyValueType> MyDispatcher;
        MyPolicy operators;
        MyDispatcher dispatcher(operators);
#   if SEMANTIC_DOMAIN == SYMBOLIC_DOMAIN
        operators.set_solver(make_solver());
#   endif
#endif

#if SEMANTIC_DOMAIN == SYMBOLIC_DOMAIN && SEMANTIC_API == NEW_API && defined(TRACE)
        // Only request the orig_esp if we're going to use it later because it causes an esp value to be instantiated
        // in the state, which is printed in the output, and thus changes the answer.
        BaseSemantics::SValuePtr orig_esp = operators->readRegister(*regdict->lookup("esp"));
#endif

        /* Perform semantic analysis for each instruction in this block. The block ends when we no longer know the value of
         * the instruction pointer or the instruction pointer refers to an instruction that doesn't exist or which has already
         * been processed. */
        while (1) {
            /* Analyze current instruction */
            std::cout <<unparseInstructionWithAddress(insn) <<"\n";
#if SEMANTIC_API == NEW_API
            try {
                dispatcher->processInstruction(insn);
#if 0 /*DEBUGGING [Robb P. Matzke 2013-05-01]*/
                show_state(operators);
#else
                std::cout <<*operators;
#endif
            } catch (const BaseSemantics::Exception &e) {
                std::cout <<e <<"\n";
            }
#else       // OLD API
            try {
                dispatcher.processInstruction(insn);
                operators.print(std::cout);
            } catch (const MyDispatcher::Exception &e) {
                std::cout <<e <<"\n";
                break;
#   if SEMANTIC_DOMAIN == PARTSYM_DOMAIN
            } catch (const MyPolicy::Exception &e) {
                std::cout <<e <<"\n";
                break;
#   endif
            } catch (const SMTSolver::Exception &e) {
                std::cout <<e <<" [ "<<unparseInstructionWithAddress(insn) <<"]\n";
                break;
            }
#endif

            /* Never follow CALL instructions */
            if (insn->get_kind()==x86_call || insn->get_kind()==x86_farcall)
                break;

            /* Get next instruction of this block */
#if SEMANTIC_API == NEW_API
            BaseSemantics::SValuePtr ip = operators->readRegister(dispatcher->findRegister("eip"));
            if (!ip->is_number())
                break;
            rose_addr_t next_addr = ip->get_number();
#else       // OLD_API
#   if SEMANTIC_DOMAIN == PARTSYM_DOMAIN || SEMANTIC_DOMAIN == SYMBOLIC_DOMAIN
            MyValueType<32> ip = operators.get_ip();
            if (!ip.is_known()) break;
            rose_addr_t next_addr = ip.known_value();
#   elif SEMANTIC_DOMAIN == NULL_DOMAIN || SEMANTIC_DOMAIN == INTERVAL_DOMAIN
            MyValueType<32> ip = operators.readRegister<32>(dispatcher.REG_EIP);
            if (!ip.is_known()) break;
            rose_addr_t next_addr = ip.known_value();
#   elif SEMANTIC_DOMAIN == MULTI_DOMAIN
            PartialSymbolicSemantics::ValueType<32> ip = operators.readRegister<32>(dispatcher.REG_EIP)
                                                         .get_subvalue(MyMultiSemanticsClass::SP0());
            if (!ip.is_known()) break;
            rose_addr_t next_addr = ip.known_value();
#   else
            if (operators.newIp->get().name) break;
            rose_addr_t next_addr = operators.newIp->get().offset;
#   endif
#endif
            si = insns.find(next_addr);
            if (si==insns.end()) break;
            insn = si->second;
            insns.erase(si);
        }

        // Test substitution on the symbolic state.
#if SEMANTIC_DOMAIN == SYMBOLIC_DOMAIN && SEMANTIC_API == NEW_API && defined(TRACE)
        SymbolicSemantics::SValuePtr from = SymbolicSemantics::SValue::promote(orig_esp);
        BaseSemantics::SValuePtr newvar = operators->undefined_(32);
        newvar->set_comment("frame_pointer");
        SymbolicSemantics::SValuePtr to = SymbolicSemantics::SValue::promote(operators->add(newvar, operators->number_(32, 4)));
        std::cout <<"Substituting from " <<*from <<" to " <<*to <<"\n";
        SymbolicSemantics::RiscOperators::promote(operators)->substitute(from, to);
        std::cout <<"Substituted state:\n" <<*operators;
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
    SgProject *project = frontend(argc, argv);
    AnalyzeX86Functions analysis;
    analysis.traverse(project, postorder);
    if (0==analysis.ninterps) {
        std::cout <<"file(s) didn't have any 32-bit x86 headers.\n";
    } else {
        std::cout <<"analyzed headers: " <<analysis.ninterps<< "\n";
    }
    return 0;
}
