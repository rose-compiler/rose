#include <Rose/BinaryAnalysis/MemoryMap.h>

/** Base class for machine instructions.
 *
 *  @li Each instruction is represented by one or more instances of SgAsmInstruction.
 *
 *  @li An instruction obtained from a full AST will have a parent pointer. The instruction's first ancestor of type @ref
 *  SgAsmBlock is the basic block in which the instruction appears, and its first ancestor of type @ref SgAsmFunction is
 *  the function in which the instruction appears. There may be intervening AST nodes having other types.
 *
 *  @li An instruction obtained from a @ref Rose::BinaryAnalysis::Partitioner2::Partitioner "Partitioner" will not have a
 *  parent pointer. You can find ownership information using the Partitioner API.
 *
 *  @li An instruction's bytes will always be contiguous in the virtual address space from which the instruction was
 *  decoded, but might not be contiguous in the file (if any) where the instruction was stored. In fact, there's no
 *  guarantee that the instruction even exists entirely within one file.
 *
 *  @li Two distinct instructions (with different encodings) can start at the same virtual address if the specimen is
 *  self-modifying. Most ROSE analysis assumes that specimens are not self-modifying and uses the instruction's starting
 *  virtual address to uniquely identify the instruction.
 *
 *  @li Two distinct instructions (with different encodings) can occupy overlapping bytes in the virtual address space, and
 *  are guaranteed to have different starting addresses unless the specimen is self-modifying. */
class SgAsmInstruction: public SgAsmStatement {
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Local types
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    struct SemanticFailure {
        size_t n;
        SemanticFailure(): n(0) {}

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
    private:
        friend class boost::serialization::access;
        template<class S>
        void serialize(S &s, const unsigned /*version*/) {
            s & BOOST_SERIALIZATION_NVP(n);
        }
#endif
};

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Properties
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /** Property: Instruction mnemonic string.
     *
     *  The short string that describes the instruction. When comparing instructions, it's faster to use the @ref get_kind
     *  or @ref get_anyKind methods instead of comparing mnemonic strings. But be aware that some architectures have
     *  mnemonics that include information about the instruction operands and this information is typically not represented
     *  by the instruction kind enum constants. */
    [[using Rosebud: rosetta, ctor_arg]]
    std::string mnemonic;

    /** Property: Raw bytes of an instruction.
     *
     *  These are the bytes that were actually decoded to obtain the instruction AST. */
    [[using Rosebud: rosetta]]
    SgUnsignedCharList raw_bytes;

    /** Property: AST node that holds all operands.
     *
     *  This is the @ref SgAsmOperandList AST node that holds all the operands of this instruction. A separate node is
     *  necessary (rather than storing the operand list directly in the instruction node) due to limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmOperandList* operandList = nullptr;

    /** Property: Cache lock count.
     *
     *  Number of locks held on this object, preventing the AST rooted at this node from being evicted from a cache.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    [[using Rosebud: rosetta, accessors(), mutators()]]
    size_t cacheLockCount = 0;

    size_t cacheLockCount() const;
    void adjustCacheLockCount(int increment);
    /** @} */

    // FIXME[Robb P Matzke 2017-02-13]: unused?
    [[using Rosebud: rosetta, accessors(), mutators()]]
    SgAsmStatementPtrList sources;
    void appendSources( SgAsmInstruction* instruction );

    // FIXME[Robb Matzke 2023-03-18]: is the no_serialize a bug?
    /** Property: Stack pointer at start of instruction relative to start of instruction's function.
     *
     *  If the stack delta was not computed, or could not be computed, or is a non-numeric value then the special value
     *  @ref INVALID_STACK_DELTA is used. */
    [[using Rosebud: rosetta, no_serialize]]
    int64_t stackDeltaIn = SgAsmInstruction::INVALID_STACK_DELTA;

