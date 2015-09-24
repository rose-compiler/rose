/* Algorithms to detect what instructions make up basic blocks and which blocks make up functions, and how to create the
 * necessary SgAsmBlock and SgAsmFunction IR nodes from this information. */
#include "sage3basic.h"

#include "Diagnostics.h"
#include "Partitioner.h"
#include "Assembler.h"
#include "AssemblerX86.h"
#include "AsmUnparser_compat.h"
#include "BinaryLoader.h"
#include "MemoryCellList.h"
#include "PartialSymbolicSemantics.h"           // FIXME: expensive to compile; remove when no longer needed [RPM 2012-05-06]
#include "stringify.h"

#include "PartialSymbolicSemantics2.h"
#include "DispatcherX86.h"

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <Sawyer/Optional.h>
#include <Sawyer/ProgressBar.h>
#include <stdarg.h>

namespace rose {
namespace BinaryAnalysis {

using namespace Diagnostics;
using namespace StringUtility;

/* See header file for full documentation. */

std::ostream& operator<<(std::ostream &o, const Partitioner::Exception &e)
{
    e.print(o);
    return o;
}

Sawyer::Message::Facility Partitioner::mlog;

// class method
void Partitioner::initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        mlog = Sawyer::Message::Facility("rose::BinaryAnalysis::Partitioner", Diagnostics::destination);
        Diagnostics::mfacilities.insertAndAdjust(mlog);
    }
}

/* class method */
SgAsmInstruction *
Partitioner::isSgAsmInstruction(const Instruction *insn)
{
    return insn ? isSgAsmInstruction(insn->node) : NULL;
}

/* class method */
SgAsmInstruction *
Partitioner::isSgAsmInstruction(SgNode *node)
{
    return ::isSgAsmInstruction(node);
}

/* class method */
SgAsmX86Instruction *
Partitioner::isSgAsmX86Instruction(const Instruction *insn)
{
    return insn ? isSgAsmX86Instruction(insn->node) : NULL;
}

/* class method */
SgAsmX86Instruction *
Partitioner::isSgAsmX86Instruction(SgNode *node)
{
    return ::isSgAsmX86Instruction(node);
}

/* class method */
SgAsmM68kInstruction *
Partitioner::isSgAsmM68kInstruction(const Instruction *insn)
{
    return insn ? isSgAsmM68kInstruction(insn->node) : NULL;
}

/* class method */
SgAsmM68kInstruction *
Partitioner::isSgAsmM68kInstruction(SgNode *node)
{
    return ::isSgAsmM68kInstruction(node);
}

/* Progress report class variables. */
double Partitioner::progress_interval = 10.0;
double Partitioner::progress_time = 0.0;

/* Set progress reporting values. */
void
Partitioner::set_progress_reporting(double min_interval)
{
    progress_interval = min_interval;
}

/* Produce a progress report if enabled. */
void
Partitioner::update_progress(SgAsmBlock::Reason reason, size_t pass) const
{
    if (progress_interval>=0 && mlog[INFO]) {
        double curtime = Sawyer::Message::now();
        if (curtime - progress_time >= progress_interval) {
            mlog[INFO] <<"starting " <<stringifySgAsmBlockReason(reason, "BLK_") <<" pass " <<pass
                       <<": " <<StringUtility::plural(functions.size(), "functions")
                       <<", " <<StringUtility::plural(insns.size(), "instructions")
                       <<", " <<StringUtility::plural(basic_blocks.size(), "blocks")
                       <<"\n";
            progress_time = curtime;
        }
    }
}

struct ProgressSuffix {
    const Partitioner *p;
    ProgressSuffix(): p(NULL) {}
    ProgressSuffix(const Partitioner *p): p(p) {}
    void print(std::ostream &o) const {
        if (p!=NULL) {
            o <<(1==p->basic_blocks.size() ? " block" : " blocks");// label for value printed by ProgressBar
            o <<" " <<StringUtility::plural(p->functions.size(), "functions");
        }
    }
};

std::ostream& operator<<(std::ostream &o, const ProgressSuffix &suffix) {
    suffix.print(o);
    return o;
}

void
Partitioner::update_progress() const
{
    static Sawyer::ProgressBar<size_t, ProgressSuffix> *progressBar = NULL;
    if (!progressBar)
        progressBar = new Sawyer::ProgressBar<size_t, ProgressSuffix>(mlog[MARCH], "");
    progressBar->suffix(ProgressSuffix(this));
    progressBar->value(basic_blocks.size());
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
        } else if (word=="call_target") {
            bits = SgAsmFunction::FUNC_CALL_TARGET;
        } else if (word=="call_insn") {
            bits = SgAsmFunction::FUNC_CALL_INSN;
        } else if (word=="call") {
            bits = SgAsmFunction::FUNC_CALL_TARGET | SgAsmFunction::FUNC_CALL_INSN;
        } else if (word=="eh" || word=="eh_frame") {
            bits = SgAsmFunction::FUNC_EH_FRAME;
        } else if (word=="import") {
            bits = SgAsmFunction::FUNC_IMPORT;
        } else if (word=="export") {
            bits = SgAsmFunction::FUNC_EXPORT;
        } else if (word=="symbol") {
            bits = SgAsmFunction::FUNC_SYMBOL;
        } else if (word=="pattern") {
            bits = SgAsmFunction::FUNC_PATTERN;
        } else if (word=="userdef") {
            bits = SgAsmFunction::FUNC_USERDEF;
        } else if (word=="pad" || word=="padding" || word=="interpad") {
            bits = SgAsmFunction::FUNC_PADDING;
        } else if (word=="intrablock") {
            bits = SgAsmFunction::FUNC_INTRABLOCK;
        } else if (word=="thunk") {
            bits = SgAsmFunction::FUNC_THUNK;
        } else if (word=="misc" || word=="miscellaneous" || word=="interpadfunc") {
            bits = SgAsmFunction::FUNC_MISCMASK;
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
            this->ro_map.require(MemoryMap::READABLE).prohibit(MemoryMap::WRITABLE).keep();
        }
    } else {
        this->ro_map.clear();
    }
}

/* Looks for a jump table. Documented in header file. */
Disassembler::AddressSet
Partitioner::discover_jump_table(BasicBlock *bb, bool do_create, ExtentMap *table_extent)
{
    using namespace BinaryAnalysis::InstructionSemantics2;

    /* Do some cheap up-front checks. */
    SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(bb->last_insn());
    if (!insn_x86 || (insn_x86->get_kind()!=x86_jmp && insn_x86->get_kind()==x86_farjmp) ||
        1!=insn_x86->get_operandList()->get_operands().size())
        return Disassembler::AddressSet();
    SgAsmExpression *target_expr = insn_x86->get_operandList()->get_operands()[0];
    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(target_expr);
    SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(target_expr);
    if (!mre && !rre)
        return Disassembler::AddressSet(); // no indirection

    /* Evaluate the basic block semantically to get an expression for the final EIP. */
    const RegisterDictionary *regdict = RegisterDictionary::dictionary_amd64(); // compatible w/ older x86 models
    const RegisterDescriptor *REG_EIP = regdict->lookup("eip");
    PartialSymbolicSemantics::RiscOperatorsPtr ops = PartialSymbolicSemantics::RiscOperators::instance(regdict);
    BaseSemantics::DispatcherPtr dispatcher = DispatcherX86::instance(ops, 32);
    ops->set_memory_map(&ro_map);
    try {
        for (size_t i=0; i<bb->insns.size(); ++i) {
            insn_x86 = isSgAsmX86Instruction(bb->insns[i]->node);
            ASSERT_not_null(insn_x86); // we know we're in a basic block of x86 instructions already
            dispatcher->processInstruction(insn_x86);
        }
    } catch (...) {
        return Disassembler::AddressSet(); // something went wrong, so just give up (e.g., unhandled instruction)

    }

    /* Scan through memory to find from whence the EIP value came.  There's no need to scan for an EIP which is a known value
     * since such control flow successors would be picked the usual way elsewhere.  It's also quite possible that the EIP value
     * is also stored at some other memory addresses outside the jump table (e.g., a function pointer argument stored on the
     * stack), so we also skip over any memory whose address is known. */
    Disassembler::AddressSet successors;
    BaseSemantics::SValuePtr eip = ops->readRegister(*REG_EIP);
    static const size_t entry_size = 4; // FIXME: bytes per jump table entry
    uint8_t *buf = new uint8_t[entry_size];
    if (!eip->is_number()) {
        BaseSemantics::MemoryCellListPtr mem = BaseSemantics::MemoryCellList::promote(ops->get_state()->get_memory_state());
        for (BaseSemantics::MemoryCellList::CellList::iterator mi=mem->get_cells().begin(); mi!=mem->get_cells().end(); ++mi) {
            BaseSemantics::MemoryCellPtr cell = *mi;
            if (cell->get_address()->is_number() && cell->get_value()->must_equal(eip)) {
                rose_addr_t base_va = cell->get_address()->get_number();
                size_t nentries = 0;
                while (1) {
                    size_t nread = ro_map.readQuick(buf, base_va+nentries*entry_size, entry_size);
                    if (nread!=entry_size)
                        break;
                    rose_addr_t target_va = 0;
                    for (size_t i=0; i<entry_size; i++)
                        target_va |= buf[i] << (i*8);
                    if (!map->at(target_va).require(MemoryMap::EXECUTABLE).exists())
                        break;
                    successors.insert(target_va);
                    ++nentries;
                }
                if (nentries>0) {
                    if (table_extent)
                        table_extent->insert(Extent(base_va, nentries*entry_size));
                    if (do_create) {
                        DataBlock *dblock = find_db_starting(base_va, nentries*entry_size);
                        append(bb, dblock, SgAsmBlock::BLK_JUMPTABLE);
                    }
                    mlog[TRACE] <<"[jump table at " <<addrToString(base_va) <<"+" <<nentries <<"*" <<entry_size <<"]";
                }
            }
        }
    }
    delete [] buf;
    return successors;
}

/** Runs local block analyses if their cached results are invalid and caches the results.  A local analysis is one whose
 *  results only depend on the specified block and which are valid into the future as long as the instructions in the block do
 *  not change. */
