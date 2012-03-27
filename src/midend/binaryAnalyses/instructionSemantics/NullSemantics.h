#ifndef Rose_NullSemantics_H
#define Rose_NullSemantics_H

#include "x86InstructionSemantics.h"
#include "BaseSemantics.h"

namespace BinaryAnalysis { // documented elsewhere
    namespace InstructionSemantics { // documented elsewhere
        

        /** Semantic domain that does nothing, but is well documented.
         *
         *  This semantic domain is useful for testing, as a no-op in a multi-policy, as documentation for the instruction
         *  semantics API (the RISC operations), or for debugging. */
        namespace NullSemantics {

            /** Type of values in semantic domain.  The semantic policy defines the data type for values.  Values should be
             *  copyable and printable objects.  They don't need a default constructor, but if there is a default constructor
             *  it should construct a value that has an undefined/unconstrained/unknown value. */
            template<size_t nBits>
            struct ValueType {
                ValueType() {}
                explicit ValueType(uint64_t) {}
                bool is_known() const { return false; }
                uint64_t known_value() const { abort(); } // values are NEVER known
                void print(std::ostream &o) const {
                    o <<"VOID";
                }
            };

            /** Output operator.  Values in a semantic domain should be printable. */
            template<size_t nBits>
            std::ostream& operator<<(std::ostream &o, const ValueType<nBits> &e) {
                e.print(o);
                return o;
            }

            /** Virtual machine state.  Policies generally maintain a "current state" upon which RISC operations operate.  For
             *  instantce, the writeRegister() method modifies the current state.  A state contains values whose types are
             *  defined by the policy. */
            template <template <size_t> class ValueType=NullSemantics::ValueType>
            struct State {};

            /** Null policy.  The policy is responsible for defining the RISC operations needed by the translation class.  This
             *  particular policy's RISC operations don't do anything (other than returning a null value), but we document them
             *  here anyway in order to describe the API.
             *
             *  Policies generally take template arguments to define the data types for the machine state and value types,
             *  although they default to reasonable types. */
            template<
                template <template <size_t> class ValueType> class State = NullSemantics::State,
                template <size_t nBits> class ValueType = NullSemantics::ValueType
            >
            class Policy: public BaseSemantics::Policy {
            public:

                /** Default constructor.  The default constructor should probably indicate the register dictionary used by the
                 *  policy.  The user can change the register dictionary later, but should not change it after the policy is
                 *  used to instantiate a translation object (see X86InstructionSemantics). */
                Policy() {
                    set_register_dictionary(RegisterDictionary::dictionary_pentium4());
                }

                /** Called at the beginning of every instruction.  This method is invoked every time the translation object
                 *  begins processing an instruction.  Some policies use this to update a pointer to the current
                 *  instruction. */
                void startInstruction(SgAsmInstruction*) {}

                /** Called at the end of every instruction.  This method is invoked whenever the translation object ends
                 *  processing for an instruction.  Don't expect it to be called if there's an exception during processing
                 *  (FIXME). */
                void finishInstruction(SgAsmInstruction*) {}

                /** Returns a true value. */
                ValueType<1> true_() {
                    return ValueType<1>();
                }

                /** Returns a false value. */
                ValueType<1> false_() {
                    return ValueType<1>();
                }

                /** Returns an undefined single-bit value. The value is undefined, unknown, and/or unconstrained. */
                template<size_t nBits>
                ValueType<nBits> undefined_() {
                    return ValueType<nBits>();
                }

                /** Returns a number of the specified bit width.  This method converts an unsigned integer to a value in the
                 *  semantic domain. */
                template<size_t nBits>
                ValueType<nBits> number(uint64_t) {
                    return ValueType<nBits>();
                }

                /** Invoked to filter call targets.  This method is called whenever the translation object is about to invoke a
                 *  function call.  The target address is passed as an argument and a (new) target should be returned. */
                ValueType<32> filterCallTarget(const ValueType<32> &a) {
                    return a;
                }

