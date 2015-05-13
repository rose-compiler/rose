/* SgAsmInstruction member definitions.  Do not move them to src/ROSETTA/Grammar/BinaryInstruction.code (or any *.code file)
 * because then they won't get indexed/formatted/etc. by C-aware tools. */

#include "sage3basic.h"
#include "BinaryNoOperation.h"
#include "Diagnostics.h"
#include "Disassembler.h"

using namespace rose;
using namespace rose::Diagnostics;
using namespace rose::BinaryAnalysis;

/** Indicates concrete stack delta is not known or not calculated. */
const int64_t SgAsmInstruction::INVALID_STACK_DELTA = (uint64_t)1 << 63; // fairly arbitrary, but far from zero

/** Returns instruction kind for any architecture.
 *
 *  Instruction kinds are specific to the architecture so it doesn't make sense to compare an instruction kind from x86 with an
 *  instruction kind from m68k.  However, this virtual function exists so that we don't need to implement switch statements
 *  every time we want to compare two instructions from the same architecture.  For instance, instead of code like this:
 *
 * @code
 *  bool areSame(SgAsmInstruction *a, SgAsmInstruction *b) {
 *      if (a->variantT() != b->variantT())
 *          return false;
 *      if (SgAsmM68kInstruction *aa = isSgAsmM68kInstruction(a)) {
 *          SgAsmM68kInstruction *bb = isSgAsmM68kInstruction(b);
 *          return aa->get_kind() == bb->get_kind();
 *      }
 *      if (SgAsmMipsInstruction *aa = isSgAsmMipsInstruction(a)) {
 *          SgAsmMipsInstruction *bb = isSgAsmMipsInstruction(b);
 *          return aa->get_kind() == bb->get_kind();
 *      }
 *      ...
 *      ... // and many others
 *      ...
 *      ASSERT_not_reachable("architecture is not implemented yet");
 *  }
 * @endcode
 *
 *  we can write future-proof code:
 *
 * @code
 *  bool areSame(SgAsmInstruction *a, SgAsmInstruction *b) {
 *      return a->variantT()==b->variantT() && a->get_anyKind()==b->get_anyKind();
 *  }
 * @endcode */
unsigned
SgAsmInstruction::get_anyKind() const {
    // ROSETTA doesn't support pure virtual, so run-time errors is the best we can do.
    ASSERT_not_reachable("SgAsmInstruction::get_kind() should have been implemented in " + class_name());
}

/** Control flow successors for a single instruction.  The return value does not consider neighboring instructions, and
 *  therefore is quite naive.  It returns only the information it can glean from this single instruction.  If the returned set
 *  of virtual instructions is fully known then the @p complete argument will be set to true, otherwise false.  The base class
 *  implementation always aborts()--it must be defined in an architecture-specific subclass (pure virtual is not possible due
 *  to ROSETTA). */
std::set<rose_addr_t>
SgAsmInstruction::getSuccessors(bool *complete) {
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
SgAsmInstruction::getSuccessors(const std::vector<SgAsmInstruction*>& basic_block, bool *complete/*out*/,
                                const MemoryMap *initial_memory/*=NULL*/)
{
    if (basic_block.size()==0) {
        if (complete) *complete = true;
        return std::set<rose_addr_t>();
    }
    return basic_block.back()->getSuccessors(complete);
}

/** Determines if an instruction can terminate a basic block.  The analysis only looks at the individual instruction and
 *  therefore is not very sophisticated.  For instance, a conditional branch will always terminate a basic block by this method
 *  even if its condition is constant.  The base class implementation always aborts; architecture-specific subclasses should
 *  override this to do something useful (pure virtual is not possible due to ROSETTA). */
bool
SgAsmInstruction::terminatesBasicBlock()
{
    abort();                                            // rosetta doesn't support pure virtual functions
#ifdef _MSC_VER
    return false;                                       // tps (12/9/2009) : MSC requires a return value
#endif
}

/** Returns true if the specified basic block looks like a function call.  This instruction object is only used to select the
 *  appropriate virtual method; the basic block to be analyzed is the first argument to the function.  If the basic
 *  block looks like a function call then this method returns true.  If (and only if) the target address is known (i.e.,
 *  the address of the called function) then @p target is set to this address (otherwise @p target is unmodified). If
 *  the return address is known or can be guessed, then return_va is initialized to the return address, which is normally the
 *  fall-through address of the last instruction; otherwise the return_va is unmodified.
 *
 *  The "fast" and "slow" versions differ only in what kind of anlysis they do.  The "fast" version typically looks only at
 *  instruction patterns while the slow version might incur more expense by looking at instruction semantics.
 *
 * @{ */
bool
SgAsmInstruction::isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *return_va)
{
    return false;
}
bool
SgAsmInstruction::isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *return_va)
{
    return false;
}
/** @} */

