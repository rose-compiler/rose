/* Prints information about the division of a binary executable into functions.
 *
 * Usage: binaryFunctionBoundary a.out
 *
 * Note: most of the code that was here has been migrated into src/frontend/Disassemblers/detectFunctionStarts.C on 2009-01-22. */

#define __STDC_FORMAT_MACROS
#include "rose.h"
#include <inttypes.h>


class ShowFunctions : public SgSimpleProcessing {
    public:
    ShowFunctions()
        : nfuncs(0)
        {}
    size_t nfuncs;
    void visit(SgNode *node) {
        SgAsmFunctionDeclaration *defn = isSgAsmFunctionDeclaration(node);
        if (defn) {
            /* Scan through the function's instructions to find the range of addresses for the function. */
            rose_addr_t func_start=~(rose_addr_t)0, func_end=0;
            size_t ninsns=0, nbytes=0;
            SgAsmStatementPtrList func_stmts = defn->get_statementList();
            for (size_t i=0; i<func_stmts.size(); i++) {
                SgAsmBlock *bb = isSgAsmBlock(func_stmts[i]);
                if (bb) {
                    SgAsmStatementPtrList block_stmts = bb->get_statementList();
                    for (size_t j=0; j<block_stmts.size(); j++) {
                        SgAsmInstruction *insn = isSgAsmInstruction(block_stmts[j]);
                        if (insn) {
                            ninsns++;
                            func_start = std::min(func_start, insn->get_address());
                            func_end = std::max(func_end, insn->get_address()+insn->get_raw_bytes().size());
                            nbytes += insn->get_raw_bytes().size();
                        }
                    }
                }
            }

            /* Reason that this is a function */
            printf("    %3zu 0x%08"PRIx64" 0x%08"PRIx64" %5zu/%-6zu ", ++nfuncs, func_start, func_end, ninsns, nbytes);
            fputs(defn->reason_str(true).c_str(), stdout);

            /* Kind of function */
            switch (defn->get_function_kind()) {
              case SgAsmFunctionDeclaration::e_unknown:    fputs("  unknown", stdout); break;
              case SgAsmFunctionDeclaration::e_standard:   fputs(" standard", stdout); break;
              case SgAsmFunctionDeclaration::e_library:    fputs("  library", stdout); break;
              case SgAsmFunctionDeclaration::e_imported:   fputs(" imported", stdout); break;
              case SgAsmFunctionDeclaration::e_thunk:      fputs("    thunk", stdout); break;
              default:                                     fputs("    other", stdout); break;
            }

            /* Function name if known */
            if (defn->get_name()!="")
                printf(" %s", defn->get_name().c_str());
            fputc('\n', stdout);
        }
    }
};

/* Example partitioner that demonstrates how to write a user-defined function detector that uses at least one protected data
 * member (the instruction cache, in this case).  Note that this is a contrived example since the instruction map is also
 * available via Partitioner::get_instructions(). */
class MyPartitioner: public Partitioner {
public:
    MyPartitioner() {
        set_search(get_search() & ~SgAsmFunctionDeclaration::FUNC_PATTERN);
        add_function_detector(user_pattern);
    }
private:
    /* Looks for "push bp" (any word size) and makes them the start of functions. */
    static void user_pattern(Partitioner* p_, SgAsmGenericHeader* hdr) {
        if (hdr) return; /*this function doesn't depend on anything in a file header*/
        MyPartitioner *p = dynamic_cast<MyPartitioner*>(p_);
        ROSE_ASSERT(p!=NULL);
        for (Disassembler::InstructionMap::const_iterator ii=p->insns.begin(); ii!=p->insns.end(); ii++) {
            rose_addr_t addr = ii->first;
            SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
            if (!insn || insn->get_kind()!=x86_push) continue;
            SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
            if (operands.size()!=1) continue;
            SgAsmx86RegisterReferenceExpression *rre = isSgAsmx86RegisterReferenceExpression(operands[0]);
            if (!rre ||
                rre->get_descriptor().get_major() != x86_regclass_gpr ||
                rre->get_descriptor().get_minor() != x86_gpr_bp)
                continue;
            printf("Marking 0x%08"PRIx64" as the start of a function.\n", addr);
            p->add_function(addr, SgAsmFunctionDeclaration::FUNC_USERDEF);
        }
    }
};

/* Example disassembler to replace the ROSE-defined x86 disassembler.  The example disassembler is identical in every respect
 * except it defines its own partitioner. */
class MyDisassembler: public DisassemblerX86 {
public:
    MyDisassembler(size_t wordsize)
        : DisassemblerX86(wordsize) {
        set_partitioner(new MyPartitioner());
    }
};

int
main(int argc, char *argv[])
{
#if 0
    /* Here's an example of how to influence the default disassembly. When ROSE needs a disassembler it calls
     * Disassembler::lookup(), which asks MyDisassembler if it can handle that kind of architecture. If it can, then ROSE uses
     * it for disassembly.  We register three instances of MyDisassembler because we know that DisassemblerX86 can handle
     * these three word sizes. */
    Disassembler::register_subclass(new MyDisassembler(2));
    Disassembler::register_subclass(new MyDisassembler(4));
    Disassembler::register_subclass(new MyDisassembler(8));
#endif

    SgProject *project = frontend(argc, argv);

    printf("Functions detected from binary executable:\n");
    printf("    Key for reason(s) address is a suspected function:\n");
    printf("      E = entry address         C = call target           X = exception frame\n");
    printf("      S = function symbol       P = instruction pattern   G = interblock branch graph\n");
    printf("      U = user-def detection    N = NOP/Zero padding      D = discontiguous blocks\n");
    printf("      H = insn sequence head\n");
    printf("\n");
    printf("    Num  Low-Addr   End-Addr  Insns/Bytes   Reason      Kind   Name\n");
    printf("    --- ---------- ---------- ------------ --------- -------- --------------------------------\n");
    ShowFunctions().traverseInputFiles(project, preorder);
    printf("    --- ---------- ---------- ------------ --------- -------- --------------------------------\n");
    return 0;
}
