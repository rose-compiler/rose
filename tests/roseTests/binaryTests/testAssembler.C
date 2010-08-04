/* Parses a binary file, removes a basic block, then unparses the file. The basic block to remove is specified by the first
 * command-line argument.
 *
 * usage: testAssemble BLOCK_ADDR [SWITCHES] BINARY_FILE
 */

#include "rose.h"

/* Collects all instructions from all basic blocks into a map by instruction address. */
class InstructionCollector: public SgSimpleProcessing {
public:
    Disassembler::InstructionMap insns; /*instructions by address*/
    InstructionCollector(SgNode *ast) {
        traverse(ast, preorder);
    }
    void visit(SgNode *node) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(node);
        if (insn)
            insns[insn->get_address()] = insn;
    }
};

/* Assembles all instructions of an interpretation.  ROSE allows blocks to be non-contiguous (i.e., an unconditiona jump can
 * appear in the middle of a basic block).  However, we need to disassemble contiguous instructions. */
static void
assemble_all(SgAsmInterpretation *interp)
{
    size_t nassembled = 0;
    Assembler *assembler = Assembler::create(interp);
    ROSE_ASSERT(assembler!=NULL);
    InstructionCollector collector(interp);
    for (Disassembler::InstructionMap::iterator ii=collector.insns.begin(); ii!=collector.insns.end(); ++ii) {
        rose_addr_t original_va = ii->first;

        /* The new_va is the virtual address of the instruction now that we may have moved it to a new location in memory.
         * We're leaving this implementation for later. For now, just assume that instructions don't move in memory. */
        rose_addr_t new_va = original_va;

        SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
        ROSE_ASSERT(insn!=NULL);
        SgUnsignedCharList machine_code;
        try {
            insn->set_address(new_va);
            machine_code = assembler->assembleOne(insn);
            ROSE_ASSERT(!machine_code.empty());
            ++nassembled;
        } catch (const Assembler::Exception &e) {
            std::cerr <<"assembly failed at " <<StringUtility::addrToString(e.insn->get_address())
                      <<": " <<e.mesg <<std::endl;
            if (!assembler->get_debug()) {
                assembler->set_debug(stderr);
                try {
                    assembler->assembleOne(insn);
                } catch (...) {
                    /*void*/
                }
                assembler->set_debug(false);
            }
            return;
        }

        /* We don't handle the case where an instruction grows because that could cause us to require that the section
         * containing the instruction grows, which opens a whole can of worms. */
        ROSE_ASSERT(machine_code.size() <= insn->get_raw_bytes().size());
        
        /* We're using the same memory map as what was used when we loaded the binary and disassembled it. Therefore, the
         * machine code that we're writing back needs to fall within those same areas of the virtual address space: we cannot
         * write past the end of mapped memory, nor can we write to the space (if any) between mapped memory chunks. */
        size_t nwritten = interp->get_map()->write(&(machine_code[0]), new_va, machine_code.size());
        ROSE_ASSERT(nwritten==machine_code.size());
    }

    std::cout <<"Assembled " <<nassembled <<" instruction" <<(1==nassembled?"":"s") <<"\n";
    delete assembler;
}

int
main(int argc, char *argv[])
{
    /* Process the command-line; remove the block address and pass the remainder to ROSE. */
    ROSE_ASSERT(argc>=2);
    char *rest;
    rose_addr_t removal_addr = strtoull(argv[1], &rest, 0);
    if (!*rest) {
        for (int i=2; i<=argc; i++) /*include null ptr at end of list*/
            argv[i-1]=argv[i];
        --argc;
    } else {
        std::cerr <<"warning: no basic block will be removed" <<std::endl;
    }

    SgProject *project = frontend(argc, argv);
    ROSE_ASSERT(project!=NULL);

    /* Remove the specified basic block */
    struct T1: public SgSimpleProcessing {
        rose_addr_t removal_addr;
        T1(rose_addr_t a): removal_addr(a) {}
        void visit(SgNode *node) {
            SgAsmBlock *bb = isSgAsmBlock(node);
            SgAsmFunctionDeclaration *func = bb ? isSgAsmFunctionDeclaration(bb->get_parent()) : NULL;
            if (func && bb->get_address()==removal_addr) {
                SgAsmStatementPtrList::iterator found = std::find(func->get_statementList().begin(), 
                                                                  func->get_statementList().end(),
                                                                  bb);
                ROSE_ASSERT(found!=func->get_statementList().end());
                func->get_statementList().erase(found);
                std::cout <<"removed basic block " <<StringUtility::addrToString(removal_addr) <<std::endl;
                // throw 1 /* found the one-and-only block, so we can abandon the traversal */
            }
        }
    };
    T1(removal_addr).traverse(project, preorder);

    /* Unparse the file using assembled data from above */
    std::vector<SgAsmInterpretation*> interps = SageInterface::querySubTree<SgAsmInterpretation>(project, V_SgAsmInterpretation);
    ROSE_ASSERT(!interps.empty());
    for (size_t i=0; i<interps.size(); ++i)
        assemble_all(interps[i]);
    backend(project);
}
