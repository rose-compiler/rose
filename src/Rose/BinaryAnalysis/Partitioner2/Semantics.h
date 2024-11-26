#ifndef ROSE_BinaryAnalysis_Partitioner2_Semantics_H
#define ROSE_BinaryAnalysis_Partitioner2_Semantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
#include <boost/serialization/access.hpp>
#endif

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Instruction semantics for the partitioner.
 *
 *  The partitioner semantic domain is closely related to the basic @ref
 *  Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics "symbolic" domain and its classes inherit from the semantic
 *  domain. The main difference is that the partitioner's domain also takes into account the concrete memory of the
 *  specimen; e.g., the @ref MemoryMap that was initialized by loading the specimen into virtual memory. */
namespace Semantics {

/** Semantic value in the partitioner. */
using SValue = InstructionSemantics::SymbolicSemantics::SValue;

/** Reference counting pointer to semantic value. */
using SValuePtr = InstructionSemantics::SymbolicSemantics::SValuePtr;

/** Register state for the partitioner. */
using RegisterState = InstructionSemantics::BaseSemantics::RegisterStateGeneric;

/** Reference counting pointer to register state. */
using RegisterStatePtr = InstructionSemantics::BaseSemantics::RegisterStateGenericPtr;

/** Total state (registers and memory) for the partitioner. */
using State = InstructionSemantics::BaseSemantics::State;

/** Reference counting pointer to total state. */
using StatePtr = InstructionSemantics::BaseSemantics::StatePtr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Memory state.
 *
 *  Reading from an address that is either not writable (lacks MemoryMap::WRITABLE) or which is initialized (has
 *  MemoryMap::INITIALIZED) obtains the data directly from the memory map.
 *
 *  Addresses for each read operation are saved in a list which is nominally reset at the beginning of each instruction. */
template<class Super = InstructionSemantics::SymbolicSemantics::MemoryListState> // or MemoryMapState
class MemoryState: public Super {
public:
    /** Shared-ownership pointer to a @ref Semantics::MemoryState. See @ref heap_object_shared_ownership. */
    using Ptr = boost::shared_ptr<MemoryState>;

private:
    MemoryMapPtr map_;
    std::vector<SValuePtr> addressesRead_;
    bool enabled_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;
    template<class S> void serialize(S&, unsigned version);
#endif

protected:
    MemoryState()                                       // for serialization
        : enabled_(true) {}

    explicit MemoryState(const InstructionSemantics::BaseSemantics::MemoryCellPtr &protocell)
        : Super(protocell), enabled_(true) {}

    MemoryState(const InstructionSemantics::BaseSemantics::SValuePtr &addrProtoval,
                const InstructionSemantics::BaseSemantics::SValuePtr &valProtoval)
        : Super(addrProtoval, valProtoval), enabled_(true) {}

public:
    /** Instantiates a new memory state having specified prototypical cells and value. */
    static Ptr instance(const InstructionSemantics::BaseSemantics::MemoryCellPtr &protocell) {
        return Ptr(new MemoryState(protocell));
    }

    /** Instantiates a new memory state having specified prototypical value. */
    static  Ptr instance(const InstructionSemantics::BaseSemantics::SValuePtr &addrProtoval,
                         const InstructionSemantics::BaseSemantics::SValuePtr &valProtoval) {
        return Ptr(new MemoryState(addrProtoval, valProtoval));
    }

    /** Instantiates a new deep copy of an existing state. */
    static Ptr instance(const Ptr &other) {
        return Ptr(new MemoryState(*other));
    }

public:
    /** Virtual constructor. Creates a memory state having specified prototypical value. */
    virtual InstructionSemantics::BaseSemantics::MemoryStatePtr
    create(const InstructionSemantics::BaseSemantics::SValuePtr &addrProtoval,
           const InstructionSemantics::BaseSemantics::SValuePtr &valProtoval) const override {
        return instance(addrProtoval, valProtoval);
    }

    /** Virtual constructor. Creates a new memory state having specified prototypical cells and value. */
    virtual InstructionSemantics::BaseSemantics::MemoryStatePtr
    create(const InstructionSemantics::BaseSemantics::MemoryCellPtr &protocell) const override {
        return instance(protocell);
    }

