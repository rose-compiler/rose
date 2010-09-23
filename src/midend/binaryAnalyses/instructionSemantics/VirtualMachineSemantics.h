#ifndef Rose_VirtualMachineSemantics_H
#define Rose_VirtualMachineSemantics_H
#include <stdint.h>
#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "x86InstructionSemantics.h"

#include <map>
#include <vector>


/** A policy for x86InstructionSemantics.
 *
 *  This policy can be used to emulate the execution of a single basic block of instructions.  It is similar in nature to the
 *  FindConstantsPolicy except much simpler, much faster, and much more memory-lean.  The main classes are:
 *
 *  <ul>
 *    <li>Policy: the policy class used to instantiate X86InstructionSemantic instances.</li>
 *    <li>State: represents the state of the virtual machine, its registers and memory.</li>
 *    <li>ValueType: the values stored in registers and memory and used for memory addresses.</li>
 *  </ul>
 *
 *  Each value is either a known value or an unknown value. An unknown value consists of a base name and offset and whether
 *  the value is negated. */
namespace VirtualMachineSemantics {

extern uint64_t name_counter;

typedef std::map<uint64_t, uint64_t> RenameMap;

/** A value is either known or unknown. Unknown values have a base name (unique ID number), offset, and sign. */
template<size_t nBits>
struct ValueType {
    uint64_t name;                      /**< Zero for constants; non-zero ID number for everything else. */
    uint64_t offset;                    /**< The constant (if name==0) or an offset w.r.t. an unknown (named) base value. */
    bool negate;                        /**< Switch between name+offset and (-name)+offset; should be false for constants. */

    /** Construct a value that is unknown and unique. */
    ValueType(): name(++name_counter), offset(0), negate(false) {}

    /** Copy-construct a value, truncating or extending at msb the source value. */
    template <size_t Len>
    ValueType(const ValueType<Len> &other) {
        name = other.name;
        offset = other.offset & IntegerOps::GenMask<uint64_t, nBits>::value; /*make sure high-order bits are zeroed out*/
        negate = other.negate;
    }

    /** Construct a ValueType with a known value. */
    ValueType(uint64_t n)   /*implicit*/
        : name(0), offset(n), negate(false) {
        this->offset &= IntegerOps::GenMask<uint64_t, nBits>::value;
    }

    /** Low-level constructor used internally. */
    ValueType(uint64_t name, uint64_t offset, bool negate=false)
        : name(name), offset(offset), negate(negate) {
        this->offset &= IntegerOps::GenMask<uint64_t, nBits>::value;
    }

    /** Returns true if the value is known, false if the value only has a name. */
    bool is_known() const {
        return 0==name;
    }

    /** Returns the value if it is known. */
    uint64_t known_value() const {
        ROSE_ASSERT(is_known());
        return offset;
    }

    /** Returns a new, optionally renamed, value.  If the rename map, @p rmap, is non-null and this value is a named value,
     *  then its name will be transformed by looking up the name in the map and using the value found there. If the name is
     *  not in the map then a new entry is created in the map.  Remapped names start counting from one.  For example, if
     *  "v904885611+0xfc" is the first value to be renamed, it will become "v1+0xfc". */
    ValueType<nBits> rename(RenameMap *rmap=NULL) const;

    /** Print the value. If a rename map is specified a named value will be renamed to have a shorter name.  See the rename()
     *  method for details. */
    void print(std::ostream &o, RenameMap *rmap=NULL) const {
        uint64_t sign_bit = (uint64_t)1 << (nBits-1); /* e.g., 80000000 */
        uint64_t val_mask = sign_bit - 1;             /* e.g., 7fffffff */
        uint64_t negative = nBits>1 && (offset & sign_bit) ? (~offset & val_mask) + 1 : 0; /*magnitude of negative value*/

        if (name!=0) {
            /* This is a named value rather than a constant. */
            uint64_t renamed = name;
            if (rmap) {
                RenameMap::iterator found = rmap->find(name);
                if (found==rmap->end()) {
                    renamed = rmap->size()+1;
                    rmap->insert(std::make_pair(name, renamed));
                } else {
                    renamed = found->second;
                }
            }
            const char *sign = negate ? "-" : "";
            o <<sign <<"v" <<std::dec <<renamed;
            if (negative) {
                o <<"-0x" <<std::hex <<negative;
            } else if (offset) {
                o <<"+0x" <<std::hex <<offset;
            }
        } else {
            /* This is a constant */
            ROSE_ASSERT(!negate);
            o  <<"0x" <<std::hex <<offset;
            if (negative)
                o <<" (-0x" <<std::hex <<negative <<")";
        }
    }

