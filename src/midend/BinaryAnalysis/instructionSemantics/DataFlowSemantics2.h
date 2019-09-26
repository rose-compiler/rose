#ifndef Rose_BinaryAnalysis_DataFlowSemantics_H
#define Rose_BinaryAnalysis_DataFlowSemantics_H

#include "AbstractLocation.h"
#include "BaseSemantics2.h"
#include "MultiSemantics2.h"
#include "NullSemantics2.h"

#include <boost/foreach.hpp>
#include <Sawyer/Assert.h>
#include <Sawyer/Graph.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace DataFlowSemantics {

/** Information per dataflow edge.
 *
 *  This struct holds information associated with each edge of a dataflow graph. */
struct DataFlowEdge {
    /** Type of dataflow edge.
     *
     *  A dataflow edge can represent a flow that clobbers the previous value of the target, or a flow that augments the previous
     *  value of a target.  For instance, when reading four bytes from memory into a 32-bit register, the first byte is marked as
     *  clobbering any previous value in the register and the remaining three bytes are marked as augmenting the register.
     *  Therefore, an instruction like "MOV EAX, [ESP]" will have these data flow edges:
     *
     *  @code
     *   data flow #0 from esp_0[32] to clobber eax
     *   data flow #1 from (add[32] esp_0[32], 1[32]) to augment eax
     *   data flow #2 from (add[32] esp_0[32], 2[32]) to augment eax
     *   data flow #3 from (add[32] esp_0[32], 3[32]) to augment eax
     *  @endcode */
    enum EdgeType {
        CLOBBER,                                            /**< This edge clobbers previous flows into the target. */
        AUGMENT                                             /**< This edge augments previous flows into the target. */
    };

    size_t sequence;                                    /**< Edge sequence number unique and constant within graph. */
    EdgeType edgeType;                                  /**< Whether edge resets or augments previous flows to target. */
    DataFlowEdge(): sequence((size_t)(-1)), edgeType(CLOBBER) {}
    DataFlowEdge(size_t sequence, EdgeType edgeType): sequence(sequence), edgeType(edgeType) {}
};

/** Data flow graph.
 *
 *  Vertices are abstract locations (register descriptors and/or memory addresses) and edges indicate the flow of data from
 *  abstract location to another.  The edges are numbered in the order they were added. */
typedef Sawyer::Container::Graph<AbstractLocation, DataFlowEdge> DataFlowGraph;

/** Shared-ownership pointer for dataflow RISC operators. See @ref heap_object_shared_ownership. */
typedef boost::shared_ptr<class RiscOperators> RiscOperatorsPtr;

/** Dataflow RISC operators.
 *
 *  This is a MultiSemantics domain that contains a user-specified domain and a dataflow inner domain.  The user-specified
 *  domain is used to calculate memory addresses, and the dataflow inner domain tracks dataflow across RISC operators (like the
 *  fact that if <em>C = A + B</em> then the definers for <em>C</em> are the union of the definers for <em>A</em> and <em>B</em>;
 *  or that if <em>E = D xor D</em> then <em>E</em> has no definers since it is a constant).  State-accessing operations
 *  (reading and writing registers and memory) are handled above these two subdomains: read operations instantiate a value with
 *  a given set of definers, and write operations insert information into the final data flow graph. */
class RiscOperators: public MultiSemantics::RiscOperators {
    typedef                 MultiSemantics::RiscOperators Super;

    const RegisterDictionary *regdict_;                 // register dictionary used to print abstract locations
    size_t innerDomainId_;                              // subdomain identifier for the dataflow's inner domain
    size_t userDomainId_;                               // subdomain identifier for the user-supplied domain (memory addrs)
    DataFlowGraph dflow_;                               // dataflow graph accumulated over processInstruction() calls.

    void init(const BaseSemantics::RiscOperatorsPtr &userDomain);

