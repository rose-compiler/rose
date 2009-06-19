/* Algorithms to detect where functions begin in the set of machine instructions. See detectFunctionStarts() near the end of this file. */
#define __STDC_FORMAT_MACROS
#include "rose.h"
#include <inttypes.h>

/* See header file for full documentation. */

/* Partitions instructions into functions of basic blocks. */
SgAsmBlock *
Partitioner::partition(SgAsmGenericHeader *fhdr, const Disassembler::InstructionMap &insns) const
{
    BasicBlockStarts bb_starts = detectBasicBlocks(insns);
    FunctionStarts func_starts = detectFunctions(fhdr, insns, bb_starts);
    return buildTree(insns, bb_starts, func_starts);
}

/* Find the beginnings of basic blocks based on instruction type and call targets. */
Partitioner::BasicBlockStarts
Partitioner::detectBasicBlocks(const Disassembler::InstructionMap &insns) const
{
    BasicBlockStarts bb_starts;

    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SgAsmInstruction *insn = ii->second;
        rose_addr_t insn_va = insn->get_address();
        rose_addr_t next_va = insn->get_address() + insn->get_raw_bytes().size();

        /* If this instruction is one which terminates a basic block then make the next instruction (if any) the beginning of
         * a basic block. */
        if (insn->terminatesBasicBlock()) {
            Disassembler::InstructionMap::const_iterator found = insns.find(next_va);
            if (found!=insns.end() && bb_starts.find(next_va)==bb_starts.end())
                bb_starts[next_va] = BasicBlockStarts::mapped_type();
        }

        /* If this instruction has multiple known successors then make each of those successors the beginning of a basic
         * block. However, if there's only one successor and it's the fall-through address then ignore it. */
        Disassembler::AddressSet successors = insn->get_successors();
        for (Disassembler::AddressSet::const_iterator si=successors.begin(); si!=successors.end(); ++si) {
            rose_addr_t successor_va = *si;
            if (successor_va != next_va || successors.size()>1)
                bb_starts[successor_va].insert(insn_va);
        }
    }
    return bb_starts;
}

/* Find beginnings of functions. */
Partitioner::FunctionStarts
Partitioner::detectFunctions(SgAsmGenericHeader *fhdr, const Disassembler::InstructionMap &insns,
                             BasicBlockStarts &bb_starts/*out*/) const
{
    FunctionStarts func_starts;

    if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_ENTRY_POINT)
        mark_entry_targets(fhdr, insns, func_starts);
    if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_CALL_TARGET)
        mark_call_targets(fhdr, insns, func_starts);
    if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_EH_FRAME)
        mark_eh_frames(fhdr, insns, func_starts);
    if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_SYMBOL)
        mark_func_symbols(fhdr, insns, func_starts);
    if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_PATTERN)
        mark_func_patterns(fhdr, insns, func_starts);

    /* All function entry points are also the starts of basic blocks. */
    for (FunctionStarts::iterator i=func_starts.begin(); i!=func_starts.end(); i++) {
        if (bb_starts.find(i->first)==bb_starts.end())
            bb_starts[i->first] = BasicBlockStarts::mapped_type();
    }

    /* This one depends on basic block starts being consistent with function starts. */
    if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_GRAPH)
        mark_graph_edges(fhdr, insns, bb_starts, func_starts);

    /* This doesn't detect new functions, it just gives names to ELF .plt trampolines */
    name_plt_entries(fhdr, insns, func_starts);

    return func_starts;
}

