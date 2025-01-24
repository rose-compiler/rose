#ifndef ROSE_BinaryAnalysis_InstructionSemantics_DispatcherCil_H
#define ROSE_BinaryAnalysis_InstructionSemantics_DispatcherCil_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>
#include <Rose/BinaryAnalysis/InstructionEnumsCil.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** Shared-ownership pointer to an CIL instruction dispatcher. */
typedef boost::shared_ptr<class DispatcherCil> DispatcherCilPtr;

class DispatcherCil: public BaseSemantics::Dispatcher {
public:
    /** Base type. */
    using Super = BaseSemantics::Dispatcher;

    /** Shared-ownership pointer. */
    using Ptr = DispatcherCilPtr;

public:
    /** Cached register.
     *
     *  This register is cached so that there are not so many calls to Dispatcher::findRegister(). Changing the @ref
     *  registerDictionary property invalidates all entries of the cache.
     *
     * @{ */
    RegisterDescriptor REG_D[8], REG_A[8], REG_FP[8], REG_PC, REG_CCR, REG_CCR_C, REG_CCR_V, REG_CCR_Z, REG_CCR_N, REG_CCR_X;
    RegisterDescriptor REG_MACSR_SU, REG_MACSR_FI, REG_MACSR_N, REG_MACSR_Z, REG_MACSR_V, REG_MACSR_C, REG_MAC_MASK;
    RegisterDescriptor REG_MACEXT0, REG_MACEXT1, REG_MACEXT2, REG_MACEXT3, REG_SSP, REG_SR_S, REG_SR, REG_VBR;
    // Floating-point condition code bits
    RegisterDescriptor REG_FPCC_NAN, REG_FPCC_I, REG_FPCC_Z, REG_FPCC_N;
    // Floating-point status register exception bits
    RegisterDescriptor REG_EXC_BSUN, REG_EXC_OPERR, REG_EXC_OVFL, REG_EXC_UNFL, REG_EXC_DZ, REG_EXC_INAN;
    RegisterDescriptor REG_EXC_IDE, REG_EXC_INEX;
    // Floating-point status register accrued exception bits
    RegisterDescriptor REG_AEXC_IOP, REG_AEXC_OVFL, REG_AEXC_UNFL, REG_AEXC_DZ, REG_AEXC_INEX;
    /** @} */

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
    };

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        regcache_init();
        iproc_init();
        memory_init();
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

private:
    DispatcherCil();                                    // used only by boost::serialization

protected:
    DispatcherCil(const Architecture::BaseConstPtr&);   // prototypical constructor

    DispatcherCil(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

    /** Loads the iproc table with instruction processing functors.
     *
     *  This normally happens from the constructor. */
    void iproc_init();

    /** Load the cached register descriptors.
     *
     *  This happens at construction when the @ref registerDictionary property is changed. */
    void regcache_init();

    /** Make sure memory is set up correctly.
     *
     *  For instance, byte order should be big endian. */
    void memory_init();

public:
    ~DispatcherCil();

    /** Construct a prototypical dispatcher.
     *
     *  The only thing this dispatcher can be used for is to create another dispatcher with the virtual @ref create method. */
    static DispatcherCilPtr instance(const Architecture::BaseConstPtr&);

    /** Constructor. */
    static DispatcherCilPtr instance(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

    /** Virtual constructor. */
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr&) const override;

    /** Dynamic cast to DispatcherCilPtr with assertion. */
    static DispatcherCilPtr promote(const BaseSemantics::DispatcherPtr&);

    virtual RegisterDescriptor instructionPointerRegister() const override;
    virtual RegisterDescriptor stackPointerRegister() const override;
    virtual int iprocKey(SgAsmInstruction *insn_) const override;
    virtual BaseSemantics::SValuePtr read(SgAsmExpression*, size_t value_nbits, size_t addr_nbits=0) override;

    /** Set or clear FPSR EXC INAN bit. */
    void updateFpsrExcInan(const BaseSemantics::SValuePtr &a, SgAsmType *aType,
                           const BaseSemantics::SValuePtr &b, SgAsmType *bType);

    /** Set or clear FPSR EXC IDE bit. */
    void updateFpsrExcIde(const BaseSemantics::SValuePtr &a, SgAsmType *aType,
                          const BaseSemantics::SValuePtr &b, SgAsmType *bType);

    /** Set or clear FPSR EXC OVFL bit.
     *
     *  Set if the destination is a floating-point data register or memory (@p dstType) and the intermediate result (@p
     *  intermediate) has an exponent that is greater than or equal to the maximum exponent value of the selected rounding
     *  precision (@p rounding) */
    void updateFpsrExcOvfl(const BaseSemantics::SValuePtr &intermediate, SgAsmType *valueType,
                           SgAsmType *rounding, SgAsmType *dstType);

    /** Set or clear FPSR EXC UVFL bit.
     *
     *  Set if the intermediate result of an arithmetic instruction is too small to be represented as a normalized number in a
     *  floating-point register or memory using the selected rounding precision, that is, when the intermediate result exponent
     *  is less than or equal to the minimum exponent value of the selected rounding precision. Cleared otherwise. Underflow
     *  can ony occur when the desitnation format is single or double precision. When the destination is byte, word, or
     *  longword, the conversion ounderflows to zero without causing an underflow or an operand error. */
    void updateFpsrExcUnfl(const BaseSemantics::SValuePtr &intermediate, SgAsmType *valueType,
                           SgAsmType *rounding, SgAsmType *dstType);

    /** Set or clear FPSR EXC INEX bit. */
    void updateFpsrExcInex();

    /** Determines if an instruction should branch. */
    BaseSemantics::SValuePtr condition(CilInstructionKind, BaseSemantics::RiscOperators*);

    /** Update accrued floating-point exceptions. */
    void accumulateFpExceptions();

    /** Set floating point condition codes according to result. */
    void adjustFpConditionCodes(const BaseSemantics::SValuePtr &result, SgAsmFloatType*);
};

} // namespace
} // namespace
} // namespace

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::DispatcherCil);
#endif

#endif
#endif