void
Partitioner::update_analyses(BasicBlock *bb)
{
    ASSERT_not_null(bb);
    ASSERT_forbid(bb->insns.empty());
    if (bb->valid_cache()) return;

    /* Successor analysis. */
    std::vector<SgAsmInstruction*> inodes;
    for (InstructionVector::const_iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii)
        inodes.push_back(isSgAsmInstruction(*ii));
    bb->cache.sucs = bb->insns.front()->node->getSuccessors(inodes, &(bb->cache.sucs_complete), &ro_map);

    /* Try to handle indirect jumps of the form "jmp ds:[BASE+REGISTER*WORDSIZE]".  The trick is to assume that some kind of
     * jump table exists beginning at address BASE, and that the table contains only addresses of valid code.  All we need to
     * do is look for the first entry in the table that doesn't point into an executable region of the disassembly memory
     * map. We use a "do" loop so the logic nesting doesn't get so deep: just break when we find that something doesn't match
     * what we expect. */
    if (!bb->cache.sucs_complete && bb->cache.sucs.empty()) {
        ExtentMap table_extent;
        Disassembler::AddressSet table_entries = discover_jump_table(bb, true, &table_extent);
        if (!table_entries.empty()) {
            bb->cache.sucs.insert(table_entries.begin(), table_entries.end());
            mlog[TRACE] <<"[jump table at " <<table_extent <<"]";
        }
    }

    // Remove successors for certain kinds of indirect calls (calls made through a register or memory).  If this is an indirect
    // call, then scan through the successors and remove some.  Each successor will be in one of the following categories:
    //   1. a mapped address at which we can make an instruction                                [keep the successor]
    //   2. a mapped address at which we cannot make an instruction, e.g., illegal opcode       [erase the successor]
    //   3. an address which is not mapped, but which might be mapped in the future             [keep the successor]
    //   4. a non-address, e.g., an "ordinal" from a PE Import Address Table                    [erase the successor]
    //
    // For instance, the x86 PE code "call ds:[IAT+X]" is a call to an imported function.  If the dynamic linker hasn't run or
    // been simulated yet, then the Import Address Table entry [IAT+X] could be either an address that isn't mapped, or a
    // non-address "ordinal". We want to erase successors that are ordinals or other garbage, but keep those that are
    // addresses. We keep even the unmapped addresses because the Partitioner might not have the whole picture right now (the
    // usr might run the Partitioner, then simulate dynamic linking, then run another Partitioner on the libraries, then join
    // things together into one large control flow graph).  It's not generally possible to distinguish between ordinals and
    // addresses, but we can use the fact that ordinals are table indices and are therefore probably relatively small.  We also
    // look for indirect calls through registers so that we can support things like "mov edi, ds:[IAT+X]; ...; call edi"
    if (SgAsmX86Instruction *last_insn = isSgAsmX86Instruction(bb->last_insn())) {
        static const rose_addr_t largest_ordinal = 10000;               // arbitrary
        bool is_call = last_insn->get_kind() == x86_call || last_insn->get_kind() == x86_farcall;
        const SgAsmExpressionPtrList &operands = last_insn->get_operandList()->get_operands();
        if (is_call && 1==operands.size() &&
            (isSgAsmRegisterReferenceExpression(operands[0]) || isSgAsmMemoryReferenceExpression(operands[0])) &&
            bb->cache.sucs.size() > 0) {
            for (Disassembler::AddressSet::iterator si=bb->cache.sucs.begin(); si!=bb->cache.sucs.end(); /*void*/) {
                rose_addr_t successor_va = *si;
                if (find_instruction(successor_va)) {                   // category 1
                    ++si;                            
                } else if (map->at(successor_va).exists()) {            // category 2
                    bb->cache.sucs.erase(si++);
                } else if (successor_va > largest_ordinal) {            // category 3
                    ++si;
                } else {                                                // category 4
                    bb->cache.sucs.erase(si++);
                }
            }
        }
    }

    /* Call target analysis. A function call is any CALL-like instruction except when the call target is the fall-through
     * address and the instruction at the fall-through address pops the top of the stack (this is one way position independent
     * code loads the instruction pointer register into a general-purpose register). FIXME: For now we'll assume that any call
     * to the fall-through address is not a function call. */
    rose_addr_t fallthrough_va = bb->last_insn()->get_address() + bb->last_insn()->get_size();
    rose_addr_t target_va = NO_TARGET;
    bool looks_like_call = bb->insns.front()->node->isFunctionCallSlow(inodes, &target_va, NULL);
    if (looks_like_call && target_va!=fallthrough_va) {
        bb->cache.is_function_call = true;
        bb->cache.call_target = target_va;
    } else {
        bb->cache.is_function_call = false;
        bb->cache.call_target = NO_TARGET;
    }

    /* Function return analysis */
    bb->cache.function_return = !bb->cache.sucs_complete &&
                                bb->insns.front()->node->isFunctionReturnSlow(inodes);

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

    // If this block ends with what appears to be a function call then we should perhaps add the fall-through address as a
    // successor.  In fact, since most calls may return, assume that this call also may return unless we already know there's a
    // function there and we haven't yet proven that it may return.
    if (bb->cache.is_function_call) {
        rose_addr_t fall_through_va = canonic_block(bb->last_insn()->get_address() + bb->last_insn()->get_size());
        rose_addr_t call_target_va = call_target(bb);
        if (call_target_va!=NO_TARGET) {
            Instruction *target_insn = find_instruction(call_target_va, true);
            BasicBlock *target_bb = target_insn ? find_bb_starting(call_target_va, false) : NULL;
            if (!target_insn) {
                // We know the call target, but could not obtain an instruction there.  The target might be a dynamically
                // linked function that isn't mapped yet.  Assume it may return since most of them can.
                retval.insert(fall_through_va);
            } else if (target_bb && target_bb->function) {
                // There is a basic block at the call target and the block belongs to a function already. This call may
                // return if the target function may return
                if (target_bb->function->possible_may_return())
                    retval.insert(fall_through_va);
            } else if (target_bb) {
                // There is a basic block at the call target but it hasn't been assigned to a function yet. Assume that it can
                // return since most calls can.
                retval.insert(fall_through_va);
            } else {
                // We were able to disassemble an instruction at the call target, but no basic block exists there yet. Assume
                // that the call may return since most calls can.
                assert(target_insn);
                assert(!target_bb);                     
                retval.insert(fall_through_va);
            }
        } else {
            retval.insert(fall_through_va);             // most calls may return, so assume this one can
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
    using namespace BinaryAnalysis::InstructionSemantics;

    bool on_stack = true; /*assume return value stays on stack; prove otherwise*/

    /* Create the basic block if possible, but if we created it here then we should clear it below. */
    BasicBlock *bb = find_bb_containing(va, false);
    bool preexisting = bb!=NULL;
    if (!bb) bb = find_bb_containing(va);
    if (!bb) return false;
    try {

        SgAsmX86Instruction *last_insn = isSgAsmX86Instruction(bb->last_insn());

        typedef PartialSymbolicSemantics::Policy<> Policy;
        typedef X86InstructionSemantics<Policy, PartialSymbolicSemantics::ValueType> Semantics;
        Policy policy;
        policy.set_map(get_map());
        PartialSymbolicSemantics::ValueType<32> orig_retaddr;
        policy.writeMemory(x86_segreg_ss, policy.readRegister<32>("esp"), orig_retaddr, policy.true_());
        Semantics semantics(policy);

        try {
            for (InstructionVector::iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
                SgAsmX86Instruction *insn = isSgAsmX86Instruction(*ii);
                if (!insn) return false;
                if (insn==last_insn && insn->get_kind()==x86_ret) break;
                semantics.processInstruction(insn);
            }
            on_stack = policy.on_stack(orig_retaddr);
            if (!on_stack)
                mlog[TRACE] <<"[B" <<addrToString(va) <<"#" <<bb->insns.size() <<" discards return address]";
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
    ASSERT_forbid(insns.empty());
    return insns.front()->get_address();
}

/** Returns the first address of a data block.  This might not be the lowest address--it's just the starting address of the
 *  first data node that was added. */
rose_addr_t
Partitioner::DataBlock::address() const
{
    ASSERT_forbid(nodes.empty());
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
Partitioner::Instruction *
Partitioner::BasicBlock::last_insn() const
{
    ASSERT_forbid(insns.empty());
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

/* Move basic blocks from the other function to this one. */
void
Partitioner::Function::move_basic_blocks_from(Function *other)
{
    for (BasicBlocks::iterator bbi=other->basic_blocks.begin(); bbi!=other->basic_blocks.end(); ++bbi) {
        basic_blocks[bbi->first] = bbi->second;
        bbi->second->function = this;
    }
    other->basic_blocks.clear();

    heads.insert(other->heads.begin(), other->heads.end());
    other->heads.clear();
}

/* Move data blocks from the other function to this one. */
void
Partitioner::Function::move_data_blocks_from(Function *other)
{
    for (DataBlocks::iterator dbi=other->data_blocks.begin(); dbi!=other->data_blocks.end(); ++dbi) {
        data_blocks[dbi->first] = dbi->second;
        dbi->second->function = this;
    }
    other->data_blocks.clear();
}

/* Increase knowledge about may-return property. */
void
Partitioner::Function::promote_may_return(SgAsmFunction::MayReturn new_value) {
    switch (get_may_return()) {
        case SgAsmFunction::RET_UNKNOWN:
            set_may_return(new_value);
            break;
        case SgAsmFunction::RET_SOMETIMES:
            if (SgAsmFunction::RET_ALWAYS==new_value || SgAsmFunction::RET_NEVER==new_value)
                set_may_return(new_value);
            break;
        case SgAsmFunction::RET_ALWAYS:
        case SgAsmFunction::RET_NEVER:
            break;
    }
}

void
Partitioner::Function::show_properties(std::ostream &o) const
{
    std::string may_return_str = stringifySgAsmFunctionMayReturn(get_may_return(), "RET_");
    for (size_t i=0; i<may_return_str.size(); ++i)
        may_return_str[i] = tolower(may_return_str[i]);
    o <<"{nbblocks=" <<basic_blocks.size() <<", ndblocks=" <<data_blocks.size() <<", " <<may_return_str;
}

Partitioner::BasicBlock *
Partitioner::Function::entry_basic_block() const
{
    BasicBlocks::const_iterator bi=basic_blocks.find(entry_va);
    return bi==basic_blocks.end() ? NULL : bi->second;
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
     * have pointed to this block have already been deleted. */
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

    /* Delete all instructions by deleting the Partitioner::Instruction objects, but not the underlying SgAsmInstruction
     * objects because the latter might be used by whatever's calling the Partitioner. */
    for (InstructionMap::iterator ii=insns.begin(); ii!=insns.end(); ++ii)
        delete ii->second;
    insns.clear();

    /* Clear all disassembly failures from the cache. */
    clear_disassembler_errors();

    /* Clear statistics and code criteria.  This object manages memory for its statistics, but the caller manages the memory
     * for the code criteria. */
    delete aggregate_mean;           aggregate_mean     = NULL;
    delete aggregate_variance;       aggregate_variance = NULL;
    code_criteria = NULL; // owned by user
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
    ASSERT_not_null(bb);
    ASSERT_require(bb==find_bb_containing(va));

    /* Find the cut point in the instruction vector. I.e., the first instruction to remove from the vector. */
    InstructionVector::iterator cut = bb->insns.begin();
    while (cut!=bb->insns.end() && (*cut)->get_address()!=va) ++cut;
    ASSERT_require(cut!=bb->insns.begin()); /*we can't remove them all since basic blocks are never empty*/

    /* Remove instructions (from the cut point and beyond) and all the data blocks. */
    for (InstructionVector::iterator ii=cut; ii!=bb->insns.end(); ++ii) {
        Instruction *insn = *ii;
        ASSERT_require(insn->bblock==bb);
        insn->bblock = NULL;
    }
    if (cut!=bb->insns.end()) {
        bb->insns.erase(cut, bb->insns.end());
        bb->clear_data_blocks();
    }
}

/* Append instruction to basic block */
void
Partitioner::append(BasicBlock* bb, Instruction* insn)
{
    ASSERT_not_null(bb);
    ASSERT_not_null(insn);
    ASSERT_require2(NULL==insn->bblock, "instruction must not have already belonged to a basic block");
    insn->bblock = bb;
    bb->insns.push_back(insn);
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
    ASSERT_not_null(bb);
    ASSERT_not_null(db);
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
    ASSERT_not_null(f);
    ASSERT_not_null(bb);

    if (keep)
        f->heads.insert(bb->address());
    bb->reason |= reason;

    if (bb->function==f)
        return;

    ASSERT_require(bb->function==NULL);
    bb->function = f;
    f->basic_blocks[bb->address()] = bb;

    /* If the block is a function return then mark the function as returning.  On a transition from a non-returning function
     * to a returning function, we must mark all calling functions as pending so that the fall-through address of their
     * function calls to this function are eventually discovered.  This includes recursive calls since we may have already
     * discovered the recursive call but not followed the fall-through address.  Marking callers as "pending" happens in the
     * analyze_cfg() method, were we can handle all the calls at the same time (more efficient than doing one at a time right
     * here). */
    update_analyses(bb);
    if (bb->cache.function_return)
        f->promote_may_return(SgAsmFunction::RET_SOMETIMES);
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
    ASSERT_not_null(func);
    ASSERT_not_null(block);

    if (force) {
        if (block->function)
            remove(block->function, block);
        if (block->basic_block)
            remove(block->basic_block, block);
    }

    block->reason |= reason;
    if (block->function==func)
        return;

    ASSERT_require(block->function==NULL);
    block->function = func;
    func->data_blocks[block->address()] = block;
}

/** Remove a basic block from a function.  The block and function continue to exist--only the association between them is
 *  broken. */
void
Partitioner::remove(Function* f, BasicBlock* bb)
{
    ASSERT_not_null(f);
    ASSERT_not_null(bb);
    ASSERT_require(bb->function==f);
    bb->function = NULL;
    f->basic_blocks.erase(bb->address());
}

/** Remove a data block from a function. The block and function continue to exist--only the association between them is
 *  broken.  The data block might also be associated with a basic block, in which case the data block will ultimately belong to
 *  the same function as the basic block. */
void
Partitioner::remove(Function *f, DataBlock *db)
{
    ASSERT_not_null(f);
    ASSERT_not_null(db);
    ASSERT_require(db->function==f);
    db->function = NULL;
    f->data_blocks.erase(db->address());
}

/** Remove a data block from a basic block.  The blocks continue to exist--only the association between them is broken.  The
 *  data block might still be associated with a function, in which case it will ultimately end up in that function. */
void
Partitioner::remove(BasicBlock *bb, DataBlock *db)
{
    ASSERT_not_null(bb);
    if (db && db->basic_block==bb) {
        bb->data_blocks.erase(db);
        db->basic_block = NULL;
    }
}

/* Remove instruction from consideration. */
Partitioner::Instruction *
Partitioner::discard(Instruction *insn, bool discard_entire_block)
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
        ASSERT_require(NULL==bb->function);

        /* Remove instructions from the block, returning them to the (implied) list of free instructions. */
        for (InstructionVector::iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
            Instruction *insn = *ii;
            ASSERT_require(insn->bblock==bb);
            insn->bblock = NULL;
        }

        /* Remove the association between data blocks and this basic block. */
        bb->clear_data_blocks();

        /* Remove the block from the partitioner. */
        basic_blocks.erase(bb->address());
        delete bb;
    }
    return NULL;
}

/* Finds (or possibly creates) an instruction beginning at the specified address. */
Partitioner::Instruction *
Partitioner::find_instruction(rose_addr_t va, bool create/*=true*/)
{
    InstructionMap::iterator ii = insns.find(va);
    if (create && disassembler && ii==insns.end() && bad_insns.find(va)==bad_insns.end()) {
        Instruction *insn = NULL;
        try {
            insn = new Instruction(disassembler->disassembleOne(map, va, NULL));
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
    Instruction *insn = find_instruction(va);
    if (!insn)
        return NULL;
    if (!create || insn->bblock!=NULL)
        return insn->bblock;
    update_progress();
    BasicBlock *bb = insn->bblock;
    if (!bb) {
        bb = new BasicBlock;
        basic_blocks.insert(std::make_pair(va, bb));
    }

    while (1) {
        append(bb, insn);

        /* Find address of next instruction, or whether this insn is the end of the block */
        va += insn->get_size();
        if (insn->terminates_basic_block()) { /*naively terminates?*/
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

        /* Get the next instruction */
        insn = find_instruction(va);
        if (!insn || insn->bblock)
            break;
    }
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
    mlog[TRACE] <<"[split from B" <<addrToString(bb->address()) <<"#" <<bb->insns.size() <<"]";
    if (bb->function!=NULL)
        bb->function->pending = true;
    truncate(bb, va);
    bb = find_bb_containing(va);
    ASSERT_not_null(bb);
    ASSERT_require(va==bb->address());
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
        mlog[TRACE] <<"[B" <<addrToString(va) <<"->B" <<addrToString(bb->cache.alias_for) <<"]";
        va = bb->cache.alias_for;
    }
    ASSERT_not_reachable("possible alias loop");
    return va;
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
        ASSERT_require(f->entry_va==entry_va);
        if (reasons & SgAsmFunction::FUNC_MISCMASK)
            f->reason &= ~SgAsmFunction::FUNC_MISCMASK;
        f->reason |= reasons;
        if (name!="") f->name = name;
    }
    return f;
}

/* Do whatever's necessary to finish loading IPD configuration. */
void
Partitioner::mark_ipd_configuration()
{
    using namespace BinaryAnalysis::InstructionSemantics;

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
            char block_name_str[64];
            sprintf(block_name_str, "B%08"PRIx64, va);
            std::string block_name = block_name_str;
            mlog[DEBUG] << "running successors program for " <<block_name_str <<"\n";

            // FIXME: Use a copy (COW) version of the map so we don't need to modify the real map and so that the simulated
            // program can't accidentally modify the stuff being disassembled. [RPM 2012-05-07]
            MemoryMap *map = get_map();
            ASSERT_not_null(map);
            typedef PartialSymbolicSemantics::Policy<> Policy;
            typedef X86InstructionSemantics<Policy, PartialSymbolicSemantics::ValueType> Semantics;
            Policy policy;
            policy.set_map(map);
            Semantics semantics(policy);

            mlog[DEBUG] <<"  running semantics for the basic block...\n";
            for (InstructionVector::iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
                SgAsmX86Instruction *insn = isSgAsmX86Instruction(*ii);
                ASSERT_not_null(insn);
                semantics.processInstruction(insn);
            }

            /* Load the program. Keep at least one unmapped byte between the program text, stack, and svec areas in order to
             * help with debugging. */
            mlog[DEBUG] <<"  loading the program...\n";

            /* Load the instructions to execute */
            rose_addr_t text_va = *map->findFreeSpace(bconf->sucs_program.size(), 4096);
            AddressInterval textInterval = AddressInterval::baseSize(text_va, bconf->sucs_program.size());
            map->insert(textInterval, 
                        MemoryMap::Segment::staticInstance(&bconf->sucs_program[0], bconf->sucs_program.size(),
                                                           MemoryMap::READABLE | MemoryMap::EXECUTABLE,
                                                           "successors program text"));

            /* Create a stack */
            static const size_t stack_size = 8192;
            rose_addr_t stack_va = *map->findFreeSpace(stack_size, stack_size, 4096);
            AddressInterval stackInterval = AddressInterval::baseSize(stack_va, stack_size);
            map->insert(stackInterval,
                        MemoryMap::Segment::anonymousInstance(stack_size, MemoryMap::READABLE|MemoryMap::WRITABLE,
                                                              block_name + " successors stack"));
            rose_addr_t stack_ptr = stack_va + stack_size;

            /* Create an area for the returned vector of successors */
            static const size_t svec_size = 8192;
            rose_addr_t svec_va = *map->findFreeSpace(text_va, svec_size, 4096);
            AddressInterval svecInterval = AddressInterval::baseSize(svec_va, svec_size);
            map->insert(svecInterval,
                        MemoryMap::Segment::anonymousInstance(svec_size, MemoryMap::READABLE|MemoryMap::WRITABLE,
                                                              block_name + " successors vector"));

            /* What is the "return" address. Eventually the successors program will execute a "RET" instruction that will
             * return to this address.  We can choose something arbitrary as long as it doesn't conflict with anything else.
             * We'll use the first byte past the end of the successor program, which gives the added benefit that the
             * successor program doesn't actually have to even return -- it can just fall off the end. */
            rose_addr_t return_va = text_va + bconf->sucs_program.size();
            if (mlog[DEBUG]) {
                mlog[DEBUG] <<"    memory map after program is loaded:\n";
                map->dump(mlog[DEBUG], "      ");
            }

            /* Push arguments onto the stack in reverse order. */
            mlog[DEBUG] <<"  setting up the call frame...\n";

            /* old stack pointer */
            stack_ptr -= 4;
            policy.writeMemory<32>(x86_segreg_ss, policy.number<32>(stack_ptr),
                                   policy.readRegister<32>("esp"), policy.true_());

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
            policy.writeRegister("esp", policy.number<32>(stack_ptr));

            /* Interpret the program */
            mlog[DEBUG] <<"  running the program...\n";
            Disassembler *disassembler = Disassembler::lookup(new SgAsmPEFileHeader(new SgAsmGenericFile()));
            ASSERT_not_null(disassembler);
            policy.writeRegister("eip", policy.number<32>(text_va));
            while (1) {
                rose_addr_t ip = policy.readRegister<32>("eip").known_value();
                if (ip==return_va) break;
                SgAsmX86Instruction *insn = isSgAsmX86Instruction(disassembler->disassembleOne(map, ip));
                mlog[DEBUG] <<"    " <<addrToString(ip) <<": " <<(insn?unparseInstruction(insn):std::string("<null>")) <<"\n";
                ASSERT_not_null(insn);
                semantics.processInstruction(insn);
                ASSERT_require(policy.readRegister<32>("eip").is_known());
                SageInterface::deleteAST(insn);
            }

            /* Extract the list of successors. The number of successors is the first element of the list. */
            mlog[DEBUG] <<"  extracting program return values...\n";
            PartialSymbolicSemantics::ValueType<32> nsucs = policy.readMemory<32>(x86_segreg_ss, policy.number<32>(svec_va),
                                                                                  policy.true_());
            ASSERT_require(nsucs.is_known());
            mlog[DEBUG] <<"    number of successors: " <<nsucs.known_value() <<"\n";
            ASSERT_require(nsucs.known_value()*4 <= svec_size-4); /*first entry is size*/
            for (size_t i=0; i<nsucs.known_value(); i++) {
                PartialSymbolicSemantics::ValueType<32> suc_va = policy.readMemory<32>(x86_segreg_ss,
                                                                                       policy.number<32>(svec_va+4+i*4),
                                                                                       policy.true_());
                if (suc_va.is_known()) {
                    mlog[DEBUG] <<"    #" <<i <<": " <<addrToString(suc_va.known_value()) <<"\n";
                    bb->cache.sucs.insert(suc_va.known_value());
                } else {
                    mlog[DEBUG] <<"    #" <<i <<": unknown\n";
                    bb->cache.sucs_complete = false;
                }
            }

            /* Unmap the program */
            mlog[DEBUG] <<"  unmapping the program...\n";
            map->erase(textInterval);
            map->erase(stackInterval);
            map->erase(svecInterval);

            mlog[DEBUG] <<"  done.\n";
        }
    }
}

/* Marks program entry addresses as functions. */
void
Partitioner::mark_entry_targets(SgAsmGenericHeader *fhdr)
{
    assert(fhdr!=NULL);
    SgRVAList entries = fhdr->get_entry_rvas();

    // Libraries don't have entry addresses
    if ((entries.empty() || (1==entries.size() && 0==entries[0].get_rva())) &&
        SgAsmExecutableFileFormat::PURPOSE_LIBRARY==fhdr->get_exec_format()->get_purpose()) {
        return;
    }

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
        Instruction *insn = find_instruction(plt->get_mapped_actual_va()+plt_offset);
        if (!insn) {
            ++plt_offset;
            continue;
        }
        plt_offset += insn->get_size();
        SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(insn);
        if (!insn_x86) continue;

        rose_addr_t gotplt_va = get_indirection_addr(insn_x86, elf->get_base_va()+gotplt->get_mapped_preferred_rva());
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
            "longjmp@plt"!=name && "__longjmp@plt"!=name && "siglongjmp@plt"!=name) {
            plt_func->set_may_return(SgAsmFunction::RET_ALWAYS);
        } else {
            plt_func->set_may_return(SgAsmFunction::RET_NEVER);
        }
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
                rose_addr_t value = fhdr->get_base_va() + symbol->get_value();
                SgAsmGenericSection *section = symbol->get_bound();

                // Add a function at the symbol's value. If the symbol is bound to a section and the section is mapped at a
                // different address than it expected to be mapped, then adjust the symbol's value by the same amount.
                rose_addr_t va_1 = value;
                if (section!=NULL && section->is_mapped() &&
                    section->get_mapped_preferred_va()!=section->get_mapped_actual_va()) {
                    va_1 += section->get_mapped_actual_va() - section->get_mapped_preferred_va();
                }
                if (find_instruction(va_1))
                    add_function(va_1, SgAsmFunction::FUNC_SYMBOL, symbol->get_name()->get_string());

                // Sometimes weak symbol values are offsets from a section (this code handles that), but other times they're
                // the value is used directly (the above code handled that case). */
                if (section && symbol->get_binding()==SgAsmGenericSymbol::SYM_WEAK)
                    value += section->get_mapped_actual_va();
                if (find_instruction(value))
                    add_function(value, SgAsmFunction::FUNC_SYMBOL, symbol->get_name()->get_string());
            }
        }
    }
}

