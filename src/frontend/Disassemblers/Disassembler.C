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
    return create(interp->get_header());
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
    SgAsmGenericHeader *header = interp->get_header();
    ROSE_ASSERT(header);
    InstructionMap insns = disassembleInterp(header, successors, bad);
    Partitioner *p = p_partitioner ? p_partitioner : new Partitioner;
    SgAsmBlock *top = p->partition(header, insns);
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

/* Class method for backward compatibility with the old Disassembler name space. */
void
Disassembler::disassembleFile(SgAsmFile *file)
{
    const SgAsmInterpretationPtrList& interps = file->get_interpretations();
    for (size_t i=0; i<interps.size(); i++)
        disassembleInterpretation(interps[i]);
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
    map.insert(MemoryMap::MapElement(buf_va, buf_size, 0));
    return disassembleOne(buf, &map, start_va, successors);
}

/* Disassemble one basic block. */
Disassembler::InstructionMap
Disassembler::disassembleBlock(const unsigned char *buf, const MemoryMap *map, rose_addr_t start_va,
                               AddressSet *successors)
{
    InstructionMap insns;
    SgAsmInstruction *insn = NULL;
    rose_addr_t va = start_va;

    if (p_debug)
        fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: disassembling basic block\n", start_va);

    while (1) {
        /* Disassemble the instruction */
        try {
            insn = disassembleOne(buf, map, va, NULL);
        } catch(const Exception &e) {
            if ((p_search & SEARCH_UNKNOWN) && e.bytes.size()>0) {
                insn = make_unknown_instruction(e);
            } else {
                if (insns.size()==0 || !(p_search & SEARCH_DEADEND)) {
                    if (p_debug)
                        fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: disassembly failed in basic block 0x%08"PRIx64": %s\n",
                                e.ip, start_va, e.mesg.c_str());
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
    map.insert(MemoryMap::MapElement(buf_va, buf_size, 0));
    return disassembleBlock(buf, &map, start_va, successors);
}

/* Disassemble reachable instructions from a buffer */
Disassembler::InstructionMap
Disassembler::disassembleBuffer(const unsigned char *buf, const MemoryMap *map, size_t start_va,
                                AddressSet *successors, BadMap *bad)
{
    AddressSet worklist;
    worklist.insert(start_va);
    return disassembleBuffer(buf, map, worklist, successors, bad);
}

/* Disassemble reachable instructions from a buffer */
Disassembler::InstructionMap
Disassembler::disassembleBuffer(const unsigned char *buf, const MemoryMap *map, AddressSet worklist,
                                AddressSet *successors, BadMap *bad)
{
    rose_addr_t next_search = 0;
    InstructionMap insns;

    /* Per-buffer search methods */
    if (p_search & SEARCH_WORDS)
        search_words(&worklist, buf, map, bad);

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
             * disassembleBlock() will throw an exception that we'll add to the bad list. */
            InstructionMap bb;
            try {
                bb = disassembleBlock(buf, map, va, &worklist);
                insns.insert(bb.begin(), bb.end());
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
Disassembler::search_words(AddressSet *worklist, const unsigned char *buf, const MemoryMap *map, const BadMap *bad)
{
    const std::vector<MemoryMap::MapElement> &mes = map->get_elements();
    for (size_t i=0; i<mes.size(); i++) {
        const MemoryMap::MapElement &me = mes[i];
        if (me.is_anonymous())
            continue;
        rose_addr_t va = me.get_va();
        va = (va+p_alignment-1) & ~(p_alignment-1); /*align*/
        while (va+p_wordsize <= me.get_va()+me.get_size()) {
            rose_addr_t constant = 0; /*virtual address*/
            ROSE_ASSERT(p_wordsize<=sizeof constant);
            for (size_t i=0; i<p_wordsize; i++) {
                switch (p_sex) {
                    case SgAsmExecutableFileFormat::ORDER_LSB:
                        constant |= buf[me.get_offset()+i] << (8*i);
                        break;
                    case SgAsmExecutableFileFormat::ORDER_MSB:
                        constant |= buf[me.get_offset()+i] << (8*(p_wordsize-(i+1)));
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
    map.insert(MemoryMap::MapElement(buf_va, buf_size, 0));
    return disassembleBuffer(buf, &map, start_va, successors, bad);
}

/* Disassemble instructions in a single mapped section. */
Disassembler::InstructionMap
Disassembler::disassembleSection(SgAsmGenericSection *section, rose_addr_t start_va,
                                 AddressSet *successors, BadMap *bad)
{
    SgAsmGenericFile *file = section->get_file();
    ROSE_ASSERT(file!=NULL);

    MemoryMap map;
    map.insert(section);

    SgFileContentList content = file->content(section->get_offset(), section->get_size());
    const unsigned char *buf = &(content[0]);
    return disassembleBuffer(buf, &map, start_va, successors, bad);
}

/* Disassemble instructions reachable from a file header. */
Disassembler::InstructionMap
Disassembler::disassembleInterp(SgAsmGenericHeader *header, AddressSet *successors, BadMap *bad)
{
    AddressSet worklist;

    /* Decide which sections should be disassembled. */
    Loader *loader = Loader::find_loader(header);
    MemoryMap *map = NULL;
    if (p_search & SEARCH_NONEXE) {
        map = loader->map_all_sections(header->get_sections()->get_sections());
    } else {
        map = loader->map_code_sections(header->get_sections()->get_sections());
    }

    /* Show final virtual address map */
    if (p_debug) {
        fprintf(stderr, "Disassembler: final virtual address to file offset mapping for disassembly:\n");
        map->dump(p_debug, "    ");
    }

    /* Seed the disassembly with the entry point(s) stored in the file header. */
    SgRVAList entry_rvalist = header->get_entry_rvas();
    for (size_t i=0; i<entry_rvalist.size(); i++) {
        rose_addr_t entry_va = entry_rvalist[i].get_rva() + header->get_base_va();
        worklist.insert(entry_va);
    }

    /* Obtain the entire file content. Note that this doesn't actually copy the file, it just returns a container that points
     * to the same file content as we originally read.  In fact, it doesn't even necessarily read any of the file except for
     * what we're actually disassembling. */
    SgAsmGenericFile *file = header->get_file();
    SgFileContentList file_content = file->content(0, file->get_orig_size());

    /* Disassemble all that we've mapped, according to aggressiveness settings. */
    InstructionMap retval = disassembleBuffer(&(file_content[0]), map, worklist, successors, bad);

    /* Mark the parts of the file corresponding to the instructions as having been referenced, since this is part of parsing. */
    bool was_tracking = file->get_tracking_references();
    file->set_tracking_references(true);
    try {
        mark_referenced_instructions(file, map, retval);
    } catch(...) {
        file->set_tracking_references(was_tracking);
        throw;
    }
    file->set_tracking_references(was_tracking);

    return retval;
}

/* Re-read instruction bytes from file if necessary in order to mark them as referenced. */
void
Disassembler::mark_referenced_instructions(SgAsmGenericFile *file, const MemoryMap *map, const InstructionMap &insns)
{
    if (file->get_tracking_references()) {
        unsigned char buf[32];
        for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            SgAsmInstruction *insn = ii->second;
            ROSE_ASSERT(insn->get_raw_bytes().size()<=sizeof buf);
            file->read_content(map, insn->get_address(), buf, insn->get_raw_bytes().size(), false);
        }
    }
}
