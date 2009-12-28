/* Algorithms to detect what instructions make up basic blocks (see detectBasicBlocks()), what basic blocks make up
 * functions (see detectFunctions()), and how to create the resulting SgAsmBlock and SgAsmFunctionDeclaration IR nodes from
 * this information (see buildTree()). */
#define __STDC_FORMAT_MACROS
#include "rose.h"
#include <inttypes.h>

/* See header file for full documentation. */

static void
add_to_reason_string(std::string &result, bool isset, bool do_pad, const std::string &abbr, const std::string &full) {
    if (isset) {
        if (do_pad) {
            result += abbr;
        } else {
            if (result.size()>0) result += ", ";
            result += full;
        }
    } else if (do_pad) {
        result += ".";
    }
}

/* This has no home, so it's here for now. */
std::string
SgAsmFunctionDeclaration::reason_str(bool do_pad) const
{
    std::string result;
    unsigned r = get_reason();

    /* entry point and instruction heads are mutually exclusive, so we use the same column for both when padding. */
    if (r & SgAsmFunctionDeclaration::FUNC_ENTRY_POINT) {
        add_to_reason_string(result, true, do_pad, "E", "entry point");
    } else {
        add_to_reason_string(result, (r & SgAsmFunctionDeclaration::FUNC_INSNHEAD), do_pad, "H", "insn head");
    }
    
    add_to_reason_string(result, (r & SgAsmFunctionDeclaration::FUNC_CALL_TARGET), do_pad, "C", "call target");
    add_to_reason_string(result, (r & SgAsmFunctionDeclaration::FUNC_EH_FRAME),    do_pad, "X", "exception frame");
    add_to_reason_string(result, (r & SgAsmFunctionDeclaration::FUNC_SYMBOL),      do_pad, "S", "symbol");
    add_to_reason_string(result, (r & SgAsmFunctionDeclaration::FUNC_PATTERN),     do_pad, "P", "pattern");
    add_to_reason_string(result, (r & SgAsmFunctionDeclaration::FUNC_GRAPH),       do_pad, "G", "graph");
    add_to_reason_string(result, (r & SgAsmFunctionDeclaration::FUNC_USERDEF),     do_pad, "U", "user defined");
    add_to_reason_string(result, (r & SgAsmFunctionDeclaration::FUNC_INTERPAD),    do_pad, "N", "NOP padding");
    add_to_reason_string(result, (r & SgAsmFunctionDeclaration::FUNC_DISCONT),     do_pad, "D", "discontiguous");
    return result;
}

/* Partitions instructions into functions of basic blocks. */
SgAsmBlock *
Partitioner::partition(SgAsmInterpretation *interp, const Disassembler::InstructionMap &insns) const
{
    BasicBlockStarts bb_starts = detectBasicBlocks(insns);
    FunctionStarts func_starts = detectFunctions(interp, insns, bb_starts);
    return buildTree(insns, bb_starts, func_starts);
}

/* Find the beginnings of basic blocks based on instruction type and call targets. */
Partitioner::BasicBlockStarts
Partitioner::detectBasicBlocks(const Disassembler::InstructionMap &insns) const
{
    BasicBlockStarts bb_starts;

    /* The first instruction always starts a basic block. */
    if (insns.size()>0) {
        rose_addr_t insn_va = insns.begin()->first;
        bb_starts[insn_va] = BasicBlockStarts::mapped_type();
    }

    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SgAsmInstruction *insn = ii->second;
        rose_addr_t insn_va = insn->get_address();
        rose_addr_t next_va = insn->get_address() + insn->get_raw_bytes().size();

        /* If this instruction is one which terminates a basic block then make the next instruction (if any) the beginning of
         * a basic block. However, a sequence like the following should not be a basic block boundary because the CALL is
         * acting more like a "PUSH EIP" (we should probably just look at the CALL instruction itself rather than also looking
         * for the following POP, but since ROSE doesn't currently apply the relocation tables before disassembling, the CALL
         * with a zero offset is quite common. [RPM 2009-08-24] */
        if (insn->terminatesBasicBlock()) {
            Disassembler::InstructionMap::const_iterator found = insns.find(next_va);
            if (found!=insns.end()) {
                SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(insn);
                SgAsmx86Instruction *insn2_x86 = isSgAsmx86Instruction(found->second);
                rose_addr_t branch_target_va;
                if (insn_x86 &&
                    (insn_x86->get_kind()==x86_call || insn_x86->get_kind()==x86_farcall) &&
                    x86GetKnownBranchTarget(insn_x86, branch_target_va) &&
                    branch_target_va==next_va && insn2_x86->get_kind()==x86_pop) {
                    /* The CALL is acting more like a "PUSH EIP" and should not end the basic block. */
                } else if (bb_starts.find(next_va)==bb_starts.end()) {
                    bb_starts[next_va] = BasicBlockStarts::mapped_type();
                }   
            }
        }

        /* If this instruction has multiple known successors then make each of those successors the beginning of a basic
         * block (provided there's an instruction at that address). However, if there's only one successor and it's the
         * fall-through address then ignore it. */
        bool complete;
        Disassembler::AddressSet successors = insn->get_successors(&complete);
        for (Disassembler::AddressSet::const_iterator si=successors.begin(); si!=successors.end(); ++si) {
            rose_addr_t successor_va = *si;
            if ((successor_va != next_va || successors.size()>1) && insns.find(successor_va)!=insns.end())
                bb_starts[successor_va].insert(insn_va);
        }
    }
    return bb_starts;
}

