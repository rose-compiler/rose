#include "rose.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* See header file for full documentation of all methods in this file. */

/* List of disassembler subclasses */
std::vector<Disassembler*> Disassembler::disassemblers;

/* Hook for construction */
void Disassembler::ctor() {
#if 0
    p_debug = stderr;
#endif
}

/* Initialize the class */
void
Disassembler::initclass()
{
    static bool initialized=false;
    if (!initialized) {
        initialized = true;
        register_subclass(new DisassemblerArm);
        register_subclass(new DisassemblerPowerpc);
        register_subclass(new DisassemblerX86);
    }
}

/* Class method to register a new disassembler subclass */
void
Disassembler::register_subclass(Disassembler *factory)
{
    initclass();
    ROSE_ASSERT(factory!=NULL);
    disassemblers.push_back(factory);
}

/* Class factory method */
Disassembler *
Disassembler::create(SgAsmInterpretation *interp)
{
    Disassembler *retval=NULL;
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (size_t i=0; i<headers.size(); i++) {
        Disassembler *candidate = create(headers[i]);
        if (retval && retval!=candidate)
            throw Exception("interpretation has multiple disassemblers");
        retval = candidate;
    }
    return retval;
}

/* Class factory method */
Disassembler *
Disassembler::create(SgAsmGenericHeader *header)
{
    initclass();
    for (size_t i=disassemblers.size(); i>0; --i) {
        Disassembler *factory = disassemblers[i-1];
        ROSE_ASSERT(factory);
        Disassembler *d = factory->can_disassemble(header);
        if (d)
            return d;
    }
    throw Exception("no disassembler for architecture");
}

/* High-level function for disassembling a whole interpretation. */
void
Disassembler::disassemble(SgAsmInterpretation *interp, AddressSet *successors, BadMap *bad)
{
    InstructionMap insns = disassembleInterp(interp, successors, bad);
    Partitioner *p = p_partitioner ? p_partitioner : new Partitioner;
    SgAsmBlock *top = p->partition(interp, insns);
    interp->set_global_block(top);
    top->set_parent(interp);
    if (!p_partitioner)
        delete p;
}

/* Class method for backward compatability with the old Disassembler name space. */
void
Disassembler::disassembleInterpretation(SgAsmInterpretation *interp)
{
    Disassembler *disassembler = Disassembler::create(interp);
    disassembler->disassemble(interp, NULL, NULL);
    delete disassembler;
}

/* Accessor */
void
Disassembler::set_wordsize(size_t n)
{
    ROSE_ASSERT(n>0);
    ROSE_ASSERT(n<=sizeof(rose_addr_t));
    p_wordsize = n;
}

/* Accessor */
void
Disassembler::set_alignment(size_t n)
{
#ifndef NDEBUG
    int nbits=0;
    for (size_t i=0; i<8*sizeof(n); i++)
        nbits += (((size_t)1<<i) & n) ? 1 : 0;
    ROSE_ASSERT(1==nbits);
#endif
    p_alignment = n;
}

/* Disassemble one instruction. */
SgAsmInstruction *
Disassembler::disassembleOne(const unsigned char *buf, rose_addr_t buf_va, size_t buf_size, rose_addr_t start_va,
                             AddressSet *successors)
{
    MemoryMap map;
    map.insert(MemoryMap::MapElement(buf_va, buf_size, buf, 0));
    return disassembleOne(&map, start_va, successors);
}