/** Returns true if the specified basic block looks like a function return. This instruction object is only used to select the
 *  appropriate virtual method; the basic block to be analyzed is the first argument to the function.
 *  
 *  The "fast" and "slow" versions differ only in what kind of anlysis they do.  The "fast" version typically looks only at
 *  instruction patterns while the slow version might incur more expense by looking at instruction semantics.
 *
 * @{ */
bool
SgAsmInstruction::isFunctionReturnFast(const std::vector<SgAsmInstruction*>&)
{
    return false;
}
bool
SgAsmInstruction::isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&)
{
    return false;
}
/** @} */

/** Returns true if this instruction is the first instruction in a basic block. */
bool
SgAsmInstruction::isFirstInBlock()
{
    SgAsmBlock *bb = SageInterface::getEnclosingNode<SgAsmBlock>(this);
    if (bb) {
        const SgAsmStatementPtrList &stmts = bb->get_statementList();
        for (size_t i=0; i<stmts.size(); ++i) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(stmts[i]))
                return insn==this;
        }
    }
    return false;
}

/** Returns true if this instruction is the last instruction in a basic block. */
bool
SgAsmInstruction::isLastInBlock()
{
    SgAsmBlock *bb = SageInterface::getEnclosingNode<SgAsmBlock>(this);
    if (bb) {
        const SgAsmStatementPtrList &stmts = bb->get_statementList();
        for (size_t i=stmts.size(); i>0; --i) {
            if (SgAsmInstruction *insn = isSgAsmInstruction(stmts[i-1]))
                return insn==this;
        }
    }
    return false;
}

/** Obtains the virtual address for a branching instruction.  Returns true if this instruction is a branching instruction and
  *  the target address is known; otherwise, returns false and @p target is not modified. */
bool
SgAsmInstruction::getBranchTarget(rose_addr_t *target/*out*/) {
    return false;
}

