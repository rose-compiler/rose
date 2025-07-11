#ifndef ROSE_BinaryAnalysis_Partitioner2_Semantics_H
#define ROSE_BinaryAnalysis_Partitioner2_Semantics_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/As.h>
#include <Rose/BinaryAnalysis/MemoryMap.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicTypes.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryCellMap.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
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

#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
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
#ifdef ROSE_ENABLE_BOOST_SERIALIZATION
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
    ASSERT_require2(8==dflt->nBits(), "multi-byte reads should have been handled above this call");

    if (!enabled_)
        return dflt->copy();

    addressesRead_.push_back(SValue::promote(addr));

    // Symbolic memory takes precedence over concrete memory when the address is concrete. I.e., reading from concrete address
    // 0x1234 the first time will return the value stored in concrete memory, say 0 and not make any modifications to the symbolic
    // state. If the value 1 is then written to 0x1234 it will be written to the symbolic state, not the concrete state (because
    // changing the concrete state is probably a global side effect unless the entire memory map has been copied). Subsequent reads
    // from 0x1234 should return the 1 that we wrote, not the 0 that's stored in the concrete memory.
    BaseSemantics::SValuePtr symbolicDefault = dflt;
    if (map_ && addr->toUnsigned()) {
        Address va = *addr->toUnsigned();
        bool isModifiable = map_->at(va).require(MemoryMap::WRITABLE).exists();
        bool isInitialized = map_->at(va).require(MemoryMap::INITIALIZED).exists();

        // Read the byte from concrete memory if possible.
        SymbolicExpression::Ptr valueRead;
        if (!isModifiable || isInitialized) {
            uint8_t byte;
            if (1 == map_->at(va).limit(1).read(&byte).size())
                valueRead = SymbolicExpression::makeIntegerConstant(8, byte);
        }

        // If concrete memory is mutable, then we don't really know the value for sure.
        if (isModifiable) {
            const auto indeterminate = SymbolicExpression::makeIntegerVariable(8);
            if (valueRead) {
                valueRead = SymbolicExpression::makeSet(valueRead, indeterminate, valOps->solver());
            } else {
                valueRead = indeterminate;
            }
        }

        // Make the value that we read from concrete memory the default for reading from symbolic memory.
        if (valueRead) {
            auto val = SymbolicSemantics::SValue::promote(valOps->undefined_(8));
            val->set_expression(valueRead);
            symbolicDefault = val;
        }

        // Does symbolic memory define this address?
        const bool isPresentSymbolically = [this, &addr]() {
            struct CellFinder: BaseSemantics::MemoryCell::Visitor {
                BaseSemantics::SValue::Ptr needle;
                bool found = false;

                explicit CellFinder(const BaseSemantics::SValue::Ptr &addr)
                    : needle(addr) {
                    ASSERT_not_null(addr);
                }

                void operator()(BaseSemantics::MemoryCell::Ptr &haystack) override {
                    ASSERT_not_null(haystack);
                    if (haystack->address()->mustEqual(needle))
                        found = true;
                }
            } cellFinder(addr);
            this->traverse(cellFinder);
            return cellFinder.found;
        }();

        // Perhaps there is no need to update the symbolic memory.
        if (!isPresentSymbolically && isInitialized && !isModifiable)
            return symbolicDefault;

        // But if we do have to update the symbolic memory, do so in a way that makes it look like the value was always there.
        if (!isPresentSymbolically && withSideEffects) {
            SgAsmInstruction *addrInsn = addrOps->currentInstruction(); // order is important because addrOps and valOps
            SgAsmInstruction *valInsn = valOps->currentInstruction();   // might be the same object. Read both of them
            addrOps->currentInstruction(nullptr);                       // before clearing either of them.
            valOps->currentInstruction(nullptr);
            Super::writeMemory(addr, symbolicDefault, addrOps, valOps);
            addrOps->currentInstruction(addrInsn);
            valOps->currentInstruction(valInsn);
        }
    }

    // Read from symbolic memory
    if (withSideEffects) {
        return Super::readMemory(addr, symbolicDefault, addrOps, valOps);
    } else {
        return Super::peekMemory(addr, symbolicDefault, addrOps, valOps);
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
    namespace BS = InstructionSemantics::BaseSemantics;

    out <<fmt.get_line_prefix() <<"concrete memory:\n";
    if (map_) {
        map_->dump(out, fmt.get_line_prefix() + "  ");
    } else {
        out <<fmt.get_line_prefix() <<"no memory map\n";
    }

    const bool isEmpty = as<const BS::MemoryCellState>(this) ?
                         as<const BS::MemoryCellState>(this)->allCells().empty() :
                         false;

    if (as<const BS::MemoryCellMap>(this)) {
        out <<fmt.get_line_prefix() <<"symbolic memory (map-based):\n";
    } else {
        out <<fmt.get_line_prefix() <<"symbolic memory (list-based):\n";
    }
    BS::Indent indent(fmt);
    if (isEmpty) {
        out <<fmt.get_line_prefix() <<"empty\n";
    } else {
        Super::print(out, fmt);
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