    friend bool operator==(const ValueType &a, const ValueType &b) {
        return a.name==b.name && (!a.name || a.negate==b.negate) && a.offset==b.offset;
    }

    friend bool operator!=(const ValueType &a, const ValueType &b) {
        return !(a==b);
    }

    friend bool operator<(const ValueType &a, const ValueType &b) {
        if (a.name<b.name) return true;
        if (b.name<a.name) return false;
        if (a.name && a.negate<b.negate) return true;
        if (a.name && b.negate<a.negate) return false;
        return a.offset < b.offset;
    }
};

template<size_t Len>
std::ostream& operator<<(std::ostream &o, const ValueType<Len> &e) {
    e.print(o, NULL);
    return o;
}

/** Represents one location in memory. Has an address data and size in bytes.
 *
 *  When a state is created, every register and memory location will be given a unique named value. However, it's not
 *  practicle to store a named value for every possible memory address, yet we want the following example to work correctly:
 *  \code
 *  1: mov eax, ds:[edx]    // first read returns V1
 *  2: mov eax, ds:[edx]    // subsequent reads from same address also return V1
 *  3: mov ds:[ecx], eax    // write to unknown address clobbers all memory
 *  4: mov eax, ds:[edx]    // read from same address as above returns V2
 *  5: mov eax, ds:[edx]    // subsequent reads from same address also return V2
 *  \endcode
 *
 *  Furthermore, the read from ds:[edx] at #1 above, retroactively stores V1 in the original memory state. That way if we need
 *  to do additional analyses starting from the same initial state it will be available to use.
 *
 *  To summarize: every memory address is given a unique named value. These values are implicit until the memory location is
 *  actually read.
 *
 *  See also readMemory() and writeMemory(). */
struct MemoryCell {
    ValueType<32> address;
    ValueType<32> data;
    size_t nbytes;
    bool clobbered;             /* Set to invalidate possible aliases during writeMemory() */
    bool written;               /* Set to true by writeMemory */

    template <size_t Len>
    MemoryCell(const ValueType<32> &address, const ValueType<Len> data, size_t nbytes)
        : address(address), data(data), nbytes(nbytes), clobbered(false), written(false) {}

    bool is_clobbered() const { return clobbered; }
    void set_clobbered() { clobbered = true; }
    bool is_written() const { return written; }
    void set_written() { written = true; }

    /** Returns true if this memory value could possibly overlap with the @p other memory value.  In other words, returns false
     *  only if this memory location cannot overlap with @p other memory location. Two addresses that are identical alias one
     *  another. */
    bool may_alias(const MemoryCell &other) const;

    /** Returns true if this memory address is the same as the @p other. Note that "same" is more strict than "overlap". */
    bool must_alias(const MemoryCell &other) const;

    /** Prints the value of a memory cell on a single line. If a rename map is specified then named values will be renamed to
     *  have a shorter name.  See the ValueType<>::rename() method for details. */
    void print(std::ostream&, RenameMap *rmap=NULL) const;

    friend bool operator==(const MemoryCell &a, const MemoryCell &b) {
        return a.address==b.address && a.data==b.data && a.nbytes==b.nbytes && a.clobbered==b.clobbered && a.written==b.written;
    }
    friend bool operator!=(const MemoryCell &a, const MemoryCell &b) {
        return !(a==b);
    }
    friend bool operator<(const MemoryCell &a, const MemoryCell &b) {
        return a.address < b.address;
    }
    friend std::ostream& operator<<(std::ostream &o, const MemoryCell &me) {
        me.print(o, NULL);
        return o;
    }
};

typedef std::vector<MemoryCell> Memory;

/** Represents the entire state of the machine. However, the instruction pointer is not included in the state. */
struct State {
    static const size_t n_gprs = 8;             /**< Number of general-purpose registers in this state. */
    static const size_t n_segregs = 6;          /**< Number of segmentation registers in this state. */
    static const size_t n_flags = 16;           /**< Number of flag registers in this state. */

    ValueType<32> ip;                           /**< Instruction pointer. */
    ValueType<32> gpr[n_gprs];                  /**< General-purpose registers */
    ValueType<16> segreg[n_segregs];            /**< Segmentation registers. */
    ValueType<1> flag[n_flags];                 /**< Control/status flags (i.e., FLAG register). */
    Memory mem;                                 /**< Core memory. */

    /** Print the state in a human-friendly way.  If a rename map is specified then named values will be renamed to have a
     *  shorter name.  See the ValueType<>::rename() method for details. */
    void print(std::ostream &o, RenameMap *rmap=NULL) const;

    /** Print info about how registers differ.  If a rename map is specified then named values will be renamed to have a
     *  shorter name.  See the ValueType<>::rename() method for details. */
    void print_diff_registers(std::ostream &o, const State&, RenameMap *rmap=NULL) const;

