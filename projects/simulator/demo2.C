/* Demonstrates how to write a (very simple, naive) sign analysis.
 *
 * Assumes:
 *   * All values are signed integers (except 1-bit flags)
 *   * Operators are defined over infinite integers rather than modulo 2^31
 *   * This is an example, so we can afford to be rather sloppy
 *
 * A more accurate (and slower) way to do sign analysis is to use the SymbolicSemantics policy and an SMT solver to answer
 * questions about whether things are equal to zero, less than zero, or greater than zero.
 *
 * This example has two parts:
 *    1. The actual sign analysis engine contained in the SignAnalysisExample name space.  This is a collection of classes for
 *       value types (the domain), State (the model), and Policy (the operators).
 *    2. A small program that instantiates a simulator and registers a callback that drives the sign analysis.
 *
 * We drive the sign analysis in synchrony with the actual simulation.  This is a bit useless since we already know the sign
 * just by looking at the concrete values in the simulator.  But we actually need to do it this way (due to our design) because
 * the SignAnalysisExample's domain contains only <zero,positive,negative>, which isn't sufficient to determine the address of
 * the next instruction to simulate.  The instruction pointer, the EIP register, is after all, just a member of the sign
 * analysis state.
 *
 * There are several alternatives to running synchronously.  For instance, if we know the current instruction then we can
 * perform sign analysis on that instruction and all following instructions as long as we can determine a unique control-flow
 * successor.  I.e., we can do the analysis on following instructions of the same basic block, plus subsequent basic blocks as
 * long as we can determine branch targets at the end of each block.  However, as we've defined the domain for
 * SignAnalysisExample, the instruction fetching would have to be done outside that domain.
 *
 * Memory addresses:  since memory addresses are elements of the domain, we have only three possible memory locations: zero,
 * positive, and negative.  We use "must alias" and "may alias" when reading and writing to these three locations.
 */


#include "rose.h"
#include "RSIM_Private.h"

#ifdef ROSE_ENABLE_SIMULATOR /* protects this whole file */

#include "RSIM_Linux32.h"

/* Using VirtualMachineSemantics as a model, we put everything in a name space. */
namespace SignAnalysisExample {


    /* The ValueType defines the domain over which operators are defined. In this case, each value of our domain could be zero,
     * positive, or negative.  We use three bits since a value could be more than one at a time.  For instance, a completely
     * unknown value could be zero, it could be positive, and it could be negative, so we set all three bits. */
    enum Signedness {
        ZERO            = 0x01,
        POSITIVE        = 0x02,
        NEGATIVE        = 0x04
    };

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

    /* Instructions operate on a state, transitioning one state to the next state.  The state usually consists of the registers
     * and memory. We're assuming an i386. See also, Registers.h */
    class State {
    public:
        static const size_t n_gprs = 8;         /* Number of general purpose registers */
        static const size_t n_segregs = 6;      /* Number of 16-bit segmentation descriptor registers */
        static const size_t n_flags = 32;       /* We treat EFLAGS as individual 1-bit (unsigned) values */

        ValueType<32> ip;                       /* Instruction pointer */
        ValueType<32> gpr[n_gprs];              /* General purpose registers */
        ValueType<32> segreg[n_segregs];        /* Segmentation registers */
        ValueType<1> flag[n_flags];             /* Bits of EFLAGS */

        /* Memory cells are <address,value> pairs.  Addresses belong to the same domain as values, therefore we have only three
         * possible addresses: negative, zero, positive. */
        ValueType<32> memory[3];                /* 0=>zero, 1=>positive, 2=>negative */

        void print(std::ostream &o) const {
            for (size_t i=0; i<n_gprs; i++)
                o <<gprToString((X86GeneralPurposeRegister)i) <<"=" <<gpr[i] <<" ";
            for (size_t i=0; i<n_segregs; i++)
                o <<segregToString((X86SegmentRegister)i) <<"=" <<segreg[i] <<" ";
            o <<"ip=" <<ip <<" ";
            for (size_t i=0; i<n_flags; i++)
                o <<flagToString((X86Flag)i) <<"=" <<flag[i] <<" ";
            o <<"mem[0]=" <<memory[0] <<" ";
            o <<"mem[+]=" <<memory[1] <<" ";
            o <<"mem[-]=" <<memory[2] << std::endl;
        }

