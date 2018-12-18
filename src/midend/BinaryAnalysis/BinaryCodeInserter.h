#ifndef Rose_BinaryAnalysis_CodeInserter_H
#define Rose_BinaryAnalysis_CodeInserter_H

#include <Partitioner2/Partitioner.h>
#include <Sawyer/Map.h>

namespace Rose {
namespace BinaryAnalysis {

/** Whether to commit memory allocations. */
namespace Commit {
enum Boolean {
    NO,                                                 /**< Only query an allocation. */
    YES                                                 /**< Allocate memory for real. */
};
} // namespace

/** Insert new code in place of existing instructions. */
class CodeInserter {
public:
    /** What other instructions can be moved to make room. These are bit flags. */
    enum AggregationDirection {
        AGGREGATE_PREDECESSORS  = 0x00000001,           /**< Move preceding instructions in CFG. */
        AGGREGATE_SUCCESSORS    = 0x00000002            /**< Move succeeding instructions in CFG. */
    };

    /** How to pad with no-ops. */
    enum NopPadding {
        PAD_NOP_BACK,                                   /**< Add no-ops to the end of replacements. */
        PAD_NOP_FRONT,                                  /**< Add no-ops to the front of replacements. */
        PAD_RANDOM_BACK                                 /**< Add random data to the end of replacements. */
    };

    /** Type of relocation to perform.
     *
     *  Each enum constant identifies a function whose description is given below. In those descriptions, the following
     *  variables are used:
     *
     *  @li @p input is a vector of bytes that was specified by the user as the new code to be inserted.
     *
     *  @li @p reloc_value is the value data member of the relocation record. It has various interpretations depending on the
     *      relocation function.
     *
     *  @li @ addend is the value currerntly stored at the destination bytes of the input interpretted as a signed value in the
     *      byte order and width specified by the function name.
     *
     *  The last word of the function name specifies the format used to write the computed value back to the output:
     *
     *  @li @c LE32 writes the low-order 32 bits of the computed value as a 32-bit integer in little-endian order.
     *
     *  @li @c LE32HI writes the high-order 32 bits of the computed value as a 32-bit integer in little-endian order. */
    enum RelocType {
        RELOC_INDEX_ABS_LE32,                           /**< Interprets the @p reloc_value as an index of some byte in the
                                                         *   @p input, and computes that byte's virtual address. */

        RELOC_INDEX_ABS_LE32HI,                         /**< Interprets the @p reloc_value as an index of some byte in the @p
                                                         *   input, and computes that byte's virtual address.*/

        RELOC_INDEX_ABS_BE32,                           /**< Interprets the @p reloc_value as an index of some byte in the
                                                         *   @p input, and computes that byte's virtual address. */

        RELOC_ADDR_REL_LE32,                            /**< Interprets the @p reloc_value as a virtual address and computes
                                                         *   the offset from the output virtual address to that specified
                                                         *   virtual address, adjusted with the addend. */

        RELOC_ADDR_REL_BE32,                            /**< Interprets the @p reloc_value as a virtual address and computes
                                                         *   the offset from the output virtual address to that specified
                                                         *   virtual address, adjusted with the addend. */

        RELOC_INSN_ABS_LE32,                            /**< Interprets the @p reloc_value as an instruction relative index for
                                                         *   some instruction of the original basic block. Negative indexes are
                                                         *   measured backward from the insertion point, and non-negative
                                                         *   indexes are measured forward from one past the last deleted
                                                         *   instruction (or insertion point if no deletions). This relocation
                                                         *   function calculates the address of the specified instruction. This
                                                         *   accounts for cases when the referenced instruction has been
                                                         *   moved. */

        RELOC_INSN_REL_LE32,                            /**< Interprets the @p reloc_value as an instruction relative index for
                                                         *   some instruction of the original basic block. Negative indexes are
                                                         *   measured backward from the insertion point, and non-negative
                                                         *   indexes are measured forward from one past the last deleted
                                                         *   instruction (or insertion point if no deletions). This relocation
                                                         *   function calculates the offset from the output virtual address to
                                                         *   that instructions virtual address, adjusted with the addend. This
                                                         *   accounts for cases when the referenced instruction has been
                                                         *   moved. */

        RELOC_INSN_REL_BE32                             /**< Interprets the @p reloc_value as an instruction relative index for
                                                         *   some instruction of the original basic block. Negative indexes are
                                                         *   measured backward from the insertion point, and non-negative
                                                         *   indexes are measured forward from one past the last deleted
                                                         *   instruction (or insertion point if no deletions). This relocation
                                                         *   function calculates the offset from the output virtual address to
                                                         *   that instructions virtual address, adjusted with the addend. This
                                                         *   accounts for cases when the referenced instruction has been
                                                         *   moved. */
    };