    /** Tests registers of two states for equality. */
    bool equal_registers(const State&) const;

    /** Removes from memory those values at addresses below the current stack pointer. This is automatically called after each
     *  instruction if the policy's p_discard_popped_memory property is set. */
    void discard_popped_memory();

    friend std::ostream& operator<<(std::ostream &o, const State& state) {
        state.print(o);
        return o;
    }
};

/** A policy that is supplied to the semantic analysis constructor. */
class Policy {
private:
    SgAsmInstruction *cur_insn;         /**< Set by startInstruction(), cleared by finishInstruction() */
    mutable State orig_state;           /**< Original machine state, initialized by constructor and mem_write. This data
                                         *   member is mutable because a mem_read() operation, although conceptually const,
                                         *   may cache the value that was read so that subsquent reads from the same address
                                         *   will return the same value. This member is initialized by the first call to
                                         *   startInstruction() (as called by X86InstructionSemantics::processInstruction())
                                         *   which allows the user to initialize the original conditions using the same
                                         *   interface that's used to process instructions.  In other words, if one wants the
                                         *   stack pointer to contain a specific original value, then one may initialize the
                                         *   stack pointer by calling writeGPR() before processing the first instruction. */
    mutable State cur_state;            /**< Current machine state updated by each processInstruction().  The instruction
                                         *   pointer is updated before we process each instruction. This data member is
                                         *   mutable because a mem_read() operation, although conceptually const, may cache
                                         *   the value that was read so that subsequent reads from the same address will
                                         *   return the same value. */
    bool p_discard_popped_memory;       /**< Property that determines how the stack behaves.  When set, any time the stack
                                         * pointer is adjusted, memory below the stack pointer and having the same address
                                         * name as the stack pointer is removed (the memory location becomes undefined). The
                                         * default is false, that is, no special treatment for the stack. */
    size_t ninsns;                      /**< Total number of instructions processed. This is incremented by startInstruction(),
                                         *   which is the first thing called by X86InstructionSemantics::processInstruction(). */
    MemoryMap *map;                     /**< Initial known memory values for known addresses. */

public:
    struct Exception {
        Exception(const std::string &mesg): mesg(mesg) {}
        friend std::ostream& operator<<(std::ostream &o, const Exception &e) {
            o <<"VirtualMachineSemantics exception: " <<e.mesg;
            return o;
        }
        std::string mesg;
    };

    Policy(): cur_insn(NULL), p_discard_popped_memory(false), ninsns(0), map(NULL) {
        /* So that named values are identical in both; reinitialized by first call to startInstruction(). */
        orig_state = cur_state;
    }

    /** Set the memory map that holds known values for known memory addresses.  This map is not modified by the policy and
     *  data is read from but not written to the map. */
    void set_map(MemoryMap *map) {
        this->map = map;
    }

    /** Returns the number of instructions processed. This counter is incremented at the beginning of each instruction. */
    size_t get_ninsns() const {
        return ninsns;
    }

    /** Returns current instruction. Returns the null pointer if no instruction is being processed. */
    SgAsmInstruction *get_insn() const {
        return cur_insn;
    }

    /** Returns the current state. */
    const State& get_state() const { return cur_state; }
    State& get_state() { return cur_state; }

    /** Returns the original state.  The original state is initialized to be equal to the current state twice: once by the
     *  constructor, and then again when the first instruction is processed. */
    const State& get_orig_state() const { return orig_state; }
    State& get_orig_state() { return orig_state; }

    /** Returns the current instruction pointer. */
    const ValueType<32>& get_ip() const { return cur_state.ip; }

    /** Returns the original instruction pointer. See also get_orig_state(). */
    const ValueType<32>& get_orig_ip() const { return orig_state.ip; }

    /** Returns a copy of the state after removing memory that is not pertinent to an equal_states() comparison. */
    Memory memory_for_equality(const State&) const;

    /** Returns a copy of the current state after removing memory that is not pertinent to an equal_states() comparison. */
    Memory memory_for_equality() const { return memory_for_equality(cur_state); }

    /** Compares two states for equality. The comarison looks at all register values and the memory locations that are
     *  different than their original value (but excluding differences due to clobbering). It does not compare memory that has
     *  only been read. */
    bool equal_states(const State&, const State&) const;

    /** Print the current state of this policy.  If a rename map is specified then named values will be renamed to have a
     *  shorter name.  See the ValueType<>::rename() method for details. */
    void print(std::ostream&, RenameMap *rmap=NULL) const;
    friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
        p.print(o, NULL);
        return o;
    }