    // The normal C++ constructors; protected because this object is reference counted
protected:
    explicit RiscOperators(const BaseSemantics::RiscOperatorsPtr &userDomain)
        : MultiSemantics::RiscOperators(MultiSemantics::SValue::instance(), SmtSolverPtr()) {
        init(userDomain);
    }

public:
    /** Static allocating constructor.
     *
     *  Constructs a semantic framework for discovering data flow. The caller provides a semantic domain that is used to
     *  identify memory locations.  Any domain can be used, but usually a SymbolicSemantics domain works best. */
    static RiscOperatorsPtr instance(const BaseSemantics::RiscOperatorsPtr &childOps) {
        return RiscOperatorsPtr(new RiscOperators(childOps));
    }

    // Virtual constructors inherited from the super class.  These are disabled because users are expected to create this
    // dataflow semantics framework only through the "instance" method. (But we still must override them.)
private:
    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::SValuePtr &protoval,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_reachable("should not be called by user code");
#ifdef _MSC_VER
        return BaseSemantics::RiscOperatorsPtr();
#endif
    }

    virtual BaseSemantics::RiscOperatorsPtr create(const BaseSemantics::StatePtr &state,
                                                   const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        ASSERT_not_reachable("should not be called by user code");
#ifdef _MSC_VER
        return BaseSemantics::RiscOperatorsPtr();
#endif
    }

    // Dynamic pointer cast
public:
    /** Run-time promotion of a base RiscOperators pointer to operators for this domain. This is a checked conversion--it
     *  will fail if @p x does not point to an object with appropriate dynamic type. */
    static RiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        RiscOperatorsPtr retval = boost::dynamic_pointer_cast<RiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

public:
    /** Clear dataflow graph.
     *
     *  Clears the dataflow graph without affecting the user-supplied virtual machine state. */
    void clearGraph() { dflow_.clear(); }

    /** Return the dataflow graph. */
    const DataFlowGraph& getGraph() const { return dflow_; }

private:
    // Temporarily disables a subdomain, restoring it the its original state when this object is canceled or destroyed.
    class TemporarilyDeactivate {
        MultiSemantics::RiscOperators *ops_;
        size_t id_;
        bool wasActive_, canceled_;
    public:
        TemporarilyDeactivate(MultiSemantics::RiscOperators *ops, size_t id)
            : ops_(ops), id_(id), wasActive_(ops->is_active(id)), canceled_(false) {
            ops->set_active(id, false);
        }
        ~TemporarilyDeactivate() {
            cancel();
        }
        void cancel() {
            if (!canceled_) {
                ops_->set_active(id_, wasActive_);
                canceled_ = true;
            }
        }
    };

    // Insert edges (and vertices if necessary) into the data flow graph, dgraph_
    void insertDataFlowEdge(const AbstractLocation &source, const AbstractLocation &target, DataFlowEdge::EdgeType);
    void insertDataFlowEdges(const BaseSemantics::SValuePtr &svalue, const AbstractLocation &target);
    
    // Override state-accessing methods.  These methods do the normal thing in the user domain, but in the dataflow domain they
    // behave differently.  When reading, they return the set of abstract locations that were read (either a register or the
    // address for the bytes of memory); when writing they add vertices and edges to the dataflow graph.
public:
    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor reg,
                                                  const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;
    virtual BaseSemantics::SValuePtr peekRegister(RegisterDescriptor reg,
                                                  const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE;

    virtual void writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &a_) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr_,
                                                const BaseSemantics::SValuePtr &dflt_,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;

    virtual BaseSemantics::SValuePtr peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr_,
                                                const BaseSemantics::SValuePtr &dflt_) ROSE_OVERRIDE;

    virtual void writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr_,
                             const BaseSemantics::SValuePtr &data_, const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE;

protected:
    BaseSemantics::SValuePtr readOrPeekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                                              const BaseSemantics::SValuePtr &dflt,
                                              const BaseSemantics::SValuePtr &cond, bool allowSideEffects);

};

} // namespace
} // namespace
} // namespace
} // namespace

#endif
