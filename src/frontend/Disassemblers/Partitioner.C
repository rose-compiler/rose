/* Algorithms to detect what instructions make up basic blocks and which blocks make up functions, and how to create the
 * necessary SgAsmBlock and SgAsmFunction IR nodes from this information. */
#define __STDC_FORMAT_MACROS
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include <inttypes.h>

#include "Partitioner.h"
#include "Assembler.h"
#include "AssemblerX86.h"
#include "AsmUnparser_compat.h"
#include "VirtualMachineSemantics.h"
#include "stringify.h"

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>

/* See header file for full documentation. */


std::ostream& operator<<(std::ostream &o, const Partitioner::Exception &e)
{
    e.print(o);
    return o;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// These SgAsmFunction methods have no other home, so they're here for now. Do not move them into
// src/ROSETTA/Grammar/BinaryInstruction.code because then they can't be indexed by C-aware tools.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

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

/** Returns a multi-line string describing the letters used for function reasons.  The letters are returned by the padding
 *  version of reason_str(). */
std::string
SgAsmFunction::reason_key(const std::string &prefix)
{
    return (prefix + "E = entry address         H = CFG head             C = function call(*)\n" +
            prefix + "X = exception frame       T = thunk                I = imported/dyn-linked\n" +
            prefix + "S = function symbol       P = instruction pattern  G = CFG graph analysis\n" +
            prefix + "U = user-def detection    N = NOP/zero padding     D = discontiguous blocks\n" +
            prefix + "V = intra-function block  L = leftover blocks\n" +
            prefix + "Note: \"c\" means this is the target of a call-like instruction or instruction\n" +
            prefix + "      sequence but the call is not present in the global control flow graph, while\n" +
            prefix + "      \"C\" means the call is in the CFG.\n");
}

/** Returns reason string for this function. */
std::string
SgAsmFunction::reason_str(bool do_pad) const
{
    return reason_str(do_pad, get_reason());
}

/** Class method that converts a reason bit vector to a human-friendly string. The second argument is the bit vector of
 *  SgAsmFunction::FunctionReason bits. */
std::string
SgAsmFunction::reason_str(bool do_pad, unsigned r)
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

    if (r & FUNC_EH_FRAME) {
        add_to_reason_string(result, true,               do_pad, "X", "exception frame");
    } else {
        add_to_reason_string(result, (r & FUNC_THUNK),   do_pad, "T", "thunk");
    }
    add_to_reason_string(result, (r & FUNC_IMPORT),      do_pad, "I", "import");
    add_to_reason_string(result, (r & FUNC_SYMBOL),      do_pad, "S", "symbol");
    add_to_reason_string(result, (r & FUNC_PATTERN),     do_pad, "P", "pattern");
    add_to_reason_string(result, (r & FUNC_GRAPH),       do_pad, "G", "graph");
    add_to_reason_string(result, (r & FUNC_USERDEF),     do_pad, "U", "user defined");
    add_to_reason_string(result, (r & FUNC_INTERPAD),    do_pad, "N", "padding");
    add_to_reason_string(result, (r & FUNC_DISCONT),     do_pad, "D", "discontiguous");
    add_to_reason_string(result, (r & FUNC_LEFTOVERS),   do_pad, "L", "leftovers");
    add_to_reason_string(result, (r & FUNC_INTRABLOCK),  do_pad, "V", "intrablock");
    return result;
}

/** Returns information about the function addresses.  Every non-empty function has a minimum (inclusive) and maximum
 *  (exclusive) address which are returned by reference, but not all functions own all the bytes within that range of
 *  addresses. Therefore, the exact bytes are returned by adding them to the optional ExtentMap argument.  This function
 *  returns the number of nodes (instructions and static data items) in the function.  If the function contains no nodes then
 *  the extent map is not modified and the low and high addresses are both set to zero.
 *
 *  If an @p exclude functor is provided, then any node for which it returns true is not considered part of the function.  This
 *  can be used for such things as filtering out data blocks that are marked as padding.  For example:
 *
 *  @code
 *  class NotPadding: public SgAsmFunction::NodeSelector {
 *  public:
 *      virtual bool operator()(SgNode *node) {
 *          SgAsmStaticData *data = isSgAsmStaticData(node);
 *          SgAsmBlock *block = SageInterface::getEnclosingNode<SgAsmBlock>(data);
 *          return !data || !block || block->get_reason()!=SgAsmBlock::BLK_PADDING;
 *      }
 *  } notPadding;
 *
 *  ExtentMap extents;
 *  function->get_extent(&extents, NULL, NULL, &notPadding);
 *  @endcode
 *
 *  Here's another example that calculates the extent of only the padding data, based on the negation of the filter in the
 *  previous example:
 *
 *  @code
 *  class OnlyPadding: public NotPadding {
 *  public:
 *      virtual bool operator()(SgNode *node) {
 *          return !NotPadding::operator()(node);
 *      }
 *  } onlyPadding;
 *
 *  ExtentMap extents;
 *  function->get_extent(&extents, NULL, NULL, &onlyPadding);
 *  @endcode
 */