    /** Returns true if the specified value exists in memory and is provably at or above the stack pointer.  The stack pointer
     *  need not have a known value. */
    bool on_stack(const ValueType<32> &value) const;

    /** Changes how the policy treats the stack.  See the p_discard_popped_memory property data member for details. */
    void set_discard_popped_memory(bool b) {
        p_discard_popped_memory = b;
    }

    /** Returns the current setting for the property that determines how the stack behaves. See the
     *  p_set_discard_popped_memory property data member for details. */
    bool get_discard_popped_memory() const {
        return p_discard_popped_memory;
    }

    /** Print only the differences between two states.  If a rename map is specified then named values will be renamed to have a
     *  shorter name.  See the ValueType<>::rename() method for details. */
    void print_diff(std::ostream&, const State&, const State&, RenameMap *rmap=NULL) const ;

    /** Print the difference between a state and the initial state.  If a rename map is specified then named values will be
     *  renamed to have a shorter name.  See the ValueType<>::rename() method for details. */
    void print_diff(std::ostream &o, const State &state, RenameMap *rmap=NULL) const {
        print_diff(o, orig_state, state, rmap);
    }

    /** Print the difference between the current state and the initial state.  If a rename map is specified then named values
     *  will be renamed to have a shorter name.  See the ValueType<>::rename() method for details. */
    void print_diff(std::ostream &o, RenameMap *rmap=NULL) const {
        print_diff(o, orig_state, cur_state, rmap);
    }

    /** Returns the SHA1 hash of the difference between the current state and the original state.  If libgcrypt is not
     *  available then the return value will be an empty string. */
    std::string SHA1() const;

    /** Obtains the binary SHA1 hash of the difference between the current state and the original state.  The hash is returned
     *  through the @p digest argument, which must be at least 20 bytes long.  Returns true if the hash can be computed. If
     *  libgcrypt is not available then the first 20 bytes of @p digest will be set to zero and a false value is returned. */
    bool SHA1(unsigned char *digest) const;

    /** Sign extend from @p FromLen bits to @p ToLen bits. */
    template <size_t FromLen, size_t ToLen>
    ValueType<ToLen> signExtend(const ValueType<FromLen> &a) const {
        if (a.name) return ValueType<ToLen>();
        return IntegerOps::signExtend<FromLen, ToLen>(a.offset);
    }

    /** Extracts certain bits from the specified value and shifts them to the low-order positions in the result.  The bits of
     *  the result include bits from BeginAt (inclusive) through EndAt (exclusive).  The lsb is number zero. */
    template <size_t BeginAt, size_t EndAt, size_t Len>
    ValueType<EndAt-BeginAt> extract(const ValueType<Len> &a) const {
        if (0==BeginAt) return ValueType<EndAt-BeginAt>(a);
        if (a.name) return ValueType<EndAt-BeginAt>();
        return (a.offset >> BeginAt) & (IntegerOps::SHL1<uint64_t, EndAt-BeginAt>::value - 1);
    }

    /** Return a newly sized value by either truncating the most significant bits or by adding more most significant bits that
     *  are set to zeros. */
    template <size_t FromLen, size_t ToLen>
    ValueType<ToLen> extendByMSB(const ValueType<FromLen> &a) const {
        return ValueType<ToLen>(a);
    }

    /** Reads a value from memory in a way that always returns the same value provided there are not intervening writes that
     *  would clobber the value either directly or by aliasing.  Also, if appropriate, the value is added to the original
     *  memory state (thus changing the value at that address from an implicit named value to an explicit named value).
     *
     *  It is safe to call this function and supply the policy's original state as the state argument.
     *
     *  The documentation for MemoryCell has an example that demonstrates the desired behavior of mem_read() and mem_write(). */
    template <size_t Len> ValueType<Len> mem_read(State &state, const ValueType<32> &addr) const {
        MemoryCell new_cell(addr, ValueType<32>(), Len/8);
        bool aliased = false; /*is new_cell aliased by any existing writes?*/

        for (Memory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
            if (new_cell.must_alias(*mi)) {
                if ((*mi).clobbered) {
                    (*mi).clobbered = false;
                    (*mi).data = new_cell.data;
                    return new_cell.data;
                } else {
                    return (*mi).data;
                }
            } else if (new_cell.may_alias(*mi) && (*mi).written) {
                aliased = true;
            }
        }

        if (!aliased && &state!=&orig_state) {
            /* We didn't find the memory cell in the specified state and it's not aliased to any writes in that state. Therefore
             * use the value from the initial memory state (creating it if necessary). */
            for (Memory::iterator mi=orig_state.mem.begin(); mi!=orig_state.mem.end(); ++mi) {
                if (new_cell.must_alias(*mi)) {
                    ROSE_ASSERT(!(*mi).clobbered);
                    ROSE_ASSERT(!(*mi).written);
                    state.mem.push_back(*mi);
                    std::sort(state.mem.begin(), state.mem.end());
                    return (*mi).data;
                }
            }

            /* Not found in intial state. But if we have a known address and a valid memory map then initialize the original
             * state with data from the memory map. */      
            if (map && addr.is_known()) {
                uint8_t buf[sizeof(uint64_t)];
                ROSE_ASSERT(Len/8 < sizeof buf);
                size_t nread = map->read(buf, addr.known_value(), Len/8);
                if (nread==Len/8) {
                    uint64_t n = 0;
                    for (size_t i=0; i<Len/8; i++)
                        n |= buf[i] << (8*i);
                    new_cell.data = number<32>(n);
                }
            }

            orig_state.mem.push_back(new_cell);
            std::sort(orig_state.mem.begin(), orig_state.mem.end());
        }

        /* Create the cell in the current state. */
        state.mem.push_back(new_cell);
        std::sort(state.mem.begin(), state.mem.end());
        return new_cell.data;
    }

