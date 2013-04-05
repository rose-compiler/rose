#ifndef Rose_PartialSymbolicSemantics_H
#define Rose_PartialSymbolicSemantics_H


#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include <map>
#include <stdint.h>
#include <vector>

#include "rosePublicConfig.h"
#ifdef ROSE_HAVE_GCRYPT_H
#include <gcrypt.h>
#endif

#include "x86InstructionSemantics.h"
#include "BaseSemantics.h"
#include "MemoryMap.h"
#include "FormatRestorer.h"

namespace BinaryAnalysis {                      // documented elsewhere
    namespace InstructionSemantics {            // documented elsewhere


        /** A fast, partially symbolic semantic domain.
         *
         *  This policy can be used to emulate the execution of a single basic block of instructions.  It is similar in nature
         *  to the FindConstantsPolicy except much simpler, much faster, and much more memory-lean.  The main classes are:
         *
         *  <ul>
         *    <li>Policy: the policy class used to instantiate X86InstructionSemantic instances.</li>
         *    <li>State: represents the state of the virtual machine, its registers and memory.</li>
         *    <li>ValueType: the values stored in registers and memory and used for memory addresses.</li>
         *  </ul>
         *
         *  Each value is either a known value or an unknown value. An unknown value consists of a base name and offset and
         *  whether the value is negated. */
        namespace PartialSymbolicSemantics {

            extern uint64_t name_counter;

            typedef std::map<uint64_t, uint64_t> RenameMap;

            /** A value is either known or unknown. Unknown values have a base name (unique ID number), offset, and sign. */
            template<size_t nBits>
            struct ValueType {
                uint64_t name;                      /**< Zero for constants; non-zero ID number for everything else. */
                uint64_t offset;                    /**< The constant (if name==0) or an offset w.r.t. an unknown (named) base
                                                     *    value. */
                bool negate;                        /**< Switch between name+offset and (-name)+offset; should be false for
                                                     *    constants. */

                /** Construct a value that is unknown and unique. */
                ValueType(): name(++name_counter), offset(0), negate(false) {}