/* Disassemble one basic block. */
Disassembler::InstructionMap
Disassembler::disassembleBlock(const MemoryMap *map, rose_addr_t start_va, AddressSet *successors)
{
    InstructionMap insns;
    SgAsmInstruction *insn = NULL;
    rose_addr_t va = start_va;

    if (p_debug)
        fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: disassembling basic block\n", start_va);

    while (1) {
        /* Disassemble the instruction */
        try {
            insn = disassembleOne(map, va, NULL);
        } catch(const Exception &e) {
            if ((p_search & SEARCH_UNKNOWN) && e.bytes.size()>0) {
                insn = make_unknown_instruction(e);
            } else {
                if (insns.size()==0 || !(p_search & SEARCH_DEADEND)) {
                    if (p_debug)
                        fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: disassembly failed in basic block 0x%08"PRIx64": %s\n",
                                e.ip, start_va, e.mesg.c_str());
                    for (InstructionMap::iterator ii=insns.begin(); ii!=insns.end(); ++ii)
                        delete ii->second;
                    throw;
                }
                break;
            }
        }
        insns.insert(std::make_pair(va, insn));
        p_ndisassembled++;

        /* Is this the end of a basic block? */
        if (insn->terminatesBasicBlock()) {
            if (p_debug)
                fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: \"%s\" terminates block\n", va, unparseMnemonic(insn).c_str());
            break;
        }

        /* Advance to the next instruction */
        va += insn->get_raw_bytes().size();

        /* Progress report */
        if (0==p_ndisassembled % 10000)
            fprintf(stderr, "Disassembler[va 0x%08"PRIx64"]: disassembled %zu instructions\n", va, p_ndisassembled);
    }

    /* Add last instruction's successors to returned successors. */
    if (successors) {
        AddressSet suc = insn->get_successors();
        successors->insert(suc.begin(), suc.end());
        if (p_debug) {
            fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: basic block successors:", start_va);
            for (AddressSet::iterator si=suc.begin(); si!=suc.end(); si++)
                fprintf(p_debug, " 0x%08"PRIx64, *si);
            fprintf(p_debug, "\n");
        }
    }

    return insns;
}

/* Disassemble one basic block. */
Disassembler::InstructionMap
Disassembler::disassembleBlock(const unsigned char *buf, rose_addr_t buf_va, size_t buf_size, rose_addr_t start_va,
                               AddressSet *successors)
{
    MemoryMap map;
    map.insert(MemoryMap::MapElement(buf_va, buf_size, buf, 0));
    return disassembleBlock(&map, start_va, successors);
}

/* Disassemble reachable instructions from a buffer */
Disassembler::InstructionMap
Disassembler::disassembleBuffer(const MemoryMap *map, size_t start_va, AddressSet *successors, BadMap *bad)
{
    AddressSet worklist;
    worklist.insert(start_va);
    return disassembleBuffer(map, worklist, successors, bad);
}

/* Disassemble reachable instructions from a buffer */
Disassembler::InstructionMap
Disassembler::disassembleBuffer(const MemoryMap *map, AddressSet worklist, AddressSet *successors, BadMap *bad)
{
    InstructionMap insns;
    try {
        rose_addr_t next_search = 0;

        /* Per-buffer search methods */
        if (p_search & SEARCH_WORDS)
            search_words(&worklist, map, bad);

        /* Look for more addresses */
        if (worklist.size()==0 && (p_search & (SEARCH_ALLBYTES|SEARCH_UNUSED))) {
            bool avoid_overlap = (p_search & SEARCH_UNUSED) ? true : false;
            search_next_address(&worklist, next_search, map, insns, bad, avoid_overlap);
            if (worklist.size()>0)
                next_search = *(--worklist.end())+1;
        }

        while (worklist.size()>0) {
            /* Get next address to disassemble */
            AddressSet::iterator i = worklist.begin();
            rose_addr_t va = *i;
            worklist.erase(i);


            if (insns.find(va)!=insns.end() || (bad && bad->find(va)!=bad->end())) {
                /* Skip this if we've already tried to disassemble it. */
            } else if (NULL==map->find(va)) {
                /* Any address that's outside the range we're allowed to work on will be added to the successors. */
                if (successors)
                    successors->insert(va);
            } else {
                /* Disassemble a basic block and add successors to the work list. If a disassembly error occurs then
                 * disassembleBlock() will throw an exception that we'll add to the bad list. We must be careful when adding the
                 * basic block's instructions to the return value: although we check above to prevent disassembling the same
                 * basic block more than once, it's still possible that two basic blocks could overlap (e.g., block A could start
                 * at the second instruction of block B, or on a viariable-size instruction architecture, block A could start
                 * between instructions of block B and then become synchronized with B). */
                InstructionMap bb;
                try {
                    bb = disassembleBlock(map, va, &worklist);
                    for (InstructionMap::iterator bbi=bb.begin(); bbi!=bb.end(); ++bbi) {
                        InstructionMap::iterator exists = insns.find(bbi->first);
                        if (exists!=insns.end()) {
                            delete exists->second; /*don't delete bbi->second because we use it below*/
                            exists->second = bbi->second;
                        } else {
                            insns.insert(*bbi);
                        }
                    }
                } catch(const Exception &e) {
                    if (bad)
                        bad->insert(std::make_pair(va, e));
                }

                /* Per-basicblock search methods */
                if (p_search & SEARCH_FOLLOWING)
                    search_following(&worklist, bb, map, bad);
                if (p_search & SEARCH_IMMEDIATE)
                    search_immediate(&worklist, bb, map, bad);
            }

            /* Look for more addresses */
            if (worklist.size()==0 && (p_search & (SEARCH_ALLBYTES|SEARCH_UNUSED))) {
                bool avoid_overlap = (p_search & SEARCH_UNUSED) ? true : false;
                search_next_address(&worklist, next_search, map, insns, bad, avoid_overlap);
                if (worklist.size()>0)
                    next_search = *(--worklist.end())+1;

            }
        }
    } catch(...) {
        for (InstructionMap::iterator ii=insns.begin(); ii!=insns.end(); ++ii)
            delete ii->second;
        throw;
    }

    return insns;
}