size_t
SgAsmFunction::get_extent(ExtentMap *extents, rose_addr_t *lo_addr, rose_addr_t *hi_addr, NodeSelector *selector)
{
    struct T1: public AstSimpleProcessing {
        ExtentMap *extents;
        rose_addr_t *lo_addr, *hi_addr;
        NodeSelector *selector;
        size_t nnodes;
        T1(ExtentMap *extents, rose_addr_t *lo_addr, rose_addr_t *hi_addr, NodeSelector *selector)
            : extents(extents), lo_addr(lo_addr), hi_addr(hi_addr), selector(selector), nnodes(0) {
            if (lo_addr)
                *lo_addr = 0;
            if (hi_addr)
                *hi_addr = 0;
        }
        void visit(SgNode *node) {
            if (selector && !(*selector)(node))
                return;
            SgAsmInstruction *insn = isSgAsmInstruction(node);
            SgAsmStaticData *data = isSgAsmStaticData(node);
            rose_addr_t lo, hi;
            if (insn) {
                lo = insn->get_address();
                hi = lo + insn->get_size();
            } else if (data) {
                lo = data->get_address();
                hi = lo + data->get_size();
            } else {
                return;
            }

            if (0==nnodes++) {
                if (lo_addr)
                    *lo_addr = lo;
                if (hi_addr)
                    *hi_addr = hi;
            } else {
                if (lo_addr)
                    *lo_addr = std::min(*lo_addr, lo);
                if (hi_addr)
                    *hi_addr = std::max(*hi_addr, hi);
            }
            if (extents && hi>lo)
                extents->insert(Extent(lo, hi-lo));
        }
    } t1(extents, lo_addr, hi_addr, selector);
    t1.traverse(this, preorder);
    return t1.nnodes;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// These SgAsmBlock methods have no other home, so they're here for now. Do not move them into
// src/ROSETTA/Grammar/BinaryInstruction.code because then they can't be indexed by C-aware tools.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Returns a multi-line string describing the letters used for basic block reasons.  The letters are returned by the padding
 *  version of reason_str(). */
std::string
SgAsmBlock::reason_key(const std::string &prefix)
{
    return (prefix + "L = left over blocks    N = NOP/zero padding     V = intrafunction block\n" +
            prefix + "J = jump table          E = Function entry\n" +
            prefix + "H = CFG head            1 = first CFG traversal 2 = second CFG traversal\n" +
            prefix + "U = user-def reason\n");
}

/** Returns reason string for this block. */
std::string
SgAsmBlock::reason_str(bool do_pad) const
{
    return reason_str(do_pad, get_reason());
}

/** Class method that converts a reason bit vector to a human-friendly string. The second argument is the bit vector of
 *  SgAsmBlock::Reason bits.  Some of the positions in the padded return value are used for more than one bit.  For instance,
 *  the first character can be "L" for leftovers, "N" for padding, "E" for entry point, or "-" for none of the above. */
std::string
SgAsmBlock::reason_str(bool do_pad, unsigned r)
{
    std::string result;

    if (r & BLK_LEFTOVERS) {
        add_to_reason_string(result, true, do_pad, "L", "leftovers");
    } else if (r & BLK_PADDING) {
        add_to_reason_string(result, true, do_pad, "N", "padding");
    } else if (r & BLK_INTRAFUNC) {
        add_to_reason_string(result, true, do_pad, "V", "intrafunc"); // because V is used for FUNC_INTRABLOCK
    } else if (r & BLK_JUMPTABLE) {
        add_to_reason_string(result, true, do_pad, "J", "jumptable");
    } else {
        add_to_reason_string(result, (r & BLK_ENTRY_POINT),  do_pad, "E", "entry point");
    }

    if (r & BLK_CFGHEAD) {
        add_to_reason_string(result, true, do_pad, "H", "CFG head");
    } else if (r & BLK_GRAPH1) {
        add_to_reason_string(result, true, do_pad, "1", "graph-1");
    } else {
        add_to_reason_string(result, (r & BLK_GRAPH2), do_pad, "2", "graph-2");
    }

    add_to_reason_string(result, (r & BLK_USERDEF),      do_pad, "U", "user defined");
    return result;
}
/*
 *
 *******************************************************************************************************************************/

/* Progress report class variables. */
time_t Partitioner::progress_interval = 10;
time_t Partitioner::progress_time = 0;
FILE *Partitioner::progress_file = stderr;

/* Set progress reporting values. */
void
Partitioner::set_progress_reporting(FILE *output, unsigned min_interval)
{
    progress_file = output;
    progress_interval = min_interval;
}

/* Produce a progress report if enabled. */
void
Partitioner::progress(FILE *debug, const char *fmt, ...) const
{
    time_t now = time(NULL);

    if (0==progress_time)
        progress_time = now;

    if (progress_file!=NULL && now-progress_time >= progress_interval) {
        progress_time = now;
        va_list ap;
        va_start(ap, fmt);
        vfprintf(progress_file, fmt, ap);
        va_end(ap);
    }

    if (debug!=NULL) {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(debug, fmt, ap);
        va_end(ap);
    }
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
            bits = SgAsmFunction::FUNC_ENTRY_POINT;
        } else if (word=="call" || word=="call_target") {
            bits = SgAsmFunction::FUNC_CALL_TARGET;
        } else if (word=="eh" || word=="eh_frame") {
            bits = SgAsmFunction::FUNC_EH_FRAME;
        } else if (word=="import") {
            bits = SgAsmFunction::FUNC_IMPORT;
        } else if (word=="symbol") {
            bits = SgAsmFunction::FUNC_SYMBOL;
        } else if (word=="pattern") {
            bits = SgAsmFunction::FUNC_PATTERN;
        } else if (word=="userdef") {
            bits = SgAsmFunction::FUNC_USERDEF;
        } else if (word=="pad" || word=="padding" || word=="interpad") {
            bits = SgAsmFunction::FUNC_INTERPAD;
        } else if (word=="unassigned" || word=="unclassified" || word=="leftover" || word=="leftovers") {
            bits = SgAsmFunction::FUNC_LEFTOVERS;
        } else if (word=="default") {
            bits = SgAsmFunction::FUNC_DEFAULT;
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

static bool
is_writable(const MemoryMap::MapElement &me) {
    return 0 != (me.get_mapperms() & MemoryMap::MM_PROT_WRITE);
}

static bool
is_unexecutable(const MemoryMap::MapElement &me)
{
    return 0 == (me.get_mapperms() & MemoryMap::MM_PROT_EXEC);
}

/* Set disassembly and memory initialization maps. */
void
Partitioner::set_map(MemoryMap *map, MemoryMap *ro_map)
{
    this->map = map;
    if (map) {
        if (ro_map) {
            this->ro_map = *ro_map;
        } else {
            this->ro_map = *map;
            this->ro_map.prune(is_writable);
        }
    } else {
        this->ro_map.clear();
    }
}

/** Runs local block analyses if their cached results are invalid and caches the results.  A local analysis is one whose
 *  results only depend on the specified block and which are valid into the future as long as the instructions in the block do
 *  not change. */
void
Partitioner::update_analyses(BasicBlock *bb)
{
    assert(bb!=NULL && !bb->insns.empty());
    if (bb->valid_cache()) return;

    /* Successor analysis */
    bb->cache.sucs = bb->insns.front()->get_successors(bb->insns, &(bb->cache.sucs_complete), &ro_map);

    /* Try to handle indirect jumps of the form "jmp ds:[BASE+REGISTER*WORDSIZE]".  The trick is to assume that some kind of
     * jump table exists beginning at address BASE, and that the table contains only addresses of valid code.  All we need to
     * do is look for the first entry in the table that doesn't point into an executable region of the disassembly memory
     * map. We use a "do" loop so the logic nesting doesn't get so deep: just break when we find that something doesn't match
     * what we expect. */
    if (!bb->cache.sucs_complete && bb->cache.sucs.empty()) {
        do {
            SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(bb->last_insn());
            if (!insn_x86 || (insn_x86->get_kind()!=x86_jmp && insn_x86->get_kind()==x86_farjmp) ||
                1!=insn_x86->get_operandList()->get_operands().size())
                break;
            SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(insn_x86->get_operandList()->get_operands()[0]);
            if (!mre)
                break;
            SgAsmBinaryAdd *add = isSgAsmBinaryAdd(mre->get_address());
            if (!add)
                break;
            SgAsmValueExpression *base = isSgAsmValueExpression(add->get_lhs());
            if (!base)
                break;
            SgAsmBinaryMultiply *mult = isSgAsmBinaryMultiply(add->get_rhs());
            if (!mult)
                break;
            SgAsmRegisterReferenceExpression *reg = isSgAsmRegisterReferenceExpression(mult->get_lhs());
            if (!reg)
                break;
            SgAsmValueExpression *step = isSgAsmValueExpression(mult->get_rhs());
            if (!step)
                break;
            rose_addr_t base_va = value_of(base);
            size_t entry_size = value_of(step);
            if (! ((entry_size==2 && insn_x86->get_operandSize()==x86_insnsize_16) ||
                   (entry_size==4 && insn_x86->get_operandSize()==x86_insnsize_32) ||
                   (entry_size==8 && insn_x86->get_operandSize()==x86_insnsize_64)))
                break;

            /* How big is the table? */
            size_t nentries = 0;
            while (1) {
                uint8_t buf[8];
                size_t nread = ro_map.read(buf, base_va+nentries*entry_size, entry_size);
                if (nread!=entry_size)
                    break;
                rose_addr_t target_va = 0;
                for (size_t i=0; i<entry_size; i++)
                    target_va |= buf[i] << (i*8);
                const MemoryMap::MapElement *me = map->find(target_va);
                if (!me || 0==(me->get_mapperms() & MemoryMap::MM_PROT_EXEC))
                    break;
                ++nentries;
                bb->cache.sucs.insert(target_va);
            }
            if (0==nentries)
                break;

            /* Create a data block for the jump table. */
            DataBlock *dblock = find_db_starting(base_va, nentries*entry_size);
            append(bb, dblock, SgAsmBlock::BLK_JUMPTABLE);

            if (debug)
                fprintf(debug, "[jump table at 0x%08"PRIx64"+%zu*%zu]", base_va, nentries, entry_size);
        } while (0);
    }

    /* Call target analysis. For x86, a function call is any CALL instruction except when the call target is the fall-through
     * address and the instruction at the fall-through address pops the top of the stack (this is how position independent
     * code loads EIP into a general-purpose register). FIXME: For now we'll assume that any call to the fall-through address
     * is not a function call. */
    rose_addr_t fallthrough_va = bb->last_insn()->get_address() + bb->last_insn()->get_size();
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
        rose_addr_t fall_through_va = canonic_block(bb->last_insn()->get_address() + bb->last_insn()->get_size());
        rose_addr_t call_target_va = call_target(bb);
        if (call_target_va!=NO_TARGET) {
            BasicBlock *target_bb = find_bb_starting(call_target_va, false);
            if (target_bb && target_bb->function && target_bb->function->may_return)
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

/** Returns the first address of a basic block.  Since the instructions of a basic block are not necessarily monotonically
 * increasing, the first address might not be the lowest address. */
rose_addr_t
Partitioner::BasicBlock::address() const
{
    assert(!insns.empty());
    return insns.front()->get_address();
}

/** Returns the first address of a data block.  This might not be the lowest address--it's just the starting address of the
 *  first data node that was added. */
rose_addr_t
Partitioner::DataBlock::address() const
{
    assert(!nodes.empty());
    return nodes.front()->get_address();
}

/** Returns the function to which this data block is effectively assigned.  This returns, in this order, the function to which
 *  this data block is explicitly assigned, the function to which this block is implicitly assigned via an association with a
 *  basic block, or a null pointer. */
Partitioner::Function *
Partitioner::effective_function(DataBlock *dblock)
{
    if (dblock->function)
        return dblock->function;
    if (dblock->basic_block)
        return dblock->basic_block->function;
    return NULL;
}

/* Returns last instruction, the one that exits from the block. */
SgAsmInstruction *
Partitioner::BasicBlock::last_insn() const
{
    assert(insns.size()>0);
    return insns.back();
}

/* Removes association between data blocks and basic blocks. */
void
Partitioner::BasicBlock::clear_data_blocks()
{
    for (std::set<DataBlock*>::iterator di=data_blocks.begin(); di!=data_blocks.end(); ++di)
        (*di)->basic_block = NULL;
    data_blocks.clear();
}

/* Release all basic blocks from a function. Do not delete the blocks. */
void
Partitioner::Function::clear_basic_blocks()
{
    for (BasicBlocks::iterator bi=basic_blocks.begin(); bi!=basic_blocks.end(); ++bi)
        bi->second->function = NULL;
    basic_blocks.clear();
}

/* Release all data blocks from a function. Do not delete the blocks. */
void
Partitioner::Function::clear_data_blocks()
{
    for (DataBlocks::iterator bi=data_blocks.begin(); bi!=data_blocks.end(); ++bi)
        bi->second->function = NULL;
    data_blocks.clear();
}

/* Return partitioner to initial state */
void
Partitioner::clear()
{
    /* Delete all functions */
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        fi->second->clear_basic_blocks();
        fi->second->clear_data_blocks();
        delete fi->second;
    }
    functions.clear();

    /* Delete all basic blocks. We don't need to call Partitioner::discard() to fix up ptrs because all functions that might
     * have pointed to this block have already been deleted, and the insn2block map has also been cleared. */
    insn2block.clear();
    for (BasicBlocks::iterator bi=basic_blocks.begin(); bi!=basic_blocks.end(); ++bi)
        delete bi->second;
    basic_blocks.clear();

    /* Delete all data blocks, but not the SgAsmStaticData nodes to which they point. */
    for (DataBlocks::iterator bi=data_blocks.begin(); bi!=data_blocks.end(); ++bi)
        delete bi->second;
    data_blocks.clear();

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
    if (nread<0 || nread<sb.st_size) {
        delete[] config;
        close(fd);
        throw IPDParser::Exception(strerror(errno), filename);
    }
    IPDParser(this, config, sb.st_size, filename).parse();
    delete[] config;
    close(fd);
#endif
}

/** Reduces the size of a basic block by truncating its list of instructions.  The new block contains initial instructions up
 *  to but not including the instruction at the specified virtual address.  The addresses of the instructions (aside from the
 *  instruction with the specified split point), are irrelevant since the choice of where to split is based on the relative
 *  positions in the basic block's instruction vector rather than instruction address.
 *
 *  If this basic block's size decreased, then any data blocks associated with this basic block are no longer associated with
 *  this basic block. */
void
Partitioner::truncate(BasicBlock* bb, rose_addr_t va)
{
    assert(bb);
    assert(bb==find_bb_containing(va));

    /* Find the cut point in the instruction vector. I.e., the first instruction to remove from the vector. */
    std::vector<SgAsmInstruction*>::iterator cut = bb->insns.begin();
    while (cut!=bb->insns.end() && (*cut)->get_address()!=va) ++cut;
    assert(cut!=bb->insns.begin()); /*we can't remove them all since basic blocks are never empty*/

    /* Remove instructions (from the cut point and beyond) and all the data blocks. */
    for (std::vector<SgAsmInstruction*>::iterator ii=cut; ii!=bb->insns.end(); ++ii) {
        assert(insn2block[(*ii)->get_address()] == bb);
        insn2block[(*ii)->get_address()] = NULL;
    }
    if (cut!=bb->insns.end()) {
        bb->insns.erase(cut, bb->insns.end());
        bb->clear_data_blocks();
    }
}

/* Append instruction to basic block */
void
Partitioner::append(BasicBlock* bb, SgAsmInstruction* insn)
{
    assert(bb);
    assert(insn);
    assert(insn2block[insn->get_address()]==NULL); /*insn must not already belong to a basic block*/
    bb->insns.push_back(insn);
    insn2block[insn->get_address()] = bb;
}

/** Associate a data block with a basic block.  Any basic block can point to zero or more data blocks.  The data block will
 *  then be kept with the same function as the basic block.  This is typically used for things like jump tables, where the last
 *  instruction of the basic block is an indirect jump, and the data block contains the jump table.  When a blasic block is
 *  truncated, it looses its data blocks.
 *
 *  A data block's explicit function assignment (i.e., its "function" member) overrides its assignment via a basic block.  A
 *  data block can be assigned to at most one basic block.
 *
 *  The @p reason argument is a bit vector of SgAsmBlock::Reason bits that are added to the data block's reasons for existing. */
void
Partitioner::append(BasicBlock *bb, DataBlock *db, unsigned reason)
{
    assert(bb!=NULL);
    assert(db!=NULL);
    db->reason |= reason;

    if (db->basic_block!=NULL)
        db->basic_block->data_blocks.erase(db);

    bb->data_blocks.insert(db);
    db->basic_block = bb;
}

/** Append basic block to function.  This method is a bit of a misnomer because the order that blocks are appended to a
 *  function is irrelevant -- the blocks are stored in a map by order of block entry address.  The block being appended must
 *  not already belong to some other function, but it's fine if the block already belongs to the function to which it is being
 *  appended (it is not added a second time).
 *
 *  Whenever a block is added to a function, we should supply a reason for adding it.  The @p reasons bit vector are those
 *  reasons.  The bits are from the SgAsmBlock::Reason enum.
 *
 *  If the @p keep argument is true, then the block's entry address is also added to the function's list of control flow graph
 *  (CFG) heads.  These are the addresses of blocks which are used to start the recursive CFG analysis phase of function block
 *  discovery.  The function's entry address is always considered a CFG head even if it doesn't appear in the set of heads. */
void
Partitioner::append(Function* f, BasicBlock *bb, unsigned reason, bool keep/*=false*/)
{
    assert(f);
    assert(bb);

    if (keep)
        f->heads.insert(bb->address());
    bb->reason |= reason;

    if (bb->function==f)
        return;

    assert(bb->function==NULL);
    bb->function = f;
    f->basic_blocks[bb->address()] = bb;

    /* If the block is a function return then mark the function as returning.  On a transition from a non-returning function
     * to a returning function, we must mark all calling functions as pending so that the fall-through address of their
     * function calls to this function are eventually discovered.  This includes recursive calls since we may have already
     * discovered the recursive call but not followed the fall-through address.
     *
     * FIXME: It's probably no longer necessary to go back and mark calling functions as pending because we do that in the
     *        analyze_cfg() loop.  Doing it in analyze_cfg() is probably more efficient than running these nested loops each
     *        time we have a transition. [RPM 2010-07-30] */
    update_analyses(bb);
    if (bb->cache.function_return && !f->may_return) {
        f->may_return = true;
        if (debug) fprintf(debug, "[returns-to");
        for (BasicBlocks::iterator bbi=basic_blocks.begin(); bbi!=basic_blocks.end(); ++bbi) {
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

/** Append data region to function.  This method is a bit of a misnomer because the order that the data blocks are appended to
 *  the function is irrelevant -- the blocks are stored in a map by order of block address.  The data block being appended must
 *  not already belong to some other function, but it's fine if the block already belongs to the function to which it is being
 *  appended (it is not added a second time).
 *
 *  Whenever a block is added to a function, we should supply a reason for adding it.  The @p reason bit vector are those
 *  reasons.  The bits are from the SgAsmBlock::Reason enum.
 *
 *  If @p force is true then the data block is first removed from any basic block or function to which it already belongs. */
void
Partitioner::append(Function *func, DataBlock *block, unsigned reason, bool force)
{
    assert(func);
    assert(block);

    if (force) {
        if (block->function)
            remove(block->function, block);
        if (block->basic_block)
            remove(block->basic_block, block);
    }

    block->reason |= reason;
    if (block->function==func)
        return;

    assert(block->function==NULL);
    block->function = func;
    func->data_blocks[block->address()] = block;
}

/** Remove a basic block from a function.  The block and function continue to exist--only the association between them is
 *  broken. */
void
Partitioner::remove(Function* f, BasicBlock* bb)
{
    assert(f);
    assert(bb);
    assert(bb->function==f);
    bb->function = NULL;
    f->basic_blocks.erase(bb->address());
}

/** Remove a data block from a function. The block and function continue to exist--only the association between them is
 *  broken.  The data block might also be associated with a basic block, in which case the data block will ultimately belong to
 *  the same function as the basic block. */
void
Partitioner::remove(Function *f, DataBlock *db)
{
    assert(f);
    assert(db);
    assert(db->function==f);
    db->function = NULL;
    f->data_blocks.erase(db->address());
}

/** Remove a data block from a basic block.  The blocks continue to exist--only the association between them is broken.  The
 *  data block might still be associated with a function, in which case it will ultimately end up in that function. */
void
Partitioner::remove(BasicBlock *bb, DataBlock *db)
{
    assert(bb!=NULL);
    if (db && db->basic_block==bb) {
        bb->data_blocks.erase(db);
        db->basic_block = NULL;
    }
}

/* Remove instruction from consideration. */
SgAsmInstruction *
Partitioner::discard(SgAsmInstruction *insn, bool discard_entire_block)
{
    if (insn) {
        rose_addr_t va = insn->get_address();
        BasicBlock *bb = find_bb_containing(va, false);
        if (bb) {
            if (discard_entire_block) {
                discard(bb);
            } else if (bb->insns.front()==insn) {
                discard(bb);
            } else {
                truncate(bb, va);
            }
        }
        insns.erase(va);
    }
    return NULL;
}

/* Delete block, returning its instructions back to the (implied) list of free instructions. */
Partitioner::BasicBlock *
Partitioner::discard(BasicBlock *bb)
{
    if (bb) {
        assert(NULL==bb->function);

        /* Remove instructions from the block, returning them to the (implied) list of free instructions. */
        for (std::vector<SgAsmInstruction*>::iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii)
            insn2block.erase((*ii)->get_address());

        /* Remove the association between data blocks and this basic block. */
        bb->clear_data_blocks();

        /* Remove the block from the partitioner. */
        basic_blocks.erase(bb->address());
        delete bb;
    }
    return NULL;
}

/* Finds (or possibly creates) an instruction beginning at the specified address. */
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
            basic_blocks.insert(std::make_pair(va, bb));
        }
        append(bb, insn);
        va += insn->get_size();
        if (insn->terminatesBasicBlock()) { /*naively terminates?*/
            bool complete;
            const Disassembler::AddressSet& sucs = successors(bb, &complete);
            if ((func_heuristics & SgAsmFunction::FUNC_CALL_TARGET) && is_function_call(bb, NULL)) {
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
    assert(!bb || bb->insns.size()>0);
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
    if (va==bb->address())
        return bb;
    if (!create)
        return NULL;
    if (debug)
        fprintf(debug, "[split from B%08"PRIx64"]", bb->address());
    if (bb->function!=NULL)
        bb->function->pending = true;
    truncate(bb, va);
    bb = find_bb_containing(va);
    assert(bb!=NULL);
    assert(va==bb->address());
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
    assert(!"possible alias loop");
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
        assert(f->entry_va==entry_va);
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
            assert(map!=NULL);
            using namespace VirtualMachineSemantics;
            typedef X86InstructionSemantics<Policy, ValueType> Semantics;
            Policy policy;
            policy.set_map(map);
            Semantics semantics(policy);

            if (debug) fprintf(stderr, "  running semantics for the basic block...\n");
            for (std::vector<SgAsmInstruction*>::iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(*ii);
                assert(insn!=NULL);
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
                                   policy.number<32>(bb->insns.back()->get_address()+bb->insns.back()->get_size()),
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
            assert(disassembler!=NULL);
            policy.writeIP(policy.number<32>(text_va));
            while (1) {
                rose_addr_t ip = policy.readIP().known_value();
                if (ip==return_va) break;
                SgAsmx86Instruction *insn = isSgAsmx86Instruction(disassembler->disassembleOne(map, ip));
                if (debug) fprintf(stderr, "    0x%08"PRIx64": %s\n", ip, insn?unparseInstruction(insn).c_str():"<null>");
                assert(insn!=NULL);
                semantics.processInstruction(insn);
                assert(policy.readIP().is_known());
                SageInterface::deleteAST(insn);
            }

            /* Extract the list of successors. The number of successors is the first element of the list. */
            if (debug) fprintf(stderr, "  extracting program return values...\n");
            ValueType<32> nsucs = policy.readMemory<32>(x86_segreg_ss, policy.number<32>(svec_va), policy.true_());
            assert(nsucs.is_known());
            if (debug) fprintf(stderr, "    number of successors: %"PRId64"\n", nsucs.known_value());
            assert(nsucs.known_value()*4 <= svec_size-4); /*first entry is size*/
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
            add_function(entry_va, SgAsmFunction::FUNC_ENTRY_POINT);
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
                        add_function(target, SgAsmFunction::FUNC_EH_FRAME);
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
        if (reloc_section)
            rsects.insert(reloc_section);
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
        plt_offset += insn->get_size();
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
            SgAsmElfSymbolList *symbols = symbol_section ? symbol_section->get_symbols() : NULL;
            for (size_t ei=0; ei<entries->get_entries().size() && name.empty() && symbols; ++ei) {
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

        Function *plt_func = add_function(insn->get_address(), SgAsmFunction::FUNC_IMPORT, name);

        /* FIXME: Assume that most PLT functions return. We make this assumption for now because the PLT table contains an
         *        indirect jump through the .plt.got data area and we don't yet do static analysis of the data.  Because of
         *        that, all the PLT functons will contain only a basic block with the single indirect jump, and no return
         *        (e.g., x86 RET or RETF) instruction, and therefore the function would not normally be marked as returning.
         *        [RPM 2010-05-11] */
        if ("abort@plt"!=name && "execl@plt"!=name && "execlp@plt"!=name && "execv@plt"!=name && "execvp@plt"!=name &&
            "exit@plt"!=name && "_exit@plt"!=name && "fexecve@plt"!=name &&
            "longjmp@plt"!=name && "__longjmp@plt"!=name && "siglongjmp@plt"!=name)
            plt_func->may_return = true;
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
                    add_function(value, SgAsmFunction::FUNC_SYMBOL, symbol->get_name()->get_string());

                /* Sometimes weak symbol values are offsets from a section (this code handles that), but other times they're
                 * the value is used directly (the above code handled that case). */
                SgAsmGenericSection *section = symbol->get_bound();
                if (section && symbol->get_binding()==SgAsmGenericSymbol::SYM_WEAK)
                    value += section->get_mapped_actual_va();
                if (find_instruction(value))
                    add_function(value, SgAsmFunction::FUNC_SYMBOL, symbol->get_name()->get_string());
            }
        }
    }
}

/** See Partitioner::mark_func_patterns. Tries to match "(mov rdi,rdi)?; push rbp; mov rbp,rsp" (or the 32-bit equivalent). The
 *  first MOV instruction is a two-byte no-op used for hot patching of executables (single instruction rather than two NOP
 *  instructions so that no thread is executing at the second byte when the MOV is replaced by a JMP).  The PUSH and second MOV
 *  are the standard way to set up the stack frame. */
static Disassembler::InstructionMap::const_iterator
pattern1(const Disassembler::InstructionMap& insns, Disassembler::InstructionMap::const_iterator first,
         Disassembler::AddressSet &exclude)
{
    Disassembler::InstructionMap::const_iterator ii = first;
    Disassembler::AddressSet matches;

    /* Look for optional "mov rdi, rdi"; if found, advance ii iterator to fall-through instruction */
    do {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
        if (!insn || insn->get_kind()!=x86_mov)
            break;
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=2)
            break;
        SgAsmx86RegisterReferenceExpression *rre = isSgAsmx86RegisterReferenceExpression(opands[0]);
        if (!rre ||
            rre->get_descriptor().get_major()!=x86_regclass_gpr ||
            rre->get_descriptor().get_minor()!=x86_gpr_di)
            break;
        rre = isSgAsmx86RegisterReferenceExpression(opands[1]);
        if (!rre ||
            rre->get_descriptor().get_major()!=x86_regclass_gpr ||
            rre->get_descriptor().get_minor()!=x86_gpr_di)
            break;
        matches.insert(ii->first);
        ii = insns.find(ii->first + insn->get_size());
    } while (0);

    /* Look for "push rbp" */
    {
        if (ii==insns.end())
            return insns.end();
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
        if (!insn || insn->get_kind()!=x86_push)
            return insns.end();
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=1)
            return insns.end();
        SgAsmx86RegisterReferenceExpression *rre = isSgAsmx86RegisterReferenceExpression(opands[0]);
        if (!rre ||
            rre->get_descriptor().get_major()!=x86_regclass_gpr ||
            rre->get_descriptor().get_minor()!=x86_gpr_bp)
            return insns.end();
        matches.insert(ii->first);
        ii = insns.find(ii->first + insn->get_size());
    }

    /* Look for "mov rbp,rsp" */
    {
        if (ii==insns.end())
            return insns.end();
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
        if (!insn || insn->get_kind()!=x86_mov)
            return insns.end();
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=2)
            return insns.end();
        SgAsmx86RegisterReferenceExpression *rre = isSgAsmx86RegisterReferenceExpression(opands[0]);
        if (!rre ||
            rre->get_descriptor().get_major()!=x86_regclass_gpr ||
            rre->get_descriptor().get_minor()!=x86_gpr_bp)
            return insns.end();
        rre = isSgAsmx86RegisterReferenceExpression(opands[1]);
        if (!rre ||
            rre->get_descriptor().get_major()!=x86_regclass_gpr ||
            rre->get_descriptor().get_minor()!=x86_gpr_sp)
            return insns.end();
        matches.insert(ii->first);
    }

    exclude.insert(matches.begin(), matches.end());
    return first;
}

#if 0 /*commented out in Partitioner::mark_func_patterns()*/
/** See Partitioner::mark_func_patterns. Tries to match "nop;nop;nop" followed by something that's not a nop and returns the
 *  something that's not a nop if successful. */
static Disassembler::InstructionMap::const_iterator
pattern2(const Disassembler::InstructionMap& insns, Disassembler::InstructionMap::const_iterator first,
         Disassembler::AddressSet &exclude)
{
    Disassembler::InstructionMap::const_iterator ii = first;
    Disassembler::AddressSet matches;

    /* Look for three "nop" instructions */
    for (size_t i=0; i<3; i++) {
        SgAsmx86Instruction *nop = isSgAsmx86Instruction(ii->second);
        if (!nop) return insns.end();
        if (nop->get_kind()!=x86_nop) return insns.end();
        if (nop->get_operandList()->get_operands().size()!=0) return insns.end(); /*only zero-arg NOPs allowed*/
        matches.insert(ii->first);
        ii = insns.find(ii->first + nop->get_size());
        if (ii==insns.end()) return insns.end();
    }

    /* Look for something that's not a "nop"; this is the function entry point. */
    SgAsmx86Instruction *notnop = isSgAsmx86Instruction(ii->second);
    if (!notnop) return insns.end();
    if (notnop->get_kind()==x86_nop) return insns.end();
    matches.insert(ii->first);

    exclude.insert(matches.begin(), matches.end());
    return ii;
}
#endif

#if 0 /* commented out in Partitioner::mark_func_patterns() */
/** See Partitioner::mark_func_patterns. Tries to match "leave;ret" followed by one or more "nop" followed by a non-nop
 *  instruction and if matching, returns the iterator for the non-nop instruction. */
static Disassembler::InstructionMap::const_iterator
pattern3(const Disassembler::InstructionMap& insns, Disassembler::InstructionMap::const_iterator first,
         Disassembler::AddressSet &exclude)
{
    Disassembler::InstructionMap::const_iterator ii = first;
    Disassembler::AddressSet matches;

    /* leave; ret; nop */
    for (size_t i=0; i<3; i++) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
        if (!insn) return insns.end();
        if ((i==0 && insn->get_kind()!=x86_leave) ||
            (i==1 && insn->get_kind()!=x86_ret)   ||
            (i==2 && insn->get_kind()!=x86_nop))
            return insns.end();
        matches.insert(ii->first);
        ii = insns.find(ii->first + insn->get_size());
        if (ii==insns.end()) return insns.end();
    }

    /* Zero or more "nop" instructions */
    while (1) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
        if (!insn) return insns.end();
        if (insn->get_kind()!=x86_nop) break;
        matches.insert(ii->first);
        ii = insns.find(ii->first + insn->get_size());
        if (ii==insns.end()) return insns.end();
    }

    /* This must be something that's not a "nop", but make sure it's an x86 instruction anyway. */
    SgAsmx86Instruction *insn = isSgAsmx86Instruction(ii->second);
    if (!insn) return insns.end();
    matches.insert(ii->first);

    exclude.insert(matches.begin(), matches.end());
    return ii;
}
#endif

