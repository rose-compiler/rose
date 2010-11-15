/* Algorithms to detect what instructions make up basic blocks and which blocks make up functions, and how to create the
 * necessary SgAsmBlock and SgAsmFunctionDeclaration IR nodes from this information. */
#define __STDC_FORMAT_MACROS
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <inttypes.h>

#include "Partitioner.h"
#include "Assembler.h"
#include "AssemblerX86.h"
#include "AsmUnparser_compat.h"
#include "VirtualMachineSemantics.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>

/* See header file for full documentation. */


std::ostream& operator<<(std::ostream &o, const Partitioner::Exception &e)
{
    e.print(o);
    return o;
}

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

/** Returns reason string for this function. */
std::string
SgAsmFunctionDeclaration::reason_str(bool do_pad) const
{
    return reason_str(do_pad, get_reason());
}

/** Class method that converts a reason bit vector to a human-friendly string. The second argument is the bit vector of
 *  SgAsmFunctionDeclaration::FunctionReason bits. */
std::string
SgAsmFunctionDeclaration::reason_str(bool do_pad, unsigned r)
{
    std::string result;

    /* entry point and instruction heads are mutually exclusive, so we use the same column for both when padding. */
    if (r & FUNC_ENTRY_POINT) {
        add_to_reason_string(result, true, do_pad, "E", "entry point");
    } else {
        add_to_reason_string(result, (r & FUNC_INSNHEAD), do_pad, "H", "insn head");
    }

    /* Function call:
     *   "C" means the function was detected because we saw a call-like instructon (such as x86 CALL or FARCALL) or instruction
     *       sequence (such as pushing the return value and then branching) in code that was determined to be reachable by
     *       analyzing the control flow graph.
     *
     *   "c" means this function is the target of some call-like instruction (such as x86 CALL or FARCALL) but could not
     *       determine whether the instruction is actually executed.
     */
    if (r & FUNC_CALL_TARGET) {
        add_to_reason_string(result, true, do_pad, "C", "function call");
    } else {
        add_to_reason_string(result, (r & FUNC_CALL_INSN), do_pad, "c", "call instruction");
    }

    add_to_reason_string(result, (r & FUNC_EH_FRAME),    do_pad, "X", "exception frame");
    add_to_reason_string(result, (r & FUNC_IMPORT),      do_pad, "I", "import");
    add_to_reason_string(result, (r & FUNC_SYMBOL),      do_pad, "S", "symbol");
    add_to_reason_string(result, (r & FUNC_PATTERN),     do_pad, "P", "pattern");
    add_to_reason_string(result, (r & FUNC_GRAPH),       do_pad, "G", "graph");
    add_to_reason_string(result, (r & FUNC_USERDEF),     do_pad, "U", "user defined");
    add_to_reason_string(result, (r & FUNC_INTERPAD),    do_pad, "N", "padding");
    add_to_reason_string(result, (r & FUNC_DISCONT),     do_pad, "D", "discontiguous");
    add_to_reason_string(result, (r & FUNC_LEFTOVERS),   do_pad, "L", "leftovers");
    return result;
}
/*
 *
 *******************************************************************************************************************************/

/* Progress report class variables. */
time_t Partitioner::progress_interval = 10;
time_t Partitioner::progress_time = 0;
FILE *Partitioner::progress_file = stderr;

/* Produce a progress report if enabled. */
void
Partitioner::progress(FILE *debug, const char *fmt, ...) const
{
    va_list ap;
    va_start(ap, fmt);

    time_t now = time(NULL);
    
    if (0==progress_time)
        progress_time = now;
    
    if (progress_file!=NULL && now-progress_time >= progress_interval) {
        progress_time = now;
        vfprintf(progress_file, fmt, ap);
    }

    if (debug!=NULL)
        vfprintf(debug, fmt, ap);
}