/* Adds PE exports as function entry points. */
void
Partitioner::mark_export_entries(SgAsmGenericHeader *fhdr)
{
    SgAsmGenericSectionList *sections = fhdr->get_sections();
    for (size_t i=0; i<sections->get_sections().size(); ++i) {
        if (SgAsmPEExportSection *export_section = isSgAsmPEExportSection(sections->get_sections()[i])) {
            const SgAsmPEExportEntryPtrList &exports = export_section->get_exports()->get_exports();
            for (SgAsmPEExportEntryPtrList::const_iterator ei=exports.begin(); ei!=exports.end(); ++ei) {
                rose_addr_t va = (*ei)->get_export_rva().get_va();
                if (find_instruction(va))
                    add_function(va, SgAsmFunction::FUNC_EXPORT, (*ei)->get_name()->get_string());
            }
        }
    }
}

/* Tries to match x86 "(mov rdi,rdi)?; push rbp; mov rbp,rsp" (or the 32-bit equivalent). */
Partitioner::InstructionMap::const_iterator
Partitioner::pattern1(const InstructionMap& insns, InstructionMap::const_iterator first, Disassembler::AddressSet &exclude)
{
    InstructionMap::const_iterator ii = first;
    Disassembler::AddressSet matches;

    /* Look for optional "mov rdi, rdi"; if found, advance ii iterator to fall-through instruction */
    do {
        SgAsmX86Instruction *insn = isSgAsmX86Instruction(ii->second);
        if (!insn || insn->get_kind()!=x86_mov)
            break;
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=2)
            break;
        SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
        if (!rre ||
            rre->get_descriptor().get_major()!=x86_regclass_gpr ||
            rre->get_descriptor().get_minor()!=x86_gpr_di)
            break;
        rre = isSgAsmRegisterReferenceExpression(opands[1]);
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
        SgAsmX86Instruction *insn = isSgAsmX86Instruction(ii->second);
        if (!insn || insn->get_kind()!=x86_push)
            return insns.end();
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=1)
            return insns.end();
        SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
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
        SgAsmX86Instruction *insn = isSgAsmX86Instruction(ii->second);
        if (!insn || insn->get_kind()!=x86_mov)
            return insns.end();
        const SgAsmExpressionPtrList &opands = insn->get_operandList()->get_operands();
        if (opands.size()!=2)
            return insns.end();
        SgAsmRegisterReferenceExpression *rre = isSgAsmRegisterReferenceExpression(opands[0]);
        if (!rre ||
            rre->get_descriptor().get_major()!=x86_regclass_gpr ||
            rre->get_descriptor().get_minor()!=x86_gpr_bp)
            return insns.end();
        rre = isSgAsmRegisterReferenceExpression(opands[1]);
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
/* Tries to match x86 "nop;nop;nop" followed by something that's not a nop. */
Partitioner::InstructionMap::const_iterator
Partitioner::pattern2(const InstructionMap& insns, InstructionMap::const_iterator first, Disassembler::AddressSet &exclude)
{
    InstructionMap::const_iterator ii = first;
    Disassembler::AddressSet matches;

    /* Look for three "nop" instructions */
    for (size_t i=0; i<3; i++) {
        SgAsmX86Instruction *nop = isSgAsmX86Instruction(ii->second);
        if (!nop) return insns.end();
        if (nop->get_kind()!=x86_nop) return insns.end();
        if (nop->get_operandList()->get_operands().size()!=0) return insns.end(); /*only zero-arg NOPs allowed*/
        matches.insert(ii->first);
        ii = insns.find(ii->first + nop->get_size());
        if (ii==insns.end()) return insns.end();
    }

    /* Look for something that's not a "nop"; this is the function entry point. */
    SgAsmX86Instruction *notnop = isSgAsmX86Instruction(ii->second);
    if (!notnop) return insns.end();
    if (notnop->get_kind()==x86_nop) return insns.end();
    matches.insert(ii->first);

    exclude.insert(matches.begin(), matches.end());
    return ii;
}
#endif

#if 0 /* commented out in Partitioner::mark_func_patterns() */
/* Tries to match x86 "leave;ret" followed by one or more "nop" followed by a non-nop */
Partitioner::InstructionMap::const_iterator
Partitioner::pattern3(const InstructionMap& insns, InstructionMap::const_iterator first, Disassembler::AddressSet &exclude)
{
    InstructionMap::const_iterator ii = first;
    Disassembler::AddressSet matches;

    /* leave; ret; nop */
    for (size_t i=0; i<3; i++) {
        SgAsmX86Instruction *insn = isSgAsmX86Instruction(ii->second);
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
        SgAsmX86Instruction *insn = isSgAsmX86Instruction(ii->second);
        if (!insn) return insns.end();
        if (insn->get_kind()!=x86_nop) break;
        matches.insert(ii->first);
        ii = insns.find(ii->first + insn->get_size());
        if (ii==insns.end()) return insns.end();
    }

    /* This must be something that's not a "nop", but make sure it's an x86 instruction anyway. */
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(ii->second);
    if (!insn) return insns.end();
    matches.insert(ii->first);

    exclude.insert(matches.begin(), matches.end());
    return ii;
}
#endif

// class method: Matches an x86 "ENTER xxxx, 0" instruction.
Partitioner::InstructionMap::const_iterator
Partitioner::pattern4(const InstructionMap &insns, InstructionMap::const_iterator first, Disassembler::AddressSet &exclude)
{
    SgAsmX86Instruction *insn = isSgAsmX86Instruction(first->second);
    if (!insn || insn->get_kind()!=x86_enter)
        return insns.end();
    const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
    if (args.size()!=2)
        return insns.end();
    SgAsmIntegerValueExpression *arg = isSgAsmIntegerValueExpression(args[1]);
    if (!arg || 0!=arg->get_absoluteValue())
        return insns.end();

    for (size_t i=0; i<insn->get_size(); ++i)
        exclude.insert(insn->get_address() + i);
    return first;
}

// class method: tries to match m68k "link.w a6, IMM16" where IMM16 is zero or negative
Partitioner::InstructionMap::const_iterator
Partitioner::pattern5(const InstructionMap &insns, InstructionMap::const_iterator first, Disassembler::AddressSet &exclude)
{
    SgAsmM68kInstruction *insn = isSgAsmM68kInstruction(first->second);
    if (!insn || insn->get_kind()!=m68k_link)
        return insns.end();
    const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
    if (args.size()!=2)
        return insns.end();
    SgAsmDirectRegisterExpression *rre = isSgAsmDirectRegisterExpression(args[0]);
    SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(args[1]);
    if (!rre || !ival)
        return insns.end();
    RegisterDescriptor reg = rre->get_descriptor();
    if (reg.get_major()!=m68k_regclass_addr || reg.get_minor()!=6/*link register*/)
        return insns.end();
    int64_t displacement = ival->get_signedValue();
    if (displacement>0)
        return insns.end();

    for (size_t i=0; i<insn->get_size(); ++i)
        exclude.insert(insn->get_address() + i);
    return first;
}