/** Seeds functions according to instruction patterns.  Note that this pattern matcher only looks at existing instructions--it
 *  does not actively disassemble new instructions.  In other words, this matcher is intended mostly for passive-mode
 *  partitioners where the disassembler has already disassembled everything it can. */
void
Partitioner::mark_func_patterns()
{
    Disassembler::AddressSet exclude;
    Disassembler::InstructionMap::const_iterator found;

    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        if (exclude.find(ii->first)==exclude.end() && (found=pattern1(insns, ii, exclude))!=insns.end())
            add_function(found->first, SgAsmFunction::FUNC_PATTERN);
    }
#if 0 /* Disabled because NOPs sometimes legitimately appear inside functions */
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        if (exclude.find(ii->first)==exclude.end() && (found=pattern2(insns, ii, exclude))!=insns.end())
            add_function(found->first, SgAsmFunction::FUNC_PATTERN);
    }
#endif
#if 0 /* Disabled because NOPs sometimes follow "leave;ret" for functions with multiple returns. */
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        if (exclude.find(ii->first)==exclude.end() && (found=pattern3(insns, ii, exclude))!=insns.end())
            add_function(found->first, SgAsmFunction::FUNC_PATTERN);
    }
#endif
}

/* Make all CALL/FARCALL targets functions.  This is a naive approach that won't work for some obfuscated software. A more
 * thorough approach considers only those calls that are reachable.  A CALL whose target is the address following the CALL
 * instruction is not counted as a function call. */
void
Partitioner::mark_call_insns()
{
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        std::vector<SgAsmInstruction*> iv;
        iv.push_back(ii->second);
        rose_addr_t target_va=NO_TARGET;
        if (ii->second->is_function_call(iv, &target_va) && target_va!=NO_TARGET &&
            target_va!=ii->first + ii->second->get_size()) {
            add_function(target_va, SgAsmFunction::FUNC_CALL_TARGET, "");
        }
    }
}