    /** See memory_reference_type(). */
    enum MemRefType { MRT_STACK_PTR, MRT_FRAME_PTR, MRT_OTHER_PTR };

    /** Determines if the specified address is related to the current stack or frame pointer. This is used by mem_write() when
     *  we're operating under the assumption that memory written via stack pointer is different than memory written via frame
     *  pointer, and that memory written by either pointer is different than all other memory. */
    MemRefType memory_reference_type(const State &state, const ValueType<32> &addr) const {
        if (addr.name) {
            if (addr.name==state.gpr[x86_gpr_sp].name) return MRT_STACK_PTR;
            if (addr.name==state.gpr[x86_gpr_bp].name) return MRT_FRAME_PTR;
            return MRT_OTHER_PTR;
        }
        if (addr==state.gpr[x86_gpr_sp]) return MRT_STACK_PTR;
        if (addr==state.gpr[x86_gpr_bp]) return MRT_FRAME_PTR;
        return MRT_OTHER_PTR;
    }

    /** Writes a value to memory. If the address written to is an alias for other addresses then the other addresses will be
     *  clobbered. Subsequent reads from clobbered addresses will return new values. See also, mem_read(). */
    template <size_t Len> void mem_write(State &state, const ValueType<32> &addr, const ValueType<Len> &data) {
        ROSE_ASSERT(&state!=&orig_state);
        MemoryCell new_cell(addr, data, Len/8);
        new_cell.set_written();
        bool saved = false; /* has new_cell been saved into memory? */

        /* Is new memory reference through the stack pointer or frame pointer? */
        MemRefType new_mrt = memory_reference_type(state, addr);

        /* Overwrite and/or clobber existing memory locations. */
        for (Memory::iterator mi=state.mem.begin(); mi!=state.mem.end(); ++mi) {
            if (new_cell.must_alias(*mi)) {
                *mi = new_cell;
                saved = true;
            } else if (p_discard_popped_memory && new_mrt!=memory_reference_type(state, (*mi).address)) {
                /* Assume that memory referenced through the stack pointer does not alias that which is referenced through the
                 * frame pointer, and neither of them alias memory that is referenced other ways. */
            } else if (new_cell.may_alias(*mi)) {
                (*mi).set_clobbered();
            } else {
                /* memory cell *mi is not aliased to cell being written */
            }
        }
        if (!saved) {
            state.mem.push_back(new_cell);
            std::sort(state.mem.begin(), state.mem.end());
        }
    }


    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for every processed instructions
     *************************************************************************************************************************/

    /* Called at the beginning of X86InstructionSemantics::processInstruction() */
    void startInstruction(SgAsmInstruction *insn) {
        cur_state.ip = ValueType<32>(insn->get_address());
        if (0==ninsns++)
            orig_state = cur_state;
        cur_insn = insn;
    }

    /* Called at the end of X86InstructionSemantics::processInstruction() */
    void finishInstruction(SgAsmInstruction*) {
        if (p_discard_popped_memory)
            cur_state.discard_popped_memory();
        cur_insn = NULL;
    }



    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class to construct values
     *************************************************************************************************************************/

    /** True value */
    ValueType<1> true_() const {
        return 1;
    }

    /** False value */
    ValueType<1> false_() const {
        return 0;
    }

    /** Undefined Boolean */
    ValueType<1> undefined_() const {
        return ValueType<1>();
    }

    /** Used to build a known constant. */
    template <size_t Len>
    ValueType<Len> number(uint64_t n) const {
        return n;
    }



    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for individual instructions
     *************************************************************************************************************************/