// class method: tries to match m68k instructions: "rts; (trapf)?; lea.l [a7-X], a7"
Partitioner::InstructionMap::const_iterator
Partitioner::pattern6(const InstructionMap &insns, InstructionMap::const_iterator first, Disassembler::AddressSet &exclude)
{
    // rts
    SgAsmM68kInstruction *insn = isSgAsmM68kInstruction(first->second);
    if (!insn || insn->get_kind()!=m68k_rts)
        return insns.end();
    ++first;

    // trapf (padding)
    insn = isSgAsmM68kInstruction(first->second);
    if (insn && insn->get_kind()==m68k_trapf)
        ++first;

    // leal. [a7-X], a7
    insn = isSgAsmM68kInstruction(first->second);
    if (!insn || insn->get_kind()!=m68k_lea || insn->get_operandList()->get_operands().size()!=2)
        return insns.end();
    const SgAsmExpressionPtrList &args = insn->get_operandList()->get_operands();
    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(args[0]);
    SgAsmBinaryAdd *sum = mre ? isSgAsmBinaryAdd(mre->get_address()) : NULL;
    SgAsmDirectRegisterExpression *reg1 = sum ? isSgAsmDirectRegisterExpression(sum->get_lhs()) : NULL;
    SgAsmIntegerValueExpression *addend = sum ? isSgAsmIntegerValueExpression(sum->get_rhs()) : NULL;
    SgAsmDirectRegisterExpression *reg2 = isSgAsmDirectRegisterExpression(args[1]);
    if (!reg1 || reg1->get_descriptor()!=RegisterDescriptor(m68k_regclass_addr, 7, 0, 32) ||
        !reg2 || reg2->get_descriptor()!=RegisterDescriptor(m68k_regclass_addr, 7, 0, 32) ||
        !addend || addend->get_signedValue()>0 || addend->get_signedValue()<4096 /*arbitrary*/)
        return insns.end();

    return first;                                       // the LEA instruction is the start of a function
}

/** Seeds functions according to byte and instruction patterns.  Note that the instruction pattern matcher looks only at
 *  existing instructions--it does not actively disassemble new instructions.  In other words, this matcher is intended mostly
 *  for passive-mode partitioners where the disassembler has already disassembled everything it can. The byte pattern matcher
 *  works whether or not instructions are available. */
void
Partitioner::mark_func_patterns()
{
    // Create functions when we see certain patterns of bytes
    struct T1: ByteRangeCallback {
        Partitioner *p;
        T1(Partitioner *p): p(p) {}
        virtual bool operator()(bool enabled, const Args &args) ROSE_OVERRIDE {
            ASSERT_not_null(args.restrict_map);
            uint8_t buf[4096];
            if (enabled) {
                rose_addr_t va = args.range.first();
                while (va<=args.range.last()) {
                    size_t nbytes = std::min(args.range.last()+1-va, (rose_addr_t)sizeof buf);
                    size_t nread = args.restrict_map->readQuick(buf, va, nbytes);
                    for (size_t i=0; i<nread; ++i) {
                        if (i+5<nread &&                                // x86:
                            0x8b==buf[i+0] && 0xff==buf[i+1] &&         //   mov edi, edi
                            0x55==buf[i+2] &&                           //   push ebp
                            0x8b==buf[i+3] && 0xec==buf[i+4]) {         //   mov ebp, esp
                            p->add_function(va+i, SgAsmFunction::FUNC_PATTERN);
                            i += 4;
                        } else if (i+3<nread &&                         // x86:
                                   0x55==buf[i+0] &&                    //   push ebp
                                   0x8b==buf[i+1] && 0xec==buf[i+2]) {  //   mov ebp, esp
                            p->add_function(va+i, SgAsmFunction::FUNC_PATTERN);
                            i += 2;
                        }
                    }
                    va += nread;
                }
            }
            return enabled;
        }
    } t1(this);
    MemoryMap *mm = get_map();
    if (mm)
        scan_unassigned_bytes(&t1, mm);

    // Create functions when we see certain patterns of instructions. Note that this will only work if we've already
    // disassembled the instructions.
    Disassembler::AddressSet exclude;
    InstructionMap::const_iterator found;

    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        if (exclude.find(ii->first)==exclude.end() &&
            ((found=pattern1(insns, ii, exclude))!=insns.end() ||
             (found=pattern4(insns, ii, exclude))!=insns.end() ||
             (found=pattern5(insns, ii, exclude))!=insns.end() ||
             (found=pattern6(insns, ii, exclude))!=insns.end()))
            add_function(found->first, SgAsmFunction::FUNC_PATTERN);
    }
#if 0 /* Disabled because NOPs sometimes legitimately appear inside functions */
    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        if (exclude.find(ii->first)==exclude.end() && (found=pattern2(insns, ii, exclude))!=insns.end())
            add_function(found->first, SgAsmFunction::FUNC_PATTERN);
    }
#endif
#if 0 /* Disabled because NOPs sometimes follow "leave;ret" for functions with multiple returns. */
    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
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
    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        std::vector<SgAsmInstruction*> iv;
        iv.push_back(ii->second->node);
        rose_addr_t target_va=NO_TARGET;
        if (ii->second->node->isFunctionCallSlow(iv, &target_va, NULL) && target_va!=NO_TARGET &&
            target_va!=ii->first + ii->second->get_size()) {
            add_function(target_va, SgAsmFunction::FUNC_CALL_TARGET, "");
        }
    }
}

/* Scan through ranges of contiguous instructions */
void
Partitioner::scan_contiguous_insns(InstructionMap insns, InsnRangeCallbacks &cblist,
                                   Instruction *prev, Instruction *end)
{
    while (!insns.empty()) {
        Instruction *first = insns.begin()->second;
        rose_addr_t va = first->get_address();
        InstructionMap::iterator ii = insns.find(va);
        InstructionVector contig;
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
    InstructionMap all = this->insns;
    InstructionMap range;
    Instruction *prev = NULL;
    for (InstructionMap::iterator ai=all.begin(); ai!=all.end(); ++ai) {
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
                ASSERT_require(bb_lt && bb_lt->function); // because we're invoked from scan_unassigned_insns
                ASSERT_require(bb_rt && bb_rt->function); // ditto
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
                ASSERT_require(bb_lt && bb_lt->function); // because we're invoked from scan_unassigned_insns
                ASSERT_require(bb_rt && bb_rt->function); // ditto
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
Partitioner::scan_unassigned_bytes(ByteRangeCallbacks &cblist, MemoryMap *restrict_map/*=NULL*/)
{
    if (cblist.empty())
        return;

    /* Get range map for addresses assigned to functions (function instructions and data w/ function pointers). */
    FunctionRangeMap assigned;
    function_extent(&assigned);

    /* Unassigned ranges are the inverse of everything assigned.  Then further restrict the unassigned range map according to
     * the supplied memory map. */
    ExtentMap unassigned = assigned.invert<ExtentMap>();
    if (restrict_map) {
        AddressIntervalSet nonmappedAddrs(*restrict_map);
        nonmappedAddrs.invert();
        ExtentMap toRemove = toExtentMap(nonmappedAddrs);
        unassigned.erase_ranges(toRemove);
    }

    /* Traverse the unassigned map, invoking the callbacks for each range. */
    for (ExtentMap::iterator ri=unassigned.begin(); ri!=unassigned.end(); ++ri)
        cblist.apply(true, ByteRangeCallback::Args(this, restrict_map, assigned, ri->first));
}

void
Partitioner::scan_intrafunc_bytes(ByteRangeCallbacks &cblist, MemoryMap *restrict_map/*=NULL*/)
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
    scan_unassigned_bytes(cblist2, restrict_map);
}

void
Partitioner::scan_interfunc_bytes(ByteRangeCallbacks &cblist, MemoryMap *restrict_map/*=NULL*/)
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
    scan_unassigned_bytes(cblist2, restrict_map);
}

bool
Partitioner::FindDataPadding::operator()(bool enabled, const Args &args)
{
    Stream trace(mlog[TRACE]);
    trace.facilityName("Partitioner::FindDataPadding");
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
    ASSERT_not_null(func);

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
    MemoryMap *map = args.restrict_map ? args.restrict_map : &p->ro_map;
    SgUnsignedCharList buf = map->readVector(range.first(), range.size());
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
            ASSERT_require(psize>0);
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
                ASSERT_not_null(dblock);
                p->append(func, dblock, SgAsmBlock::BLK_PADDING);
                ++nblocks;
                ++nfound;
                if (trace) {
                    if (1==nblocks)
                        trace <<"FindDataPadding for F" <<addrToString(func->entry_va) <<": added";
                    trace <<" D" <<addrToString(range.first());
                }
                range.first(range.first()+nrep*psize-1); // will be incremented after break
                break;
            }
        }
        if (!range.empty())
            range.first(range.first()+1);
    }
    if (trace && nblocks>0)
        trace <<"\n";

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
    ASSERT_not_null(func);

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
    ASSERT_not_null(dblock);
    p->append(func, dblock, SgAsmBlock::BLK_FINDDATA);
    ++nfound;
    mlog[TRACE] <<"FindData: for F" <<addrToString(func->entry_va) <<": added D" <<addrToString(args.range.first()) <<"\n";

    return true;
}

/* Create functions or data for inter-function padding instruction sequences.  Returns true if we did not find interfunction
 * padding and other padding callbacks should proceed; returns false if we did find padding and the others should be skipped. */
