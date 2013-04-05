#ifndef ROSE_Simulator_SignAnalysisExample_H
#define ROSE_Simulator_SignAnalysisExample_H

/** Example sign analysis.
 *
 *  This example demonstrates how to write a (very simple, naive) sign analysis and the structure of this file is based on the
 *  PartialSymbolicSemantics.h file.  This analysis makes certain assumptions to make a simple example:
 *
 *  <ul>
 *    <li>All values are signed integers (except 1-bit flags, which are unsigned).</li>
 *    <li>Some operators are defined over infinite integers rather than modulo 2^32.</li>
 *    <li>This is only an example, so we're rather sloppy in some places.</li>
 *    <li>Some parts of this analysis are not well tested.</li>
 *  </ul>
 *
 * This namespace has three major components: the ValueType template class (the domain), a State class (the model), and the
 * Policy class (model transition operators).  The Policy is plugged into the X86InstructionSemantics class and the analysis is
 * driven by processing one instruction at a time in the order they would be executed.
 *
 * This analysis handles one control flow path.  If you want to do sign analysis over multiple paths you'd need to write code
 * to (1) traverse the paths, (2) split the state a points where control flow splits, (3) join states when control flow
 * joins. We don't cover that advanced topic in this example analysis. */
namespace SignAnalysisExample {
    enum Signedness {
        ZERO            = 0x01,
        POSITIVE        = 0x02,
        NEGATIVE        = 0x04
    };

    /** Domain over which operators are defined. In this case, each value of our domain could be zero, positive, or negative.
     * We use three bits since a value could be more than one at a time.  For instance, a completely unknown value could be
     * zero, it could be positive, and it could be negative, so we set all three bits. */
    template <size_t nBits>
    class ValueType {
    public:
        unsigned sign;          /* The Signedness bits */

        ValueType()
            : sign(ZERO|POSITIVE|NEGATIVE) {
            if (1==nBits)
                sign &= ~NEGATIVE;
        }

        explicit ValueType(unsigned sign)
            : sign(sign) {
            if (1==nBits && (sign & NEGATIVE)) {
                this->sign &= ~NEGATIVE;
                this->sign |= POSITIVE;
            }
        }
        
        template<size_t Len>
        ValueType(const ValueType<Len> &other) {
            sign = other.sign;
            if (1==nBits && (sign & NEGATIVE)) {
                sign &= ~NEGATIVE;
                sign |= POSITIVE;
            }
        }

        void print(std::ostream &o) const {
            o <<"<";
            if (sign & POSITIVE) o <<"+";
            if (sign & NEGATIVE) o <<"-";
            if (sign & ZERO)     o <<"0";
            o <<">";
        }
    };

    template<size_t Len>
    std::ostream& operator<<(std::ostream &o, const ValueType<Len> &a)
    {
        a.print(o);
        return o;
    }

    /** The analysis model.  Instructions operate on a state, transitioning one state to the next state.  The state usually
     * consists of the registers and memory. We're assuming an i386. See also ROSE's <Registers.h> */
    class State {
    public:
        struct {
            static const size_t n_gprs = 8;         /**< Number of general purpose registers */
            static const size_t n_segregs = 6;      /**< Number of 16-bit segmentation descriptor registers */
            static const size_t n_flags = 32;       /**< We treat EFLAGS as individual 1-bit (unsigned) values */

            ValueType<32> ip;                       /**< Instruction pointer */
            ValueType<32> gpr[n_gprs];              /**< General purpose registers */
            ValueType<32> segreg[n_segregs];        /**< Segmentation registers */
            ValueType<1> flag[n_flags];             /**< Bits of EFLAGS */
        } registers;

        /** Memory cells are <address,value> pairs.  Addresses belong to the same domain as values, therefore we have only
         * three possible addresses: negative, zero, positive.  This makes it impossible to determine the next instruction
         * using only this analysis, so SignAnalysisExample needs to always be driven by some other kind of analysis. */
        ValueType<32> memory[3];                /* 0=>zero, 1=>positive, 2=>negative */

