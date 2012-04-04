#ifndef Rose_NullSemantics_H
#define Rose_NullSemantics_H

#include "x86InstructionSemantics.h"

/** Semantic policy that does nothing.
 *
 *  This policy is useful for testing, as a no-op in a multi-policy, as documentation for the instruction semantics API (the
 *  RISC operations), or for debugging. */
namespace NullSemantics {

    template<size_t nBits>
    struct ValueType {
        bool is_known() const { return false; }
        uint64_t known_value() const { abort(); } // values are NEVER known
        void print(std::ostream &o) const {
            o <<"VOID";
        }
    };

    template<size_t nBits>
    std::ostream& operator<<(std::ostream &o, const ValueType<nBits> &e) {
        e.print(o);
        return o;
    }

    template <template <size_t> class ValueType=NullSemantics::ValueType>
    struct State {};

    template<
        template <template <size_t> class ValueType> class State,
        template <size_t nBits> class ValueType
    >
    class Policy {
    protected:
        // FIXME: This should probably be moved into a base class. [RPM 2012-04-04] */
        const RegisterDictionary *regdict;

    public:
        Policy(): regdict(NULL) {}

        void startInstruction(SgAsmInstruction*) {}
        void finishInstruction(SgAsmInstruction*) {}

        // FIXME: This should probably be moved into a base class. [RPM 2012-04-04] */
        struct Exception {
            Exception(const std::string &mesg): mesg(mesg) {}
            friend std::ostream& operator<<(std::ostream &o, const Exception &e) {
                o <<"NullSemantics exception: " <<e.mesg;
                return o;
            }
            std::string mesg;
        };

        // FIXME: This should probably be moved into a base class. [RPM 2012-04-04] */
        const RegisterDescriptor& findRegister(const std::string &regname, size_t nbits=0) {
            const RegisterDescriptor *reg = get_register_dictionary()->lookup(regname);
            if (!reg) {
                std::ostringstream ss;
                ss <<"Invalid register: \"" <<regname <<"\"";
                throw Exception(ss.str());
            }
            if (nbits>0 && reg->get_nbits()!=nbits) {
                std::ostringstream ss;
                ss <<"Invalid " <<nbits <<"-bit register: \"" <<regname <<"\" is "
                   <<reg->get_nbits() <<" " <<(1==reg->get_nbits()?"byte":"bytes");
                throw Exception(ss.str());
            }
            return *reg;
        }
            
        // FIXME: This should probably be moved into a base class. [RPM 2012-04-04] */
        const RegisterDictionary *get_register_dictionary() const {
            return regdict ? regdict : RegisterDictionary::dictionary_pentium4();
        }

        // FIXME: This should probably be moved into a base class. [RPM 2012-04-04] */
        void set_register_dictionary(const RegisterDictionary *regdict) {
            this->regdict = regdict;
        }

        ValueType<1> true_() {
            return ValueType<1>();
        }

        ValueType<1> false_() {
            return ValueType<1>();
        }

        ValueType<1> undefined_() {
            return ValueType<1>();
        }

        template<size_t nBits>
        ValueType<nBits> number(uint64_t) const {
            return ValueType<nBits>();
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

        void hlt() {}

        void cpuid() {}

        ValueType<64> rdtsc() {
            return ValueType<64>();
        }

        void interrupt(uint8_t) {}

        void sysenter() {}

        template<size_t nBits>
        ValueType<nBits> add(const ValueType<nBits> &a, const ValueType<nBits> &b) const {
            return ValueType<nBits>();
        }

        template<size_t nBits>
        ValueType<nBits> addWithCarries(const ValueType<nBits> &a, const ValueType<nBits> &b, const ValueType<1> &c,
                                        ValueType<nBits> &carry_out) const {
            return ValueType<nBits>();
        }

        template<size_t nBits>
        ValueType<nBits> and_(const ValueType<nBits> &a, const ValueType<nBits> &b) const {
            return ValueType<nBits>();
        }

        template<size_t nBits>
        ValueType<1> equalToZero(const ValueType<nBits> &a) const {
            return ValueType<1>();
        }

        template<size_t nBits>
        ValueType<nBits> invert(const ValueType<nBits> &a) const {
            return ValueType<nBits>();
        }

        template<size_t BeginBit, size_t EndBit, size_t nBitsA>
        ValueType<EndBit-BeginBit> extract(const ValueType<nBitsA> &a) const {
            return ValueType<EndBit-BeginBit>();
        }

        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsA+nBitsB> concat(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsA+nBitsB>();
        }

        template<size_t nBits>
        ValueType<nBits> ite(const ValueType<1> &cond, const ValueType<nBits> &a, const ValueType<nBits> &b) const {
            return ValueType<nBits>();
        }

        template<size_t nBits>
        ValueType<nBits> leastSignificantSetBit(const ValueType<nBits> &a) const {
            return ValueType<nBits>();
        }

        template<size_t nBits>
        ValueType<nBits> mostSignificantSetBit(const ValueType<nBits> &a) const {
            return ValueType<nBits>();
        }
        
        template<size_t nBits>
        ValueType<nBits> negate(const ValueType<nBits> &a) const {
            return ValueType<nBits>();
        }

        template<size_t nBits>
        ValueType<nBits> or_(const ValueType<nBits> &a, const ValueType<nBits> &b) const {
            return ValueType<nBits>();
        }

        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsA> rotateLeft(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsA>();
        }
        
        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsA> rotateRight(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsA>();
        }
        
        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsA> shiftLeft(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsA>();
        }
        
        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsA> shiftRight(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsA>();
        }
        
        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsA> shiftRightArithmetic(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsA>();
        }

        template<size_t From, size_t To>
        ValueType<To> signExtend(const ValueType<From> &a) {
            return ValueType<To>();
        }

        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsA> signedDivide(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsA>();
        }

        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsB> signedModulo(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsB>();
        }

        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsA+nBitsB> signedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsA+nBitsB>();
        }

        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsA> unsignedDivide(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsA>();
        }

        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsB> unsignedModulo(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsB>();
        }

        template<size_t nBitsA, size_t nBitsB>
        ValueType<nBitsA+nBitsB> unsignedMultiply(const ValueType<nBitsA> &a, const ValueType<nBitsB> &b) const {
            return ValueType<nBitsA+nBitsB>();
        }

        template<size_t nBits>
        ValueType<nBits> xor_(const ValueType<nBits> &a, const ValueType<nBits> &b) const {
            return ValueType<nBits>();
        }

        template<size_t nBits>
        ValueType<nBits> readRegister(const char *regname) const {
            return ValueType<nBits>();
        }

        template<size_t nBits>
        ValueType<nBits> readRegister(const RegisterDescriptor &reg) const {
            return ValueType<nBits>();
        }

        template<size_t nBits>
        void writeRegister(const char *regname, const ValueType<nBits> &a) {
            return ValueType<nBits>();
        }

        template<size_t nBits>
        void writeRegister(const RegisterDescriptor &reg, const ValueType<nBits> &a) {}

        template<size_t nBits>
        ValueType<nBits> readMemory(X86SegmentRegister, const ValueType<32> &addr, const ValueType<1> &cond) const {
            return ValueType<nBits>();
        }

        template<size_t nBits>
        void writeMemory(X86SegmentRegister, const ValueType<32> &addr, const ValueType<nBits> &data, const ValueType<1> &cond) {}
    };
}; /*namespace*/

#endif