/** Determines whether a single instruction has an effect.
 *
 *  An instruction has an effect if it does anything other than setting the instruction pointer to a concrete
 *  value. Instructions that have no effect are called "no-ops".  The x86 NOP instruction is an example of a no-op, but there
 *  are others also.
 *
 *  The following information about x86 no-ops is largely from Cory Cohen at CMU/SEI. In the discussion that follows, we are
 *  careful to distinguish between NOP (the mneumonic for instructions 90, and 0f1f) and "no-op" (any instruction whose only
 *  effect is to advance the instruction pointer).
 *
 * @code
 *  Opcode bytes         Intel assembly syntax
 *  -------------------- ---------------------- 
 *  90                   nop
 *
 *  89c0                 mov eax,eax            Intel's old recommended two-byte no-op was to
 *  89c9                 mov ecx,ecx            move a register to itself...  The second byte of these are mod/rm
 *  89d2                 mov edx,edx            bytes, and can generally be substituded wherever you see 0xc0 in
 *  89db                 mov ebx,ebx            subsequent examples.
 *  89e4                 mov esp,esp
 *  89ed                 mov ebp,ebp
 *  89f6                 mov esi,esi
 *  89ff                 mov edi,edi
 *
 *  88c0                 mov al,al              The above are also available in 8-bit form with a leading byte of 0x88
 *  6689c0               mov ax,ax              and with an operand size prefix (0x66).
 *
 *  66666689c0           mov ax,ax              The prefixes can be repeated. One source seemed to imply that up to
 *                                              three are reliably supported by the actual Intel processors. ROSE supports
 *                                              any number up to the maximum instruction size (varies by mode).
 *
 *  6688c0               mov al,al              The operand size prefix can even be nonsensical.
 *
 *  8ac0                 mov al,al              These are also presumabely no-ops.  As with most instructions, these will
 *  8bc0                 mov eax,eax            accept operand size prefixes as well.
 *
 *  f090                 lock nop               Most of these instructions will accept a lock prefix as well, which does
 *  f0f090               lock nop               not materially affect the result. As before, they can occur repeatedly, and
 *  f066f090             lock nop               even in wacky combinations.
 *  f066f06666f0f066f090 lock nop
 *  
 *  f290                 repne nop              Cory Cohen strongly suspects that the other instruction prefixes are
 *  f390                 rep nop                ignored as well, although to be complete, we might want to conduct a few
 *  2690                 es nop                 tests into the behavior of common processors.
 *  2e90                 cs nop
 *  3690                 ss nop
 *  3e90                 ds nop
 *  6490                 fs nop
 *  6590                 gs nop
 *  6790                 nop
 *  
 *  8d00                 lea eax,[eax]          Intel's old recommendation for larger no-ops was to use the LEA
 *  8d09                 lea ecx,[ecx]          instruction in various dereferencing modes.
 *  8d12                 lea edx,[edx]
 *  8d1b                 lea ebx,[ebx]
 *  8d36                 lea esi,[esi]
 *  8d3f                 lea edi,[edi]
 *  
 *  8d4000               lea eax,[eax+0x0]
 *  8d4900               lea ecx,[ecx+0x0]
 *  8d5200               lea edx,[edx+0x0]
 *  8d5b00               lea ebx,[ebx+0x0]
 *  8d7600               lea esi,[esi+0x0]
 *  8d7f00               lea edi,[edi+0x0]
 *  
 *  8d8000000000         lea eax,[eax+0x0]      This last block is really the [reg*0x1+0x0] dereferencing mode.
 *  8d8900000000         lea ecx,[ecx+0x0]
 *  8d9200000000         lea edx,[edx+0x0]
 *  8d9b00000000         lea ebx,[ebx+0x0]
 *  8db600000000         lea esi,[esi+0x0]
 *  8dbf00000000         lea edi,[edi+0x0]
 *
 *  8d0420               lea eax,[eax]          Then there's funky equivalents involving SIB bytes.
 *  8d0c21               lea ecx,[ecx]
 *  8d1422               lea edx,[edx]
 *  8d1c23               lea ebx,[ebx]
 *  8d2424               lea esp,[esp]
 *  8d3426               lea esi,[esi]
 *  8d3c27               lea edi,[edi]
 *  
 *  8d442000             lea eax,[eax+0x0]
 *  8d4c2100             lea ecx,[ecx+0x0]
 *  8d542200             lea edx,[edx+0x0]
 *  8d5c2300             lea ebx,[ebx+0x0]
 *  8d642400             lea esp,[esp+0x0]
 *  8d742600             lea esi,[esi+0x0]
 *  8d7c2700             lea edi,[edi+0x0]
 *  
 *  8d842000000000       lea eax,[eax+0x0]
 *  8d8c2100000000       lea ecx,[ecx+0x0]
 *  8d942200000000       lea edx,[edx+0x0]
 *  8d9c2300000000       lea ebx,[ebx+0x0]
 *  8da42400000000       lea esp,[esp+0x0]
 *  8db42600000000       lea esi,[esi+0x0]
 *  8dbc2700000000       lea edi,[edi+0x0]
 *  
 *  8d2c2d00000000       lea ebp,[ebp+0x0]      The EBP variants don't exactly follow the pattern above.
 *  8d6c2500             lea ebp,[ebp+0x0]
 *  8dac2500000000       lea ebp,[ebp+0x0]
 *
 *  0f1f00               nop [eax]              P4+ adds the 0f1f instruction. Each of these can be prefixed with the
 *  0f1f4000             nop [eax+0x0]          0x66 operand size prefix. In fact, Intel recommends doing this now for the
 *  0f1f440000           nop [eax+0x0]          optimally efficient 6- and 9-byte sequences.
 *  0f1f8000000000       nop [eax+0x0]
 *  0f1f840000000000     nop [eax+0x0]
 *
 *  0f0dxx               nop [xxx]              The latest version of the manual implies that this sequence is also
 *                                              reserved for NOP, although I can find almost no references to it except
 *                                              in the latest instruction manual on page A-13 of volume 2B. It's also mentioned
 *                                              on x86asm.net. [CORY 2010-04]
 *                                              
 *  d9d0                 fnop                   These aren't really no-ops on the chip, but are no-ops from the program's
 *  9b                   wait                   perspective. Most of these instructions are related to improving cache
 *  0f08                 invd                   efficiency and performance, but otherwise do not affect the program
 *  0f09                 wbinvd                 behavior.
 *  0f01c9               mwait
 *  0f0138               invlpg [eax]
 *  0f01bf00000000       invlpg [edi+0x0]       and more...
 *  0f18 /0              prefetchnta [xxx]
 *  0f18 /1              prefetch0 [xxx]
 *  0f18 /2              prefetch1 [xxx]
 *  0f18 /3              prefetch2 [xxx]
 *  0fae /5              lfence [xxx]
 *  0fae /6              mfence [xxx]
 *  0fae /7              sfence [xxx]
 *
 *  0f18xx through 0f1exx                       This opcode rante is officially undefined but is probably reserved for
 *                                              no-ops as well.  Any instructions encountered in this range are probably
 *                                              consequences of bad code and should be ingored.
 *                                              
 *  JMP, Jcc, PUSH/RET, etc.                    Branches are considered no-ops if they can be proven to always branch
 *                                              to the fall-through address.
 * @endcode
 */
