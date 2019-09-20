#ifndef Rose_BinaryAnalysis_NativeSemantics_H
#define Rose_BinaryAnalysis_NativeSemantics_H

#include <BinaryDebugger.h>
#include <ConcreteSemantics2.h>

#include <boost/noncopyable.hpp>
#include <boost/filesystem.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {

/** Domain related to an actual running process. */
namespace NativeSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Value type
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Concrete values from the specimen. */
typedef ConcreteSemantics::SValue SValue;

/** Shared-ownership pointer to @ref SValue. See @ref heap_object_shared_ownership. */
typedef ConcreteSemantics::SValuePtr SValuePtr;

/** Formatter for printing values. */
typedef ConcreteSemantics::Formatter Formatter;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Register state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to @ref RegisterState. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class RegisterState> RegisterStatePtr;

/** Collection of registers.
 *
 *  Since the running specimen is the machine state, @ref RegisterState objects in this semantic domain are not copyable. */
class RegisterState: public BaseSemantics::RegisterState, boost::noncopyable {
    Debugger::Ptr process_;

    //----------------------------------------
    // Real constructors
    //----------------------------------------
protected:
    RegisterState() {}

    RegisterState(const BaseSemantics::SValuePtr &protoval, const Debugger::Ptr &process)
        : BaseSemantics::RegisterState(protoval, process->registerDictionary()), process_(process) {
        ASSERT_not_null(process);
    }
    
    //----------------------------------------
    // Static allocating constructors
    //----------------------------------------
public:
    /** Construct a state not attached to any subordinate process. */
    static RegisterStatePtr instance() {
        return RegisterStatePtr(new RegisterState);
    }

    /** Construct a state attached to the specified process. */
    static RegisterStatePtr instance(const BaseSemantics::SValuePtr &protoval, const Debugger::Ptr &process) {
        ASSERT_not_null(protoval);
        (void) SValue::promote(protoval);
        return RegisterStatePtr(new RegisterState(protoval, process));
    }
    
    //----------------------------------------
    // Virtual constructors
    //----------------------------------------
public:
    virtual BaseSemantics::RegisterStatePtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const RegisterDictionary *regdict) const ROSE_OVERRIDE {
        ASSERT_not_implemented("not applicable for this class");
    }

    virtual BaseSemantics::RegisterStatePtr clone() const ROSE_OVERRIDE {
        ASSERT_not_implemented("not applicable for this class");
    }
    
    //----------------------------------------
    // Dynamic pointer casts
    //----------------------------------------
public:
    static RegisterStatePtr promote(const BaseSemantics::RegisterStatePtr &x) {
        RegisterStatePtr retval = boost::dynamic_pointer_cast<RegisterState>(x);
        ASSERT_not_null(retval);
        return retval;
    }

    //----------------------------------------
    // Virtual function implementations
    //----------------------------------------

public:
    virtual void clear() ROSE_OVERRIDE {}

    virtual void zero() ROSE_OVERRIDE {
        TODO("[Robb Matzke 2019-09-05]");               // set all registers to zero
    }

    virtual bool merge(const BaseSemantics::RegisterStatePtr &other, BaseSemantics::RiscOperators *ops) ROSE_OVERRIDE {
        ASSERT_not_implemented("[Robb Matzke 2019-09-05]");
    }

    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt,
                                                  BaseSemantics::RiscOperators *ops) ROSE_OVERRIDE {
        return peekRegister(reg, dflt, ops);
    }
    
    virtual BaseSemantics::SValuePtr peekRegister(RegisterDescriptor, const BaseSemantics::SValuePtr &dflt,
                                                  BaseSemantics::RiscOperators*) ROSE_OVERRIDE;

    virtual void writeRegister(RegisterDescriptor, const BaseSemantics::SValuePtr &value,
                               BaseSemantics::RiscOperators*) ROSE_OVERRIDE;

    virtual void print(std::ostream&, Formatter&) const ROSE_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to @ref MemoryState. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class MemoryState> MemoryStatePtr;

/** Collection of memory address/value pairs.
 *
 *  Since the running specimen is the machine state, @ref MemoryState objects in this semantic domain are not copyable. */
class MemoryState: public BaseSemantics::MemoryState, boost::noncopyable {
    Debugger::Ptr process_;

    //----------------------------------------
    // Real constructors
    //----------------------------------------
protected:
    MemoryState() {}

    MemoryState(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval,
                const Debugger::Ptr &process)
        : BaseSemantics::MemoryState(addrProtoval, valProtoval), process_(process) {
        ASSERT_not_null(process);
    }

    //----------------------------------------
    // Static allocating constructors
    //----------------------------------------
public:
    /** Construct a state not attached to any subordinate process. */
    static MemoryStatePtr instance() {
        return MemoryStatePtr(new MemoryState);
    }

    /** Construct a state attached to the specified process. */
    static MemoryStatePtr instance(const BaseSemantics::SValuePtr &addrProtoval, const BaseSemantics::SValuePtr &valProtoval,
                                   const Debugger::Ptr &process) {
        return MemoryStatePtr(new MemoryState(addrProtoval, valProtoval, process));
    }

    //----------------------------------------
    // Virtual constructors
    //----------------------------------------
public:
    virtual BaseSemantics::MemoryStatePtr create(const BaseSemantics::SValuePtr &addrProtoval,
                                                 const BaseSemantics::SValuePtr &valProtoval) const ROSE_OVERRIDE {
        ASSERT_not_implemented("not applicable for this class");
    }

    virtual BaseSemantics::MemoryStatePtr clone() const ROSE_OVERRIDE {
        ASSERT_not_implemented("not applicable for this class");
    }

