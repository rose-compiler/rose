/* Prints information about the division of a binary executable into functions.
 *
 * Usage: binaryFunctionBoundary a.out
 *
 * Note: most of the code that was here has been migrated into src/frontend/Disassemblers/detectFunctionStarts.C on 2009-01-22. */

#define __STDC_FORMAT_MACROS
#include "rose.h"
#include "AsmFunctionIndex.h"

#include <inttypes.h>

using namespace rose::BinaryAnalysis;

/* Example partitioner that demonstrates how to write a user-defined function detector that uses at least one protected data
 * member (the instruction cache, in this case).  Note that this is a contrived example since the instruction map is also
 * available via Partitioner::get_instructions(). */
class MyPartitioner: public Partitioner {
public:
    MyPartitioner() {
        set_search(get_search() & ~SgAsmFunction::FUNC_PATTERN);
        add_function_detector(user_pattern);
    }
private:
    /* Looks for "push bp" (any word size) and makes them the start of functions. */
    static void user_pattern(Partitioner* p_, SgAsmGenericHeader* hdr) {
        if (hdr) return; /*this function doesn't depend on anything in a file header*/
        MyPartitioner *p = dynamic_cast<MyPartitioner*>(p_);
        ROSE_ASSERT(p!=NULL);
        for (InstructionMap::const_iterator ii=p->insns.begin(); ii!=p->insns.end(); ii++) {
            rose_addr_t addr = ii->first;
            SgAsmX86Instruction *insn = isSgAsmX86Instruction(ii->second);
            if (!insn || insn->get_kind()!=x86_push) continue;
            SgAsmExpressionPtrList &operands = insn->get_operandList()->get_operands();
            if (operands.size()!=1) continue;
            SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(operands[0]);
            if (!rre ||
                rre->get_descriptor().get_major() != x86_regclass_gpr ||
                rre->get_descriptor().get_minor() != x86_gpr_bp)
                continue;
            printf("Marking 0x%08"PRIx64" as the start of a function.\n", addr);
            p->add_function(addr, SgAsmFunction::FUNC_USERDEF);
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

    std::cout <<"Functions detected from binary executable:\n"
              <<AsmFunctionIndex(project).sort_by_entry_addr();
    return 0;
}
