#ifndef Rose_IntervalSemantics_H
#define Rose_IntervalSemantics_H
#include <stdint.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif
#include <inttypes.h>

#include "x86InstructionSemantics.h"
#include "BaseSemantics.h"
#include "integerOps.h"
#include "rangemap.h"

namespace BinaryAnalysis {              // documented elsewhere
    namespace InstructionSemantics {    // documented elsewhere

        /** Range of possible values.  We only define this so the range-printing methods are a bit more intuitive for semantic
         *  analysis.  Otherwise we'll end up using the Extent::print() method which is more suitable for things like section
         *  addresses. */
        class Interval: public Range<uint64_t> {
        public:
            Interval(): Range<uint64_t>() {}
            explicit Interval(uint64_t first): Range<uint64_t>(first) {}
            Interval(uint64_t first, uint64_t size): Range<uint64_t>(first, size) {}
            Interval(const Range<uint64_t> &other): Range<uint64_t>(other) {} /*implicit*/

            static Interval inin(uint64_t first, uint64_t last) {
                assert(first<=last);
                Interval retval;
                retval.first(first);
                retval.last(last);
                return retval;
            }

            static std::string to_string(uint64_t n) {
                const char *fmt = NULL;
                if (n<=9) {
                    fmt = "%"PRIu64;
                } else if (n<=0xff) {
                    fmt = "0x%02"PRIx64;
                } else if (n<=0xffff) {
                    fmt = "0x%04"PRIx64;
                } else {
                    fmt = "0x%08"PRIx64;
                }
                char buf[64];
                snprintf(buf, sizeof buf, fmt, n);
                return buf;
            }

            void print(std::ostream &o) const {
                if (empty()) {
                    o <<"<empty>";
                } else if (first()==last()) {
                    o <<to_string(first());
                } else if (0==first() && 0xff==last()) {
                    o <<"byte";
                } else if (0==first() && 0xffff==last()) {
                    o <<"word";
                } else if (0==first() && 0xffffffffull==last()) {
                    o <<"doubleword";
                } else if (0==first() && 0xffffffffffffffffull==last()) {
                    o <<"quadword";
                } else {
                    o <<to_string(first()) <<".." <<to_string(last());
                }
            }

            friend std::ostream& operator<<(std::ostream &o, const Interval &x) {
                x.print(o);
                return o;
            }
        };

        /** Set of intervals. */
        typedef RangeMap<Interval> Intervals;

        /** An interval analysis semantic domain.
         *
         *  Each value in this domain is a set of intervals in the 32-bit unsigned integer space.  The intervals are
         *  represented by ROSE's Range type and the set of ranges is represented by ROSE's RangeMap class.
         *
         *  As with all instruction semantic domains, the InstructionSemanitcs name space has three primary member classes:
         *
         *  <ul>
         *    <li>Policy: the policy class used to instantiate X86InstructionSemantic instances.</li>
         *    <li>State: represents the state of the virtual machine, its registers and memory.</li>
         *    <li>ValueType: the values stored in registers and memory and used for memory addresses.</li>
         *  </ul>
         */
        namespace IntervalSemantics {

            /******************************************************************************************************************
             *                          ValueType
             ******************************************************************************************************************/

            template<size_t nBits>
            class ValueType {
            protected:
                Intervals p_intervals;

            public:
                /** Construct value with no interval constraints. */
                ValueType() {
                    p_intervals.insert(Interval::inin(0, IntegerOps::genMask<uint64_t>(nBits)));
                }

                /** Copy constructor. */
                ValueType(const ValueType &other) {
                    p_intervals = other.p_intervals;
                }

                /** Construct a ValueType with a known value. */
                explicit ValueType(uint64_t n) {
                    n &= IntegerOps::GenMask<uint64_t, nBits>::value;
                    p_intervals.insert(Interval(n));
                }

                /** Construct a ValueType that's constrained to be between two unsigned values, inclusive. */
                explicit ValueType(uint64_t v1, uint64_t v2) {
                    v1 &= IntegerOps::GenMask<uint64_t, nBits>::value;
                    v2 &= IntegerOps::GenMask<uint64_t, nBits>::value;
                    assert(v1<=v2);
                    p_intervals.insert(Interval::inin(v1, v2));
                }

                /** Construct a ValueType from a rangemap. Note that this does not truncate the rangemap to contain only values
                 *  that would be possible for the ValueType size--see unsignedExtend() for that. */
                explicit ValueType(const Intervals &intervals) {
                    assert(!intervals.empty());
                    assert((intervals.max() <= IntegerOps::GenMask<uint64_t, nBits>::value));
                    p_intervals = intervals;
                }

