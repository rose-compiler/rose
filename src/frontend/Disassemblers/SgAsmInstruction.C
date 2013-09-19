/* SgAsmInstruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file)
 * because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"

/** Control flow successors for a single instruction.  The return value does not consider neighboring instructions, and
 *  therefore is quite naive.  It returns only the information it can glean from this single instruction.  If the returned set
 *  of virtual instructions is fully known then the @p complete argument will be set to true, otherwise false.  The base class
 *  implementation always aborts()--it must be defined in an architecture-specific subclass (pure virtual is not possible due
 *  to ROSETTA). */
std::set<rose_addr_t>
SgAsmInstruction::get_successors(bool *complete) {
    abort();
    // tps (12/9/2009) : MSC requires a return value
    std::set<rose_addr_t> t;
    return t;
}

/** Control flow successors for a basic block.  The @p basic_block argument is a vector of instructions that is assumed to be a
 *  basic block that is entered only at the first instruction and exits only at the last instruction.  A memory map can supply
 *  initial values for the analysis' memory state.  The return value is a set of control flow successor virtual addresses, and
 *  the @p complete argument return value indicates whether the returned set is known to be complete (aside from interrupts,
 *  faults, etc).  The base class implementation just calls the single-instruction version, so architecture-specific subclasses
 *  might want to override this to do something more sophisticated. */
std::set<rose_addr_t>
SgAsmInstruction::get_successors(const std::vector<SgAsmInstruction*>& basic_block, bool *complete/*out*/,
                                 MemoryMap *initial_memory/*=NULL*/)
{
    if (basic_block.size()==0) {
        if (complete) *complete = true;
        return std::set<rose_addr_t>();
    }
    return basic_block.back()->get_successors(complete);
}

/** Determines if an instruction can terminate a basic block.  The analysis only looks at the individual instruction and
 *  therefore is not very sophisticated.  For instance, a conditional branch will always terminate a basic block by this method
 *  even if its condition is constant.  The base class implementation always aborts; architecture-specific subclasses should
 *  override this to do something useful (pure virtual is not possible due to ROSETTA). */
bool
SgAsmInstruction::terminates_basic_block()
{
    abort();
    // tps (12/9/2009) : MSC requires a return value
    return false;
}

/** Returns true if the specified basic block looks like a function call.  This instruction object is only used to select the
 *  appropriate virtual is_function_call(); the basic block to be analyzed is the first argument to the function.  If the basic
 *  block looks like a function call then is_function_call() returns true.  If (and only if) the target address is known (i.e.,
 *  the address of the called function) then @p target is set to this address (otherwise @p target is unmodified). */
bool
SgAsmInstruction::is_function_call(const std::vector<SgAsmInstruction*>&, rose_addr_t *target)
{
    return false;
}

/** Returns true if the specified basic block looks like a function return. This instruction object is only used to select the
 *  appropriate virtual is_function_return(); the basic block to be analyzed is the first argument to the function. */
bool
SgAsmInstruction::is_function_return(const std::vector<SgAsmInstruction*>&)
{
    return false;
}


/** Obtains the virtual address for a branching instruction.  Returns true if this instruction is a branching instruction and
  *  the target address is known; otherwise, returns false and @p target is not modified. */
bool
SgAsmInstruction::get_branch_target(rose_addr_t *target/*out*/) {
    return false;
}

/** Virtual method to determine if a single instruction has an effect. Unless subclass redefines, assume all instructions have
 *  an effect other than adjusting the instruction pointer. See SgAsmx86Instruction implementation for complete
 *  documentation. */
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

/** Returns the size of an instruction in bytes.  This is only a convenience function that returns the size of the
 *  instruction's raw byte vector.  If an instruction or its arguments are modified, then the size returned by this function
 *  might not reflect the true size of the modified instruction if it were to be reassembled. */
size_t
SgAsmInstruction::get_size() const
{
    return p_raw_bytes.size();
}

/** Returns true if this instruction is the special "unknown" instruction. Each instruction architecture in ROSE defines an
 *  "unknown" instruction to be used when the disassembler is unable to create a real instruction.  This can happen, for
 *  instance, if the bit pattern does not represent a valid instruction for the architecture. */
bool
SgAsmInstruction::is_unknown() const
{
    abort(); // too bad ROSETTA doesn't allow virtual base classes
    return false;
}
