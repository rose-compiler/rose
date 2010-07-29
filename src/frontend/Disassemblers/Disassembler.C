// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "Assembler.h"
#include "AssemblerX86.h"
#include "AsmUnparser_compat.h"
#include "Disassembler.h"
#include "DisassemblerPowerpc.h"
#include "DisassemblerArm.h"
#include "DisassemblerX86.h"
#include "Loader.h"
#include "Partitioner.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

/* See header file for full documentation of all methods in this file. */

/* This has no other home, so it's here for now. */
rose_addr_t
SgAsmBlock::get_fallthrough_va()
{
    ROSE_ASSERT(!get_statementList().empty());
    SgAsmInstruction *last = isSgAsmInstruction(get_statementList().back());
    ROSE_ASSERT(last!=NULL);
    return last->get_address() + last->get_raw_bytes().size();
}

/* This has no other home, so it's here for now. Virtual method should be overridden by subclasses. */
std::set<rose_addr_t>
SgAsmInstruction::get_successors(bool *complete) {
    abort();
    // tps (12/9/2009) : MSC requires a return value
    std::set<rose_addr_t> t;
    return t;
}

/* This has no other home, so it's here for now. Virtual method to return successors of a basic block. */
std::set<rose_addr_t>
SgAsmInstruction::get_successors(const std::vector<SgAsmInstruction*>& basic_block, bool *complete/*out*/)
{
    if (basic_block.size()==0) {
        if (complete) *complete = true;
        return std::set<rose_addr_t>();
    }
    return basic_block.back()->get_successors(complete);
}

/* This has no other home, so it's here for now. */
bool
SgAsmInstruction::terminatesBasicBlock()
{
    abort();
    // tps (12/9/2009) : MSC requires a return value
    return false;
}

/** Virtual method to determine if a single instruction has an effect. Unless subclass redefines, assume all instructions have
 *  an effect. See SgAsmx86Instruction implementation for complete documentation. */
bool
SgAsmInstruction::has_effect()
{
    return true;
}

/** Virtual method to determine if an instruction sequence has an effect. Unless subclass redefines, assume all instruction
 *  sequences have an effect. See SgAsmx86Instruction implementation for complete documentation. */
bool
SgAsmInstruction::has_effect(const std::vector<SgAsmInstruction*>&, bool allow_branch/*false*/,
                             bool relax_stack_semantics/*false*/)
{
    return true;
}

/** Virtual method to find subsequences of an instruction sequence that are effectively no-ops. Unless subclass redefines,
 *  assume that the sequence has no no-op subsequences. See SgAsmx86Instruction implementation for complete documentation.
 *  
 *  FIXME: Instead of leaving this unimplemented, we could implement it in terms of has_effect() and let the subclasses
 *         reimplement it only if they can do so more efficiently (which they probably can). [RPM 2010-04-30] */
std::vector<std::pair<size_t,size_t> >
SgAsmInstruction::find_noop_subsequences(const std::vector<SgAsmInstruction*>& insns, bool allow_branch/*false*/, 
                                         bool relax_stack_semantics/*false*/)
{
    std::vector<std::pair<size_t, size_t> > retval;
    return retval;
}

/* List of disassembler subclasses */
std::vector<Disassembler*> Disassembler::disassemblers;

/* Hook for construction */
void Disassembler::ctor() {
#if 0
    p_debug = stderr;
#endif
}