                /** Print the value. */
                void print(std::ostream &o, const BaseSemantics::SEMANTIC_NO_PRINT_HELPER *pr=NULL) const {
                    o <<p_intervals <<"[" <<nBits <<"]";
                }
                friend std::ostream& operator<<(std::ostream &o, const ValueType &e) {
                    e.print(o);
                    return o;
                }

                /** Returns true if the value is a known constant. */
                bool is_known() const {
                    return 1==p_intervals.size();
                }

                /** Returns the value of a known constant. Assumes this value is a known constant. */
                uint64_t known_value() const {
                    assert(1==p_intervals.size());
                    return p_intervals.min();
                }

                /** Returns the rangemap stored in this value. */
                const Intervals& get_intervals() const {
                    return p_intervals;
                }

                /** Changes the rangemap stored in the value. */
                void set_intervals(const Intervals &intervals) {
                    p_intervals = intervals;
                }

                /** Returns all possible bits that could be set. */
                uint64_t possible_bits() const {
                    uint64_t bits=0;
                    for (Intervals::const_iterator ii=p_intervals.begin(); ii!=p_intervals.end(); ++ii) {
                        uint64_t lo=ii->first.first(), hi=ii->first.last();
                        bits |= lo | hi;
                        for (uint64_t bitno=0; bitno<32; ++bitno) {
                            uint64_t bit = IntegerOps::shl1<uint64_t>(bitno);
                            if (0 == (bits & bit)) {
                                uint64_t base = lo & ~IntegerOps::genMask<uint64_t>(bitno);
                                if (ii->first.contains(Interval(base+bit)))
                                    bits |= bit; 
                            }
                        }
                    }
                    return bits;
                }

                /** Generate ranges from bits. Given the set of bits that could be set, generate a range.  We have to be
                 *  careful here because we could end up generating very large rangemaps: a rangemap where the high 31 bits
                 *  could be set but the zero bit must be cleared would create a rangemap with 2^31 singleton entries. */
                static ValueType from_bits(uint64_t possible_bits) {
                    static const uint64_t max_complexity = 50; // arbitrary
                    Intervals retval;
                    possible_bits &= IntegerOps::genMask<uint64_t>(nBits);

                    size_t nset = 0, lobit=nBits, hibit=0;
                    for (size_t i=0; i<nBits; ++i) {
                        if (0 != (possible_bits & IntegerOps::shl1<uint64_t>(i))) {
                            ++nset;
                            lobit = std::min(lobit, i);
                            hibit = std::max(hibit, i);
                        }
                    }
                    
                    if (possible_bits == IntegerOps::genMask<uint64_t>(nset)) {
                        // The easy case: all possible bits are grouped together at the low end.
                        retval.insert(Interval::inin(0, possible_bits));
                    } else {
                        // Low-order bit of result must be clear, so the rangemap will have 2^nset ranges
                        uint64_t nranges = IntegerOps::shl1<uint64_t>(nset); // 2^nset
                        if (nranges>max_complexity) {
                            uint64_t hi = IntegerOps::genMask<uint64_t>(hibit+1) ^ IntegerOps::genMask<uint64_t>(lobit);
                            retval.insert(Interval::inin(0, hi));
                        } else {
                            for (uint64_t i=0; i<nranges; ++i) {
                                uint64_t lo=0, tmp=i;
                                for (uint64_t j=0; j<nBits; ++j) {
                                    uint64_t bit = IntegerOps::shl1<uint64_t>(j);
                                    if (0 != (possible_bits & bit)) {
                                        lo |= (tmp & 1) << j;
                                        tmp = tmp >> 1;
                                    }
                                }
                                retval.insert(Interval(lo));
                            }
                        }
                    }
                    return ValueType<nBits>(retval);
                }
            };


            /******************************************************************************************************************
             *                          MemoryCell
             ******************************************************************************************************************/

            /** Memory cell with interval address and data.  The ValueType template argument should be a subclass of
             *  IntervalSemantics::ValueType. */
            template<template<size_t> class ValueType=IntervalSemantics::ValueType>
            class MemoryCell: public BaseSemantics::MemoryCell<ValueType> {
            public:

                /** Returns true if this memory value could possibly overlap with the @p other memory value.  Addresses overlap
                 *  if their ranges overlap. */
                bool may_alias(const MemoryCell &other) const {
                    if (must_alias(other))
                        return true; /*this is faster*/
                    return this->get_address().get_intervals().contains(other.get_address().get_intervals());
                }

