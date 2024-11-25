// Symbolic memory -- a memory state where memory is represented by an SMT function whose domain is the address space and whose
// range are the bytes stored at those addresses.
#ifndef ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_SymbolicMemory_H
#define ROSE_BinaryAnalysis_InstructionSemantics_BaseSemantics_SymbolicMemory_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/InstructionSemantics/BaseSemantics/MemoryState.h>
#include <Rose/BinaryAnalysis/SymbolicExpression.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace BaseSemantics {

/** Shared-ownership pointer for symbolic memory state. */
using SymbolicMemoryPtr = boost::shared_ptr<class SymbolicMemory>;

/** Purely symbolic memory state.
 *
 *  This memory state stores memory as a symbolic expression consisting of read and/or write operations expressed
 *  symbolically. The memory state can be passed to SMT solvers and included in if-then-else symbolic expressions to represent
 *  different memory states according to different paths through a specimen. */
class SymbolicMemory: public MemoryState {
public:
    /** Base type. */
    using Super = MemoryState;

    /** Shared-ownership pointer. */
    using Ptr = SymbolicMemoryPtr;

private:
    SymbolicExpression::Ptr mem_;

public:
    ~SymbolicMemory();

protected:
    // All memory states should be heap allocated; use instance(), create(), or clone() instead.
    explicit SymbolicMemory(const SValuePtr &addrProtoval, const SValuePtr &valProtoval);

public:
    /** Instantiate a new empty memory state on the heap. */
    static SymbolicMemoryPtr instance(const SValuePtr &addrProtoval, const SValuePtr &valProtoval);

public:
    // documented in base class
    virtual MemoryStatePtr create(const SValuePtr &addrProtoval, const SValuePtr &valProtoval) const override;

    // documented in base class
    virtual MemoryStatePtr clone() const override;

    /** Convert pointer to a SymbolicMemory pointer.
     *
     *  Converts @p x to a SymbolicMemoryPtr and asserts that it is non-null. */
    static SymbolicMemoryPtr promote(const MemoryStatePtr&);

public:
    /** Property: the symbolic expression for the memory.
     *
     * @{ */
    SymbolicExpression::Ptr expression() const;
    void expression(const SymbolicExpression::Ptr &mem);
    /** @} */

public:
    virtual bool merge(const MemoryStatePtr &other, RiscOperators *addrOps, RiscOperators *valOps) override;

    virtual void clear() override;

    virtual SValuePtr readMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) override;

    virtual void writeMemory(const SValuePtr &address, const SValuePtr &value,
                             RiscOperators *addrOps, RiscOperators *valOps) override;

    virtual SValuePtr peekMemory(const SValuePtr &address, const SValuePtr &dflt,
                                 RiscOperators *addrOps, RiscOperators *valOps) override;

public:
    virtual void hash(Combinatorics::Hasher&, RiscOperators *addrOps, RiscOperators *valOps) const override;

    virtual void print(std::ostream&, Formatter&) const override;
};
    
} // namespace
} // namespace
} // namespace
} // namespace

#endif
#endif