bool
Partitioner::FindInsnPadding::operator()(bool enabled, const Args &args)
{
    Stream trace(mlog[TRACE]);
    trace.facilityName("Partitioner::FindInsnPadding");

    if (!enabled)
        return false;
    if (!args.insn_prev)
        return true;
    ASSERT_require(args.ninsns>0);
    ASSERT_not_null(args.insn_prev);
    ASSERT_not_null(args.insn_begin);

    if (begins_contiguously &&
        args.insn_begin->get_address()!=args.insn_prev->get_address()+args.insn_prev->get_size())
        return true;

    /* The preceding function.  We'll add the padding as data to this function, unless we're creating explicity padding
     * functions. */
    Partitioner *p = args.partitioner;
    Function *prev_func = NULL;
    {
        BasicBlock *last_block = p->find_bb_containing(args.insn_prev->get_address(), false);
        ASSERT_not_null(last_block);
        prev_func = last_block->function;
    }

    /* Loop over the inter-function instructions and accumulate contiguous ranges of padding. */
    bool retval = true;
    InstructionVector padding;
    rose_addr_t va = args.insn_begin->get_address();
    Instruction *insn = p->find_instruction(va);
    for (size_t i=0; i<args.ninsns && insn!=NULL; i++) {

        /* Does this instruction match? */
        bool matches = false;
        ASSERT_not_null(insn); // callback is being invoked over instructions
        BasicBlock *bb = p->find_bb_containing(va, false);
        if (bb && bb->function)
            break; // insn is already assigned to a function

        SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(p->find_instruction(va));
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
        ASSERT_forbid(padding.empty());
        if (add_as_data) {
            ASSERT_not_null(prev_func);
            rose_addr_t begin_va = padding.front()->get_address();
            rose_addr_t end_va = padding.back()->get_address() + padding.back()->get_size();
            ASSERT_require(end_va>begin_va);
            size_t size = end_va - begin_va;
            DataBlock *dblock = p->find_db_starting(begin_va, size);
            ASSERT_not_null(dblock);
            p->append(prev_func, dblock, SgAsmBlock::BLK_PADDING);
            for (size_t i=0; i<padding.size(); i++)
                p->discard(padding[i]);
            trace <<"for F" <<addrToString(prev_func->entry_va) <<": added D" <<addrToString(begin_va) <<"\n";
        } else {
            Function *new_func = p->add_function(padding.front()->get_address(), SgAsmFunction::FUNC_PADDING);
            p->find_bb_starting(padding.front()->get_address()); // split first block if necessary
            p->find_bb_starting(va); // split last block if necessary
            trace <<"for F" <<addrToString(new_func->entry_va) <<": added";
            for (size_t i=0; i<padding.size(); i++) {
                BasicBlock *bb = p->find_bb_containing(padding[i]->get_address());
                if (bb && !bb->function) {
                    p->append(new_func, bb, SgAsmBlock::BLK_PADDING, true/*head of CFG subgraph*/);
                    trace <<" B" <<addrToString(bb->address()) <<"#" <<bb->insns.size();
                }
            }
            trace <<"\n";
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

    SgUnsignedCharList raw_bytes = map->readVector(start_va, size, 0);
    ASSERT_require(raw_bytes.size()==size);
    SgAsmStaticData *datum = new SgAsmStaticData;
    datum->set_address(start_va);
    datum->set_raw_bytes(raw_bytes);
    db->nodes.push_back(datum);

    return db;
}

bool
Partitioner::FindFunctionFragments::operator()(bool enabled, const Args &args)
{
    if (!enabled)
        return false;
    Partitioner *p = args.partitioner;

    /* Compute and cache the extents of all known functions. */
    if (!function_extents) {
        function_extents = new FunctionRangeMap;
        p->function_extent(function_extents);
    }

    /* Compute and cache code criteria. */
    if (!code_criteria) {
        RegionStats *mean = p->aggregate_statistics();
        RegionStats *variance = p->get_aggregate_variance();
        code_criteria = p->new_code_criteria(mean, variance, threshold);
    }

    /* This range must begin contiguously with a valid function. */
    if (args.range.first()<=Extent::minimum())
        return true;
    FunctionRangeMap::iterator prev = function_extents->find(args.range.first()-1);
    if (prev==function_extents->end())
        return true;
    Function *func = prev->second.get();
    if (0!=(func->reason & excluded_reasons))
        return true;

    /* Should this range end contiguously with the same function?  Perhaps we should relax this and only require that the
     * preceding function has an address after this range? [RPM 2011-11-25] */
    if (require_intrafunction) {
        if (args.range.last()>=Extent::maximum())
            return true;
        FunctionRangeMap::iterator next = function_extents->find(args.range.last()+1);
        if (next==function_extents->end() || next->second.get()!=func)
            return true;
    }

    /* If the preceding function is interleaved with another then how can we know that the instructions in question should
     * actually belong to this function?  If we're interleaved with one other function, then we could very easily be
     * interleaved with additional functions and this address region could belong to any of them. */
    if (require_noninterleaved && !p->is_contiguous(func, false))
        return true;

    /* Bail unless the region statistically looks like code. */
    ExtentMap pending;
    pending.insert(args.range);
    RegionStats *stats = p->region_statistics(pending);
    double raw_vote;
    if (!code_criteria->satisfied_by(stats, &raw_vote))
        return true;
    
    /* Get the list of basic blocks for the instructions in this range and their address extents.  Bail if a basic block
     * extends beyond the address ranges we're considering, rather than splitting the block.  Also bail if we encounter two or
     * more instructions that overlap since that's a pretty strong indication that this isn't code. */
    std::set<BasicBlock*> bblocks;
    while (!pending.empty()) {
        rose_addr_t va = pending.min();
        BasicBlock *bb = va==args.range.first() ? p->find_bb_starting(va) : p->find_bb_containing(va);
        if (!bb || bb->function)
            return true;
        if (bblocks.find(bb)==bblocks.end()) {
            bblocks.insert(bb);
            for (InstructionVector::iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
                Extent ie((*ii)->get_address(), (*ii)->get_size());
                if (!pending.contains(ie))
                    return true;
                pending.erase(ie);
            }
        }
    }

    /* All looks good.  Add the basic blocks to the preceding function. */
    for (std::set<BasicBlock*>::iterator bi=bblocks.begin(); bi!=bblocks.end(); ++bi) {
        (*bi)->code_likelihood = raw_vote;
        p->append(func, *bi, SgAsmBlock::BLK_FRAGMENT, true/*CFG head*/);
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
        Instruction *insn = p->find_instruction(va);
        ASSERT_not_null(insn);
        next_va = va + insn->get_size();

        /* Instruction must be an x86 JMP */
        SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(insn);
        if (!insn_x86 || (insn_x86->get_kind()!=x86_jmp && insn_x86->get_kind()!=x86_farjmp))
            continue;

        /* Instruction must not be in the middle of an existing basic block. */
        BasicBlock *bb = p->find_bb_containing(va, false);
        if (bb && bb->address()!=va)
            continue;

        if (validate_targets) {
            /* Instruction must have a single successor */
            bool complete;
            Disassembler::AddressSet succs = insn->getSuccessors(&complete);
            if (!complete && 1!=succs.size())
                continue;
            rose_addr_t target_va = *succs.begin();

            /* The target (single successor) must be a known function which is not padding. */
            Functions::iterator fi = p->functions.find(target_va);
            if (fi==p->functions.end() || 0!=(fi->second->reason & SgAsmFunction::FUNC_PADDING))
                continue;
        }

        /* Create the basic block for the JMP instruction.  This block must be a single instruction, which it should be since
         * we already checked that its only successor is another function. */
        if (!bb)
            bb = p->find_bb_starting(va);
        ASSERT_not_null(bb);
        ASSERT_require(1==bb->insns.size());
        Function *thunk = p->add_function(va, SgAsmFunction::FUNC_THUNK);
        p->append(thunk, bb, SgAsmBlock::BLK_ENTRY_POINT);
        ++nfound;

        mlog[TRACE] <<"FindThunks: found F" <<addrToString(va) <<"\n";
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
    Function *new_func = p->add_function(args.range.first(), SgAsmFunction::FUNC_INTERPADFUNC);
    p->append(new_func, next_dblock, SgAsmBlock::BLK_PADDING, true/*force*/);
    ++nfound;

    mlog[TRACE] <<"FindInterPadFunctions: added F" <<addrToString(new_func->entry_va) <<"\n";
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
        InstructionMap thunks; // each thunk is a single JMP instruction
        bool in_table = false;
        rose_addr_t va;
        for (va=range.first(); va<=range.last() && (in_table || thunks.empty()); va++) {
            if (begins_contiguously && !in_table && va>args.range.first())
                return true;

            /* Must be a JMP instruction. */
            Instruction *insn = p->find_instruction(va);
            SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(insn);
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
            ASSERT_not_null(bb);

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
            for (InstructionMap::iterator ii=thunks.begin(); ii!=thunks.end(); ++ii) {
                Function *thunk = p->add_function(ii->first, SgAsmFunction::FUNC_THUNK);
                BasicBlock *bb = p->find_bb_starting(ii->first);
                p->append(thunk, bb, SgAsmBlock::BLK_ENTRY_POINT);
                ++nfound;
                mlog[TRACE] <<"FindThunkTable: thunk F" <<addrToString(thunk->entry_va) <<"\n";
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
    ASSERT_not_null(func);
    if (1!=func->basic_blocks.size())
        return false;

    BasicBlock *bb = func->basic_blocks.begin()->second;
    if (1!=bb->insns.size())
        return false;

    Instruction *insn = bb->insns.front();
    SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(insn);
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
        0!=(fi->second->reason & SgAsmFunction::FUNC_PADDING))
        return false;

    return true;
}

bool
Partitioner::FindPostFunctionInsns::operator()(bool enabled, const Args &args)
{
    Stream trace(mlog[TRACE]);
    trace.facilityName("Partitioner::FindPostFunctionInsns");

    if (!enabled)
        return false;
    if (!args.insn_prev || args.insn_begin->get_address()!=args.insn_prev->get_address()+args.insn_prev->get_size())
        return true;
    Partitioner *p = args.partitioner;
    BasicBlock *bb = p->find_bb_containing(args.insn_prev->get_address());
    if (!bb || !bb->function)
        return true;
    Function *func = bb->function;
    if (0!=(func->reason & SgAsmFunction::FUNC_PADDING) ||
        0!=(func->reason & SgAsmFunction::FUNC_THUNK))
        return true; // don't append instructions to certain "functions"

    size_t nadded = 0;
    rose_addr_t va = args.insn_begin->get_address();
    for (size_t i=0; i<args.ninsns; i++) {
        bb = p->find_bb_containing(va);
        ASSERT_not_null(bb); // because we know va is an instruction
        if (!bb->function) {
            if (trace) {
                if (0==nadded)
                    trace <<"for F" <<addrToString(func->entry_va) <<": added";
                trace <<" B" <<addrToString(bb->address()) <<"#" <<bb->insns.size();
            }
            p->append(func, bb, SgAsmBlock::BLK_POSTFUNC, true/*head of CFG subgraph*/);
            func->pending = true;
            ++nadded;
            ++nfound;
        }

        Instruction *insn = p->find_instruction(va);
        ASSERT_not_null(insn);
        va += insn->get_size();
    }
    if (nadded)
        trace <<"\n";

    return true;
}

/* class method */
rose_addr_t
Partitioner::value_of(SgAsmValueExpression *e)
{
    if (!e) {
        return 0;
    } else if (isSgAsmIntegerValueExpression(e)) {
        return isSgAsmIntegerValueExpression(e)->get_value();
    } else {
        return 0;
    }
}

/* class method */
rose_addr_t
Partitioner::get_indirection_addr(SgAsmInstruction *g_insn, rose_addr_t offset)
{
    rose_addr_t retval = 0;

    SgAsmX86Instruction *insn = isSgAsmX86Instruction(g_insn);
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
        SgAsmRegisterReferenceExpression *reg = isSgAsmRegisterReferenceExpression(mref_bin->get_lhs());
        SgAsmValueExpression *val = isSgAsmValueExpression(mref_bin->get_rhs());
        if (reg!=NULL && val!=NULL) {
            if (reg->get_descriptor().get_major()==x86_regclass_ip) {
                retval = value_of(val) + insn->get_address() + insn->get_size();
            } else if (reg->get_descriptor().get_major()==x86_regclass_gpr) {
                retval = value_of(val) + offset;
            }
        }
    } else if (isSgAsmValueExpression(mref_addr)) {
        retval = value_of(isSgAsmValueExpression(mref_addr));
    }

    return retval; /*calculated value, or defaults to zero*/
}

/** Gives names to dynmaic linking trampolines for ELF.  This method gives names to the dynamic linking trampolines in the .plt
 *  section if the Partitioner detected them as functions. If mark_elf_plt_entries() was called then they all would have been
 *  marked as functions and given names. Otherwise, ROSE might have detected some of them in other ways (like CFG analysis) and
 *  this function will give them names. */
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
        Instruction *insn = find_instruction(func_addr);
        if (NULL==insn)
            continue;

        /* The target in the ".plt" section will be an indirect (through the .got.plt section) jump to the actual dynamically
         * linked function (or to the dynamic linker itself). The .got.plt address is what we're really interested in. */
        SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(insn);
        ASSERT_not_null(insn_x86);
        rose_addr_t gotplt_va = get_indirection_addr(insn_x86, elf->get_base_va() + gotplt->get_mapped_preferred_rva());

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
                        ASSERT_require(symbol_idx < symbols->get_symbols().size());
                        SgAsmElfSymbol *symbol = symbols->get_symbols()[symbol_idx];
                        fi->second->name = symbol->get_name()->get_string() + "@plt";
                    }
                }
            }
        }
    }
}

/** Gives names to dynamic linking thunks for PE.  This method gives names to thunks for imported functions.  The thunks must
 *  have already been detected by the partitioner--this method does not create new functions.  The algorithm scans the list of
 *  unnamed functions looking for functions whose entry instruction is an indirect jump.  When found, check whether the jump is
 *  through a memory address that part of an import address table.  If so, use the corresponding import name as the name of
 *  this function and append "@import".  The "@import" is to distinguish between the actual function whose name is given in the
 *  PE Import Section, and the thunk that jumps to that function.  It's possible to have multiple thunks that all jump to the
 *  same imported function and thus all have the same name. */
void
Partitioner::name_import_entries(SgAsmGenericHeader *fhdr)
{
    /* This function is PE x86 specific */
    SgAsmPEFileHeader *pe = isSgAsmPEFileHeader(fhdr);
    if (!pe)
        return;

    /* Build an index mapping memory addresses to function names.  The addresses are the virtual address through which an
     * indirect jump would go when calling an imported function. */
    struct ImportIndexBuilder: public AstSimpleProcessing {
        typedef std::map<rose_addr_t, std::string> Index;
        typedef Index::iterator iterator;
        Partitioner *partitioner;
        Index index;
        SgAsmGenericHeader *fhdr;
        ImportIndexBuilder(Partitioner *partitioner, SgAsmGenericHeader *fhdr): partitioner(partitioner), fhdr(fhdr) {
            traverse(fhdr, preorder);
        }
        void visit(SgNode *node) {
            SgAsmPEImportItem *import = isSgAsmPEImportItem(node);
            if (import) {
                std::string name = import->get_name()->get_string();
                rose_addr_t va = import->get_bound_rva().get_va();
                if (va!=0 && !name.empty())
                    index[va] = name;
            }
        }
    } imports(this, fhdr);

    /* Look for functions whose first instruction is an indirect jump through one of the memory addresses we indexed above. */
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        Function *func = fi->second;
        if (!func->name.empty())
            continue;
        Instruction *insn = find_instruction(func->entry_va);
        SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(insn);
        if (!insn_x86 ||
            (insn_x86->get_kind()!=x86_jmp && insn_x86->get_kind()!=x86_farjmp) ||
            1!=insn_x86->get_operandList()->get_operands().size())
            continue;
        SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(insn_x86->get_operandList()->get_operands()[0]);
        if (!mre)
            continue;
        SgAsmValueExpression *base = isSgAsmValueExpression(mre->get_address());
        if (!base)
            continue;
        rose_addr_t base_va = value_of(base);

        ImportIndexBuilder::iterator found = imports.index.find(base_va);
        if (found==imports.index.end())
            continue;
        func->name = found->second + "@import";
        mlog[TRACE] <<"name_import_entries: F" <<addrToString(func->entry_va) <<": named \"" <<func->name <<"\"\n";
    }
}

/** Find the addresses for all PE Import Address Tables. Adds them to Partitioner::pe_iat_extents. */
void
Partitioner::find_pe_iat_extents(SgAsmGenericHeader *hdr)
{
    SgAsmGenericSectionPtrList iat_sections = hdr->get_sections_by_name("Import Address Table");
    for (size_t i=0; i<iat_sections.size(); ++i) {
        if (-1==iat_sections[i]->get_id() && iat_sections[i]->is_mapped())
            pe_iat_extents.insert(Extent(iat_sections[i]->get_mapped_actual_va(), iat_sections[i]->get_mapped_size()));
    }
}