        /** Prints state.  Prints all registers and the three memory locations.  The values are printed as "0", "+", and/or "-"
         *  depending on their sign. */
        void print(std::ostream &o) const {
            for (size_t i=0; i<registers.n_gprs; i++)
                o <<gprToString((X86GeneralPurposeRegister)i) <<"=" <<registers.gpr[i] <<" ";
            for (size_t i=0; i<registers.n_segregs; i++)
                o <<segregToString((X86SegmentRegister)i) <<"=" <<registers.segreg[i] <<" ";
            o <<"ip=" <<registers.ip <<" ";
            for (size_t i=0; i<registers.n_flags; i++)
                o <<flagToString((X86Flag)i) <<"=" <<registers.flag[i] <<" ";
            o <<"mem[0]=" <<memory[0] <<" ";
            o <<"mem[+]=" <<memory[1] <<" ";
            o <<"mem[-]=" <<memory[2] << std::endl;
        }

        /** See print() */
        friend std::ostream& operator<<(std::ostream &o, const State &state) {
            state.print(o);
            return o;
        }
    };


    /** Defines how operators transition the state. The Policy defines the operators, whose operands and result are all
     *  ValueType of various sizes. Most of the methods here have signatures dictated by how the policy is used by the
     *  instruction semantics class, X86InstructionSemantics.  */
    class Policy {
    private:
        State cur_state;
        const RegisterDictionary *regdict;

    public:
        struct Exception {
            Exception(const std::string &mesg): mesg(mesg) {}
            friend std::ostream& operator<<(std::ostream &o, const Exception &e) {
                o <<"SignAnalysisExample exception: " <<e.mesg;
                return o;
            }
            std::string mesg;
        };
        
        Policy(): regdict(NULL) {}

        /**********************************************************************************************************************
         * Some extra functions that are normally defined but not absolutely necessary.
         **********************************************************************************************************************/
    public:
        void print(std::ostream &o) const { o <<cur_state; }
        friend std::ostream& operator<<(std::ostream &o, const Policy &p) {
            p.print(o);
            return o;
        }

        /**********************************************************************************************************************
         * The operator signatures are dictated by the instruction semantics layer (such as x86InstructionSemantics class)
         * since the policy is "plugged into" an instruction semantics class.  See PartialSymbolicSemantics.h for documentation
         * of each method.
         **********************************************************************************************************************/
    public:
        template<size_t FromLen, size_t ToLen>
        ValueType<ToLen> signExtend(const ValueType<FromLen> &a) const {
            return ValueType<ToLen>(a.sign);
        }
        
        template<size_t BeginAt, size_t EndAt, size_t Len>
        ValueType<EndAt-BeginAt> extract(const ValueType<Len> &a) const {
            if (EndAt==Len) return ValueType<EndAt-BeginAt>(a.sign);
            return ValueType<EndAt-BeginAt>();
        }

        template<size_t FromLen, size_t ToLen>
        ValueType<ToLen> unsignedExtend(const ValueType<FromLen> &a) const {
            if (ToLen==FromLen) return ValueType<ToLen>(a);
            if (ToLen>FromLen) return ValueType<ToLen>(POSITIVE);
            return ValueType<ToLen>();
        }

        void startInstruction(SgAsmInstruction *insn) {
            cur_state.registers.ip = ValueType<32>(insn->get_address());
        }

        void finishInstruction(SgAsmInstruction*) {}

        void startBlock(rose_addr_t) {}

        void finishBlock(rose_addr_t) {}

        ValueType<1> true_() const {
            return ValueType<1>(POSITIVE);
        }

        ValueType<1> false_() const {
            return ValueType<1>(ZERO);
        }

        template<size_t Len>
        ValueType<Len> undefined_() const {
            return ValueType<Len>();
        }

        template<size_t Len>
        ValueType<Len> number(uint64_t n) const {
            uint64_t mask = ((uint64_t)1 << Len) - 1;
            uint64_t sbit = (uint64_t)1 << (Len-1);
            if (0==(n & mask))
                return ValueType<Len>(ZERO);
            if (0!=(n & sbit))
                return ValueType<Len>(NEGATIVE);
            return ValueType<Len>(POSITIVE);
        }