/* Scan through ranges of contiguous instructions */
void
Partitioner::scan_contiguous_insns(Disassembler::InstructionMap insns, InsnRangeCallbacks &cblist,
                                   SgAsmInstruction *prev, SgAsmInstruction *end)
{
    while (!insns.empty()) {
        SgAsmInstruction *first = insns.begin()->second;
        rose_addr_t va = first->get_address();
        Disassembler::InstructionMap::iterator ii = insns.find(va);
        std::vector<SgAsmInstruction*> contig;
        while (ii!=insns.end()) {
            contig.push_back(ii->second);
            va += ii->second->get_size();
            ii = insns.find(va);
        }
        cblist.apply(true, InsnRangeCallback::Args(this, prev, first, end, contig.size()));
        for (size_t i=0; i<contig.size(); i++)
            insns.erase(contig[i]->get_address());
    }
}

/* Scan through all unassigned instructions */
void
Partitioner::scan_unassigned_insns(InsnRangeCallbacks &cblist)
{
    if (cblist.empty())
        return;

    /* We can't iterate over the instruction list while invoking callbacks because one of them might change the instruction
     * list.  Therefore, iterate over a copy of the list.  Instructions are never deleted by the partitioner (only added), so
     * this is safe to do. */
    Disassembler::InstructionMap all = this->insns;
    Disassembler::InstructionMap range;
    SgAsmInstruction *prev = NULL;
    for (Disassembler::InstructionMap::iterator ai=all.begin(); ai!=all.end(); ++ai) {
        BasicBlock *bb = find_bb_containing(ai->first, false);
        Function *func = bb ? bb->function : NULL;
        if (func) {
            if (!range.empty()) {
                scan_contiguous_insns(range, cblist, prev, ai->second);
                range.clear();
            }
            prev = ai->second;
        } else {
            range.insert(*ai);
        }
    }
    if (!range.empty())
        scan_contiguous_insns(range, cblist, prev, NULL);
}

/* Similar to scan_unassigned_insns except only invokes callbacks when the "prev" and "end" instructions belong to different
 * functions or one of them doesn't exist. */
void
Partitioner::scan_interfunc_insns(InsnRangeCallbacks &cblist)
{
    if (cblist.empty())
        return;

    struct Filter: public InsnRangeCallback {
        virtual bool operator()(bool enabled, const Args &args) {
            if (enabled) {
                if (!args.insn_prev || !args.insn_end)
                    return true;
                BasicBlock *bb_lt = args.partitioner->find_bb_containing(args.insn_prev->get_address(), false);
                BasicBlock *bb_rt = args.partitioner->find_bb_containing(args.insn_end->get_address(), false);
                assert(bb_lt && bb_lt->function); // because we're invoked from scan_unassigned_insns
                assert(bb_rt && bb_rt->function); // ditto
                enabled = bb_lt->function != bb_rt->function;
            }
            return enabled;
        }
    } filter;
    InsnRangeCallbacks cblist2 = cblist;
    cblist2.prepend(&filter);
    scan_unassigned_insns(cblist2);
}

/* Similar to scan_unassigned_insns except only invokes callbacks when "prev" and "end" instructions belong to the same
 * function. */
void
Partitioner::scan_intrafunc_insns(InsnRangeCallbacks &cblist)
{
    if (cblist.empty())
        return;

    struct Filter: public InsnRangeCallback {
        virtual bool operator()(bool enabled, const Args &args) {
            if (enabled) {
                if (!args.insn_prev || !args.insn_end)
                    return false;
                BasicBlock *bb_lt = args.partitioner->find_bb_containing(args.insn_prev->get_address(), false);
                BasicBlock *bb_rt = args.partitioner->find_bb_containing(args.insn_end->get_address(), false);
                assert(bb_lt && bb_lt->function); // because we're invoked from scan_unassigned_insns
                assert(bb_rt && bb_rt->function); // ditto
                enabled = bb_lt->function == bb_rt->function;
            }
            return enabled;
        }
    } filter;
    InsnRangeCallbacks cblist2 = cblist;
    cblist2.prepend(&filter);
    scan_unassigned_insns(cblist2);
}

void
Partitioner::scan_unassigned_bytes(ByteRangeCallbacks &cblist, MemoryMap *restrict/*=NULL*/)
{
    if (cblist.empty())
        return;

    /* Get range map for addresses assigned to functions (function instructions and data w/ function pointers). */
    FunctionRangeMap assigned;
    function_extent(&assigned);

    /* Unassigned ranges are the inverse of everything assigned.  Then further restrict the unassigned range map according to
     * the supplied memory map. */
    ExtentMap unassigned = assigned.invert<ExtentMap>();
    if (restrict)
        unassigned.erase_ranges(restrict->va_extents().invert<ExtentMap>());

    /* Traverse the unassigned map, invoking the callbacks for each range. */
    for (ExtentMap::iterator ri=unassigned.begin(); ri!=unassigned.end(); ++ri)
        cblist.apply(true, ByteRangeCallback::Args(this, assigned, ri->first));
}

void
Partitioner::scan_intrafunc_bytes(ByteRangeCallbacks &cblist, MemoryMap *restrict/*=NULL*/)
{
    if (cblist.empty())
        return;

    struct Filter: public ByteRangeCallback {
        virtual bool operator()(bool enabled, const Args &args) {
            if (enabled) {
                if (args.range.first()<=Extent::minimum() || args.range.last()>=Extent::maximum())
                    return false; // nothing before and/or after

                /* Find the closest function before this range. */
                FunctionRangeMap::const_iterator prev = args.ranges.find_prior(args.range.first()-1);
                if (prev==args.ranges.end())
                    return false; // nothing before this range

                /* Find the closest function above this range. */
                FunctionRangeMap::const_iterator next = args.ranges.lower_bound(args.range.last()+1);
                if (next==args.ranges.end())
                    return false; // nothing after this range

                /* Continue only if this range is between two of the same functions. */
                enabled = prev->second.get()==next->second.get();
            }
            return enabled;
        }
    } filter;
    ByteRangeCallbacks cblist2 = cblist;
    cblist2.prepend(&filter);
    scan_unassigned_bytes(cblist2, restrict);
}

void
Partitioner::scan_interfunc_bytes(ByteRangeCallbacks &cblist, MemoryMap *restrict/*=NULL*/)
{
    if (cblist.empty())
        return;

    struct Filter: public ByteRangeCallback {
        virtual bool operator()(bool enabled, const Args &args) {
            if (enabled) {
                if (args.range.first()<=Extent::minimum() || args.range.last()>=Extent::maximum())
                    return true; // nothing before and/or after

                /* Find the closest function before this range. */
                FunctionRangeMap::const_iterator prev = args.ranges.find_prior(args.range.first()-1);
                if (prev==args.ranges.end())
                    return true; // nothing before this range

                /* Find the closest function above this range. */
                FunctionRangeMap::const_iterator next = args.ranges.lower_bound(args.range.last()+1);
                if (next==args.ranges.end())
                    return true; // nothing after this range

                /* Continue only if this range is between two different functions. */
                enabled = prev->second.get()!=next->second.get();
            }
            return enabled;
        }
    } filter;
    ByteRangeCallbacks cblist2 = cblist;
    cblist2.prepend(&filter);
    scan_unassigned_bytes(cblist2, restrict);
}

bool
Partitioner::FindDataPadding::operator()(bool enabled, const Args &args)
{
    if (!enabled)
        return false;
    Partitioner *p = args.partitioner;
    Extent range = args.range;

    /* What is the maximum pattern length in bytes? */
    if (patterns.empty())
        return true;
    size_t max_psize = patterns[0].size();
    for (size_t pi=1; pi<patterns.size(); ++pi)
        max_psize = std::max(max_psize, patterns[pi].size());


    /* What is the previous function?  The one to which padding is to be attached. */
    if (range.first()<=Extent::minimum())
        return true;
    FunctionRangeMap::const_iterator prev = begins_contiguously ?
                                            args.ranges.find(range.first()-1) :
                                            args.ranges.find_prior(range.first()-1);
    if (prev==args.ranges.end())
        return true;
    Function *func = prev->second.get();
    assert(func!=NULL);


    /* Do we need to be contiguous with a following function?  This only checks whether the incoming range ends at the
     * beginning of a function.  We'll check below whether a padding sequence also ends at the end of this range. */
    if (ends_contiguously) {
        if (max_psize*maximum_nrep < range.size())
            return true;
        if (range.last()>=Extent::maximum())
            return true;
        FunctionRangeMap::const_iterator next = args.ranges.find(range.last()+1);
        if (next==args.ranges.end())
            return true;
    }

    /* To keep things simple, we read the entire range (which might not be contigous in the MemoryMap) into a contiguous
     * buffer.  However, that means we had better not try to read huge, anonymous ranges.   Also handle the case of a short
     * read, although this shouldn't happen if the caller supplied the correct memory map to the scan_*_bytes() method. */
    if (range.size() > maximum_range_size)
        return true;
    SgUnsignedCharList buf = p->ro_map.read(range.first(), range.size());
    if (ends_contiguously && buf.size()<range.size())
        return true;
    range.resize(buf.size());

    /* There might be more than one sequence of padding bytes.  Look for all of them, but constrained according to
     * begins_contiguously and ends_contiguously. */
    size_t nblocks = 0; // number of blocks added to function
    while (!range.empty()) {
        if (begins_contiguously && range.first()>args.range.first())
            return true;
        for (size_t pi=0; pi<patterns.size(); ++pi) {
            size_t psize = patterns[pi].size();
            assert(psize>0);
            size_t nrep = 0;
            for (size_t offset=range.first()-args.range.first();
                 offset+psize<=buf.size() && nrep<maximum_nrep;
                 offset+=psize, ++nrep) {
                if (memcmp(&buf[offset], &patterns[pi][0], psize))
                    break;
            }
            if (nrep>0 && nrep>=minimum_nrep && (!ends_contiguously || nrep*psize==range.size())) {
                /* Found a matching repeated pattern.  Add data block to function. */
                DataBlock *dblock = p->find_db_starting(range.first(), nrep*psize);
                assert(dblock!=NULL);
                p->append(func, dblock, SgAsmBlock::BLK_PADDING);
                ++nblocks;
                ++nfound;
                if (p->debug) {
                    if (1==nblocks)
                        fprintf(p->debug, "Partitioner::FindDataPadding for F%08"PRIx64": added", func->entry_va);
                    fprintf(p->debug, " D%08"PRIx64, range.first());
                }
                range.first(range.first()+nrep*psize-1); // will be incremented after break
                break;
            }
        }
        if (!range.empty())
            range.first(range.first()+1);
    }
    if (p->debug && nblocks>0)
        fprintf(p->debug, "\n");

    return true;
}

bool
Partitioner::FindData::operator()(bool enabled, const Args &args)
{
    if (!enabled)
        return false;
    Partitioner *p = args.partitioner;

    /* We must have a function immediately before this range and to which this range's data can be attached. */
    if (args.range.first()<=Extent::minimum())
        return true;
    FunctionRangeMap::const_iterator prev = args.ranges.find(args.range.first()-1);
    if (prev==args.ranges.end())
        return true;
    Function *func = prev->second.get();
    assert(func!=NULL);

    /* Don't append data to non-functions. */
    if (0!=(func->reason & excluded_reasons))
        return true;

    /* Padding ranges are computed once and cached. */
    if (NULL==padding_ranges) {
        padding_ranges = new DataRangeMap;
        p->padding_extent(padding_ranges);
    }

    /* Don't append data if the previous thing is padding. */
    if (padding_ranges->find(args.range.first()-1)!=padding_ranges->end())
        return true;

    /* Create a data block and add it to the previous function. */
    DataBlock *dblock = p->find_db_starting(args.range.first(), args.range.size());
    assert(dblock!=NULL);
    p->append(func, dblock, SgAsmBlock::BLK_USERDEF);
    ++nfound;
    if (p->debug)
        fprintf(p->debug, "Partitioner::FindData: for F%08"PRIx64": added D%08"PRIx64"\n",
                func->entry_va, args.range.first());
    return true;
}

/* Create functions or data for inter-function padding instruction sequences.  Returns true if we did not find interfunction
 * padding and other padding callbacks should proceed; returns false if we did find padding and the others should be skipped. */
