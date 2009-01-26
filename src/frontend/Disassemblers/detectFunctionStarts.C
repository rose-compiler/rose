#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "rose.h"

#define ENTRY_POINT "E"
#define CALL_TARGET "C"
#define EH_FRAME    "F"
#define FUNC_SYMBOL "S"

/** Marks program entry addresses (stored in the SgAsmGenericHeader) as functions. */
static void
mark_entry_targets(SgAsmInterpretation *interp,
                   std::map<uint64_t, SgAsmInstruction*> &insns,
                   std::map<uint64_t, bool> &bb_starts,
                   std::map<uint64_t, std::string> &func_starts)
{
    SgAsmGenericHeader *fhdr = interp->get_header();
    ROSE_ASSERT(fhdr!=NULL);
    SgRVAList entries = fhdr->get_entry_rvas();
    for (size_t i=0; i<entries.size(); i++) {
        rose_addr_t entry_rva = entries[i].get_rva();
        if (insns.find(entry_rva)!=insns.end()) {
            bb_starts[entry_rva] = true;
            func_starts[entry_rva] += ENTRY_POINT;
        }
    }
}

/** Marks CALL targets as functions. Not all functions are called this way, but we assume that all appearances of a CALL-like
 *  instruction with a constant target which corresponds to the known address of an instruction causes that target to become
 *  the entry point of a function. */
static void
mark_call_targets(SgAsmInterpretation *interp,
                  std::map<uint64_t, SgAsmInstruction*> &insns,
                  std::map<uint64_t, bool> &bb_starts,
                  std::map<uint64_t, std::string> &func_starts)
{
    std::map<uint64_t, SgAsmInstruction*>::iterator ii;
    for (ii=insns.begin(); ii!=insns.end(); ii++) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);

        /* FIXME [RPM 2009-01-16] */
        static bool reported=false;
        if (!insn && !reported) {
            fprintf(stderr, "%s:%u: warning: we only handle x86 instructions at this time.\n", __FILE__, __LINE__);
            reported=true;
        }

        if (insn && (x86_call==insn->get_kind() || x86_farcall==insn->get_kind())) {
            rose_addr_t callee_rva = 0;
            if (x86GetKnownBranchTarget(insn, callee_rva/*out*/) &&
                insns.find(callee_rva)!=insns.end()) {
                bb_starts[callee_rva] = true;
                func_starts[callee_rva] += CALL_TARGET;
            }
        }
    }
}

/** Use the Frame Descriptor Entry Records of the ELF .eh_frame section to mark functions. */
static void
mark_eh_frames(SgAsmInterpretation *interp,
               std::map<uint64_t, SgAsmInstruction*> &insns,
               std::map<uint64_t, bool> &bb_starts,
               std::map<uint64_t, std::string> &func_starts)
{
    SgAsmGenericHeader *fhdr = interp->get_header();
    SgAsmGenericSectionList *sections = fhdr->get_sections();
    for (size_t i=0; i<sections->get_sections().size(); i++) {
        SgAsmElfEHFrameSection *ehframe = isSgAsmElfEHFrameSection(sections->get_sections()[i]);
        if (ehframe!=NULL) {
            SgAsmElfEHFrameEntryCIList *ci_entries = ehframe->get_ci_entries();
            for (size_t j=0; j<ci_entries->get_entries().size(); j++) {
                SgAsmElfEHFrameEntryCI *cie = ci_entries->get_entries()[j];
                SgAsmElfEHFrameEntryFDList *fd_entries = cie->get_fd_entries();
                for (size_t k=0; k<fd_entries->get_entries().size(); k++) {
                    SgAsmElfEHFrameEntryFD *fde = fd_entries->get_entries()[k];
                    rose_addr_t target = fde->get_begin_rva().get_rva();
                    if (insns.find(target)!=insns.end()) {
                        bb_starts[target] = true;
                        func_starts[target] += EH_FRAME;
                    }
                }
            }
        }
    }
}

