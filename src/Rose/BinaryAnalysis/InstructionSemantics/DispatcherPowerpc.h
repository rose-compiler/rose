// Semantics for 32-bit Motorola-IBM PowerPC microprocessors using ROSE instruction semantics API2
// This code was derived from $ROSE/projects/assemblyToSourceAst/powerpcInstructionSemantics.h,
// which is mostly a copy of $ROSE/projects/SemanticSignatureVectors/powerpcInstructionSemantics.h
//
// The ROSE style guide indicates that PowerPC, when used as part of a symbol in ROSE source code,
// should be capitalized as "Powerpc" (e.g., "DispatcherPowerpc", the same rule that consistently
// capitializes x86 as "DispatcherX86").
#ifndef ROSE_BinaryAnalysis_InstructionSemantics_DispatcherPowerpc_H
#define ROSE_BinaryAnalysis_InstructionSemantics_DispatcherPowerpc_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** Shared-ownership pointer to a PowerPC instruction dispatcher. */
typedef boost::shared_ptr<class DispatcherPowerpc> DispatcherPowerpcPtr;

class DispatcherPowerpc: public BaseSemantics::Dispatcher {
public:
    /** Base type. */
    using Super = BaseSemantics::Dispatcher;

    /** Shared-ownership pointer. */
    using Ptr = DispatcherPowerpcPtr;

    /** Cached register.
     *
     *  This register is cached so that there are not so many calls to Dispatcher::findRegister(). The register descriptor is
     *  updated only when the @ref registerDictionary property is changed.
     *
     *  @{ */
    RegisterDescriptor REG_IAR, REG_LR, REG_XER, REG_XER_CA, REG_XER_OV, REG_XER_SO, REG_CTR;
    RegisterDescriptor REG_CR, REG_CR0, REG_CR0_LT;
    /** @}*/

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
        regcache_init();
        iproc_init();
        memory_init();
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER();
#endif

private:
    DispatcherPowerpc();                                // used only by boost::serialization

protected:
    DispatcherPowerpc(const Architecture::BaseConstPtr&); // Prototypical constructor

    DispatcherPowerpc(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

public:
    ~DispatcherPowerpc();

private:
    /** Loads the iproc table with instruction processing functors.
     *
     *  This normally happens from the constructor. */
    void iproc_init();

    /** Load the cached register descriptors.
     *
     *  This happens at construction and when the @ref registerDictionary is changed. */
    void regcache_init();

    /** Make sure memory is set up correctly.
     *
     *  For instance, byte order should be little endian. */
    void memory_init();

public:
    /** Construct a prototypical dispatcher.
     *
     *  The only thing this dispatcher can be used for is to create another dispatcher with the virtual @ref create method. */
    static DispatcherPowerpcPtr instance(const Architecture::BaseConstPtr&);

    /** Constructor. */
    static DispatcherPowerpcPtr instance(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

    /** Virtual constructor. */
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr&) const override;

    /** Dynamic cast to a DispatcherPowerpcPtr with assertion. */
    static DispatcherPowerpcPtr promote(const BaseSemantics::DispatcherPtr&);

    virtual RegisterDescriptor instructionPointerRegister() const override;
    virtual RegisterDescriptor callReturnRegister() const override;

    virtual int iprocKey(SgAsmInstruction *insn_) const override;

    /** Set the XER OV and SO bits as specified.
     *
     *  The XER OV bit is assigned the argument, and the XER SO bit is set only if the argument is set. This function should
     *  be called before @ref updateCr0 since @ref updateCr0 will copy some of the XER into the CR result. */
    void setXerOverflow(const BaseSemantics::SValuePtr &hadOverflow);

    /** Write status flags for result. */
    virtual void updateCr0(const BaseSemantics::SValuePtr &result);

    /** Reads from a memory address and updates a register with the effective address that was read. The address expression
     *  must be a binary add operation whose first argument is a register, and it is this register that gets updated. */
    BaseSemantics::SValuePtr readAndUpdate(BaseSemantics::RiscOperators*, SgAsmExpression*, size_t valueNBits);

    /** Writes a value to a memory address and updates a register with the effective address to which the value was
     *  written. The address expression must be a binary add operation whose first argument is a register, and it is this
     *  register that gets updated. */
    void writeAndUpdate(BaseSemantics::RiscOperators*, SgAsmExpression *destination, const BaseSemantics::SValuePtr &value);
};
        
} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics::DispatcherPowerpc);
#endif

#endif
#endif