bool
Partitioner::FindInsnPadding::operator()(bool enabled, const Args &args)
{
    if (!enabled)
        return false;
    if (!args.insn_prev)
        return true;
    assert(args.ninsns>0);
    assert(args.insn_prev!=NULL);
    assert(args.insn_begin!=NULL);

    if (begins_contiguously &&
        args.insn_begin->get_address()!=args.insn_prev->get_address()+args.insn_prev->get_size())
        return true;

    /* The preceding function.  We'll add the padding as data to this function, unless we're creating explicity padding
     * functions. */
    Partitioner *p = args.partitioner;
    Function *prev_func = NULL;
    {
        BasicBlock *last_block = p->find_bb_containing(args.insn_prev->get_address(), false);
        assert(last_block!=NULL);
        prev_func = last_block->function;
    }

    /* Loop over the inter-function instructions and accumulate contiguous ranges of padding. */
    bool retval = true;
    std::vector<SgAsmInstruction*> padding;
    rose_addr_t va = args.insn_begin->get_address();
    SgAsmInstruction *insn = p->find_instruction(va);
    for (size_t i=0; i<args.ninsns && insn!=NULL; i++) {

        /* Does this instruction match? */
        bool matches = false;
        assert(insn!=NULL); // callback is being invoked over instructions
        BasicBlock *bb = p->find_bb_containing(va, false);
        if (bb && bb->function)
            break; // insn is already assigned to a function

        SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(p->find_instruction(va));
        if (!matches && insn_x86) {
            if (x86_kinds.find(insn_x86->get_kind())!=x86_kinds.end())
                matches = true;
        }

        for (size_t j=0; !matches && j<byte_patterns.size(); j++) {
            if (byte_patterns[j]==insn->get_raw_bytes())
                matches = true;
        }


        /* Advance to next instruction, or null. We do this inside the loop so we only need one copy of the code that inserts
         * the basic blocks for padding. */
        va += insn->get_size();
        if (matches)
            padding.push_back(insn);
        insn = i+1<args.ninsns ? p->find_instruction(va) : NULL;

        /* Do we have padding to insert, and are we at the end of that padding? If not, then continue looping. */
        if ((matches && insn) || padding.empty())
            continue; // try to grab more padding instructions
        if (begins_contiguously &&
            padding.front()->get_address()!=args.insn_prev->get_address()+args.insn_prev->get_size())
            return true; // no point in even continuing the loop, since we're already past the first instruction now.
        if (ends_contiguously) {
            if (!matches) {
                padding.clear();
                continue;
            } else if (args.insn_end) {
                if (padding.back()->get_address()+padding.back()->get_size() != args.insn_end->get_address()) {
                    padding.clear();
                    continue;
                }
            } else if (i+1<args.ninsns) {
                padding.clear();
                continue;
            }
        }

        /* Make sure we got enough bytes.  We can subtract first from last since we know they're contiguous in memory. */
        if (padding.back()->get_address()+padding.back()->get_size() - padding.front()->get_address() < minimum_size) {
            padding.clear();
            continue;
        }

        /* If we get here, then we have padding instructions.  Either create a data block to hold the padding, or a new
         * function to hold the padding.  When creating a function, the basic blocks are added as CFG heads, which cause the
         * block to remain with the function even though it might not be reachable by the CFG starting from the function's
         * entry point.  This is especially true for padding like x86 INT3 instructions, which have no known CFG successors and
         * occupy singleton basic blocks. */
        ++nfound;
        assert(!padding.empty());
        if (add_as_data) {
            assert(prev_func!=NULL);
            rose_addr_t begin_va = padding.front()->get_address();
            rose_addr_t end_va = padding.back()->get_address() + padding.back()->get_size();
            assert(end_va>begin_va);
            size_t size = end_va - begin_va;
            DataBlock *dblock = p->find_db_starting(begin_va, size);
            assert(dblock!=NULL);
            p->append(prev_func, dblock, SgAsmBlock::BLK_PADDING);
            for (size_t i=0; i<padding.size(); i++)
                p->discard(padding[i]);
            if (p->debug)
                fprintf(p->debug, "Partitioner::FindInsnPadding: for F%08"PRIx64": added D%08"PRIx64"\n",
                        prev_func->entry_va, begin_va);
        } else {
            Function *new_func = p->add_function(padding.front()->get_address(), SgAsmFunction::FUNC_INTERPAD);
            p->find_bb_starting(padding.front()->get_address()); // split first block if necessary
            p->find_bb_starting(va); // split last block if necessary
            if (p->debug)
                fprintf(p->debug, "Partitioner::FindInsnPadding: for F%08"PRIx64": added", new_func->entry_va);
            for (size_t i=0; i<padding.size(); i++) {
                BasicBlock *bb = p->find_bb_containing(padding[i]->get_address());
                if (bb && !bb->function) {
                    p->append(new_func, bb, SgAsmBlock::BLK_PADDING, true/*head of CFG subgraph*/);
                    if (p->debug)
                        fprintf(p->debug, " B%08"PRIx64, bb->address());
                }
            }
            if (p->debug)
                fprintf(p->debug, "\n");
        }

        retval = padding.size()!=args.ninsns; // allow other callbacks to run only if we didn't suck up all the instructions
        padding.clear();
    }
    return retval;
}

/** Finds (or creates) a data block.  Finds a data block starting at the specified address.  If @p size is non-zero then the
 *  existing data block must contain all bytes in the range @p start_va (inclusive) to @p start_va + @p size (exclusive), and
 *  if it doesn't then a new SgAsmStaticData node is created and appended to either a new data block or a data block that
 *  already begins at the specified address.   If size is zero an no block exists, then the null pointer is returned.  The size
 *  of the existing block does not matter if size is zero. */
Partitioner::DataBlock *
Partitioner::find_db_starting(rose_addr_t start_va, size_t size/*=0*/)
{
    DataBlock *db = NULL;
    DataBlocks::iterator dbi = data_blocks.find(start_va);
    if (dbi!=data_blocks.end()) {
        db = dbi->second;
        if (0==size)
            return db; /* caller doesn't care about the size, only whether the block is present. */

        /* Check whether the block contains all the addresses we want. They might not all be in the first node. */
        DataRangeMap want; want.insert(Extent(start_va, size));
        DataRangeMap have; datablock_extent(db, &have);
        want.erase_ranges(have);
        if (want.empty())
            return db;
    }
    if (0==size)
        return NULL;

    /* Create a new SgAsmStaticData node to represent the entire address range and add it to the (possibly new) data block.
     * When adding to an existing block, the new SgAsmStaticData node will overlap with at least the initial node, and possibly
     * others. */
    if (!db) {
        db = new DataBlock;
        data_blocks[start_va] = db;
    }

    SgUnsignedCharList raw_bytes = map->read(start_va, size, MemoryMap::MM_PROT_NONE);
    assert(raw_bytes.size()==size);
    SgAsmStaticData *datum = new SgAsmStaticData;
    datum->set_address(start_va);
    datum->set_raw_bytes(raw_bytes);
    db->nodes.push_back(datum);

    return db;
}

bool
Partitioner::FindIntraFunctionInsns::operator()(bool enabled, const Args &args)
{
    if (!enabled)
        return false;
    Partitioner *p = args.partitioner;

    /* Compute and cache the extents of all known functions. */
    if (!function_extents) {
        function_extents = new FunctionRangeMap;
        p->function_extent(function_extents);
    }

    /* This range must begin contiguously with a function. */
    if (args.range.first()<=Extent::minimum())
        return true;
    FunctionRangeMap::iterator prev = function_extents->find(args.range.first()-1);
    if (prev==function_extents->end())
        return true;
    Function *func = prev->second.get();

    /* This range must end contiguously with the same function. */
    if (args.range.last()>=Extent::maximum())
        return true;
    FunctionRangeMap::iterator next = function_extents->find(args.range.last()+1);
    if (next==function_extents->end() || next->second.get()!=func)
        return true;

    /* If this function is interleaved with another then how can we know that the instructions in question should actually
     * belong to this function?  If we're interleaved with one other function, then we could very easily be interleaved with
     * additional functions and this block could belong to any of them. */
    if (require_contiguous && !p->is_contiguous(func, false))
        return true;

    /* Get the list of basic blocks for the instructions in this range and their address extents. */
    std::set<BasicBlock*> bblocks;
    ExtentMap block_extents;
    rose_addr_t va = args.range.first();
    while (va<=args.range.last()) {
        BasicBlock *bb = va==args.range.first() ? p->find_bb_starting(va) : p->find_bb_containing(va);
        if (!bb || bb->function)
            return true;
        bblocks.insert(bb);
        for (std::vector<SgAsmInstruction*>::iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
            rose_addr_t first = (*ii)->get_address();
            size_t size = (*ii)->get_size();
            block_extents.insert(Extent(first, size));
        }
        va = block_extents.begin()->first.last() + 1; // the address of the first missing instruction
    }

    /* Make sure that all the instructions we got fit in the hole we're trying to fill. */
    ExtentMap hole; hole.insert(args.range);
    if (!hole.contains(block_extents))
        return true;

    /* Add the basic blocks to the function. */
    for (std::set<BasicBlock*>::iterator bi=bblocks.begin(); bi!=bblocks.end(); ++bi) {
        p->append(func, *bi, SgAsmBlock::BLK_INTRAFUNC, true/*head of CFG subgraph*/);
        ++nfound;
    }

    return true;
}

/* Create functions for any basic blocks that consist of only a JMP to another function. */
bool
Partitioner::FindThunks::operator()(bool enabled, const Args &args)
{
    if (!enabled)
        return false;

    Partitioner *p = args.partitioner;
    rose_addr_t va = args.insn_begin->get_address();
    rose_addr_t next_va = 0;
    for (size_t i=0; i<args.ninsns; i++, va=next_va) {
        SgAsmInstruction *insn = p->find_instruction(va);
        assert(insn);
        next_va = va + insn->get_size();

        /* Instruction must be an x86 JMP */
        SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(insn);
        if (!insn_x86 || (insn_x86->get_kind()!=x86_jmp && insn_x86->get_kind()!=x86_farjmp))
            continue;

        /* Instruction must not be in the middle of an existing basic block. */
        BasicBlock *bb = p->find_bb_containing(va, false);
        if (bb && bb->address()!=va)
            continue;

        if (validate_targets) {
            /* Instruction must have a single successor */
            bool complete;
            Disassembler::AddressSet succs = insn->get_successors(&complete);
            if (!complete && 1!=succs.size())
                continue;
            rose_addr_t target_va = *succs.begin();

            /* The target (single successor) must be a known function which is not padding. */
            Functions::iterator fi = p->functions.find(target_va);
            if (fi==p->functions.end() || 0!=(fi->second->reason & SgAsmFunction::FUNC_INTERPAD))
                continue;
        }

        /* Create the basic block for the JMP instruction.  This block must be a single instruction, which it should be since
         * we already checked that its only successor is another function. */
        if (!bb)
            bb = p->find_bb_starting(va);
        assert(bb!=NULL);
        assert(1==bb->insns.size());
        Function *thunk = p->add_function(va, SgAsmFunction::FUNC_THUNK);
        p->append(thunk, bb, SgAsmBlock::BLK_ENTRY_POINT);
        ++nfound;

        if (p->debug)
            fprintf(p->debug, "Partitioner::FindThunks: found F%08"PRIx64"\n", va);
    }

    return true;
}

bool
Partitioner::FindInterPadFunctions::operator()(bool enabled, const Args &args)
{
    if (!enabled)
        return false;
    Partitioner *p = args.partitioner;

    /* Initialize the data block ranges once and cache it in this object. */
    if (!padding_ranges) {
        padding_ranges = new DataRangeMap;
        p->padding_extent(padding_ranges);
    }

    /* Range must be immediately preceded by a padding block. */
    if (args.range.first()<=Extent::minimum())
        return true;
    if (padding_ranges->find(args.range.first()-1) == padding_ranges->end())
        return true;

    /* Range must be immediately followed by a padding block. */
    if (args.range.last()>=Extent::maximum())
        return true;
    DataRangeMap::iterator next = padding_ranges->find(args.range.last()+1);
    if (next==padding_ranges->end())
        return true;
    DataBlock *next_dblock = next->second.get();

    /* Create a new function and move the following padding to the new function. */
    Function *new_func = p->add_function(args.range.first(), SgAsmFunction::FUNC_USERDEF);
    p->append(new_func, next_dblock, SgAsmBlock::BLK_PADDING, true/*force*/);
    ++nfound;

    if (p->debug)
        fprintf(p->debug, "Partitioner::FindInterPadFunctions: added F%08"PRIx64"\n", new_func->entry_va);
    return true;
}