/** Use symbol tables to determine function entry points. */
static void
mark_func_symbols(SgAsmInterpretation *interp,
                  std::map<uint64_t, SgAsmInstruction*> &insns,
                  std::map<uint64_t, bool> &bb_starts,
                  std::map<uint64_t, std::string> &func_starts)
{
    /*FIXME: We could glean even more info from the symbol table since the symbols
     *       also contain information about where the end of each function is located. [RPM 2009-01-16] */
    SgAsmGenericHeader *fhdr = interp->get_header();

    /* FIXME [RPM 2009-01-16] */
    static bool reported=false;
    if (!isSgAsmElfFileHeader(fhdr) && !reported) {
        fprintf(stderr, "%s:%u: we only handle ELF symbol tables at this time.\n", __FILE__, __LINE__);
        reported = true;
    }

    SgAsmGenericSectionList *sections = fhdr->get_sections();
    for (size_t i=0; i<sections->get_sections().size(); i++) {
        SgAsmElfSymbolSection *symsec = isSgAsmElfSymbolSection(sections->get_sections()[i]);
        if (symsec!=NULL) {
            SgAsmElfSymbolList *symbols = symsec->get_symbols();
            for (size_t j=0; j<symbols->get_symbols().size(); j++) {
                SgAsmGenericSymbol *symbol = symbols->get_symbols()[j];
                if (symbol->get_def_state()==SgAsmGenericSymbol::SYM_DEFINED &&
                    symbol->get_type()==SgAsmGenericSymbol::SYM_FUNC &&
                    insns.find(symbol->get_value())!=insns.end()) {
                    rose_addr_t value = symbol->get_value();
                    bb_starts[value] = true;
                    func_starts[value] += FUNC_SYMBOL;
                }
            }
        }
    }
}
                    
void
Disassembler::detectFunctionStarts(SgAsmInterpretation *interp,
                                   std::map<uint64_t, SgAsmInstruction*> &insns,
                                   std::map<uint64_t, bool> &basicBlockStarts,
                                   std::map<uint64_t, std::string> &functionStarts)
{
    mark_entry_targets(interp, insns, basicBlockStarts, functionStarts);
    mark_call_targets(interp, insns, basicBlockStarts, functionStarts);
    mark_eh_frames(interp, insns, basicBlockStarts, functionStarts);
    mark_func_symbols(interp, insns, basicBlockStarts, functionStarts);
}









#if 0

/* Add edges to the call graph based on the last instruction of each block */
static void
add_insn_edges(map<uint64_t, SgAsmBlock*> &bblocks) {
    for (iterator i=bblocks.begin(); i!=bblocks.end(); i++) {
        BasicBlock *bb = i->second;
        SgAsmStatementPtrList insn_list = bb->block_node->get_statementList();
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(insn_list.back());
        bool call_target = (x86_call==insn->get_kind() || x86_farcall==insn->get_kind());

        rose_addr_t callee_rva = 0;
        if (x86GetKnownBranchTarget(insn, callee_rva)) {
            BasicBlock *callee = lookup_by_rva(callee_rva);
            if (callee) {
                bb->add_callee(callee);
                callee->add_caller(bb, call_target);
            }
        }

        if (!x86InstructionIsUnconditionalBranch(insn)) {
            callee_rva = bb->get_address() + bb->get_size();
            BasicBlock *callee = lookup_by_rva(callee_rva);
            if (callee) {
                bb->add_callee(callee);
                callee->add_caller(bb, false);
            }
        }
    }
}


/** Assigns basic blocks (SgAsmBlock) to newly-created function definitions (SgAsmFunctionDeclaration). */
std::vector<SgAsmFunctionDeclaration*>
assignBlocksToFunctions(map<uint64_t, SgAsmBlock*> &basicBlocks, SgAsmGenericHeader *fhdr)
{
    map<uint64_t, std::string> functionStarts;
    mark_entry_points(basicBlocks, fhdr, &function_starts);
}

#endif