                /** Copy-construct a value, truncating or extending at msb the source value. */
                template <size_t Len>
                ValueType(const ValueType<Len> &other) {
                    name = other.name;
                    /*make sure high-order bits are zeroed out*/
                    offset = other.offset & IntegerOps::GenMask<uint64_t, nBits>::value;
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

                /** Returns a new, optionally renamed, value.  If the rename map, @p rmap, is non-null and this value is a
                 *  named value, then its name will be transformed by looking up the name in the map and using the value found
                 *  there. If the name is not in the map then a new entry is created in the map.  Remapped names start counting
                 *  from one.  For example, if "v904885611+0xfc" is the first value to be renamed, it will become "v1+0xfc". */
                ValueType<nBits> rename(RenameMap *rmap=NULL) const;

                /** Print the value. If a rename map is specified a named value will be renamed to have a shorter name.  See the
                 *  rename() method for details. */
                void print(std::ostream &o, RenameMap *rmap=NULL) const {
                    FormatRestorer restorer(o); // restore format flags when we leave this scope
                    uint64_t sign_bit = (uint64_t)1 << (nBits-1); /* e.g., 80000000 */
                    uint64_t val_mask = sign_bit - 1;             /* e.g., 7fffffff */
                    /*magnitude of negative value*/
                    uint64_t negative = nBits>1 && (offset & sign_bit) ? (~offset & val_mask) + 1 : 0;

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
                void print(std::ostream &o, BaseSemantics::SEMANTIC_NO_PRINT_HELPER *unused=NULL) const {
                    print(o, (RenameMap*)0);
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
                e.print(o, (RenameMap*)0);
                return o;
            }

            /** Memory cell with partially symbolic address and data.  The ValueType template argument should be a subclass of
             * PartialSymbolicSemantics::ValueType. */
            template <template <size_t> class ValueType=PartialSymbolicSemantics::ValueType>
            class MemoryCell: public BaseSemantics::MemoryCell<ValueType> {
            public:

                /** Constructor same as super class. */
                template <size_t Len>
                MemoryCell(const ValueType<32> &address, const ValueType<Len> data, size_t nbytes)
                    : BaseSemantics::MemoryCell<ValueType>(address, data, nbytes) {}

                /** Returns true if this memory value could possibly overlap with the @p other memory value.  In other words,
                 *  returns false only if this memory location cannot overlap with @p other memory location. Two addresses that
                 *  are identical alias one another. */
                bool may_alias(const MemoryCell &other) const;

                /** Returns true if this memory address is the same as the @p other. Note that "same" is more strict than
                 * "overlap". */
                bool must_alias(const MemoryCell &other) const;

                friend bool operator==(const MemoryCell &a, const MemoryCell &b) {
                    return (a.get_address()==b.get_address() &&
                            a.get_data()==b.get_data() &&
                            a.get_nbytes()==b.get_nbytes() &&
                            a.get_clobbered()==b.get_clobbered() &&
                            a.get_written()==b.get_written());
                }
                friend bool operator!=(const MemoryCell &a, const MemoryCell &b) {
                    return !(a==b);
                }
                friend bool operator<(const MemoryCell &a, const MemoryCell &b) {
                    return a.get_address() < b.get_address();
                }
            };

            /** Represents the entire state of the machine. */
            template <template <size_t> class ValueType=PartialSymbolicSemantics::ValueType>
            struct State: public BaseSemantics::StateX86<MemoryCell, ValueType> {
                typedef typename BaseSemantics::StateX86<MemoryCell, ValueType>::Memory Memory;

                /** Print info about how registers differ.  If a rename map is specified then named values will be renamed to
                 *  have a shorter name.  See the ValueType<>::rename() method for details. */
                void print_diff_registers(std::ostream &o, const State&, RenameMap *rmap=NULL) const;

                /** Tests registers of two states for equality. */
                bool equal_registers(const State&) const;

                /** Removes from memory those values at addresses below the current stack pointer. This is automatically called
                 *  after each instruction if the policy's p_discard_popped_memory property is set. */
                void discard_popped_memory();
            };

            /** A policy that is supplied to the semantic analysis constructor. */
            template<
                template <template <size_t> class ValueType> class State = PartialSymbolicSemantics::State,
                template <size_t nBits> class ValueType = PartialSymbolicSemantics::ValueType
                >
            class Policy: public BaseSemantics::Policy {
            protected:
                SgAsmInstruction *cur_insn;         /**< Set by startInstruction(), cleared by finishInstruction() */
                mutable State<ValueType> orig_state;/**< Original machine state, initialized by constructor and mem_write. This
                                                     *   data member is mutable because a mem_read() operation, although
                                                     *   conceptually const, may cache the value that was read so that
                                                     *   subsquent reads from the same address will return the same value. This
                                                     *   member is initialized by the first call to startInstruction() (as
                                                     *   called by X86InstructionSemantics::processInstruction()) which allows
                                                     *   the user to initialize the original conditions using the same
                                                     *   interface that's used to process instructions.  In other words, if one
                                                     *   wants the stack pointer to contain a specific original value, then one
                                                     *   may initialize the stack pointer by calling writeGPR() before
                                                     *   processing the first instruction. */
                mutable State<ValueType> cur_state;/**< Current machine state updated by each processInstruction().  The
                                                     *   instruction pointer is updated before we process each
                                                     *   instruction. This data member is mutable because a mem_read()
                                                     *   operation, although conceptually const, may cache the value that was
                                                     *   read so that subsequent reads from the same address will return the
                                                     *   same value. */
                bool p_discard_popped_memory;       /**< Property that determines how the stack behaves.  When set, any time
                                                     * the stack pointer is adjusted, memory below the stack pointer and having
                                                     * the same address name as the stack pointer is removed (the memory
                                                     * location becomes undefined). The default is false, that is, no special
                                                     * treatment for the stack. */
                size_t ninsns;                      /**< Total number of instructions processed. This is incremented by
                                                     *   startInstruction(), which is the first thing called by
                                                     *   X86InstructionSemantics::processInstruction(). */
                MemoryMap *map;                     /**< Initial known memory values for known addresses. */

            public:
                typedef State<ValueType> StateType;

                Policy(): cur_insn(NULL), p_discard_popped_memory(false), ninsns(0), map(NULL) {
                    /* So that named values are identical in both; reinitialized by first call to startInstruction(). */
                    set_register_dictionary(RegisterDictionary::dictionary_pentium4());
                    orig_state = cur_state;
                }

                /** Set the memory map that holds known values for known memory addresses.  This map is not modified by the
                 *  policy and data is read from but not written to the map. */
                void set_map(MemoryMap *map) {
                    this->map = map;
                }

                /** Returns the number of instructions processed. This counter is incremented at the beginning of each
                 *  instruction. */
                size_t get_ninsns() const {
                    return ninsns;
                }

                /** Sets the number instructions processed. This is the same counter incremented at the beginning of each
                 *  instruction and returned by get_ninsns(). */
                void set_ninsns(size_t n) {
                    ninsns = n;
                }

                /** Returns current instruction. Returns the null pointer if no instruction is being processed. */
                SgAsmInstruction *get_insn() const {
                    return cur_insn;
                }

                /** Returns the current state.
                 * @{ */
                const State<ValueType>& get_state() const { return cur_state; }
                State<ValueType>& get_state() { return cur_state; }
                /** @} */

                /** Returns the original state.  The original state is initialized to be equal to the current state twice: once
                 *  by the constructor, and then again when the first instruction is processed. */
                const State<ValueType>& get_orig_state() const { return orig_state; }
                State<ValueType>& get_orig_state() { return orig_state; }

                /** Returns the current instruction pointer. */
                const ValueType<32>& get_ip() const { return cur_state.registers.ip; }

                /** Returns the original instruction pointer. See also get_orig_state(). */
                const ValueType<32>& get_orig_ip() const { return orig_state.ip; }

                /** Returns a copy of the state after removing memory that is not pertinent to an equal_states() comparison. */
                typename State<ValueType>::Memory memory_for_equality(const State<ValueType>&) const;

                /** Returns a copy of the current state after removing memory that is not pertinent to an equal_states()
                 *  comparison. */
                typename State<ValueType>::Memory memory_for_equality() const { return memory_for_equality(cur_state); }

                /** Compares two states for equality. The comarison looks at all register values and the memory locations that
                 *  are different than their original value (but excluding differences due to clobbering). It does not compare
                 *  memory that has only been read. */
                bool equal_states(const State<ValueType>&, const State<ValueType>&) const;

                /** Print the current state of this policy.  If a rename map is specified then named values will be renamed to
                 *  have a shorter name.  See the ValueType<>::rename() method for details. */
                void print(std::ostream&, RenameMap *rmap=NULL) const;
                friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
                    p.print(o, NULL);
                    return o;
                }

                /** Returns true if the specified value exists in memory and is provably at or above the stack pointer.  The
                 *  stack pointer need not have a known value. */
                bool on_stack(const ValueType<32> &value) const;

                /** Changes how the policy treats the stack.  See the p_discard_popped_memory property data member for
                 *  details. */
                void set_discard_popped_memory(bool b) {
                    p_discard_popped_memory = b;
                }

                /** Returns the current setting for the property that determines how the stack behaves. See the
                 *  p_set_discard_popped_memory property data member for details. */
                bool get_discard_popped_memory() const {
                    return p_discard_popped_memory;
                }

                /** Print only the differences between two states.  If a rename map is specified then named values will be
                 *  renamed to have a shorter name.  See the ValueType<>::rename() method for details. */
                void print_diff(std::ostream&, const State<ValueType>&, const State<ValueType>&, RenameMap *rmap=NULL) const ;

                /** Print the difference between a state and the initial state.  If a rename map is specified then named values
                 *  will be renamed to have a shorter name.  See the ValueType<>::rename() method for details. */
                void print_diff(std::ostream &o, const State<ValueType> &state, RenameMap *rmap=NULL) const {
                    print_diff(o, orig_state, state, rmap);
                }

                /** Print the difference between the current state and the initial state.  If a rename map is specified then
                 *  named values will be renamed to have a shorter name.  See the ValueType<>::rename() method for details. */
                void print_diff(std::ostream &o, RenameMap *rmap=NULL) const {
                    print_diff(o, orig_state, cur_state, rmap);
                }

                /** Returns the SHA1 hash of the difference between the current state and the original state.  If libgcrypt is
                 *  not available then the return value will be an empty string. */
                std::string SHA1() const;

                /** Obtains the binary SHA1 hash of the difference between the current state and the original state.  The hash
                 *  is returned through the @p digest argument, which must be at least 20 bytes long.  Returns true if the hash
                 *  can be computed. If libgcrypt is not available then the first 20 bytes of @p digest will be set to zero and
                 *  a false value is returned. */
                bool SHA1(unsigned char *digest) const;

                /** Sign extend from @p FromLen bits to @p ToLen bits. */
                template <size_t FromLen, size_t ToLen>
                ValueType<ToLen> signExtend(const ValueType<FromLen> &a) const {
                    if (ToLen==FromLen) return a;
                    if (a.name) return ValueType<ToLen>();
                    return ValueType<ToLen>(IntegerOps::signExtend<FromLen, ToLen>(a.offset));
                }

                /** See NullSemantics::Policy::extract() */
                template <size_t BeginAt, size_t EndAt, size_t Len>
                ValueType<EndAt-BeginAt> extract(const ValueType<Len> &a) const {
                    if (0==BeginAt) return ValueType<EndAt-BeginAt>(a);
                    if (a.name) return ValueType<EndAt-BeginAt>();
                    return ValueType<EndAt-BeginAt>((a.offset >> BeginAt) &
                                                    (IntegerOps::SHL1<uint64_t, EndAt-BeginAt>::value - 1));
                }

                /** Return a newly sized value by either truncating the most significant bits or by adding more most significant
                 *  bits that are set to zeros. */
                template <size_t FromLen, size_t ToLen>
                ValueType<ToLen> unsignedExtend(const ValueType<FromLen> &a) const {
                    return ValueType<ToLen>(a);
                }

                /** Reads a value from memory in a way that always returns the same value provided there are not intervening
                 *  writes that would clobber the value either directly or by aliasing.  Also, if appropriate, the value is
                 *  added to the original memory state (thus changing the value at that address from an implicit named value to
                 *  an explicit named value).
                 *
                 *  It is safe to call this function and supply the policy's original state as the state argument.
                 *
                 *  The documentation for MemoryCell has an example that demonstrates the desired behavior of mem_read() and
                 *  mem_write(). */
                template <size_t Len> ValueType<Len> mem_read(State<ValueType> &state, const ValueType<32> &addr) const {
                    MemoryCell<ValueType> new_cell(addr, ValueType<32>(), Len/8);
                    bool aliased = false; /*is new_cell aliased by any existing writes?*/

                    for (typename State<ValueType>::Memory::iterator mi=state.memory.begin(); mi!=state.memory.end(); ++mi) {
                        if (new_cell.must_alias(*mi)) {
                            if (mi->is_clobbered()) {
                                mi->set_clobbered(false);
                                mi->set_data(new_cell.get_data());
                                return new_cell.get_data();
                            } else {
                                return mi->get_data();
                            }
                        } else if (new_cell.may_alias(*mi) && mi->is_written()) {
                            aliased = true;
                        }
                    }

                    if (!aliased && &state!=&orig_state) {
                        /* We didn't find the memory cell in the specified state and it's not aliased to any writes in that
                         * state. Therefore use the value from the initial memory state (creating it if necessary). */
                        for (typename State<ValueType>::Memory::iterator mi=orig_state.memory.begin();
                             mi!=orig_state.memory.end();
                             ++mi) {
                            if (new_cell.must_alias(*mi)) {
                                ROSE_ASSERT(!(*mi).is_clobbered());
                                ROSE_ASSERT(!(*mi).is_written());
                                state.memory.push_back(*mi);
                                return (*mi).get_data();
                            }
                        }

                        /* Not found in initial state. But if we have a known address and a valid memory map then initialize
                         * the original state with data from the memory map. */      
                        if (map && addr.is_known()) {
                            uint8_t buf[sizeof(uint64_t)];
                            ROSE_ASSERT(Len/8 < sizeof buf);
                            size_t nread = map->read(buf, addr.known_value(), Len/8);
                            if (nread==Len/8) {
                                uint64_t n = 0;
                                for (size_t i=0; i<Len/8; i++)
                                    n |= buf[i] << (8*i);
                                new_cell.set_data(number<32>(n));
                            }
                        }

                        orig_state.memory.push_back(new_cell);
                    }

                    /* Create the cell in the current state. */
                    state.memory.push_back(new_cell);
                    return new_cell.get_data();
                }

                /** See memory_reference_type(). */
                enum MemRefType { MRT_STACK_PTR, MRT_FRAME_PTR, MRT_OTHER_PTR };

                /** Determines if the specified address is related to the current stack or frame pointer. This is used by
                 *  mem_write() when we're operating under the assumption that memory written via stack pointer is different
                 *  than memory written via frame pointer, and that memory written by either pointer is different than all
                 *  other memory. */
                MemRefType memory_reference_type(const State<ValueType> &state, const ValueType<32> &addr) const {
                    if (addr.name) {
                        if (addr.name==state.registers.gpr[x86_gpr_sp].name) return MRT_STACK_PTR;
                        if (addr.name==state.registers.gpr[x86_gpr_bp].name) return MRT_FRAME_PTR;
                        return MRT_OTHER_PTR;
                    }
                    if (addr==state.registers.gpr[x86_gpr_sp]) return MRT_STACK_PTR;
                    if (addr==state.registers.gpr[x86_gpr_bp]) return MRT_FRAME_PTR;
                    return MRT_OTHER_PTR;
                }

                /** Writes a value to memory. If the address written to is an alias for other addresses then the other
                 *  addresses will be clobbered. Subsequent reads from clobbered addresses will return new values. See also,
                 *  mem_read(). */
                template <size_t Len> void mem_write(State<ValueType> &state, const ValueType<32> &addr,
                                                     const ValueType<Len> &data) {
                    ROSE_ASSERT(&state!=&orig_state);
                    MemoryCell<ValueType> new_cell(addr, data, Len/8);
                    new_cell.set_written();
                    bool saved = false; /* has new_cell been saved into memory? */

                    /* Is new memory reference through the stack pointer or frame pointer? */
                    MemRefType new_mrt = memory_reference_type(state, addr);

                    /* Overwrite and/or clobber existing memory locations. */
                    for (typename State<ValueType>::Memory::iterator mi=state.memory.begin(); mi!=state.memory.end(); ++mi) {
                        if (new_cell.must_alias(*mi)) {
                            *mi = new_cell;
                            saved = true;
                        } else if (p_discard_popped_memory && new_mrt!=memory_reference_type(state, (*mi).get_address())) {
                            /* Assume that memory referenced through the stack pointer does not alias that which is referenced
                             * through the frame pointer, and neither of them alias memory that is referenced other ways. */
                        } else if (new_cell.may_alias(*mi)) {
                            (*mi).set_clobbered();
                        } else {
                            /* memory cell *mi is not aliased to cell being written */
                        }
                    }
                    if (!saved)
                        state.memory.push_back(new_cell);
                }

                /*************************************************************************************************************
                 * Functions invoked by the X86InstructionSemantics class for every processed instruction or block
                 *************************************************************************************************************/

                /** See NullSemantics::Policy::startInstruction() */
                void startInstruction(SgAsmInstruction *insn) {
                    cur_state.registers.ip = ValueType<32>(insn->get_address());
                    if (0==ninsns++)
                        orig_state = cur_state;
                    cur_insn = insn;
                }

                /** See NullSemantics::Policy::finishInstruction() */
                void finishInstruction(SgAsmInstruction*) {
                    if (p_discard_popped_memory)
                        cur_state.discard_popped_memory();
                    cur_insn = NULL;
                }

                /* Called at the beginning of X86InstructionSemantics::processBlock() */
                void startBlock(rose_addr_t addr) {}

                /* Called at the end of X86InstructionSemantics::processBlock() */
                void finishBlock(rose_addr_t addr) {}

                /*************************************************************************************************************
                 * Functions invoked by the X86InstructionSemantics class to construct values
                 *************************************************************************************************************/

                /** See NullSemantics::Policy::true_() */
                ValueType<1> true_() const {
                    return ValueType<1>(1);
                }

                /** See NullSemantics::Policy::false_() */
                ValueType<1> false_() const {
                    return ValueType<1>(0);
                }

                /** See NullSemantics::Policy::undefined_() */
                template <size_t Len>
                ValueType<Len> undefined_() const {
                    return ValueType<Len>();
                }

                /** See NullSemantics::Policy::number() */
                template <size_t Len>
                ValueType<Len> number(uint64_t n) const {
                    return ValueType<Len>(n);
                }



                /*************************************************************************************************************
                 * Functions invoked by the X86InstructionSemantics class for individual instructions
                 *************************************************************************************************************/

                /** See NullSemantics::Policy::filterCallTarget() */
                ValueType<32> filterCallTarget(const ValueType<32> &a) const {
                    return a;
                }

                /** See NullSemantics::Policy::filterReturnTarget() */
                ValueType<32> filterReturnTarget(const ValueType<32> &a) const {
                    return a;
                }

                /** See NullSemantics::Policy::filterIndirectJumpTarget() */
                ValueType<32> filterIndirectJumpTarget(const ValueType<32> &a) const {
                    return a;
                }

                /** See NullSemantics::Policy::hlt() */
                void hlt() {} // FIXME

                /** See NullSemantics::Policy::cpuid() */
                void cpuid() {} // FIXME

                /** See NullSemantics::Policy::rdtsc() */
                ValueType<64> rdtsc() {
                    return 0;
                }

                /** See NullSemantics::Policy::interrupt() */
                void interrupt(uint8_t num) {
                    cur_state = State<ValueType>(); /*reset entire machine state*/
                }

                /** See NullSemantics::Policy::sysenter() */
                void sysenter() {
                    cur_state = State<ValueType>(); /*reset entire machine state*/
                }



                /*************************************************************************************************************
                 * Functions invoked by the X86InstructionSemantics class for data access operations
                 *************************************************************************************************************/

#include "ReadWriteRegisterFragment.h"

                /** See NullSemantics::Policy::readMemory() */
                template <size_t Len> ValueType<Len>
                readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<1> &cond) const {
                    return mem_read<Len>(cur_state, addr);
                }

                /** See NullSemantics::Policy::writeMemory() */
                template <size_t Len> void
                writeMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<Len> &data,
                            const ValueType<1> &cond) {
                    mem_write<Len>(cur_state, addr, data);
                }



