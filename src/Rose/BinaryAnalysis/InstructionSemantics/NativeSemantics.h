#ifndef ROSE_BinaryAnalysis_InstructionSemantics_NativeSemantics_H
#define ROSE_BinaryAnalysis_InstructionSemantics_NativeSemantics_H
#include <featureTests.h>
#if defined(ROSE_ENABLE_BINARY_ANALYSIS) && defined(ROSE_ENABLE_DEBUGGER_LINUX)

#include <Rose/BinaryAnalysis/BasicTypes.h>
#include <Rose/BinaryAnalysis/Debugger/Linux.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/ConcreteSemantics.h>

#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {

/** Domain related to an actual running process. */
namespace NativeSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Value type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Concrete values from the specimen. */
typedef ConcreteSemantics::SValue SValue;

/** Shared-ownership pointer to @ref ConcreteSemantics::SValue. */
typedef ConcreteSemantics::SValuePtr SValuePtr;

/** Formatter for printing values. */
typedef ConcreteSemantics::Formatter Formatter;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer. */
typedef boost::shared_ptr<class RegisterState> RegisterStatePtr;

/** Collection of registers.
 *
 *  Since the running specimen is the machine state, RegisterState objects in this semantic domain are not copyable. */
class RegisterState: public BaseSemantics::RegisterState, boost::noncopyable {
public:
    /** Base type. */
    using Super = BaseSemantics::RegisterState;

    /** Shared-ownership pointer. */
    using Ptr = RegisterStatePtr;

private:
    Debugger::Linux::Ptr process_;

    //----------------------------------------
    // Real constructors
    //----------------------------------------
protected:
    RegisterState();

    RegisterState(const BaseSemantics::SValuePtr &protoval, const Debugger::Linux::Ptr &process);

    //----------------------------------------
    // Static allocating constructors
    //----------------------------------------
public:
    ~RegisterState();

    /** Construct a state not attached to any subordinate process. */
    static RegisterStatePtr instance();

    /** Construct a state attached to the specified process. */
    static RegisterStatePtr instance(const BaseSemantics::SValuePtr &protoval, const Debugger::Linux::Ptr &process);

    //----------------------------------------
    // Virtual constructors
    //----------------------------------------
public:
    virtual BaseSemantics::RegisterStatePtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const RegisterDictionaryPtr&) const override;

    virtual BaseSemantics::AddressSpacePtr clone() const override;

    //----------------------------------------
    // Dynamic pointer casts
    //----------------------------------------
public:
    static RegisterStatePtr promote(const BaseSemantics::AddressSpacePtr&);

    //----------------------------------------
    // Additional properties
    //----------------------------------------
public:
    /** Property: Subordinate process storing the registers. */
    Debugger::Linux::Ptr process() const;

    //----------------------------------------
    // Virtual function implementations
    //----------------------------------------
public:
    virtual void clear() override {}

    virtual void zero() override {
        TODO("[Robb Matzke 2019-09-05]");               // set all registers to zero
    }

    virtual bool merge(const BaseSemantics::AddressSpacePtr&, BaseSemantics::RiscOperators*,
                       BaseSemantics::RiscOperators*) override {
        ASSERT_not_implemented("[Robb Matzke 2019-09-05]");
    }

    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt,
                                                  BaseSemantics::RiscOperators *ops) override {
        return peekRegister(reg, dflt, ops);
    }
    
    virtual BaseSemantics::SValuePtr peekRegister(RegisterDescriptor, const BaseSemantics::SValuePtr &dflt,
                                                  BaseSemantics::RiscOperators*) override;

    virtual void writeRegister(RegisterDescriptor, const BaseSemantics::SValuePtr &value,
                               BaseSemantics::RiscOperators*) override;

    virtual void updateReadProperties(RegisterDescriptor) override {}
    virtual void updateWriteProperties(RegisterDescriptor, BaseSemantics::InputOutputProperty) override {}

    virtual void hash(Combinatorics::Hasher&, BaseSemantics::RiscOperators*, BaseSemantics::RiscOperators*) const override;

    virtual void print(std::ostream&, Formatter&) const override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer. */
typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

/** Collection of memory address/value pairs.
 *
 *  Since the running specimen is the machine state, MemoryState objects in this semantic domain are not copyable. */
class MemoryState: public BaseSemantics::MemoryState, boost::noncopyable {
public:
    /** Base type. */
    using Super = BaseSemantics::MemoryState;

