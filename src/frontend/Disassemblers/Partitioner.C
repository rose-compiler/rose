/* Algorithms to detect what instructions make up basic blocks and which blocks make up functions, and how to create the
 * necessary SgAsmBlock and SgAsmFunctionDeclaration IR nodes from this information. */
#define __STDC_FORMAT_MACROS
#include "rose.h"
#include <inttypes.h>

/* See header file for full documentation. */





/********************************************************************************************************************************
 * These SgAsmFunctionDeclaration methods have no other home, so they're here for now. Do not move them into
 * src/ROSETTA/Grammar/BinaryInstruction.code because then they can't be indexed by C-aware tools.
 *
 */
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
/*
 *
 *******************************************************************************************************************************/









/* Return known successors of basic block and cache them. */
const Disassembler::AddressSet&
Partitioner::BasicBlock::successors(bool *complete)
{
    if (insns.size()!=sucs_ninsns) {
        /* Compute successors from scratch. */
        sucs.clear();
        bool b;
        if (!complete) complete=&b;
        sucs = insns.front()->get_successors(insns, complete);
        sucs_ninsns = insns.size();
    }
    return sucs;
}

/* Returns true if block ends with what appears to be a function call. Don't modify @p target if this isn't a function call. */
bool
Partitioner::BasicBlock::is_function_call(rose_addr_t *target)
{
    if (insns.size()==0)
        return false;
    rose_addr_t va;
    if (insns.front()->is_function_call(insns, &va)) {
        *target = va;
        return true;
    }
    return false;
}

/* Release all blocks from a function. */
void
Partitioner::Function::clear_blocks()
{
    for (BasicBlocks::iterator bi=blocks.begin(); bi!=blocks.end(); ++bi)
        bi->second->function = NULL;
    blocks.clear();
}

/* Return address of first instruction of basic block */
rose_addr_t
Partitioner::address(BasicBlock* bb) const
{
    ROSE_ASSERT(bb->insns.size()>0);
    return bb->insns.front()->get_address();
}

/* Split a basic block into two so that the instruction before @p va remain in the original block and a new block is created
 * to hold the instructions at @p va and after.  The new block is returned. Both blocks remain in the same function, if any. */
Partitioner::BasicBlock*
Partitioner::split(BasicBlock* bb1, rose_addr_t va)
{
    ROSE_ASSERT(bb1==find_bb_containing(va));
    BasicBlock *bb2 = new BasicBlock;

    /* Move some instructions from bb1 to bb2 */
    std::vector<SgAsmInstruction*>::iterator cut = bb1->insns.begin();
    while (cut!=bb1->insns.end() && (*cut)->get_address()<va) ++cut;
    for (std::vector<SgAsmInstruction*>::iterator ii=cut; ii!=bb1->insns.end(); ++ii) {
        bb2->insns.push_back(*ii);
        insn2block[(*ii)->get_address()] = bb2;
    }
    bb1->insns.erase(cut, bb1->insns.end());

    /* Insert bb2 into the same function as bb1 */
    if (bb1->function)
        append(bb1->function, bb2);

    return bb2;
}

/* Append instruction to basic block */
void
Partitioner::append(BasicBlock* bb, SgAsmInstruction* insn)
{
    ROSE_ASSERT(bb);
    ROSE_ASSERT(insn);
    if (bb->insns.size()>0) {
        rose_addr_t next_va = bb->insns.back()->get_address() + bb->insns.back()->get_raw_bytes().size();
        ROSE_ASSERT(insn->get_address()==next_va);
    }
    ROSE_ASSERT(insn2block.find(insn->get_address())==insn2block.end()); /*insn must not already belong to a basic block*/

    bb->insns.push_back(insn);
    insn2block[insn->get_address()] = bb;
}

/* Append basic block to function */
void
Partitioner::append(Function* f, BasicBlock *bb)
{
    ROSE_ASSERT(f);
    ROSE_ASSERT(bb);
    ROSE_ASSERT(bb->function==NULL);
    bb->function = f;
    f->blocks[address(bb)] = bb;
}

/* Find (or create) a basic block containing the specified instruction address. The address is assumed to be an instruction
 * boundary, not an arbitrary address. */