                /** Returns true if this memory address is the same as the @p other. Note that "same" is more strict than
                 *  "overlap". */
                bool must_alias(const MemoryCell &other) const {
                    return (this->get_address().is_known() && other.get_address().is_known() &&
                            this->get_address().known_value()==other.get_address().known_value());
                }
            };

            /******************************************************************************************************************
             *                          State
             ******************************************************************************************************************/

            /** Represents the entire state of the machine. */
            template <template <size_t> class ValueType=IntervalSemantics::ValueType>
            struct State: public BaseSemantics::StateX86<MemoryCell, ValueType> {};

            /******************************************************************************************************************
             *                          Policy
             ******************************************************************************************************************/

            /** A policy that is supplied to the semantic analysis constructor. See documentation for the SymbolicSemantics
             *  namespace.  The RISC-like operations are documented in the
             *  BinaryAnalysis::InstructionSemantics::NullSemantics::Policy class. */
            template <
                template <template <size_t> class ValueType> class State = IntervalSemantics::State,
                template <size_t> class ValueType = IntervalSemantics::ValueType
                >
            class Policy: public BaseSemantics::Policy {
            protected:
                typedef typename State<ValueType>::Memory Memory;

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
                size_t ninsns;                      /**< Total number of instructions processed. This is incremented by
                                                     * startInstruction(), which is the first thing called by
                                                     * X86InstructionSemantics::processInstruction(). */
            public:
                /** Constructs a new policy and makes initial and current states consistent. */
                Policy() {
                    init();
                    /* So that named values are identical in both; reinitialized by first call to startInstruction(). */
                    orig_state = cur_state;
                }

                /** Initialize undefined policy. Used by constructors so initialization is in one location. */
                void init() {
                    set_register_dictionary(RegisterDictionary::dictionary_pentium4());
                    cur_insn = NULL;
                    ninsns = 0;
                }

                /** Returns the current state. */
                const State<ValueType>& get_state() const { return cur_state; }
                State<ValueType>& get_state() { return cur_state; }

                /** Returns the original state.  The original state is initialized to be equal to the current state twice: once
                 *  by the constructor, and then again when the first instruction is processed. */
                const State<ValueType>& get_orig_state() const { return orig_state; }
                State<ValueType>& get_orig_state() { return orig_state; }

                /** Returns the current instruction pointer. */
                const ValueType<32>& get_ip() const { return cur_state.ip; }

                /** Returns the original instruction pointer. See also get_orig_state(). */
                const ValueType<32>& get_orig_ip() const { return orig_state.ip; }

                /** Returns true if the specified value exists in memory and is provably at or above the stack pointer.  The
                 *  stack pointer need not have a known value. */
                bool on_stack(const ValueType<32> &value) const;

                /** Return a newly sized value by either truncating the most significant bits or by adding more most significant
                 *  bits that are set to zeros. */
                template <size_t FromLen, size_t ToLen>
                ValueType<ToLen> unsignedExtend(const ValueType<FromLen> &a) const {
                    Intervals retval = a.get_intervals();
                    if (ToLen<FromLen) {
                        uint64_t lo = IntegerOps::SHL1<uint64_t, FromLen>::value;
                        uint64_t hi = IntegerOps::GenMask<uint64_t, ToLen>::value;
                        retval.erase(Interval::inin(lo, hi));
                    }
                    return ValueType<ToLen>(retval);
                }

                /**************************************************************************************************************
                 *                                      Instruction Semantics Dispatcher API
                 * See NullSemantics.h for documentation.
                 **************************************************************************************************************/
            public:

                /** See NullSemantics::Policy::startInstruction() */
                void startInstruction(SgAsmInstruction *insn) {
                    cur_state.registers.ip = ValueType<32>(insn->get_address());
                    if (0==ninsns++)
                        orig_state = cur_state;
                    cur_insn = insn;
                }

                /** See NullSemantics::Policy::finishInstruction() */
                void finishInstruction(SgAsmInstruction *insn) {
                    cur_insn = NULL;
                }

                /** See NullSemantics::Policy::true_() */
                ValueType<1> true_() const {
                    return ValueType<1>(1);
                }

                /** See NullSemantics::Policy::false_() */
                ValueType<1> false_() const {
                    return ValueType<1>(0);
                }

                /** See NullSemantics::Policy::undefined_() */
                template<size_t nBits>
                ValueType<nBits> undefined_() const {
                    return ValueType<nBits>();
                }