/* Update basic blocks so that every function start also starts a basic block. */
void
Partitioner::update_basic_blocks(const FunctionStarts& func_starts, BasicBlockStarts& bb_starts/*out*/)
{
    for (FunctionStarts::const_iterator i=func_starts.begin(); i!=func_starts.end(); i++) {
        if (bb_starts.find(i->first)==bb_starts.end())
            bb_starts[i->first] = BasicBlockStarts::mapped_type();
    }
}

/* Checks consistency of functions, basic blocks, and instructions. */
void
Partitioner::check_consistency(const Disassembler::InstructionMap& insns, const BasicBlockStarts& bb_starts,
                               const FunctionStarts& func_starts, bool verbose)
{
    if (verbose) {
        fprintf(stderr, "===== Checking Partitioner data consistency ====\n");
        fprintf(stderr, "functions at");
    }
    for (FunctionStarts::const_iterator fi=func_starts.begin(); fi!=func_starts.end(); fi++) {
        if (verbose)
            fprintf(stderr, " 0x%08"PRIx64, fi->first);
        if (!verbose && bb_starts.find(fi->first)==bb_starts.end())
            check_consistency(insns, bb_starts, func_starts, true);
        ROSE_ASSERT(bb_starts.find(fi->first)!=bb_starts.end());
    }
    if (verbose)
        fprintf(stderr, "\nbasic blocks at");
    for (BasicBlockStarts::const_iterator bi=bb_starts.begin(); bi!=bb_starts.end(); bi++) {
        if (verbose)
            fprintf(stderr, " 0x%08"PRIx64, bi->first);
        if (!verbose && insns.find(bi->first)==insns.end())
            check_consistency(insns, bb_starts, func_starts, true);
        ROSE_ASSERT(insns.find(bi->first)!=insns.end());
    }
    if (verbose) {
        fprintf(stderr, "\n");
        fprintf(stderr, "============== Consistency checks out OK =======\n");
    }
}

/* Checks final consistency of AST.  Note that this is very slow! */
void
Partitioner::check_consistency(const Disassembler::InstructionMap& insns)
{
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SgAsmInstruction *insn = ii->second;

        /* Every instruction belongs to a basic block which points to that instruction. */
        SgAsmBlock *block = isSgAsmBlock(insn->get_parent());
        {
            ROSE_ASSERT(block!=NULL);
            bool found = false;
            const SgAsmStatementPtrList &iv = block->get_statementList();
            for (size_t i=0; !found && i<iv.size(); i++) {
                ROSE_ASSERT(isSgAsmInstruction(iv[i]));
                found = insn == iv[i];
            }
            ROSE_ASSERT(found);
        }

        /* Every basic block belongs to a function which points to that basic block. */
        SgAsmFunctionDeclaration *function = isSgAsmFunctionDeclaration(block->get_parent());
        {
            ROSE_ASSERT(function!=NULL);
            bool found = false;
            const SgAsmStatementPtrList &bv = function->get_statementList();
            for (size_t i=0; !found && i<bv.size(); i++) {
                ROSE_ASSERT(isSgAsmBlock(bv[i]));
                found = block == bv[i];
            }
            ROSE_ASSERT(found);
        }

        /* Every function belongs to the top block which points to that function. */
        SgAsmBlock *top = isSgAsmBlock(function->get_parent());
        {
            ROSE_ASSERT(top!=NULL);
            bool found = false;
            const SgAsmStatementPtrList &fv = top->get_statementList();
            for (size_t i=0; !found && i<fv.size(); i++) {
                ROSE_ASSERT(fv[i]!=NULL);
                ROSE_ASSERT(isSgAsmFunctionDeclaration(fv[i]));
                found = function == fv[i];
            }
            ROSE_ASSERT(found);
        }
    }
}

/* Find beginnings of functions. */
Partitioner::FunctionStarts
Partitioner::detectFunctions(SgAsmInterpretation *interp, const Disassembler::InstructionMap &insns,
                             BasicBlockStarts &bb_starts/*out*/) const
{
    FunctionStarts func_starts;

    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (size_t i=0; i<headers.size(); i++) {
        if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_ENTRY_POINT)
            mark_entry_targets(headers[i], insns, func_starts);
        if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_EH_FRAME)
            mark_eh_frames(headers[i], insns, func_starts);
        if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_SYMBOL)
            mark_func_symbols(headers[i], insns, func_starts);
        if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_PATTERN)
            mark_func_patterns(headers[i], insns, func_starts);
    }
    if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_CALL_TARGET)
        mark_call_targets(insns, func_starts);
    update_basic_blocks(func_starts, bb_starts);