    // FIXME[Robb Matzke 2023-03-18]: is the no_serialize a bug?
    /** Property: Ordered list of instruction semantics.
     *
     *  If instruction semantics are available and attached to the instruction, then this subtree will contain a list of
     *  semantic side effects of the instruction. The semantics are attached by using the @ref
     *  Rose::BinaryAnalysis::InstructionSemantics::StaticSemantics semantic domain. */
    [[using Rosebud: rosetta, traverse, no_serialize]]
    SgAsmExprListExp* semantics = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Non-property data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    SemanticFailure semanticFailure_;

    // Synchronized data members. All the following data members (as listed in binaryInstruction.C, not the ROSETTA-generated
    // code) should be procted by the mutex_. Additionally, the p_cacheLockCount data member is synchronized.
    mutable SAWYER_THREAD_TRAITS::Mutex mutex_;

public:
    /** Represents an invalid stack delta.
     *
     *  This value is used for the result of a stack delta analysis stored in the instruction AST if the stack delta
     *  analysis was not run or did not produce a numeric result. */
    static const int64_t INVALID_STACK_DELTA;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Functions
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    /** Return a description of this instruction.
     *
     *  Descriptions are useful for generating comments in the disassembly listing to say what each instruction does when
     *  the audience is not well versed in that instruction set architecture.  The base implementation always returns an
     *  empty string. */
    virtual std::string description() const { return ""; }

    /** Number of operands. */
    size_t nOperands() const;

    /** Nth operand.
     *
     *  If the operand index is out of range, then null is returned. */
    SgAsmExpression* operand(size_t) const;

    /** Determines if this instruction normally terminates a basic block.
     *
     *  The analysis only looks at the individual instruction and therefore is not very sophisticated.  For instance, a
     *  conditional branch will always terminate a basic block by this method even if its condition is opaque.  The base
     *  class implementation always aborts; architecture-specific subclasses should override this to do something useful
     *  (pure virtual is not possible due to ROSETTA). */
    virtual bool terminatesBasicBlock();

