#ifndef ROSE_BinaryAnalysis_InstructionSemantics_DispatcherAarch64_H
#define ROSE_BinaryAnalysis_InstructionSemantics_DispatcherAarch64_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_AARCH64

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionEnumsAarch64.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** Shared-ownership pointer to an A64 instruction dispatcher. */
using DispatcherAarch64Ptr = boost::shared_ptr<class DispatcherAarch64>;

class DispatcherAarch64: public BaseSemantics::Dispatcher {
public:
    /** Base type. */
    using Super = BaseSemantics::Dispatcher;

    /** Shared-ownership pointer. */
    using Ptr = DispatcherAarch64Ptr;

public:
    /** Cached register.
     *
     *  This register is cached so that there are not so amny calls to @ref BaseSemantics::Dispatcher::findRegister. Changing
     *  the @ref BaseSemantics::Dispatcher::registerDictionary "registerDictionary" property updates all entries of this cache.
     *
     *  @{ */
    RegisterDescriptor REG_PC, REG_SP, REG_LR;
    RegisterDescriptor REG_CPSR_N, REG_CPSR_Z, REG_CPSR_C, REG_CPSR_V;
    /** @} */

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void save(S &s, const unsigned /*version*/) const {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
    }

    template<class S>
    void load(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        initializeRegisterDescriptors();
        initializeInsnDispatchTable();
        initializeMemory();
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

private:
    DispatcherAarch64();                                // used only by boost::serialization

protected:
    // prototypical constructor
    DispatcherAarch64(const Architecture::BaseConstPtr&);

    DispatcherAarch64(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

public:
    ~DispatcherAarch64();

    /** Construct a prototypical dispatcher.
     *
     *  The only thing this dispatcher can be used for is to create another dispatcher with the virtual @ref create method. */
    static DispatcherAarch64Ptr instance(const Architecture::BaseConstPtr&);

    /** Allocating constructor. */
    static DispatcherAarch64Ptr instance(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

    /** Virtual constructor. */
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr&) const override;

    /** Dynamic cast to DispatcherAarch64 with assertion. */
    static DispatcherAarch64Ptr promote(const BaseSemantics::DispatcherPtr&);

protected:
    /** Initialized cached register descriptors from the register dictionary. */
    void initializeRegisterDescriptors();

    /** Initializes the instruction dispatch table.
     *
     *  This is called from the constructor. */
    void initializeInsnDispatchTable();

    /** Make sure memory is configured correctly, such as setting the byte order. */
    void initializeMemory();

public:
    BaseSemantics::SValuePtr read(SgAsmExpression*, size_t value_nbits=0, size_t addr_nbits=0) override;
    void write(SgAsmExpression*, const BaseSemantics::SValuePtr &value, size_t addr_nbits=0) override;

    // Operations more or less defined by the A64 reference manual. Replicates the specified value according to the vector type.
    BaseSemantics::SValuePtr advSimdExpandImm(SgAsmType*, const BaseSemantics::SValuePtr&);

    struct NZCV {
        BaseSemantics::SValuePtr n;
        BaseSemantics::SValuePtr z;
        BaseSemantics::SValuePtr c;
        BaseSemantics::SValuePtr v;

        NZCV() {}

        NZCV(const BaseSemantics::SValuePtr &n, const BaseSemantics::SValuePtr &z,
             const BaseSemantics::SValuePtr &c, const BaseSemantics::SValuePtr &v)
            : n(n), z(z), c(c), v(v) {}
    };

    // Compute the NZCV bits based on the result of an addition and the carries returned by RiscOperators::addWithCarries.
    NZCV computeNZCV(const BaseSemantics::SValuePtr &sum, const BaseSemantics::SValuePtr &carries);

    // Set the NZCV bits based on the result of an addition and the carries returned by RiscOperators::addWithCarries.
    void updateNZCV(const BaseSemantics::SValuePtr &sum, const BaseSemantics::SValuePtr &carries);

    // Return true or false depending on whether the condition holds.
    BaseSemantics::SValuePtr conditionHolds(Aarch64InstructionCondition);

    // From ARM documentation: "Decode AArch64 bitfield and logical immediate masks which use a similar encoding structure."
    std::pair<uint64_t, uint64_t> decodeBitMasks(size_t m, bool immN, uint64_t imms, uint64_t immr, bool immediate);

    // Handles the rather tricky BFM instruction, which is a general case of a few other instructions.
    void bitfieldMove(BaseSemantics::RiscOperators *ops, SgAsmExpression *dst, SgAsmExpression *src, bool n,
                      uint64_t immR, uint64_t immS);

    // Handles the rather tricky UBFM instruction, which is a general case of a few other instructions.
    void unsignedBitfieldMove(BaseSemantics::RiscOperators *ops, SgAsmExpression *dst, SgAsmExpression *src, bool n,
                              uint64_t immR, uint64_t immS);

    // Handles the rather tricky SBFM instruction, which is a general case of a few other instructions.
    void signedBitfieldMove(BaseSemantics::RiscOperators *ops, SgAsmExpression *dst, SgAsmExpression *src, bool n,
                            uint64_t immR, uint64_t immS);

protected:
    int iprocKey(SgAsmInstruction*) const override;
    RegisterDescriptor instructionPointerRegister() const override;
    RegisterDescriptor stackPointerRegister() const override;
    RegisterDescriptor stackFrameRegister() const override;
    RegisterDescriptor callReturnRegister() const override;
};

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::DispatcherAarch64);
#endif

#endif
#endif