                /*************************************************************************************************************
                 * Functions invoked by the X86InstructionSemantics class for arithmetic operations
                 *************************************************************************************************************/

                /** See NullSemantics::Policy::add() */
                template <size_t Len>
                ValueType<Len> add(const ValueType<Len> &a, const ValueType<Len> &b) const {
                    if (a.name==b.name && (!a.name || a.negate!=b.negate)) {
                        /* [V1+x] + [-V1+y] = [x+y]  or
                         * [x] + [y] = [x+y] */
                        return ValueType<Len>(a.offset + b.offset);
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

                /** See NullSemantics::Policy::addWithCarries() */
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

                /** See NullSemantics::Policy::and_() */
                template <size_t Len>
                ValueType<Len> and_(const ValueType<Len> &a, const ValueType<Len> &b) const {
                    if ((!a.name && 0==a.offset) || (!b.name && 0==b.offset)) return ValueType<Len>(0);
                    if (a.name || b.name) return ValueType<Len>();
                    return ValueType<Len>(a.offset & b.offset);
                }

                /** See NullSemantics::Policy::equalToZero() */
                template <size_t Len>
                ValueType<1> equalToZero(const ValueType<Len> &a) const {
                    if (a.name) return undefined_<1>();
                    return a.offset ? false_() : true_();
                }

                /** See NullSemantics::Policy::invert() */
                template <size_t Len>
                ValueType<Len> invert(const ValueType<Len> &a) const {
                    if (a.name) return ValueType<Len>(a.name, ~a.offset, !a.negate);
                    return ValueType<Len>(~a.offset);
                }

                /** See NullSemantics::Policy::concat() */
                template<size_t Len1, size_t Len2>
                ValueType<Len1+Len2> concat(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (a.name || b.name) return ValueType<Len1+Len2>();
                    return ValueType<Len1+Len2>(a.offset | (b.offset << Len1));
                }

                /** See NullSemantics::Policy::ite() */
                template <size_t Len>
                ValueType<Len> ite(const ValueType<1> &sel, const ValueType<Len> &ifTrue, const ValueType<Len> &ifFalse) const {
                    if (ifTrue==ifFalse) return ifTrue;
                    if (sel.name) return ValueType<Len>();
                    return sel.offset ? ifTrue : ifFalse;
                }

                /** See NullSemantics::Policy::leastSignificantSetBit() */
                template <size_t Len>
                ValueType<Len> leastSignificantSetBit(const ValueType<Len> &a) const {
                    if (a.name) return ValueType<Len>();
                    for (size_t i=0; i<Len; ++i) {
                        if (a.offset & ((uint64_t)1 << i))
                            return ValueType<Len>(i);
                    }
                    return ValueType<Len>(0);
                }

                /** See NullSemantics::Policy::mostSignificantSetBit() */
                template <size_t Len>
                ValueType<Len> mostSignificantSetBit(const ValueType<Len> &a) const {
                    if (a.name) return ValueType<Len>();
                    for (size_t i=Len; i>0; --i) {
                        if (a.offset & ((uint64_t)1 << (i-1)))
                            return ValueType<Len>(i-1);
                    }
                    return ValueType<Len>(0);
                }

                /** See NullSemantics::Policy::negate() */
                template <size_t Len>
                ValueType<Len> negate(const ValueType<Len> &a) const {
                    if (a.name) return ValueType<Len>(a.name, -a.offset, !a.negate);
                    return ValueType<Len>(-a.offset);
                }

                /** See NullSemantics::Policy::or_() */
                template <size_t Len>
                ValueType<Len> or_(const ValueType<Len> &a, const ValueType<Len> &b) const {
                    if (a==b) return a;
                    if (!a.name && !b.name) return ValueType<Len>(a.offset | b.offset);
                    if (!a.name && a.offset==IntegerOps::GenMask<uint64_t, Len>::value) return a;
                    if (!b.name && b.offset==IntegerOps::GenMask<uint64_t, Len>::value) return b;
                    return ValueType<Len>();
                }

                /** See NullSemantics::Policy::rotateLeft() */
                template <size_t Len, size_t SALen>
                ValueType<Len> rotateLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
                    if (!a.name && !sa.name) return ValueType<Len>(IntegerOps::rotateLeft<Len>(a.offset, sa.offset));
                    if (!sa.name && 0==sa.offset % Len) return a;
                    return ValueType<Len>();
                }

                /** See NullSemantics::Policy::rotateRight() */
                template <size_t Len, size_t SALen>
                ValueType<Len> rotateRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
                    if (!a.name && !sa.name) return ValueType<Len>(IntegerOps::rotateRight<Len>(a.offset, sa.offset));
                    if (!sa.name && 0==sa.offset % Len) return a;
                    return ValueType<Len>();
                }