/* Seed function starts based on criteria other than control flow graph. */
void
Partitioner::pre_cfg(SgAsmInterpretation *interp/*=NULL*/)
{
    mlog[TRACE] <<"function reasons referenced by Partitioner debugging output:\n"
                <<SgAsmFunction::reason_key("  ");

    mark_ipd_configuration();   /*seed partitioner based on IPD configuration information*/

    if (interp) {
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        for (size_t i=0; i<headers.size(); i++) {
            find_pe_iat_extents(headers[i]);
            if (func_heuristics & SgAsmFunction::FUNC_ENTRY_POINT)
                mark_entry_targets(headers[i]);
            if (func_heuristics & SgAsmFunction::FUNC_EH_FRAME)
                mark_eh_frames(headers[i]);
            if (func_heuristics & SgAsmFunction::FUNC_SYMBOL)
                mark_func_symbols(headers[i]);
            if (func_heuristics & SgAsmFunction::FUNC_IMPORT)
                mark_elf_plt_entries(headers[i]);
            if (func_heuristics & SgAsmFunction::FUNC_EXPORT)
                mark_export_entries(headers[i]);
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
    Stream trace(mlog[TRACE]);
    trace.facilityName("Partitioner::discover_first_block");

    trace <<"1st block " <<SgAsmFunction::reason_str(true, func->reason)
          <<" F" <<addrToString(func->entry_va) <<" \"" <<func->name <<"\": B" <<addrToString(func->entry_va) <<" ";
    BasicBlock *bb = find_bb_containing(func->entry_va);

    /* If this function's entry block collides with some other function, then truncate that other function's block and
     * subsume part of it into this function. Mark the other function as pending because its block may have new
     * successors now. */
    if (bb && func->entry_va!=bb->address()) {
        ASSERT_require(bb->function!=func);
        trace <<"[split from B" <<addrToString(bb->address());
        if (bb->function) {
            trace <<" in F" <<addrToString(bb->address()) <<" \"" <<bb->function->name <<"\"";
            bb->function->pending = true;
        }
        trace <<"] ";
        truncate(bb, func->entry_va);
        bb = find_bb_containing(func->entry_va);
        ASSERT_not_null(bb);
        ASSERT_require(func->entry_va==bb->address());
    } else if (bb && bb->function!=NULL && bb->function!=func) {
        trace <<"[removing B" <<addrToString(func->entry_va) <<" from F" <<addrToString(bb->function->entry_va) <<"]";
        bb->function->pending = true;
        remove(bb->function, bb);
    }

    if (bb) {
        append(func, bb, SgAsmBlock::BLK_ENTRY_POINT);
        trace <<"#" <<bb->insns.size() <<" ";
    } else {
        trace <<"no instruction at function entry address ";
    }
    if (trace) {
        func->show_properties(trace);
        trace <<"\n";
    }
}

/** Discover the basic blocks that belong to the current function. This function recursively adds basic blocks to function @p f
 *  by following the successors of each block.  If a successor is an instruction belonging to some other
 *  function then it's either a function call (if it branches to the entry point of that function) or it's a collision.
 *  Collisions are resolved by discarding and rediscovering the blocks of the other function. */
void
Partitioner::discover_blocks(Function *f, rose_addr_t va, unsigned reason)
{
    Stream trace(mlog[TRACE]);
    trace.facilityName("Partitioner::discover_blocks");
    trace <<" B" <<addrToString(va);

    Instruction *insn = find_instruction(va);
    if (!insn) return; /* No instruction at this address. */
    rose_addr_t target_va = NO_TARGET; /*target of function call instructions (e.g., x86 CALL and FARCALL)*/

    /* This block might be the entry address of a function even before that function has any basic blocks assigned to it. This
     * can happen when a new function was discovered during the current pass. It can't happen for functions discovered in a
     * previous pass since we would have called discover_first_block() by now for any such functions. */
    Functions::iterator fi = functions.find(va);
    if (fi!=functions.end() && fi->second!=f) {
        trace <<"[entry \"" <<fi->second->name <<"\"]";
        return;
    }

    /* Find basic block at address, creating it if necessary. */
    BasicBlock *bb = find_bb_starting(va);
    ASSERT_not_null(bb);
    trace <<"#" <<bb->insns.size();

    /* If the current function has been somehow marked as pending then we might as well give up discovering its blocks because
     * some of its blocks' successors may have changed.  This can happen, for instance, if the create_bb() called above had to
     * split one of this function's blocks. */
    if (f->pending) {
        trace <<" abandon";
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
            trace <<"[entry \"" <<bb->function->name <<"\"]";
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
                trace <<"[conflict F" <<addrToString(bb->function->entry_va) <<" \"" <<bb->function->name <<"\"]";
            } else {
                trace <<"[possible conflict F" <<addrToString(bb->function->entry_va) <<" \"" <<bb->function->name <<"\"]";
            }
            bb->function->pending = f->pending = true;
            trace <<" abandon";
            throw AbandonFunctionDiscovery();
        }
    } else if ((target_va=call_target(bb))!=NO_TARGET) {
        /* Call to a known target */
        trace <<"[call F" <<addrToString(target_va) <<"]";

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

bool
Partitioner::is_pe_dynlink_thunk(Instruction *insn)
{
    SgAsmX86Instruction *insn_x86 = insn ? isSgAsmX86Instruction(insn->node) : NULL;
    if (!insn_x86 || x86_jmp!=insn_x86->get_kind() || insn_x86->get_operandList()->get_operands().size()!=1)
        return false; // not a thunk: wrong instruction
    SgAsmMemoryReferenceExpression *mre = isSgAsmMemoryReferenceExpression(insn_x86->get_operandList()->get_operands()[0]);
    SgAsmIntegerValueExpression *addr = mre ? isSgAsmIntegerValueExpression(mre->get_address()) : NULL;
    if (!addr)
        return false; // not a dynamic linking thunk: wrong addressing mode
    return pe_iat_extents.contains(Extent(addr->get_absoluteValue(), 4));
}

bool
Partitioner::is_pe_dynlink_thunk(BasicBlock *bb)
{
    return bb && bb->insns.size()==1 && is_pe_dynlink_thunk(bb->insns.front());
}

bool
Partitioner::is_pe_dynlink_thunk(Function *func)
{
    return func && func->basic_blocks.size()==1 && is_pe_dynlink_thunk(func->entry_basic_block());
}

void
Partitioner::analyze_cfg(SgAsmBlock::Reason reason)
{
    Stream trace(mlog[TRACE]);
    trace.facilityName("Partitioner::analyze_cfg");
    
    for (size_t pass=1; true; pass++) {
        trace <<"========== Partitioner::analyze_cfg() pass " <<pass <<" ==========\n";
        update_progress(reason, pass);

        /* Analyze function return characteristics. */
        for (BasicBlocks::iterator bi=basic_blocks.begin(); bi!=basic_blocks.end(); ++bi) {
            BasicBlock *bb = bi->second;
            if (!bb->function)
                continue;

            rose_addr_t target_va = NO_TARGET; /*call target*/
            bool iscall = is_function_call(bb, &target_va);
            rose_addr_t return_va = canonic_block(bb->last_insn()->get_address() + bb->last_insn()->get_size()); // fall through
            BasicBlock *return_bb = NULL, *target_bb = NULL; /* computed only if needed since they might split basic blocks */
            bool succs_complete;
            Disassembler::AddressSet succs = successors(bb, &succs_complete);

            if (iscall && target_va!=NO_TARGET &&
                NULL!=(return_bb=find_bb_starting(return_va)) &&
                NULL!=(target_bb=find_bb_starting(target_va, false)) &&
                target_bb->function && target_bb->function->possible_may_return()) {
                if (return_bb->function && return_va==target_bb->function->entry_va && !bb->function->possible_may_return()) {
                    /* This handles the case when function A's return from B falls through into B. In this case, since B
                     * returns then A also returns.  We mark A as returning.
                     *    function_A:
                     *        ...
                     *        CALL function_B
                     *    function_B:
                     *        RET
                     */
                    bb->function->promote_may_return(SgAsmFunction::RET_SOMETIMES);
                    trace <<"  function F" <<addrToString(bb->function->entry_va)
                          <<" may return by virtue of call fall-through at B" <<addrToString(bb->address()) <<"\n";
                }
            } else if (!bb->function->possible_may_return() && !is_function_call(bb, NULL) && succs_complete) {
                for (Disassembler::AddressSet::iterator si=succs.begin();
                     si!=succs.end() && !bb->function->possible_may_return();
                     ++si) {
                    target_va = *si;
                    target_bb = target_va!=0 ? find_bb_starting(target_va, false) : NULL;
                    if (target_bb && target_bb->function && target_bb->function!=bb->function &&
                        target_va==target_bb->function->entry_va && target_bb->function->possible_may_return()) {
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
                        bb->function->promote_may_return(SgAsmFunction::RET_SOMETIMES);
                        trace <<"  F" <<addrToString(bb->function->entry_va)
                              <<" may return by virtue of branching to function F" <<addrToString(target_bb->function->entry_va)
                              <<" which may return\n";
                    }
                }
            } else if (!bb->function->possible_may_return() && !is_function_call(bb, NULL) && !succs_complete) {
                /* If the basic block's successor is not known, then we must assume that it branches to something that could
                 * return. */
                bb->function->promote_may_return(SgAsmFunction::RET_SOMETIMES);
                trace <<"  F" <<addrToString(bb->function->entry_va) <<" may return by virtue of incomplete successors\n";
            }

            // PE dynamic linking thunks are typically placed in the .text section and consist of an indirect jump through one
            // of the import address tables.  If we didn't dynamically link in ROSE, then the IATs probably don't hold valid
            // function addresses, in which case we can't determine if the thunk returns.  Therefore, when this situation
            // happens, we assume that the imported function returns.
            if (!bb->function->possible_may_return() && is_pe_dynlink_thunk(bb->function)) {
                bool invalid_callee_va = !succs_complete;
                for (Disassembler::AddressSet::iterator si=succs.begin(); !invalid_callee_va && si!=succs.end(); ++si)
                    invalid_callee_va = NULL==find_instruction(*si);
                if (invalid_callee_va) {// otherwise we can just analyze the linked-in code
                    bb->function->promote_may_return(SgAsmFunction::RET_SOMETIMES);
                }
            }
        }

        /* Which functions did we think didn't return but now think they might return? */
        Disassembler::AddressSet might_now_return;
        for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            Function *func = fi->second;
            if (func->changed_may_return() && func->possible_may_return()) {
                trace <<(might_now_return.empty()?"newly returning functions:":"") <<" F" <<addrToString(func->entry_va);
                might_now_return.insert(func->entry_va);
                func->commit_may_return();
            }
        }
        if (!might_now_return.empty())
            trace <<"\n";

        /* If we previously thought a function didn't return, but now we think it might return, we need to mark as pending all
         * callers if the return address in that caller isn't already part of the caller function.   There's no need to do this
         * fairly expensive loop of we didn't transition any functions from does-not-return to may-return.  We use the
         * might_now_return set rather than looking up functions with find_function() because the former is probably faster,
         * especially if we have lots of functions but only a few transitioned from does-not-return to may-return, which is the
         * common case. */
        if (!might_now_return.empty()) {
            for (BasicBlocks::iterator bi=basic_blocks.begin(); bi!=basic_blocks.end(); ++bi) {
                BasicBlock *bb = bi->second;
                if (bb->function && !bb->function->pending) {
                    Disassembler::AddressSet succs = successors(bb, NULL);
                    for (Disassembler::AddressSet::iterator si=succs.begin(); si!=succs.end(); ++si) {
                        if (might_now_return.find(*si)!=might_now_return.end()) {
                            // This is a call from a basic block (bb) to a function that we now think might return.  If the
                            // return-to block is not already part of the calling function, then we should mark the calling
                            // function as pending.
                            rose_addr_t return_va = canonic_block(bb->last_insn()->get_address() + bb->last_insn()->get_size());
                            BasicBlock *return_bb = find_bb_starting(return_va, false); // do not create the block
                            if (return_bb && return_bb->function!=bb->function) {
                                bb->function->pending = true;
                                if (trace) {
                                    Function *called_func = find_function(*si); // don't call this unless debugging (performance)
                                    ASSERT_not_null(called_func);
                                    trace <<"newreturn " <<SgAsmFunction::reason_str(true, called_func->reason)
                                          <<" F" <<addrToString(called_func->entry_va) <<" \"" <<called_func->name <<"\""
                                          <<" returns to B" <<addrToString(return_bb->address())
                                          <<" in F" <<addrToString(bb->function->entry_va) <<"\n";
                                }
                            }
                        }
                    }
                }
            }
        }

        /* Get a list of functions we need to analyze because they're marked as pending. */
        std::vector<Function*> pending;
        for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
            ASSERT_require(fi->second->entry_va==fi->first);
            if (fi->second->pending) {
                fi->second->clear_basic_blocks();
                fi->second->pending = false; /*might be set back to true by discover_blocks() in loop below*/
                pending.push_back(fi->second);
            }
        }

        if (pending.size()==0) {
            trace <<"finished for " <<stringifySgAsmBlockReason(reason, "BLK_");
            break;
        }

        /* Make sure all functions have an initial basic block if possible. */
        for (size_t i=0; i<pending.size(); ++i)
            discover_first_block(pending[i]);

        /* (Re)discover each function's blocks starting with the function entry point */
        for (size_t i=0; i<pending.size(); ++i) {
            trace <<"analyzing " <<SgAsmFunction::reason_str(true, pending[i]->reason)
                  <<" F" <<addrToString(pending[i]->entry_va) <<" \"" <<pending[i]->name <<"\" pass " <<pass <<": ";
            try {
                discover_blocks(pending[i], reason);
            } catch (const AbandonFunctionDiscovery&) {
                /* thrown when discover_blocks() decides it needs to start over on a function */
            }
            if (trace) {
                trace <<" ";
                pending[i]->show_properties(trace);
                trace <<"\n";
            }
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
    if (NULL==entry_bb || entry_bb->function!=func)
        return false;
    if (func->basic_blocks.size()==1 && entry_bb->insns.size()==1)
        return false;

    /* Don't split function whose first instruction is not an x86 JMP. */
    SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(entry_bb->insns[0]);
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

    /* The JMP target must be an instruction in the same function. */
    if (BasicBlock *target_bb = find_bb_containing(second_va)) {
        if (target_bb->function!=func)
            return false;
    } else {
        return false;
    }

    /* Don't split the function if the first instruction is a successor of any of the function's blocks. */
    for (BasicBlocks::iterator bi=func->basic_blocks.begin(); bi!=func->basic_blocks.end(); ++bi) {
        BasicBlock *bb = bi->second;
        Disassembler::AddressSet succs = successors(bb, NULL);
        if (std::find(succs.begin(), succs.end(), func->entry_va) != succs.end())
            return false;
    }

    /* Create a new function to hold everything but the entry instruction */
    mlog[TRACE] <<"detach_thunk: detaching thunk F" <<addrToString(func->entry_va)
                <<" from body F" <<addrToString(second_va) <<"\n";
    Function *new_func = add_function(second_va, func->reason);
    new_func->name = func->name;
    new_func->set_may_return(func->get_may_return());

    /* Adjust the old function, which now represents the thunk. */
    func->reason |= SgAsmFunction::FUNC_THUNK;
    func->pending = false;
    if (!func->name.empty() && std::string::npos==func->name.find("-thunk"))
        func->name += "-thunk";
    
    /* Transfer all instructions (except the thunk itself) to new_func. */
    new_func->heads = func->heads;
    func->heads.clear();
    new_func->heads.erase(func->entry_va);
    BasicBlocks bblocks = func->basic_blocks;
    for (BasicBlocks::iterator bi=bblocks.begin(); bi!=bblocks.end(); ++bi) {
        if (bi->first==func->entry_va) {
            BasicBlock *new_bb = find_bb_starting(second_va);
            assert(new_bb!=NULL);
            if (new_bb->function==func) {
                remove(func, new_bb);
                append(new_func, new_bb, SgAsmBlock::BLK_ENTRY_POINT);
            } else if (new_bb->function==new_func) {
                /*void*/
            } else {
                ASSERT_require(NULL==new_bb->function);
                append(new_func, new_bb, SgAsmBlock::BLK_ENTRY_POINT);
            }
            append(new_func, new_bb, SgAsmBlock::BLK_ENTRY_POINT);
        } else {
            BasicBlock *bb = bi->second;
            if (bb->function!=new_func) {
                ASSERT_require(bb->function==func);
                remove(func, bb);
                append(new_func, bb, bb->reason);
            }
        }
    }

    /* Transfer all data blocks to new_func. */
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

/* Merge function fragments when possible. */
void
Partitioner::merge_function_fragments()
{
    Stream trace(mlog[TRACE]);
    trace.facilityName("Partitioner::merge_function_fragments");

    // Find connected components of the control flow graph, but only considering function fragments.  We do this in a single
    // pass, and at the end of this loop each function fragment, F, will have group number group_number[traversal_number[F]].
    typedef std::map<Function*, size_t> TravNumMap;
    TravNumMap traversal_number;       // which DFS traversal first visited the function?
    std::vector<size_t> group_number;  // group number for each traversal (indexed by traversal number)
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        if (SgAsmFunction::FUNC_GRAPH!=fi->second->reason)
            continue; // only consider functions that are strictly fragments
        if (traversal_number.find(fi->second)!=traversal_number.end())
            continue; // we already visited this function

        size_t tnum = group_number.size();
        group_number.push_back(tnum);
        traversal_number[fi->second] = tnum;

        // Depth first search considering only function fragments
        std::vector<Function*> dfs_functions;
        dfs_functions.push_back(fi->second);
        while (!dfs_functions.empty()) {
            Function *source_func = dfs_functions.back(); dfs_functions.pop_back();
            for (BasicBlocks::iterator bi=source_func->basic_blocks.begin(); bi!=source_func->basic_blocks.end(); ++bi) {
                BasicBlock *source_bb = bi->second;
                Disassembler::AddressSet succs = successors(source_bb);
                for (Disassembler::AddressSet::iterator si=succs.begin(); si!=succs.end(); ++si) {
                    BasicBlock *target_bb = find_bb_starting(*si, false); // do not create the block
                    Function *target_func = target_bb ? target_bb->function : NULL;
                    if (target_func && target_func!=source_func && SgAsmFunction::FUNC_GRAPH==target_func->reason) {
                        bool inserted = traversal_number.insert(std::make_pair(target_func, tnum)).second;
                        if (inserted) {
                            dfs_functions.push_back(target_func);
                        } else {
                            group_number[traversal_number[target_func]] = tnum;
                        }
                    }
                }
            }
        }
    }

    /* Reorganize so that we have lists of function fragments by group number. */
    typedef std::vector<std::vector<Function*> > FragmentIndex;
    FragmentIndex fragment_index(group_number.size(), std::vector<Function*>());
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        TravNumMap::iterator tn_found = traversal_number.find(fi->second);
        if (tn_found!=traversal_number.end()) {
            size_t gnum = group_number[tn_found->second];
            fragment_index[gnum].push_back(fi->second);
        }
    }
            
    /* Find the non-fragment predecessors of each fragment group. A fragment group can be merged into another function only if
     * the fragment group has a single predecessor. */
    std::vector<Function*> parent(fragment_index.size(), NULL);
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        Function *source_func = fi->second;
        if (SgAsmFunction::FUNC_GRAPH!=source_func->reason) {
            bool multi_parents = false;
            for (BasicBlocks::iterator bi=source_func->basic_blocks.begin();
                 bi!=source_func->basic_blocks.end() && !multi_parents;
                 ++bi) {
                Disassembler::AddressSet succs = successors(bi->second);
                for (Disassembler::AddressSet::iterator si=succs.begin(); si!=succs.end() && !multi_parents; ++si) {
                    BasicBlock *target_bb = find_bb_starting(*si, false/*do not create*/);
                    Function *target_func = target_bb ? target_bb->function : NULL;
                    TravNumMap::iterator tn_found = target_func ? traversal_number.find(target_func) : traversal_number.end();
                    size_t gnum = tn_found!=traversal_number.end() ? group_number[tn_found->second] : (size_t)(-1);
                    if (gnum!=(size_t)(-1)) {
                        /* source_func (non-fragment) branches to fragment group number <gnum> */
                        if (parent[gnum]) {
                            parent[gnum] = NULL;
                            fragment_index[gnum].clear(); // multiple non-fragment predecessors of this group; discard group
                            multi_parents = true;
                        } else {
                            parent[gnum] = source_func;
                        }
                    }
                }
            }
        }
    }

    /* Merge functions */
    for (size_t gnum=0; gnum<fragment_index.size(); ++gnum) {
        if (parent[gnum]!=NULL && !fragment_index[gnum].empty()) {
            trace <<"fragments " <<SgAsmFunction::reason_str(true, parent[gnum]->reason)
                  <<" F" <<addrToString(parent[gnum]->entry_va) <<" \"" <<parent[gnum]->name <<"\" merging";
            for (std::vector<Function*>::iterator fi=fragment_index[gnum].begin(); fi!=fragment_index[gnum].end(); ++fi) {
                trace <<" F" <<addrToString((*fi)->entry_va);
                merge_functions(parent[gnum], *fi); *fi = NULL;
                parent[gnum]->reason &= ~SgAsmFunction::FUNC_GRAPH;
            }
            if (trace) {
                trace <<" ";
                parent[gnum]->show_properties(trace);
                trace <<"\n";
            }
        }
    }
}