#if 0
    /* FIXME: Temporarily commented out because I don't think it understands that functions can overlap with each other.
     * [RPM 2009-12-06] */
    if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_GRAPH) {
        mark_graph_edges(insns, bb_starts, func_starts);
        update_basic_blocks(func_starts, bb_starts);
    }
#endif

    /* This doesn't detect new functions, it just gives names to ELF .plt trampolines */
    for (size_t i=0; i<headers.size(); i++) {
        name_plt_entries(headers[i], insns, func_starts);
    }

    /* Run user-defined function detectors, making sure that the basic block starts are up-to-date for each call. */
    if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_USERDEF) {
        for (size_t i=0; i<p_func_detectors.size(); i++) {
            for (size_t j=0; j<=headers.size(); j++) {
                SgAsmGenericHeader *hdr = 0==j ? NULL : headers[j-1];
                p_func_detectors[i](hdr, insns, bb_starts, func_starts);
                update_basic_blocks(func_starts, bb_starts);
            }
        }
        check_consistency(insns, bb_starts, func_starts); /*user-supplied code is prone to errors.*/
    }

    /* Mark the heads of instruction sequences as functions regardless of whether p_func_heuristics has the FUNC_INSNHEAD bit
     * set. This is required because buildTree() requires that every instruction belongs to a function.  This should be called
     * before methods that operate at the basic block level, namely mark_func_padding() and mark_func_discont(). */
    mark_insn_heads(insns, bb_starts, func_starts);
    update_basic_blocks(func_starts, bb_starts);

    /* Run analyses that improve upon what we've already detected. */
    if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_INTERPAD) {
        mark_nop_padding(insns, bb_starts, func_starts);
        update_basic_blocks(func_starts, bb_starts);
        mark_zero_padding(insns, bb_starts, func_starts);
        update_basic_blocks(func_starts, bb_starts);
    }
    if (p_func_heuristics & SgAsmFunctionDeclaration::FUNC_DISCONT) {
        mark_func_discont(insns, bb_starts, func_starts);
        update_basic_blocks(func_starts, bb_starts);
    }

    return func_starts;
}

/* Organize instructions into basic blocks and function. */
SgAsmBlock*
Partitioner::buildTree(const Disassembler::InstructionMap& insns, const BasicBlockStarts& bb_starts,
                       const FunctionStarts& func_starts) const
{
    /* This can be cleared for debugging, in which case the discontiguous function chunks marked with FUNC_DISCONT are not
     * folded into their main function.  When set, they are folded in and the FUNC_DISCONT bit is set for the main function to
     * indicate that it contains discontiguous chunks. */
    static const bool join_discont_chunks = true;

    check_consistency(insns, bb_starts, func_starts, false);
    SgAsmBlock *retval = new SgAsmBlock();
    std::map<rose_addr_t, SgAsmFunctionDeclaration*> funcs;

    /* Every function start must also be a basic block start. This buildTree method will work fine even if we encounter a
     * violation of this rule, but a violation probably indicates that there's bad logic elsewhere. */
    for (FunctionStarts::const_iterator fsi=func_starts.begin(); fsi!=func_starts.end(); ++fsi) {
        BasicBlockStarts::const_iterator bbsi = bb_starts.find(fsi->first);
        ROSE_ASSERT(bbsi!=bb_starts.end());
    }

    /* The worklist holds instruction addresses that need to be processed. */
    Disassembler::AddressSet workset, saved;
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ii++)
        workset.insert(ii->first);
        
    /* First pass is for normal functions, second pass is for detached chunks. */
    for (size_t pass=0; pass<2; pass++) {
        
        /* Process instructions through the outer loop in order of address, but the inner loop will remove reachable instructions
         * in arbitrary order if instructions overlap. */
        while (workset.size()>0) {
            rose_addr_t addr = *(workset.begin());
            Disassembler::InstructionMap::const_iterator ii = insns.find(addr);
            FunctionStarts::const_iterator fi = func_starts.find(addr); ROSE_ASSERT(fi!=func_starts.end());
            BasicBlockStarts::const_iterator bi = bb_starts.find(addr); ROSE_ASSERT(bi!=bb_starts.end());
            bool is_detached = (fi->second.reason & SgAsmFunctionDeclaration::FUNC_DISCONT)!=0;

            /* Get or create the SgAsmFunctionDeclaration; or save work for second pass. */
            SgAsmFunctionDeclaration* func = NULL;
            if (0==pass && is_detached && join_discont_chunks) {
                /* Detached chunks are saved for the second pass */
                do {
                    saved.insert(addr);
                    workset.erase(addr);
                    addr += ii->second->get_raw_bytes().size();
                    ii = insns.find(addr);
                } while (ii!=insns.end() && func_starts.find(ii->first)==func_starts.end());
                continue;
            } else if (1==pass && is_detached) {
                /* Use an existing function IR node */
                ROSE_ASSERT(funcs.find(fi->second.part_of)!=funcs.end());
                func = funcs[fi->second.part_of];
                func->set_reason(func->get_reason() | SgAsmFunctionDeclaration::FUNC_DISCONT);
            } else {
                /* Build a new function IR node */
                func = new SgAsmFunctionDeclaration;
                func->set_address(addr); /*lowest instruction address of function, but not necessarily the entry point*/
                retval->get_statementList().push_back(func);
                func->set_parent(retval);
                func->set_name(fi->second.name);
                func->set_reason(fi->second.reason);
                func->set_entry_va(addr);
                funcs[addr] = func;
            }

            /* Build the basic blocks */
            SgAsmBlock *block = NULL;
            while (1) {
                if (block==NULL) {
                    block = new SgAsmBlock;
                    block->set_id(addr);                    /* I'm not sure what this is for. [RPM 2009-12-07] */
                    block->set_address(addr);
                    func->get_statementList().push_back(block);
                    block->set_parent(func);
                }
                block->get_statementList().push_back(ii->second);
                ii->second->set_parent(block);
                workset.erase(addr);

                addr += ii->second->get_raw_bytes().size();
                ii = insns.find(addr);
                if (ii==insns.end())
                    break; /*reached the end of an instruction sequence, block, and function*/
                if (func_starts.find(addr)!=func_starts.end())
                    break; /*reached a new function; choose next lowest insn so functions are defined in address order*/
                if (bb_starts.find(addr)!=bb_starts.end())
                    block = NULL; /*reached a new basic block*/
            }
        }

        workset = saved;
        saved.clear();
    }

    check_consistency(insns); /*DEBUGGING [RPM 2009-12-09]*/
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
        rose_addr_t entry_va = entries[i].get_rva() + fhdr->get_base_va();
        if (insns.find(entry_va)!=insns.end())
            func_starts[entry_va].reason |= SgAsmFunctionDeclaration::FUNC_ENTRY_POINT;
    }
}

