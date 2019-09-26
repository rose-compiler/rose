#ifndef ROSE_BinaryAnalysis_Partitioner_Semantics_H
#define ROSE_BinaryAnalysis_Partitioner_Semantics_H

#include <Partitioner2/BasicTypes.h>
#include "SymbolicSemantics2.h"

#include <boost/serialization/access.hpp>
#include <boost/serialization/base_object.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>

namespace Rose {
namespace BinaryAnalysis {
namespace Partitioner2 {

/** Instruction semantics for the partitioner.
 *
 *  The partitioner semantic domain is closely related to the basic @ref
 *  Rose::BinaryAnalysis::InstructionSemantics2::SymbolicSemantics "symbolic" domain and its classes inherit from the semantic
 *  domain. The main difference is that the partitioner's domain also takes into account the concrete memory of the
 *  specimen; e.g., the @ref MemoryMap that was initialized by loading the specimen into virtual memory. */
namespace Semantics {

/** Semantic value in the partitioner. */
typedef InstructionSemantics2::SymbolicSemantics::SValue SValue;

/** Reference counting pointer to semantic value. */
typedef InstructionSemantics2::SymbolicSemantics::SValuePtr SValuePtr;

/** Register state for the partitioner. */
typedef InstructionSemantics2::BaseSemantics::RegisterStateGeneric RegisterState;

/** Reference counting pointer to register state. */
typedef InstructionSemantics2::BaseSemantics::RegisterStateGenericPtr RegisterStatePtr;

/** Total state (registers and memory) for the partitioner. */
typedef InstructionSemantics2::BaseSemantics::State State;

/** Reference counting pointer to total state. */
typedef InstructionSemantics2::BaseSemantics::StatePtr StatePtr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Memory state.
 *
 *  Reading from an address that is either not writable (lacks MemoryMap::WRITABLE) or which is initialized (has
 *  MemoryMap::INITIALIZED) obtains the data directly from the memory map.
 *
 *  Addresses for each read operation are saved in a list which is nominally reset at the beginning of each instruction. */
template<class Super = InstructionSemantics2::SymbolicSemantics::MemoryListState> // or MemoryMapState
class MemoryState: public Super {
public:
    /** Shared-ownership pointer to a @ref MemoryState. See @ref heap_object_shared_ownership. */
    typedef boost::shared_ptr<MemoryState> Ptr;

private:
    MemoryMap::Ptr map_;
    std::vector<SValuePtr> addressesRead_;
    bool enabled_;

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
        s & BOOST_SERIALIZATION_NVP(map_);
        s & BOOST_SERIALIZATION_NVP(addressesRead_);
        s & BOOST_SERIALIZATION_NVP(enabled_);
    }
#endif

protected:
    MemoryState()                                       // for serialization
        : enabled_(true) {}

    explicit MemoryState(const InstructionSemantics2::BaseSemantics::MemoryCellPtr &protocell)
        : Super(protocell), enabled_(true) {}

    MemoryState(const InstructionSemantics2::BaseSemantics::SValuePtr &addrProtoval,
                const InstructionSemantics2::BaseSemantics::SValuePtr &valProtoval)
        : Super(addrProtoval, valProtoval), enabled_(true) {}

public:
    /** Instantiates a new memory state having specified prototypical cells and value. */
    static Ptr instance(const InstructionSemantics2::BaseSemantics::MemoryCellPtr &protocell) {
        return Ptr(new MemoryState(protocell));
    }

    /** Instantiates a new memory state having specified prototypical value. */
    static  Ptr instance(const InstructionSemantics2::BaseSemantics::SValuePtr &addrProtoval,
                         const InstructionSemantics2::BaseSemantics::SValuePtr &valProtoval) {
        return Ptr(new MemoryState(addrProtoval, valProtoval));
    }

    /** Instantiates a new deep copy of an existing state. */
    static Ptr instance(const Ptr &other) {
        return Ptr(new MemoryState(*other));
    }

public:
    /** Virtual constructor. Creates a memory state having specified prototypical value. */
    virtual InstructionSemantics2::BaseSemantics::MemoryStatePtr
    create(const InstructionSemantics2::BaseSemantics::SValuePtr &addrProtoval,
           const InstructionSemantics2::BaseSemantics::SValuePtr &valProtoval) const ROSE_OVERRIDE {
        return instance(addrProtoval, valProtoval);
    }

    /** Virtual constructor. Creates a new memory state having specified prototypical cells and value. */
    virtual InstructionSemantics2::BaseSemantics::MemoryStatePtr
    create(const InstructionSemantics2::BaseSemantics::MemoryCellPtr &protocell) const ROSE_OVERRIDE {
        return instance(protocell);
    }