/* Add basic block following address to work list. */
void
Disassembler::search_following(AddressSet *worklist, const InstructionMap &bb, const MemoryMap *map, const BadMap *bad)
{
    if (bb.size()==0)
        return;
    InstructionMap::const_iterator bbi = bb.end();
    --bbi;
    SgAsmInstruction *last_insn = bbi->second;
    rose_addr_t following_va = last_insn->get_address() + last_insn->get_raw_bytes().size();
    if (map->find(following_va) && (!bad || bad->find(following_va)==bad->end())) {
        if (p_debug && worklist->find(following_va)==worklist->end()) {
            rose_addr_t va = bb.begin()->first;
            fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: SEARCH_FOLLOWING added 0x%08"PRIx64"\n", va, following_va);
        }
        worklist->insert(following_va);
    }
}

/* Add values of immediate operands to work list */
void
Disassembler::search_immediate(AddressSet *worklist, const InstructionMap &bb,  const MemoryMap *map, const BadMap *bad)
{
    for (InstructionMap::const_iterator bbi=bb.begin(); bbi!=bb.end(); bbi++) {
        const std::vector<SgAsmExpression*> &operands = bbi->second->get_operandList()->get_operands();
        for (size_t i=0; i<operands.size(); i++) {
            uint64_t constant=0;
            switch (operands[i]->variantT()) {
                case V_SgAsmWordValueExpression:
                    constant = isSgAsmWordValueExpression(operands[i])->get_value();
                    break;
                case V_SgAsmDoubleWordValueExpression:
                    constant = isSgAsmDoubleWordValueExpression(operands[i])->get_value();
                    break;
                case V_SgAsmQuadWordValueExpression:
                    constant = isSgAsmQuadWordValueExpression(operands[i])->get_value();
                    break;
                default:
                    continue; /* Not an appropriately-sized constant */
            }
            if (map->find(constant) && (!bad || bad->find(constant)==bad->end())) {
                if (p_debug && worklist->find(constant)==worklist->end())
                    fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: SEARCH_IMMEDIATE added 0x%08"PRIx64"\n",
                            bbi->first, constant);
                worklist->insert(constant);
            }
        }
    }
}

/* Add word-aligned values to work list */
void
Disassembler::search_words(AddressSet *worklist, const MemoryMap *map, const BadMap *bad)
{
    const std::vector<MemoryMap::MapElement> &mes = map->get_elements();
    for (size_t i=0; i<mes.size(); i++) {
        const MemoryMap::MapElement &me = mes[i];
        rose_addr_t va = me.get_va();
        va = ALIGN_UP(va, p_alignment);

        /* Scan through this map element. */
        while (va+p_wordsize <= me.get_va()+me.get_size()) {
            rose_addr_t constant = 0; /*virtual address*/
            unsigned char buf[sizeof constant];
            ROSE_ASSERT(p_wordsize<=sizeof constant);
            if (map->read(buf, va, p_wordsize)<p_wordsize)
                break; /*shouldn't happen since we checked sizes above*/

            for (size_t i=0; i<p_wordsize; i++) {
                switch (p_sex) {
                    case SgAsmExecutableFileFormat::ORDER_LSB:
                        constant |= buf[i] << (8*i);
                        break;
                    case SgAsmExecutableFileFormat::ORDER_MSB:
                        constant |= buf[i] << (8*(p_wordsize-(i+1)));
                        break;
                    default:
                        ROSE_ASSERT(!"not implemented");
                }
            }
            if (map->find(constant) && (!bad || bad->find(constant)==bad->end())) {
                if (p_debug && worklist->find(constant)==worklist->end())
                    fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: SEARCH_WORD added 0x%08"PRIx64"\n", va, constant);
                worklist->insert(constant);
            }
            va += p_alignment;
        }
    }
}