Partitioner::BasicBlock *
Partitioner::find_bb_containing(rose_addr_t va)
{
    std::map<rose_addr_t, BasicBlock*>::iterator i2b_i = insn2block.find(va);
    if (i2b_i!=insn2block.end() && i2b_i->second!=NULL) return i2b_i->second;

    BasicBlock *bb = NULL;
    while (1) {
        Disassembler::InstructionMap::const_iterator ii = insns.find(va);
        if (ii==insns.end())
            break; /*no instruction*/
        i2b_i = insn2block.find(va);
        if (i2b_i!=insn2block.end() && i2b_i->second!=NULL)
            break; /*we've reached another block*/
        SgAsmInstruction *insn = ii->second;
        rose_addr_t next_va = va + insn->get_raw_bytes().size();
        if (!bb)
            bb = new BasicBlock;
        append(bb, insn);
        if (insn->terminatesBasicBlock()) {
            bool complete;
            const Disassembler::AddressSet& successors = bb->successors(&complete);
            if (!complete || successors.size()!=1 || *(successors.begin())!=next_va)
                break; /*we proved that this block (so far) always falls through to the next instruction*/
        }
        va = next_va;
    }
    ROSE_ASSERT(bb);
    ROSE_ASSERT(bb->insns.size()>0);
    return bb;
}

Partitioner::Function *
Partitioner::add_function(rose_addr_t entry_va, unsigned reasons, std::string name)
{
    Function *f = NULL;
    Functions::iterator fi = functions.find(entry_va);
    if (fi==functions.end()) {
        f = new Function(entry_va, reasons, name);
        functions[entry_va] = f;
    } else {
        f = fi->second;
        ROSE_ASSERT(f->entry_va==entry_va);
        f->reason |= reasons;
        if (name!="") f->name = name;
    }
    return f;
}

/* Marks program entry addresses as functions. */
void
Partitioner::mark_entry_targets(SgAsmGenericHeader *fhdr)
{
    SgRVAList entries = fhdr->get_entry_rvas();
    for (size_t i=0; i<entries.size(); i++) {
        rose_addr_t entry_va = entries[i].get_rva() + fhdr->get_base_va();
        if (insns.find(entry_va)!=insns.end())
            add_function(entry_va, SgAsmFunctionDeclaration::FUNC_ENTRY_POINT);
    }
}

/** Use the Frame Descriptor Entry Records of the ELF .eh_frame section to mark functions. */
void
Partitioner::mark_eh_frames(SgAsmGenericHeader *fhdr)
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
                        add_function(target, SgAsmFunctionDeclaration::FUNC_EH_FRAME);
                }
            }
        }
    }
}

/** Use symbol tables to determine function entry points. */
void
Partitioner::mark_func_symbols(SgAsmGenericHeader *fhdr)
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
                if (insns.find(value)!=insns.end())
                    add_function(value, SgAsmFunctionDeclaration::FUNC_SYMBOL, symbol->get_name()->get_string());

                /* Sometimes weak symbol values are offsets from a section (this code handles that), but other times they're
                 * the value is used directly (the above code handled that case). */            
                SgAsmGenericSection *section = symbol->get_bound();
                if (section && symbol->get_binding()==SgAsmGenericSymbol::SYM_WEAK)
                    value += section->get_header()->get_base_va() + section->get_mapped_actual_rva();
                if (insns.find(value)!=insns.end())
                    add_function(value, SgAsmFunctionDeclaration::FUNC_SYMBOL, symbol->get_name()->get_string());
            }
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
Partitioner::mark_func_patterns(SgAsmGenericHeader*)
{
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        Disassembler::InstructionMap::const_iterator found = insns.end();

        /* Try each pattern until one succeeds */
        if (found==insns.end()) found = pattern1(insns, ii);
#if 0   /* Disabled because NOP's sometimes legitimately appear inside functions */
        if (found==insns.end()) found = pattern2(insns, ii);
#endif
        if (found==insns.end()) found = pattern3(insns, ii);

        
        /* We found a function entry point */
        if (found!=insns.end())
            add_function(found->first, SgAsmFunctionDeclaration::FUNC_PATTERN);
    }
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

/* Gives names to the dynamic linking trampolines in the .plt section. */
void
Partitioner::name_plt_entries(SgAsmGenericHeader *fhdr)
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
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); fi++) {
        rose_addr_t func_addr = fi->first;

        if (fi->second->name!="")
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
        for (std::set<SgAsmElfRelocSection*>::iterator ri=rsects.begin(); ri!=rsects.end() && fi->second->name==""; ri++) {
            SgAsmElfRelocEntryList *entries = (*ri)->get_entries();
            SgAsmElfSymbolSection *symbol_section = isSgAsmElfSymbolSection((*ri)->get_linked_section());
            SgAsmElfSymbolList *symbols = symbol_section->get_symbols();
            for (size_t ei=0; ei<entries->get_entries().size() && fi->second->name==""; ei++) {
                SgAsmElfRelocEntry *rel = entries->get_entries()[ei];
                if (rel->get_r_offset()==gotplt_va) {
                    unsigned long symbol_idx = rel->get_sym();
                    ROSE_ASSERT(symbol_idx < symbols->get_symbols().size());
                    SgAsmElfSymbol *symbol = symbols->get_symbols()[symbol_idx];
                    fi->second->name = symbol->get_name()->get_string();
                }
            }
        }
    }
}

