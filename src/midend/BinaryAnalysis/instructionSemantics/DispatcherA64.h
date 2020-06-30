#include <featureTests.h>
#ifdef ROSE_ENABLE_ASM_A64

#include <BaseSemantics2.h>

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/split_member.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

/** Shared-ownership pointer to an A64 instruction dispatcher. See @ref heap_object_shared_ownership. */
using DispatcherA64Ptr = boost::shared_ptr<class DispatcherA64>;

class DispatcherA64: public BaseSemantics::Dispatcher {
public:
    using Super = BaseSemantics::Dispatcher;

public:
    /** Cached register.
     *
     *  This register is cached so that there are not so amny calls to @ref Dispatcher::findRegister. Changing the register
     *  dictionary via @ref set_register_dictionary updates all entries of this cache. */
    RegisterDescriptor REG_PC, REG_SP, REG_LR;

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

protected:
    // prototypical constructor
    DispatcherA64()
        : BaseSemantics::Dispatcher(64, RegisterDictionary::dictionary_a64()) {}

    DispatcherA64(const BaseSemantics::RiscOperatorsPtr &ops, const RegisterDictionary *regs)
        : BaseSemantics::Dispatcher(ops, 64, regs ? regs : RegisterDictionary::dictionary_a64()) {
        initializeRegisterDescriptors();
        initializeInsnDispatchTable();
        initializeMemory();
    }

public:
    /** Construct a prototypical dispatcher.
     *
     *  The only thing this dispatcher can be used for is to create another dispatcher with the virtual Wref create method. */
    static DispatcherA64Ptr instance() {
        return DispatcherA64Ptr(new DispatcherA64);
    }

    /** Allocating constructor. */
    static DispatcherA64Ptr instance(const BaseSemantics::RiscOperatorsPtr &ops, const RegisterDictionary *regs = nullptr) {
        return DispatcherA64Ptr(new DispatcherA64(ops, regs));
    }

    /** Virtual constructor. */
    virtual BaseSemantics::DispatcherPtr create(const BaseSemantics::RiscOperatorsPtr &ops, size_t addrWidth = 0,
                                                const RegisterDictionary *regs = nullptr) const override {
        ASSERT_require(0 == addrWidth || 64 == addrWidth);
        return instance(ops, regs);
    }

    /** Dynamic cast to DispatcherA64 with assertion. */
    static DispatcherA64Ptr promote(const BaseSemantics::DispatcherPtr &d) {
        DispatcherA64Ptr retval = boost::dynamic_pointer_cast<DispatcherA64>(d);
        ASSERT_not_null(retval);
        return retval;
    }

    /** Process any SgAsmAddPreUpdate nodes. */
    void preUpdate(SgAsmExpression*);

    /** Process any SgAsmAddPostUpdate nodes. */
    void postUpdate(SgAsmExpression*);

protected:
    /** Initialized cached register descriptors from the register dictionary. */
    void initializeRegisterDescriptors();

    /** Initializes the instruction dispatch table.
     *
     *  This is called from the constructor. */
    void initializeInsnDispatchTable();

    /** Make sure memory is configured correctly, such as setting the byte order. */
    void initializeMemory();

protected:
    int iproc_key(SgAsmInstruction*) const override;
    RegisterDescriptor instructionPointerRegister() const override;
    RegisterDescriptor stackPointerRegister() const override;
    RegisterDescriptor callReturnRegister() const override;
    void set_register_dictionary(const RegisterDictionary*) override;
};

} // namespace
} // namespace
} // namespace

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::InstructionSemantics2::DispatcherA64);
#endif

#endif