/* Organize instructions into basic blocks and functions. */
SgAsmBlock *
Partitioner::buildTree(const Disassembler::InstructionMap &insns, const BasicBlockStarts &bb_starts,
                       const FunctionStarts &func_starts) const
{
    SgAsmBlock* retval = new SgAsmBlock();
    std::map<rose_addr_t, SgAsmBlock*> bbs = buildBasicBlocks(insns, bb_starts);
    std::map<rose_addr_t, SgAsmFunctionDeclaration*> funcs;

    /* Every function start must also be a basic block start. This buildTree method will work fine even if we encounter a
     * violation of this rule, but a violation probably indicates that there's bad logic elswhere. */
    for (FunctionStarts::const_iterator fsi=func_starts.begin(); fsi!=func_starts.end(); ++fsi) {
        BasicBlockStarts::const_iterator bbsi = bb_starts.find(fsi->first);
        ROSE_ASSERT(bbsi!=bb_starts.end());
    }

    /* If there are any basic blocks whose addresses are less than the first known function, then create a new function to
     * hold all such basic blocks. */
    if (bbs.size()>0 && func_starts.size()>0) {
        rose_addr_t lowest_bb_addr = bbs.begin()->first;
        rose_addr_t lowest_func_addr = func_starts.begin()->first;
        if (lowest_bb_addr < lowest_func_addr) {
            SgAsmFunctionDeclaration *f = new SgAsmFunctionDeclaration;
            f->set_address(lowest_bb_addr);
            funcs.insert(std::make_pair(lowest_bb_addr, f));
            retval->get_statementList().push_back(f);
            f->set_parent(retval);
        }
    }

    /* Build the list of functions based on their entry addresses. We'll populate them with basic blocks below. Discard
     * function starts that don't have any basic blocks (we could keep them as empty functions, but various analyses assume
     * that a function always has at least one instruction). */
    bool displayed=false;
    for (FunctionStarts::const_iterator i = func_starts.begin(); i != func_starts.end(); ++i) {
        if (bbs.find(i->first)!=bbs.end()) {
            SgAsmFunctionDeclaration* f = new SgAsmFunctionDeclaration();
            f->set_address(i->first);
            f->set_name(i->second.name);
            f->set_reason(i->second.reason);
            funcs.insert(std::make_pair(i->first, f));
            retval->get_statementList().push_back(f);
            f->set_parent(retval);
        } else {
            if (!displayed)
                fprintf(stderr, "%s: notice: discarding empty functions:", __func__);
            displayed=true;
            fprintf(stderr, " 0x%"PRIx64, i->first);
        }
    }
    if (displayed)
        fputc('\n', stderr);

    /* Add basic blocks to functions */
    for (std::map<rose_addr_t, SgAsmBlock*>::iterator bbi=bbs.begin(); bbi!=bbs.end(); ++bbi) {
        SgAsmBlock *bb = bbi->second;
        uint64_t bb_va = bb->get_address();
        ROSE_ASSERT(bb_va==bb->get_address());

        /* This is based on the algorithm in putInstructionsIntoBasicBlocks, with the same restrictions.  This is not quite as
         * good as what IDA does -- I've read that it takes control flow into account so functions can be non-contiguous in
         * memory. [Willcock r587 2008-05-30] */
        std::map<rose_addr_t, SgAsmFunctionDeclaration*>::const_iterator fi = funcs.upper_bound(bb_va);
        ROSE_ASSERT(fi!=funcs.begin());
        --fi;
        SgAsmFunctionDeclaration *func = fi->second;
        func->get_statementList().push_back(bb);
        bb->set_parent(func);
    }

    /* Make sure every function has at least one basic block */
    for (std::map<rose_addr_t, SgAsmFunctionDeclaration*>::iterator fi=funcs.begin(); fi!=funcs.end(); ++fi) {
        SgAsmFunctionDeclaration *func = fi->second;
        ROSE_ASSERT(func->get_statementList().size()>0);
    }

    return retval;
}

/* Organize instructions into basic blocks based on address of first insn in each block. */
std::map<rose_addr_t, SgAsmBlock*>
Partitioner::buildBasicBlocks(const Disassembler::InstructionMap &c_insns, const BasicBlockStarts &bb_starts) const
{
    std::map<rose_addr_t, SgAsmBlock*> retval;
    Disassembler::InstructionMap insns(c_insns);
    while (insns.size()>0) {
        /* Create the basic block */
        Disassembler::InstructionMap::iterator ii = insns.begin();
        SgAsmBlock *bb = new SgAsmBlock;
        bb->set_id(ii->first);
        bb->set_address(ii->first);
        retval.insert(std::make_pair(ii->first, bb));

        /* Insert instructions */
        while (ii!=insns.end()) {
            rose_addr_t insn_va = ii->first;
            SgAsmInstruction *insn = ii->second;
            ROSE_ASSERT(insn_va==insn->get_address());

            bb->get_statementList().push_back(insn);
            insn->set_parent(bb);
            insns.erase(ii);

            insn_va += insn->get_raw_bytes().size();
            if (insn->terminatesBasicBlock() || bb_starts.find(insn_va)!=bb_starts.end())
                break;
            ii = insns.find(insn_va);
        }
    }
    return retval;
}

/* class method */
rose_addr_t
Partitioner::value_of(SgAsmValueExpression *e)
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