    /** Virtual copy constructor. Creates a new deep copy of this memory state. */
    virtual InstructionSemantics2::BaseSemantics::MemoryStatePtr
    clone() const ROSE_OVERRIDE {
        return Ptr(new MemoryState(*this));
    }

public:
    /** Recasts a base pointer to a symbolic memory state. This is a checked cast that will fail if the specified pointer does
     *  not point to an object of our class. */
    static Ptr
    promote(const InstructionSemantics2::BaseSemantics::MemoryStatePtr &x) {
        Ptr retval = boost::dynamic_pointer_cast<MemoryState>(x);
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
    MemoryMap::Ptr memoryMap() const { return map_; }
    void memoryMap(const MemoryMap::Ptr &map) { map_ = map; }
    /** @} */

    /** Property: concrete virtual addresses that were read.
     *
     * @{ */
    const std::vector<SValuePtr>& addressesRead() const { return addressesRead_; }
    std::vector<SValuePtr>& addressesRead() { return addressesRead_; }
    /** @} */

public:
    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    readMemory(const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
               InstructionSemantics2::BaseSemantics::RiscOperators *addrOps,
               InstructionSemantics2::BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE;

    virtual void
    writeMemory(const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                const InstructionSemantics2::BaseSemantics::SValuePtr &value,
                InstructionSemantics2::BaseSemantics::RiscOperators *addrOps,
                InstructionSemantics2::BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE;

    virtual InstructionSemantics2::BaseSemantics::SValuePtr
    peekMemory(const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
               const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
               InstructionSemantics2::BaseSemantics::RiscOperators *addrOps,
               InstructionSemantics2::BaseSemantics::RiscOperators *valOps) ROSE_OVERRIDE;

private:
    InstructionSemantics2::BaseSemantics::SValuePtr
    readOrPeekMemory(const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                     const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
                     InstructionSemantics2::BaseSemantics::RiscOperators *addrOps,
                     InstructionSemantics2::BaseSemantics::RiscOperators *valOps,
                     bool withSideEffects);

public:
    void print(std::ostream&, InstructionSemantics2::BaseSemantics::Formatter&) const ROSE_OVERRIDE;
};

/** Memory state using a chronological list of cells. */
typedef MemoryState<InstructionSemantics2::SymbolicSemantics::MemoryListState> MemoryListState;

/** Memory state indexed by hash of address expressions. */
typedef MemoryState<InstructionSemantics2::SymbolicSemantics::MemoryMapState> MemoryMapState;

/** Shared-ownership pointer to a @ref MemoryListState. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<MemoryListState> MemoryListStatePtr;

/** Shared-ownership pointer to a @ref MemoryMapState. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<MemoryMapState> MemoryMapStatePtr;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      RISC Operators
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/** Shared-ownership pointer to the RISC operators object. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Semantic operators.
 *
 *  Most operations are delegated to the symbolic state. The return value from the symbolic state is replaced with an unknown
 *  if the expression grows beyond a certain complexity. */
class RiscOperators: public InstructionSemantics2::SymbolicSemantics::RiscOperators {
public:
    typedef InstructionSemantics2::SymbolicSemantics::RiscOperators Super;

private:
    static const size_t TRIM_THRESHOLD_DFLT = 100;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Serialization
#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
private:
    friend class boost::serialization::access;

    template<class S>
    void serialize(S &s, const unsigned /*version*/) {
        s & BOOST_SERIALIZATION_BASE_OBJECT_NVP(Super);
    }
#endif

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Real constructors
protected:
    RiscOperators() {}                                  // for serialization

    explicit RiscOperators(const InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
                           const SmtSolverPtr &solver = SmtSolverPtr())
        : InstructionSemantics2::SymbolicSemantics::RiscOperators(protoval, solver) {
        name("PartitionerSemantics");
        (void)SValue::promote(protoval);                // make sure its dynamic type is appropriate
        trimThreshold(TRIM_THRESHOLD_DFLT);
    }

    explicit RiscOperators(const InstructionSemantics2::BaseSemantics::StatePtr &state,
                           const SmtSolverPtr &solver = SmtSolverPtr())
        : InstructionSemantics2::SymbolicSemantics::RiscOperators(state, solver) {
        name("PartitionerSemantics");
        (void)SValue::promote(state->protoval());
        trimThreshold(TRIM_THRESHOLD_DFLT);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Static allocating constructors
public:
    /** Instantiate a new RiscOperators object and configure it using default values. */
    static RiscOperatorsPtr instance(const RegisterDictionary *regdict, const SmtSolverPtr &solver = SmtSolverPtr(),
                                     SemanticMemoryParadigm memoryParadigm = LIST_BASED_MEMORY) {
        InstructionSemantics2::BaseSemantics::SValuePtr protoval = SValue::instance();
        InstructionSemantics2::BaseSemantics::RegisterStatePtr registers = RegisterState::instance(protoval, regdict);
        InstructionSemantics2::BaseSemantics::MemoryStatePtr memory;
        switch (memoryParadigm) {
            case LIST_BASED_MEMORY:
                memory = MemoryListState::instance(protoval, protoval);
                break;
            case MAP_BASED_MEMORY:
                memory = MemoryMapState::instance(protoval, protoval);
                break;
        }
        InstructionSemantics2::BaseSemantics::StatePtr state = State::instance(registers, memory);
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    /** Instantiate a new RiscOperators object with specified prototypical values. */
    static RiscOperatorsPtr
    instance(const InstructionSemantics2::BaseSemantics::SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(protoval, solver));
    }

    /** Instantiate a new RiscOperators with specified state. */
    static RiscOperatorsPtr
    instance(const InstructionSemantics2::BaseSemantics::StatePtr &state, const SmtSolverPtr &solver = SmtSolverPtr()) {
        return RiscOperatorsPtr(new RiscOperators(state, solver));
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Virtual constructors
public:
    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::SValuePtr &protoval,
           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(protoval, solver);
    }

    virtual InstructionSemantics2::BaseSemantics::RiscOperatorsPtr
    create(const InstructionSemantics2::BaseSemantics::StatePtr &state,
           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return instance(state, solver);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Dynamic pointer casts
public:
    /** Run-time promotion of a base RiscOperators pointer to our operators. This is a checked conversion--it
     *  will fail if @p x does not point to our object. */
    static RiscOperatorsPtr
    promote(const InstructionSemantics2::BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        assert(retval!=NULL);
        return retval;
    }
    
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Override methods from base class.
public:
    virtual void startInstruction(SgAsmInstruction*) ROSE_OVERRIDE;
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                      Memory State
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Super>
InstructionSemantics2::BaseSemantics::SValuePtr
MemoryState<Super>::readMemory(const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                               const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
                               InstructionSemantics2::BaseSemantics::RiscOperators *addrOps,
                               InstructionSemantics2::BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(addr, dflt, addrOps, valOps, true/*with side effects*/);
}

template<class Super>
InstructionSemantics2::BaseSemantics::SValuePtr
MemoryState<Super>::peekMemory(const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                               const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
                               InstructionSemantics2::BaseSemantics::RiscOperators *addrOps,
                               InstructionSemantics2::BaseSemantics::RiscOperators *valOps) {
    return readOrPeekMemory(addr, dflt, addrOps, valOps, false/*no side effects*/);
}

template<class Super>
InstructionSemantics2::BaseSemantics::SValuePtr
MemoryState<Super>::readOrPeekMemory(const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                                     const InstructionSemantics2::BaseSemantics::SValuePtr &dflt,
                                     InstructionSemantics2::BaseSemantics::RiscOperators *addrOps,
                                     InstructionSemantics2::BaseSemantics::RiscOperators *valOps,
                                     bool withSideEffects) {
    using namespace InstructionSemantics2;

    if (!enabled_)
        return dflt->copy();

    addressesRead_.push_back(SValue::promote(addr));
    if (map_ && addr->is_number()) {
        ASSERT_require2(8==dflt->get_width(), "multi-byte reads should have been handled above this call");
        rose_addr_t va = addr->get_number();
        bool isModifiable = map_->at(va).require(MemoryMap::WRITABLE).exists();
        bool isInitialized = map_->at(va).require(MemoryMap::INITIALIZED).exists();
        if (!isModifiable || isInitialized) {
            uint8_t byte;
            if (1 == map_->at(va).limit(1).read(&byte).size()) {
                SymbolicExpr::Ptr expr = SymbolicExpr::makeInteger(8, byte);
                if (isModifiable) {
                    SymbolicExpr::Ptr indet = SymbolicExpr::makeVariable(8);
                    expr = SymbolicExpr::makeSet(expr, indet, valOps->solver());
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
MemoryState<Super>::writeMemory(const InstructionSemantics2::BaseSemantics::SValuePtr &addr,
                                const InstructionSemantics2::BaseSemantics::SValuePtr &value,
                                InstructionSemantics2::BaseSemantics::RiscOperators *addrOps,
                                InstructionSemantics2::BaseSemantics::RiscOperators *valOps) {
    if (!enabled_)
        return;
    Super::writeMemory(addr, value, addrOps, valOps);
}

template<class Super>
void
MemoryState<Super>::print(std::ostream &out, InstructionSemantics2::BaseSemantics::Formatter &fmt) const {
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

#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::Partitioner2::Semantics::MemoryListState);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::Partitioner2::Semantics::MemoryMapState);
BOOST_CLASS_EXPORT_KEY(Rose::BinaryAnalysis::Partitioner2::Semantics::RiscOperators);
#endif

#endif
