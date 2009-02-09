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
                    if (func_starts[value].name=="")
                        func_starts[value].name = symbol->get_name()->get_string();
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

/* Returns the value of an SgAsmValueExpression by casting to each possible subtype since there's no virtual function to get
 * the value. */
static rose_addr_t
value_of(SgAsmValueExpression *e)
{
    if (!e) {
        return 0;
    } else if (isSgAsmWordValueExpression(e)) {
        return isSgAsmWordValueExpression(e)->get_value();
    } else if (isSgAsmDoubleWordValueExpression(e)) {
        return isSgAsmDoubleWordValueExpression(e)->get_value();
    } else if (isSgAsmQuadWordValueExpression(e)) {
        return isSgAsmQuadWordValueExpression(e)->get_value();
    } else {
        return 0;
    }
}

/* Return the virtual address that holds the branch target for an indirect branch. For example, when called with these
 * instructions:
 *     jmp DWORD PTR ds:[0x80496b0]        -> (x86)   returns 80496b0
 *     jmp QWORD PTR ds:[rip+0x200b52]     -> (amd64) returns 200b52 + address following instruction
 *
 * We only instructions that appear as the first instruction in an ELF .plt entry. */
static rose_addr_t
get_indirection_addr(SgAsmInstruction *g_insn)
{
    rose_addr_t retval = 0;

    SgAsmx86Instruction *insn = isSgAsmx86Instruction(g_insn);
    if (!insn ||
        !x86InstructionIsUnconditionalBranch(insn) ||
        1!=insn->get_operandList()->get_operands().size())
        return retval;
    
    SgAsmMemoryReferenceExpression *mref = isSgAsmMemoryReferenceExpression(insn->get_operandList()->get_operands()[0]);
    if (!mref)
        return retval;

    SgAsmExpression *mref_addr = mref->get_address();
    if (isSgAsmBinaryExpression(mref_addr)) {
        SgAsmBinaryExpression *mref_bin = isSgAsmBinaryExpression(mref_addr);
        SgAsmx86RegisterReferenceExpression *reg = isSgAsmx86RegisterReferenceExpression(mref_bin->get_lhs());
        SgAsmValueExpression *val = isSgAsmValueExpression(mref_bin->get_rhs());
        if (reg->get_register_class()==x86_regclass_ip && val!=NULL) {
            retval = value_of(val) + insn->get_address() + insn->get_raw_bytes().size();
        }
    } else if (isSgAsmValueExpression(mref_addr)) {
        retval = value_of(isSgAsmValueExpression(mref_addr));
    }

    return retval; /*calculated value, or defaults to zero*/
}

/** Give names to the dynamic linking trampolines in the .plt section. This method must be called after function entry points
 *  are detected since all it does is give names to functions that already exist. */
static void
name_plt_entries(SgAsmInterpretation *interp, 
                 std::map<uint64_t, SgAsmInstruction*> &insns,
                 FunctionStarts &func_starts)
{
    /* This function is ELF, x86 specific. [FIXME RPM 2009-02-06] */
    SgAsmElfFileHeader *elf = isSgAsmElfFileHeader(interp->get_header());
    if (!elf) return;

    /* Find important sections */
    SgAsmGenericSection *plt = elf->get_section_by_name(".plt");
    if (!plt || !plt->is_mapped()) return;
    SgAsmGenericSection *gotplt = elf->get_section_by_name(".got.plt");
    if (!gotplt || !gotplt->is_mapped()) return;

    /* Find all relocation sections */
    std::set<SgAsmElfRelocSection*> rsects;
    for (SgAsmGenericSectionPtrList::iterator si=elf->get_sections()->get_sections().begin();
         si!=elf->get_sections()->get_sections().end(); 
         si++) {
        SgAsmElfRelocSection *reloc_section = isSgAsmElfRelocSection(*si);
        if (reloc_section) {
            ROSE_ASSERT(isSgAsmElfSymbolSection(reloc_section->get_linked_section()));
            rsects.insert(reloc_section);
        }
    }
    if (rsects.empty()) return;

    /* Process each .plt trampoline */
    for (FunctionStarts::iterator fi=func_starts.begin(); fi!=func_starts.end(); fi++) {
        rose_addr_t func_addr = fi->first;

        if (fi->second.name!="")
            continue; /* function already has a name */

        if (func_addr <  elf->get_base_va() + plt->get_mapped_rva() &&
            func_addr >= elf->get_base_va() + plt->get_mapped_rva() + plt->get_mapped_size())
            continue; /* function is not in the .plt section */
            
        /* The target in the ".plt" section will be an indirect (through the .got.plt section) jump to the actual dynamically
         * linked function (or to the dynamic linker itself). The .got.plt address is what we're really interested in. */
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(insns[func_addr]);
        rose_addr_t gotplt_va = get_indirection_addr(insn);

        if (gotplt_va <  elf->get_base_va() + gotplt->get_mapped_rva() ||
            gotplt_va >= elf->get_base_va() + gotplt->get_mapped_rva() + gotplt->get_mapped_size())
            continue; /* PLT entry doesn't dereference a value in the .got.plt section */
        
        /* Find the relocation entry whose offset is the gotplt_rva and use that entries symbol for the function name. */
        for (std::set<SgAsmElfRelocSection*>::iterator ri=rsects.begin(); ri!=rsects.end() && fi->second.name==""; ri++) {
            SgAsmElfRelocEntryList *entries = (*ri)->get_entries();
            SgAsmElfSymbolSection *symbol_section = isSgAsmElfSymbolSection((*ri)->get_linked_section());
            SgAsmElfSymbolList *symbols = symbol_section->get_symbols();
            for (size_t ei=0; ei<entries->get_entries().size() && fi->second.name==""; ei++) {
                SgAsmElfRelocEntry *rel = entries->get_entries()[ei];
                if (rel->get_r_offset()==gotplt_va) {
                    unsigned long symbol_idx = rel->get_sym();
                    ROSE_ASSERT(symbol_idx < symbols->get_symbols().size());
                    SgAsmElfSymbol *symbol = symbols->get_symbols()[symbol_idx];
                    fi->second.name = symbol->get_name()->get_string();
                }
            }
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

    /* This doesn't detect new functions, it just gives names to ELF .plt trampolines */
    name_plt_entries(interp, insns, functionStarts);
}


} /*namespace*/