    /** Called only for CALL instructions before assigning new value to IP register. */
    ValueType<32> filterCallTarget(const ValueType<32> &a) const {
        return a;
    }

    /** Called only for RET instructions before adjusting the IP register. */
    ValueType<32> filterReturnTarget(const ValueType<32> &a) const {
        return a;
    }

    /** Called only for JMP instructions before adjusting the IP register. */
    ValueType<32> filterIndirectJumpTarget(const ValueType<32> &a) const {
        return a;
    }

    /** Called only for the HLT instruction. */
    void hlt() {} // FIXME

    /** Called only for the RDTSC instruction. */
    ValueType<64> rdtsc() {
        return 0;
    }

    /** Called only for the INT instruction. */
    void interrupt(uint8_t num) {
        cur_state = State(); /*reset entire machine state*/
    }

    /** Called only for the SYSENTER instruction. */
    void sysenter() {
        cur_state = State(); /*reset entire machine state*/
    }



    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for data access operations
     *************************************************************************************************************************/

    /** Returns value of the specified 32-bit general purpose register. */
    ValueType<32> readGPR(X86GeneralPurposeRegister r) const {
        return cur_state.gpr[r];
    }

    /** Places a value in the specified 32-bit general purpose register. */
    void writeGPR(X86GeneralPurposeRegister r, const ValueType<32> &value) {
        cur_state.gpr[r] = value;
    }

    /** Reads a value from the specified 16-bit segment register. */
    ValueType<16> readSegreg(X86SegmentRegister sr) const {
        return cur_state.segreg[sr];
    }

    /** Places a value in the specified 16-bit segment register. */
    void writeSegreg(X86SegmentRegister sr, const ValueType<16> &value) {
        cur_state.segreg[sr] = value;
    }

    /** Returns the value of the instruction pointer as it would be during the execution of the instruction. In other words,
     *  it points to the first address past the end of the current instruction. */
    ValueType<32> readIP() const {
        return cur_state.ip;
    }

    /** Changes the value of the instruction pointer. */
    void writeIP(const ValueType<32> &value) {
        cur_state.ip = value;
    }

    /** Returns the value of a specific control/status/system flag. */
    ValueType<1> readFlag(X86Flag f) const {
        return cur_state.flag[f];
    }

    /** Changes the value of the specified control/status/system flag. */
    void writeFlag(X86Flag f, const ValueType<1> &value) {
        cur_state.flag[f] = value;
    }