                template<size_t nBits>
                /** See NullSemantics::Policy::number() */
                ValueType<nBits> number(uint64_t n) const {
                    return ValueType<nBits>(n);
                }

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
                void hlt() {}

                /** See NullSemantics::Policy::cpuid() */
                void cpuid() {}

                /** See NullSemantics::Policy::rdtsc() */
                ValueType<64> rdtsc() {
                    return ValueType<64>();
                }

                /** See NullSemantics::Policy::interrupt() */
                void interrupt(uint8_t) {}

                /** See NullSemantics::Policy::sysenter() */
                void sysenter() {}

                /** See NullSemantics::Policy::add() */
                template<size_t nBits>
                ValueType<nBits> add(const ValueType<nBits> &a, const ValueType<nBits> &b) const {
                    const Intervals &aints=a.get_intervals(), &bints=b.get_intervals();
                    Intervals result;
                    for (Intervals::const_iterator ai=aints.begin(); ai!=aints.end(); ++ai) {
                        for (Intervals::const_iterator bi=bints.begin(); bi!=bints.end(); ++bi) {
                            uint64_t lo = (ai->first.first() + bi->first.first()) & IntegerOps::GenMask<uint64_t, nBits>::value;
                            uint64_t hi = (ai->first.last()  + bi->first.last())  & IntegerOps::GenMask<uint64_t, nBits>::value;
                            if (lo < ai->first.first() || lo < bi->first.first()) {
                                // lo and hi both overflow
                                result.insert(Interval::inin(lo, hi));
                            } else if (hi < ai->first.last() || hi < bi->first.last()) {
                                // hi overflows, but not lo
                                result.insert(Interval::inin(lo, IntegerOps::genMask<uint64_t>(nBits)));
                                result.insert(Interval::inin(0, hi));
                            } else {
                                // no overflow
                                result.insert(Interval::inin(lo, hi));
                            }
                        }
                    }
                    return ValueType<nBits>(result);
                }

                /** See NullSemantics::Policy::addWithCarries() */
                template<size_t nBits>
                ValueType<nBits> addWithCarries(const ValueType<nBits> &a, const ValueType<nBits> &b, const ValueType<1> &c,
                                                ValueType<nBits> &carry_out) const {
                    ValueType<nBits> wide_carry = unsignedExtend<1, nBits>(c);
                    ValueType<nBits> retval = add(add(a, b), wide_carry);
                    carry_out = ValueType<nBits>(); // FIXME: we can do better
                    return retval;
                }

                /** See NullSemantics::Policy::and_() */
                template<size_t nBits>
                ValueType<nBits> and_(const ValueType<nBits> &a, const ValueType<nBits> &b) const {
                    uint64_t abits = a.possible_bits(), bbits = b.possible_bits();
                    uint64_t rbits = abits & bbits; // bits that could be set in the result
                    return ValueType<nBits>::from_bits(rbits);
                }

                /** See NullSemantics::Policy::equalToZero() */
                template<size_t nBits>
                ValueType<1> equalToZero(const ValueType<nBits> &a) const {
                    if (a.is_known())
                        return 0==a.known_value() ? true_() : false_();
                    if (!a.get_intervals().contains(Interval(0)))
                        return false_();
                    return undefined_<1>();
                }

                /** See NullSemantics::Policy::invert() */
                template<size_t nBits>
                ValueType<nBits> invert(const ValueType<nBits> &a) const {
                    Intervals result;
                    uint64_t mask = IntegerOps::GenMask<uint64_t, nBits>::value;
                    for (Intervals::const_iterator ai=a.get_intervals().begin(); ai!=a.get_intervals().end(); ++ai) {
                        const Interval &iv = ai->first;
                        uint64_t lo = ~iv.last() & mask;
                        uint64_t hi = ~iv.first() & mask;
                        result.insert(Interval::inin(lo, hi));
                    }
                    return ValueType<nBits>(result);
                }