                /** See NullSemantics::Policy::shiftLeft() */
                template <size_t Len, size_t SALen>
                ValueType<Len> shiftLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
                    if (!a.name && !sa.name) return ValueType<Len>(IntegerOps::shiftLeft<Len>(a.offset, sa.offset));
                    if (!sa.name) {
                        if (0==sa.offset) return a;
                        if (sa.offset>=Len) return ValueType<Len>(0);
                    }
                    return ValueType<Len>();
                }

                /** See NullSemantics::Policy::shiftRight() */
                template <size_t Len, size_t SALen>
                ValueType<Len> shiftRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
                    if (!a.name && !sa.name) return ValueType<Len>(IntegerOps::shiftRightLogical<Len>(a.offset, sa.offset));
                    if (!sa.name) {
                        if (0==sa.offset) return a;
                        if (sa.offset>=Len) return ValueType<Len>(0);
                    }
                    return ValueType<Len>();
                }

                /** See NullSemantics::Policy::shiftRightArithmetic() */
                template <size_t Len, size_t SALen>
                ValueType<Len> shiftRightArithmetic(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
                    if (!a.name && !sa.name) return ValueType<Len>(IntegerOps::shiftRightArithmetic<Len>(a.offset, sa.offset));
                    if (!sa.name && 0==sa.offset) return a;
                    return ValueType<Len>();
                }