/* Marks CALL targets as functions. */
void
Partitioner::mark_call_targets(const Disassembler::InstructionMap &insns, FunctionStarts &func_starts/*out*/) const
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
            rose_addr_t callee_va = 0;
            if (x86GetKnownBranchTarget(insn, callee_va/*out*/) &&
                insns.find(callee_va)!=insns.end()) {
                bool needs_reloc = callee_va == insn->get_address() + insn->get_raw_bytes().size();
                if (!needs_reloc)
                    func_starts[callee_va].reason |= SgAsmFunctionDeclaration::FUNC_CALL_TARGET;
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
            if (symbol->get_def_state()==SgAsmGenericSymbol::SYM_DEFINED && symbol->get_type()==SgAsmGenericSymbol::SYM_FUNC) {
                rose_addr_t value = symbol->get_value();

                if (insns.find(value)!=insns.end()) {
                    func_starts[value].reason |= SgAsmFunctionDeclaration::FUNC_SYMBOL;
                    if (func_starts[value].name=="")
                        func_starts[value].name = symbol->get_name()->get_string();
                }

                /* Sometimes weak symbol values are offsets from a section (this code handles that), but other times they're
                 * the value is used directly (the above code handled that case). */            
                SgAsmGenericSection *section = symbol->get_bound();
                if (section && symbol->get_binding()==SgAsmGenericSymbol::SYM_WEAK)
                    value += section->get_header()->get_base_va() + section->get_mapped_actual_rva();
                if (insns.find(value)!=insns.end()) {
                    func_starts[value].reason |= SgAsmFunctionDeclaration::FUNC_SYMBOL;
                    if (func_starts[value].name=="")
                        func_starts[value].name = symbol->get_name()->get_string();
                }
            }
        }
    }
}

/* Use control flow graph to find function starts. */
void
Partitioner::mark_graph_edges(const Disassembler::InstructionMap &insns,
                              const BasicBlockStarts &basicBlockStarts, FunctionStarts &functionStarts/*out*/) const
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
        BasicBlockStarts::const_iterator bbi = basicBlockStarts.find(func_begin);
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
            for (std::set<rose_addr_t>::const_iterator ci=bbi->second.begin(); !called && ci!=bbi->second.end(); ci++)
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