                /** See NullSemantics::Policy::extract() */
                template<size_t BeginBit, size_t EndBit, size_t nBitsA>
                ValueType<EndBit-BeginBit> extract(const ValueType<nBitsA> &a) const {
                    using namespace IntegerOps;
                    Intervals result;
                    uint64_t discard_mask = ~GenMask<uint64_t, EndBit>::value; // hi-order bits being discarded
                    uint64_t src_mask = GenMask<uint64_t, nBitsA>::value; // significant bits in the source operand
                    uint64_t dst_mask = GenMask<uint64_t, EndBit-BeginBit>::value; // sig. bits in the result
                    for (Intervals::const_iterator ai=a.get_intervals().begin(); ai!=a.get_intervals().end(); ++ai) {
                        const Interval &iv = ai->first;
                        uint64_t d1 = shiftRightLogical<nBitsA>(iv.first() & discard_mask, EndBit); // discarded part, lo
                        uint64_t d2 = shiftRightLogical<nBitsA>(iv.last()  & discard_mask, EndBit); // discarded part, hi
                        uint64_t k1 = shiftRightLogical<nBitsA>(iv.first() & src_mask, BeginBit) & dst_mask; // keep part, lo
                        uint64_t k2 = shiftRightLogical<nBitsA>(iv.last()  & src_mask, BeginBit) & dst_mask; // keep part, hi
                        if (d1==d2) {                   // no overflow in the kept bits
                            assert(k1<=k2);
                            result.insert(Interval::inin(k1, k2));
                        } else if (d1+1<d2 || k1<k2) {  // complete overflow
                            result.insert(Interval::inin(0, dst_mask));
                            break;
                        } else {                        // partial overflow
                            result.insert(Interval::inin(0, k2));
                            result.insert(Interval::inin(k1, dst_mask));
                        }
                    }
                    return ValueType<EndBit-BeginBit>(result);
                }

                /** See NullSemantics::Policy::concat(). Bits @p b high; bits @p a low. */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA+nBitsB> concat(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    Intervals result;
                    uint64_t mask_a = IntegerOps::GenMask<uint64_t, nBitsA>::value;
                    uint64_t mask_b = IntegerOps::GenMask<uint64_t, nBitsB>::value;
                    for (Intervals::const_iterator ai=a.get_intervals().begin(); ai!=a.get_intervals().end(); ++ai) {
                        for (Intervals::const_iterator bi=b.get_intervals().begin(); bi!=b.get_intervals().end(); ++bi) {
                            uint64_t lo = (IntegerOps::shiftLeft<nBitsA+nBitsB>(bi->first.first() & mask_b, nBitsA) |
                                           (ai->first.first() & mask_a));
                            uint64_t hi = (IntegerOps::shiftLeft<nBitsA+nBitsB>(bi->first.last() & mask_b, nBitsA) |
                                           (ai->first.last() & mask_a));
                            result.insert(Interval::inin(lo, hi));
                        }
                    }
                    return ValueType<nBitsA+nBitsB>(result);
                }

                /** See NullSemantics::Policy::ite() */
                template<size_t nBits>
                ValueType<nBits> ite(const ValueType<1> &cond, const ValueType<nBits> &a, const ValueType<nBits> &b) const {
                    if (cond.is_known())
                        return cond.known_value() ? a : b;
                    Intervals result = a.get_intervals();
                    result.insert_ranges(b.get_intervals());
                    return ValueType<nBits>(result);
                }

                /** See NullSemantics::Policy::leastSignificantSetBit() */
                template<size_t nBits>
                ValueType<nBits> leastSignificantSetBit(const ValueType<nBits> &a) const {
                    if (a.is_known()) {
                        uint64_t av = a.known_value();
                        if (av) {
                            for (size_t i=0; i<nBits; ++i) {
                                if (av & IntegerOps::shl1<uint64_t>(i))
                                    return ValueType<nBits>(i);
                            }
                        }
                        return ValueType<nBits>(0);
                    }

                    uint64_t abits = a.possible_bits();
                    assert(abits!=0); // or else the value would be known to be zero and handled above
                    uint64_t lo=nBits, hi=0;
                    for (size_t i=0; i<nBits; ++i) {
                        if (abits & IntegerOps::shl1<uint64_t>(i)) {
                            lo = std::min(lo, (uint64_t)i);
                            hi = std::max(hi, (uint64_t)i);
                        }
                    }
                    Intervals result;
                    result.insert(Interval(0));
                    result.insert(Interval::inin(lo, hi));
                    return ValueType<nBits>(result);
                }

                /** See NullSemantics::Policy::mostSignificantSetBit() */
                template<size_t nBits>
                ValueType<nBits> mostSignificantSetBit(const ValueType<nBits> &a) const {
                    if (a.is_known()) {
                        uint64_t av = a.known_value();
                        if (av) {
                            for (size_t i=nBits; i>0; --i) {
                                if (av && IntegerOps::shl1<uint64_t>((i-1)))
                                    return ValueType<nBits>(i-1);
                            }
                        }
                        return ValueType<nBits>(0);
                    }

                    uint64_t abits = a.possible_bits();
                    assert(abits!=0); // or else the value would be known to be zero and handled above
                    uint64_t lo=nBits, hi=0;
                    for (size_t i=nBits; i>0; --i) {
                        if (abits & IntegerOps::shl1<uint64_t>(i-1)) {
                            lo = std::min(lo, (uint64_t)i-1);
                            hi = std::max(hi, (uint64_t)i-1);
                        }
                    }
                    Intervals result;
                    result.insert(Interval(0));
                    result.insert(Interval::inin(lo, hi));
                    return ValueType<nBits>(result);
                }