unsigned
Disassembler::parse_switches(const std::string &s, unsigned flags)
{
    size_t at=0;
    while (at<s.size()) {
        enum { SET_BIT, CLEAR_BIT, SET_VALUE, NOT_SPECIFIED } howset = NOT_SPECIFIED;

        if (s[at]=='-') {
            howset = CLEAR_BIT;
            at++;
        } else if (s[at]=='+') {
            howset = SET_BIT;
            at++;
        } else if (s[at]=='=') {
            howset = SET_VALUE;
            at++;
        }
        if (at>=s.size())
            throw Exception("heuristic name must follow qualifier");
        
             
        size_t comma = s.find(",", at);
        std::string word = std::string(s, at, comma-at);
        if (word.size()==0)
            throw Exception("heuristic name must follow comma");
        
        unsigned bits = 0;
        if (word == "following") {
            bits = SEARCH_FOLLOWING;
        } else if (word == "immediate") {
            bits = SEARCH_IMMEDIATE;
        } else if (word == "words") {
            bits = SEARCH_WORDS;
        } else if (word == "allbytes") {
            bits = SEARCH_ALLBYTES;
        } else if (word == "unused") {
            bits = SEARCH_UNUSED;
        } else if (word == "nonexe") {
            bits = SEARCH_NONEXE;
        } else if (word == "deadend") {
            bits = SEARCH_DEADEND;
        } else if (word == "unknown") {
            bits = SEARCH_UNKNOWN;
        } else if (word == "funcsyms") {
            bits = SEARCH_FUNCSYMS;
        } else if (word == "default") {
            bits = SEARCH_DEFAULT;
            if (howset==NOT_SPECIFIED) howset = SET_VALUE;
        } else if (isdigit(word[0])) {
            bits = strtol(word.c_str(), NULL, 0);
        } else {
            throw Exception("unknown disassembler heuristic: " + word);
        }

        switch (howset) {
            case SET_VALUE:
                flags = 0;
            case NOT_SPECIFIED:
            case SET_BIT:
                flags |= bits;
                break;
            case CLEAR_BIT:
                flags &= ~bits;
                break;
        }

        at = comma==std::string::npos ? s.size() : comma+1;
    }
    return flags;
}