/* Parse argument for "-rose:partitioner_search" command-line swich. */
unsigned
Partitioner::parse_switches(const std::string &s, unsigned flags)
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
        if (word=="entry" || word=="entry_point") {
            bits = SgAsmFunctionDeclaration::FUNC_ENTRY_POINT;
        } else if (word=="call" || word=="call_target") {
            bits = SgAsmFunctionDeclaration::FUNC_CALL_TARGET;
        } else if (word=="eh" || word=="eh_frame") {
            bits = SgAsmFunctionDeclaration::FUNC_EH_FRAME;
        } else if (word=="import") {
            bits = SgAsmFunctionDeclaration::FUNC_IMPORT;
        } else if (word=="symbol") {
            bits = SgAsmFunctionDeclaration::FUNC_SYMBOL;
        } else if (word=="pattern") {
            bits = SgAsmFunctionDeclaration::FUNC_PATTERN;
        } else if (word=="userdef") {
            bits = SgAsmFunctionDeclaration::FUNC_USERDEF;
        } else if (word=="pad" || word=="padding" || word=="interpad") {
            bits = SgAsmFunctionDeclaration::FUNC_INTERPAD;
        } else if (word=="unassigned" || word=="unclassified" || word=="leftover" || word=="leftovers") {
            bits = SgAsmFunctionDeclaration::FUNC_LEFTOVERS;
        } else if (word=="default") {
            bits = SgAsmFunctionDeclaration::FUNC_DEFAULT;
            if (howset==NOT_SPECIFIED) howset = SET_VALUE;
        } else if (isdigit(word[0])) {
            bits = strtol(word.c_str(), NULL, 0);
        } else {
            throw Exception("unknown partitioner heuristic: \"" + word + "\"");
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

/** Runs local block analyses if their cached results are invalid and caches the results.  A local analysis is one whose
 *  results only depend on the specified block and which are valid into the future as long as the instructions in the block do
 *  not change. */
void
Partitioner::update_analyses(BasicBlock *bb)
{
    ROSE_ASSERT(bb!=NULL && !bb->insns.empty());
    if (bb->valid_cache()) return;

    /* Successor analysis */
    bb->cache.sucs = bb->insns.front()->get_successors(bb->insns, &(bb->cache.sucs_complete));

    /* Call target analysis. For x86, a function call is any CALL instruction except when the call target is the fall-through
     * address and the instruction at the fall-through address pops the top of the stack (this is how position independent
     * code loads EIP into a general-purpose register). FIXME: For now we'll assume that any call to the fall-through address
     * is not a function call. */
    rose_addr_t fallthrough_va = bb->last_insn()->get_address() + bb->last_insn()->get_raw_bytes().size();
    rose_addr_t target_va = NO_TARGET;
    bool looks_like_call = bb->insns.front()->is_function_call(bb->insns, &target_va);
    if (looks_like_call && target_va!=fallthrough_va) {
        bb->cache.is_function_call = true;
        bb->cache.call_target = target_va;
    } else {
        bb->cache.is_function_call = false;
        bb->cache.call_target = NO_TARGET;
    }

    /* Function return analysis */
    bb->cache.function_return = !bb->cache.sucs_complete &&
                                bb->insns.front()->is_function_return(bb->insns);

    bb->validate_cache();
}

/** Returns true if basic block appears to end with a function call.  If the call target can be determined and @p target_va is
 *  non-null, then @p target_va will be initialized to contain the virtual address of the call target; otherwise it will
 *  contain the constant NO_TARGET. */
bool
Partitioner::is_function_call(BasicBlock *bb, rose_addr_t *target_va)
{
    update_analyses(bb); /*make sure cache is current*/
    if (target_va) *target_va = bb->cache.call_target;
    return bb->cache.is_function_call;
}

/** Returns known successors of a basic block.
 *
 *  There are two types of successor analyses:  one is an analysis that depends only on the instructions of the basic block
 *  for which successors are being calculated.  It is safe to cache these based on properties of the block itself (e.g., the
 *  number of instructions in the block).
 *
 *  The other category is analyses that depend on other blocks, such as determining whether the target of an x86 CALL
 *  instruction returns to the instruction after the CALL site.  The results of these analyses cannot be cached at the block
 *  that needs them and must be recomputed for each call.  However, they can be cached at either the block or function that's
 *  analyzed, so recomputing them here in this block is probably not too expensive.
 *
 *  All successor addresses are translated according to the alias_for links in existing blocks via calls to canonic_block(). */
Disassembler::AddressSet
Partitioner::successors(BasicBlock *bb, bool *complete)
{
    update_analyses(bb); /*make sure cache is current*/

    /* Follow alias_for links. */
    Disassembler::AddressSet retval;
    for (Disassembler::AddressSet::const_iterator si=bb->cache.sucs.begin(); si!=bb->cache.sucs.end(); ++si)
        retval.insert(canonic_block(*si));
    if (complete) *complete = bb->cache.sucs_complete;

    /* Run non-local analyses if necessary. These are never cached here in this block. */

    /* If this block ends with what appears to be a function call then we should perhaps add the fall-through address as a
     * successor. */
    if (bb->cache.is_function_call) {
        rose_addr_t fall_through_va = canonic_block(bb->last_insn()->get_address() + bb->last_insn()->get_raw_bytes().size());
        rose_addr_t call_target_va = call_target(bb);
        if (call_target_va!=NO_TARGET) {
            BasicBlock *target_bb = find_bb_starting(call_target_va, false);
            if (target_bb && target_bb->function && target_bb->function->returns)
                retval.insert(fall_through_va);
        } else {
            retval.insert(fall_through_va); /*true 99% of the time*/
        }
    }

    return retval;
}

/** Returns call target if block could be a function call. If the specified block looks like it could be a function call
 *  (using only local analysis) then return the call target address.  If the block does not look like a function call or the
 *  target address cannot be statically computed, then return Partitioner::NO_TARGET. */
rose_addr_t
Partitioner::call_target(BasicBlock *bb)
{
    update_analyses(bb); /*make sure cache is current*/
    if (bb->cache.call_target==NO_TARGET) return NO_TARGET;
    return canonic_block(bb->cache.call_target);
}

/* Returns true if the basic block at the specified virtual address appears to pop the return address from the top of the
 * stack without returning.
 *
 * FIXME: This is far from perfect: it analyzes only the first basic block; it may have incomplete information about where the
 *        basic block ends due to not yet having discovered all incoming CFG edges; it doesn't consider cases where the return
 *        value is popped but saved and restored later; etc.  It also only handles x86 instructions at this time.
 *        [RPM 2010-04-30] */
bool
Partitioner::pops_return_address(rose_addr_t va)
{
    bool on_stack = true; /*assume return value stays on stack; prove otherwise*/

    /* Create the basic block if possible, but if we created it here then we should clear it below. */
    BasicBlock *bb = find_bb_containing(va, false);
    bool preexisting = bb!=NULL;
    if (!bb) bb = find_bb_containing(va);
    if (!bb) return false;
    try {

        SgAsmx86Instruction *last_insn = isSgAsmx86Instruction(bb->last_insn());

        typedef VirtualMachineSemantics::Policy Policy;
        typedef X86InstructionSemantics<Policy, VirtualMachineSemantics::ValueType> Semantics;
        Policy policy;
        policy.set_map(get_map());
        VirtualMachineSemantics::ValueType<32> orig_retaddr;
        policy.writeMemory(x86_segreg_ss, policy.readGPR(x86_gpr_sp), orig_retaddr, policy.true_());
        Semantics semantics(policy);

#if 0
        fputs("Partitioner::pops_return_address:\n", stderr);
#endif
        try {
            for (std::vector<SgAsmInstruction*>::iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(*ii);
                if (!insn) return false;
                if (insn==last_insn && insn->get_kind()==x86_ret) break;
                semantics.processInstruction(insn);
#if 0
                std::ostringstream s;
                s << "Analysis for " <<unparseInstructionWithAddress(insn) <<std::endl
                  <<policy.get_state()
                fputs(s.str().c_str(), stderr);
#endif
            }
            on_stack = policy.on_stack(orig_retaddr);
            if (!on_stack && debug)
                fprintf(debug, "[B%08"PRIx64" discards return address]", va);
        } catch (const Semantics::Exception&) {
            /*void*/
        } catch (const Policy::Exception&) {
            /*void*/
        }
        
    } catch(...) {
        if (!preexisting)
            discard(bb);
        throw;
    }

    /* We don't want to have a basic block created just because we did some analysis. */
    if (!preexisting)
        discard(bb);

    /* Is the original return value still on the stack? */
    return !on_stack;
}

Partitioner::BasicBlock*
Partitioner::discard(BasicBlock *bb)
{
    if (bb!=NULL) {
        /* Erase the block from the list of known blocks. */
        BasicBlocks::iterator bbi=blocks.find(address(bb));
        ROSE_ASSERT(bbi!=blocks.end());
        ROSE_ASSERT(bbi->second==bb);
        blocks.erase(bbi);

        /* Erase the instruction-to-block link in the insn2block map. */
        for (size_t i=0; i<bb->insns.size(); ++i) {
            SgAsmInstruction *insn = bb->insns[i];
            std::map<rose_addr_t, BasicBlock*>::iterator bbi = insn2block.find(insn->get_address());
            ROSE_ASSERT(bbi!=insn2block.end());
            ROSE_ASSERT(bbi->second==bb);
            bbi->second = NULL; /*much faster than erasing, as determined by real testing*/
        }

        delete bb;
    }
    return NULL;
}

/* Returns instruction with highest address */
SgAsmInstruction *
Partitioner::BasicBlock::last_insn() const
{
    ROSE_ASSERT(insns.size()>0);
    return insns.back();
}

/* Release all blocks from a function. Do not delete the blocks. */
void
Partitioner::Function::clear_blocks()
{
    for (BasicBlocks::iterator bi=blocks.begin(); bi!=blocks.end(); ++bi)
        bi->second->function = NULL;
    blocks.clear();
}

/* Return this function's block having the highest address. */
Partitioner::BasicBlock *
Partitioner::Function::last_block() const
{
    ROSE_ASSERT(blocks.size()>0);
    BasicBlocks::const_iterator bi = blocks.end();
    --bi;
    return bi->second;
}

/* Return partitioner to initial state */
void
Partitioner::clear()
{
    /* Delete all functions */
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        fi->second->clear_blocks();
        delete fi->second;
    }
    functions.clear();

    /* Delete all basic blocks. We don't need to call Partitioner::discard() to fix up ptrs because all functions that might
     * have pointed to this block have already been deleted, and the insn2block map has also been cleared. */
    insn2block.clear();
    for (BasicBlocks::iterator bi=blocks.begin(); bi!=blocks.end(); ++bi)
        delete bi->second;
    blocks.clear();

    /* Delete all block IPD configuration data. */
    for (BlockConfigMap::iterator bci=block_config.begin(); bci!=block_config.end(); ++bci)
        delete bci->second;
    block_config.clear();

    /* Release all instructions (but do not delete) and disassembly failures from the cache. */
    insns.clear();
    clear_disassembler_errors();
}

void
Partitioner::load_config(const std::string &filename) {
    if (filename.empty())
        return;
#ifdef _MSC_VER /* tps (06/23/2010) : Does not work under Windows */
    throw IPDParser::Exception("IPD parsing not supported on Windows platforms");
#else
    int fd = open(filename.c_str(), O_RDONLY);
    if (fd<0)
        throw IPDParser::Exception(strerror(errno), filename);
    struct stat sb;
    fstat(fd, &sb);
    char *config = new char[sb.st_size];
    ssize_t nread = read(fd, config, sb.st_size);
    if (nread<0 || nread<sb.st_size)
        throw IPDParser::Exception(strerror(errno), filename);
    IPDParser(this, config, sb.st_size, filename).parse();
    delete[] config;
    close(fd);
#endif
}

/* Return address of first instruction of basic block */
rose_addr_t
Partitioner::address(BasicBlock* bb) const
{
    ROSE_ASSERT(bb->insns.size()>0);
    return bb->insns.front()->get_address();
}

/* Reduces the size of a basic block by truncating its list of instructions.  The new block contains initial instructions up
 * to but not including the instruction at the specified virtual address.  The addresses of the instructions (aside from the
 * instruction with the specified split point), are irrelevant since the choice of where to split is based on the relative
 * positions in the basic block's instruction vector rather than instruction address. */
void
Partitioner::truncate(BasicBlock* bb, rose_addr_t va)
{
    ROSE_ASSERT(bb);
    ROSE_ASSERT(bb==find_bb_containing(va));

    /* Find the cut point in the instruction vector. I.e., the first instruction to remove from the vector. */
    std::vector<SgAsmInstruction*>::iterator cut = bb->insns.begin();
    while (cut!=bb->insns.end() && (*cut)->get_address()!=va) ++cut;
    ROSE_ASSERT(cut!=bb->insns.begin()); /*we can't remove them all since basic blocks are never empty*/

    /* Remove instructions from the cut point and beyond. */
    for (std::vector<SgAsmInstruction*>::iterator ii=cut; ii!=bb->insns.end(); ++ii) {
        ROSE_ASSERT(insn2block[(*ii)->get_address()] == bb);
        insn2block[(*ii)->get_address()] = NULL;
    }
    bb->insns.erase(cut, bb->insns.end());
}