bool
Partitioner::FindThunkTables::operator()(bool enabled, const Args &args)
{
    if (!enabled)
        return false;
    Partitioner *p = args.partitioner;
    Extent range = args.range;

    while (!range.empty()) {

        /* Find a single, contiguous thunk table. */
        Disassembler::InstructionMap thunks; // each thunk is a single JMP instruction
        bool in_table = false;
        rose_addr_t va;
        for (va=range.first(); va<=range.last() && (in_table || thunks.empty()); va++) {
            if (begins_contiguously && !in_table && va>args.range.first())
                return true;

            /* Must be a JMP instruction. */
            SgAsmInstruction *insn = p->find_instruction(va);
            SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(insn);
            if (!insn_x86 || (insn_x86->get_kind()!=x86_jmp && insn_x86->get_kind()!=x86_farjmp)) {
                in_table = false;
                continue;
            }

            /* Instruction must not be part of a larger basic block. Be careful not to create basic blocks unecessarily. */
            BasicBlock *bb = p->find_bb_containing(va, false);
            if (bb && bb->insns.size()>1) {
                in_table = false;
                continue;
            }
            if (!bb)
                bb = p->find_bb_starting(va);
            assert(bb!=NULL);

            if (validate_targets) {
                /* Find successors of the JMP instruction. */
                Disassembler::AddressSet succs;
                bool complete;
                if (bb->insns.size()>1) {
                    succs.insert(bb->insns[1]->get_address());
                    complete = true;
                } else {
                    succs = p->successors(bb, &complete);
                }

                /* If the successor is known, it should point to another instruction. */
                bool points_to_insn = true;
                for (Disassembler::AddressSet::iterator si=succs.begin(); si!=succs.end() && points_to_insn; ++si)
                    points_to_insn = NULL != p->find_instruction(*si);
                if (!points_to_insn) {
                    in_table = false;
                    continue;
                }
            }

            /* This is a thunk. Save it and skip ahead to the start of the following instruction. */
            in_table = true;
            thunks[insn->get_address()] = insn;
            va += insn->get_size() - 1; // also incremented by loop
        }

        /* This is only a thunk table if we found enough thunks and (if appropriate) ends at the end of the range. */
        if (thunks.size()>minimum_nthunks && (!ends_contiguously || va==args.range.last()+1)) {
            for (Disassembler::InstructionMap::iterator ii=thunks.begin(); ii!=thunks.end(); ++ii) {
                Function *thunk = p->add_function(ii->first, SgAsmFunction::FUNC_THUNK);
                BasicBlock *bb = p->find_bb_starting(ii->first);
                p->append(thunk, bb, SgAsmBlock::BLK_ENTRY_POINT);
                ++nfound;
                if (p->debug)
                    fprintf(p->debug, "Partitioner::FindThunkTable: thunk F%08"PRIx64"\n", thunk->entry_va);
            }
        }

        range.first(va);
    }
    return true;
}

/** Determines if function is a thunk.  A thunk is a small piece of code (a function) whose only purpose is to branch to
 *  another function.   This predicate should not be confused with the SgAsmFunction::FUNC_THUNK reason bit; the
 *  latter is only an indication of why the function was originally created.  A thunk (as defined by this predicate) might not
 *  have the FUNC_THUNK reason bit set if this function was detected by other means (such as being a target of a function
 *  call). Conversely, a function that has the FUNC_THUNK reason bit set might not qualify as being a thunk by the definition
 *  implemented in this predicate (additional blocks or instructions might have been discovered that disqualify this function
 *  even though it was originally thought to be a thunk). */
bool
Partitioner::is_thunk(Function *func)
{
    assert(func);
    if (1!=func->basic_blocks.size())
        return false;

    BasicBlock *bb = func->basic_blocks.begin()->second;
    if (1!=bb->insns.size())
        return false;

    SgAsmInstruction *insn = bb->insns.front();
    SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(insn);
    if (!insn_x86 || (insn_x86->get_kind()!=x86_jmp && insn_x86->get_kind()!=x86_farjmp))
        return false;

    bool complete;
    Disassembler::AddressSet succs = successors(bb, &complete);
    if (!complete || 1!=succs.size())
        return false;

    rose_addr_t target_va = *succs.begin();
    Functions::iterator fi = functions.find(target_va);
    Function *target_func = fi==functions.end() ? NULL : fi->second;
    if (!target_func)
        return false;

    if (0!=(target_func->reason & SgAsmFunction::FUNC_LEFTOVERS) ||
        0!=(fi->second->reason & SgAsmFunction::FUNC_INTERPAD))
        return false;

    return true;
}

bool
Partitioner::FindPostFunctionInsns::operator()(bool enabled, const Args &args)
{
    if (!enabled)
        return false;
    if (!args.insn_prev || args.insn_begin->get_address()!=args.insn_prev->get_address()+args.insn_prev->get_size())
        return true;
    Partitioner *p = args.partitioner;
    BasicBlock *bb = p->find_bb_containing(args.insn_prev->get_address());
    if (!bb || !bb->function)
        return true;
    Function *func = bb->function;
    if (0!=(func->reason & SgAsmFunction::FUNC_INTERPAD) ||
        0!=(func->reason & SgAsmFunction::FUNC_THUNK))
        return true; // don't append instructions to certain "functions"

    size_t nadded = 0;
    rose_addr_t va = args.insn_begin->get_address();
    for (size_t i=0; i<args.ninsns; i++) {
        bb = p->find_bb_containing(va);
        assert(bb!=NULL); // because we know va is an instruction
        if (!bb->function) {
            if (p->debug) {
                if (0==nadded)
                    fprintf(p->debug, "Partitioner::PostFunctionBlocks: for F%08"PRIx64": added", func->entry_va);
                fprintf(p->debug, " B%08"PRIx64, bb->address());
            }
            p->append(func, bb, SgAsmBlock::BLK_USERDEF, true/*head of CFG subgraph*/);
            func->pending = true;
            ++nadded;
            ++nfound;
        }

        SgAsmInstruction *insn = p->find_instruction(va);
        assert(insn!=NULL);
        va += insn->get_size();
    }
    if (p->debug && nadded)
        fprintf(p->debug, "\n");

    return true;
}

/* class method */
rose_addr_t
Partitioner::value_of(SgAsmValueExpression *e)
{
    if (!e) {
        return 0;
    } else if (isSgAsmByteValueExpression(e)) {
        return isSgAsmByteValueExpression(e)->get_value();
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
            retval = value_of(val) + insn->get_address() + insn->get_size();
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
        if (reloc_section)
            rsects.insert(reloc_section);
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
        assert(insn_x86!=NULL);
        rose_addr_t gotplt_va = get_indirection_addr(insn_x86);

        if (gotplt_va <  elf->get_base_va() + gotplt->get_mapped_preferred_rva() ||
            gotplt_va >= elf->get_base_va() + gotplt->get_mapped_preferred_rva() + gotplt->get_mapped_size())
            continue; /* PLT entry doesn't dereference a value in the .got.plt section */

        /* Find the relocation entry whose offset is the gotplt_rva and use that entry's symbol for the function name. */
        for (std::set<SgAsmElfRelocSection*>::iterator ri=rsects.begin(); ri!=rsects.end() && fi->second->name==""; ri++) {
            SgAsmElfRelocEntryList *entries = (*ri)->get_entries();
            SgAsmElfSymbolSection *symbol_section = isSgAsmElfSymbolSection((*ri)->get_linked_section());
            if (symbol_section) {
                SgAsmElfSymbolList *symbols = symbol_section->get_symbols();
                for (size_t ei=0; ei<entries->get_entries().size() && fi->second->name==""; ei++) {
                    SgAsmElfRelocEntry *rel = entries->get_entries()[ei];
                    if (rel->get_r_offset()==gotplt_va) {
                        unsigned long symbol_idx = rel->get_sym();
                        assert(symbol_idx < symbols->get_symbols().size());
                        SgAsmElfSymbol *symbol = symbols->get_symbols()[symbol_idx];
                        fi->second->name = symbol->get_name()->get_string() + "@plt";
                    }
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
            if (func_heuristics & SgAsmFunction::FUNC_ENTRY_POINT)
                mark_entry_targets(headers[i]);
            if (func_heuristics & SgAsmFunction::FUNC_EH_FRAME)
                mark_eh_frames(headers[i]);
            if (func_heuristics & SgAsmFunction::FUNC_SYMBOL)
                mark_func_symbols(headers[i]);
            if (func_heuristics & SgAsmFunction::FUNC_IMPORT)
                mark_elf_plt_entries(headers[i]);
        }
    }
    if (func_heuristics & SgAsmFunction::FUNC_PATTERN)
        mark_func_patterns();
    if (func_heuristics & SgAsmFunction::FUNC_CALL_INSN)
        mark_call_insns();

    /* Run user-defined function detectors, making sure that the basic block starts are up-to-date for each call. */
    if (func_heuristics & SgAsmFunction::FUNC_USERDEF) {
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
                SgAsmFunction::reason_str(true, func->reason).c_str(),
                func->entry_va, func->name.c_str(), func->entry_va);
    }
    BasicBlock *bb = find_bb_containing(func->entry_va);

    /* If this function's entry block collides with some other function, then truncate that other function's block and
     * subsume part of it into this function. Mark the other function as pending because its block may have new
     * successors now. */
    if (bb && func->entry_va!=bb->address()) {
        assert(bb->function!=func);
        if (debug) fprintf(debug, "[split from B%08"PRIx64, bb->address());
        if (bb->function) {
            if (debug) fprintf(debug, " in F%08"PRIx64" \"%s\"", bb->address(), bb->function->name.c_str());
            bb->function->pending = true;
        }
        if (debug) fprintf(debug, "] ");
        truncate(bb, func->entry_va);
        bb = find_bb_containing(func->entry_va);
        assert(bb!=NULL);
        assert(func->entry_va==bb->address());
    }

    if (bb) {
        append(func, bb, SgAsmBlock::BLK_ENTRY_POINT);
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
Partitioner::discover_blocks(Function *f, rose_addr_t va, unsigned reason)
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
    assert(bb!=NULL);

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
    if (bb->function==f && bb->address()!=f->entry_va)
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
                add_function(va, SgAsmFunction::FUNC_GRAPH);
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

        append(f, bb, reason);
        BasicBlock *target_bb = find_bb_containing(target_va);

        /* Optionally create or add reason flags to called function. */
        Function *new_function = NULL;
        if ((func_heuristics & SgAsmFunction::FUNC_CALL_TARGET)) {
            new_function = add_function(target_va, SgAsmFunction::FUNC_CALL_TARGET);
        } else if (find_function(target_va)!=NULL) {
            find_function(target_va)->reason |= SgAsmFunction::FUNC_CALL_TARGET;
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
                discover_blocks(f, *si, reason);
        }

    } else {
        append(f, bb, reason);
        const Disassembler::AddressSet& suc = successors(bb);
        for (Disassembler::AddressSet::const_iterator si=suc.begin(); si!=suc.end(); ++si) {
            if (*si!=f->entry_va)
                discover_blocks(f, *si, reason);
        }
    }
}

void
Partitioner::discover_blocks(Function *f, unsigned reason)
{
    Disassembler::AddressSet heads = f->heads;
    heads.insert(f->entry_va);
    for (Disassembler::AddressSet::iterator hi=heads.begin(); hi!=heads.end(); ++hi)
        discover_blocks(f, *hi, reason);
}

void
Partitioner::analyze_cfg(SgAsmBlock::Reason reason)
{
    for (size_t pass=1; true; pass++) {
        if (debug) fprintf(debug, "========== Partitioner::analyze_cfg() pass %zu ==========\n", pass);
        progress(debug, "Partitioner: starting %s pass %zu: "
                 "%zu function%s, %zu insn%s assigned to %zu block%s (ave %d insn/blk)\n",
                 stringifySgAsmBlockReason(reason, "BLK_").c_str(), pass,
                 functions.size(), 1==functions.size()?"":"s", insn2block.size(), 1==insn2block.size()?"":"s",
                 basic_blocks.size(), 1==basic_blocks.size()?"":"s",
                 basic_blocks.size()?(int)(1.0*insn2block.size()/basic_blocks.size()+0.5):0);

        /* Analyze function return characteristics. */
        for (BasicBlocks::iterator bi=basic_blocks.begin(); bi!=basic_blocks.end(); ++bi) {
            BasicBlock *bb = bi->second;
            rose_addr_t return_va = canonic_block(bb->last_insn()->get_address() + bb->last_insn()->get_size());
            BasicBlock *return_bb = find_bb_starting(return_va);
            rose_addr_t target_va = NO_TARGET; /*call target*/
            BasicBlock *target_bb = NULL;
            bool succs_complete;
            Disassembler::AddressSet succs = successors(bb, &succs_complete);
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
                target_bb->function && target_bb->function->may_return) {
                if (!return_bb->function) {
                    /* Function A makes a call to function B and we know that function B could return but the return address is
                     * not part of any function.  Mark function A as pending so that we rediscover its blocks. */
                    bb->function->pending = true;
                    if (debug) {
                        fprintf(debug, "  F%08"PRIx64" may return to B%08"PRIx64" in F%08"PRIx64"\n",
                                target_bb->function->entry_va, return_va, bb->function->entry_va);
                    }
                } else if (return_va==target_bb->function->entry_va && !bb->function->may_return) {
                    /* This handles the case when function A's return from B falls through into B. In this case, since B
                     * returns then A also returns.  We mark A as returning and we'll catch A's callers on the next pass.
                     *    function_A:
                     *        ...
                     *        CALL function_B
                     *    function_B:
                     *        RET
                     */
                    bb->function->may_return = true;
                    if (debug) {
                        fprintf(debug, "  Function F%08"PRIx64" may return by virtue of call fall-through at B%08"PRIx64"\n",
                                bb->function->entry_va, bb->address());
                    }
                }
            } else if (bb->function && !bb->function->may_return && !is_function_call(bb, NULL) && succs_complete) {
                for (Disassembler::AddressSet::iterator si=succs.begin(); si!=succs.end() && !bb->function->may_return; ++si) {
                    if (0!=(target_va=*si) && NULL!=(target_bb=find_bb_starting(target_va, false)) &&
                        target_bb->function && target_bb->function!=bb->function &&
                        target_va==target_bb->function->entry_va && target_bb->function->may_return) {
                        /* The block bb isn't a function call, but branches to the entry point of another function.  If that
                         * function returns then so does this one.  This handles situations like:
                         *      function_A:
                         *          ...
                         *          JMP function_B
                         *      ...
                         *      function_B:
                         *          RET
                         * We don't need to set function_A->pending because the reachability of the instruction after its JMP
                         * won't change regardless of whether the "called" function returns (i.e., the return is to the caller
                         * of function_A, not to function_A itself. */
                        bb->function->may_return = true;
                        if (debug) {
                            fprintf(debug, "  F%08"PRIx64" may return by virtue of branching to function F%08"PRIx64
                                    " which may return\n", bb->function->entry_va, target_bb->function->entry_va);
                        }
                    }
                }
            }
        }

        /* Get a list of functions we need to analyze because they're marked as pending. */
        std::vector<Function*> pending;
        for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            assert(fi->second->entry_va==fi->first);
            if (fi->second->pending) {
                fi->second->clear_basic_blocks();
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
                        SgAsmFunction::reason_str(true, pending[i]->reason).c_str(),
                        pending[i]->entry_va, pending[i]->name.c_str(), pass);
            }
            try {
                discover_blocks(pending[i], reason);
            } catch (const AbandonFunctionDiscovery&) {
                /* thrown when discover_blocks() decides it needs to start over on a function */
            }
            if (debug) fprintf(debug, "\n");
        }
    }
}