/** See Partitioner::mark_func_patterns. Tries to match "push rbp; mov rbp,rsp" (or the 32-bit equivalent) */
static Disassembler::InstructionMap::const_iterator
pattern1(const Disassembler::InstructionMap& insns, Disassembler::InstructionMap::const_iterator ii)
{
    /* Look for "push rbp" */
    SgAsmx86Instruction *insn1 = isSgAsmx86Instruction(ii->second);
    if (!insn1) return insns.end();
    if (insn1->get_kind()!=x86_push) return insns.end();
    const SgAsmExpressionPtrList &opands1 = insn1->get_operandList()->get_operands();
    if (opands1.size()!=1) return insns.end();
    SgAsmx86RegisterReferenceExpression *rre = isSgAsmx86RegisterReferenceExpression(opands1[0]);
    if (!rre) return insns.end();
    if (rre->get_register_class()!=x86_regclass_gpr || rre->get_register_number()!=x86_gpr_bp) return insns.end();

    /* Look for "mov rbp,rsp" */
    Disassembler::InstructionMap::const_iterator ij=insns.find(ii->first + insn1->get_raw_bytes().size());
    if (ij==insns.end()) return insns.end();
    SgAsmx86Instruction *insn2 = isSgAsmx86Instruction(ij->second);
    if (!insn2) return insns.end();
    if (insn2->get_kind()!=x86_mov) return insns.end();
    const SgAsmExpressionPtrList &opands2 = insn2->get_operandList()->get_operands();
    if (opands2.size()!=2) return insns.end();
    rre = isSgAsmx86RegisterReferenceExpression(opands2[0]);
    if (!rre) return insns.end();
    if (rre->get_register_class()!=x86_regclass_gpr || rre->get_register_number()!=x86_gpr_bp) return insns.end();
    rre = isSgAsmx86RegisterReferenceExpression(opands2[1]);
    if (!rre) return insns.end();
    if (rre->get_register_class()!=x86_regclass_gpr || rre->get_register_number()!=x86_gpr_sp) return insns.end();

    return ii;
}

/** See Partitioner::mark_func_patterns. Tries to match "nop;nop;nop" followed by something that's not a nop and returns the
 *  something that's not a nop if successful. */
static Disassembler::InstructionMap::const_iterator
pattern2(const Disassembler::InstructionMap& insns, Disassembler::InstructionMap::const_iterator ii)
{
    /* Look for three "nop" instructions */
    for (size_t i=0; i<3; i++) {
        SgAsmx86Instruction *nop = isSgAsmx86Instruction(ii->second);
        if (!nop) return insns.end();
        if (nop->get_kind()!=x86_nop) return insns.end();
        if (nop->get_operandList()->get_operands().size()!=0) return insns.end(); /*only zero-arg NOPs allowed*/
        ii = insns.find(ii->first + nop->get_raw_bytes().size());
        if (ii==insns.end()) return insns.end();
    }
    
    /* Look for something that's not a "nop"; this is the function entry point. */
    SgAsmx86Instruction *notnop = isSgAsmx86Instruction(ii->second);
    if (!notnop) return insns.end();
    if (notnop->get_kind()==x86_nop) return insns.end();
    return ii;
}

/** See Partitioner::mark_func_patterns. Tries to match "leave;ret" followed by one or more "nop" followed by a non-nop
 *  instruction and if matching, returns the iterator for the non-nop instruction. */
static Disassembler::InstructionMap::const_iterator
pattern3(const Disassembler::InstructionMap& insns, Disassembler::InstructionMap::const_iterator ii)
{
    /* leave; ret; nop */
    for (size_t i=0; i<3; i++) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
        if (!insn) return insns.end();
        if ((i==0 && insn->get_kind()!=x86_leave) ||
            (i==1 && insn->get_kind()!=x86_ret)   ||
            (i==2 && insn->get_kind()!=x86_nop))
            return insns.end();
        ii = insns.find(ii->first + insn->get_raw_bytes().size());
        if (ii==insns.end()) return insns.end();
    }
    
    /* Zero or more "nop" instructions */
    while (1) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
        if (!insn) return insns.end();
        if (insn->get_kind()!=x86_nop) break;
        ii = insns.find(ii->first + insn->get_raw_bytes().size());
        if (ii==insns.end()) return insns.end();
    }
    
    /* This must be something that's not a "nop", but make sure it's an x86 instruction anyway. */
    SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
    if (!insn) return insns.end();
    return ii;
}
        
/* Look for instruction patterns. */
void
Partitioner::mark_func_patterns(SgAsmGenericHeader*, const Disassembler::InstructionMap &insns,
                                FunctionStarts &func_starts/*out*/) const
{
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        Disassembler::InstructionMap::const_iterator found = insns.end();

        /* Try each pattern until one succeeds */
        if (found==insns.end()) found = pattern1(insns, ii);
        if (found==insns.end()) found = pattern2(insns, ii);
        if (found==insns.end()) found = pattern3(insns, ii);

        
        /* We found a function entry point */
        if (found!=insns.end())
            func_starts[found->first].reason |= SgAsmFunctionDeclaration::FUNC_PATTERN;
    }
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

        if (func_addr <  elf->get_base_va() + plt->get_mapped_preferred_rva() &&
            func_addr >= elf->get_base_va() + plt->get_mapped_preferred_rva() + plt->get_mapped_size())
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

        if (gotplt_va <  elf->get_base_va() + gotplt->get_mapped_preferred_rva() ||
            gotplt_va >= elf->get_base_va() + gotplt->get_mapped_preferred_rva() + gotplt->get_mapped_size())
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