        ValueType<32> filterCallTarget(const ValueType<32> &a) const {
            return a;
        }

        ValueType<32> filterReturnTarget(const ValueType<32> &a) const {
            return a;
        }

        ValueType<32> filterIndirectJumpTarget(const ValueType<32> &a) const {
            return a;
        }

        void hlt() {};

        void cpuid() {}

        ValueType<64> rdtsc() {
            return ValueType<64>(ZERO);
        }

        void interrupt(uint8_t n) {
            cur_state = State();        // we have no idea what might have changed
        }

        void sysenter() {
            cur_state = State();        // we have no idea what might have changed
        }

        template<size_t Len>
        ValueType<Len> add(const ValueType<Len> &a, const ValueType<Len> &b) const {
            if (a.sign==ZERO && b.sign==ZERO)
                return ValueType<Len>(ZERO);
            if (0==(a.sign & NEGATIVE) && 0==(b.sign & NEGATIVE))
                return ValueType<Len>(POSITIVE);
            if (0==(a.sign & POSITIVE) && 0==(b.sign & POSITIVE))
                return ValueType<Len>(NEGATIVE);
            return ValueType<Len>();
        }

        template<size_t Len>
        ValueType<Len> addWithCarries(const ValueType<Len> &a, const ValueType<Len> &b, const ValueType<1> &c,
                                      ValueType<Len> &carry_out) const {
            if (a.sign==ZERO && b.sign==ZERO) {
                if (c.sign==ZERO) {
                    carry_out = ValueType<Len>(ZERO);
                    return ValueType<Len>(ZERO);
                } else {
                    carry_out = ValueType<Len>(ZERO);
                    return ValueType<Len>(POSITIVE);
                }
            }
            
            carry_out = ValueType<Len>();
            if (0==(a.sign & NEGATIVE) && 0==(b.sign & NEGATIVE))
                return ValueType<Len>(POSITIVE);
            if (a.sign==NEGATIVE && b.sign==NEGATIVE)
                return ValueType<Len>(NEGATIVE); // regardless of c
            if (0==(a.sign & POSITIVE) && 0==(b.sign & POSITIVE)) {
                if (c.sign==ZERO)
                    return ValueType<Len>(NEGATIVE);
                return ValueType<Len>(ZERO|NEGATIVE);
            }
            return ValueType<Len>();
        }

        template<size_t Len>
        ValueType<Len> and_(const ValueType<Len> &a, const ValueType<Len> &b) const {
            if (a.sign==ZERO || b.sign==ZERO)
                return ValueType<Len>(ZERO);
            if ((a.sign & NEGATIVE) && (b.sign & NEGATIVE))
                return ValueType<Len>(NEGATIVE);
            if (0==(a.sign & NEGATIVE) && 0==(b.sign & NEGATIVE))
                return ValueType<Len>(ZERO|POSITIVE);
            return ValueType<Len>();
        }

        template<size_t Len>
        ValueType<1> equalToZero(const ValueType<Len> &a) const {
            return ValueType<1>(a.sign==ZERO ? POSITIVE : ZERO);
        }

        template<size_t Len>
        ValueType<Len> invert(const ValueType<Len> &a) const {
            unsigned sign = (a.sign & ZERO) ? NEGATIVE : 0;
            if (a.sign & NEGATIVE)
                sign |= POSITIVE | ZERO;
            if (a.sign & POSITIVE)
                sign |= NEGATIVE;
            return ValueType<Len>(sign);
        }

        template<size_t Len1, size_t Len2>
        ValueType<Len1+Len2> concat(const ValueType<Len1> &lo, const ValueType<Len2> &hi) const {
            unsigned sign = ZERO | NEGATIVE | POSITIVE;
            if (1==Len2) {
                assert(0==(hi.sign & NEGATIVE));
                if (0==(hi.sign & POSITIVE))
                    sign &= ~NEGATIVE;
            } else if (0==(hi.sign & NEGATIVE)) {
                sign &= ~NEGATIVE;
            }
            if (hi.sign==ZERO && lo.sign==ZERO)
                sign = ZERO;
            return ValueType<Len1+Len2>(sign);
        }