/* Seed function starts based on criteria other than control flow graph. */
void
Partitioner::seed_functions(SgAsmInterpretation *interp)
{
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (size_t i=0; i<headers.size(); i++) {
        if (func_heuristics & SgAsmFunctionDeclaration::FUNC_ENTRY_POINT)
            mark_entry_targets(headers[i]);
        if (func_heuristics & SgAsmFunctionDeclaration::FUNC_EH_FRAME)
            mark_eh_frames(headers[i]);
        if (func_heuristics & SgAsmFunctionDeclaration::FUNC_SYMBOL)
            mark_func_symbols(headers[i]);
        if (func_heuristics & SgAsmFunctionDeclaration::FUNC_PATTERN)
            mark_func_patterns(headers[i]);
    }

    /* This doesn't detect new functions, it just gives names to ELF .plt trampolines */
    for (size_t i=0; i<headers.size(); i++) {
        name_plt_entries(headers[i]);
    }

    /* Run user-defined function detectors, making sure that the basic block starts are up-to-date for each call. */
    if (func_heuristics & SgAsmFunctionDeclaration::FUNC_USERDEF) {
        for (size_t i=0; i<user_detectors.size(); i++) {
            for (size_t j=0; j<=headers.size(); j++) {
                SgAsmGenericHeader *hdr = 0==j ? NULL : headers[j-1];
                user_detectors[i](this, hdr, insns, functions);
            }
        }
    }
}

/** Discover the basic blocks that belong to the current function. This function recursively adds basic blocks to function @p f
 *  by following the successors of each block.  Two types of successors are recognized: "call successors" are successors
 *  that invoke a function (on x86 this is usually a CALL instruction, see is_function_call()), and "flow control" successors
 *  that usually (but not always) branches within a single function.  If a successor is an instruction belonging to some other
 *  function then its either a function call (if it branches to the entry point of that function) or its a collision.
 *  Collisions are resolved by discarding and rediscovering the blocks of the other function. */
void
Partitioner::discover_blocks(Function *f, rose_addr_t va)
{
    fprintf(stderr, " B%08"PRIx64, va);
    Disassembler::InstructionMap::const_iterator ii = insns.find(va);
    if (ii==insns.end()) return; /* No instruction at this address. */
    SgAsmInstruction *insn = ii->second;
    static const rose_addr_t NO_TARGET = (rose_addr_t)-1;

    /* This block might be the entry address of a function even before that function has any basic blocks assigned to it. */
    Functions::iterator fi = functions.find(va);
    if (fi!=functions.end() && fi->second!=f) {
        fprintf(stderr, "[entry \"%s\"]", fi->second->name.c_str());
        return;
    }

    /* Split an existing basic block if necessary since basic blocks can have only one entry address. */
    bool did_split = false;
    BasicBlock *bb = find_bb_containing(insn);
    if (va!=address(bb)) {
        fprintf(stderr, "[split from B%08"PRIx64"]", address(bb));
        did_split = true;
        bb = split(bb, va);
    }
    
    if (bb->function==f) {
        /* This block already belongs to this function. However, if we split the block then its successors may have changed
         * and we need to follow the new successors. */    
        if (did_split) {
            const Disassembler::AddressSet& suc = bb->successors();
            for (Disassembler::AddressSet::const_iterator si=suc.begin(); si!=suc.end(); ++si)
                discover_blocks(f, *si);
        }
    } else if (bb->function && va==bb->function->entry_va) {
        /* This is a call to an existing function. Do not add it to the current function. */
        fprintf(stderr, "[entry \"%s\"]", bb->function->name.c_str());
    } else if (bb->function) {
        /* This block belongs to some other function. Since ROSE requires that blocks be owned by exactly one function (the
         * function/block relationship is an edge in the abstract syntax tree), we have to remove this block from the other
         * function (and not add it to this one either).  We'll mark the function as being in conflict so we can try again
         * later. */
        fprintf(stderr, "[conflicts F%08"PRIx64" \"%s\"]", bb->function->entry_va, bb->function->name.c_str());
        bb->function->pending = true;
        if (functions.find(va)==functions.end())
            add_function(va, SgAsmFunctionDeclaration::FUNC_GRAPH);
    } else {
        /* Add this block to the function and follow its successors. If a successor appears to be a function call then create
         * that function and don't add that block to this function. */
        rose_addr_t call_target = NO_TARGET;
        if (bb->is_function_call(&call_target) && call_target!=NO_TARGET) {
            fprintf(stderr, "[calling F%08"PRIx64"]", call_target);
            add_function(call_target, SgAsmFunctionDeclaration::FUNC_CALL_TARGET);
        }
        append(f, bb);
        const Disassembler::AddressSet& suc = bb->successors();
        for (Disassembler::AddressSet::const_iterator si=suc.begin(); si!=suc.end(); ++si) {
            if (*si!=call_target)
                discover_blocks(f, *si);
        }
    }
}