        friend std::ostream& operator<<(std::ostream &o, const State &state) {
            state.print(o);
            return o;
        }
    };


    /* The Policy defines the operators, whose operands and result are all ValueType of various sizes. */
    class Policy {
    private:
        State cur_state;

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
         * since the policy is "plugged into" an instruction semantics class.  See VirtualMachineSemantics.h for documentation
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
        ValueType<ToLen> extendByMSB(const ValueType<FromLen> &a) const {
            if (ToLen==FromLen) return ValueType<ToLen>(a);
            if (ToLen>FromLen) return ValueType<ToLen>(POSITIVE);
            return ValueType<ToLen>();
        }

        void startInstruction(SgAsmInstruction *insn) {
            cur_state.ip = ValueType<32>(insn->get_address());
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

        ValueType<1> undefined_() const {
            return ValueType<1>();
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

        ValueType<32> readGPR(X86GeneralPurposeRegister r) const {
            return cur_state.gpr[r];
        }

        void writeGPR(X86GeneralPurposeRegister r, const ValueType<32> &value) {
            cur_state.gpr[r] = value;
        }

        ValueType<16> readSegreg(X86SegmentRegister sr) const {
            return cur_state.segreg[sr];
        }

        void writeSegreg(X86SegmentRegister sr, const ValueType<16> &value) {
            cur_state.segreg[sr] = value;
        }

        ValueType<32> readIP() const {
            return cur_state.ip;
        }

        void writeIP(const ValueType<32> &value) {
            cur_state.ip = value;
        }

        ValueType<1> readFlag(X86Flag f) const {
            return cur_state.flag[f];
        }

        void writeFlag(X86Flag f, const ValueType<1> &value) {
            cur_state.flag[f] = value;
        }

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
    };
}; /*namespace*/





/*******************************************************************************************************************************/





/* This instruction callback constructs a sign analyzer and processes each instruction as it is executed.  Running it this way
 * is sort of pointless since we already know the sign of all concrete values (but this is only an example).  There are other
 * ways to run the analysis--for example, you could analyze instructions out beyond our current execution point.  See top of
 * this file for details. */
class SynchronousSignAnalysis: public RSIM_Callbacks::InsnCallback {
private:
    SignAnalysisExample::Policy policy;
    X86InstructionSemantics<SignAnalysisExample::Policy, SignAnalysisExample::ValueType> semantics;
public:
    SynchronousSignAnalysis(): semantics(policy) {}
    virtual SynchronousSignAnalysis *clone() { return this; }
    virtual bool operator()(bool enabled, const Args &args) {
        SgAsmx86Instruction *insn = isSgAsmx86Instruction(args.insn);
        if (enabled && insn) {
            semantics.processInstruction(insn);
            std::cerr <<policy;
        }
        return enabled;
    }
};

int
main(int argc, char *argv[], char *envp[])
{
    RSIM_Linux32 sim;
    int n = sim.configure(argc, argv, envp);

    /* Turn on instruction tracing regardless of command-line settings.  Note: This interface is not ideal; we're planning to
     * add methods to adjust tracing in a more friendly manner. */
    static const char *extras[] = {"INTERNAL", "--debug=insn"};
    sim.configure(2, (char**)extras, NULL);

    /* Install an instruction callback that does the sign analysis.  We could use the simpler install_callback() method, but we
     * want this callback to trigger _after_ each instruction in order to make the output more readable. */
    sim.get_callbacks().add_insn_callback(RSIM_Callbacks::AFTER, new SynchronousSignAnalysis);

    /* Run the simulation.  This is pretty much boiler plate. */
    sim.exec(argc-n, argv+n);
    sim.activate();
    sim.main_loop();
    sim.deactivate();
    sim.describe_termination(stderr);
    sim.terminate_self(); // never returns
    return 0;
}



#else

int main(int argc, char *argv[])
{
    std::cerr <<argv[0] <<": not supported on this platform" <<std::endl;
    return 0;
}

#endif