void
Partitioner::merge_functions(Function *parent, Function *other)
{
    parent->reason |= other->reason;

    if (parent->name.empty()) {
        parent->name = other->name;
    } else if (!other->name.empty() && 0!=parent->name.compare(other->name)) {
        parent->name += "+" + other->name;
    }

    parent->move_basic_blocks_from(other);
    parent->move_data_blocks_from(other);

    if (other->pending)
        parent->pending = true;

    parent->promote_may_return(other->get_may_return());

    functions.erase(other->entry_va);
    delete other;
}

/** Mark PE dynamic linking thunks as thunks and give them a name if possible. */
void
Partitioner::name_pe_dynlink_thunks(SgAsmInterpretation *interp/*=NULL*/)
{
    // AST visitor finds PE Import Items and adds their address/name pair to a map.
    struct AddrName: AstSimpleProcessing {
        typedef std::map<rose_addr_t, std::string> NameMap;
        NameMap names;
        SgAsmGenericHeader *hdr;
        AddrName(SgAsmInterpretation *interp) {
            if (interp) {
                const SgAsmGenericHeaderPtrList &hdrs = interp->get_headers()->get_headers();
                for (SgAsmGenericHeaderPtrList::const_iterator hi=hdrs.begin(); hi!=hdrs.end(); ++hi) {
                    hdr = *hi;
                    traverse(hdr, preorder);
                }
            }
        }
        void visit(SgNode *node) {
            if (SgAsmPEImportItem *import_item = isSgAsmPEImportItem(node)) {
                std::string name = import_item->get_name()->get_string();
                if (!name.empty() && !import_item->get_by_ordinal()) {
                    // Add a name for both the absolute virtual address and the relative virtual address. The IAT will contain
                    // relative addresses unless BinaryLoader applied fixups.
                    rose_addr_t va = import_item->get_hintname_rva().get_va();
                    names[va] = name;
                    rose_addr_t rva = va - hdr->get_base_va();
                    names[rva] = name;
                }
            }
        }
        std::string operator()(rose_addr_t va) const {
            NameMap::const_iterator found = names.find(va);
            return found==names.end() ? std::string() : found->second;
        }
    } names(interp);

    // Identify PE dynamic linking thunks and give them the name of the imported function to which they branch.  FIXME: we
    // might want to change the name slightly because otherwise the thunk will have the same name as the linked-in function
    // if/after BinaryLoader does the linking.  In contrast, the ELF executables typically place their dynamic linking
    // thunks in a ".plt" section (Procedure Lookup Table) and we name the thunks so that if the linked-in function is
    // named "printf", the thunk is named "printf@plt".
    for (Functions::iterator fi=functions.begin(); fi!=functions.end(); ++fi) {
        Function *func = fi->second;
        if (is_pe_dynlink_thunk(func)) {
            func->reason |= SgAsmFunction::FUNC_THUNK;
            if (func->name.empty()) {
                BasicBlock *bb = func->entry_basic_block();
                bool complete;
                Disassembler::AddressSet succs = successors(bb, &complete);
                if (complete && 1==succs.size())
                    func->name = names(*succs.begin());
            }
        }
    }
}

Partitioner::Function *
Partitioner::find_function_containing_code(rose_addr_t va)
{
    if (Instruction *insn = find_instruction(va, false  /* do not create */)) {
        if (insn->bblock)
            return insn->bblock->function;
    }
    return NULL;
}

Partitioner::Function *
Partitioner::find_function_containing_data(rose_addr_t va)
{
    DataRangeMap dblock_ranges;
    datablock_extent(&dblock_ranges /*out*/); // FIXME[Robb P. Matzke 2014-04-11]: this is not particularly fast
    DataRangeMap::iterator found = dblock_ranges.find(va);
    if (found!=dblock_ranges.end()) {
        DataBlock *dblock = found->second.get();
        return dblock->function;
    }
    return NULL;
}

Partitioner::Function *
Partitioner::find_function_containing(rose_addr_t va)
{
    if (Function *func = find_function_containing_code(va))
        return func;
    return find_function_containing_data(va);
}

ExtentMap
Partitioner::unused_addresses()
{
    FunctionRangeMap used_addresses;
    function_extent(&used_addresses /*out*/);
    return used_addresses.invert<ExtentMap>();
}

bool
Partitioner::is_used_address(rose_addr_t va)
{
    FunctionRangeMap used_addresses;
    function_extent(&used_addresses /*out*/);
    return used_addresses.find(va) != used_addresses.end();
}

Sawyer::Optional<rose_addr_t>
Partitioner::next_unused_address(const MemoryMap &map, rose_addr_t start_va)
{
    Sawyer::Nothing NOT_FOUND;
    ExtentMap unused = unused_addresses();              // all unused addresses regardless of whether they're mapped

    while (1) {
        // get the next unused virtual address, but it might not be mapped
        ExtentMap::iterator ui = unused.lower_bound(start_va);
        if (ui==unused.end())
            return NOT_FOUND;
        rose_addr_t unused_va = std::max(start_va, ui->first.first());

        // get the next mapped address, but it might not be unused
        rose_addr_t mapped_unused_va = 0;
        if (!map.atOrAfter(unused_va).next().assignTo(mapped_unused_va))
            return NOT_FOUND;                           // no higher mapped address
        if (unused.contains(Extent(mapped_unused_va)))
            return mapped_unused_va;                    // found

        // try again at a higher address
        start_va = mapped_unused_va + 1;
        if (start_va==0)
            return NOT_FOUND;                           // overflow
    }
}

void
Partitioner::discover_post_padding_functions(const MemoryMap &map)
{
    Stream debug = mlog[DEBUG];
    if (map.isEmpty())
        return;

    std::vector<uint8_t> padding_bytes;
    padding_bytes.push_back(0x90);                      // x86 NOP
    padding_bytes.push_back(0xcc);                      // x86 INT3
    padding_bytes.push_back(0);                         // zero padding

    debug <<"discover_post_padding_functions()\n";

    rose_addr_t next_va = map.hull().least();           // first address in the map
    while (1) {
        debug <<"  current position is " <<addrToString(next_va) <<"\n";

        // Find an address that is mapped but not part of any function.
        rose_addr_t unused_va;
        if (!next_unused_address(map, next_va).assignTo(unused_va))
            break;
        debug <<"  next unused address is " <<addrToString(unused_va) <<"\n";

        // Find the next occurrence of padding bytes.
        Extent search_limits = Extent::inin(unused_va, map.hull().greatest());
        rose_addr_t padding_va;
        if (!map.findAny(search_limits, padding_bytes).assignTo(padding_va))
            break;

        // Skip over all padding bytes. After loop, candidate_va is one past end of padding (but possibly not mapped).
        rose_addr_t candidate_va = padding_va;
        while (1) {
            uint8_t byte;
            if (1!=map.at(candidate_va).limit(1).singleSegment().read(&byte).size() ||
                std::find(padding_bytes.begin(), padding_bytes.end(), byte)==padding_bytes.end())
                break;
            ++candidate_va;
        }
        rose_addr_t npadding = candidate_va - padding_va;
        next_va = candidate_va + 1;                     // for next time through this loop
        debug <<"  address after padding is " <<addrToString(candidate_va) <<"\n"
              <<"  number of padding bytes is " <<npadding <<"\n";

        // Only consider this to be padding if we found some minimum number of padding bytes.
        if (npadding < 5)                               // arbitrary
            continue;

        // Make sure we found padding and that the address after the padding is not already part of some function
        if (candidate_va<=padding_va || is_used_address(candidate_va))
            continue;

        // Look at the next few bytes and do some simple tests to see if this looks like code or data.
        if (NULL==find_instruction(candidate_va))
            continue;                                   // can't be a function if there's no instruction
        uint8_t buf[64];                                // arbitrary
        size_t nread = map.readQuick(buf, candidate_va, sizeof buf);
        if (nread < 5)                                  // arbitrary
            continue;                                   // too small to be a function
        size_t nzeros = 0, nprint = 0;
        for (size_t i=0; i<nread; ++i) {
            if (buf[i]==0)
                ++nzeros;
            if (isprint(buf[i]))
                ++nprint;
        }
        if ((double)nzeros / nread > 0.5)               // arbitrary
            continue;                                   // probably data since there are so many zero bytes
        if ((double)nprint / nread > 0.8)               // arbitrary
            continue;                                   // looks like ASCII data
        debug <<"  discovering function at " <<addrToString(candidate_va) <<"\n"
              <<"    nread=" <<nread <<", nzeros=" <<nzeros <<", nprint=" <<nprint <<"\n";

        // Mark the candidate address as a function entry point and discover the basic blocks for this function.
        mlog[TRACE] <<"Partitioner::discover_post_padding_functions: candidate function at "
                    <<addrToString(candidate_va) <<"\n";
        add_function(candidate_va, SgAsmFunction::FUNC_INTERPADFUNC);
        analyze_cfg(SgAsmBlock::BLK_GRAPH2);
    }
    debug <<"  discover_post_padding_functions analysis has completed\n";
}
    