    /** Reads a value from memory. */
    template <size_t Len> ValueType<Len>
    readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> cond) const {
        return mem_read<Len>(cur_state, addr);
    }

    /** Writes a value to memory. */
    template <size_t Len> void
    writeMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<Len> &data, ValueType<1> cond) {
        mem_write<Len>(cur_state, addr, data);
    }



    /*************************************************************************************************************************
     * Functions invoked by the X86InstructionSemantics class for arithmetic operations
     *************************************************************************************************************************/

    /** Adds two values. */
    template <size_t Len>
    ValueType<Len> add(const ValueType<Len> &a, const ValueType<Len> &b) const {
        if (a.name==b.name && (!a.name || a.negate!=b.negate)) {
            /* [V1+x] + [-V1+y] = [x+y]  or
             * [x] + [y] = [x+y] */
            return a.offset + b.offset;
        } else if (!a.name || !b.name) {
            /* [V1+x] + [y] = [V1+x+y]   or
             * [x] + [V2+y] = [V2+x+y]   or
             * [-V1+x] + [y] = [-V1+x+y] or
             * [x] + [-V2+y] = [-V2+x+y] */
            return ValueType<Len>(a.name+b.name, a.offset+b.offset, a.negate || b.negate);
        } else {
            return ValueType<Len>();
        }
    }

    /** Add two values of equal size and a carry bit.  Carry information is returned via carry_out argument.  The carry_out
     *  value is the tick marks that are written above the first addend when doing long arithmetic like a 2nd grader would do
     *  (of course, they'd probably be adding two base-10 numbers).  For instance, when adding 00110110 and 11100100:
     *
     *  \code
     *    '''..'..         <-- carry tick marks: '=carry .=no carry
     *     00110110
     *   + 11100100
     *   ----------
     *    100011010
     *  \endcode
     *
     *  The carry_out value is 11100100.
     */
    template <size_t Len>
    ValueType<Len> addWithCarries(const ValueType<Len> &a, const ValueType<Len> &b, const ValueType<1> &c,
                                  ValueType<Len> &carry_out) const {
        int n_unknown = (a.name?1:0) + (b.name?1:0) + (c.name?1:0);
        if (n_unknown <= 1) {
            /* At most, one of the operands is an unknown value. See add() for more details. */
            uint64_t sum = a.offset + b.offset + c.offset;
            carry_out = 0==n_unknown ? ValueType<Len>((a.offset ^ b.offset ^ sum)>>1) : ValueType<Len>();
            return ValueType<Len>(a.name+b.name+c.name, sum, a.negate||b.negate||c.negate);
        } else if (a.name==b.name && !c.name && a.negate!=b.negate) {
            /* A and B are known or have bases that cancel out, and C is known */
            uint64_t sum = a.offset + b.offset + c.offset;
            carry_out = ValueType<Len>((a.offset ^ b.offset ^ sum)>>1);
            return ValueType<Len>(sum);
        } else {
            carry_out = ValueType<Len>();
            return ValueType<Len>();
        }
    }

    /** Computes bit-wise AND of two values. */
    template <size_t Len>
    ValueType<Len> and_(const ValueType<Len> &a, const ValueType<Len> &b) const {
        if ((!a.name && 0==a.offset) || (!b.name && 0==b.offset)) return 0;
        if (a.name || b.name) return ValueType<Len>();
        return a.offset & b.offset;
    }

    /** Returns true_, false_, or undefined_ depending on whether argument is zero. */
    template <size_t Len>
    ValueType<1> equalToZero(const ValueType<Len> &a) const {
        if (a.name) return undefined_();
        return a.offset ? false_() : true_();
    }

    /** One's complement */
    template <size_t Len>
    ValueType<Len> invert(const ValueType<Len> &a) const {
        if (a.name) return ValueType<Len>(a.name, ~a.offset, !a.negate);
        return ~a.offset;
    }

    /** Concatenate the values of @p a and @p b so that the result has @p b in the high-order bits and @p a in the low order
     *  bits. */
    template<size_t Len1, size_t Len2>
    ValueType<Len1+Len2> concat(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        if (a.name || b.name) return ValueType<Len1+Len2>();
        return a.offset | (b.offset << Len1);
    }

    /** Returns second or third arg depending on value of first arg. */
    template <size_t Len>
    ValueType<Len> ite(const ValueType<1> &sel, const ValueType<Len> &ifTrue, const ValueType<Len> &ifFalse) const {
        if (ifTrue==ifFalse) return ifTrue;
        if (sel.name) return ValueType<Len>();
        return sel.offset ? ifTrue : ifFalse;
    }

    /** Returns position of least significant set bit; zero when no bits are set. */
    template <size_t Len>
    ValueType<Len> leastSignificantSetBit(const ValueType<Len> &a) const {
        if (a.name) return ValueType<Len>();
        for (size_t i=0; i<Len; ++i) {
            if (a.offset & ((uint64_t)1 << i))
                return i;
        }
        return 0;
    }

    /** Returns position of most significant set bit; zero when no bits are set. */
    template <size_t Len>
    ValueType<Len> mostSignificantSetBit(const ValueType<Len> &a) const {
        if (a.name) return ValueType<Len>();
        for (size_t i=Len; i>0; --i) {
            if (a.offset & ((uint64_t)1 << (i-1)))
                return i-1;
        }
        return 0;
    }

    /** Two's complement. */
    template <size_t Len>
    ValueType<Len> negate(const ValueType<Len> &a) const {
        if (a.name) return ValueType<Len>(a.name, -a.offset, !a.negate);
        return -a.offset;
    }

    /** Computes bit-wise OR of two values. */
    template <size_t Len>
    ValueType<Len> or_(const ValueType<Len> &a, const ValueType<Len> &b) const {
        if (a==b) return a;
        if (!a.name && !b.name) return a.offset | b.offset;
        if (!a.name && a.offset==IntegerOps::GenMask<uint64_t, Len>::value) return a;
        if (!b.name && b.offset==IntegerOps::GenMask<uint64_t, Len>::value) return b;
        return ValueType<Len>();
    }

    /** Rotate bits to the left. */
    template <size_t Len, size_t SALen>
    ValueType<Len> rotateLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        if (!a.name && !sa.name) return IntegerOps::rotateLeft<Len>(a.offset, sa.offset);
        if (!sa.name && 0==sa.offset % Len) return a;
        return ValueType<Len>();
    }

    /** Rotate bits to the right. */
    template <size_t Len, size_t SALen>
    ValueType<Len> rotateRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        if (!a.name && !sa.name) return IntegerOps::rotateRight<Len>(a.offset, sa.offset);
        if (!sa.name && 0==sa.offset % Len) return a;
        return ValueType<Len>();
    }

    /** Returns arg shifted left. */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        if (!a.name && !sa.name) return IntegerOps::shiftLeft<Len>(a.offset, sa.offset);
        if (!sa.name) {
            if (0==sa.offset) return a;
            if (sa.offset>=Len) return 0;
        }
        return ValueType<Len>();
    }

    /** Returns arg shifted right logically (no sign bit). */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        if (!a.name && !sa.name) return IntegerOps::shiftRightLogical<Len>(a.offset, sa.offset);
        if (!sa.name) {
            if (0==sa.offset) return a;
            if (sa.offset>=Len) return 0;
        }
        return ValueType<Len>();
    }

    /** Returns arg shifted right arithmetically (with sign bit). */
    template <size_t Len, size_t SALen>
    ValueType<Len> shiftRightArithmetic(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
        if (!a.name && !sa.name) return IntegerOps::shiftRightArithmetic<Len>(a.offset, sa.offset);
        if (!sa.name && 0==sa.offset) return a;
        return ValueType<Len>();
    }

    /** Divides two signed values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1> signedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        if (!b.name) {
            if (0==b.offset) throw Exception("division by zero");
            if (!a.name) return IntegerOps::signExtend<Len1, 64>(a.offset) / IntegerOps::signExtend<Len2, 64>(b.offset);
            if (1==b.offset) return a;
            if (b.offset==IntegerOps::GenMask<uint64_t,Len2>::value) return negate(a);
            /*FIXME: also possible to return zero if B is large enough. [RPM 2010-05-18]*/
        }
        return ValueType<Len1>();
    }

    /** Calculates modulo with signed values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len2> signedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        if (a.name || b.name) return ValueType<Len2>();
        if (0==b.offset) throw Exception("division by zero");
        return IntegerOps::signExtend<Len1, 64>(a.offset) % IntegerOps::signExtend<Len2, 64>(b.offset);
        /* FIXME: More folding possibilities... if 'b' is a power of two then we can return 'a' with the bitsize of 'b'. */
    }

    /** Multiplies two signed values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1+Len2> signedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        if (!a.name && !b.name)
            return IntegerOps::signExtend<Len1, 64>(a.offset) * IntegerOps::signExtend<Len2, 64>(b.offset);
        if (!b.name) {
            if (0==b.offset) return 0;
            if (1==b.offset) return signExtend<Len1, Len1+Len2>(a);
            if (b.offset==IntegerOps::GenMask<uint64_t,Len2>::value) return signExtend<Len1, Len1+Len2>(negate(a));
        }
        if (!a.name) {
            if (0==a.offset) return 0;
            if (1==a.offset) return signExtend<Len2, Len1+Len2>(b);
            if (a.offset==IntegerOps::GenMask<uint64_t,Len1>::value) return signExtend<Len2, Len1+Len2>(negate(b));
        }
        return ValueType<Len1+Len2>();
    }

    /** Divides two unsigned values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1> unsignedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        if (!b.name) {
            if (0==b.offset) throw Exception("division by zero");
            if (!a.name) return a.offset / b.offset;
            if (1==b.offset) return a;
            /*FIXME: also possible to return zero if B is large enough. [RPM 2010-05-18]*/
        }
        return ValueType<Len1>();
    }

    /** Calculates modulo with unsigned values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len2> unsignedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        if (!b.name) {
            if (0==b.offset) throw Exception("division by zero");
            if (!a.name) return a.offset % b.offset;
            /* FIXME: More folding possibilities... if 'b' is a power of two then we can return 'a' with the bitsize of 'b'. */
        }
        if (extendByMSB<Len1,64>(a)==extendByMSB<Len2,64>(b)) return b;
        return ValueType<Len2>();
    }

    /** Multiply two unsigned values. */
    template <size_t Len1, size_t Len2>
    ValueType<Len1+Len2> unsignedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
        if (!a.name && !b.name)
            return a.offset * b.offset;
        if (!b.name) {
            if (0==b.offset) return 0;
            if (1==b.offset) return extendByMSB<Len1, Len1+Len2>(a);
        }
        if (!a.name) {
            if (0==a.offset) return 0;
            if (1==a.offset) return extendByMSB<Len2, Len1+Len2>(b);
        }
        return ValueType<Len1+Len2>();
    }

    /** Computes bit-wise XOR of two values. */
    template <size_t Len>
    ValueType<Len> xor_(const ValueType<Len> &a, const ValueType<Len> &b) const {
        if (!a.name && !b.name)
            return a.offset ^ b.offset;
        if (a==b)
            return 0;
        if (!b.name) {
            if (0==b.offset) return a;
            if (b.offset==IntegerOps::GenMask<uint64_t, Len>::value) return invert(a);
        }
        if (!a.name) {
            if (0==a.offset) return b;
            if (a.offset==IntegerOps::GenMask<uint64_t, Len>::value) return invert(b);
        }
        return ValueType<Len>();
    }
};

}; /*namespace*/


#endif