                /** See NullSemantics::Policy::negate() */
                template<size_t nBits>
                ValueType<nBits> negate(const ValueType<nBits> &a) const {
                    Intervals result;
                    uint64_t mask = IntegerOps::GenMask<uint64_t, nBits>::value;
                    for (Intervals::const_iterator ai=a.get_intervals().begin(); ai!=a.get_intervals().end(); ++ai) {
                        const Interval &iv = ai->first;
                        uint64_t lo = -iv.last() & mask;
                        uint64_t hi = -iv.first() & mask;
                        if (0==hi) {
                            assert(0==iv.first());
                            result.insert(Interval(0));
                            if (0!=lo) {
                                assert(0!=iv.last());
                                result.insert(Interval::inin(lo, IntegerOps::GenMask<uint64_t, nBits>::value));
                            }
                        } else {
                            assert(lo<=hi);
                            result.insert(Interval::inin(lo, hi));
                        }
                    }
                    return ValueType<nBits>(result);
                }

                /** See NullSemantics::Policy::or_() */
                template<size_t nBits>
                ValueType<nBits> or_(const ValueType<nBits> &a, const ValueType<nBits> &b) const {
                    if (a.is_known() && b.is_known()) {
                        uint64_t result = a.known_value() | b.known_value();
                        return ValueType<nBits>(result);
                    }
                    uint64_t abits = a.possible_bits(), bbits = b.possible_bits();
                    uint64_t rbits = abits | bbits; // bits that could be set in the result
                    return ValueType<nBits>::from_bits(rbits);
                }