                /** See NullSemantics::Policy::signedDivide() */
                template <size_t Len1, size_t Len2>
                ValueType<Len1> signedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (!b.name) {
                        if (0==b.offset) throw Exception("division by zero");
                        if (!a.name)
                            return ValueType<Len1>(IntegerOps::signExtend<Len1, 64>(a.offset) /
                                                   IntegerOps::signExtend<Len2, 64>(b.offset));
                        if (1==b.offset) return a;
                        if (b.offset==IntegerOps::GenMask<uint64_t,Len2>::value) return negate(a);
                        /*FIXME: also possible to return zero if B is large enough. [RPM 2010-05-18]*/
                    }
                    return ValueType<Len1>();
                }

                /** See NullSemantics::Policy::signedModulo() */
                template <size_t Len1, size_t Len2>
                ValueType<Len2> signedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (a.name || b.name) return ValueType<Len2>();
                    if (0==b.offset) throw Exception("division by zero");
                    return ValueType<Len2>(IntegerOps::signExtend<Len1, 64>(a.offset) %
                                           IntegerOps::signExtend<Len2, 64>(b.offset));
                    /* FIXME: More folding possibilities... if 'b' is a power of two then we can return 'a' with the bitsize of
                     * 'b'. */
                }

                /** See NullSemantics::Policy::signedMultiply() */
                template <size_t Len1, size_t Len2>
                ValueType<Len1+Len2> signedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (!a.name && !b.name)
                        return ValueType<Len1+Len2>(IntegerOps::signExtend<Len1, 64>(a.offset) *
                                                    IntegerOps::signExtend<Len2, 64>(b.offset));
                    if (!b.name) {
                        if (0==b.offset) return ValueType<Len1+Len2>(0);
                        if (1==b.offset) return signExtend<Len1, Len1+Len2>(a);
                        if (b.offset==IntegerOps::GenMask<uint64_t,Len2>::value) return signExtend<Len1, Len1+Len2>(negate(a));
                    }
                    if (!a.name) {
                        if (0==a.offset) return ValueType<Len1+Len2>(0);
                        if (1==a.offset) return signExtend<Len2, Len1+Len2>(b);
                        if (a.offset==IntegerOps::GenMask<uint64_t,Len1>::value) return signExtend<Len2, Len1+Len2>(negate(b));
                    }
                    return ValueType<Len1+Len2>();
                }

                /** See NullSemantics::Policy::unsignedDivide() */
                template <size_t Len1, size_t Len2>
                ValueType<Len1> unsignedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (!b.name) {
                        if (0==b.offset) throw Exception("division by zero");
                        if (!a.name) return ValueType<Len1>(a.offset / b.offset);
                        if (1==b.offset) return a;
                        /*FIXME: also possible to return zero if B is large enough. [RPM 2010-05-18]*/
                    }
                    return ValueType<Len1>();
                }

                /** See NullSemantics::Policy::unsignedModulo() */
                template <size_t Len1, size_t Len2>
                ValueType<Len2> unsignedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (!b.name) {
                        if (0==b.offset) throw Exception("division by zero");
                        if (!a.name) return ValueType<Len2>(a.offset % b.offset);
                        /* FIXME: More folding possibilities... if 'b' is a power of two then we can return 'a' with the
                         * bitsize of 'b'. */
                    }
                    if (unsignedExtend<Len1,64>(a)==unsignedExtend<Len2,64>(b)) return b;
                    return ValueType<Len2>();
                }

                /** See NullSemantics::Policy::unsignedMultiply() */
                template <size_t Len1, size_t Len2>
                ValueType<Len1+Len2> unsignedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
                    if (!a.name && !b.name)
                        return ValueType<Len1+Len2>(a.offset * b.offset);
                    if (!b.name) {
                        if (0==b.offset) return ValueType<Len1+Len2>(0);
                        if (1==b.offset) return unsignedExtend<Len1, Len1+Len2>(a);
                    }
                    if (!a.name) {
                        if (0==a.offset) return ValueType<Len1+Len2>(0);
                        if (1==a.offset) return unsignedExtend<Len2, Len1+Len2>(b);
                    }
                    return ValueType<Len1+Len2>();
                }

                /** See NullSemantics::Policy::xor_() */
                template <size_t Len>
                ValueType<Len> xor_(const ValueType<Len> &a, const ValueType<Len> &b) const {
                    if (!a.name && !b.name)
                        return ValueType<Len>(a.offset ^ b.offset);
                    if (a==b)
                        return ValueType<Len>(0);
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

            /*************************************************************************************************************
             *                              ValueType template functions
             *************************************************************************************************************/


            template<size_t Len> ValueType<Len>
            ValueType<Len>::rename(RenameMap *rmap) const
            {
                ValueType<Len> retval = *this;
                if (rmap && name>0) {
                    RenameMap::iterator found = rmap->find(name);
                    if (found==rmap->end()) {
                        retval.name = rmap->size()+1;
                        rmap->insert(std::make_pair(name, retval.name));
                    } else {
                        retval.name = found->second;
                    }
                }
                return retval;
            }



            /*************************************************************************************************************
             *                              MemoryCell template functions
             *************************************************************************************************************/

            template<template <size_t> class ValueType>
            bool
            MemoryCell<ValueType>::may_alias(const MemoryCell &other) const {
                const ValueType<32> &addr1 = this->address;
                const ValueType<32> &addr2 = other.address;
                if (addr1.name != addr2.name) return true;

                /* Same unknown values but inverses (any offset). */
                if (addr1.name && addr1.negate!=addr2.negate) return true;

                /* If they have the same base values (or are both constant) then check the offsets. The 32-bit casts are
                 * purportedly necessary to wrap propertly, but I'm not sure this will work for addresses (LatticeElements)
                 * that have a length other than 32 bits. [FIXME RPM 2009-02-03]. */
                uint32_t offsetDiff = (uint32_t)(addr1.offset - addr2.offset);
                if (offsetDiff < this->nbytes || offsetDiff > (uint32_t)(-other.nbytes))
                    return true;
                return false;
            }

            template<template<size_t> class ValueType>
            bool
            MemoryCell<ValueType>::must_alias(const MemoryCell<ValueType> &other) const {
                return this->address == other.address;
            }

            /*****************************************************************************************************************
             *                              State template functions
             *****************************************************************************************************************/

            template<template<size_t> class ValueType>
            void
            State<ValueType>::print_diff_registers(std::ostream &o, const State &other, RenameMap *rmap/*=NULL*/) const
            {
#ifndef CXX_IS_ROSE_ANALYSIS
                // DQ (5/22/2010): This code does not compile using ROSE, it needs to be investigated to be reduced to an bug
                // report.

                std::string prefix = "    ";

                for (size_t i=0; i<this->registers.n_gprs; ++i) {
                    if (this->registers.gpr[i]!=other.registers.gpr[i]) {
                        o <<prefix <<gprToString((X86GeneralPurposeRegister)i) <<": "
                          <<this->registers.gpr[i].rename(rmap) <<" -> " <<other.registers.gpr[i].rename(rmap) <<"\n";
                    }
                }
                for (size_t i=0; i<this->registers.n_segregs; ++i) {
                    if (this->registers.segreg[i]!=other.registers.segreg[i]) {
                        o <<prefix <<segregToString((X86SegmentRegister)i) <<": "
                          <<this->registers.segreg[i].rename(rmap) <<" -> " <<other.registers.segreg[i].rename(rmap) <<"\n";
                    }
                }
                for (size_t i=0; i<this->registers.n_flags; ++i) {
                    if (this->registers.flag[i]!=other.registers.flag[i]) {
                        o <<prefix <<flagToString((X86Flag)i) <<": "
                          <<this->registers.flag[i].rename(rmap) <<" -> " <<other.registers.flag[i].rename(rmap) <<"\n";
                    }
                }
                if (this->registers.ip!=other.registers.ip) {
                    o <<prefix <<"ip: " <<this->registers.ip.rename(rmap) <<" -> " <<other.registers.ip.rename(rmap) <<"\n";
                }
#endif
            }

            template<template<size_t> class ValueType>
            bool
            State<ValueType>::equal_registers(const State &other) const
            {
#ifndef CXX_IS_ROSE_ANALYSIS
                for (size_t i=0; i<this->registers.n_gprs; ++i)
                    if (this->registers.gpr[i]!=other.registers.gpr[i]) return false;
                for (size_t i=0; i<this->registers.n_segregs; ++i)
                    if (this->registers.segreg[i]!=other.registers.segreg[i]) return false;
                for (size_t i=0; i<this->registers.n_flags; ++i)
                    if (this->registers.flag[i]!=other.registers.flag[i]) return false;
                if (this->registers.ip!=other.registers.ip) return false;
#endif
                return true;
            }

            template<template<size_t> class ValueType>
            void
            State<ValueType>::discard_popped_memory()
            {
                Memory new_mem;
                const ValueType<32> &sp = this->registers.gpr[x86_gpr_sp];
                for (typename Memory::const_iterator mi=this->memory.begin(); mi!=this->memory.end(); ++mi) {
                    const ValueType<32> &addr = mi->get_address();
                    if (addr.name!=sp.name || addr.negate!=sp.negate || (int32_t)addr.offset>=(int32_t)sp.offset)
                        new_mem.push_back(*mi);
                }
                this->memory = new_mem;
            }

            /*****************************************************************************************************************
             *                              Policy template functions
             *****************************************************************************************************************/

            /* Returns memory that needs to be compared by equal_states() */
            template<
                template <template <size_t> class ValueType> class State,
                template<size_t> class ValueType>
            typename State<ValueType>::Memory
            Policy<State, ValueType>::memory_for_equality(const State<ValueType> &state) const
            {
                State<ValueType> tmp_state = state;
                typename State<ValueType>::Memory retval;
#ifndef CXX_IS_ROSE_ANALYSIS
                for (typename State<ValueType>::Memory::const_iterator mi=state.memory.begin(); mi!=state.memory.end(); ++mi) {
                    if (mi->is_written() && mi->get_data()!=mem_read<32>(orig_state, mi->get_address()))
                        retval.push_back(*mi);
                }
#endif
                return retval;
            }

            template<
                template <template <size_t> class ValueType> class State,
                template<size_t> class ValueType>
            bool
            Policy<State, ValueType>::equal_states(const State<ValueType> &s1, const State<ValueType> &s2) const
            {
#ifndef CXX_IS_ROSE_ANALYSIS
                if (!s1.equal_registers(s2))
                    return false;
                typename State<ValueType>::Memory m1 = memory_for_equality(s1);
                typename State<ValueType>::Memory m2 = memory_for_equality(s2);
                std::vector<typename State<ValueType>::Memory::value_type> v1(m1.begin(), m1.end());
                std::vector<typename State<ValueType>::Memory::value_type> v2(m1.begin(), m1.end());
                if (v1.size()!=v2.size())
                    return false;
                std::sort(v1.begin(), v1.end());
                std::sort(v2.begin(), v2.end());
                for (size_t i=0; i<v1.size(); ++i) {
                    if (v1[i].get_nbytes() != v2[i].get_nbytes() ||
                        v1[i].get_address()!= v2[i].get_address() ||
                        v1[i].get_data()   != v2[i].get_data())
                        return false;
                }
#endif
                return true;
            }

            template<
                template <template <size_t> class ValueType> class State,
                template<size_t> class ValueType>
            void
            Policy<State, ValueType>::print(std::ostream &o, RenameMap *rmap/*=NULL*/) const
            {
                cur_state.print(o, "", rmap);
            }

            template<
                template <template <size_t> class ValueType> class State,
                template<size_t> class ValueType>
            void
            Policy<State, ValueType>::print_diff(std::ostream &o, const State<ValueType> &s1,
                                                 const State<ValueType> &s2, RenameMap *rmap/*=NULL*/) const
            {
#ifndef CXX_IS_ROSE_ANALYSIS
                s1.print_diff_registers(o, s2, rmap);

                /* Get all addresses that have been written and are not currently clobbered. */
                std::set<ValueType<32> > addresses;
                for (typename State<ValueType>::Memory::const_iterator mi=s1.memory.begin(); mi!=s1.memory.end(); ++mi) {
                    if (!mi->is_clobbered() && mi->is_written())
                        addresses.insert(mi->get_address());
                }
                for (typename State<ValueType>::Memory::const_iterator mi=s2.memory.begin(); mi!=s2.memory.end(); ++mi) {
                    if (!mi->is_clobbered() && mi->is_written())
                        addresses.insert(mi->get_address());
                }

                State<ValueType> tmp_s1 = s1;
                State<ValueType> tmp_s2 = s2;
                size_t nmemdiff = 0;
                for (typename std::set<ValueType<32> >::const_iterator ai=addresses.begin(); ai!=addresses.end(); ++ai) {
                    ValueType<32> v1 = mem_read<32>(tmp_s1, *ai);
                    ValueType<32> v2 = mem_read<32>(tmp_s2, *ai);
                    if (v1 != v2) {
                        if (0==nmemdiff++) o <<"    memory:\n";
                        o <<"      " <<(*ai).rename(rmap) <<": " <<v1.rename(rmap) <<" -> " <<v2.rename(rmap) <<"\n";
                    }
                }
#endif
            }

            template<
                template <template <size_t> class ValueType> class State,
                template<size_t> class ValueType>
            bool
            Policy<State, ValueType>::on_stack(const ValueType<32> &value) const
            {
#ifndef CXX_IS_ROSE_ANALYSIS
                const ValueType<32> sp_inverted = invert(cur_state.registers.gpr[x86_gpr_sp]);
                for (typename State<ValueType>::Memory::const_iterator mi=cur_state.memory.begin();
                     mi!=cur_state.memory.end();
                     ++mi) {
                    if (mi->get_nbytes()!=4 || !(mi->get_data()==value)) continue;
                    const ValueType<32> &addr = mi->get_address();

                    /* Is addr >= sp? */
                    ValueType<32> carries = 0;
                    ValueType<32> diff = addWithCarries(addr, sp_inverted, true_(), carries/*out*/);
                    ValueType<1> sf = extract<31,32>(diff);
                    ValueType<1> of = xor_(extract<31,32>(carries), extract<30,31>(carries));
                    if (sf==of) return true;
                }
#endif
                return false;
            }

            template<
                template <template <size_t> class ValueType> class State,
                template<size_t> class ValueType>
            bool
            Policy<State, ValueType>::SHA1(unsigned char digest[20]) const
            {
#ifdef ROSE_HAVE_GCRYPT_H
                /* libgcrypt requires gcry_check_version() to be called "before any other function in the library", but doesn't
                 * include an API function for determining if this has already been performed. It also doesn't indicate what
                 * happens when it's called more than once, or how expensive the call is.  Therefore, instead of calling it
                 * every time through this function, we'll just call it the first time. */
                static bool initialized = false;
                if (!initialized) {
                    gcry_check_version(NULL);
                    initialized = true;
                }

                std::stringstream s;
                RenameMap rmap;
                print_diff(s, &rmap);
                ROSE_ASSERT(gcry_md_get_algo_dlen(GCRY_MD_SHA1)==20);
                gcry_md_hash_buffer(GCRY_MD_SHA1, digest, s.str().c_str(), s.str().size());
                return true;
#else
                memset(digest, 0, 20);
                return false;
#endif
            }

            template<
                template <template <size_t> class ValueType> class State,
                template<size_t> class ValueType>
            std::string
            Policy<State, ValueType>::SHA1() const
            {
                std::string digest_str;
                unsigned char digest[20];
                if (SHA1(digest)) {
                    for (size_t i=sizeof digest; i>0; --i) {
                        digest_str += "0123456789abcdef"[(digest[i-1] >> 4) & 0xf];
                        digest_str += "0123456789abcdef"[digest[i-1] & 0xf];
                    }
                }
                return digest_str;
            }
    
        } /*namespace*/
    } /*namespace*/
} /*namespace*/


#endif