                /** Invoked to filter return targets.  This method is called whenever the translation object is about to return
                 *  from a function call (such as for the x86 "RET" instruction).  The return address is passed as an argument
                 *  and a (new) return address should be returned. */
                ValueType<32> filterReturnTarget(const ValueType<32> &a) {
                    return a;
                }

                /** Invoked to filter indirect jumps.  This method is called whenever the translation object is about to
                 *  unconditionally jump to a new address (such as for the x86 "JMP" instruction).  The target address is passed
                 *  as an argument and a (new) target address should be returned. */
                ValueType<32> filterIndirectJumpTarget(const ValueType<32> &a) {
                    return a;
                }

                /** Invoked for the x86 HLT instruction. */
                void hlt() {}

                /** Invoked for the x86 CPUID instruction. */
                void cpuid() {}

                /** Invoked for the x86 RDTSC instruction. */
                ValueType<64> rdtsc() {
                    return ValueType<64>();
                }

                /** Invoked for the x86 INT instruction. */
                void interrupt(uint8_t) {}

                /** Invoked for the x86 SYSENTER instruction. */
                void sysenter() {}

                /** Adds two integers. */
                template<size_t nBits>
                ValueType<nBits> add(const ValueType<nBits> &a, const ValueType<nBits> &b) {
                    return ValueType<nBits>();
                }

                /** Add two values of equal size and a carry bit.  Carry information is returned via carry_out argument.  The
                 *  carry_out value is the tick marks that are written above the first addend when doing long arithmetic like a
                 *  2nd grader would do (of course, they'd probably be adding two base-10 numbers).  For instance, when adding
                 *  00110110 and 11100100:
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
                template<size_t nBits>
                ValueType<nBits> addWithCarries(const ValueType<nBits> &a, const ValueType<nBits> &b, const ValueType<1> &c,
                                                ValueType<nBits> &carry_out) {
                    return ValueType<nBits>();
                }

                /** Computes bit-wise AND of two values. */
                template<size_t nBits>
                ValueType<nBits> and_(const ValueType<nBits> &a, const ValueType<nBits> &b) {
                    return ValueType<nBits>();
                }

                /** Determines whether a value is equal to zero.  Returns true, false, or undefined (in the semantic domain)
                 *  depending on whether argument is zero. */
                template<size_t nBits>
                ValueType<1> equalToZero(const ValueType<nBits> &a) {
                    return ValueType<1>();
                }

                /** One's complement */
                template<size_t nBits>
                ValueType<nBits> invert(const ValueType<nBits> &a) {
                    return ValueType<nBits>();
                }

                /** Extracts bits from a value.  The specified bits from BeginBit (inclusive) through EndBit (exclusive) are
                 *  copied into the low-order bits of the return value (other bits in the return value are cleared). The least
                 *  significant bit is number zero. */
                template<size_t BeginBit, size_t EndBit, size_t nBitsA>
                ValueType<EndBit-BeginBit> extract(const ValueType<nBitsA> &a) {
                    return ValueType<EndBit-BeginBit>();
                }

                /** Concatenates the bits of two values.  The bits of @p a and @p b are concatenated so that the result has @p
                 *  b in the high-order bits and @p a in the low order bits. */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA+nBitsB> concat(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsA+nBitsB>();
                }

                /** If-then-else.  Returns operand @p a if @p cond is true, operand @p b if @p cond is false, or some other
                 *  value if the condition is unknown. */
                template<size_t nBits>
                ValueType<nBits> ite(const ValueType<1> &cond, const ValueType<nBits> &a, const ValueType<nBits> &b) {
                    return ValueType<nBits>();
                }

                /** Returns position of least significant set bit; zero when no bits are set. */
                template<size_t nBits>
                ValueType<nBits> leastSignificantSetBit(const ValueType<nBits> &a) {
                    return ValueType<nBits>();
                }

                /** Returns position of most significant set bit; zero when no bits are set. */
                template<size_t nBits>
                ValueType<nBits> mostSignificantSetBit(const ValueType<nBits> &a) {
                    return ValueType<nBits>();
                }