    /** Virtual copy constructor. Creates a new deep copy of this memory state. */
    virtual InstructionSemantics::BaseSemantics::AddressSpacePtr
    clone() const override {
        return Ptr(new MemoryState(*this));
    }

public:
    /** Recasts a base pointer to a symbolic memory state. This is a checked cast that will fail if the specified pointer does
     *  not point to an object of our class. */
    static Ptr
    promote(const InstructionSemantics::BaseSemantics::MemoryStatePtr &x) {
        Ptr retval = as<MemoryState>(x);
        assert(x!=NULL);
        return retval;
    }

public:
    /** Property: Enabled.
     *
     *  A memory state can be disabled, in which case writes are ignored and reads always return a copy of the provided default
     *  value. Disabling a memory state is useful for certain data-flow analyses that don't need memory.
     *
     * @{ */
    bool enabled() const { return enabled_; }
    void enabled(bool b) { enabled_ = b; }
    /** @} */

    /** The memory map for the specimen.
     *
     *  If this memory map exists and contains segments that have read permission but lack write permission, then any reads
     *  from such addresses will return the concrete values read from the map.  Any writes to such addresses will cause
     *  warnings and no operation to be performed.
     *
     *  @{ */
    MemoryMapPtr memoryMap() const { return map_; }
    void memoryMap(const MemoryMapPtr &map) { map_ = map; }
    /** @} */

    /** Property: concrete virtual addresses that were read.
     *
     * @{ */
    const std::vector<SValuePtr>& addressesRead() const { return addressesRead_; }
    std::vector<SValuePtr>& addressesRead() { return addressesRead_; }
    /** @} */

public:
    virtual InstructionSemantics::BaseSemantics::SValuePtr
    readMemory(const InstructionSemantics::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics::BaseSemantics::SValuePtr &dflt,
               InstructionSemantics::BaseSemantics::RiscOperators *addrOps,
               InstructionSemantics::BaseSemantics::RiscOperators *valOps) override;

    virtual void
    writeMemory(const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                const InstructionSemantics::BaseSemantics::SValuePtr &value,
                InstructionSemantics::BaseSemantics::RiscOperators *addrOps,
                InstructionSemantics::BaseSemantics::RiscOperators *valOps) override;

    virtual InstructionSemantics::BaseSemantics::SValuePtr
    peekMemory(const InstructionSemantics::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics::BaseSemantics::SValuePtr &dflt,
               InstructionSemantics::BaseSemantics::RiscOperators *addrOps,
               InstructionSemantics::BaseSemantics::RiscOperators *valOps) override;

private:
    InstructionSemantics::BaseSemantics::SValuePtr
    readOrPeekMemory(const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                     const InstructionSemantics::BaseSemantics::SValuePtr &dflt,
                     InstructionSemantics::BaseSemantics::RiscOperators *addrOps,
                     InstructionSemantics::BaseSemantics::RiscOperators *valOps,
                     bool withSideEffects);

public:
    void print(std::ostream&, InstructionSemantics::BaseSemantics::Formatter&) const override;
};

/** Memory state using a chronological list of cells. */
using MemoryListState = MemoryState<InstructionSemantics::SymbolicSemantics::MemoryListState>;

/** Memory state indexed by hash of address expressions. */
using MemoryMapState = MemoryState<InstructionSemantics::SymbolicSemantics::MemoryMapState>;

/** Shared-ownership pointer to a @ref MemoryListState. See @ref heap_object_shared_ownership. */
using MemoryListStatePtr = boost::shared_ptr<MemoryListState>;

/** Shared-ownership pointer to a @ref MemoryMapState. See @ref heap_object_shared_ownership. */
using MemoryMapStatePtr = boost::shared_ptr<MemoryMapState>;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC Operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to the RISC operators object. See @ref heap_object_shared_ownership. */
using RiscOperatorsPtr = boost::shared_ptr<class RiscOperators>;

/** Semantic operators.
 *
 *  Most operations are delegated to the symbolic state. The return value from the symbolic state is replaced with an unknown
 *  if the expression grows beyond a certain complexity. */
class RiscOperators: public InstructionSemantics::SymbolicSemantics::RiscOperators {
public:
    using Super = InstructionSemantics::SymbolicSemantics::RiscOperators;
    using Ptr = RiscOperatorsPtr;

private:
    static const size_t TRIM_THRESHOLD_DFLT = 100;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;
    template<class S> void serialize(S&, unsigned version);
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    RiscOperators();                                    // for serialization

    RiscOperators(const InstructionSemantics::BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver);