/* Append instruction to basic block */
void
Partitioner::append(BasicBlock* bb, SgAsmInstruction* insn)
{
    ROSE_ASSERT(bb);
    ROSE_ASSERT(insn);
    ROSE_ASSERT(insn2block[insn->get_address()]==NULL); /*insn must not already belong to a basic block*/
    bb->insns.push_back(insn);
    insn2block[insn->get_address()] = bb;
}

/* Append basic block to function */
void
Partitioner::append(Function* f, BasicBlock *bb)
{
    ROSE_ASSERT(f);
    ROSE_ASSERT(bb);
    if (bb->function==f) return;
    ROSE_ASSERT(bb->function==NULL);
    bb->function = f;
    f->blocks[address(bb)] = bb;

    /* If the block is a function return then mark the function as returning.  On a transition from a non-returning function
     * to a returning function, we must mark all calling functions as pending so that the fall-through address of their
     * function calls to this function are eventually discovered.  This includes recursive calls since we may have already
     * discovered the recursive call but not followed the fall-through address.
     *
     * FIXME: It's probably no longer necessary to go back and mark calling functions as pending because we do that in the
     *        analyze_cfg() loop.  Doing it in analyze_cfg() is probably more efficient than running these nested loops each
     *        time we have a transition. [RPM 2010-07-30] */
    update_analyses(bb);
    if (bb->cache.function_return && !f->returns) {
        f->returns = true;
        if (debug) fprintf(debug, "[returns-to");
        for (BasicBlocks::iterator bbi=blocks.begin(); bbi!=blocks.end(); ++bbi) {
            if (bbi->second->function!=NULL) {
                const Disassembler::AddressSet &sucs = successors(bbi->second, NULL);
                for (Disassembler::AddressSet::const_iterator si=sucs.begin(); si!=sucs.end(); ++si) {
                    if (*si==f->entry_va) {
                        if (debug) fprintf(debug, " F%08"PRIx64, bbi->second->function->entry_va);
                        bbi->second->function->pending = true;
                        break;
                    }
                }
            }
        }
        if (debug) fprintf(debug, "]");
    }
}

/* Remove a basic block from a function */
void
Partitioner::remove(Function* f, BasicBlock* bb) 
{
    ROSE_ASSERT(f);
    ROSE_ASSERT(bb);
    ROSE_ASSERT(bb->function==f);
    bb->function = NULL;
    f->blocks.erase(address(bb));
}

/* Finds (or possibly creates) an instruction at the specified address. */
SgAsmInstruction *
Partitioner::find_instruction(rose_addr_t va, bool create/*=true*/)
{
    Disassembler::InstructionMap::iterator ii = insns.find(va);
    if (create && disassembler && ii==insns.end() && bad_insns.find(va)==bad_insns.end()) {
        SgAsmInstruction *insn = NULL;
        try {
            insn = disassembler->disassembleOne(map, va, NULL);
            ii = insns.insert(std::make_pair(va, insn)).first;
        } catch (const Disassembler::Exception &e) {
            bad_insns.insert(std::make_pair(va, e));
        }
    }
    return ii==insns.end() ? NULL : ii->second;
}

/** Finds a basic block containing the specified instruction address. If no basic block exists and @p create is set, then a
 *  new block is created which starts at the specified address.  The return value, in the case when a block already exists,
 *  may be a block where the specified virtual address is either the beginning of the block or somewhere inside the block. In
 *  any case, the virtual address will always represent a function.
 *
 *  If no instruction can be found at the specified address then no block is created and a null pointer is returned.
 *
 *  Blocks are created by adding the initial instruction to the block, then repeatedly attempting to add more instructions as
 *  follows: if the block successors can all be statically determined, and there is exactly one successor, and that successor
 *  is not already part of a block, then the successor is appended to the block.
 *
 *  Block creation is recursive in nature since the computation of a (partial) block's successors might require creation of
 *  other blocks. Consider the case of an x86 CALL instruction:  after a CALL is appended to a block, the successors are
 *  calculated by looking at the target of the CALL. If the target is known and it can be proved that the target block
 *  (recursively constructed) discards the return address, then the fall-through address of the CALL is not a direct
 *  successor.
 *
 *  See also, set_allow_discontiguous_blocks().
 */
Partitioner::BasicBlock *
Partitioner::find_bb_containing(rose_addr_t va, bool create/*true*/)
{
    if (insn2block[va]!=NULL)
        return insn2block[va];
    if (!create)
        return NULL;

    BasicBlock *bb = NULL;
    while (1) {
        if (insn2block[va]!=NULL) break; /*we've reached another block*/
        SgAsmInstruction *insn = find_instruction(va);
        if (!insn) break;
        if (!bb) {
            bb = new BasicBlock;
            blocks.insert(std::make_pair(va, bb));
        }
        append(bb, insn);
        va += insn->get_raw_bytes().size();
        if (insn->terminatesBasicBlock()) { /*naively terminates?*/
            bool complete;
            const Disassembler::AddressSet& sucs = successors(bb, &complete);
            if ((func_heuristics & SgAsmFunctionDeclaration::FUNC_CALL_TARGET) && is_function_call(bb, NULL)) {
                /* When we are detecting functions based on x86 CALL instructions (or similar for other architectures) then
                 * the instruction after the CALL should never be part of this basic block. Otherwise allow the call to be
                 * part of the basic block initially and we'll split the block later if we need to. */
                break;
            } else if (allow_discont_blocks) {
                if (!complete || sucs.size()!=1)
                    break;
                va = *(sucs.begin());
            } else {
                if (!complete || sucs.size()!=1 || *(sucs.begin())!=va)
                    break;
            }
        }
    }
    ROSE_ASSERT(!bb || bb->insns.size()>0);
    return bb;
}

/** Makes sure the block at the specified address exists.  This is similar to find_bb_containing() except it makes sure that
 *  @p va starts a new basic block if it was previously in the middle of a block.  If an existing block had to be truncated to
 *  start this new block then the original block's function is marked as pending rediscovery. */
Partitioner::BasicBlock *
Partitioner::find_bb_starting(rose_addr_t va, bool create/*true*/)
{
    BasicBlock *bb = find_bb_containing(va, create);
    if (!bb)
        return NULL;
    if (va==address(bb))
        return bb;
    if (!create)
        return NULL;
    if (debug)
        fprintf(debug, "[split from B%08"PRIx64"]", address(bb));
    if (bb->function!=NULL)
        bb->function->pending = true;
    truncate(bb, va);
    bb = find_bb_containing(va);
    ROSE_ASSERT(bb!=NULL);
    ROSE_ASSERT(va==address(bb));
    return bb;
}

/** Folows alias_for links in basic blocks. The input value is the virtual address of a basic block (which need not exist). We
 *  recursively look up the specified block and follow its alias_for link until either the block does not exist or it has no
 *  alias_for. */
rose_addr_t
Partitioner::canonic_block(rose_addr_t va)
{
    for (size_t i=0; i<100; i++) {
        BasicBlock *bb = find_bb_starting(va, false);
        if (!bb || !bb->cache.alias_for) return va;
        if (debug) fprintf(debug, "[B%08"PRIx64"->B%08"PRIx64"]", va, bb->cache.alias_for);
        va = bb->cache.alias_for;
    }
    ROSE_ASSERT(!"possible alias loop");
}

/* Finds an existing function definition. */
Partitioner::Function *
Partitioner::find_function(rose_addr_t entry_va)
{
    Functions::iterator fi = functions.find(entry_va);
    if (fi==functions.end()) return NULL;
    return fi->second;
}

/* Adds or updates a function definition. */
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