/* Find next unused address. */
void
Disassembler::search_next_address(AddressSet *worklist, rose_addr_t start_va, const MemoryMap *map,
                                  const InstructionMap &insns, const BadMap *bad, bool avoid_overlap)
{
    /* Assume a maximum instruction size so that while we're search backward (by virtual address) through previously
     * disassembled instructions we don't have to go all the way to the beginning of the instruction map to prove that an
     * instruction doesn't overlap with a specified address. */
    rose_addr_t next_va = start_va;

    while (1) {

        /* Advance to the next valid mapped address if necessary by scanning for the first map element that has a higher
         * virtual address. */
        if (!map->find(next_va)) {
            const std::vector<MemoryMap::MapElement> &mes = map->get_elements();
            const MemoryMap::MapElement *me = NULL;
            for (size_t i=0; i<mes.size(); i++) {
                if (mes[i].get_va() > next_va) {
                    me = &(mes[i]);
                    break;
                }
            }
            if (!me) return; /*no subsequent valid mapped address*/
            next_va = me->get_va();
        }

        /* If we tried to disassemble at this address and failed, then try the next address. */
        if (bad && bad->find(next_va)!=bad->end()) {
            next_va++;
            continue; /*tail recursion*/
        }

        if (avoid_overlap) {
            /* Are their any instructions that overlap with this address? */
            SgAsmInstruction *overlap = find_instruction_containing(insns, next_va);
            if (overlap) {
                next_va = overlap->get_address() + overlap->get_raw_bytes().size() + 1;
                continue; /*tail recursion*/
            }
        } else if (insns.find(next_va)!=insns.end()) {
            /* There is an instruction starting at this address */
            next_va++;
            continue; /*tail recursion*/
        }

        if (p_debug)
            fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: SEARCH_%s added 0x%08"PRIx64"\n",
                    start_va, avoid_overlap?"UNUSED":"ALLBYTES", next_va);

        worklist->insert(next_va);
        return;
    }
}

void
Disassembler::search_function_symbols(AddressSet *worklist, const MemoryMap *map, SgAsmGenericHeader *header)
{
    struct T: public AstSimpleProcessing {
        T(AddressSet *wl, const MemoryMap *map, rose_addr_t base_va, FILE *f)
            : worklist(wl), map(map), base_va(base_va), p_debug(f) {}
        void visit(SgNode *node) {
            SgAsmGenericSymbol *symbol = isSgAsmGenericSymbol(node);
            if (symbol && symbol->get_type()==SgAsmGenericSymbol::SYM_FUNC) {
                SgAsmGenericSection *section = symbol->get_bound();
                if (section && (section->is_mapped() || section->get_contains_code())) {
                    rose_addr_t va = base_va + section->get_mapped_actual_rva();
                    if (map->find(va)) {
                        if (p_debug)
                            fprintf(p_debug, "Disassembler: SEARCH_FUNCSYMS added 0x%08"PRIx64" for \"%s\"\n",
                                    va, symbol->get_name()->c_str());
                        worklist->insert(va);
                    }
                }
            }
        }
        AddressSet *worklist;
        const MemoryMap *map;
        rose_addr_t base_va;
        FILE *p_debug;
    } t(worklist, map, header->get_base_va(), p_debug);
    t.traverse(header, preorder);
}

SgAsmInstruction *
Disassembler::find_instruction_containing(const InstructionMap &insns, rose_addr_t va)
{
    const size_t max_insns_size = 16;
    InstructionMap::const_iterator ii=insns.upper_bound(va);
    if (ii==insns.begin())
        return NULL;
    while (1) {
        --ii;
        ROSE_ASSERT(ii->first <= va);
        if (ii->first + max_insns_size < va)
            return NULL;
        if (ii->first + ii->second->get_raw_bytes().size() > va)
            return ii->second;
        if (ii==insns.begin())
            return NULL;
    }
}
    
/* Disassemble reachable instructions from a buffer */
Disassembler::InstructionMap
Disassembler::disassembleBuffer(const unsigned char *buf, rose_addr_t buf_va, size_t buf_size, rose_addr_t start_va,
                                AddressSet *successors, BadMap *bad)
{
    MemoryMap map;
    map.insert(MemoryMap::MapElement(buf_va, buf_size, buf, 0));
    return disassembleBuffer(&map, start_va, successors, bad);
}