/* Returns information about the end of a function. */
std::pair<rose_addr_t, rose_addr_t>
Partitioner::end_of_function(rose_addr_t addr, const Disassembler::InstructionMap& insns, const BasicBlockStarts& bb_starts, 
                             const FunctionStarts& func_starts)
{
    Disassembler::InstructionMap::const_iterator ii = insns.find(addr);
    ROSE_ASSERT(ii!=insns.end());
    ROSE_ASSERT(func_starts.find(addr)!=func_starts.end());
    ROSE_ASSERT(bb_starts.find(addr)!=bb_starts.end());
    rose_addr_t last_block_addr = addr;

    while (1) {
        rose_addr_t next_addr = ii->first + ii->second->get_raw_bytes().size();
        if (func_starts.find(next_addr)!=func_starts.end())
            break; /*we hit the next function*/
        ii = insns.find(next_addr);
        if (ii==insns.end())
            break; /*we've advanced past the last instruction*/
        if (bb_starts.find(next_addr)!=bb_starts.end())
            last_block_addr = next_addr;
        addr = next_addr;
    }
    return std::pair<rose_addr_t, rose_addr_t>(addr, last_block_addr);
}

/* Returns information about the end of a block. */
rose_addr_t
Partitioner::end_of_block(rose_addr_t addr, const Disassembler::InstructionMap& insns, const BasicBlockStarts& bb_starts)
{
    Disassembler::InstructionMap::const_iterator ii = insns.find(addr);
    ROSE_ASSERT(ii!=insns.end());
    ROSE_ASSERT(bb_starts.find(addr)!=bb_starts.end());
    while (1) {
        rose_addr_t next_addr = ii->first + ii->second->get_raw_bytes().size();
        if (bb_starts.find(next_addr)!=bb_starts.end())
            break; /*we hit the next basic block*/
        ii = insns.find(next_addr);
        if (ii == insns.end())
            break;
        addr = next_addr;
    }
    return addr;
}

/* Splits trailing zero padding into their own functional units. */
void
Partitioner::mark_zero_padding(const Disassembler::InstructionMap& insns, const BasicBlockStarts& bb_starts,
                               FunctionStarts &func_starts/*out*/) const
{
    Disassembler::AddressSet new_functions;
    for (FunctionStarts::iterator fi=func_starts.begin(); fi!=func_starts.end(); fi++) {
        if (fi->second.reason & SgAsmFunctionDeclaration::FUNC_INTERPAD) continue; /*some other kind of padding*/
        rose_addr_t last_block_addr = end_of_function(fi->first, insns, bb_starts, func_starts).second;
        if (last_block_addr==fi->first) continue; /*function only has one block*/
        FunctionStarts::iterator next_func = func_starts.upper_bound(last_block_addr); /*irrespective of overlapping*/

        /* Get extent of addresses to check for zero */
        rose_addr_t stop_before; /*check zeros up to, but not including, this address*/
        if (next_func!=func_starts.end()) {
            stop_before = next_func->first;
        } else {
            rose_addr_t last_insn_addr = end_of_block(last_block_addr, insns, bb_starts);
            Disassembler::InstructionMap::const_iterator ii = insns.find(last_insn_addr);
            ROSE_ASSERT(ii!=insns.end());
            stop_before = ii->first + ii->second->get_raw_bytes().size();
        }

        /* Check that all instructions of the last block contain zero up to the stop_before address. The last instruction(s)
         * might actually extend past the stop_before address and might contain non-zero bytes that we aren't interested in. */
        bool all_zero=true;
        for (rose_addr_t addr=last_block_addr; addr<stop_before && all_zero; /*void*/) {
            Disassembler::InstructionMap::const_iterator ii=insns.find(addr);
            if (ii==insns.end()) break; /*we've reached the last instruction of the block*/
            size_t nbytes = std::min(ii->second->get_raw_bytes().size(), stop_before-addr);
            for (size_t i=0; i<nbytes && all_zero; i++)
                all_zero = (ii->second->get_raw_bytes()[i] == 0);
            addr += ii->second->get_raw_bytes().size();
        }
        
        /* Create a new function start. */
        if (all_zero) {
            ROSE_ASSERT(last_block_addr!=fi->first);
            ROSE_ASSERT(func_starts.find(last_block_addr)==func_starts.end());
            new_functions.insert(last_block_addr);
        }
    }
    
    /* Create new functions. */
    for (Disassembler::AddressSet::iterator fi=new_functions.begin(); fi!=new_functions.end(); fi++)
        func_starts[*fi].reason |= SgAsmFunctionDeclaration::FUNC_INTERPAD;
}