    /** Relocation record.
     *
     *  A relocation record describes how the new encoded instructions need to have their encodings adjusted when they're
     *  placed at some specific memory address. This is necessary because the instructions are encoded by the user before an
     *  address is chosen by the CodeInserter.
     *
     *  Each relocation entry applies to a specific index in the vector of bytes of the encoded instructions. This is called
     *  the relocation @p offset.  The @p type of the relocation indicates which predefined function is used to calculate the
     *  value that is to be written to that output offset, and @p value is an optional argument that gets passed to that function. */
    struct Relocation {
        size_t offset;                                  /**< Location of relocation in replacement code. */
        RelocType type;                                 /**< Relocation algorithm. */
        rose_addr_t value;                              /**< Argument for relocation algorithm. */

        /** Constructor.
         *
         *  Construct a relocation that will change the value at the specified @p offset in the replacement code. The size of
         *  the area overwritten as well as the algorithm for computing the new bytes are determined by the relocation @p
         *  type. The @p value is one of the inputs to the relocation algorithm (the other is the virtual address of the byte
         *  at the specified @p offset in the replacement. */
        Relocation(size_t offset, RelocType type, rose_addr_t value)
            : offset(offset), type(type), value(value) {}
    };

    /** Information about an instruction within the basic block being modified. */
    struct InstructionInfo {
        rose_addr_t originalVa;                         /**< Original address of instruction. */
        Sawyer::Optional<rose_addr_t> newVaOffset;      /**< Offset of instruction from front of encoded insn vector. */

        explicit InstructionInfo(SgAsmInstruction *insn)
            : originalVa(insn->get_address()) {}
    };

    /** Information about instructions within the basic block being modified.
     *
     *  The instructions are numbered relative to their position with the insertion point and deleted instructions. Negative
     *  keys refer to instructions that appear before the insertion point, and non-negative keys refer to instructions starting
     *  one past the last deleted instruction or, if no instructions are deleted, the instruction originally at the insertion
     *  point. */
    typedef Sawyer::Container::Map<int, InstructionInfo> InstructionInfoMap;

protected:
    const Rose::BinaryAnalysis::Partitioner2::Partitioner &partitioner_;
    AddressInterval chunkAllocationRegion_;             // where chunks can be allocated
    size_t minChunkAllocationSize_;                     // size of each chunk in bytes (also the alignment)
    size_t chunkAllocationAlignment_;                   // alignment for allocating large chunks
    std::string chunkAllocationName_;                   // name to give new areas of the memory map
    AddressIntervalSet allocatedChunks_;                // large allocated chunks that populate freeSpace_
    AddressIntervalSet freeSpace_;                      // parts of mapped memory serving as free space for allocations
    unsigned aggregationDirection_;                     // AggregationDirection bits
    NopPadding nopPadding_;                             // where to add no-op padding

public:
    /** Facility for emitting diagnostics. */
    static Diagnostics::Facility mlog;

public:
    explicit CodeInserter(const Rose::BinaryAnalysis::Partitioner2::Partitioner &partitioner)
        : partitioner_(partitioner), minChunkAllocationSize_(8192), chunkAllocationAlignment_(4096),
          chunkAllocationName_("new code"), aggregationDirection_(AGGREGATE_PREDECESSORS | AGGREGATE_SUCCESSORS),
          nopPadding_(PAD_NOP_BACK) {
        ASSERT_not_null(partitioner.memoryMap());
        if (!partitioner.memoryMap()->isEmpty() &&
            partitioner.memoryMap()->hull().greatest() < AddressInterval::whole().greatest()) {
            chunkAllocationRegion_ = AddressInterval::hull(partitioner.memoryMap()->hull().greatest() + 1,
                                                           AddressInterval::whole().greatest());
        } else {
            chunkAllocationRegion_ = AddressInterval::whole();
        }
    }

    /** Initialize diagnostic streams.
     *
     *  This is called automatically by @ref Rose::Diagnostics::initialize. */
    static void initDiagnostics();

    /** Property: Where chunks are allocated.
     *
     *  This region defines the part of the memory map where new chunks are allocated in order to hold replacement code that
     *  doesn't fit into the same space as the instructions its replacing.  The default is the part of the address space
     *  immediately after the last mapped address in the partitioner passed to the constructor.
     *
     * @{ */
    const AddressInterval& chunkAllocationRegion() const { return chunkAllocationRegion_; }
    void chunkAllocationRegion(const AddressInterval& i) { chunkAllocationRegion_ = i; }
    /** @} */