/* Initialize the class */
void
Disassembler::initclass()
{
    static bool initialized=false;
    if (!initialized) {
        initialized = true;
        register_subclass(new DisassemblerArm());
        register_subclass(new DisassemblerPowerpc());
        register_subclass(new DisassemblerX86(2)); /*16-bit*/
        register_subclass(new DisassemblerX86(4)); /*32-bit*/
        register_subclass(new DisassemblerX86(8)); /*64-bit*/
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

/* Class method */
Disassembler *
Disassembler::lookup(SgAsmInterpretation *interp)
{
    Disassembler *retval=NULL;
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (size_t i=0; i<headers.size(); i++) {
        Disassembler *candidate = lookup(headers[i]);
        if (retval && retval!=candidate)
            throw Exception("interpretation has multiple disassemblers");
        retval = candidate;
    }
    return retval;
}

/* Class method */
Disassembler *
Disassembler::lookup(SgAsmGenericHeader *header)
{
    initclass();
    for (size_t i=disassemblers.size(); i>0; --i) {
        if (disassemblers[i-1]->can_disassemble(header))
            return disassemblers[i-1];
    }
    throw Exception("no disassembler for architecture");
}

/* High-level function for disassembling a whole interpretation. */
void
Disassembler::disassemble(SgAsmInterpretation *interp, AddressSet *successors, BadMap *bad)
{
    InstructionMap insns = disassembleInterp(interp, successors, bad);
    Partitioner *p = p_partitioner ? p_partitioner : new Partitioner;
    if (p_debug && !p_partitioner)
        p->set_debug(get_debug());
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
    /* Create a new disassembler so we can modify its behavior locally. */
    Disassembler *disassembler = Disassembler::lookup(interp)->clone();

    /* Search methods specified with "-rose:disassembler_search" are stored in the SgFile object. Use them rather than the
     * defaults built into the Disassembler class. */
    SgNode *file = interp;
    while (file && !isSgFile(file)) file = file->get_parent();
    ROSE_ASSERT(file);
    disassembler->set_search(isSgFile(file)->get_disassemblerSearchHeuristics());

    /* Partitioning methods are specified with "-rose:partitioner_search" and are stored in SgFile also. Use them rather than
     * the default partitioner. */
    Partitioner *partitioner = new Partitioner;
    partitioner->set_search(isSgFile(file)->get_partitionerSearchHeuristics());

    /* Partitioner configuration file specified with "-rose:partitioner_config" is stored in SgFile. Use it rather than
     * the default configuration file. */
    partitioner->set_config(isSgFile(file)->get_partitionerConfigurationFileName());

    disassembler->set_partitioner(partitioner);
    disassembler->disassemble(interp, NULL, NULL);

    delete disassembler;
    delete partitioner;
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
    rose_addr_t va=0, next_va=start_va;

    if (p_debug)
        fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: disassembling basic block\n", start_va);

    do { /*tail recursion*/

        /* Disassemble each instruction of what we naively consider to be a basic block (semantic analysis may prove
         * otherwise). This loop exits locally if we reach an address that cannot be disassembled (and we're not calling
         * make_unknown_instruction()) or we reach an instruction that naively terminates a basic block.  In the former case,
         * INSN will be the last instruction, VA is its virtual address, and NEXT_VA is the address of the following
         * instruction; otherwise INSN is null, VA is the address where disassembly failed, and NEXT_VA is meaningless. */
        while (1) {
            va = next_va;
            try {
                insn = disassembleOne(map, va, NULL);
            } catch(const Exception &e) {
                if ((p_search & SEARCH_UNKNOWN) && e.bytes.size()>0) {
                    insn = make_unknown_instruction(e);
                } else {
                    if (insns.size()==0 || !(p_search & SEARCH_DEADEND)) {
                        if (p_debug)
                            fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: "
                                    "disassembly failed in basic block 0x%08"PRIx64": %s\n",
                                    e.ip, start_va, e.mesg.c_str());
                        for (InstructionMap::iterator ii=insns.begin(); ii!=insns.end(); ++ii)
                            SageInterface::deleteAST(ii->second);
                        throw;
                    }
                    /* Terminate tail recursion. Make sure we don't try to disassemble here again within this call, even if
                     * semantic analysis can prove that the next instruction address is the only possible successor. */
                    insn = NULL;
                    break;
                }
            }
            next_va = va + insn->get_raw_bytes().size();
            insns.insert(std::make_pair(va, insn));
            p_ndisassembled++;

            /* Is this the end of a basic block? This is naive logic that bases the decision only on the single instruction.
             * A more thorough analysis can be performed below in the get_block_successors() call. */          
            if (insn->terminatesBasicBlock()) {
                if (p_debug)
                    fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: \"%s\" at 0x%08"PRIx64" naively terminates block\n",
                            start_va, unparseMnemonic(insn).c_str(), va);
                break;
            }

            /* Progress report */
            if (0==p_ndisassembled % 10000)
                fprintf(stderr, "Disassembler[va 0x%08"PRIx64"]: disassembled %zu instructions\n", va, p_ndisassembled);
        }

        /* Try to figure out the successor addresses.  If we can prove that the only successor is the address following the
         * last instruction then we can continue disassembling as if this were a single basic block. */
        bool complete=false;
        AddressSet suc = get_block_successors(insns, &complete);
        if (insn && complete && suc.size()==1 && *(suc.begin())==next_va) {
            if (p_debug) {
                fprintf(p_debug,
                        "Disassembler[va 0x%08"PRIx64"]: semantic analysis proves basic block continues after 0x%08"PRIx64"\n",
                        start_va, va);
            }
        } else {
            insn = NULL; /*terminate recursion*/
        }

        /* Save block successors in return value before we exit scope */
        if (!insn && successors) {
            successors->insert(suc.begin(), suc.end());
            if (p_debug) {
                fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: basic block successors:", start_va);
                for (AddressSet::iterator si=suc.begin(); si!=suc.end(); si++)
                    fprintf(p_debug, " 0x%08"PRIx64, *si);
                fprintf(p_debug, "\n");
            }
        }
    } while (insn);
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
                            SageInterface::deleteAST(exists->second); /*don't delete bbi->second because we use it below*/
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
            SageInterface::deleteAST(ii->second);
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

    /* Use the memory map attached to the interpretation, or build a new one and attach it. */
    MemoryMap *map = interp->get_map();
    if (!map) {
        map = new MemoryMap();
        for (size_t i=0; i<headers.size(); i++) {
            if (NULL==interp->get_map()) {
                Loader *loader = Loader::find_loader(headers[i]);
                if (p_search & SEARCH_NONEXE) {
                    loader->map_all_sections(map, headers[i]->get_sections()->get_sections());
                } else {
                    loader->map_code_sections(map, headers[i]->get_sections()->get_sections());
                }
            }
        }
        interp->set_map(map);
    }
    if (p_debug) {
        fprintf(p_debug, "Disassembler: MemoryMap for disassembly:\n");
        map->dump(p_debug, "    ");
    }

    /* Seed disassembly with entry points and function symbols from each header. */
    for (size_t i=0; i<headers.size(); i++) {
        SgRVAList entry_rvalist = headers[i]->get_entry_rvas();
        for (size_t j=0; j<entry_rvalist.size(); j++) {
            rose_addr_t entry_va = entry_rvalist[j].get_rva() + headers[i]->get_base_va();
            worklist.insert(entry_va);
            if (p_debug)
                fprintf(p_debug, "Disassembler[va 0x%08"PRIx64"]: entry point\n", entry_va);
        }
        if (p_search & SEARCH_FUNCSYMS)
            search_function_symbols(&worklist, map, headers[i]);
    }

    /* Disassemble all that we've mapped, according to aggressiveness settings. */
    InstructionMap retval = disassembleBuffer(map, worklist, successors, bad);

#if 0
    /* Mark the parts of the file corresponding to the instructions as having been referenced, since this is part of parsing.
     *
     * NOTE: I turned this off because it's slow if there's a lot of instructions (e.g., about 20s/million instructions on my
     *       machine). If the user really needs to know this information they can probably calculate it using an ExtentMap and
     *       traversing the instructions in the final AST.  Another problem is that since the disassembler runs before the
     *       partitioner, and the partitioner might throw away unused instructions, calculating the references here in the
     *       disassembler is not accurate.  [RPM 2010-04-30] */
    mark_referenced_instructions(interp, map, retval);
#endif

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

    /* Re-read each instruction so the file has a chance to track the reference. */
    try {
        for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            SgAsmInstruction *insn = ii->second;
            ROSE_ASSERT(insn->get_raw_bytes().size()<=sizeof buf);
            rose_addr_t va = insn->get_address();
            size_t nbytes = insn->get_raw_bytes().size();

            while (nbytes>0) {
                /* Find the map element and the file that goes with that element (if any) */
                if (!me || va<me->get_va() || va>=me->get_va()+me->get_size()) {
                    if (file) {
                        file->set_tracking_references(was_tracking);
                        file = NULL;
                    }
                    me = map->find(va);
                    if (!me) {
                        /* This byte of the instruction is not mapped. Perhaps the next one is. */
                        ++va;
                        --nbytes;
                        continue;
                    }
                    if (!me->is_anonymous()) {
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
                }

                /* Read the file */
                size_t me_offset = va - me->get_va();
                size_t n = std::min(nbytes, me->get_size()-me_offset);
                if (file) {
                    size_t file_offset = me->get_offset() + me_offset;
                    file->read_content(file_offset, buf, n, false);
                }
                nbytes -= n;
                va += n;
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

/* Add last instruction's successors to returned successors. */
Disassembler::AddressSet
Disassembler::get_block_successors(const InstructionMap& insns, bool *complete)
{
    std::vector<SgAsmInstruction*> block;
    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii)
        block.push_back(ii->second);
    Disassembler::AddressSet successors = block.front()->get_successors(block, complete);

    /* For the purposes of disassembly, assume that a CALL instruction eventually executes a RET that causes execution to
     * resume at the address following the CALL. This is true 99% of the time.  Higher software layers (e.g., Partitioner) may
     * make other assumptions, which is why this code is not in SgAsmx86Instruction::get_successors(). [RPM 2010-05-09] */
    rose_addr_t target;
    SgAsmInstruction *last_insn = block.back();
    if (last_insn->is_function_call(block, &target))
        successors.insert(last_insn->get_address() + last_insn->get_raw_bytes().size());

    return successors;
}