/* Disassemble instructions in a single section. */
Disassembler::InstructionMap
Disassembler::disassembleSection(SgAsmGenericSection *section, rose_addr_t section_va, rose_addr_t start_offset,
                                 AddressSet *successors, BadMap *bad)
{
    SgAsmGenericFile *file = section->get_file();
    ROSE_ASSERT(file!=NULL);
    const void *file_buf = &(file->get_data()[0]);

    MemoryMap map;
    map.insert(MemoryMap::MapElement(section_va, section->get_size(), file_buf, section->get_offset()));
    return disassembleBuffer(&map, section_va+start_offset, successors, bad);
}

/* Disassemble instructions for an interpretation (set of headers) */
Disassembler::InstructionMap
Disassembler::disassembleInterp(SgAsmInterpretation *interp, AddressSet *successors, BadMap *bad)
{
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    AddressSet worklist;

    MemoryMap *map = NULL;
    if (interp->get_map() == NULL)
        map = new MemoryMap();
    
    for (size_t i=0; i<headers.size(); i++) {
        /* Seed disassembly with entry points. */
        SgRVAList entry_rvalist = headers[i]->get_entry_rvas();
        for (size_t j=0; j<entry_rvalist.size(); j++) {
            rose_addr_t entry_va = entry_rvalist[j].get_rva() + headers[i]->get_base_va();
            worklist.insert(entry_va);
        }

        /* Seed disassembly with function entry points. */
        if (p_search & SEARCH_FUNCSYMS)
            search_function_symbols(&worklist, map, headers[i]);

        /* Incrementally build the map describing the relationship between virtual memory and binary file(s). */
        if (map) {
            Loader *loader = Loader::find_loader(headers[i]);
            if (p_search & SEARCH_NONEXE) {
                loader->map_all_sections(map, headers[i]->get_sections()->get_sections());
            } else {
                loader->map_code_sections(map, headers[i]->get_sections()->get_sections());
            }
        }
    }

    /* Use map stored in interpretation, or save the one we just created. */
    if (map==NULL) {
        map = interp->get_map();
    } else {
        interp->set_map(map);
    }
    if (p_debug) {
        fprintf(p_debug, "Disassembler: MemoryMap for disassembly:\n");
        map->dump(p_debug, "    ");
    }

    /* Disassemble all that we've mapped, according to aggressiveness settings. */
    InstructionMap retval = disassembleBuffer(map, worklist, successors, bad);

    /* Mark the parts of the file corresponding to the instructions as having been referenced, since this is part of parsing. */
    mark_referenced_instructions(interp, map, retval);
    return retval;
}
        
/* Re-read instruction bytes from file if necessary in order to mark them as referenced. */
void
Disassembler::mark_referenced_instructions(SgAsmInterpretation *interp, const MemoryMap *map, const InstructionMap &insns)
{
    unsigned char buf[32];
    const MemoryMap::MapElement *me = NULL;
    SgAsmGenericFile *file = NULL;
    const SgAsmGenericFilePtrList &files = interp->get_files();
    bool was_tracking;

    try {
        for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            SgAsmInstruction *insn = ii->second;
            ROSE_ASSERT(insn->get_raw_bytes().size()<=sizeof buf);
            rose_addr_t va = insn->get_address();
            size_t nbytes = insn->get_raw_bytes().size();

            while (nbytes>0) {
                /* Find the map element and the file that goes with that element (if any) */
                if (!me || va<me->get_va() || va>=me->get_va()+me->get_size()) {
                    if (file)
                        file->set_tracking_references(was_tracking);
                    file = NULL;
                    me = map->find(insn->get_address());
                    ROSE_ASSERT(me!=NULL);
                    if (me->is_anonymous())
                        break;
                    for (size_t i=0; i<files.size(); i++) {
                        if (&(files[i]->get_data()[0]) == me->get_base()) {
                            file = files[i];
                            was_tracking = file->get_tracking_references();
                            file->set_tracking_references(true);
                            break;
                        }
                    }
                    ROSE_ASSERT(file);
                }

                /* Read the file */
                size_t me_offset = va - me->get_va();
                size_t n = std::min(nbytes, me->get_size()-me_offset);
                if (file) {
                    size_t file_offset = me->get_offset() + me_offset;
                    file->read_content(file_offset, buf, n);
                }
                nbytes -= n;
            }
        }
        if (file)
            file->set_tracking_references(was_tracking);
    } catch(...) {
        if (file)
            file->set_tracking_references(was_tracking);
        throw;
    }
}