/* Do whatever's necessary to finish loading IPD configuration. */
void
Partitioner::mark_ipd_configuration()
{
    for (BlockConfigMap::iterator bci=block_config.begin(); bci!=block_config.end(); ++bci) {
        rose_addr_t va = bci->first;
        BlockConfig *bconf = bci->second;

        BasicBlock *bb = find_bb_starting(va);
        if (!bb)
            throw Exception("cannot obtain IPD-specified basic block at " + StringUtility::addrToString(va));
        if (bb->insns.size()<bconf->ninsns)
            throw Exception("cannot obtain " + StringUtility::numberToString(bconf->ninsns) + "-instruction basic block at " +
                            StringUtility::addrToString(va) + " (only " + StringUtility::numberToString(bb->insns.size()) +
                            " available)");
        if (bb->insns.size()>bconf->ninsns)
            truncate(bb, bb->insns[bconf->ninsns]->get_address());

        /* Initial analysis followed augmented by settings from the configuration. */
        update_analyses(bb);
        bb->cache.alias_for = bconf->alias_for;
        if (bconf->sucs_specified) {
            bb->cache.sucs = bconf->sucs;
            bb->cache.sucs_complete = bconf->sucs_complete;
        }
        if (!bconf->sucs_program.empty()) {
            /* "Execute" the program that will detect successors. We do this by interpreting the basic block to initialize
             * registers, loading the successor program, pushing some arguments onto the program's stack, interpreting the
             * program, extracting return values from memory, and unloading the program. */
            bool debug = false;
            char block_name_str[64];
            sprintf(block_name_str, "B%08"PRIx64, va);
            std::string block_name = block_name_str;
            if (debug) fprintf(stderr, "running successors program for %s\n", block_name_str);

            MemoryMap *map = get_map();
            ROSE_ASSERT(map!=NULL);
            using namespace VirtualMachineSemantics;
            typedef X86InstructionSemantics<Policy, ValueType> Semantics;
            Policy policy;
            policy.set_map(map);
            Semantics semantics(policy);
            
            if (debug) fprintf(stderr, "  running semantics for the basic block...\n");
            for (std::vector<SgAsmInstruction*>::iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(*ii);
                ROSE_ASSERT(insn!=NULL);
                semantics.processInstruction(insn);
            }

            /* Load the program. Keep at least one unmapped byte between the program text, stack, and svec areas in order to
             * help with debugging. */       
            if (debug) fprintf(stderr, "  loading the program...\n");

            /* Load the instructions to execute */
            rose_addr_t text_va = map->find_free(0, bconf->sucs_program.size(), 4096);
            MemoryMap::MapElement text_me(text_va, bconf->sucs_program.size(), &(bconf->sucs_program[0]), 0, 
                                          MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_EXEC);
            text_me.set_name(block_name + " successors program text");
            map->insert(text_me);

            /* Create a stack */
            static const size_t stack_size = 8192;
            rose_addr_t stack_va = map->find_free(text_va+bconf->sucs_program.size()+1, stack_size, 4096);
            MemoryMap::MapElement stack_me(stack_va, stack_size, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE);
            stack_me.set_name(block_name + " successors stack");
            map->insert(stack_me);
            rose_addr_t stack_ptr = stack_va + stack_size;

            /* Create an area for the returned vector of successors */
            static const size_t svec_size = 8192;
            rose_addr_t svec_va = map->find_free(stack_va+stack_size+1, svec_size, 4096);
            MemoryMap::MapElement svec_me(svec_va, svec_size, MemoryMap::MM_PROT_READ|MemoryMap::MM_PROT_WRITE);
            svec_me.set_name(block_name + " successors vector");
            map->insert(svec_me);

            /* What is the "return" address. Eventually the successors program will execute a "RET" instruction that will
             * return to this address.  We can choose something arbitrary as long as it doesn't conflict with anything else.
             * We'll use the first byte past the end of the successor program, which gives the added benefit that the
             * successor program doesn't actually have to even return -- it can just fall off the end. */
            rose_addr_t return_va = text_va + bconf->sucs_program.size();
            if (debug) {
                fprintf(stderr, "    memory map after program is loaded:\n");
                map->dump(stderr, "      ");
            }

            /* Push arguments onto the stack in reverse order. */
            if (debug) fprintf(stderr, "  setting up the call frame...\n");

            /* old stack pointer */
            stack_ptr -= 4;
            policy.writeMemory<32>(x86_segreg_ss, policy.number<32>(stack_ptr),
                                   policy.readGPR(x86_gpr_sp), policy.true_());

            /* address past the basic block's last instruction */
            stack_ptr -= 4;
            policy.writeMemory<32>(x86_segreg_ss, policy.number<32>(stack_ptr),
                                   policy.number<32>(bb->insns.back()->get_address()+bb->insns.back()->get_raw_bytes().size()),
                                   policy.true_());

            /* address of basic block's first instruction */
            stack_ptr -= 4;
            policy.writeMemory<32>(x86_segreg_ss, policy.number<32>(stack_ptr),
                                   policy.number<32>(bb->insns.front()->get_address()), policy.true_());

            /* size of svec in bytes */
            stack_ptr -= 4;
            policy.writeMemory<32>(x86_segreg_ss, policy.number<32>(stack_ptr),
                                   policy.number<32>(svec_size), policy.true_());

            /* address of svec */
            stack_ptr -= 4;
            policy.writeMemory<32>(x86_segreg_ss, policy.number<32>(stack_ptr),
                                   policy.number<32>(svec_va), policy.true_());

            /* return address for successors program */
            stack_ptr -= 4;
            policy.writeMemory<32>(x86_segreg_ss, policy.number<32>(stack_ptr),
                                   policy.number<32>(return_va), policy.true_());

            /* Adjust policy stack pointer */
            policy.writeGPR(x86_gpr_sp, policy.number<32>(stack_ptr));

            /* Interpret the program */
            if (debug) fprintf(stderr, "  running the program...\n");
            Disassembler *disassembler = Disassembler::lookup(new SgAsmPEFileHeader(new SgAsmGenericFile()));
            ROSE_ASSERT(disassembler!=NULL);
            policy.writeIP(policy.number<32>(text_va));
            while (1) {
                rose_addr_t ip = policy.readIP().known_value();
                if (ip==return_va) break;
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(disassembler->disassembleOne(map, ip));
                if (debug) fprintf(stderr, "    0x%08"PRIx64": %s\n", ip, insn?unparseInstruction(insn).c_str():"<null>");
                ROSE_ASSERT(insn!=NULL);
                semantics.processInstruction(insn);
                ROSE_ASSERT(policy.readIP().is_known());
                SageInterface::deleteAST(insn);
            }

            /* Extract the list of successors. The number of successors is the first element of the list. */
            if (debug) fprintf(stderr, "  extracting program return values...\n");
            ValueType<32> nsucs = policy.readMemory<32>(x86_segreg_ss, policy.number<32>(svec_va), policy.true_());
            ROSE_ASSERT(nsucs.is_known());
            if (debug) fprintf(stderr, "    number of successors: %"PRId64"\n", nsucs.known_value());
            ROSE_ASSERT(nsucs.known_value()*4 <= svec_size-4); /*first entry is size*/
            for (size_t i=0; i<nsucs.known_value(); i++) {
                ValueType<32> suc_va = policy.readMemory<32>(x86_segreg_ss, policy.number<32>(svec_va+4+i*4), policy.true_());
                if (suc_va.is_known()) {
                    if (debug) fprintf(stderr, "    #%zu: 0x%08"PRIx64"\n", i, suc_va.known_value());
                    bb->cache.sucs.insert(suc_va.known_value());
                } else {
                    if (debug) fprintf(stderr, "    #%zu: unknown\n", i);
                    bb->cache.sucs_complete = false;
                }
            }

            /* Unmap the program */
            if (debug) fprintf(stderr, "  unmapping the program...\n");
            map->erase(text_me);
            map->erase(stack_me);
            map->erase(svec_me);

            if (debug) fprintf(stderr, "  done.\n");
        }
    }
}

/* Marks program entry addresses as functions. */
void
Partitioner::mark_entry_targets(SgAsmGenericHeader *fhdr)
{
    SgRVAList entries = fhdr->get_entry_rvas();
    for (size_t i=0; i<entries.size(); i++) {
        rose_addr_t entry_va = entries[i].get_rva() + fhdr->get_base_va();
        if (find_instruction(entry_va))
            add_function(entry_va, SgAsmFunctionDeclaration::FUNC_ENTRY_POINT);
    }
}

/* Use the Frame Descriptor Entry Records of the ELF .eh_frame section to mark functions. */
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
                    if (find_instruction(target))
                        add_function(target, SgAsmFunctionDeclaration::FUNC_EH_FRAME);
                }
            }
        }
    }
}

