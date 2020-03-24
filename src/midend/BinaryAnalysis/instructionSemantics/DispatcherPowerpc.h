// Semantics for 32-bit Motorola-IBM PowerPC microprocessors using ROSE instruction semantics API2
// This code was derived from $ROSE/projects/assemblyToSourceAst/powerpcInstructionSemantics.h,
// which is mostly a copy of $ROSE/projects/SemanticSignatureVectors/powerpcInstructionSemantics.h
//
// The ROSE style guide indicates that PowerPC, when used as part of a symbol in ROSE source code,
// should be capitalized as "Powerpc" (e.g., "DispatcherPowerpc", the same rule that consistently
// capitializes x86 as "DispatcherX86").
#ifndef ROSE_DispatcherPpc_H
#define ROSE_DispatcherPpc_H
#include <rosePublicConfig.h>
#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT

#include "BaseSemantics2.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

/** Shared-ownership pointer to a PowerPC instruction dispatcher. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class DispatcherPowerpc> DispatcherPowerpcPtr;

class DispatcherPowerpc: public BaseSemantics::Dispatcher {
public:
    typedef BaseSemantics::Dispatcher Super;

    /** Cached register. This register is cached so that there are not so many calls to Dispatcher::findRegister(). The
     *  register descriptor is updated only when the register dictionary is changed (see set_register_dictionary()).
     * @{ */
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

protected:
    // Prototypical constructor
    DispatcherPowerpc(): BaseSemantics::Dispatcher(32, RegisterDictionary::dictionary_powerpc32()) {}

    // Prototypical constructor
    DispatcherPowerpc(size_t addrWidth, const RegisterDictionary *regs/*=NULL*/)
        : BaseSemantics::Dispatcher(addrWidth, regs ? regs : SgAsmPowerpcInstruction::registersForWidth(addrWidth)) {}
    
    DispatcherPowerpc(const BaseSemantics::RiscOperatorsPtr &ops, size_t addrWidth, const RegisterDictionary *regs)
        : BaseSemantics::Dispatcher(ops, addrWidth, regs ? regs : SgAsmPowerpcInstruction::registersForWidth(addrWidth)) {
        ASSERT_require(32==addrWidth || 64==addrWidth);
        regcache_init();
        iproc_init();
        memory_init();
    }

    /** Loads the iproc table with instruction processing functors. This normally happens from the constructor. */
    void iproc_init();

    /** Load the cached register descriptors.  This happens at construction and on set_register_dictionary() calls. */
    void regcache_init();

    /** Make sure memory is set up correctly. For instance, byte order should be little endian. */
    void memory_init();

public:
    /** Construct a prototypical dispatcher.  The only thing this dispatcher can be used for is to create another dispatcher
     *  with the virtual @ref create method. */
    static DispatcherPowerpcPtr instance() {
        return DispatcherPowerpcPtr(new DispatcherPowerpc);
    }

    /** Constructor. */
    static DispatcherPowerpcPtr instance(size_t addrWidth, const RegisterDictionary *regs = NULL) {
        return DispatcherPowerpcPtr(new DispatcherPowerpc(addrWidth, regs));
    }
            
    /** Constructor. */
    static DispatcherPowerpcPtr instance(const BaseSemantics::RiscOperatorsPtr &ops, size_t addrWidth,
                                         const RegisterDictionary *regs=NULL) {
        return DispatcherPowerpcPtr(new DispatcherPowerpc(ops, addrWidth, regs));
    }

    /** Virtual constructor. */
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr &ops, size_t addrWidth=0,
                                                const RegisterDictionary *regs=NULL) const ROSE_OVERRIDE {
        if (0==addrWidth)
            addrWidth = addressWidth();
        if (!regs)
            regs = get_register_dictionary();
        return instance(ops, addrWidth, regs);
    }

    /** Dynamic cast to a DispatcherPowerpcPtr with assertion. */
    static DispatcherPowerpcPtr promote(const BaseSemantics::DispatcherPtr &d) {
        DispatcherPowerpcPtr retval = boost::dynamic_pointer_cast<DispatcherPowerpc>(d);
        assert(retval!=NULL);
        return retval;
    }

    virtual void set_register_dictionary(const RegisterDictionary *regdict) ROSE_OVERRIDE;

    virtual RegisterDescriptor instructionPointerRegister() const ROSE_OVERRIDE;
    virtual RegisterDescriptor stackPointerRegister() const ROSE_OVERRIDE;
    virtual RegisterDescriptor callReturnRegister() const ROSE_OVERRIDE;

    virtual int iproc_key(SgAsmInstruction *insn_) const ROSE_OVERRIDE {
        SgAsmPowerpcInstruction *insn = isSgAsmPowerpcInstruction(insn_);
        assert(insn!=NULL);
        return insn->get_kind();
    }

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
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherPowerpc);
#endif

#endif
#endif