                /** See NullSemantics::Policy::rotateLeft() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> rotateLeft(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    if (a.is_known() && b.is_known()) {
                        uint64_t bn = b.known_value();
                        uint64_t result = ((a.known_value() << bn) & IntegerOps::GenMask<uint64_t, nBitsA>::value) |
                                          ((a.known_value() >> (nBitsA-bn)) & IntegerOps::genMask<uint64_t>(bn));
                        return ValueType<nBitsA>(result);
                    }
                    uint64_t abits = a.possible_bits();
                    uint64_t rbits = 0, done = IntegerOps::GenMask<uint64_t, nBitsA>::value;
                    for (uint64_t i=0; i<(uint64_t)nBitsA && rbits!=done; ++i) {
                        if (b.get_intervals().contains(Interval(i))) {
                            rbits |= ((abits << i) & IntegerOps::GenMask<uint64_t, nBitsA>::value) |
                                     ((abits >> (nBitsA-i)) & IntegerOps::genMask<uint64_t>(i));
                        }
                    }
                    return ValueType<nBitsA>::from_bits(rbits);
                }

                /** See NullSemantics::Policy::rotateRight() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> rotateRight(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    if (a.is_known() && b.is_known()) {
                        uint64_t bn = b.known_value();
                        uint64_t result = ((a.known_value() >> bn) & IntegerOps::genMask<uint64_t>(nBitsA-bn)) |
                                          ((a.known_value() << (nBitsA-bn)) & IntegerOps::GenMask<uint64_t, nBitsA>::value);
                        return ValueType<nBitsA>(result);
                    }
                    uint64_t abits = a.possible_bits();
                    uint64_t rbits = 0, done = IntegerOps::GenMask<uint64_t, nBitsA>::value;
                    for (uint64_t i=0; i<(uint64_t)nBitsA && rbits!=done; ++i) {
                        if (b.get_intervals().contains(Interval(i))) {
                            rbits |= ((abits >> i) & IntegerOps::genMask<uint64_t>(nBitsA-i)) |
                                     ((abits << (nBitsA-i)) & IntegerOps::GenMask<uint64_t, nBitsA>::value);
                        }
                    }
                    return ValueType<nBitsA>::from_bits(rbits);

                }

                /** See NullSemantics::Policy::shiftLeft() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> shiftLeft(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    if (a.is_known() && b.is_known()) {
                        uint64_t result = b.known_value()<nBitsA ? a.known_value() << b.known_value() : (uint64_t)0;
                        return ValueType<nBitsA>(result);
                    }
                    uint64_t abits = a.possible_bits();
                    uint64_t rbits = 0, done = IntegerOps::GenMask<uint64_t, nBitsA>::value;
                    for (uint64_t i=0; i<(uint64_t)nBitsA && rbits!=done; ++i) {
                        if (b.get_intervals().contains(Interval(i)))
                            rbits |= (abits << i) & IntegerOps::GenMask<uint64_t, nBitsA>::value;
                    }
                    return ValueType<nBitsA>::from_bits(rbits);
                }

                /** See NullSemantics::Policy::shiftRight() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> shiftRight(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    if (b.is_known()) {
                        uint64_t bn = b.known_value();
                        if (bn >= nBitsA)
                            return ValueType<nBitsA>(0);
                        if (a.is_known())
                            return ValueType<nBitsA>(a.known_value() >> bn);
                        Intervals result;
                        for (Intervals::const_iterator ai=a.get_intervals().begin(); ai!=a.get_intervals().end(); ++ai) {
                            uint64_t lo = ai->first.first() >> bn;
                            uint64_t hi = ai->first.last() >> bn;
                            result.insert(Interval::inin(lo, hi));
                        }
                        return ValueType<nBitsA>(result);
                    }
                    Intervals result;
                    for (Intervals::const_iterator ai=a.get_intervals().begin(); ai!=a.get_intervals().end(); ++ai) {
                        for (uint64_t i=0; i<(uint64_t)nBitsA; ++i) {
                            if (b.get_intervals().contains(Interval(i))) {
                                uint64_t lo = ai->first.first() >> i;
                                uint64_t hi = ai->first.first() >> i;
                                result.insert(Interval::inin(lo, hi));
                            }
                        }
                    }
                    return ValueType<nBitsA>(result);
                }

                /** See NullSemantics::Policy::shiftRightArithmetic() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> shiftRightArithmetic(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    if (a.is_known() && b.is_known()) {
                        uint64_t bn = b.known_value();
                        uint64_t result = a.known_value() >> bn;
                        if (IntegerOps::signBit<nBitsA>(a.is_known()))
                            result |= IntegerOps::GenMask<uint64_t, nBitsA>::value ^ IntegerOps::genMask<uint64_t>(nBitsA-bn);
                        return ValueType<nBitsA>(result);
                    }
                    uint64_t abits = a.possible_bits();
                    uint64_t rbits = 0, done = IntegerOps::GenMask<uint64_t, nBitsA>::value;
                    for (uint64_t i=0; i<(uint64_t)nBitsA && rbits!=done; ++i) {
                        if (b.get_intervals().contains(Interval(i))) {
                            rbits |= ((abits >> i) & IntegerOps::genMask<uint64_t>(nBitsA-i)) |
                                     (IntegerOps::signBit<nBitsA>(abits) ?
                                      done ^ IntegerOps::genMask<uint64_t>(nBitsA-i) : (uint64_t)0);
                        }
                    }
                    return ValueType<nBitsA>::from_bits(rbits);
                }

                /** See NullSemantics::Policy::signExtend() */
                template<size_t From, size_t To>
                ValueType<To> signExtend(const ValueType<From> &a) {
                    uint64_t old_signbit = IntegerOps::SHL1<uint64_t, From-1>::value;
                    uint64_t new_signbit = IntegerOps::SHL1<uint64_t, To-1>::value;
                    Intervals result;
                    for (Intervals::const_iterator ai=a.get_intervals().begin(); ai!=a.get_intervals().end(); ++ai) {
                        uint64_t lo = IntegerOps::signExtend<From, To>(ai->first.first());
                        uint64_t hi = IntegerOps::signExtend<From, To>(ai->first.last());
                        if (0==(lo & new_signbit) && 0!=(hi & new_signbit)) {
                            result.insert(Interval::inin(lo, IntegerOps::GenMask<uint64_t, From-1>::value));
                            result.insert(Interval::inin(old_signbit, hi));
                        } else {
                            result.insert(Interval::inin(lo, hi));
                        }
                    }
                    return ValueType<To>(result);
                }