/* Adds each entry of the ELF procedure lookup table (.plt section) to the list of functions. */
void
Partitioner::mark_elf_plt_entries(SgAsmGenericHeader *fhdr)
{
    /* This function is ELF, x86 specific. */
    SgAsmElfFileHeader *elf = isSgAsmElfFileHeader(fhdr);
    if (!elf) return;
    
    /* Find important sections */
    SgAsmGenericSection *plt = elf->get_section_by_name(".plt");
    if (!plt || !plt->is_mapped()) return;
    SgAsmGenericSection *gotplt = elf->get_section_by_name(".got.plt");
    if (!gotplt || !gotplt->is_mapped()) return;

    /* Find all relocation sections */
    std::set<SgAsmElfRelocSection*> rsects;
    const SgAsmGenericSectionPtrList &sections = elf->get_sections()->get_sections();
    for (SgAsmGenericSectionPtrList::const_iterator si=sections.begin(); si!=sections.end(); ++si) {
        SgAsmElfRelocSection *reloc_section = isSgAsmElfRelocSection(*si);
        if (reloc_section) {
            ROSE_ASSERT(isSgAsmElfSymbolSection(reloc_section->get_linked_section()));
            rsects.insert(reloc_section);
        }
    }
    if (rsects.empty()) return;

    /* Look at each instruction in the .plt section. If the instruction is a computed jump to an address stored in the
     * .got.plt then we've found the beginning of a plt trampoline. */
    rose_addr_t plt_offset = 14; /* skip the first entry (PUSH ds:XXX; JMP ds:YYY; 0x00; 0x00)--the JMP is not a function*/
    while (plt_offset<plt->get_mapped_size()) {

        /* Find an x86 instruction */
        SgAsmInstruction *insn = find_instruction(plt->get_mapped_actual_va()+plt_offset);
        if (!insn) {
            ++plt_offset;
            continue;
        }
        plt_offset += insn->get_raw_bytes().size();
        SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(insn);
        if (!insn_x86) continue;
            
        rose_addr_t gotplt_va = get_indirection_addr(insn_x86);
        if (gotplt_va <  elf->get_base_va() + gotplt->get_mapped_preferred_rva() ||
            gotplt_va >= elf->get_base_va() + gotplt->get_mapped_preferred_rva() + gotplt->get_mapped_size()) {
            continue; /* jump is not indirect through the .got.plt section */
        }
        

        /* Find the relocation entry whose offset is the gotplt_va and use that entry's symbol for the function name. */
        std::string name;
        for (std::set<SgAsmElfRelocSection*>::iterator ri=rsects.begin(); ri!=rsects.end() && name.empty(); ++ri) {
            SgAsmElfRelocEntryList *entries = (*ri)->get_entries();
            SgAsmElfSymbolSection *symbol_section = isSgAsmElfSymbolSection((*ri)->get_linked_section());
            SgAsmElfSymbolList *symbols = symbol_section->get_symbols();
            for (size_t ei=0; ei<entries->get_entries().size() && name.empty(); ++ei) {
                SgAsmElfRelocEntry *rel = entries->get_entries()[ei];
                if (rel->get_r_offset()==gotplt_va) {
                    unsigned long symbol_idx = rel->get_sym();
                    if (symbol_idx < symbols->get_symbols().size()) {
                        SgAsmElfSymbol *symbol = symbols->get_symbols()[symbol_idx];
                        name = symbol->get_name()->get_string() + "@plt";
                    }
                }
            }
        }
        
        Function *plt_func = add_function(insn->get_address(), SgAsmFunctionDeclaration::FUNC_IMPORT, name);

        /* FIXME: Assume that most PLT functions return. We make this assumption for now because the PLT table contains an
         *        indirect jump through the .plt.got data area and we don't yet do static analysis of the data.  Because of
         *        that, all the PLT functons will contain only a basic block with the single indirect jump, and no return
         *        (e.g., x86 RET or RETF) instruction, and therefore the function would not normally be marked as returning.
         *        [RPM 2010-05-11] */
        if ("abort@plt"!=name && "execl@plt"!=name && "execlp@plt"!=name && "execv@plt"!=name && "execvp@plt"!=name &&
            "exit@plt"!=name && "_exit@plt"!=name && "fexecve@plt"!=name &&
            "longjmp@plt"!=name && "__longjmp@plt"!=name && "siglongjmp@plt"!=name)
            plt_func->returns = true;
    }
}

/* Use symbol tables to determine function entry points. */
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
            if (symbol->get_def_state()==SgAsmGenericSymbol::SYM_DEFINED &&
                symbol->get_type()==SgAsmGenericSymbol::SYM_FUNC &&
                symbol->get_value()!=0) {
                rose_addr_t value = symbol->get_value();
                if (find_instruction(value))
                    add_function(value, SgAsmFunctionDeclaration::FUNC_SYMBOL, symbol->get_name()->get_string());

                /* Sometimes weak symbol values are offsets from a section (this code handles that), but other times they're
                 * the value is used directly (the above code handled that case). */            
                SgAsmGenericSection *section = symbol->get_bound();
                if (section && symbol->get_binding()==SgAsmGenericSymbol::SYM_WEAK)
                    value += section->get_mapped_actual_va();
                if (find_instruction(value))
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
    if (rre->get_descriptor().get_major()!=x86_regclass_gpr || rre->get_descriptor().get_minor()!=x86_gpr_bp) return insns.end();

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
    if (rre->get_descriptor().get_major()!=x86_regclass_gpr || rre->get_descriptor().get_minor()!=x86_gpr_bp) return insns.end();
    rre = isSgAsmx86RegisterReferenceExpression(opands2[1]);
    if (!rre) return insns.end();
    if (rre->get_descriptor().get_major()!=x86_regclass_gpr || rre->get_descriptor().get_minor()!=x86_gpr_sp) return insns.end();

    return ii;
}

#if 0 /*commented out in Partitioner::mark_func_patterns()*/
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
#endif

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
        
/** Seeds functions according to instruction patterns.  Note that this pattern matcher only looks at existing instructions--it
 *  does not actively disassemble new instructions.  In other words, this matcher is intended mostly for passive-mode
 *  partitioners where the disassembler has already disassembled everything it can. */
void
Partitioner::mark_func_patterns()
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

/* Make all CALL/FARCALL targets functions.  This is a naive approach that won't work for some obfuscated software. A more
 * thorough approach considers only those calls that are reachable. */
void
Partitioner::mark_call_insns()
{
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        std::vector<SgAsmInstruction*> iv;
        iv.push_back(ii->second);
        rose_addr_t target_va=NO_TARGET;
        if (ii->second->is_function_call(iv, &target_va) && target_va!=NO_TARGET)
            add_function(target_va, SgAsmFunctionDeclaration::FUNC_CALL_TARGET, "");
    }
}

/* Look for NOP padding between functions */
void
Partitioner::create_nop_padding()
{

    /* Find no-op blocks that follow known functions and which are not already part of a function */
    Disassembler::AddressSet new_functions;
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); fi++) {
        Function *func = fi->second;
        if (func->reason & SgAsmFunctionDeclaration::FUNC_INTERPAD) continue; /*some other kind of padding*/
        if (0==func->blocks.size()) continue;
        SgAsmInstruction *last_insn = func->last_block()->last_insn();
        rose_addr_t va = last_insn->get_address() + last_insn->get_raw_bytes().size();
        BasicBlock *bb = find_bb_containing(va);
        if (!bb || bb->function!=NULL) continue;

        /* FIXME: We should have a more portable way to find blocks that are no-ops. */
        bool is_noop=true;
        for (size_t i=0; i<bb->insns.size() && is_noop; i++) {
            SgAsmx86Instruction     *insn_x86 = isSgAsmx86Instruction(bb->insns[i]);
            SgAsmArmInstruction     *insn_arm = isSgAsmArmInstruction(bb->insns[i]);
            SgAsmPowerpcInstruction *insn_ppc = isSgAsmPowerpcInstruction(bb->insns[i]);
            ROSE_ASSERT(insn_x86 || insn_arm || insn_ppc);
            is_noop = (insn_x86 && insn_x86->get_kind()==x86_nop); /*only x86 has NOPs*/
        }
        if (!is_noop) continue;
        new_functions.insert(va);
    }
    
    /* Create functions */
    for (Disassembler::AddressSet::iterator ai=new_functions.begin(); ai!=new_functions.end(); ++ai) {
        Function *padfunc = add_function(*ai, SgAsmFunctionDeclaration::FUNC_INTERPAD);
        BasicBlock *bb = find_bb_containing(*ai);
        ROSE_ASSERT(bb!=NULL);
        append(padfunc, bb);
    }
}

/** Creates functions to hold zero padding. This is done by looking at existing instructions only -- not disassembling any new
 *  instructions. */