    /** Returns the parts of the virtual address space that were allocated for new instructions. The returned value will
     *  be a subset of the @ref chunkAllocationRegion. The return value indicates where large chunks of memory were allocated,
     *  but not what bytes within that memory were actually used for new instructions. */
    const AddressIntervalSet& allocatedChunks() const { return allocatedChunks_; }

    /** Property: Minimum size of allocated chunks.
     *
     *  When allocating space for replacement code, never allocate less than this many bytes at a time. Note that multiple
     *  replacement codes can be written to a single chunk since we maintain a free list within chunks.
     *
     * @{ */
    size_t minChunkAllocationSize() const { return minChunkAllocationSize_; }
    void minChunkAllocationSize(size_t n) { minChunkAllocationSize_ = n; }
    /** @} */

    /** Property: Alignment for large allocated chunks.
     *
     * @{ */
    size_t chunkAllocationAlignment() const { return chunkAllocationAlignment_; }
    void chunkAllocationAlignment(size_t n);
    /** @} */

    /** Property: Name for newly allocated regions of memory.
     *
     * @{ */
    const std::string& chunkAllocationName() const { return chunkAllocationName_; }
    void chunkAllocationName(const std::string &s) { chunkAllocationName_ = s; }
    /** @} */

    /** Property: Whether additional instructions can be moved.
     *
     *  This property controls which additional instructions can be moved by the @ref replaceBlockInsns method in order to make
     *  room for the replacement. It is a bit vector of @ref AggregationDirection bits and defaults to both successors and
     *  predecessors. When both are present, successors are added first (all the way to the end of the block) and then
     *  predecessors are also added (all the way to the beginning of the block).
     *
     * @{ */
    unsigned aggregationDirection() const { return aggregationDirection_; }
    void aggregationDirection(unsigned d) { aggregationDirection_ = d; }
    /** @} */

    /** Property: Where to add no-ops when padding.
     *
     *  When to-be-replaced instructions are overwritten with a replacement and the replacement is smaller than the replaced
     *  instructions, then the replacement is padded with no-op instructions according to this property.
     *
     * @{ */
    NopPadding nopPadding() const { return nopPadding_; }
    void nopPadding(NopPadding p) { nopPadding_ = p; }
    /** @} */

    /** Replace instructions in basic block.
     *
     *  This replaces @p nInsns instructions in the basic block starting at instruction number @p startIdx. The @p nInsns may
     *  be zero, in which case the replacement is inserted before the @p startIdx instruction. The new code is inserted either
     *  by overwriting the to-be-replaced instructions with the @p replacement padded at the end by no-ops if necessary (so
     *  called "overwrite" mode), or the replacement is written to some other part of the address space and unconditional
     *  branches are inserted to branch to the replacement and then back again (so called "branch-aside" mode).
     *
     *  If the neither the replacement (in overwrite mode) nor the unconditional branch (in branch-aside mode) fit in the area
     *  vacated by the to-be-replaced instructions, then the to-be-replaced instructions are extended by moving a neighboring
     *  instruction into the @p replacement. The @ref aggregationDirection property controls which instructions can be
     *  joined. This often works for branch-aside mode, but can even sometimes work for overwrite mode if the basic block
     *  instructions are not executed in address order. The overwrite situation can work when a subsequent or earlier
     *  instruction fills in a hole in the to-be-replaced address set.
     *
     *  This method assumes that the @p replacement is entered at the first byte and exits to one past the last byte. Since
     *  some instruction encodings depend on the location of the replacement in virtual memory, the @p relocations can be used
     *  to patch the replacement as it's written to memory.
     *
     *  Inserting code in this manner is not without risk.  For instance, enlarging the to-be-replaced set might mean that
     *  additional instructions are moved to a different address without changing their encoding. Examples are moving
     *  instructions that reference global variables relative to the instruction's address, branches that span the branch-aside
     *  gap, etc.
     *
     *  Returns true if successful, false otherwise. */
    virtual bool replaceBlockInsns(const Rose::BinaryAnalysis::Partitioner2::BasicBlock::Ptr&,
                                          size_t startIdx, size_t nInsns, std::vector<uint8_t> replacement,
                                          const std::vector<Relocation> &relocations = std::vector<Relocation>());