                /** Two's complement. */
                template<size_t nBits>
                ValueType<nBits> negate(const ValueType<nBits> &a) {
                    return ValueType<nBits>();
                }

                /** Computes bit-wise OR of two values. */
                template<size_t nBits>
                ValueType<nBits> or_(const ValueType<nBits> &a, const ValueType<nBits> &b) {
                    return ValueType<nBits>();
                }

                /** Rotate bits to the left. */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> rotateLeft(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsA>();
                }

                /** Rotate bits to the right. */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> rotateRight(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsA>();
                }

                /** Returns arg shifted left. */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> shiftLeft(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsA>();
                }

                /** Returns arg shifted right logically (no sign bit). */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> shiftRight(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsA>();
                }

                /** Returns arg shifted right arithmetically (with sign bit). */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> shiftRightArithmetic(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsA>();
                }

                /** Extend (or shrink) from @p FromLen bits to @p ToLen bits by adding or removing high-order bits from the
                 *  input. Added bits are always zeros. */
                template <size_t FromLen, size_t ToLen>
                ValueType<ToLen> unsignedExtend(const ValueType<FromLen> &a) {
                    return ValueType<ToLen>();
                }

                /** Sign extends a value. */
                template<size_t From, size_t To>
                ValueType<To> signExtend(const ValueType<From> &a) {
                    return ValueType<To>();
                }

                /** Divides two signed values. */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> signedDivide(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsA>();
                }

                /** Calculates modulo with signed values. */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsB> signedModulo(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsB>();
                }

                /** Multiplies two signed values. */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA+nBitsB> signedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsA+nBitsB>();
                }

                /** Divides two unsigned values. */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA> unsignedDivide(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsA>();
                }

                /** Calculates modulo with unsigned values. */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsB> unsignedModulo(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsB>();
                }

                /** Multiply two unsigned values. */
                template<size_t nBitsA, size_t nBitsB>
                ValueType<nBitsA+nBitsB> unsignedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) {
                    return ValueType<nBitsA+nBitsB>();
                }

                /** Computes bit-wise XOR of two values. */
                template<size_t nBits>
                ValueType<nBits> xor_(const ValueType<nBits> &a, const ValueType<nBits> &b) {
                    return ValueType<nBits>();
                }

                /** Reads from a named register. */
                template<size_t nBits>
                ValueType<nBits> readRegister(const char *regname) {
                    return ValueType<nBits>();
                }

                /** Generic register read. */
                template<size_t nBits>
                ValueType<nBits> readRegister(const RegisterDescriptor &reg) {
                    return ValueType<nBits>();
                }

                /** Writes to a named register. */
                template<size_t nBits>
                void writeRegister(const char *regname, const ValueType<nBits> &a) {}

                /** Generic register write. */
                template<size_t nBits>
                void writeRegister(const RegisterDescriptor &reg, const ValueType<nBits> &a) {}

                /** Reads a value from memory. */
                template<size_t nBits>
                ValueType<nBits> readMemory(X86SegmentRegister, const ValueType<32> &addr, const ValueType<1> &cond) {
                    return ValueType<nBits>();
                }

                /** Writes a value to memory. */
                template<size_t nBits>
                void writeMemory(X86SegmentRegister, const ValueType<32> &addr, const ValueType<nBits> &data,
                                 const ValueType<1> &cond) {}

                /**************************************************************************************************************
                 *                                      Additional methods, etc.
                 *
                 * These are not part of the semantic API but are useful nonetheless.
                 **************************************************************************************************************/
            public:

                /** Print the current state of the policy.  A NullSemantic policy has no state, so we just print the string
                 * "null state". Policy states are normally multi-line output that includes a final line terminator. */
                void print(std::ostream &o) const {
                    o <<"null state\n";
                }

                friend std::ostream& operator<<(std::ostream &o, const Policy &policy) {
                    policy.print(o);
                    return o;
                }
            };
        } /*namespace*/
    } /*namespace*/
} /*namespace*/

#endif