    /** Returns true if the specified basic block looks like a function call.
     *
     *  This instruction object is only used to select the appropriate virtual method; the basic block to be analyzed is
     *  the first argument to the function.  If the basic block looks like a function call then this method returns true.
     *  If (and only if) the target address is known (i.e., the address of the called function) then @p target is set to
     *  this address (otherwise @p target is unmodified). If the return address is known or can be guessed, then return_va
     *  is initialized to the return address, which is normally the fall-through address of the last instruction; otherwise
     *  the return_va is unmodified.
     *
     *  The "fast" and "slow" versions differ only in what kind of anlysis they do.  The "fast" version typically looks
     *  only at instruction patterns while the slow version might incur more expense by looking at instruction semantics.
     *
     * @{ */
    virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret);
    virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret);
    /** @} */

    /** Returns true if the specified basic block looks like a function return.
     *
     *  This instruction object is only used to select the appropriate virtual method; the basic block to be analyzed is
     *  the first argument to the function.
     *
     *  The "fast" and "slow" versions differ only in what kind of anlysis they do.  The "fast" version typically looks
     *  only at instruction patterns while the slow version might incur more expense by looking at instruction semantics.
     *
     * @{ */
    virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&);
    virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&);
    /** @} */

    /** Returns true if this instruction is the first instruction in a basic block.
     *
     *  This method looks only at the AST to make this determination. */
    bool isFirstInBlock();

    /** Returns true if this instruction is the last instruction in a basic block.
     *
     *  This method looks only at the AST to make this determination. */
    bool isLastInBlock();

    /** Obtains the virtual address for a branching instruction.
     *
     *  Returns the branch target address if this instruction is a branching instruction and the target is known; otherwise
     *  returns nothing. */
    virtual Sawyer::Optional<rose_addr_t> branchTarget();

    // FIXME[Robb Matzke 2021-03-02]: deprecated
    bool getBranchTarget(rose_addr_t *target /*out*/) ROSE_DEPRECATED("use branchTarget instead");

    /** Determines whether a single instruction has an effect.
     *
     *  An instruction has an effect if it does anything other than setting the instruction pointer to a concrete
     *  value. Instructions that have no effect are called "no-ops".  The x86 NOP instruction is an example of a no-op, but
     *  there are others also.
     *
     *  The following information about x86 no-ops is largely from Cory Cohen at CMU/SEI. In the discussion that follows,
     *  we are careful to distinguish between NOP (the mneumonic for instructions 90, and 0f1f) and "no-op" (any
     *  instruction whose only effect is to advance the instruction pointer).
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
     *                                              three are reliably supported by the actual Intel processors. ROSE
     *                                              supports any number up to the maximum instruction size (varies by mode).
     *
     *  6688c0               mov al,al              The operand size prefix can even be nonsensical.
     *
     *  8ac0                 mov al,al              These are also presumabely no-ops.  As with most instructions, these
     *  8bc0                 mov eax,eax            will accept operand size prefixes as well.
     *
     *  f090                 lock nop               Most of these instructions will accept a lock prefix as well, which does
     *  f0f090               lock nop               not materially affect the result. As before, they can occur repeatedly,
     *  f066f090             lock nop               and even in wacky combinations.
     *  f066f06666f0f066f090 lock nop
     *
     *  f290                 repne nop              Cory Cohen strongly suspects that the other instruction prefixes are
     *  f390                 rep nop                ignored as well, although to be complete, we might want to conduct a
     *  2690                 es nop                 few tests into the behavior of common processors.
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
     *  0f1f4000             nop [eax+0x0]          0x66 operand size prefix. In fact, Intel recommends doing this now
     *  0f1f440000           nop [eax+0x0]          for the optimally efficient 6- and 9-byte sequences.
     *  0f1f8000000000       nop [eax+0x0]
     *  0f1f840000000000     nop [eax+0x0]
     *
     *  0f0dxx               nop [xxx]              The latest version of the manual implies that this sequence is also
     *                                              reserved for NOP, although I can find almost no references to it except
     *                                              in the latest instruction manual on page A-13 of volume 2B. It's also
     *                                              mentioned on x86asm.net. [CORY 2010-04]
     *
     *  d9d0                 fnop                   These aren't really no-ops on the chip, but are no-ops from the
     *  9b                   wait                   program's perspective. Most of these instructions are related to
     *  0f08                 invd                   improving cache efficiency and performance, but otherwise do not
     *  0f09                 wbinvd                 affect the program behavior.
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
     *  0f18xx through 0f1exx                       This opcode rante is officially undefined but is probably reserved
     *                                              for no-ops as well.  Any instructions encountered in this range are
     *                                              probably consequences of bad code and should be ingored.
     *
     *  JMP, Jcc, PUSH/RET, etc.                    Branches are considered no-ops if they can be proven to always branch
     *                                              to the fall-through address.
     * @endcode
     */
    virtual bool hasEffect();

    /** Determine if an instruction sequence has an effect.
     *
     *  A sequence of instructions has an effect if it does something other than setting the instruction pointer to a
     *  concrete value.
     *
     *  This is mostly a wrapper around the @ref Rose::BinaryAnalysis::NoOperation "NoOperation" analysis. The @p
     *  allow_branch and @p relax_stack_semantics are no longer supported but perhaps will be added eventually to the
     *  NoOperation analysis. */
    virtual bool hasEffect(const std::vector<SgAsmInstruction*>&, bool allow_branch=false,
                           bool relax_stack_semantics=false);

    /** Determines what subsequences of an instruction sequence have no cumulative effect.
     *
     *  The return value is a vector of pairs where each pair is the starting index and length of subsequence.  The
     *  algorithm we use is to compute the machine state after each instruction and then look for pairs of states that are
     *  identical except for the instruction pointer.
     *
     *  This is mostly a wrapper around the @ref Rose::BinaryAnalysis::NoOperation "NoOperation" analysis. The @p
     *  allow_branch and @p relax_stack_semantics are no longer supported but perhaps will be added eventually to the
     *  NoOperation analysis. */
    virtual std::vector<std::pair<size_t,size_t> >
    findNoopSubsequences(const std::vector<SgAsmInstruction*>& insns, bool allow_branch=false,
                         bool relax_stack_semantics=false);

    /** Control flow successors for a single instruction.
     *
     *  The return value does not consider neighboring instructions, and therefore is quite naive.  It returns only the
     *  information it can glean from this single instruction.  If the returned set of virtual instructions is fully known
     *  then the @p complete argument will be set to true, otherwise false.  The base class implementation always
     *  aborts()--it must be defined in an architecture-specific subclass (pure virtual is not possible due to ROSETTA). */
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete); /*subclasses must redefine*/

    /** Control flow successors for a basic block.
     *
     *  The @p basicBlock argument is a vector of instructions that is assumed to be a basic block that is entered only at
     *  the first instruction and exits only at the last instruction.  A memory map can supply initial values for the
     *  analysis' memory state.  The return value is a set of control flow successor virtual addresses, and the @p complete
     *  argument return value indicates whether the returned set is known to be complete (aside from interrupts, faults,
     *  etc).  The base class implementation just calls the single-instruction version, so architecture-specific subclasses
     *  might want to override this to do something more sophisticated. */
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(const std::vector<SgAsmInstruction*> &basicBlock,
                                                           bool &complete,
                                                           const Rose::BinaryAnalysis::MemoryMap::Ptr &initial_memory =
                                                           Rose::BinaryAnalysis::MemoryMap::Ptr());

    /** Returns the size of an instruction in bytes.
     *
     *  This is only a convenience function that returns the size of the instruction's raw byte vector.  If an instruction
     *  or its arguments are modified, then the size returned by this function might not reflect the true size of the
     *  modified instruction if it were to be reassembled. */
    virtual size_t get_size() const;

    /** Returns true if this instruction is the special "unknown" instruction.
     *
     *  Each instruction architecture in ROSE defines an "unknown" instruction to be used when the disassembler is unable
     *  to create a real instruction.  This can happen, for instance, if the bit pattern does not represent a valid
     *  instruction for the architecture. */
    virtual bool isUnknown() const;

    /** Returns instruction kind for any architecture.
     *
     *  Instruction kinds are specific to the architecture so it doesn't make sense to compare an instruction kind from x86
     *  with an instruction kind from m68k.  However, this virtual function exists so that we don't need to implement
     *  switch statements every time we want to compare two instructions from the same architecture.  For instance, instead
     *  of code like this:
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
    virtual unsigned get_anyKind() const;

    /** Converts the instruction to a string.
     *
     *  The return value is an address, colon, mnemonic, and arguments. Only one space is used between the parts. */
    virtual std::string toString() const;

    /** Explicit constants.
     *
     *  Return the set of integer constants that appear explicitly in the instruction's operands. These are called
     *  "immediates" for some architectures such as X86. */
    virtual std::set<rose_addr_t> explicitConstants() const;

    /** Property: Whether instruction semantics failed at this location.
     *
     *  This property is incremented by various analyses that evaluate instructions semantically when semantics fails
     *  in a way that is not recoverable.  Some analyses can work around failed semantics by operating in a degraded
     *  mode, and it is up to the analysis whether to increment this property.
     *
     *  Thread safety: This method is thread safe.
     *
     * @{ */
    size_t semanticFailure() const;
    void semanticFailure(size_t);
    void incrementSemanticFailure();
    /** @} */

    /** Rewrite certain addressing modes for operands.
     *
     *  The addressing mode: <code>REG1 + REG2 * SIZE + OFFSET</code> is changed from <code>(+ (+ REG1 (* REG2 SIZE))
     *  OFFSET)</code> to <code>(+ (+ REG1 OFFSET) (* REG2 SIZE))</code>, which during instruction semantic executions causes
     *  the base register and offset to be added first to obtain the starting address of an array, and then an additional offset
     *  added to obtain the address of the desired element of that array.
     *
     *  Returns true if anything changed, false otherwise. */
    bool normalizeOperands();
};