void
Partitioner::analyze_cfg()
{
    for (size_t pass=0; true; pass++) {
        fprintf(stderr, "========== Partitioner::analyze_cfg() pass %zu ==========\n", pass);

        /* Get a list of functions we need to analyze */
        std::vector<Function*> pending;
        for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            ROSE_ASSERT(fi->second->entry_va==fi->first);
            if (fi->second->pending) {
                fi->second->clear_blocks();
                fi->second->pending = false; /*might be set back to true by discover_blocks() in loop below*/
                pending.push_back(fi->second);
            }
        }
        if (pending.size()==0)
            break;
        
        /* (Re)discover each function's blocks starting with the function entry point */
        for (size_t i=0; i<pending.size(); ++i) {
            fprintf(stderr, "analyzing F%08"PRIx64" \"%s\" pass %zu: ", pending[i]->entry_va, pending[i]->name.c_str(), pass);
            discover_blocks(pending[i], pending[i]->entry_va);
            fprintf(stderr, "\n");
        }
    }
}

SgAsmBlock *
Partitioner::build_ast()
{
    /* Build a function to hold all the unassigned instructions. */
    Function *catchall = NULL;
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        BasicBlock *bb = find_bb_containing(ii->first);
        if (!bb->function) {
            if (!catchall)
                catchall = add_function(ii->first, 0, "***unassigned blocks***");
            append(catchall, bb);
        }
    }

    /* Build the AST */
    SgAsmBlock *retval = new SgAsmBlock;
    for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        SgAsmFunctionDeclaration *func_decl = build_ast(fi->second);
        if (!func_decl) continue;
        retval->get_statementList().push_back(func_decl);
        func_decl->set_parent(retval);
    }

    /* Return catchall blocks to the free pool */
    if (catchall) {
        catchall->clear_blocks();
        functions.erase(catchall->entry_va);
        delete catchall;
    }

    return retval;
}

SgAsmFunctionDeclaration *
Partitioner::build_ast(Function* f) const
{
    if (f->blocks.size()==0) {
        fprintf(stderr, "function F%08"PRIx64" \"%s\" has no basic blocks!\n", f->entry_va, f->name.c_str());
        return NULL;
    }
    
    SgAsmFunctionDeclaration *retval = new SgAsmFunctionDeclaration;
    rose_addr_t next_block_va = f->entry_va;
    unsigned reasons = f->reason;
    
    for (BasicBlocks::iterator bi=f->blocks.begin(); bi!=f->blocks.end(); ++bi) {
        if (address(bi->second)!=next_block_va)
            reasons |= SgAsmFunctionDeclaration::FUNC_DISCONT;
        SgAsmBlock *block = build_ast(bi->second);
        retval->get_statementList().push_back(block);
        block->set_parent(retval);
        SgAsmInstruction *last = bi->second->insns.back();
        next_block_va = last->get_address() + last->get_raw_bytes().size();
    }

    BasicBlock *first_block = f->blocks.begin()->second;
    retval->set_address(address(first_block));
    retval->set_name(f->name);
    retval->set_reason(reasons);
    retval->set_entry_va(f->entry_va);
    return retval;
}

SgAsmBlock *
Partitioner::build_ast(BasicBlock* bb) const
{
    SgAsmBlock *retval = new SgAsmBlock;
    retval->set_id(bb->insns.front()->get_address());
    retval->set_address(bb->insns.front()->get_address());
    for (std::vector<SgAsmInstruction*>::const_iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
        retval->get_statementList().push_back(*ii);
        (*ii)->set_parent(retval);
    }
    return retval;
}

/* Top-level function to run the partitioner on some instructions and build an AST */
SgAsmBlock *
Partitioner::partition(SgAsmInterpretation* interp, const Disassembler::InstructionMap& insns)
{
    clear();
    add_instructions(insns);
    seed_functions(interp);
    analyze_cfg();
    return build_ast();
}


/* FIXME: Deprecated 2010-01-01 */
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

/* FIXME: Deprecated 2010-01-01 */
Partitioner::FunctionStarts
Partitioner::detectFunctions(SgAsmInterpretation*, const Disassembler::InstructionMap &insns,
                             BasicBlockStarts &bb_starts/*out*/) const
{
    FunctionStarts retval;
    for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi)
        retval.insert(std::make_pair(fi->first, FunctionStart(fi->second->reason, fi->second->name)));
    return retval;
}