    /** Replace instructions at front of basic block.
     *
     *  This is just a convenience for @ref replaceBlockInstructions that replaces @p nInsns instructions at the beginning of
     *  the specified basic block. If @p nInsns is zero, then the @p replacement is inserted at the front of the basic block
     *  without removing any instructions. */
    bool replaceInsnsAtFront(const Rose::BinaryAnalysis::Partitioner2::BasicBlock::Ptr &bb, size_t nInsns,
                             const std::vector<uint8_t> &replacement,
                             const std::vector<Relocation> &relocations = std::vector<Relocation>()) {
        return replaceBlockInsns(bb, 0, nInsns, replacement, relocations);
    }

    /** Replace instructions at back of basic block.
     *
     *  This is just a convenience for @ref replaceBlockInstructions that replaces @p nInsns instructions at the end of the
     *  specified basic block. If @p nInsns is zero, then the @p replacement is appended to the end of the basic block without
     *  removing any instructions. */
    virtual bool replaceInsnsAtBack(const Rose::BinaryAnalysis::Partitioner2::BasicBlock::Ptr &bb, size_t nInsns,
                                    const std::vector<uint8_t> &replacement,
                                    const std::vector<Relocation> &relocations = std::vector<Relocation>()) {
        ASSERT_require(nInsns <= bb->nInstructions());
        return replaceBlockInsns(bb, bb->nInstructions()-nInsns, nInsns, replacement, relocations);
    }

    /** Prepend code to a basic block.
     *
     *  This is a convenience for @ref replaceIsnsnAtFront. It inserts the @p replacement at the front of the basic block by
     *  writing the @p replacement followed by the first instruction(s) of the block to some other area of memory, overwriting
     *  the first part of the basic block with an unconditional branch to the replacement, and following the replacement with
     *  an unconditional branch back to the rest of the basic block. */
    virtual bool prependInsns(const Rose::BinaryAnalysis::Partitioner2::BasicBlock::Ptr &bb,
                              const std::vector<uint8_t> &replacement,
                              const std::vector<Relocation> &relocations = std::vector<Relocation>()) {
        return replaceInsnsAtFront(bb, 0, replacement, relocations);
    }

    /** Append code to a basic block.
     *
     *  This is a convenience for @ref replaceInsnsAtBack. It appends the @p replacement to the end of the basic block by
     *  moving the last instruction(s) of the block to some other memory followed by the replacement. The original final
     *  instructions are overwritten with an unconditional branch to that other memory, which is followed by a branch back to
     *  the rest of the basic block. */
    virtual bool appendInsns(const Rose::BinaryAnalysis::Partitioner2::BasicBlock::Ptr &bb,
                             const std::vector<uint8_t> &replacement,
                             const std::vector<Relocation> &relocations = std::vector<Relocation>()) {
        return replaceInsnsAtBack(bb, 0, replacement, relocations);
    }

    /** Replace exactly the specified instructions with some other encoding..
     *
     *  The @p replacement instructions either overwrite the @p toReplace instructions or the @p replacement is written to a
     *  newly allocated area and unconditional branches connect it to the main control flow. The assumption is that control
     *  flow enters at the beginning of @p toReplace and the replacement will exit to the first address after the last
     *  instruction in @p toReplace. Likewise, control enters at the beginning of @p replacement and exits to the first address
     *  after the end of the @p replacement.
     *
     *  If @p relocations are specified, then parts of the @p replacement are rewritten based on its final address. Relocation
     *  records that refer to instructions rather than bytes are not permitted since this function doesn't have access to the
     *  basic block in which the replacement is occuring.
     *
     *  Returns true if the replacement could be inserted, false otherwise. The only time this returns false is when the
     *  addresses of the original instructions starting with the first instruction do not occupy a contiguous region of memory
     *  large enough to hold either the replacement or a jump to the relocated replacement. This algorithm correctly handles
     *  the general case when the @p toReplace instructions are not in address order or are not contiguous. */
    virtual bool replaceInsns(const std::vector<SgAsmInstruction*> &toReplace, const std::vector<uint8_t> &replacement,
                              const std::vector<Relocation> &relocations = std::vector<Relocation>());

    /** Fill the specified memory with no-op instructions. */
    virtual void fillWithNops(const AddressIntervalSet &where);

    /** Fill the specified memory with random data. */
    virtual void fillWithRandom(const AddressIntervalSet &where);

    /** Encode an unconditional branch.
     *
     *  This encodes an unconditional branch instruction at @p srcVa that causes control to flow to @p tgtVa.  The caller
     *  should not assume that a particular size encoding will be returned. E.g., on x86, jumps to targets that are further
     *  away require more bytes to encode than jumps to nearby targets. */
    virtual std::vector<uint8_t> encodeJump(rose_addr_t srcVa, rose_addr_t tgtVa);