/* Splits trailing NOP padding into their own functional units. */
void
Partitioner::mark_nop_padding(const Disassembler::InstructionMap& insns, const BasicBlockStarts& bb_starts,
                              FunctionStarts &func_starts/*out*/) const
{
    Disassembler::AddressSet new_functions;
    for (FunctionStarts::iterator fi=func_starts.begin(); fi!=func_starts.end(); fi++) {
        if (fi->second.reason & SgAsmFunctionDeclaration::FUNC_INTERPAD) continue; /*some other kind of padding*/
        std::pair<rose_addr_t, rose_addr_t> end = end_of_function(fi->first, insns, bb_starts, func_starts);
        rose_addr_t last_insn_addr = end.first;
        rose_addr_t last_bb_addr = end.second;
        if (last_bb_addr==fi->first)
            continue; /*function has only one basic block*/
        
        /* Are the instructions from last_bb_addr through last_insn_addr all no-ops? */
        bool all_nops = true;
        for (rose_addr_t addr=last_bb_addr; all_nops && addr<=last_insn_addr; /*void*/) {
            Disassembler::InstructionMap::const_iterator ii = insns.find(addr);
            ROSE_ASSERT(ii!=insns.end());

            /*FIXME: We should have a more portable way to find instructions with no effect [RPM 2009-12-05]*/
            SgAsmx86Instruction     *insn_x86 = isSgAsmx86Instruction(ii->second);
            SgAsmArmInstruction     *insn_arm = isSgAsmArmInstruction(ii->second);
            SgAsmPowerpcInstruction *insn_ppc = isSgAsmPowerpcInstruction(ii->second);
            ROSE_ASSERT(insn_x86 || insn_arm || insn_ppc);
            all_nops = insn_x86 && insn_x86->get_kind()==x86_nop; /*only x86 has NOPs*/
            addr += ii->second->get_raw_bytes().size();
        }
        
        /* If the last basic block of function is all no-ops and is not the only basic block of the function then create a new
         * functional unit containing just the last basic block. */
        if (all_nops) {
            ROSE_ASSERT(last_bb_addr!=fi->first);
            ROSE_ASSERT(func_starts.find(last_bb_addr)==func_starts.end());
            new_functions.insert(last_bb_addr);
        }
    }
    
    /* Create the new functions. */
    for (Disassembler::AddressSet::iterator fi=new_functions.begin(); fi!=new_functions.end(); fi++)
        func_starts[*fi].reason |= SgAsmFunctionDeclaration::FUNC_INTERPAD;
}

/* Marks the start of each instruction stream as a function if not done so already. */
void
Partitioner::mark_insn_heads(const Disassembler::InstructionMap& insns, const BasicBlockStarts& bb_starts,
                             FunctionStarts &func_starts/*out*/) const
{
    Disassembler::AddressSet heads;
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ii++) {
        rose_addr_t addr = ii->first;
        if (heads.find(addr)==heads.end() && func_starts.find(addr)==func_starts.end())
            func_starts[addr].reason |= SgAsmFunctionDeclaration::FUNC_INSNHEAD;
        addr += ii->second->get_raw_bytes().size();
        if (insns.find(addr)!=insns.end())
            heads.insert(addr);

        /* Clean up abandoned instruction sequences to keep the set small and fast. */
        static const size_t maxinsn = 20; /*a value larger than the largest possible instruction*/
        if (addr>maxinsn && heads.size()>maxinsn) {
            Disassembler::AddressSet::iterator lb = heads.lower_bound(addr-maxinsn);
            if (lb!=heads.end())
                heads.erase(heads.begin(), lb);
        }
        
    }
}

/* Return the set of nodes reachable from a given node. */
void
Partitioner::discont_subgraph(DiscontGraph& graph, DiscontGraph::iterator gi, Disassembler::AddressSet *result)
{
    ROSE_ASSERT(gi!=graph.end());
    if (gi->second.first!=0) return; /*already visited this node*/
    gi->second.first = 1;
    result->insert(gi->first);
    const Disassembler::AddressSet& edges = gi->second.second;
    for (Disassembler::AddressSet::const_iterator ei=edges.begin(); ei!=edges.end(); ei++)
        discont_subgraph(graph, graph.find(*ei), result);
}