    /** Shared-ownership pointer. */
    using Ptr = MemoryStatePtr;

private:
    Debugger::Linux::Ptr process_;

    //----------------------------------------
    // Real constructors
    //----------------------------------------
protected:
    MemoryState();

    MemoryState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval,
                const Debugger::Linux::Ptr &process);

    //----------------------------------------
    // Static allocating constructors
    //----------------------------------------
public:
    ~MemoryState();

    /** Construct a state not attached to any subordinate process. */
    static MemoryStatePtr instance();

    /** Construct a state attached to the specified process. */
    static MemoryStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval,
                                   const Debugger::Linux::Ptr &process);

    //----------------------------------------
    // Virtual constructors
    //----------------------------------------
public:
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const override;

    virtual BaseSemantics::AddressSpacePtr clone() const override;

    //----------------------------------------
    // Dynamic pointer casts
    //----------------------------------------
public:
    static MemoryStatePtr promote(const BaseSemantics::AddressSpacePtr&);

    //----------------------------------------
    // Additional properties
    //----------------------------------------
public:
    /** Property: Subordinate process storing the registers. */
    Debugger::Linux::Ptr process() const;

    //----------------------------------------
    // Virtual function implementations
    //----------------------------------------
public:
    virtual void clear() override {}

    virtual bool merge(const BaseSemantics::AddressSpacePtr &/*other*/, BaseSemantics::RiscOperators */*addrOps*/,
                       BaseSemantics::RiscOperators */*valOps*/) override {
        ASSERT_not_implemented("not applicable for this class");
    }

    virtual BaseSemantics::SValuePtr readMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) override {
        return peekMemory(address, dflt, addrOps, valOps);
    }

    virtual BaseSemantics::SValuePtr peekMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) override;

    virtual void writeMemory(const BaseSemantics::SValuePtr &/*addr*/, const BaseSemantics::SValuePtr &/*value*/,
                             BaseSemantics::RiscOperators */*addrOps*/, BaseSemantics::RiscOperators */*valOps*/) override {
        ASSERT_not_implemented("[Robb Matzke 2019-09-05]");
    }

    virtual void hash(Combinatorics::Hasher&, BaseSemantics::RiscOperators */*addrOps*/,
                      BaseSemantics::RiscOperators */*valOps*/) const override {
        ASSERT_not_implemented("[Robb Matzke 2021-03-26]");
    }

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const override {
        ASSERT_not_implemented("[Robb Matzke 2019-09-05]");
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete semantic state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer. */
typedef boost::shared_ptr<class State> StatePtr;

/** Machine state.
 *
 *  Since the subordinate process contains all the state and there's no way to copy a process, state objects are not
 *  copyable. */
class State: public ConcreteSemantics::State, boost::noncopyable {
public:
    /** Base type. */
    using Super = ConcreteSemantics::State;

    /** Shared-ownership pointer. */
    using Ptr = StatePtr;

    //----------------------------------------
    // Real constructors
    //----------------------------------------
protected:
    State(const BaseSemantics::RegisterStatePtr &registers, const BaseSemantics::MemoryStatePtr &memory)
        : ConcreteSemantics::State(registers, memory) {
        (void) RegisterState::promote(registers);
        (void) MemoryState::promote(memory);
    }

    //----------------------------------------
    // Static allocating constructors
    //----------------------------------------
public:
    static StatePtr instance(const BaseSemantics::RegisterStatePtr &registers, const BaseSemantics::MemoryStatePtr &memory) {
        return StatePtr(new State(registers, memory));
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

class RiscOperators: public ConcreteSemantics::RiscOperators {
public:
    /** Base type. */
    using Super = ConcreteSemantics::RiscOperators;

    /** Shared-ownership pointer. */
    using Ptr = RiscOperatorsPtr;

    //----------------------------------------
    // Real constructors
    //----------------------------------------
protected:
    explicit RiscOperators(const BaseSemantics::StatePtr&);

    //----------------------------------------
    // Static allocating constructors
    //----------------------------------------
public:
    ~RiscOperators();

    /** Allocating constructor.
     *
     *  The register state, memory state, and combined state are instantiations of @ref NativeSemantics::RegisterState, @ref
     *  NativeSemantics::MemoryState, and @ref NativeSemantics::State, which point to the subordinate process and which are not
     *  copyable. */
    static RiscOperatorsPtr instanceFromProtoval(const BaseSemantics::SValuePtr &protoval, const Debugger::Linux::Ptr &process);

    /** Allocating constructor.
     *
     *  The @p state (registers and memory) for this object is provided by the caller and must be an instance of the @ref
     *  NativeSemantics::State that points to @ref NativeSemantics::RegisterState and @ref NativeSemantics::MemoryState.
     *  User-defined subclasses can also be used. */
    static RiscOperatorsPtr instanceFromState(const BaseSemantics::StatePtr &state);

    //----------------------------------------
    // Virtual constructors
    //----------------------------------------
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    //----------------------------------------
    // Dynamic pointer casts
    //----------------------------------------
public:
    /** Run-time promotion of a base object to a @ref NativeSemantics::RiscOperators.
     *
     *  This is a checked conversion--it will fail if @p x does not point to a @ref NativeSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr&);

    //----------------------------------------
    // Additional properties
    //----------------------------------------
public:
    /** Property: Process storing the state.
     *
     *  This is just a convenience function that queries the state for the information. */
    Debugger::Linux::Ptr process() const {
        return RegisterState::promote(currentState()->registerState())->process();
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Dispatcher
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer. */
typedef boost::shared_ptr<class Dispatcher> DispatcherPtr;

class Dispatcher: public BaseSemantics::Dispatcher {
public:
    /** Base type. */
    using Super = BaseSemantics::Dispatcher;

    /** Shared-ownership pointer. */
    using Ptr = DispatcherPtr;

private:
    Debugger::Linux::Ptr process_;

    //----------------------------------------
    // Real constructors
    //----------------------------------------
protected:
    Dispatcher(const Architecture::BaseConstPtr&, const Debugger::Linux::Ptr &process, const BaseSemantics::SValuePtr &protoval);

    Dispatcher(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

    //----------------------------------------
    // Static allocating constructors
    //----------------------------------------
public:
    ~Dispatcher();

    /** Create a new dispatcher using the specified process. */
    static DispatcherPtr instance(const Architecture::BaseConstPtr&, const Debugger::Linux::Ptr &process,
                                  const BaseSemantics::SValuePtr &protoval = SValue::instance());

    /** Create a new dispatcher using the specified executable specimen. */
    static DispatcherPtr instance(const Architecture::BaseConstPtr&, const Debugger::Linux::Specimen&,
                                  const BaseSemantics::SValuePtr &protoval = SValue::instance());

    /** Create a new dispatcher using the specified operators.
     *
     *  The operators must derive from @ref NativeSemantics::RiscOperators. */
    static DispatcherPtr instance(const Architecture::BaseConstPtr&, const BaseSemantics::RiscOperatorsPtr&);

    //----------------------------------------
    // Virtual constructors
    //----------------------------------------
public:
    virtual BaseSemantics::DispatcherPtr
    create(const BaseSemantics::RiscOperatorsPtr&) const override;

    //----------------------------------------
    // Operations
    //----------------------------------------
public:
    /** Process an instruction.
     *
     *  Since the instruction must be executed in the subordinate process, the @p insn argument serves only to supply the
     *  address of the native instruction. The instruction is executed by writing the supplied @p insn address to the
     *  subordinate's instruction pointer register and then single-stepping the subordinate to execute whatever instruction
     *  appears at that address.
     *
     * @{ */
    virtual void processInstruction(SgAsmInstruction *insn) override;
    void processInstruction(rose_addr_t va);
    /** @} */

    /** Decode the current instruction.
     *
     *  Decodes the instruction at the current instruction address. */
    virtual SgAsmInstruction* currentInstruction() const override;

    virtual RegisterDescriptor instructionPointerRegister() const override;
    virtual RegisterDescriptor stackPointerRegister() const override;
    virtual RegisterDescriptor stackFrameRegister() const override;
    virtual RegisterDescriptor callReturnRegister() const override;

    /** Disabled in this class.
     *
     * @{ */
    virtual void iprocReplace(SgAsmInstruction*, BaseSemantics::InsnProcessor*) override {
        notApplicable("iprocReplace");
    }
    virtual void iprocSet(int /*key*/, BaseSemantics::InsnProcessor*) override {
        notApplicable("iprocSet");
    }
    virtual int iprocKey(SgAsmInstruction*) const override {
        notApplicable("iprocKey");
    }
    /** @} */

private:
    [[noreturn]] void notApplicable(const std::string &what) const {
        ASSERT_not_implemented(what + " is not applicable for this class");
    }
};


} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