                /** See NullSemantics::Policy::signedDivide() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> signedDivide(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    if (!IntegerOps::signBit<nBitsA>(a.possible_bits()) && !IntegerOps::signBit<nBitsB>(b.possible_bits()))
                        return unsignedDivide(a, b);
                    return ValueType<nBitsA>(); // FIXME, we can do better
                }

                /** See NullSemantics::Policy::signedModulo() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsB> signedModulo(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    if (!IntegerOps::signBit<nBitsA>(a.possible_bits()) && !IntegerOps::signBit<nBitsB>(b.possible_bits()))
                        return unsignedModulo(a, b);
                    return ValueType<nBitsB>(); // FIXME, we can do better
                }

                /** See NullSemantics::Policy::signedMultiply() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA+nBitsB> signedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    if (!IntegerOps::signBit<nBitsA>(a.possible_bits()) && !IntegerOps::signBit<nBitsB>(b.possible_bits()))
                        return unsignedMultiply(a, b);
                    return ValueType<nBitsA+nBitsB>(); // FIXME, we can do better
                }

                /** See NullSemantics::Policy::unsignedDivide() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> unsignedDivide(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    Intervals result;
                    for (Intervals::const_iterator ai=a.get_intervals().begin(); ai!=a.get_intervals().end(); ++ai) {
                        for (Intervals::const_iterator bi=b.get_intervals().begin(); bi!=b.get_intervals().end(); ++bi) {
                            uint64_t lo = ai->first.first() / std::max(bi->first.last(),  (uint64_t)1);
                            uint64_t hi = ai->first.last()  / std::max(bi->first.first(), (uint64_t)1);
                            assert((lo<=IntegerOps::GenMask<uint64_t, nBitsA>::value));
                            assert((hi<=IntegerOps::GenMask<uint64_t, nBitsA>::value));
                            assert(lo<=hi);
                            result.insert(Interval::inin(lo, hi));
                        }
                    }
                    return ValueType<nBitsA>(result);
                }

                /** See NullSemantics::Policy::unsignedModulo() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsB> unsignedModulo(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    if (b.is_known()) {
                        // If B is a power of two then mask away the high bits of A
                        uint64_t bn = b.known_value();
                        for (size_t i=0; i<nBitsB; ++i) {
                            uint64_t twopow = IntegerOps::shl1<uint64_t>(i);
                            if (bn==twopow) {
                                uint64_t limit = IntegerOps::GenMask<uint64_t, nBitsA>::value;
                                Intervals result = a.get_intervals();
                                result.erase(Interval::inin(twopow, limit));
                                return ValueType<nBitsB>(result);
                            }
                        }
                    }
                    return ValueType<nBitsB>(); // FIXME: can we do better?
                }

                /** See NullSemantics::Policy::unsignedMultiply() */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA+nBitsB> unsignedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
                    Intervals result;
                    for (Intervals::const_iterator ai=a.get_intervals().begin(); ai!=a.get_intervals().end(); ++ai) {
                        for (Intervals::const_iterator bi=b.get_intervals().begin(); bi!=b.get_intervals().end(); ++bi) {
                            uint64_t lo = ai->first.first() * bi->first.first();
                            uint64_t hi = ai->first.last()  * bi->first.last();
                            assert(lo<=hi);
                            result.insert(Interval::inin(lo, hi));
                        }
                    }
                    return ValueType<nBitsA+nBitsB>(result);
                }

                /** See NullSemantics::Policy::xor_() */
                template<size_t nBits>
                ValueType<nBits> xor_(const ValueType<nBits> &a, const ValueType<nBits> &b) const {
                    uint64_t abits = a.possible_bits(), bbits = b.possible_bits();
                    uint64_t rbits = abits | bbits; // yes, OR, not XOR
                    return ValueType<nBits>::from_bits(rbits);
                }

                /** See NullSemantics::Policy::readMemory() */
                template<size_t nBits>
                ValueType<nBits> readMemory(X86SegmentRegister, const ValueType<32> &addr, const ValueType<1> &cond) const {
                    return ValueType<nBits>(); // FIXME
                }

                /** See NullSemantics::Policy::writeMemory() */
                template<size_t nBits>
                void writeMemory(X86SegmentRegister, const ValueType<32> &addr, const ValueType<nBits> &data,
                                 const ValueType<1> &cond) {} // FIXME

#include "ReadWriteRegisterFragment.h"

                /**************************************************************************************************************
                 *                                      Additional methods, etc.
                 *
                 * These are not part of the semantic API but are useful nonetheless.
                 **************************************************************************************************************/
            public:

                /** Print the current state of this policy. */
                void print(std::ostream &o) const {
                    cur_state.template print<BaseSemantics::SEMANTIC_NO_PRINT_HELPER>(o);
                }
                friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
                    p.print(o);
                    return o;
                }
            };
        } /*namespace*/
    } /*namespace*/
} /*namespace*/

#endif