        template<size_t Len>
        ValueType<Len> ite(const ValueType<1> &sel, const ValueType<Len> &ifTrue, const ValueType<Len> &ifFalse) const {
            assert(0==(sel.sign & NEGATIVE));
            if (sel.sign==ZERO)
                return ifFalse;
            if (sel.sign==POSITIVE)
                return ifTrue;
            return ValueType<Len>(ifTrue.sign | ifFalse.sign);
        }

        template<size_t Len>
        ValueType<Len> leastSignificantSetBit(const ValueType<Len> &a) const {
            return ValueType<Len>(a.sign==ZERO ? ZERO : POSITIVE);
        }

        template<size_t Len>
        ValueType<Len> mostSignificantSetBit(const ValueType<Len> &a) const {
            return ValueType<Len>(a.sign==ZERO ? ZERO : POSITIVE);
        }

        template<size_t Len>
        ValueType<Len> negate(const ValueType<Len> &a) const {
            unsigned sign = (a.sign & ZERO) ? ZERO : 0;
            if (a.sign & NEGATIVE) sign |= POSITIVE;
            if (a.sign & POSITIVE) sign |= NEGATIVE;
            return ValueType<Len>(sign);
        }

        template<size_t Len>
        ValueType<Len> or_(const ValueType<Len> &a, const ValueType<Len> &b) const {
            if (a.sign==ZERO && b.sign==ZERO)
                return ValueType<Len>(ZERO);
            if (0==(a.sign & NEGATIVE) && 0==(b.sign & NEGATIVE))
                return ValueType<Len>(POSITIVE);
            return ValueType<Len>(NEGATIVE);
        }

        template<size_t Len, size_t SALen>
        ValueType<Len> rotateLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
            if (sa.sign==ZERO)
                return a;
            if (a.sign==ZERO)
                return ValueType<Len>(ZERO);
            return ValueType<Len>(POSITIVE|NEGATIVE);
        }