    /** Apply relocations to create a new encoding.
     *
     *  The @p relocations are applied to the @p replacement bytes which are assumed to be mapped in virtual memory starting at
     *  @p startVa. The @p relocStart is a byte offset for all the relocations; i.e., the actual offset in the @p replacement
     *  where the relocation is applied is the relocation's offset plus the @p relocStart value. */
    virtual std::vector<uint8_t> applyRelocations(rose_addr_t startVa, std::vector<uint8_t> replacement,
                                                  const std::vector<Relocation> &relocations, size_t relocStart,
                                                  const InstructionInfoMap &insnInfoMap);

    /** Allocate virtual memory in the partitioner memory map.
     *
     *  The second argument is the target address of an unconditional jump that will be added to the end of the allocated
     *  memory but which is not included in the @p nBytes argument (it is however included in the return value).
     *
     *  If the third argument is yes, then memory is actually allocated and removed from the free list. If no, then all steps
     *  are completed except removing it from the free list. The @ref commitAllocation function can be called later to remove
     *  it from the free list. If you don't remove it from the free list, a subsequent allocation request might return the same
     *  addresses. */
    virtual AddressInterval allocateMemory(size_t nBytes, rose_addr_t jmpTargetVa, Commit::Boolean commit = Commit::YES);

    /** Commit previous allocation.
     *
     *  This commits the allocation returned by @ref allocateMemory by removing it from the free list (if the @p commit
     *  argument is true).  We do this as a separate step from the allocation so that we don't have to deallocate in all the
     *  error handling locations.  Failing to commit an allocation will be easier to spot than failing to release an unused
     *  block because the former case causes nonsense disassembly whereas the latter looks like either unreachable code or
     *  static data. */
    void commitAllocation(const AddressInterval &where, Commit::Boolean commit = Commit::YES);

    /**  Given a list of functions, return all addresses that the instructions occupy. */
    AddressIntervalSet instructionLocations(const std::vector<SgAsmInstruction*>&);

    /** Insert new code by overwriting existing instructions.
     *
     *  The @p toReplaceVas are the addresses occupied by the to-be-replaced instructions. Since the to-be-replaced
     *  instructions are not necessarily in address order or contiguous, the @p entryInterval describes the largest contiguous
     *  subset of @p toReplaceVas starting at the entry address. Since the @p replacement is assumed to be entered at its first
     *  byte, the @p replacement will be written into the @p entryInterval (if it fits). The @p replacement is padded if
     *  necessary according to the @ref nopPadding property.  All other addresses in @p toReplaceVas are filled with no-op
     *  instructions. */
    virtual bool replaceByOverwrite(const AddressIntervalSet &toReplaceVas, const AddressInterval &entryInterval,
                                    const std::vector<uint8_t> &replacement, const std::vector<Relocation> &relocations,
                                    size_t relocStart, const InstructionInfoMap &insnInfoMap);

    /** Insert new code in allocated area.
     *
     *  This inserts the @p replacement code in a newly allocated area (by calling @ref allocateMemory). The @p toReplaceVas
     *  are the addresses of all the instruction bytes that are to be replaced. Note that this is all addresses of the
     *  instructions, not just the first addresses. The @p entryInterval is a contiguous subset of @p toReplaceVas and
     *  represents the entry point of @p toReplaceVas and as many subsequent contiguous addresses as possible.  This function
     *  writes an unconditional branch in the @p entryInterval (padding it with no-ops according to @p nopPadding) that jumps
     *  to the replacement code. It appends an unconditional branch to the end of the replacement that jumps to the first
     *  address after the end of the @p toReplaceVas. All other bytes of @p toReplaceVas are overwritten with no-ops. */
    virtual bool replaceByTransfer(const AddressIntervalSet &toReplaceVas, const AddressInterval &entryInterval,
                                   const std::vector<SgAsmInstruction*> &toReplace, const std::vector<uint8_t> &replacement,
                                   const std::vector<Relocation> &relocations, size_t relocStart,
                                   const InstructionInfoMap &insnInfoMap);

    /** Obtain info about instructions for the basic block being modified.
     *
     *  Given a basic block, an insertion point, and the number of instructions that will be deleted starting at that insertion
     *  point, return information about the remaining instructions.  See documentation for @ref InstructionInfoMap for details
     *  about how the instructions are indexed in this map. */
    InstructionInfoMap computeInstructionInfoMap(const Rose::BinaryAnalysis::Partitioner2::BasicBlock::Ptr&,
                                                 size_t startIdx, size_t nDeleted);
};

} // namespace
} // namespace

#endif