/* class method */
rose_addr_t
Partitioner::get_indirection_addr(SgAsmInstruction *g_insn)
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

/* Marks program entry addresses as functions. */
void
Partitioner::mark_entry_targets(SgAsmGenericHeader *fhdr, const Disassembler::InstructionMap &insns,
                                FunctionStarts &func_starts/*out*/) const
{
    SgRVAList entries = fhdr->get_entry_rvas();
    for (size_t i=0; i<entries.size(); i++) {
        rose_addr_t entry_rva = entries[i].get_rva();
        if (insns.find(entry_rva)!=insns.end())
            func_starts[entry_rva].reason |= SgAsmFunctionDeclaration::FUNC_ENTRY_POINT;
    }
}

/* Marks CALL targets as functions. */
void
Partitioner::mark_call_targets(SgAsmGenericHeader *fhdr, const Disassembler::InstructionMap &insns,
                               FunctionStarts &func_starts/*out*/) const
{
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ii++) {
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
                bool needs_reloc = callee_rva == insn->get_address() + insn->get_raw_bytes().size();
                if (!needs_reloc)
                    func_starts[callee_rva].reason |= SgAsmFunctionDeclaration::FUNC_CALL_TARGET;
            }
        }
    }
}

/** Use the Frame Descriptor Entry Records of the ELF .eh_frame section to mark functions. */
void
Partitioner::mark_eh_frames(SgAsmGenericHeader *fhdr, const Disassembler::InstructionMap &insns,
                            FunctionStarts &func_starts/*out*/) const
{
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
void
Partitioner::mark_func_symbols(SgAsmGenericHeader *fhdr, const Disassembler::InstructionMap &insns,
                               FunctionStarts &func_starts/*out*/) const
{
    SgAsmGenericSectionList *sections = fhdr->get_sections();
    for (size_t i=0; i<sections->get_sections().size(); i++) {

        /* If this is a symbol table of some sort, then get the list of symbols. */
        std::vector<SgAsmGenericSymbol*> symbols;
        if (isSgAsmElfSymbolSection(sections->get_sections()[i])) {
            SgAsmElfSymbolList *elf_symbols = isSgAsmElfSymbolSection(sections->get_sections()[i])->get_symbols();
            for (size_t j=0; j<elf_symbols->get_symbols().size(); j++) {
                symbols.push_back(elf_symbols->get_symbols()[j]);
            }
        } else if (isSgAsmCoffSymbolTable(sections->get_sections()[i])) {
            SgAsmCoffSymbolList *coff_symbols = isSgAsmCoffSymbolTable(sections->get_sections()[i])->get_symbols();
            for (size_t j=0; j<coff_symbols->get_symbols().size(); j++) {
                symbols.push_back(coff_symbols->get_symbols()[j]);
            }
        }

        for (size_t j=0; j<symbols.size(); j++) {
            SgAsmGenericSymbol *symbol = symbols[j];
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

/* Use control flow graph to find function starts. */
void
Partitioner::mark_graph_edges(SgAsmGenericHeader *fhdr, const Disassembler::InstructionMap &insns,
                              BasicBlockStarts &basicBlockStarts/*out*/, FunctionStarts &functionStarts/*out*/) const
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

/* Look for instruction patterns */
void
Partitioner::mark_func_patterns(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                                FunctionStarts &func_starts/*out*/) const
{
    /* FIXME: not implemented yet. [RPM 2009-06-10] */
}

/* Gives names to the dynamic linking trampolines in the .plt section. */
void
Partitioner::name_plt_entries(SgAsmGenericHeader *fhdr, const Disassembler::InstructionMap &insns,
                              FunctionStarts &func_starts/*out*/) const
{
    /* This function is ELF, x86 specific. [FIXME RPM 2009-02-06] */
    SgAsmElfFileHeader *elf = isSgAsmElfFileHeader(fhdr);
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

        /* Sometimes the first instruction of a basic block cannot be disassembled and the basic block will have a different
         * starting address than its first instruction.  If that basic block is also the start of a function then the
         * function also will have no initial instruction. */
        Disassembler::InstructionMap::const_iterator ii = insns.find(func_addr);
        if (ii==insns.end())
            continue;

        /* The target in the ".plt" section will be an indirect (through the .got.plt section) jump to the actual dynamically
         * linked function (or to the dynamic linker itself). The .got.plt address is what we're really interested in. */
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
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