    RiscOperators(const InstructionSemantics::BaseSemantics::StatePtr &state, const SmtSolverPtr &solver);

public:
    ~RiscOperators();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new RiscOperators object and configure it using default values.
     *
     * @{ */
    static RiscOperatorsPtr instance(const RegisterDictionaryPtr&);
    static RiscOperatorsPtr instance(const RegisterDictionaryPtr&, const SmtSolverPtr&,
                                     SemanticMemoryParadigm memoryParadigm = LIST_BASED_MEMORY);
    /** @} */

    /** Instantiate a new RiscOperators object with specified prototypical values.
     *
     * @{ */
    static RiscOperatorsPtr instance(const InstructionSemantics::BaseSemantics::SValuePtr &protoval);
    static RiscOperatorsPtr instance(const InstructionSemantics::BaseSemantics::SValuePtr &protoval, const SmtSolverPtr&);
    /** @} */

    /** Instantiate a new RiscOperators with specified state.
     *
     * @{ */
    static RiscOperatorsPtr instance(const InstructionSemantics::BaseSemantics::StatePtr&);
    static RiscOperatorsPtr instance(const InstructionSemantics::BaseSemantics::StatePtr&, const SmtSolverPtr&);
    /** @} */

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics::BaseSemantics::SValuePtr &protoval,
           const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    virtual InstructionSemantics::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics::BaseSemantics::StatePtr &state,
           const SmtSolverPtr &solver = SmtSolverPtr()) const override;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to our operators. This is a checked conversion--it
     *  will fail if @p x does not point to our object. */
    static RiscOperatorsPtr promote(const InstructionSemantics::BaseSemantics::RiscOperatorsPtr&);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override methods from base class.
public:
    virtual void startInstruction(SgAsmInstruction*) override;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Super>
InstructionSemantics::BaseSemantics::SValuePtr
MemoryState<Super>::readMemory(const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                               const InstructionSemantics::BaseSemantics::SValuePtr &dflt,
                               InstructionSemantics::BaseSemantics::RiscOperators *addrOps,
                               InstructionSemantics::BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(addr, dflt, addrOps, valOps, true/*with side effects*/);
}

template<class Super>
InstructionSemantics::BaseSemantics::SValuePtr
MemoryState<Super>::peekMemory(const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                               const InstructionSemantics::BaseSemantics::SValuePtr &dflt,
                               InstructionSemantics::BaseSemantics::RiscOperators *addrOps,
                               InstructionSemantics::BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(addr, dflt, addrOps, valOps, false/*no side effects*/);
}

template<class Super>
InstructionSemantics::BaseSemantics::SValuePtr
MemoryState<Super>::readOrPeekMemory(const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                                     const InstructionSemantics::BaseSemantics::SValuePtr &dflt,
                                     InstructionSemantics::BaseSemantics::RiscOperators *addrOps,
                                     InstructionSemantics::BaseSemantics::RiscOperators *valOps,
                                     bool withSideEffects) {
    using namespace InstructionSemantics;

    if (!enabled_)
        return dflt->copy();

    addressesRead_.push_back(SValue::promote(addr));
    if (map_ && addr->toUnsigned()) {
        ASSERT_require2(8==dflt->nBits(), "multi-byte reads should have been handled above this call");
        rose_addr_t va = addr->toUnsigned().get();
        bool isModifiable = map_->at(va).require(MemoryMap::WRITABLE).exists();
        bool isInitialized = map_->at(va).require(MemoryMap::INITIALIZED).exists();
        if (!isModifiable || isInitialized) {
            uint8_t byte;
            if (1 == map_->at(va).limit(1).read(&byte).size()) {
                SymbolicExpression::Ptr expr = SymbolicExpression::makeIntegerConstant(8, byte);
                if (isModifiable) {
                    SymbolicExpression::Ptr indet = SymbolicExpression::makeIntegerVariable(8);
                    expr = SymbolicExpression::makeSet(expr, indet, valOps->solver());
                }
                SymbolicSemantics::SValuePtr val = SymbolicSemantics::SValue::promote(valOps->undefined_(8));
                val->set_expression(expr);
                return val;
            }
        }
    }

    if (withSideEffects) {
        return Super::readMemory(addr, dflt, addrOps, valOps);
    } else {
        return Super::peekMemory(addr, dflt, addrOps, valOps);
    }
}

template<class Super>
void
MemoryState<Super>::writeMemory(const InstructionSemantics::BaseSemantics::SValuePtr &addr,
                                const InstructionSemantics::BaseSemantics::SValuePtr &value,
                                InstructionSemantics::BaseSemantics::RiscOperators *addrOps,
                                InstructionSemantics::BaseSemantics::RiscOperators *valOps) {
    if (!enabled_)
        return;
    Super::writeMemory(addr, value, addrOps, valOps);
}

template<class Super>
void
MemoryState<Super>::print(std::ostream &out, InstructionSemantics::BaseSemantics::Formatter &fmt) const {
    if (map_) {
        map_->dump(out, fmt.get_line_prefix());
    } else {
        out <<fmt.get_line_prefix() <<"no memory map\n";
    }

    Super::print(out, fmt);
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