/* Find blocks for noncontiguous functions */
void
Partitioner::mark_func_discont(const Disassembler::InstructionMap& insns, const BasicBlockStarts& bb_starts,
                               FunctionStarts& func_starts/*out*/) const
{

   /* Map from insn address to basic block address for all instructions */
    typedef std::map<rose_addr_t, rose_addr_t> Insn2Block;
    Insn2Block insn2block;

    /* Build the nodes of the graph and initialize the instruction-to-block mapping. */
    DiscontGraph graph;
    for (BasicBlockStarts::const_iterator bbi=bb_starts.begin(); bbi!=bb_starts.end(); bbi++) {
        rose_addr_t addr = bbi->first;
        rose_addr_t hi_addr = end_of_block(addr, insns, bb_starts);
        while (addr<=hi_addr) {
            Disassembler::InstructionMap::const_iterator ii = insns.find(addr);
            ROSE_ASSERT(ii!=insns.end());
            insn2block[addr] = bbi->first;
            addr += ii->second->get_raw_bytes().size();
        }
        if (func_starts.find(bbi->first)==func_starts.end())
            graph[bbi->first].first = 0; /*instantiate node and clear its mark*/
    }

    /* Build the edges of the graph. */
    for (DiscontGraph::iterator gi=graph.begin(); gi!=graph.end(); gi++) {      /*gi is iterator for callee*/
        rose_addr_t callee_block_addr = gi->first;
        BasicBlockStarts::const_iterator bbi = bb_starts.find(callee_block_addr);
        const Disassembler::AddressSet& callers = bbi->second;
        for (Disassembler::AddressSet::const_iterator ci=callers.begin(); ci!=callers.end(); ci++) {
            rose_addr_t caller_insn_addr = *ci;
            rose_addr_t caller_block_addr = insn2block[caller_insn_addr];
            DiscontGraph::iterator gi2 = graph.find(caller_block_addr);         /*gi2 is iterator for caller*/
            if (gi2!=graph.end()) {
                gi2->second.second.insert(callee_block_addr);                   /*forward edge*/
                gi->second.second.insert(caller_block_addr);                    /*backward edge*/
            }
        }
    }

#if 0
    /* Print the graph */
    fprintf(stderr, "DiscontGraph contains %zu nodes:\n", graph.size());
    for (DiscontGraph::iterator gi=graph.begin(); gi!=graph.end(); gi++) {
        fprintf(stderr, "  0x%08"PRIx64"(%d):", gi->first, gi->second.first);
        for (Disassembler::AddressSet::iterator ci=gi->second.second.begin(); ci!=gi->second.second.end(); ci++)
            fprintf(stderr, " 0x%08"PRIx64, *ci);
        fprintf(stderr, "\n");
    }
#endif

    /* Identify subgraphs */
    for (DiscontGraph::iterator gi=graph.begin(); gi!=graph.end(); gi++) {
        if (gi->second.first!=0) continue; /*already visited this node; set by discont_subgraph()*/
        Disassembler::AddressSet nodes;
        discont_subgraph(graph, gi, &nodes);

        /* Find the callers of this subgraph which are not in this subgraph. Abort the loop if we find more than one. */
        Disassembler::AddressSet external_callers;
        for (Disassembler::AddressSet::iterator ni=nodes.begin(); ni!=nodes.end() && external_callers.size()<=1; ni++) {
            BasicBlockStarts::const_iterator bbi = bb_starts.find(*ni);
            ROSE_ASSERT(bbi!=bb_starts.end());
            for (Disassembler::AddressSet::const_iterator ci=bbi->second.begin(); ci!=bbi->second.end(); ci++) {
                rose_addr_t caller_insn_addr = *ci;
                rose_addr_t caller_block_addr = insn2block[caller_insn_addr];
                if (graph.find(caller_block_addr)==graph.end()) {
                    external_callers.insert(caller_block_addr);
                    if (external_callers.size()>1) break;
                }
            }
        }
        
        /* If there's just one external caller then the blocks of this subgraph can be considered part of that external
         * caller's function. */
        if (1==external_callers.size()) {
            for (Disassembler::AddressSet::iterator ni=nodes.begin(); ni!=nodes.end();  ni++) {
                func_starts[*ni].reason |= SgAsmFunctionDeclaration::FUNC_DISCONT;
                func_starts[*ni].part_of = *(external_callers.begin());
            }
        }
    }
    

    /* Clean up phase. We dont actually need func_start entries for discont functions whose "part_of" points to
     * the previous function. It doesn't hurt to keep them, but it makes it easier to debug other parts of the partitioner if
     * we prune away the unecessary ones. We must take into account function overlapping. */
    Disassembler::AddressSet can_delete;
    for (FunctionStarts::iterator fi=func_starts.begin(); fi!=func_starts.end(); fi++) {
        rose_addr_t first_func_addr = fi->first;
        if (fi->second.reason & SgAsmFunctionDeclaration::FUNC_DISCONT) continue; /*DISCONT is handled by inner loop*/
        if (can_delete.find(first_func_addr)!=can_delete.end()) continue; /*virtually deleted*/

        rose_addr_t func_addr = first_func_addr;
        while (1) {
            /* Find next function */
            rose_addr_t func_last_addr = end_of_function(func_addr, insns, bb_starts, func_starts).first;
            Disassembler::InstructionMap::const_iterator ii = insns.find(func_last_addr);
            ROSE_ASSERT(ii!=insns.end());
            rose_addr_t next_func_addr = func_last_addr + ii->second->get_raw_bytes().size();
            FunctionStarts::iterator fi2 = func_starts.find(next_func_addr);
            if (fi2==func_starts.end()) break;

            /* Does next function point to the same thing as the outer loop? If so, we can eventually delete it, otherwise
             * we need to stop searching since we can only combine adjacent equivalent functions. */
            if ((fi2->second.reason & SgAsmFunctionDeclaration::FUNC_DISCONT) && fi2->second.part_of==first_func_addr) {
                can_delete.insert(fi2->first);
            } else {
                break;
            }
            func_addr = next_func_addr;
        }
    }
    for (Disassembler::AddressSet::iterator di=can_delete.begin(); di!=can_delete.end(); di++)
        func_starts.erase(*di);
}