size_t
Partitioner::detach_thunks()
{
    size_t retval = 0;
    Functions functions = this->functions; // so iterators remain valid inside the loop
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        while (detach_thunk(fi->second))
            ++retval;
    }
    return retval;
}

bool
Partitioner::detach_thunk(Function *func)
{
    /* Don't split functions if it has only zero or one instruction. */
    if (func->basic_blocks.empty())
        return false;
    BasicBlock *entry_bb = find_bb_starting(func->entry_va, false);
    assert(entry_bb!=NULL && entry_bb->function==func);
    if (func->basic_blocks.size()==1 && entry_bb->insns.size()==1)
        return false;

    /* Don't split function whose first instruction is not an x86 JMP. */
    SgAsmx86Instruction *insn_x86 = isSgAsmx86Instruction(entry_bb->insns[0]);
    if (!insn_x86 || (insn_x86->get_kind()!=x86_jmp && insn_x86->get_kind()!=x86_farjmp))
        return false;

    /* The JMP must have a single target. */
    rose_addr_t second_va = 0;
    if (entry_bb->insns.size()>1) {
        second_va = entry_bb->insns[1]->get_address();
    } else {
        bool complete;
        Disassembler::AddressSet succs = successors(entry_bb, &complete);
        if (!complete || succs.size()!=1)
            return false;
        second_va = *(succs.begin());
    }

    /* Don't split the function if the first instruction is a successor of any of the function's blocks. */
    for (BasicBlocks::iterator bi=func->basic_blocks.begin(); bi!=func->basic_blocks.end(); ++bi) {
        BasicBlock *bb = bi->second;
        Disassembler::AddressSet succs = successors(bb, NULL);
        if (std::find(succs.begin(), succs.end(), func->entry_va) != succs.end())
            return false;
    }

    /* Create a new function and transfer everything but the first instruction to the new function. */
    if (debug)
        fprintf(debug, "Partitioner::detach_thunk: detaching thunk F%08"PRIx64" from body F%08"PRIx64"\n",
                func->entry_va, second_va);
    Function *new_func = add_function(second_va, func->reason);
    func->reason |= SgAsmFunction::FUNC_THUNK;
    new_func->heads = func->heads; func->heads.clear(); new_func->heads.erase(func->entry_va);
    new_func->may_return = func->may_return;

    BasicBlocks bblocks = func->basic_blocks;
    for (BasicBlocks::iterator bi=bblocks.begin(); bi!=bblocks.end(); ++bi) {
        if (bi->first==func->entry_va) {
            BasicBlock *new_bb = find_bb_starting(second_va);
            if (new_bb->function) {
                assert(new_bb->function==func);
                remove(func, new_bb);
            }
            append(new_func, new_bb, SgAsmBlock::BLK_ENTRY_POINT);
        } else {
            BasicBlock *bb = bi->second;
            if (bb->function!=new_func) {
                remove(func, bb);
                append(new_func, bb, bb->reason);
            }
        }
    }

    DataBlocks dblocks = func->data_blocks;
    for (DataBlocks::iterator di=dblocks.begin(); di!=dblocks.end(); ++di) {
        DataBlock *dblock = di->second;
        remove(func, dblock);
        append(new_func, dblock, dblock->reason);
    }
    return true;
}

/* Moves padding blocks to correct functions. */
void
Partitioner::adjust_padding()
{
    /* Compute two maps: one for non-padding bytes belonging to functions, and one for the padding bytes. */
    FunctionRangeMap nonpadding_ranges;
    function_extent(&nonpadding_ranges);
    DataRangeMap padding_ranges;
    padding_extent(&padding_ranges);
    for (DataRangeMap::iterator pi=padding_ranges.begin(); pi!=padding_ranges.end(); ++pi)
        nonpadding_ranges.erase(pi->first);

    /* For each padding block, find the closest prior function and make that the owner of the padding. */
    for (DataRangeMap::iterator pi=padding_ranges.begin(); pi!=padding_ranges.end(); ++pi) {
        DataBlock *padding = pi->second.get();
        FunctionRangeMap::iterator npi = nonpadding_ranges.find_prior(pi->first.first());
        if (npi==nonpadding_ranges.end())
            continue;
        Function *func = npi->second.get();
        if (func!=effective_function(padding))
            append(func, padding, padding->reason, true/*force*/);
    }
}

void
Partitioner::post_cfg(SgAsmInterpretation *interp/*=NULL*/)
{
    /* Add unassigned intra-function blocks to the surrounding function.  This needs to come before detecting inter-function
     * padding, otherwise it will also try to add the stuff between the true function and its following padding. */
    if (func_heuristics & SgAsmFunction::FUNC_INTRABLOCK) {
        FindIntraFunctionInsns find_intra_function_insns;
        scan_unassigned_bytes(&find_intra_function_insns);
    }

    /* Detect inter-function padding */
    if (func_heuristics & SgAsmFunction::FUNC_INTERPAD) {
        FindDataPadding cb;
        cb.minimum_nrep = 2;
        cb.maximum_nrep = 1024*1024;
        cb.begins_contiguously = false;
        cb.ends_contiguously = false;

        SgUnsignedCharList pattern;
        pattern.push_back(0x90);                /* x68 NOP */
        cb.patterns.push_back(pattern);

        pattern.clear();
        pattern.push_back(0xcc);                /* x86 INT3 */
        cb.patterns.push_back(pattern);

        /* Scan only executable regions of memory. */
        MemoryMap exe_map = *map;
        exe_map.prune(is_unexecutable);
        scan_interfunc_bytes(&cb, &exe_map);
    }

    /* Find thunks.  First use FindThunkTables, which has a more relaxed definition of a "thunk" but requires some minimum
     * number of consecutive thunks in order to trigger.  Then use FindThunks, which has a strict definition including that the
     * thunk target must be a function.  By running the latter in a loop, we can find thunks that branch to other thunks. */
    if (func_heuristics & SgAsmFunction::FUNC_THUNK) {
        FindThunkTables find_thunk_tables;
        find_thunk_tables.minimum_nthunks = 3; // at least this many JMPs per table
        find_thunk_tables.validate_targets = false;
        scan_unassigned_bytes(&find_thunk_tables, map);
        for (size_t npasses=0; npasses<5; ++npasses) {
            FindThunks find_thunks;
            scan_unassigned_insns(&find_thunks);
            if (0==find_thunks.nfound)
                break;
        }
    }

    /* Find functions that we missed between inter-function padding. */
    FindInterPadFunctions find_interpad_functions;
    scan_unassigned_bytes(&find_interpad_functions, map);

    /* Split thunks off from their jumped-to function.  Not really necessary, but the result is more like other common
     * disassemblers and also more closely matches what would happen if we had debugging information in the executable. */
    if (func_heuristics & SgAsmFunction::FUNC_THUNK)
        detach_thunks();

    /* Run one last analysis of the CFG because we may need to fix some things up after having added more blocks from the
     * post-cfg analyses we did above. If nothing happened above, then analyze_cfg() should be fast. */
    analyze_cfg(SgAsmBlock::BLK_GRAPH2);

    /* Find thunks again.  We might have more things satisfying the relatively strict thunk definition. */
    if (func_heuristics & SgAsmFunction::FUNC_THUNK) {
        for (size_t npasses=0; npasses<5; ++npasses) {
            FindThunks find_thunks;
            scan_unassigned_insns(&find_thunks);
            if (0==find_thunks.nfound)
                break;
        }
    }

    /* Append data to the end(s) of each normal function. */
    FindData find_data;
    scan_unassigned_bytes(&find_data, &ro_map);

    /* Make sure padding is back where it belongs. */
    adjust_padding();

    /* Give existing functions names from symbol tables. Don't create more functions. */
    if (interp) {
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        for (size_t i=0; i<headers.size(); i++) {
            name_plt_entries(headers[i]); // give names to ELF .plt trampolines
        }
    }

    /* Add the BLK_CFGHEAD reason to all blocks that are also in the function's CFG head list.  We do this once here rather
     * than searching the heads list in each pass of analyze_cfg(). */
    for (BasicBlocks::iterator bi=basic_blocks.begin(); bi!=basic_blocks.end(); ++bi) {
        BasicBlock *bb = bi->second;
        if (bb->function && 0==(bb->function->reason & SgAsmFunction::FUNC_LEFTOVERS) &&
            bb->function->heads.find(bb->address())!=bb->function->heads.end())
            bb->reason |= SgAsmBlock::BLK_CFGHEAD;
    }

    progress(debug,
             "Partitioner completed: %zu function%s, %zu insn%s assigned to %zu block%s (ave %d insn/blk)\n",
             functions.size(), 1==functions.size()?"":"s", insn2block.size(), 1==insn2block.size()?"":"s",
             basic_blocks.size(), 1==basic_blocks.size()?"":"s",
             basic_blocks.size()?(int)(1.0*insn2block.size()/basic_blocks.size()+0.5):0);
}

size_t
Partitioner::function_extent(FunctionRangeMap *extents)
{
    size_t retval = 0;
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi)
        retval += function_extent(fi->second, extents);
    return retval;
}

size_t
Partitioner::function_extent(Function *func,
                             FunctionRangeMap *extents/*out*/,
                             rose_addr_t *lo_addr_ptr/*out*/, rose_addr_t *hi_addr_ptr/*out*/)
{
    size_t nnodes=0;
    rose_addr_t lo_addr=0, hi_addr=0;
    std::set<DataBlock*> my_dblocks;

    for (BasicBlocks::iterator bi=func->basic_blocks.begin(); bi!=func->basic_blocks.end(); ++bi) {
        /* Find the extents for all the instructions in this basic block. */
        BasicBlock *bb = bi->second;
        for (std::vector<SgAsmInstruction*>::iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
            rose_addr_t start = (*ii)->get_address();
            size_t size = (*ii)->get_size();
            if (0==nnodes++) {
                lo_addr = start;
                hi_addr = start + size;
            } else {
                lo_addr = std::min(lo_addr, start);
                hi_addr = std::max(hi_addr, start + size);
            }
            if (extents)
                extents->insert(Extent(start, size), func);
        }

        /* Gather data blocks associated with this basic block, but only if they aren't explicitly assigned to a function. */
        for (std::set<DataBlock*>::iterator di=bb->data_blocks.begin(); di!=bb->data_blocks.end(); ++di) {
            if (NULL==(*di)->function)
                my_dblocks.insert(*di);
        }
    }

    /* Gather the data blocks associated with this function. */
    for (DataBlocks::iterator bi=func->data_blocks.begin(); bi!=func->data_blocks.end(); ++bi)
        my_dblocks.insert(bi->second);

    /* Add the extents of all this function's data blocks. */
    for (std::set<DataBlock*>::iterator di=my_dblocks.begin(); di!=my_dblocks.end(); ++di) {
        DataBlock *dblock = *di;
        DataRangeMap data_extents;
        DataRangeMap *data_extents_ptr = extents ? &data_extents : NULL;
        rose_addr_t lo, hi;
        size_t n = datablock_extent(dblock, data_extents_ptr, &lo, &hi);
        if (n>0) {
            if (0==nnodes) {
                lo_addr = lo;
                hi_addr = hi;
            } else {
                lo_addr = std::min(lo_addr, lo);
                hi_addr = std::max(hi_addr, hi);
            }
            nnodes += n;
            if (extents) {
                for (DataRangeMap::iterator di2=data_extents.begin(); di2!=data_extents.end(); ++di2)
                    extents->insert(di2->first, func);
            }
        }
    }

    /* Return values */
    if (lo_addr_ptr)
        *lo_addr_ptr = lo_addr;
    if (hi_addr_ptr)
        *hi_addr_ptr = hi_addr;
    return nnodes;
}

size_t
Partitioner::padding_extent(DataRangeMap *extents/*in,out*/)
{
    size_t nblocks = 0;
    for (DataBlocks::const_iterator di=data_blocks.begin(); di!=data_blocks.end(); ++di) {
        DataBlock *dblock = di->second;
        if (0!=(dblock->reason & SgAsmBlock::BLK_PADDING) && NULL!=effective_function(dblock)) {
            datablock_extent(dblock, extents);
            ++nblocks;
        }
    }
    return nblocks;
}

