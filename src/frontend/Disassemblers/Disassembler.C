#include "rose.h"
#include "Disassembler.h"

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
    RvaFileMap map;
    map.insert(RvaFileMap::MapElement(buf_va, buf_size, 0));
    return disassembleOne(buf, map, start_va, successors);
}

/* Disassemble one basic block. */
Disassembler::InstructionMap
Disassembler::disassembleBlock(const unsigned char *buf, const RvaFileMap &map, rose_addr_t start_va,
                               AddressSet *successors) 
{
    InstructionMap insns;
    SgAsmInstruction *insn = NULL;
    rose_addr_t va = start_va;
    static size_t ndis=0;

    if (p_debug)
        fprintf(p_debug, "disassembling basic block at 0x%08"PRIx64"\n", start_va);

    while (1) {
        /* Disassemble the instruction */
        try {
            insn = disassembleOne(buf, map, va, NULL);
        } catch(const Exception &e) {
            if (insns.size()==0 || (p_search & SEARCH_DEADEND)==0) {
                if (p_debug)
                    fprintf(p_debug, "bb 0x%08"PRIx64": disassembly failed at 0x%08"PRIx64": %s\n", 
                            start_va, e.ip, e.mesg.c_str());
                throw;
            }
            break;
        }
        insns.insert(std::make_pair(va, insn));

        /* Is this the end of a basic block? */
        if (insn->terminatesBasicBlock())
            break;
        
        /* Advance to the next instruction */
        va += insn->get_raw_bytes().size();

        /* Progress report */
        if (0==++ndis % 10000)
            fprintf(stderr, "Disassembler: disassembled %zu instructions (now at 0x%08"PRIx64")\n", ndis, va);
    }
    
    /* Add last instruction's successors to returned successors. */
    if (successors) {
        AddressSet suc = insn->get_successors();
        successors->insert(suc.begin(), suc.end());
        if (p_debug) {
            fprintf(p_debug, "bb 0x%08"PRIx64": block successors:", start_va);
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
    RvaFileMap map;
    map.insert(RvaFileMap::MapElement(buf_va, buf_size, 0));
    return disassembleBlock(buf, map, start_va, successors);
}

/* Disassemble reachable instructions from a buffer */
Disassembler::InstructionMap
Disassembler::disassembleBuffer(const unsigned char *buf, const RvaFileMap &map, size_t start_va,
                                AddressSet *successors, BadMap *bad)
{
    AddressSet worklist;
    worklist.insert(start_va);
    return disassembleBuffer(buf, map, worklist, successors, bad);
}

/* Disassemble reachable instructions from a buffer */
Disassembler::InstructionMap
Disassembler::disassembleBuffer(const unsigned char *buf, const RvaFileMap &map, AddressSet worklist,
                                AddressSet *successors, BadMap *bad)
{
    InstructionMap insns;
    while (worklist.size()>0) {
        /* Get next address to disassemble */
        AddressSet::iterator i = worklist.begin();
        rose_addr_t va = *i;
        worklist.erase(i);

        /* Skip this if we've already tried to disassemble it. */
        if (insns.find(va)!=insns.end())
            continue;
        if (bad && bad->find(va)!=bad->end())
            continue;

        /* Any address that's outside the range we're allowed to work on will be added to the successors. */
        if (NULL==map.findVA(va)) {
            if (successors)
                successors->insert(va);
            continue;
        }

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

        /* Add the immediately following instruction address to the work list? */
        if ((p_search & SEARCH_FOLLOWING) && bb.size()>0) {
            InstructionMap::iterator bbi = bb.end();
            --bbi;
            SgAsmInstruction *last_insn = bbi->second;
            rose_addr_t following_va = last_insn->get_address() + last_insn->get_raw_bytes().size();
            if (p_debug && worklist.find(following_va)==worklist.end())
                fprintf(p_debug, "bb 0x%08"PRIx64": SEARCH_FOLLOWING adding 0x%08"PRIx64"\n", va, following_va);
            worklist.insert(following_va);
        }
    }
    return insns;
}

/* Disassemble reachable instructions from a buffer */
Disassembler::InstructionMap
Disassembler::disassembleBuffer(const unsigned char *buf, rose_addr_t buf_va, size_t buf_size, rose_addr_t start_va,
                                AddressSet *successors, BadMap *bad)
{
    RvaFileMap map;
    map.insert(RvaFileMap::MapElement(buf_va, buf_size, 0));
    return disassembleBuffer(buf, map, start_va, successors, bad);
}

/* Disassemble instructions in a single mapped section. */
Disassembler::InstructionMap
Disassembler::disassembleSection(SgAsmGenericSection *section, rose_addr_t start_va,
                                 AddressSet *successors, BadMap *bad)
{
    RvaFileMap map;
    map.insert(section);
    return disassembleBuffer(section->content(0, section->get_size()), map, start_va, successors, bad);
}

/* Disassemble instructions reachable from a file header. */
Disassembler::InstructionMap
Disassembler::disassembleInterp(SgAsmGenericHeader *header, AddressSet *successors, BadMap *bad)
{
    AddressSet worklist;

    /* Decide which sections should be disassembled. */
    RvaFileMap map;
    SgAsmGenericSectionList *seclist = header->get_sections();
    for (size_t i=0; i<seclist->get_sections().size(); i++) {
        SgAsmGenericSection *section = seclist->get_sections()[i];
        ROSE_ASSERT(section->get_header() == header);
        try {
            if (section->get_contains_code()) {
                map.insert(section);
            } else if (section->is_mapped() && section->get_mapped_xperm()) {
                map.insert(section);
            } else if (isSgAsmDOSFileHeader(header)) {
                map.insert(section);
            }
        } catch(const RvaFileMap::Exception &e) {
            std::ostringstream s;
            s <<"inconsistent mapping of file sections to virtual memory\n"
              <<"section [" <<section->get_id() <<"] \"" <<section->get_name()->get_string() <<"\""
              <<" at virtual address 0x" <<std::hex <<section->get_mapped_rva()
              <<" and file offset 0x" <<std::hex <<section->get_offset()
              <<" conflicts with address 0x" <<std::hex <<e.b.get_rva()
              <<" at offset 0x" <<std::hex <<e.b.get_offset()
              <<"\n";
            fputs(s.str().c_str(), stderr);
            throw Exception(s.str());
        }
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
    SgFileContentList file_content = file->content(0, file->get_orig_size(), false);

    /* Disassemble all that we've mapped, according to aggressiveness settings. */
    return disassembleBuffer(&(file_content[0]), map, worklist, successors, bad);
}