bool
SgAsmInstruction::hasEffect()
{
    std::vector<SgAsmInstruction*> sequence;
    sequence.push_back(this);
    return hasEffect(sequence, false);
}

// Build analyzer for no-ops
static NoOperation
buildNopAnalyzer(SgAsmInterpretation *interp) {
    using namespace InstructionSemantics2;

    if (!interp) {
        static bool emitted = false;
        if (!emitted && InstructionSemantics2::mlog[WARN]) {
            InstructionSemantics2::mlog[WARN] <<"SgAsmInstruction::buildDispatcher: no binary interpretation\n";
            emitted = true;
        }
        return NoOperation(BaseSemantics::DispatcherPtr());
    }

    Disassembler *disassembler = Disassembler::lookup(interp);
    return NoOperation(disassembler);
}

/** Determine if an instruction sequence has an effect.
 *
 *  A sequence of instructions has an effect if it does something other than setting the instruction pointer to a concrete value.
 *
 *  This is mostly a wrapper around the @ref BinaryAnalysis::NoOperation analysis. The @p allow_branch and @p
 *  relax_stack_semantics are no longer supported but perhaps will be added eventually to the NoOperation analysis. */
bool
SgAsmInstruction::hasEffect(const std::vector<SgAsmInstruction*> &insns, bool allow_branch/*false*/,
                            bool relax_stack_semantics/*false*/)
{
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(insns.front());
    NoOperation analyzer = buildNopAnalyzer(interp);

    if (relax_stack_semantics) {
        static bool emitted = false;
        if (!emitted && InstructionSemantics2::mlog[WARN]) {
            InstructionSemantics2::mlog[WARN] <<"SgAsmX86Instruction::hasEffect: relax_stack_semantics not implemented\n";
            emitted = true;
        }
    }

    return !analyzer.isNoop(insns);
}

/** Determines what subsequences of an instruction sequence have no cumulative effect.
 *
 *  The return value is a vector of pairs where each pair is the starting index and length of subsequence.  The algorithm we
 *  use is to compute the machine state after each instruction and then look for pairs of states that are identical except for
 *  the instruction pointer.
 *
 *  This is mostly a wrapper around the @ref BinaryAnalysis::NoOperation analysis. The @p allow_branch and @p
 *  relax_stack_semantics are no longer supported but perhaps will be added eventually to the NoOperation analysis. */
std::vector<std::pair<size_t,size_t> >
SgAsmInstruction::findNoopSubsequences(const std::vector<SgAsmInstruction*>& insns, bool allow_branch/*false*/, 
                                       bool relax_stack_semantics/*false*/)
{
    SgAsmInterpretation *interp = SageInterface::getEnclosingNode<SgAsmInterpretation>(insns.front());
    NoOperation analyzer = buildNopAnalyzer(interp);

    if (relax_stack_semantics) {
        static bool emitted = false;
        if (!emitted && InstructionSemantics2::mlog[WARN]) {
            InstructionSemantics2::mlog[WARN] <<"SgAsmX86Instruction::hasEffect: relax_stack_semantics not implemented\n";
            emitted = true;
        }
    }

    NoOperation::IndexIntervals indexes = analyzer.findNoopSubsequences(insns);
    std::vector<std::pair<size_t, size_t> > retval;
    BOOST_FOREACH (const NoOperation::IndexInterval &interval, indexes)
        retval.push_back(std::make_pair(interval.least(), interval.size()));
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
SgAsmInstruction::isUnknown() const
{
    abort(); // too bad ROSETTA doesn't allow virtual base classes
    return false;
}