void
Partitioner::create_zero_padding()
{
    /* Find the address ranges that contain zero padding, irrespective of basic blocks. */
    std::map<rose_addr_t/*begin_va*/, rose_addr_t/*end_va*/> new_functions;
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); fi++) {
        Function *left_func = fi->second;
        if (left_func->reason & SgAsmFunctionDeclaration::FUNC_INTERPAD) continue; /*some other kind of padding*/
        if (0==left_func->blocks.size()) continue;
        SgAsmInstruction *insn = left_func->last_block()->last_insn();
        rose_addr_t begin_va = insn->get_address() + insn->get_raw_bytes().size();
        
        rose_addr_t end_va = (rose_addr_t)-1;
        Functions::iterator fi2 = fi;
        ++fi2;
        if (fi2!=functions.end())
            end_va = fi2->first;

        /* Are all bytes between begin_va (inclusive) and end_va (exclusive) zero? */
        bool all_zero = true;
        for (rose_addr_t va=begin_va; va<end_va && all_zero; /*void*/) {
            SgAsmInstruction *i2 = find_instruction(va, false); /*do not disassemble more*/
            if (NULL==i2) {
                end_va = va;
                break;
            }
            size_t nbytes = std::min((rose_addr_t)i2->get_raw_bytes().size(), end_va-va);
            for (size_t i=0; i<nbytes && all_zero; i++)
                all_zero = (i2->get_raw_bytes()[i]==0);
            va += i2->get_raw_bytes().size();
        }
        if (all_zero && begin_va<end_va)
            new_functions[begin_va] = end_va;
    }
    
    /* For ranges of zero bytes, find the blocks containing those instructions and add them to the padding function. The
     * blocks are allowed to extend beyond the range of zeros and contain non-zero bytes. */
    for (std::map<rose_addr_t, rose_addr_t>::iterator fi=new_functions.begin(); fi!=new_functions.end(); ++fi) {
        rose_addr_t begin_va = fi->first;
        rose_addr_t end_va = fi->second;
        Function *padfunc = NULL;
        for (rose_addr_t va=begin_va; va<end_va; /*void*/) {
            BasicBlock *bb = find_bb_containing(va);
            if (!bb || bb->function) break;
            if (!padfunc)
                padfunc = add_function(begin_va, SgAsmFunctionDeclaration::FUNC_INTERPAD);
            append(padfunc, bb);
            SgAsmInstruction *last_insn = bb->last_insn();
            va = last_insn->get_address() + last_insn->get_raw_bytes().size();
        }
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
        if (reg->get_descriptor().get_major()==x86_regclass_ip && val!=NULL) {
            retval = value_of(val) + insn->get_address() + insn->get_raw_bytes().size();
        }
    } else if (isSgAsmValueExpression(mref_addr)) {
        retval = value_of(isSgAsmValueExpression(mref_addr));
    }

    return retval; /*calculated value, or defaults to zero*/
}

/* Gives names to the dynamic linking trampolines in the .plt section if the Partitioner detected them as functions. If
 * mark_elf_plt_entries() was called then they all would have been marked as functions and given names. Otherwise, ROSE might
 * have detected some of them in other ways (like CFG analysis) and this function will give them names. */
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

        if (func_addr <  elf->get_base_va() + plt->get_mapped_preferred_rva() ||
            func_addr >= elf->get_base_va() + plt->get_mapped_preferred_rva() + plt->get_mapped_size())
            continue; /* function is not in the .plt section */

        /* Sometimes the first instruction of a basic block cannot be disassembled and the basic block will have a different
         * starting address than its first instruction.  If that basic block is also the start of a function then the
         * function also will have no initial instruction. */
        SgAsmInstruction *insn = find_instruction(func_addr);
        if (NULL==insn)
            continue;

        /* The target in the ".plt" section will be an indirect (through the .got.plt section) jump to the actual dynamically
         * linked function (or to the dynamic linker itself). The .got.plt address is what we're really interested in. */
        SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(insn);
        ROSE_ASSERT(insn_x86!=NULL);
        rose_addr_t gotplt_va = get_indirection_addr(insn_x86);

        if (gotplt_va <  elf->get_base_va() + gotplt->get_mapped_preferred_rva() ||
            gotplt_va >= elf->get_base_va() + gotplt->get_mapped_preferred_rva() + gotplt->get_mapped_size())
            continue; /* PLT entry doesn't dereference a value in the .got.plt section */
        
        /* Find the relocation entry whose offset is the gotplt_rva and use that entry's symbol for the function name. */
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
                    fi->second->name = symbol->get_name()->get_string() + "@plt";
                }
            }
        }
    }
}

/* Seed function starts based on criteria other than control flow graph. */
void
Partitioner::pre_cfg(SgAsmInterpretation *interp/*=NULL*/)
{
    mark_ipd_configuration();   /*seed partitioner based on IPD configuration information*/

    if (interp) {
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        for (size_t i=0; i<headers.size(); i++) {
            if (func_heuristics & SgAsmFunctionDeclaration::FUNC_ENTRY_POINT)
                mark_entry_targets(headers[i]);
            if (func_heuristics & SgAsmFunctionDeclaration::FUNC_EH_FRAME)
                mark_eh_frames(headers[i]);
            if (func_heuristics & SgAsmFunctionDeclaration::FUNC_SYMBOL)
                mark_func_symbols(headers[i]);
            if (func_heuristics & SgAsmFunctionDeclaration::FUNC_IMPORT)
                mark_elf_plt_entries(headers[i]);
        }
    }
    if (func_heuristics & SgAsmFunctionDeclaration::FUNC_PATTERN)
        mark_func_patterns();
    if (func_heuristics & SgAsmFunctionDeclaration::FUNC_CALL_INSN)
        mark_call_insns();

    /* Run user-defined function detectors, making sure that the basic block starts are up-to-date for each call. */
    if (func_heuristics & SgAsmFunctionDeclaration::FUNC_USERDEF) {
        if (interp) {
            const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
            for (size_t i=0; i<user_detectors.size(); i++) {
                for (size_t j=0; j<=headers.size(); j++) {
                    SgAsmGenericHeader *hdr = 0==j ? NULL : headers[j-1];
                    user_detectors[i](this, hdr);
                }
            }
        } else {
            for (size_t i=0; i<user_detectors.size(); i++) {
                user_detectors[i](this, NULL);
            }
        }
    }
}

/** Adds first basic block to empty function before we start discovering blocks of any other functions. This
 *  protects against cases where one function simply falls through to another within a basic block, such as:
 *   08048460 <foo>:
 *    8048460:       55                      push   ebp
 *    8048461:       89 e5                   mov    ebp,esp
 *    8048463:       83 ec 08                sub    esp,0x8
 *    8048466:       c7 04 24 d4 85 04 08    mov    DWORD PTR [esp],0x80485d4
 *    804846d:       e8 8e fe ff ff          call   8048300 <puts@plt>
 *    8048472:       c7 04 24 00 00 00 00    mov    DWORD PTR [esp],0x0
 *    8048479:       e8 a2 fe ff ff          call   8048320 <_exit@plt>
 *    804847e:       89 f6                   mov    esi,esi
 *   
 *   08048480 <handler>:
 *    8048480:       55                      push   ebp
 *    8048481:       89 e5                   mov    ebp,esp
 *    8048483:       83 ec 08                sub    esp,0x8
 */
void
Partitioner::discover_first_block(Function *func) 
{
    if (debug) {
        fprintf(debug, "1st block %s F%08"PRIx64" \"%s\": B%08"PRIx64" ",
                SgAsmFunctionDeclaration::reason_str(true, func->reason).c_str(),
                func->entry_va, func->name.c_str(), func->entry_va);
    }
    BasicBlock *bb = find_bb_containing(func->entry_va);

    /* If this function's entry block collides with some other function, then truncate that other function's block and
     * subsume part of it into this function. Mark the other function as pending because its block may have new
     * successors now. */
    if (bb && func->entry_va!=address(bb)) {
        ROSE_ASSERT(bb->function!=func);
        if (debug) fprintf(debug, "[split from B%08"PRIx64, address(bb));
        if (bb->function) {
            if (debug) fprintf(debug, " in F%08"PRIx64" \"%s\"", address(bb), bb->function->name.c_str());
            bb->function->pending = true;
        }
        if (debug) fprintf(debug, "] ");
        truncate(bb, func->entry_va);
        bb = find_bb_containing(func->entry_va);
        ROSE_ASSERT(bb!=NULL);
        ROSE_ASSERT(func->entry_va==address(bb));
    }

    if (bb) {
        append(func, bb);
        if (debug) fprintf(debug, "added %zu instruction%s\n", bb->insns.size(), 1==bb->insns.size()?"":"s");
    } else if (debug) {
        fprintf(debug, "no instruction at function entry address\n");
    }
}