void
Partitioner::post_cfg(SgAsmInterpretation *interp/*=NULL*/)
{
    /* Obtain aggregate statistics over all the functions, and cache them. These statistics describe code, so we want to do
     * this before we add data blocks to the functions.  Any statistics already cached should be considered outdated. */
    clear_aggregate_statistics();
    RegionStats *mean = aggregate_statistics();
    RegionStats *variance = get_aggregate_variance();
    mlog[TRACE] <<"=== Mean ===\n" <<*mean <<"\n"
                <<"=== Variance ===\n" <<*variance <<"\n";

    /* A memory map that contains only the executable regions.  I.e., those that might contain instructions. */
    MemoryMap exe_map = *map;
    exe_map.require(MemoryMap::EXECUTABLE).keep();

    /* Add unassigned intra-function blocks to the surrounding function.  This needs to come before detecting inter-function
     * padding, otherwise it will also try to add the stuff between the true function and its following padding. */
    if (func_heuristics & SgAsmFunction::FUNC_INTRABLOCK) {
        FindFunctionFragments fff;
        fff.require_noninterleaved = true;
        fff.require_intrafunction = true;
        fff.threshold = 0; // treat all intra-function regions as code
        scan_unassigned_bytes(&fff, &exe_map);
    }

#if 0 // [Robb P. Matzke 2014-04-29]: experimental, slow, heuristic, and a bit too greedy, but perhaps more accurate
    // Try to discover a function at each inter-function padding pattern, but interleave the discovery with the search. Each
    // time we find padding we will discover that function (by following its control flow graph) before we search for another
    // inter-function padding pattern.
    if (func_heuristics & SgAsmFunction::FUNC_PADDING)
        discover_post_padding_functions(exe_map);
#endif

    /* Detect inter-function padding */
    if (func_heuristics & SgAsmFunction::FUNC_PADDING) {
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
        exe_map.require(MemoryMap::EXECUTABLE).keep();
        scan_interfunc_bytes(&cb, &exe_map);
    }

    /* Find thunks.  First use FindThunkTables, which has a more relaxed definition of a "thunk" but requires some minimum
     * number of consecutive thunks in order to trigger.  Then use FindThunks, which has a strict definition including that the
     * thunk target must be a function.  By running the latter in a loop, we can find thunks that branch to other thunks. */
    if (func_heuristics & SgAsmFunction::FUNC_THUNK) {
        FindThunkTables find_thunk_tables;
        find_thunk_tables.minimum_nthunks = 3; // at least this many JMPs per table
        find_thunk_tables.validate_targets = false;
        scan_unassigned_bytes(&find_thunk_tables, &exe_map);
        for (size_t npasses=0; npasses<5; ++npasses) {
            FindThunks find_thunks;
            scan_unassigned_insns(&find_thunks);
            if (0==find_thunks.nfound)
                break;
        }
    }

    /* Find functions that we missed between inter-function padding. */
    if (func_heuristics & SgAsmFunction::FUNC_MISCMASK) {
        FindInterPadFunctions find_interpad_functions;
        scan_unassigned_bytes(&find_interpad_functions, &exe_map);
    }

    /* Find code fragments that appear after a function. */
    if (func_heuristics & SgAsmFunction::FUNC_INTRABLOCK) {
        FindFunctionFragments fff;
        fff.require_noninterleaved = false;
        fff.require_intrafunction = false;
        fff.threshold = 0.7;
        scan_unassigned_bytes(&fff, &exe_map);
    }

    /* Run another analysis of the CFG because we may need to fix some things up after having added more blocks from the
     * post-cfg analyses we did above. If nothing happened above, then analyze_cfg() should be fast. */
    analyze_cfg(SgAsmBlock::BLK_GRAPH2);

    /* Split thunks off from their jumped-to function.  Not really necessary, but the result is more like other common
     * disassemblers and also more closely matches what would happen if we had debugging information in the executable.  This
     * should only run after analyze_cfg() because it assumes that a function's blocks have all been discovered -- it does some
     * analysis on the function's internal control flow. */
    if (0!=(func_heuristics & SgAsmFunction::FUNC_THUNK) && detach_thunks()>0)
        analyze_cfg(SgAsmBlock::BLK_GRAPH3);

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

    /* Merge extra functions that we might have created.  Sometimes it's possible that we break a function into too many parts,
     * and we can recombine those parts now. */
    merge_function_fragments();

    /* Give existing functions names from symbol tables. Don't create more functions. */
    if (interp && 0!=(func_heuristics & SgAsmFunction::FUNC_IMPORT)) {
        name_pe_dynlink_thunks(interp);
        const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
        for (size_t i=0; i<headers.size(); i++) {
            name_plt_entries(headers[i]); // give names to ELF .plt trampolines
            name_import_entries(headers[i]); // give names to PE import thunks
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

    mlog[INFO] <<"completed " <<StringUtility::plural(functions.size(), "functions")
               <<", " <<StringUtility::plural(insns.size(), "instructions")
               <<", " <<StringUtility::plural(basic_blocks.size(), "blocks") <<"\n";
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
        for (InstructionVector::iterator ii=bb->insns.begin(); ii!=bb->insns.end(); ++ii) {
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
    InstructionMap::iterator ii = insns.lower_bound(std::max(lo_addr,max_insn_size)-max_insn_size);
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

/* Update CFG edge nodes. */
void
Partitioner::fixup_cfg_edges(SgNode *ast)
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
                    SgAsmIntegerValueExpression *target = block->get_successors()[i];
                    if (target && NULL==target->get_baseNode()) {
                        BlockMap::const_iterator bi=block_map.find(target->get_absoluteValue());
                        if (bi!=block_map.end())
                            target->makeRelativeTo(bi->second);
                    }
                }
            }
        }
    };

    BlockMap block_map;
    BlockMapBuilder(ast, &block_map);
    TargetPopulator(ast, block_map);
}

/* Make pointers relative to what they point into. */
void
Partitioner::fixup_pointers(SgNode *ast, SgAsmInterpretation *interp/*=NULL*/)
{

    struct FixerUpper: public AstPrePostProcessing {
        Partitioner *p;
        SgAsmInterpretation *interp;
        SgAsmInstruction *insn;
        SgAsmGenericSectionPtrList mapped_sections;
        DataRangeMap static_data;

        FixerUpper(Partitioner *p, SgAsmInterpretation *interp)
            : p(p), interp(interp), insn(NULL) {}

        void atTraversalStart() {
            /* Get a list of all memory-mapped sections in the interpretation. */
            if (interp) {
                const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
                for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
                    if ((*hi)->is_mapped())
                        mapped_sections.push_back(*hi);
                    SgAsmGenericSectionPtrList file_sections = (*hi)->get_mapped_sections();
                    mapped_sections.insert(mapped_sections.end(), file_sections.begin(), file_sections.end());
                }
            }

            /* Get a list of all static data blocks */
            p->datablock_extent(&static_data);
        }

        void preOrderVisit(SgNode *node) {
            if (!insn) {
                insn = isSgAsmInstruction(node);
            } else if (isSgAsmIntegerValueExpression(node)) {
                SgAsmIntegerValueExpression *ival = isSgAsmIntegerValueExpression(node);

                /* Don't monkey with constants that are already relative to some other node.  These are things that have been
                 * already fixed up by other methods. */
                if (ival->get_baseNode()!=NULL)
                    return;
                rose_addr_t va = ival->get_absoluteValue();

                /* If this constant is a code pointer, then make the pointer relative to the instruction it points to.  If that
                 * instruction is the entry instruction of a function, then point to the function instead.  A value is
                 * considered a code pointer only if it points to an existing instruction that's contained in a basic block,
                 * and that basic block is part of a valid function.  This constraint weeds out pointers to code that was
                 * disassembled but later discarded. */
                Instruction *target_insn = p->find_instruction(va, false/*do not create*/);
                if (target_insn && target_insn->bblock && target_insn->bblock->function &&
                    0==(target_insn->bblock->function->reason & SgAsmFunction::FUNC_LEFTOVERS)) {
                    SgAsmFunction *target_func = SageInterface::getEnclosingNode<SgAsmFunction>(target_insn->node);
                    if (target_func && target_func->get_entry_va()==target_insn->get_address()) {
                        ival->makeRelativeTo(target_func);
                    } else {
                        ival->makeRelativeTo(target_insn->node);
                    }
                    return;
                }

                /* If this constant points into a static data block, then make it relative to that block. */
                DataRangeMap::iterator dbi = static_data.find(va);
                if (dbi!=static_data.end()) {
                    DataBlock *dblock = dbi->second.get();
                    for (size_t i=0; i<dblock->nodes.size(); ++i) {
                        SgAsmStaticData *sd = dblock->nodes[i];
                        if (va>=sd->get_address() && va<sd->get_address()+sd->get_size()) {
                            ival->makeRelativeTo(sd);
                            return;
                        }
                    }
                }
                
                /* If this constant points into a non-executable data segment, then make the pointer relative to that data
                 * segment. */
                SgAsmGenericSection *section = SgAsmGenericFile::best_section_by_va(mapped_sections, ival->get_absoluteValue());
                if (section && !section->get_mapped_xperm()) {
                    ival->makeRelativeTo(section);
                    return;
                }
            }
        }

        void postOrderVisit(SgNode *node) {
            if (isSgAsmInstruction(node))
                insn = NULL;
        }
    };

    FixerUpper(this, interp).traverse(ast);
}

/* Build the global block containing all functions. */
SgAsmBlock *
Partitioner::build_ast(SgAsmInterpretation *interp/*=NULL*/)
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
            InstructionMap insns_copy = insns;
            for (InstructionMap::iterator ii=insns_copy.begin(); ii!=insns_copy.end(); ++ii) {
                rose_addr_t va = ii->first;
                size_t size = ii->second->get_size();
                if (!existing.contains(Extent(va, size))) {
                    BasicBlock *bb = find_bb_containing(ii->first);
                    ASSERT_not_null(bb);
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

    /* Make pointers relative to the thing into which they point. */
    fixup_cfg_edges(retval);
    fixup_pointers(retval, interp);
    return retval;
}

/* Build a function node containing all basic blocks and data blocks of the function. */
SgAsmFunction *
Partitioner::build_ast(Function* f)
{
    if (f->basic_blocks.empty()) {
        mlog[TRACE] <<"function F" <<addrToString(f->entry_va) <<" \"" <<f->name <<"\" has no basic blocks!\n";
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
        ASSERT_require(dblock->function==f);
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

    /* Set the SgAsmFunction::can_return property.  If we've never indicated that a function might return then assume it
     * doesn't return.  We're all done with analysis now, so it must not return. */
    if (SgAsmFunction::RET_UNKNOWN==f->get_may_return()) {
        retval->set_may_return(SgAsmFunction::RET_NEVER);
    } else {
        retval->set_may_return(f->get_may_return());
    }

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
    retval->set_code_likelihood(block->code_likelihood);

    for (InstructionVector::const_iterator ii=block->insns.begin(); ii!=block->insns.end(); ++ii) {
        Instruction *insn = *ii;
        retval->get_statementList().push_back(insn->node);
        insn->node->set_parent(retval);
    }

    /* Cache block successors so other layers don't have to constantly compute them.  We fill in the successor
     * SgAsmIntegerValueExpression objects with only the address and not pointers to blocks since we don't have all the blocks
     * yet.  The pointers will be initialized in the no-argument version build_ast() higher up on the stack. */
    bool complete;
    Disassembler::AddressSet successor_addrs = successors(block, &complete);
    for (Disassembler::AddressSet::iterator si=successor_addrs.begin(); si!=successor_addrs.end(); ++si) {
        SgAsmIntegerValueExpression *value = SageBuilderAsm::buildValueU64(*si);
        value->set_parent(retval);
        retval->get_successors().push_back(value);
    }
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
        ASSERT_require(NULL==(*ni)->get_parent());
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
        retval = build_ast(interp);
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
    ASSERT_not_null(d);
    disassembler = d;
    ASSERT_not_null(m);
    set_map(m);
    pre_cfg(interp);
    analyze_cfg(SgAsmBlock::BLK_GRAPH1);
    post_cfg(interp);
    return build_ast(interp);
}

void
Partitioner::add_instructions(const Disassembler::InstructionMap& insns)
{
    for (Disassembler::InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        Instruction *insn = new Instruction(ii->second);
        this->insns.insert(std::make_pair(ii->first, insn));
    }
}

Disassembler::InstructionMap
Partitioner::get_instructions() const
{
    Disassembler::InstructionMap retval;
    for (InstructionMap::const_iterator ii=insns.begin(); ii!=insns.end(); ++ii) {
        SgAsmInstruction *insn = ii->second->node;
        retval.insert(std::make_pair(ii->first, insn));
    }
    return retval;
}

// class method
void
Partitioner::disassembleInterpretation(SgAsmInterpretation *interp)
{
    assert(interp!=NULL);
    if (interp->get_global_block())
        return;

    // Map segments into virtual memory if this hasn't been done yet
    MemoryMap *map = interp->get_map();
    if (map==NULL) {
        map = new MemoryMap;
        interp->set_map(map);
        BinaryLoader *loader = BinaryLoader::lookup(interp)->clone();
        loader->remap(interp);
    }

    // Obtain a disassembler based on the type of file we're disassembling and configure it according to the
    // -rose:disassembler_search switches stored in the enclosing SgFile node.
    Disassembler *disassembler = Disassembler::lookup(interp);
    if (!disassembler)
        throw std::runtime_error("no valid disassembler for this interpretation");
    disassembler = disassembler->clone();               // so we can change settings without affecting the registry
    SgFile *file = SageInterface::getEnclosingNode<SgFile>(interp);
    assert(file!=NULL);
    disassembler->set_search(file->get_disassemblerSearchHeuristics());

    // Obtain a partitioner to organize instructions into basic blocks and basic blocks into functions.
    Partitioner *partitioner = new Partitioner();
    partitioner->set_search(file->get_partitionerSearchHeuristics());
    partitioner->load_config(file->get_partitionerConfigurationFileName());

    // Decide what to disassemble. Include at least the entry addresses.
    Disassembler::AddressSet worklist;
    const SgAsmGenericHeaderPtrList &headers = interp->get_headers()->get_headers();
    for (SgAsmGenericHeaderPtrList::const_iterator hi=headers.begin(); hi!=headers.end(); ++hi) {
        SgRVAList entry_rvalist = (*hi)->get_entry_rvas();
        for (size_t i=0; i<entry_rvalist.size(); ++i) {
            rose_addr_t entry_va = (*hi)->get_base_va() + entry_rvalist[i].get_rva();
            worklist.insert(entry_va);
        }
        if (disassembler->get_search() & Disassembler::SEARCH_FUNCSYMS)
            disassembler->search_function_symbols(&worklist, map, *hi);
    }

    // Run the disassembler first to populate the instruction map for the partitioner. This will allow the partitioner to do
    // pattern recognition to find function boundaries if desired.
    Disassembler::BadMap errors;
    Disassembler::InstructionMap insns = disassembler->disassembleBuffer(map, worklist, NULL, &errors);
    partitioner->add_instructions(insns);

    // Organize the instructions into basic blocks and functions. This will call the disassembler to get any additional
    // instructions that are needed (the partitioner does deeper analysis than the disassembler).
    if (SgAsmBlock *block = partitioner->partition(interp, disassembler, map)) {
        interp->set_global_block(block);
        block->set_parent(interp);
    }

    delete partitioner;
    delete disassembler;
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
                SgAsmX86Instruction *insn_x86 = isSgAsmX86Instruction(insn);
                SgAsmX86Instruction *insn2_x86 = isSgAsmX86Instruction(found->second);
                rose_addr_t branch_target_va;
                if (insn_x86 &&
                    (insn_x86->get_kind()==x86_call || insn_x86->get_kind()==x86_farcall) &&
                    insn->getBranchTarget(&branch_target_va) &&
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
        Disassembler::AddressSet successors = insn->getSuccessors(&complete);
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

} // namespace
} // namespace
