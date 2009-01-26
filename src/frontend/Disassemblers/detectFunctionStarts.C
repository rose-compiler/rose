#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#include "rose.h"

namespace DisassemblerCommon {

/** Marks program entry addresses (stored in the SgAsmGenericHeader) as functions. */
static void
mark_entry_targets(SgAsmInterpretation *interp,
                   std::map<uint64_t, SgAsmInstruction*> &insns,
                   FunctionStarts &func_starts)
{
    SgAsmGenericHeader *fhdr = interp->get_header();
    ROSE_ASSERT(fhdr!=NULL);
    SgRVAList entries = fhdr->get_entry_rvas();
    for (size_t i=0; i<entries.size(); i++) {
        rose_addr_t entry_rva = entries[i].get_rva();
        if (insns.find(entry_rva)!=insns.end())
            func_starts[entry_rva].reason |= SgAsmFunctionDeclaration::FUNC_ENTRY_POINT;
    }
}

/** Marks CALL targets as functions. Not all functions are called this way, but we assume that all appearances of a CALL-like
 *  instruction with a constant target which corresponds to the known address of an instruction causes that target to become
 *  the entry point of a function. */
static void
mark_call_targets(SgAsmInterpretation *interp,
                  std::map<uint64_t, SgAsmInstruction*> &insns,
                  FunctionStarts &func_starts)
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
                insns.find(callee_rva)!=insns.end())
                func_starts[callee_rva].reason |= SgAsmFunctionDeclaration::FUNC_CALL_TARGET;
        }
    }
}

/** Use the Frame Descriptor Entry Records of the ELF .eh_frame section to mark functions. */
static void
mark_eh_frames(SgAsmInterpretation *interp,
               std::map<uint64_t, SgAsmInstruction*> &insns,
               FunctionStarts &func_starts)
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
                    if (insns.find(target)!=insns.end())
                        func_starts[target].reason |= SgAsmFunctionDeclaration::FUNC_EH_FRAME;
                }
            }
        }
    }
}

/** Use symbol tables to determine function entry points. */
static void
mark_func_symbols(SgAsmInterpretation *interp,
                  std::map<uint64_t, SgAsmInstruction*> &insns,
                  FunctionStarts &func_starts)
{
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
                    func_starts[value].reason |= SgAsmFunctionDeclaration::FUNC_SYMBOL;
                }
            }
        }
    }
}

/** Use the inter-basicblock branch information to determine the length of each function, creating new functions as necessary
 *  when a suspected function is shortened.  We make the following assumptions:
 *     1. Function entry points that are already determined will remain
 *     2. The first (lowest address) basic block serves as the single entry point for a function.
 *     3. The body of the function is contiguous basic blocks.
 *     4. Each basic block that follows a function is the entry point of a subsequent function. */
static void
mark_graph_edges(SgAsmInterpretation *interp,
                 std::map<uint64_t, SgAsmInstruction*> &insns,
                 BasicBlockStarts &basicBlockStarts,
                 FunctionStarts &functionStarts)
{
    std::set<rose_addr_t, std::greater<rose_addr_t> > pending_functions; /*sorted from highest to lowest*/
    for (FunctionStarts::iterator i=functionStarts.begin(); i!=functionStarts.end(); i++)
        pending_functions.insert(i->first);

    while (!pending_functions.empty()) {
        /* Find begin/end address of highest pending function */
        rose_addr_t func_begin = *(pending_functions.begin());
        rose_addr_t func_end = (rose_addr_t)-1;
        FunctionStarts::iterator funci = functionStarts.find(func_begin);
        ROSE_ASSERT(funci!=functionStarts.end());
        if (++funci!=functionStarts.end())
            func_end = funci->first;

        /* First basic block of function under consideration is always part of that function. */
        BasicBlockStarts::iterator bbi = basicBlockStarts.find(func_begin);
        ROSE_ASSERT(bbi!=basicBlockStarts.end());

        /* Find the first subsequent basic block that can be shown to not be a part of this function.  Such blocks satisfy one
         * or more of these conditions. The first condition that is true determines the value of "called".
         *    1. We've advanced beyond the end of the basic block list (called=false), or
         *    2. This block's starting address is the starting address of another known function (called=false), or
         *    3. This block has a caller address that is less than the start of this function (called=true), or
         *    4. This block has a caller address that is beyond the assumed end of this function (called=true) */
        bool called=false;
        for (bbi++; bbi!=basicBlockStarts.end(); bbi++) {
            if (functionStarts.find(bbi->first)!=functionStarts.end())
                break; /*bb starts some other function*/
            for (std::set<rose_addr_t>::iterator ci=bbi->second.begin(); !called && ci!=bbi->second.end(); ci++)
                called = (*ci<func_begin || *ci>=func_end);
            if (called)
                break;
        }

        /* If this block isn't the start of a known function then we need to create a new function. Furthermore, we need to
         * reevaluate the boundary of this function since its implied ending address has been reduced. */
        if (called) {
            rose_addr_t bb_addr = bbi->first;
            ROSE_ASSERT(functionStarts.find(bb_addr)==functionStarts.end());
            functionStarts[bb_addr].reason = SgAsmFunctionDeclaration::FUNC_GRAPH;
            pending_functions.insert(bb_addr);
        } else {
            pending_functions.erase(func_begin);
        }
    }
}

void
detectFunctionStarts(SgAsmInterpretation *interp,
                     std::map<uint64_t, SgAsmInstruction*> &insns,
                     BasicBlockStarts &basicBlockStarts,
                     FunctionStarts &functionStarts)
{
    mark_entry_targets(interp, insns, functionStarts);
    mark_call_targets(interp, insns, functionStarts);
    mark_eh_frames(interp, insns, functionStarts);
    mark_func_symbols(interp, insns, functionStarts);

    /* All function entry points are also the starts of basic blocks. */
    for (FunctionStarts::iterator i=functionStarts.begin(); i!=functionStarts.end(); i++)
        if (basicBlockStarts.find(i->first)==basicBlockStarts.end())
            basicBlockStarts[i->first] = DisassemblerCommon::BasicBlockStarts::mapped_type();

    /* This one depends on basic block starts being consistent with function starts. */
    mark_graph_edges(interp, insns, basicBlockStarts, functionStarts);
}


} /*namespace*/