        template<size_t Len, size_t SALen>
        ValueType<Len> rotateRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
            if (sa.sign==ZERO)
                return a;
            if (a.sign==ZERO)
                return ValueType<Len>(ZERO);
            return ValueType<Len>(POSITIVE|NEGATIVE);
        }

        template<size_t Len, size_t SALen>
        ValueType<Len> shiftLeft(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
            if (sa.sign==ZERO)
                return a;
            if (a.sign==ZERO)
                return ValueType<Len>(ZERO);
            return ValueType<Len>(POSITIVE|NEGATIVE);
        }

        template<size_t Len, size_t SALen>
        ValueType<Len> shiftRight(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
            if (sa.sign==ZERO)
                return a;
            if (a.sign==ZERO)
                return ValueType<Len>(ZERO);
            return ValueType<Len>(ZERO|POSITIVE);
        }

        template<size_t Len, size_t SALen>
        ValueType<Len> shiftRightArithmetic(const ValueType<Len> &a, const ValueType<SALen> &sa) const {
            if (sa.sign==ZERO || a.sign==ZERO)
                return a;
            unsigned sign = a.sign;
            if (a.sign & POSITIVE) sign |= ZERO;
            return ValueType<Len>(sign);
        }

        template<size_t Len1, size_t Len2>
        ValueType<Len1> signedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            // ignoring division by zero for now (should eventually throw an exception of some sort)
            unsigned sign = (a.sign & ZERO) ? ZERO : 0;
            if (((a.sign & POSITIVE) && (b.sign & POSITIVE)) || ((a.sign & NEGATIVE) && (b.sign & NEGATIVE)))
                sign |= POSITIVE;
            if (((a.sign & POSITIVE) && (b.sign & NEGATIVE)) || ((a.sign & NEGATIVE) && (b.sign & POSITIVE)))
                sign |= NEGATIVE;
            return ValueType<Len1>(sign);
        }

        template<size_t Len1, size_t Len2>
        ValueType<Len2> signedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            // ignoring division by zero for now (should eventually throw an exception of some sort)
            unsigned sign = (a.sign & ZERO) ? ZERO : 0;
            if (((a.sign & POSITIVE) && (b.sign & POSITIVE)) || ((a.sign & NEGATIVE) && (b.sign & NEGATIVE)))
                sign |= POSITIVE;
            if (((a.sign & POSITIVE) && (b.sign & NEGATIVE)) || ((a.sign & NEGATIVE) && (b.sign & POSITIVE)))
                sign |= NEGATIVE;
            return ValueType<Len2>(sign);
        }

        template<size_t Len1, size_t Len2>
        ValueType<Len1+Len2> signedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            unsigned sign = ((a.sign & ZERO) || (b.sign & ZERO)) ? ZERO : 0;
            if (((a.sign & POSITIVE) && (b.sign & POSITIVE)) || ((a.sign & NEGATIVE) && (b.sign & NEGATIVE)))
                sign |= POSITIVE;
            if (((a.sign & POSITIVE) && (b.sign & NEGATIVE)) || ((a.sign & NEGATIVE) && (b.sign & POSITIVE)))
                sign |= NEGATIVE;
            return ValueType<Len1+Len2>(sign);
        }

        template<size_t Len1, size_t Len2>
        ValueType<Len1> unsignedDivide(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            // ignoring division by zero for now (should eventually throw an exception of some sort)
            return ValueType<Len1>(a.sign & ~NEGATIVE);
        }

        template<size_t Len1, size_t Len2>
        ValueType<Len2> unsignedModulo(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            // ignoring division by zero for now (should eventually throw an exception of some sort)
            return ValueType<Len2>(a.sign & ~NEGATIVE);
        }

        template<size_t Len1, size_t Len2>
        ValueType<Len1+Len2> unsignedMultiply(const ValueType<Len1> &a, const ValueType<Len2> &b) const {
            unsigned sign = ((a.sign & ZERO) || (b.sign & ZERO)) ? ZERO : 0;
            if (0==(a.sign & ZERO) || 0==(b.sign & ZERO)) sign |= POSITIVE;
            return ValueType<Len1+Len2>(sign);
        }

        template<size_t Len>
        ValueType<Len> xor_(const ValueType<Len> &a, const ValueType<Len> &b) const {
            if (a.sign==ZERO)
                return b;
            if (b.sign==ZERO)
                return a;
            return ValueType<Len>();
        }

        /** Returns the register dictionary. */
        const RegisterDictionary *get_register_dictionary() const {
            return regdict ? regdict : RegisterDictionary::dictionary_pentium4();
        }

        /** Sets the register dictionary. */
        void set_register_dictionary(const RegisterDictionary *regdict) {
            this->regdict = regdict;
        }

#include "ReadWriteRegisterFragment.h"

        template<size_t Len>
        ValueType<Len> readMemory(X86SegmentRegister segreg, const ValueType<32> &addr, ValueType<1> cond) const {
            unsigned sign = 0;
            if (addr.sign & ZERO)
                sign |= cur_state.memory[0].sign;
            if (addr.sign & POSITIVE)
                sign |= cur_state.memory[1].sign;
            if (addr.sign & NEGATIVE)
                sign |= cur_state.memory[2].sign;
            return ValueType<Len>(sign);
        }

        template<size_t Len>
        void writeMemory(X86SegmentRegister segreg, const ValueType<32> &addr, const ValueType<Len> &data, ValueType<1> cond) {
            if (addr.sign==ZERO) {
                cur_state.memory[0] = data;                       // must alias
            } else if (addr.sign==POSITIVE) {
                cur_state.memory[1] = data;                       // must alias
            } else if (addr.sign==NEGATIVE) {
                cur_state.memory[2] = data;                       // must alias
            } else {
                if (addr.sign & ZERO)
                    cur_state.memory[0].sign |= data.sign;        // may alias
                if (addr.sign & POSITIVE)
                    cur_state.memory[1].sign |= data.sign;        // may alias
                if (addr.sign & NEGATIVE)
                    cur_state.memory[2].sign |= data.sign;        // may alias
            }
        }

    };
}; /*namespace*/

#endif
