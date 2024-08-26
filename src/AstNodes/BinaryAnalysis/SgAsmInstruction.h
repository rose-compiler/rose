#include <Rose/BinaryAnalysis/Architecture/BasicTypes.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <sageContainer.h>
#include <Sawyer/Cached.h>

#ifdef ROSE_IMPL
#include <SgAsmExprListExp.h>
#include <SgAsmOperandList.h>
#endif

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
public:
    /** Property: Architecture registration ID.
     *
     *  Every instruction must belong to a registered architecture. This ID specifies the architecture of which this instruction
     *  is a member.
     *
     *  When an instruction is serialized to a file, the architecture name is saved in place of its registration ID, and when the
     *  instruction is deserialized, that name is looked up in the registry and the registration ID is saved in the reconstituted
     *  instruction. This mechanism enables the instruction to point to the correct architecture even if the writer and reader
     *  tools have slightly different sets of architectures registered. */
    [[using Rosebud: rosetta, ctor_arg, mutators(), serialize(architectureId)]]
    uint8_t architectureId;

    // Architecture registration IDs change from run to run, so serialize the architecture name instead. The architecture names
    // will probably not change as frequently as their registration IDs.
    std::string architectureIdSerialize(uint8_t id) const;
    uint8_t architectureIdDeserialize(const std::string &name) const;

    /** Property: Raw bytes of an instruction.
     *
     *  These are the bytes that were actually decoded to obtain the instruction AST. */
    [[using Rosebud: rosetta]]
    SgUnsignedCharList rawBytes;

    /** Property: AST node that holds all operands.
     *
     *  This is the @ref SgAsmOperandList AST node that holds all the operands of this instruction. A separate node is
     *  necessary (rather than storing the operand list directly in the instruction node) due to limitations of ROSETTA. */
    [[using Rosebud: rosetta, traverse]]
    SgAsmOperandList* operandList = nullptr;

    /** Property: Delay slot instructions.
     *
     *  The instruction occupying the delay slot for this instruction. A delay slot is an instruction that is executed without the
     *  effects of a preceding instruction. The most common form is a single arbitrary instruction located immediately after a
     *  branch instruction on a RISC architecture, in which case the delay instruction will execute even if the preceding branch is
     *  taken. This makes the instruction execute out-of-order compared to its location in memory or in the original assembler
     *  language code.
     *
     *  The address of the instruction pointed to by the delay slot must immediately follow this instruction. There is no AST edge
     *  from this instruction to its delay slot instruction. */
    [[using Rosebud: rosetta]]
    SgAsmInstruction* delaySlot = nullptr;

    // FIXME[Robb Matzke 2023-03-18]: is the lack of serialization a bug?
    /** Property: Stack pointer at start of instruction relative to start of instruction's function.
     *
     *  If the stack delta was not computed, or could not be computed, or is a non-numeric value then the special value
     *  @ref INVALID_STACK_DELTA is used. */
    [[using Rosebud: rosetta, serialize()]]
    int64_t stackDeltaIn = SgAsmInstruction::INVALID_STACK_DELTA;

    // FIXME[Robb Matzke 2023-03-18]: is the lack of serialization a bug?
    /** Property: Ordered list of instruction semantics.
     *
     *  If instruction semantics are available and attached to the instruction, then this subtree will contain a list of
     *  semantic side effects of the instruction. The semantics are attached by using the @ref
     *  Rose::BinaryAnalysis::InstructionSemantics::StaticSemantics semantic domain. */
    [[using Rosebud: rosetta, traverse, serialize()]]
    SgAsmExprListExp* semantics = nullptr;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Non-property data members
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
    SemanticFailure semanticFailure_;

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
    /** Architecture for instruction.
     *
     *  The architecture is looked up in the architecture registery in constant time by using the @ref architectureId property.
     *  It is generally unwise to change the architecture registery after instructions have been created since this may cause
     *  instructions to refer to an incorrect architecture.
     *
     *  Thread safety: This function is thread safe. */
    Rose::BinaryAnalysis::Architecture::BaseConstPtr architecture() const /*final*/;

    /** Property: Instruction mnemonic string.
     *
     *  The short string that describes the instruction. When comparing instructions, it's faster to use the @c kind property
     *  defined in the subclasses, or the @ref SgAsmInstruction::get_anyKind function instead of comparing mnemonic strings. But be
     *  aware that some architectures have mnemonics that include information about the instruction operands and this information is
     *  typically not represented by the instruction kind enum constants. */
    std::string get_mnemonic() const;

    // [Robb Matzke 2023-12-04]: deprecated
    virtual std::string description() const final ROSE_DEPRECATED("use Architecture::Base::instructionDescription");

    /** Number of operands. */
    size_t nOperands() const;

    /** Nth operand.
     *
     *  If the operand index is out of range, then null is returned. */
    SgAsmExpression* operand(size_t) const;

    // [Robb Matzke 2023-12-04]: deprecated
    virtual bool terminatesBasicBlock() final ROSE_DEPRECATED("use Architecture::Base::terminatesBasicBlock");
    virtual bool isFunctionCallFast(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret) final
        ROSE_DEPRECATED("use Architecture::Base::isFunctionCallFast");
    virtual bool isFunctionCallSlow(const std::vector<SgAsmInstruction*>&, rose_addr_t *target, rose_addr_t *ret) final
        ROSE_DEPRECATED("use Architecture::Base::isFunctionCallSlow");
    virtual bool isFunctionReturnFast(const std::vector<SgAsmInstruction*>&) final
        ROSE_DEPRECATED("use Architecture::Base::isFunctionReturnFast");
    virtual bool isFunctionReturnSlow(const std::vector<SgAsmInstruction*>&) final
        ROSE_DEPRECATED("use Architecture::Base::isFunctionReturnSlow");
    virtual Sawyer::Optional<rose_addr_t> branchTarget() final
        ROSE_DEPRECATED("use Architecture::Base::branchTarget");

    /** Returns true if this instruction is the first instruction in a basic block.
     *
     *  This method looks only at the AST to make this determination. */
    bool isFirstInBlock();

    /** Returns true if this instruction is the last instruction in a basic block.
     *
     *  This method looks only at the AST to make this determination. */
    bool isLastInBlock();

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

    // [Robb Matzke 2023-12-04]: deprecated
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(bool &complete)
        ROSE_DEPRECATED("use Architecture::Base::getSuccessors");
    virtual Rose::BinaryAnalysis::AddressSet getSuccessors(const std::vector<SgAsmInstruction*> &basicBlock,
                                                           bool &complete,
                                                           const Rose::BinaryAnalysis::MemoryMap::Ptr &initial_memory =
                                                           Rose::BinaryAnalysis::MemoryMap::Ptr())
        ROSE_DEPRECATED("use Architecture::Base::getSuccessors");

    /** Returns the size of an instruction in bytes.
     *
     *  This is only a convenience function that returns the size of the instruction's raw byte vector.  If an instruction
     *  or its arguments are modified, then the size returned by this function might not reflect the true size of the
     *  modified instruction if it were to be reassembled. */
    virtual size_t get_size() const;

    // [Robb Matzke 2023-12-05]: deprecated
    virtual bool isUnknown() const final ROSE_DEPRECATED("use Architecture::Base::isUnknown");

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

    /** Converts the instruction to a string.
     *
     *  The return value is a mnemonic, and arguments. Only one space is used between the parts. */
    virtual std::string toStringNoAddr() const;

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

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Deprecated 2023-11
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
    const SgUnsignedCharList& get_raw_bytes() const ROSE_DEPRECATED("use get_rawBytes");
    void set_raw_bytes(const SgUnsignedCharList&) ROSE_DEPRECATED("use set_rawBytes");
};