    //----------------------------------------
    // Dynamic pointer casts
    //----------------------------------------
public:
    static MemoryStatePtr promote(const BaseSemantics::MemoryStatePtr &x) {
        MemoryStatePtr retval = boost::dynamic_pointer_cast<MemoryState>(x);
        ASSERT_not_null(retval);
        return retval;
    }

    //----------------------------------------
    // Virtual function implementations
    //----------------------------------------
public:
    virtual void clear() ROSE_OVERRIDE {}

    virtual bool merge(const BaseSemantics::MemoryStatePtr &other, BaseSemantics::RiscOperators *addrOps,
                       BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE {
        ASSERT_not_implemented("not applicable for this class");
    }

    virtual BaseSemantics::SValuePtr readMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE {
        return peekMemory(address, dflt, addrOps, valOps);
    }

    virtual BaseSemantics::SValuePtr peekMemory(const BaseSemantics::SValuePtr &address, const BaseSemantics::SValuePtr &dflt,
                                                BaseSemantics::RiscOperators *addrOps,
                                                BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE;

    virtual void writeMemory(const BaseSemantics::SValuePtr &addr, const BaseSemantics::SValuePtr &value,
                             BaseSemantics::RiscOperators *addrOps, BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE {
        ASSERT_not_implemented("[Robb Matzke 2019-09-05]");
    }

    virtual void print(std::ostream&, BaseSemantics::Formatter&) const ROSE_OVERRIDE {
        ASSERT_not_implemented("[Robb Matzke 2019-09-05]");
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Complete semantic state
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to @ref State. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class State> StatePtr;

/** Machine state.
 *
 *  Since the subordinate process contains all the state and there's no way to copy a process, state objects are not
 *  copyable. */
class State: public ConcreteSemantics::State, boost::noncopyable {
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

/** Shared-ownership pointer to @ref RiscOperators. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

class RiscOperators: public ConcreteSemantics::RiscOperators {
    //----------------------------------------
    // Real constructors
    //----------------------------------------
protected:
    explicit RiscOperators(const BaseSemantics::StatePtr &state)
        : ConcreteSemantics::RiscOperators(state, SmtSolverPtr()) {
        name("Native");
    }

    //----------------------------------------
    // Static allocating constructors
    //----------------------------------------
public:
    /** Instantiate a new @ref RiscOperators object.
     *
     *  The @ref State (registers and memory) for this object is the subordinate process. */
    static RiscOperatorsPtr instance(const BaseSemantics::SValuePtr &protoval, const Debugger::Ptr &process) {
        RegisterStatePtr registers = RegisterState::instance(protoval, process);
        MemoryStatePtr memory = MemoryState::instance(protoval, protoval, process);
        StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state));
    }

    //----------------------------------------
    // Virtual constructors
    //----------------------------------------
public:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        TODO("[Robb Matzke 2019-09-05]");
    }

    //----------------------------------------
    // Dynamic pointer casts
    //----------------------------------------
public:
    /** Run-time promotion of a base object to a @ref NativeSemantics @ref RiscOperators. This is a checked conversion--it
     *  will fail if @p x does not point to a @ref NativeSemantics::RiscOperators object. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Dispatcher
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to @ref Dispatcher. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class Dispatcher> DispatcherPtr;

class Dispatcher: public BaseSemantics::Dispatcher {
    Debugger::Ptr process_;

    //----------------------------------------
    // Real constructors
    //----------------------------------------
protected:
    Dispatcher(const boost::filesystem::path &program, const std::vector<std::string> &args)
        : process_(Debugger::instance(Debugger::Specimen(program, args))) {
        set_register_dictionary(process_->registerDictionary());
        addressWidth(process_->kernelWordSize());
        operators = RiscOperators::instance(SValue::instance(), process_);
    }
    
    //----------------------------------------
    // Static allocating constructors
    //----------------------------------------
public:
    static DispatcherPtr instance(const boost::filesystem::path &exeName, const std::vector<std::string> &args) {
        return DispatcherPtr(new Dispatcher(exeName, args));
    }
    
    //----------------------------------------
    // Virtual constructors
    //----------------------------------------
public:
    virtual BaseSemantics::DispatcherPtr
    create(const BaseSemantics::RiscOperatorsPtr &ops, size_t addrWidth=0,
           const RegisterDictionary *regs=NULL) const ROSE_OVERRIDE {
        notApplicable("create");
    }
    
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
    virtual void processInstruction(SgAsmInstruction *insn) ROSE_OVERRIDE;
    void processInstruction(rose_addr_t va);
    /** @} */

    /** Decode the current instruction.
     *
     *  Decodes the instruction at the current instruction address. */
    virtual SgAsmInstruction* currentInstruction() const ROSE_OVERRIDE;

    virtual RegisterDescriptor instructionPointerRegister() const ROSE_OVERRIDE;
    virtual RegisterDescriptor stackPointerRegister() const ROSE_OVERRIDE;
    virtual RegisterDescriptor callReturnRegister() const ROSE_OVERRIDE;

    /** Disabled in this class.
     *
     * @{ */
    virtual void iproc_replace(SgAsmInstruction*, BaseSemantics::InsnProcessor*) ROSE_OVERRIDE {
        notApplicable("iproc_replace");
    }
    virtual void iproc_set(int key, BaseSemantics::InsnProcessor*) ROSE_OVERRIDE {
        notApplicable("iproc_set");
    }
    virtual int iproc_key(SgAsmInstruction*) const ROSE_OVERRIDE {
        notApplicable("iproc_key");
    }
    /** @} */

private:
    void notApplicable(const std::string &what) const SAWYER_ATTR_NORETURN {
        ASSERT_not_implemented(what + " is not applicable for this class");
    }
};


} // namespace
} // namespace
} // namespace
} // namespace

#endif