/** Discover the basic blocks that belong to the current function. This function recursively adds basic blocks to function @p f
 *  by following the successors of each block.  If a successor is an instruction belonging to some other
 *  function then it's either a function call (if it branches to the entry point of that function) or it's a collision.
 *  Collisions are resolved by discarding and rediscovering the blocks of the other function. */
void
Partitioner::discover_blocks(Function *f, rose_addr_t va)
{
    if (debug) fprintf(debug, " B%08"PRIx64, va);
    SgAsmInstruction *insn = find_instruction(va);
    if (!insn) return; /* No instruction at this address. */
    rose_addr_t target_va = NO_TARGET; /*target of function call instructions (e.g., x86 CALL and FARCALL)*/

    /* This block might be the entry address of a function even before that function has any basic blocks assigned to it. This
     * can happen when a new function was discovered during the current pass. It can't happen for functions discovered in a
     * previous pass since we would have called discover_first_block() by now for any such functions. */
    Functions::iterator fi = functions.find(va);
    if (fi!=functions.end() && fi->second!=f) {
        if (debug) fprintf(debug, "[entry \"%s\"]", fi->second->name.c_str());
        return;
    }

    /* Find basic block at address, creating it if necessary. */
    BasicBlock *bb = find_bb_starting(va);
    ROSE_ASSERT(bb!=NULL);

    /* If the current function has been somehow marked as pending then we might as well give up discovering its blocks because
     * some of its blocks' successors may have changed.  This can happen, for instance, if the create_bb() called above had to
     * split one of this function's blocks. */
    if (f->pending) {
        if (debug) fprintf(debug, " abandon");
        throw AbandonFunctionDiscovery();
    }

    /* Don't reprocess blocks for this function. However, we need to reprocess the first block because it was added by
     * discover_first_block(), which is not recursive.  Care should be taken so none of the recursive calls below are invoked
     * for the first block, or we'll have infinite recurision! */
    if (bb->function==f && address(bb)!=f->entry_va)
        return;

    if (bb->function && bb->function!=f) {
        if (va==bb->function->entry_va) {
            /* This is a call to some other existing function. Do not add it to the current function. */
            if (debug) fprintf(debug, "[entry \"%s\"]", bb->function->name.c_str());
        } else {
            /* This block belongs internally to some other function. Since ROSE requires that blocks be owned by exactly one
             * function (the function/block relationship is an edge in the abstract syntax tree), we have to remove this block
             * from the other function.  We'll mark both the other function and this function as being in conflict and try
             * again later.
             *
             * However, there is a special case we need to watch out for: the case when the block in conflict is no longer
             * reachable from the original function due to having made changes to other blocks in the original function. For
             * instance, consider the following sequence of events:
             *     F000 contains B000 (the entry block) and B010
             *          B000 has 10 instructions, and ends with a call to F100 which returns
             *          B010 is the fall-through address of B000
             * We then begin to discover F005 whose entry address is the fifth instruction of B000, so
             *     B000 is split into B000 containing the first five instrucitons and B005 containing the second five
             *     F000 is marked as pending due to the splitting of its B000 block
             *     B005 is added to F005 as its entry block
             *     B005 calls F100 which returns to B010, so we want to add B010 to F005
             * So we have a conflict:
             *     B010 belongs to F000 because we never removed it, but we need B010 also in F005.
             * In this example, the only CFG edge to B010 inside F000 was the fall-through edge from the call to F100, which
             * no longer exists in F000. Unfortunately we have no way of knowing (short of doing a CFG analysis in F000) that
             * the last edge was removed. Even if we did a CFG analysis, we may be working with incomplete information (F000
             * might not be fully discovered yet).
             *
             * The way we handle this special case is as follows:
             *     If the original function (F000 in the example) is marked as pending then the blocks it currently owns might
             *     not actually belong to the function anymore. Therefore we will not create a new FUNC_GRAPH function for the
             *     block in conflict, but rather mark both functions as pending and abandon until the next pass.  Otherwise we
             *     assume the block in conflict really is in conflict and we'll create a FUNC_GRAPH function. */
            if (functions.find(va)==functions.end() && !bb->function->pending) {
                add_function(va, SgAsmFunctionDeclaration::FUNC_GRAPH);
                if (debug) fprintf(debug, "[conflict F%08"PRIx64" \"%s\"]", bb->function->entry_va, bb->function->name.c_str());
            } else if (debug) {
                fprintf(debug, "[possible conflict F%08"PRIx64" \"%s\"]", bb->function->entry_va, bb->function->name.c_str());
            }
            bb->function->pending = f->pending = true;
            if (debug) fprintf(debug, " abandon");
            throw AbandonFunctionDiscovery();
        }
    } else if ((target_va=call_target(bb))!=NO_TARGET) {
        /* Call to a known target */
        if (debug) fprintf(debug, "[call F%08"PRIx64"]", target_va);

        append(f, bb);
        BasicBlock *target_bb = find_bb_containing(target_va);

        /* Optionally create or add reason flags to called function. */
        Function *new_function = NULL;
        if ((func_heuristics & SgAsmFunctionDeclaration::FUNC_CALL_TARGET)) {
            new_function = add_function(target_va, SgAsmFunctionDeclaration::FUNC_CALL_TARGET);
        } else if (find_function(target_va)!=NULL) {
            find_function(target_va)->reason |= SgAsmFunctionDeclaration::FUNC_CALL_TARGET;
        }

        /* If the call target is part of a function (the current function or some other) and it's not the entry block then we
         * might need to rediscover the blocks of that function.   We don't need to rediscover the blocks of that function if
         * that function is the current function and should remain in the current function (i.e., we didn't create a new
         * function). */
        if (target_bb && target_bb->function && target_va!=target_bb->function->entry_va &&
            (target_bb->function!=f || new_function!=NULL))
            target_bb->function->pending = true;
        
        /* Discovery continues at the successors. */
        const Disassembler::AddressSet &suc = successors(bb);
        for (Disassembler::AddressSet::const_iterator si=suc.begin(); si!=suc.end(); ++si) {
            if (*si!=f->entry_va)
                discover_blocks(f, *si);
        }

    } else {
        append(f, bb);
        const Disassembler::AddressSet& suc = successors(bb);
        for (Disassembler::AddressSet::const_iterator si=suc.begin(); si!=suc.end(); ++si) {
            if (*si!=f->entry_va)
                discover_blocks(f, *si);
        }
    }
}

