#include "rose.h"
#include "RYices.h"

struct TestPolicy: public SymbolicSemantics::Policy {
    /* Compare all registers to see if they are all the same. */
    bool register_diff(SgAsmInstruction *insn, const SymbolicSemantics::State &orig_state) {
        using namespace SymbolicSemantics;

        InternalNode *assertion = new InternalNode(1, OP_OR);
        std::ofstream yices_input("x.yices", std::ios::trunc);
        yices_input <<";; " <<unparseInstructionWithAddress(insn) <<"\n";

        /* General purpose registers */
        for (size_t i=0; i<State::n_gprs; i++) {
            yices_input <<";;   " <<gprToString((X86GeneralPurposeRegister)i) <<"\n";
            yices_input <<";;     orig = " <<orig_state.gpr[i] <<"\n";
            yices_input <<";;     cur  = " <<get_state().gpr[i] <<"\n";
            assertion->add_child(new InternalNode(1, OP_NE, orig_state.gpr[i].expr, get_state().gpr[i].expr));
        }

        /* Segment registers */
        for (size_t i=0; i<State::n_segregs; i++) {
            yices_input <<";;   " <<segregToString((X86SegmentRegister)i) <<"\n";
            yices_input <<";;     orig = " <<orig_state.segreg[i] <<"\n";
            yices_input <<";;     cur  = " <<get_state().segreg[i] <<"\n";
            assertion->add_child(new InternalNode(1, OP_NE, orig_state.segreg[i].expr, get_state().segreg[i].expr));
        }

        /* Flags */
        for (size_t i=0; i<State::n_flags; i++) {
            yices_input <<";;   " <<flagToString((X86Flag)i) <<"\n";
            yices_input <<";;     orig = " <<orig_state.flag[i] <<"\n";
            yices_input <<";;     cur  = " <<get_state().flag[i] <<"\n";
            assertion->add_child(new InternalNode(1, OP_NE, orig_state.flag[i].expr, get_state().flag[i].expr));
        }

        RYices::Definitions defns;
        RYices::out(yices_input, assertion, &defns);
        yices_input <<"\n(check)\n";
        yices_input.close();

        /* Run yices and get the first line of stdout. */
        const char *yices_cmd = "/home/matzke/junk/SMT-solvers/yices-1.0.28/bin/yices -tc x.yices";
        FILE *yices_output = popen(yices_cmd, "r");
        ROSE_ASSERT(yices_output!=NULL);
        static char *line=NULL;
        static size_t line_alloc=0;
        ssize_t nread = getline(&line, &line_alloc, yices_output);
        ROSE_ASSERT(nread>0);
        int status = pclose(yices_output);

        /* First line should be the word "sat" or "unsat" */
        if (!strcmp(line, "sat\n"))
            return true; /*registers potentially changed from initial conditions*/
        if (!strcmp(line, "unsat\n"))
            return false; /*registers could not have changed from initial conditions*/
        std::cout <<"    exit status=" <<status <<" input=" <<line;
        execl("/bin/cat", "cat", "-n", "x.yices", NULL);
        abort(); /*probably not reached*/
    }
};

typedef X86InstructionSemantics<TestPolicy, SymbolicSemantics::ValueType> Semantics;

/* Analyze a single interpretation a block at a time */
static void
analyze_interp(SgAsmInterpretation *interp)
{
    /* Get the set of all instructions */
    struct AllInstructions: public SgSimpleProcessing, public std::map<rose_addr_t, SgAsmx86Instruction*> {
        void visit(SgNode *node) {
            SgAsmx86Instruction *insn = isSgAsmx86Instruction(node);
            if (insn) insert(std::make_pair(insn->get_address(), insn));
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

        TestPolicy policy;
        Semantics semantics(policy);
        SymbolicSemantics::State prev_state = policy.get_state();

        /* Perform semantic analysis for each instruction in this block. The block ends when we no longer know the value of
         * the instruction pointer or the instruction pointer refers to an instruction that doesn't exist or which has already
         * been processed. */
        while (1) {
            /* Analyze current instruction */
            try {
                semantics.processInstruction(insn);
                if (!policy.register_diff(insn, prev_state)) {
                    printf("%-75s no-change\n", unparseInstructionWithAddress(insn).c_str());
                } else {
                    std::cout <<unparseInstructionWithAddress(insn) <<"\n";
                }
                prev_state = policy.get_state();
            } catch (const Semantics::Exception &e) {
                std::cout <<e.mesg <<": " <<unparseInstructionWithAddress(e.insn) <<"\n";
                break;
            }

            /* Never follow CALL instructions */
            if (insn->get_kind()==x86_call || insn->get_kind()==x86_farcall)
                break;

            /* Get next instruction of this block */
            if (!policy.get_ip().is_known()) break;
            rose_addr_t next_addr = policy.get_ip().value();
            si = insns.find(next_addr);
            if (si==insns.end()) break;
            insn = si->second;
            insns.erase(si);
        }
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
}