size_t
Partitioner::datablock_extent(DataRangeMap *extents/*in,out*/)
{
    size_t nblocks = 0;
    for (DataBlocks::const_iterator di=data_blocks.begin(); di!=data_blocks.end(); ++di) {
        DataBlock *dblock = di->second;
        if (NULL!=effective_function(dblock)) {
            datablock_extent(dblock, extents);
            ++nblocks;
        }
    }
    return nblocks;
}

size_t
Partitioner::datablock_extent(DataBlock *db,
                              DataRangeMap *extents/*in,out*/,
                              rose_addr_t *lo_addr_ptr/*out*/, rose_addr_t *hi_addr_ptr/*out*/)
{
    if (db->nodes.empty()) {
        if (lo_addr_ptr)
            *lo_addr_ptr = 0;
        if (hi_addr_ptr)
            *hi_addr_ptr = 0;
    } else {
        rose_addr_t start = db->nodes.front()->get_address();
        size_t size = db->nodes.front()->get_size();
        if (lo_addr_ptr)
            *lo_addr_ptr = start;
        if (hi_addr_ptr)
            *hi_addr_ptr = start+size;
        if (extents)
            extents->insert(Extent(start, size), db);
    }

    for (size_t i=1; i<db->nodes.size(); i++) {
        SgAsmStaticData *node = db->nodes[i];
        rose_addr_t start = node->get_address();
        size_t size = node->get_size();

        if (lo_addr_ptr)
            *lo_addr_ptr = std::min(*lo_addr_ptr, start);
        if (hi_addr_ptr)
            *hi_addr_ptr = std::max(*hi_addr_ptr, start+size);
        if (extents)
            extents->insert(Extent(start, size), db);
    }
    return db->nodes.size();
}

/* The function is contiguous if the stuff between its extent doesn't belong to any other function. */
bool
Partitioner::is_contiguous(Function *func, bool strict)
{
    FunctionRangeMap extents;
    rose_addr_t lo_addr, hi_addr;
    if (0==function_extent(func, &extents, &lo_addr, &hi_addr) || 1==extents.size())
        return true;
    if (strict)
        return false;

    /* Check for instructions belonging to other functions. */
    const rose_addr_t max_insn_size = 16; /* FIXME: This is a kludge, but should work 99% of the time [RPM 2011-09-16] */
    Disassembler::InstructionMap::iterator ii = insns.lower_bound(std::max(lo_addr,max_insn_size)-max_insn_size);
    for (/*void*/; ii!=insns.end() && ii->first<hi_addr; ++ii) {
        if (ii->first>=lo_addr) {
            BasicBlock *bb = find_bb_containing(ii->first, false);
            if (bb && bb->function && bb->function!=func)
                return false;
        }
    }

    /* Check for data belonging to other functions.
     * FIXME: we could use a faster method of doing this! [RPM 2011-09-29] */
    for (DataBlocks::iterator dbi=data_blocks.begin(); dbi!=data_blocks.end(); ++dbi) {
        DataBlock *block = dbi->second;
        Function *block_func = effective_function(block);
        if (block_func!=NULL && block_func!=func) {
            for (size_t i=0; i<block->nodes.size(); i++) {
                if (block->nodes[i]->get_address() < hi_addr &&
                    block->nodes[i]->get_address() + block->nodes[i]->get_size() > lo_addr)
                    return false;
            }
        }
    }

    return true;
}

/* Update SgAsmTarget nodes. */
void
Partitioner::update_targets(SgNode *ast)
{
    typedef std::map<rose_addr_t, SgAsmBlock*> BlockMap;

    /* Build a map from address to SgAsmBlock so we can do lookups quickly. */
    struct BlockMapBuilder: public SgSimpleProcessing {
        BlockMap *block_map;
        BlockMapBuilder(SgNode *ast, BlockMap *block_map): block_map(block_map) {
            traverse(ast, preorder);
        }
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            if (block!=NULL) {
                const SgAsmStatementPtrList &stmts = block->get_statementList();
                SgAsmInstruction *insn = stmts.empty() ? NULL : isSgAsmInstruction(stmts.front());
                if (insn)
                    block_map->insert(std::make_pair(insn->get_address(), block));
            }
        }
    };

    /* Now add block pointers to the successor targets. */
    struct TargetPopulator: public SgSimpleProcessing {
        const BlockMap &block_map;
        TargetPopulator(SgNode *ast, const BlockMap &block_map): block_map(block_map) {
            traverse(ast, preorder);
        }
        void visit(SgNode *node) {
            SgAsmBlock *block = isSgAsmBlock(node);
            if (block) {
                for (size_t i=0; i<block->get_successors().size(); i++) {
                    SgAsmTarget *target = block->get_successors()[i];
                    if (target && NULL==target->get_block()) {
                        BlockMap::const_iterator bi=block_map.find(target->get_address());
                        if (bi!=block_map.end())
                            target->set_block(bi->second);
                    }
                }
            }
        }
    };

    BlockMap block_map;
    BlockMapBuilder(ast, &block_map);
    TargetPopulator(ast, block_map);
}

/* Build the global block containing all functions. */
SgAsmBlock *
Partitioner::build_ast()
{
    /* Build a function to hold all the unassigned instructions.  Update documentation if changing the name of
     * this generated function!  We do this by traversing the instructions and obtaining a basic block for each one.  If the
     * basic block doesn't belong to a function yet, we add it to this special one.  Note that we cannot traverse the list of
     * instructions directly because creating the basic block might cause additional instructions to be created.
     *
     * Do not include the instruction in the leftovers functions if that instruction is completely overlapped by the bytes of
     * an existing, non-leftovers function. */
    Function *catchall = NULL;
    if ((func_heuristics & SgAsmFunction::FUNC_LEFTOVERS)) {

        /* List of all bytes occupied by functions. */
        FunctionRangeMap existing;
        function_extent(&existing);

        /* Repeatedly add unassigned instructions to the leftovers function. */
        bool process_instructions;
        do {
            process_instructions = false;
            Disassembler::InstructionMap insns_copy = insns;
            for (Disassembler::InstructionMap::iterator ii=insns_copy.begin(); ii!=insns_copy.end(); ++ii) {
                rose_addr_t va = ii->first;
                size_t size = ii->second->get_size();
                if (!existing.contains(Extent(va, size))) {
                    BasicBlock *bb = find_bb_containing(ii->first);
                    assert(bb!=NULL);
                    if (!bb->function) {
                        if (!catchall)
                            catchall = add_function(ii->first, SgAsmFunction::FUNC_LEFTOVERS, "***uncategorized blocks***");
                        append(catchall, bb, SgAsmBlock::BLK_LEFTOVERS);
                        process_instructions = true;
                    }
                }
            }
        } while (process_instructions);
    }

    /* Build the AST */
    SgAsmBlock *retval = new SgAsmBlock;
    for (Functions::const_iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        SgAsmFunction *func_decl = build_ast(fi->second);
        if (!func_decl) continue;
        retval->get_statementList().push_back(func_decl);
        func_decl->set_parent(retval);
    }

    /* Return catchall blocks to the free pool */
    if (catchall) {
        catchall->clear_basic_blocks();
        catchall->clear_data_blocks();
        functions.erase(catchall->entry_va);
        delete catchall;
    }

    update_targets(retval);
    return retval;
}

/* Build a function node containing all basic blocks and data blocks of the function. */
SgAsmFunction *
Partitioner::build_ast(Function* f)
{
    if (f->basic_blocks.empty()) {
        if (debug)
            fprintf(debug, "function F%08"PRIx64" \"%s\" has no basic blocks!\n", f->entry_va, f->name.c_str());
        return NULL;
    }

    /* Get the list of basic blocks and data blocks.  We'll want them to be added to the function in order of their starting
     * address, with basic blocks and data blocks interleaved. */
    typedef std::multimap<rose_addr_t, SgAsmStatement*> NodeMap;
    std::set<DataBlock*> my_data_blocks;
    NodeMap nodes;
    BasicBlock *first_basic_block = NULL;
    for (BasicBlocks::iterator bi=f->basic_blocks.begin(); bi!=f->basic_blocks.end(); ++bi) {
        BasicBlock *bblock = bi->second;
        if (!first_basic_block)
            first_basic_block = bblock;

        /* The instructions for this basic block */
        SgAsmStatement *node = build_ast(bblock);
        nodes.insert(std::make_pair(bblock->address(), node));

        /* The data associated with this basic block */
        for (std::set<DataBlock*>::iterator di=bblock->data_blocks.begin(); di!=bblock->data_blocks.end(); ++di) {
            DataBlock *dblock = *di;
            Function *dblock_func = effective_function(dblock);
            if (dblock_func==f)
                my_data_blocks.insert(dblock);
        }
    }

    for (DataBlocks::iterator di=f->data_blocks.begin(); di!=f->data_blocks.end(); ++di) {
        DataBlock *dblock = di->second;
        assert(dblock->function==f);
        my_data_blocks.insert(dblock);
    }

    for (std::set<DataBlock*>::iterator di=my_data_blocks.begin(); di!=my_data_blocks.end(); ++di) {
        DataBlock *dblock = *di;
        SgAsmBlock *ast_block = build_ast(dblock);
        nodes.insert(std::make_pair(dblock->address(), ast_block));
    }

    /* Create the AST function node. */
    SgAsmFunction *retval = new SgAsmFunction;
    retval->set_entry_va(f->entry_va);
    retval->set_name(f->name);
    retval->set_address(first_basic_block->address());
    retval->set_can_return(f->may_return);

    for (NodeMap::iterator ni=nodes.begin(); ni!=nodes.end(); ++ni) {
        retval->get_statementList().push_back(ni->second);
        ni->second->set_parent(retval);
    }

    unsigned reasons = f->reason;
    if (0==(reasons & SgAsmFunction::FUNC_DISCONT)) {
        FunctionRangeMap extent;
        function_extent(f, &extent);
        if (extent.nranges()>1)
            reasons |= SgAsmFunction::FUNC_DISCONT;
    }
    retval->set_reason(reasons);
    return retval;
}

/* Build a basic block node containing all instructions for the basic block. */
SgAsmBlock *
Partitioner::build_ast(BasicBlock* block)
{
    SgAsmBlock *retval = new SgAsmBlock;
    retval->set_id(block->address());
    retval->set_address(block->address());
    retval->set_reason(block->reason);

    for (std::vector<SgAsmInstruction*>::const_iterator ii=block->insns.begin(); ii!=block->insns.end(); ++ii) {
        retval->get_statementList().push_back(*ii);
        (*ii)->set_parent(retval);
    }

    /* Cache block successors so other layers don't have to constantly compute them.  We fill in the successor SgAsmTarget
     * objects with only the address and not pointers to blocks since we don't have all the blocks yet.  The pointers will be
     * initialized in the no-argument version build_ast() higher up on the stack. */
    bool complete;
    Disassembler::AddressSet successor_addrs = successors(block, &complete);
    for (Disassembler::AddressSet::iterator si=successor_addrs.begin(); si!=successor_addrs.end(); ++si)
        retval->get_successors().push_back(new SgAsmTarget(retval, NULL, *si));
    retval->set_successors_complete(complete);
    return retval;
}

/* Buid a data block node for each data block. */
SgAsmBlock *
Partitioner::build_ast(DataBlock *block)
{
    SgAsmBlock *retval = new SgAsmBlock;
    retval->set_id(block->address());
    retval->set_address(block->address());
    retval->set_reason(block->reason);

    for (std::vector<SgAsmStaticData*>::const_iterator ni=block->nodes.begin(); ni!=block->nodes.end(); ++ni) {
        retval->get_statementList().push_back(*ni);
        assert(NULL==(*ni)->get_parent());
        (*ni)->set_parent(retval);
    }
    return retval;
}

/* Top-level function to run the partitioner in passive mode. */
SgAsmBlock *
Partitioner::partition(SgAsmInterpretation* interp/*=NULL*/, const Disassembler::InstructionMap& insns, MemoryMap *map)
{
    disassembler = NULL;
    add_instructions(insns);

    MemoryMap *old_map = get_map();
    MemoryMap old_ro_map = ro_map;
    if (!map && !old_map)
        throw Exception("no memory map");
    if (map)
        set_map(map);

    SgAsmBlock *retval = NULL;
    try {
        pre_cfg(interp);
        analyze_cfg(SgAsmBlock::BLK_GRAPH1);
        post_cfg(interp);
        retval = build_ast();
        set_map(old_map, &old_ro_map);
    } catch (...) {
        set_map(old_map, &old_ro_map);
        throw;
    }

    return retval;
}

/* Top-level function to run the partitioner in active mode. */
SgAsmBlock *
Partitioner::partition(SgAsmInterpretation* interp/*=NULL*/, Disassembler *d, MemoryMap *m)
{
    assert(d!=NULL);
    disassembler = d;
    assert(m!=NULL);
    set_map(m);
    pre_cfg(interp);
    analyze_cfg(SgAsmBlock::BLK_GRAPH1);
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
        rose_addr_t next_va = insn->get_address() + insn->get_size();

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