void
Partitioner::analyze_cfg()
{
    for (size_t pass=1; true; pass++) {
        if (debug) fprintf(debug, "========== Partitioner::analyze_cfg() pass %zu ==========\n", pass);
        progress(debug, "Partitioner: starting pass %zu: %zu function%s, %zu insn%s assigned to %zu block%s (ave %d insn/blk)\n",
                 pass, functions.size(), 1==functions.size()?"":"s", insn2block.size(), 1==insn2block.size()?"":"s", 
                 blocks.size(), 1==blocks.size()?"":"s",
                 blocks.size()?(int)(1.0*insn2block.size()/blocks.size()+0.5):0);

        /* If function A makes a call to function B and we know that function B could return but the return address is not
         * part of any function, then mark function A as pending so that we rediscover its blocks. */   
        for (BasicBlocks::iterator bi=blocks.begin(); bi!=blocks.end(); ++bi) {
            BasicBlock *bb = bi->second;
            rose_addr_t return_va = canonic_block(bb->last_insn()->get_address() + bb->last_insn()->get_raw_bytes().size());
            BasicBlock *return_bb = find_bb_starting(return_va);
            rose_addr_t target_va = NO_TARGET; /*call target*/
            BasicBlock *target_bb = NULL;
#if 0 /*DEBUG [RPM 2010-07-30]*/
            do {
                static rose_addr_t req_call_block_va   = 0x08048364;
                static rose_addr_t req_target_va       = 0x0804836e;
                static rose_addr_t req_return_va       = 0x0804836e;

                if (address(bb)!=req_call_block_va) break;
                fprintf(stderr, "in pass %zu found block 0x%08"PRIx64"\n", pass, req_call_block_va);
                fprintf(stderr, "  belongs to function? %s\n", bb->function?"yes":"no");
                if (!bb->function) break;
                fprintf(stderr, "    that function is 0x%08"PRIx64"\n", bb->function->entry_va);
                bool is_call = is_function_call(bb, &target_va);
                fprintf(stderr, "  is a function call? %s\n", is_call?"yes":"no");
                if (!is_call) break;
                fprintf(stderr, "    target va is 0x%08"PRIx64"\n", target_va);
                if (target_va!=req_target_va) break;
                target_bb = find_bb_starting(target_va, false);
                fprintf(stderr, "  target block exists? %s\n", target_bb?"yes":"no");
                if (!target_bb) break;
                fprintf(stderr, "  target block belongs to a function? %s\n", target_bb->function?"yes":"no");
                if (!target_bb->function) break;
                fprintf(stderr, "    that function is 0x%08"PRIx64"\n", target_bb->function->entry_va);
                if (target_bb->function->entry_va!=req_target_va) break;
                fprintf(stderr, "  target function returns? %s\n", target_bb->function->returns?"yes":"no");
                if (!target_bb->function->returns) break;
                fprintf(stderr, "  return address is 0x%08"PRIx64"\n", return_va);
                if (return_va!=req_return_va) break;
                fprintf(stderr, "  return block exists? %s\n", return_bb?"yes":"no");
                if (!return_bb) break;
                fprintf(stderr, "  return block in a function? %s\n", return_bb->function?"yes":"no");
                if (!return_bb->function) {
                    fprintf(stderr, "  marking function 0x%08"PRIx64" as pending!\n", bb->function->entry_va);
                } else if (return_bb->function->entry_va==return_va) {
                    fprintf(stderr, "    returns to fall-through address\n");
                    if (!bb->function->returns) {
                        fprintf(stderr, "    marking calling function as returning\n");
                        fprintf(stderr, "    NEED ANOTHER PASS!\n");
                        /* Track the parent now */
                        req_call_block_va       = 0x080482c8;
                        req_target_va           = 0x08048364;
                        req_return_va           = 0x080482d3;

                    }
                }
            } while (0);
#endif
            if (bb->function && return_bb &&
                is_function_call(bb, &target_va) && target_va!=NO_TARGET &&
                NULL!=(target_bb=find_bb_starting(target_va, false)) &&
                target_bb->function && target_bb->function->returns) {
                if (!return_bb->function) {
                    bb->function->pending = true;
                    if (debug) {
                        fprintf(debug, "  F%08"PRIx64" returns to B%08"PRIx64" in F%08"PRIx64"\n", 
                                target_bb->function->entry_va, return_va, bb->function->entry_va);
                    }
                } else if (return_va==target_bb->function->entry_va && !bb->function->returns) {
                    /* This handles the case when function A's return from B falls through into B. In this case, since B
                     * returns then A also returns.  We mark A as returning and we'll catch A's callers on the next pass.
                     *    function_A:
                     *        ...
                     *        CALL function_B
                     *    function_B:
                     *        RET
                     */
                    bb->function->returns = true;
                    if (debug) {
                        fprintf(debug, "  Function F%08"PRIx64" returns by virtue of call fall-through at B%08"PRIx64"\n", 
                                bb->function->entry_va, address(bb));
                    }
                }
            }
        }

        /* Get a list of functions we need to analyze because they're marked as pending. */
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

        /* Make sure all functions have an initial basic block if possible. */
        for (size_t i=0; i<pending.size(); ++i)
            discover_first_block(pending[i]);
        
        /* (Re)discover each function's blocks starting with the function entry point */
        for (size_t i=0; i<pending.size(); ++i) {
            if (debug) {
                fprintf(debug, "analyzing %s F%08"PRIx64" \"%s\" pass %zu: ",
                        SgAsmFunctionDeclaration::reason_str(true, pending[i]->reason).c_str(),
                        pending[i]->entry_va, pending[i]->name.c_str(), pass);
            }
            try {
                discover_blocks(pending[i], pending[i]->entry_va);
            } catch (const AbandonFunctionDiscovery&) {
                /* thrown when discover_blocks() decides it needs to start over on a function */
            }
            if (debug) fprintf(debug, "\n");
        }
    }
        
    progress(debug,
             "Partitioner completed: %zu function%s, %zu insn%s assigned to %zu block%s (ave %d insn/blk)\n",
             functions.size(), 1==functions.size()?"":"s", insn2block.size(), 1==insn2block.size()?"":"s", 
             blocks.size(), 1==blocks.size()?"":"s",
             blocks.size()?(int)(1.0*insn2block.size()/blocks.size()+0.5):0);
}

void
Partitioner::post_cfg(SgAsmInterpretation *interp/*=NULL*/)
{
    if (func_heuristics & SgAsmFunctionDeclaration::FUNC_INTERPAD) {
        create_nop_padding();
        create_zero_padding();
    }

    if (interp) {
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        /* This doesn't detect new functions, it just gives names to ELF .plt trampolines */
        for (size_t i=0; i<headers.size(); i++) {
            name_plt_entries(headers[i]);
        }
    }
}

SgAsmBlock *
Partitioner::build_ast()
{
    /* Build a function to hold all the unassigned instructions.  Update documentation if changing the name of
     * this generated function! */
    Function *catchall = NULL;
    if ((func_heuristics & SgAsmFunctionDeclaration::FUNC_LEFTOVERS)) {
        for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
            BasicBlock *bb = find_bb_containing(ii->first);
            ROSE_ASSERT(bb!=NULL);
            if (!bb->function) {
                if (!catchall)
                    catchall = add_function(ii->first, SgAsmFunctionDeclaration::FUNC_LEFTOVERS, "***uncategorized blocks***");
                append(catchall, bb);
            }
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
Partitioner::build_ast(Function* f)
{
    if (f->blocks.size()==0) {
        if (debug) fprintf(debug, "function F%08"PRIx64" \"%s\" has no basic blocks!\n", f->entry_va, f->name.c_str());
        return NULL;
    }
    
    SgAsmFunctionDeclaration *retval = new SgAsmFunctionDeclaration;
    rose_addr_t next_block_va = f->entry_va;
    unsigned reasons = f->reason;
    
    for (BasicBlocks::iterator bi=f->blocks.begin(); bi!=f->blocks.end(); ++bi) {
        BasicBlock *bb = bi->second;
        if (address(bb)!=next_block_va)
            reasons |= SgAsmFunctionDeclaration::FUNC_DISCONT;
        SgAsmBlock *block = build_ast(bb);
        retval->get_statementList().push_back(block);
        block->set_parent(retval);
        SgAsmInstruction *last = bb->insns.back();
        next_block_va = last->get_address() + last->get_raw_bytes().size();

        /* The function is discontiguous if blocks do not follow one another or the instructions within a block are
         * discontiguous. The former was checked above; the latter we check here. */
        for (size_t i=1; 0==(reasons & SgAsmFunctionDeclaration::FUNC_DISCONT) && i<bb->insns.size(); ++i) {
            if (bb->insns[i-1]->get_address() + bb->insns[i-1]->get_raw_bytes().size() != bb->insns[i]->get_address()) {
                reasons |= SgAsmFunctionDeclaration::FUNC_DISCONT;
            }
        }
    }

    BasicBlock *first_block = f->blocks.begin()->second;
    retval->set_address(address(first_block));
    retval->set_name(f->name);
    retval->set_reason(reasons);
    retval->set_entry_va(f->entry_va);
    return retval;
}

SgAsmBlock *
Partitioner::build_ast(BasicBlock* bb)
{
    SgAsmBlock *retval = new SgAsmBlock;
    retval->set_id(bb->insns.front()->get_address());
    retval->set_address(bb->insns.front()->get_address());
    for (std::vector<SgAsmInstruction*>::const_iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
        retval->get_statementList().push_back(*ii);
        (*ii)->set_parent(retval);
    }

    /* Cache block successors so other layers don't have to constantly compute them */
    bool complete;
    Disassembler::AddressSet sucs = successors(bb, &complete);
    SgAddressList addrlist(sucs.begin(), sucs.end());
    retval->set_cached_successors(addrlist);
    retval->set_complete_successors(complete);

    return retval;
}

/* Top-level function to run the partitioner in passive mode. */
SgAsmBlock *
Partitioner::partition(SgAsmInterpretation* interp/*=NULL*/, const Disassembler::InstructionMap& insns)
{
    disassembler = NULL;
    add_instructions(insns);
    pre_cfg(interp);
    analyze_cfg();
    post_cfg(interp);
    return build_ast();
}

/* Top-level function to run the partitioner in active mode. */
SgAsmBlock *
Partitioner::partition(SgAsmInterpretation* interp/*=NULL*/, Disassembler *d, MemoryMap *m)
{
    ROSE_ASSERT(d!=NULL);
    disassembler = d;
    ROSE_ASSERT(m!=NULL);
    set_map(m);
    pre_cfg(interp);
    analyze_cfg();
    post_cfg(interp);
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
