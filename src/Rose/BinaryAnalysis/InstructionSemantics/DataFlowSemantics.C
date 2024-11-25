#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/InstructionSemantics/DataFlowSemantics.h>

#include <Rose/As.h>
#include <Rose/Diagnostics.h>

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics {
namespace DataFlowSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                      SValue
//
// The semantic value used by the inner dataflow semantics layer.  These values only carry information about definers: each
// SValue contains the set of abstract locations which define the value.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


using SValuePtr = Sawyer::SharedPointer<class SValue>;

class SValue: public BaseSemantics::SValue {
public:
    using Ptr = SValuePtr;

private:
    std::vector<AbstractLocation> sources_;             // the locations that defined this value

    // The normal C++ constructors; same arguments as the base class
protected:
    explicit SValue(size_t nbits): BaseSemantics::SValue(nbits) {}
    SValue(size_t nbits, uint64_t /*number*/): BaseSemantics::SValue(nbits) {}

public:
    /** Instantiate a new prototypical value. Prototypical values are only used for their virtual constructors. */
    static SValue::Ptr instance() {
        return SValue::Ptr(new SValue(1));
    }
    
    /** Instantiate a new undefined value of specified width. */
    static SValue::Ptr instance(size_t nbits) {
        return SValue::Ptr(new SValue(nbits));
    }

    /** Instantiate a new concrete value. */
    static SValue::Ptr instance(size_t nbits, uint64_t value) {
        return SValue::Ptr(new SValue(nbits, value));
    }

    // Virtual allocating constructors
public:
    virtual BaseSemantics::SValue::Ptr bottom_(size_t nbits) const override {
        return instance(nbits);
    }
    virtual BaseSemantics::SValue::Ptr undefined_(size_t nbits) const override {
        return instance(nbits);
    }
    virtual BaseSemantics::SValue::Ptr unspecified_(size_t nbits) const override {
        return instance(nbits);
    }
    virtual BaseSemantics::SValue::Ptr number_(size_t nbits, uint64_t value) const override {
        return instance(nbits, value);
    }
    virtual BaseSemantics::SValue::Ptr copy(size_t new_width=0) const override {
        SValue::Ptr retval(new SValue(*this));
        if (new_width!=0 && new_width!=retval->nBits())
            retval->set_width(new_width);
        retval->sources_ = sources_;
        return retval;
    }

public:
    /** Promote a base value to a value of this type.  The value @p v must have an appropriate dynamic type. */
    static SValue::Ptr promote(const BaseSemantics::SValue::Ptr &v) { // hot
        SValue::Ptr retval = as<SValue>(v);
        ASSERT_not_null(retval);
        return retval;
    }

    // Override virtual methods...
public:
    virtual Sawyer::Optional<BaseSemantics::SValue::Ptr>
    createOptionalMerge(const BaseSemantics::SValue::Ptr &/*other*/, const BaseSemantics::Merger::Ptr&,
                        const SmtSolverPtr&) const override {
        TODO("[Robb P. Matzke 2015-08-10]");
    }

    virtual bool may_equal(const BaseSemantics::SValue::Ptr &/*other*/,
                           const SmtSolverPtr& = SmtSolverPtr()) const override {
        return true;
    }

    virtual bool must_equal(const BaseSemantics::SValue::Ptr &/*other*/,
                            const SmtSolverPtr& = SmtSolverPtr()) const override {
        return false;
    }

    virtual bool isBottom() const override {
        return false;
    }

    virtual bool is_number() const override {
        return false;
    }

    virtual uint64_t get_number() const override {
        ASSERT_not_reachable("DataFlowSementics::SValue are never concrete");
#ifdef _MSC_VER
        return 0;
#endif
    }

    virtual void hash(Combinatorics::Hasher&) const override {
        ASSERT_not_implemented("[Robb Matzke 2021-03-26]");
    }

    virtual void print(std::ostream &out, BaseSemantics::Formatter&) const override {
        out <<"{";
        for (size_t i=0; i<sources_.size(); ++i)
            out <<" " <<sources_[i];
        out <<" }";
    }

public:
    /** Insert an abstract location.
     *
     *  SValue objects track only which abstract locations contributed to a value.  This method adds another abstract
     *  location (or multiple) to the list.
     *
     *  @{ */
    void insert(const AbstractLocation &aloc) {
        for (size_t i=0; i<sources_.size(); ++i) {
            if (sources_[i].mustAlias(aloc))
                return;
        }
        sources_.push_back(aloc);
    }
    void insert(const std::vector<AbstractLocation> &alocs) {
        for (size_t i=0; i<alocs.size(); ++i)
            insert(alocs[i]);
    }
    void insert(const SValue::Ptr &other) {
        insert(other->sources_);
    }
    /** @} */

    /** Defining locations.
     *
     *  Returns the list of abstract locations that defined this value. */
    const std::vector<AbstractLocation>& sources() const { return sources_; }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              Inner Risc Operators
//
// The inner RISC oprators define their operations in terms of the SValue which stores the set of abstract locations that
// define the value.  For instance, "C = A + B" results in the definers of C being the union of the definers of A and the
// definers of B.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef boost::shared_ptr<class InnerRiscOperators> InnerRiscOperatorsPtr;

class InnerRiscOperators: public BaseSemantics::RiscOperators {
public:
    using Ptr = InnerRiscOperatorsPtr;

    // The normal C++ constructors; protected because this object is reference counted
protected:
    explicit InnerRiscOperators(const SValue::Ptr &protoval, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(protoval, solver) {
        name("DataFlow(Inner)");
    }

    // Static allocating constructor; no state since register and memory I/O methods are no-ops
public:
    static InnerRiscOperators::Ptr instance(const SmtSolverPtr &solver = SmtSolverPtr()) {
        return InnerRiscOperators::Ptr(new InnerRiscOperators(SValue::instance(), solver));
    }

    // Virtual constructors
private:
    virtual BaseSemantics::RiscOperators::Ptr create(const BaseSemantics::SValue::Ptr &/*protoval*/,
                                                   const SmtSolverPtr& = SmtSolverPtr()) const override {
        ASSERT_not_reachable("should not be called by user code");
#ifdef _MSC_VER
        return BaseSemantics::RiscOperators::Ptr();
#endif
    }

    virtual BaseSemantics::RiscOperators::Ptr create(const BaseSemantics::State::Ptr&,
                                                   const SmtSolverPtr& = SmtSolverPtr()) const override {
        ASSERT_not_reachable("should not be called by user code");
#ifdef _MSC_VER
        return BaseSemantics::RiscOperators::Ptr();
#endif
    }

    // Dynamic pointer cast
public:
    /** Run-time promotion of a base RiscOperators pointer to operators for this domain. This is a checked conversion--it
     *  will fail if @p x does not point to an object with appropriate dynamic type. */
    static InnerRiscOperators::Ptr promote(const BaseSemantics::RiscOperators::Ptr &x) {
        InnerRiscOperators::Ptr retval = as<InnerRiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

    // Create a new SValue whose source abstract locations are the union of the source abstract locations of the arguments.
protected:
    SValue::Ptr mergeSources(size_t nbits, const BaseSemantics::SValue::Ptr &a) {
        return SValue::promote(a->copy(nbits));
    }
    SValue::Ptr mergeSources(size_t nbits, const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) {
        SValue::Ptr result = SValue::promote(a->copy(nbits));
        result->insert(SValue::promote(b)->sources());
        return result;
    }
    SValue::Ptr mergeSources(size_t nbits, const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b,
                           const BaseSemantics::SValue::Ptr &c) {
        SValue::Ptr result = SValue::promote(a->copy(nbits));
        result->insert(SValue::promote(b)->sources());
        result->insert(SValue::promote(c)->sources());
        return result;
    }

    // Inherited RISC operations
public:
    virtual void interrupt(int /*major*/, int /*minor*/) override {
        ASSERT_not_implemented("[Robb P. Matzke 2014-05-19]");
    }

    virtual BaseSemantics::SValue::Ptr and_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) override {
        return mergeSources(a->nBits(), a, b);
    }

    virtual BaseSemantics::SValue::Ptr or_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) override {
        return mergeSources(a->nBits(), a, b);
    }

    virtual BaseSemantics::SValue::Ptr xor_(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) override {
        return mergeSources(a->nBits(), a, b);
    }

    virtual BaseSemantics::SValue::Ptr invert(const BaseSemantics::SValue::Ptr &a) override {
        return mergeSources(a->nBits(), a);
    }

    virtual BaseSemantics::SValue::Ptr extract(const BaseSemantics::SValue::Ptr &a,
                                             size_t begin_bit, size_t end_bit) override {
        return mergeSources(end_bit-begin_bit, a);
    }

    virtual BaseSemantics::SValue::Ptr concat(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) override {
        return mergeSources(a->nBits() + b->nBits(), a, b);
    }

    virtual BaseSemantics::SValue::Ptr leastSignificantSetBit(const BaseSemantics::SValue::Ptr &a) override {
        return mergeSources(a->nBits(), a);
    }

    virtual BaseSemantics::SValue::Ptr mostSignificantSetBit(const BaseSemantics::SValue::Ptr &a) override {
        return mergeSources(a->nBits(), a);
    }

    virtual BaseSemantics::SValue::Ptr rotateLeft(const BaseSemantics::SValue::Ptr &a,
                                                const BaseSemantics::SValue::Ptr &sa) override {
        return mergeSources(a->nBits(), a, sa);
    }

    virtual BaseSemantics::SValue::Ptr rotateRight(const BaseSemantics::SValue::Ptr &a,
                                                 const BaseSemantics::SValue::Ptr &sa) override {
        return mergeSources(a->nBits(), a, sa);
    }

    virtual BaseSemantics::SValue::Ptr shiftLeft(const BaseSemantics::SValue::Ptr &a,
                                               const BaseSemantics::SValue::Ptr &sa) override {
        return mergeSources(a->nBits(), a, sa);
    }

    virtual BaseSemantics::SValue::Ptr shiftRight(const BaseSemantics::SValue::Ptr &a,
                                                const BaseSemantics::SValue::Ptr &sa) override {
        return mergeSources(a->nBits(), a, sa);
    }

    virtual BaseSemantics::SValue::Ptr shiftRightArithmetic(const BaseSemantics::SValue::Ptr &a,
                                                          const BaseSemantics::SValue::Ptr &sa) override {
        return mergeSources(a->nBits(), a, sa);
    }

    virtual BaseSemantics::SValue::Ptr equalToZero(const BaseSemantics::SValue::Ptr &a) override {
        return mergeSources(1, a);
    }

    virtual BaseSemantics::SValue::Ptr iteWithStatus(const BaseSemantics::SValue::Ptr &sel, const BaseSemantics::SValue::Ptr &a,
                                                   const BaseSemantics::SValue::Ptr &b, IteStatus &status) override {
        status = IteStatus::BOTH;
        return mergeSources(a->nBits(), sel, a, b);
    }

    virtual BaseSemantics::SValue::Ptr unsignedExtend(const BaseSemantics::SValue::Ptr &a, size_t new_width) override {
        return mergeSources(new_width, a);
    }

    virtual BaseSemantics::SValue::Ptr signExtend(const BaseSemantics::SValue::Ptr &a, size_t new_width) override {
        return mergeSources(new_width, a);
    }

    virtual BaseSemantics::SValue::Ptr add(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b) override {
        return mergeSources(a->nBits(), a, b);
    }

    virtual BaseSemantics::SValue::Ptr addWithCarries(const BaseSemantics::SValue::Ptr &a, const BaseSemantics::SValue::Ptr &b,
                                                    const BaseSemantics::SValue::Ptr &c,
                                                    BaseSemantics::SValue::Ptr &carry_out/*out*/) override {
        carry_out = mergeSources(a->nBits(), a, b, c);
        return mergeSources(a->nBits(), a, b, c);
    }

    virtual BaseSemantics::SValue::Ptr negate(const BaseSemantics::SValue::Ptr &a) override {
        return mergeSources(a->nBits(), a);
    }
    
    virtual BaseSemantics::SValue::Ptr signedDivide(const BaseSemantics::SValue::Ptr &a,
                                                  const BaseSemantics::SValue::Ptr &b) override {
        return mergeSources(a->nBits(), a, b);
    }

    virtual BaseSemantics::SValue::Ptr signedModulo(const BaseSemantics::SValue::Ptr &a,
                                                  const BaseSemantics::SValue::Ptr &b) override {
        return mergeSources(b->nBits(), a, b);
    }

    virtual BaseSemantics::SValue::Ptr signedMultiply(const BaseSemantics::SValue::Ptr &a,
                                                    const BaseSemantics::SValue::Ptr &b) override {
        return mergeSources(a->nBits() + b->nBits(), a, b);
    }

    virtual BaseSemantics::SValue::Ptr unsignedDivide(const BaseSemantics::SValue::Ptr &a,
                                                    const BaseSemantics::SValue::Ptr &b) override {
        return mergeSources(a->nBits(), a, b);
    }

    virtual BaseSemantics::SValue::Ptr unsignedModulo(const BaseSemantics::SValue::Ptr &a,
                                                    const BaseSemantics::SValue::Ptr &b) override {
        return mergeSources(b->nBits(), a, b);
    }

    virtual BaseSemantics::SValue::Ptr unsignedMultiply(const BaseSemantics::SValue::Ptr &a,
                                                      const BaseSemantics::SValue::Ptr &b) override {
        return mergeSources(a->nBits() + b->nBits(), a, b);
    }

    virtual BaseSemantics::SValue::Ptr readRegister(RegisterDescriptor,
                                                    const BaseSemantics::SValue::Ptr &/*dflt*/) override {
        ASSERT_not_reachable("readRegister is not possible for this semantic domain");
#ifdef _MSC_VER
        return BaseSemantics::SValue::Ptr();
#endif
    }

    virtual void writeRegister(RegisterDescriptor, const BaseSemantics::SValue::Ptr&) override {
        ASSERT_not_reachable("writeRegister is not possible for this semantic domain");
    }

    virtual BaseSemantics::SValue::Ptr readMemory(RegisterDescriptor /*segreg*/,
                                                  const BaseSemantics::SValue::Ptr &/*addr*/,
                                                  const BaseSemantics::SValue::Ptr &/*dflt*/,
                                                  const BaseSemantics::SValue::Ptr &/*cond*/) override {
        ASSERT_not_reachable("readMemory is not possible for this semantic domain");
#ifdef _MSC_VER
        return BaseSemantics::SValue::Ptr();
#endif
    }
    
    virtual BaseSemantics::SValue::Ptr peekMemory(RegisterDescriptor /*segreg*/,
                                                  const BaseSemantics::SValue::Ptr &/*addr*/,
                                                  const BaseSemantics::SValue::Ptr &/*dflt*/) override {
        ASSERT_not_reachable("peekMemory is not possible for this semantic domain");
#ifdef _MSC_VER
        return BaseSemantics::SValue::Ptr();
#endif
    }
    
    virtual void writeMemory(RegisterDescriptor /*segreg*/,
                             const BaseSemantics::SValue::Ptr &/*addr*/,
                             const BaseSemantics::SValue::Ptr &/*data*/,
                             const BaseSemantics::SValue::Ptr &/*cond*/) override {
        ASSERT_not_reachable("writeMemory is not possible for this semantic domain");
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// RiscOperators::TemporarilyDeactivate
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::TemporarilyDeactivate::TemporarilyDeactivate(MultiSemantics::RiscOperators *ops, size_t id)
    : ops_(ops), id_(id), wasActive_(ops->is_active(id)), canceled_(false) {
    ops->set_active(id, false);
}

RiscOperators::TemporarilyDeactivate::~TemporarilyDeactivate() {
    cancel();
}

void
RiscOperators::TemporarilyDeactivate::cancel() {
    if (!canceled_) {
        ops_->set_active(id_, wasActive_);
        canceled_ = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              Risc Operators
//
// The (outer) RISC operators are a multi-domain defined over the inner RISC operators and a user-specified domain.  The user-
// specified domain is used to compute memory addresses that become some of the abstract locations in the result.  The inner
// RISC operators and semantic values only carry information about what abstract locations are used to define other abstract
// locations (i.e., the data flow).
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

RiscOperators::RiscOperators(const BaseSemantics::RiscOperators::Ptr &userDomain)
    : MultiSemantics::RiscOperators(MultiSemantics::SValue::instance(), SmtSolver::Ptr()) {
    init(userDomain);
}

RiscOperators::Ptr
RiscOperators::instance(const BaseSemantics::RiscOperators::Ptr &childOps) {
    return Ptr(new RiscOperators(childOps));
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::create(const BaseSemantics::SValue::Ptr &/*protoval*/, const SmtSolver::Ptr&) const {
    ASSERT_not_reachable("should not be called by user code");
#ifdef _MSC_VER                                         // microsoft can't figure out that ASSERT_not_reachable never returns
    return BaseSemantics::RiscOperators::Ptr();
#endif
}

BaseSemantics::RiscOperators::Ptr
RiscOperators::create(const BaseSemantics::State::Ptr&, const SmtSolver::Ptr&) const {
    ASSERT_not_reachable("should not be called by user code");
#ifdef _MSC_VER                                         // microsoft can't figure out that ASSERT_not_reachable never returns
    return BaseSemantics::RiscOperatorsPtr();
#endif
}

RiscOperators::Ptr
RiscOperators::promote(const BaseSemantics::RiscOperators::Ptr &x) {
    RiscOperatorsPtr retval = as<RiscOperators>(x);
    ASSERT_not_null(retval);
    return retval;
}

void
RiscOperators::clearGraph() {
    dflow_.clear();
}

const DataFlowGraph&
RiscOperators::getGraph() const {
    return dflow_;
}

void
RiscOperators::init(const BaseSemantics::RiscOperators::Ptr &userDomain) {
    name("DataFlow(Outer)");
    regdict_ = userDomain->currentState()->registerState()->registerDictionary();
    InnerRiscOperators::Ptr innerDomain = InnerRiscOperators::instance(userDomain->solver());
    innerDomainId_ = add_subdomain(innerDomain, "DataFlow(Inner)");
    userDomainId_ = add_subdomain(userDomain, userDomain->name());
}

void
RiscOperators::insertDataFlowEdge(const AbstractLocation &source, const AbstractLocation &target,
                                  DataFlowEdge::EdgeType edgeType) {
    // Try to find existing vertices.  This is a linear traversal because abstract locations for memory addresses
    // don't have relational operators (they might be symbolic expressions or other user-defined types).
    DataFlowGraph::VertexIterator sourceIter = dflow_.vertices().end();
    DataFlowGraph::VertexIterator targetIter = dflow_.vertices().end();
    for (DataFlowGraph::VertexIterator iter=dflow_.vertices().begin(); iter!=dflow_.vertices().end(); ++iter) {
        const AbstractLocation &vertex = iter->value();
        if (sourceIter == dflow_.vertices().end()) {
            if (vertex.mustAlias(source))
                sourceIter = iter;
        }
        if (targetIter == dflow_.vertices().end()) {
            if (vertex.mustAlias(target))
                targetIter = iter;
        }
    }

    // Add new vertices if we didn't find existing vertices
    if (sourceIter == dflow_.vertices().end())
        sourceIter = dflow_.insertVertex(source);
    if (targetIter == dflow_.vertices().end())
        targetIter = dflow_.insertVertex(target);

    // Add the edge. Parallel edges are allowed (even desired) and they are numbered consecutively.  We can't rely on the
    // edge ID numbers provided by the graph because those ID numbers are not stable over edge erasure, so we store the
    // sequence number as part of the edge itself.
    dflow_.insertEdge(sourceIter, targetIter, DataFlowEdge(dflow_.nEdges(), edgeType));
}

void
RiscOperators::insertDataFlowEdges(const BaseSemantics::SValue::Ptr &svalue_, const AbstractLocation &target) {
    SValue::Ptr svalue = SValue::promote(svalue_);
    DataFlowEdge::EdgeType edgeType = DataFlowEdge::CLOBBER;
    if (svalue->sources().empty()) {
        insertDataFlowEdge(AbstractLocation(), target, edgeType);
    } else {
        for (const AbstractLocation &source: svalue->sources()) {
            insertDataFlowEdge(source, target, edgeType);
            edgeType = DataFlowEdge::AUGMENT;
        }
    }
}

BaseSemantics::SValue::Ptr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &dflt) {
    TemporarilyDeactivate deactivate(this, innerDomainId_);
    MultiSemantics::SValue::Ptr result = MultiSemantics::SValue::promote(Super::readRegister(reg, dflt));
    BaseSemantics::RiscOperators::Ptr innerDomain = get_subdomain(innerDomainId_);
    SValue::Ptr value = SValue::promote(innerDomain->protoval()->undefined_(reg.nBits()));
    value->insert(AbstractLocation(reg, regdict_));
    result->set_subvalue(innerDomainId_, value);
    return result;
}

BaseSemantics::SValue::Ptr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &dflt) {
    TemporarilyDeactivate deactivate(this, innerDomainId_);
    MultiSemantics::SValue::Ptr result = MultiSemantics::SValue::promote(Super::peekRegister(reg, dflt));
    BaseSemantics::RiscOperators::Ptr innerDomain = get_subdomain(innerDomainId_);
    SValue::Ptr value = SValue::promote(innerDomain->protoval()->undefined_(reg.nBits()));
    value->insert(AbstractLocation(reg, regdict_));
    result->set_subvalue(innerDomainId_, value);
    return result;
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValue::Ptr &a_) {
    TemporarilyDeactivate deactivate(this, innerDomainId_);
    MultiSemantics::SValue::Ptr a = MultiSemantics::SValue::promote(a_);
    Super::writeRegister(reg, a);
    SValue::Ptr innerVal = SValue::promote(a->get_subvalue(innerDomainId_));
    insertDataFlowEdges(innerVal, AbstractLocation(reg, regdict_));
}

BaseSemantics::SValue::Ptr
RiscOperators::readOrPeekMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &addr_,
                                const BaseSemantics::SValue::Ptr &dflt_, const BaseSemantics::SValue::Ptr &cond,
                                bool allowSideEffects) {
    TemporarilyDeactivate deactivate(this, innerDomainId_);
    MultiSemantics::SValue::Ptr addr = MultiSemantics::SValue::promote(addr_);
    MultiSemantics::SValue::Ptr dflt = MultiSemantics::SValue::promote(dflt_);
    MultiSemantics::SValue::Ptr result;
    if (allowSideEffects) {
        result = MultiSemantics::SValue::promote(Super::readMemory(segreg, addr, dflt, cond));
    } else {
        result = MultiSemantics::SValue::promote(Super::peekMemory(segreg, addr, dflt));
    }

    size_t addrWidth = addr->nBits();
    size_t valueWidth = dflt->nBits();
    ASSERT_require(0 == valueWidth % 8);

    BaseSemantics::RiscOperators::Ptr userOps = get_subdomain(userDomainId_);
    BaseSemantics::RiscOperators::Ptr innerOps = get_subdomain(innerDomainId_);

    SValue::Ptr definers = SValue::promote(dflt->get_subvalue(innerDomainId_)->copy());
    for (size_t bytenum=0; bytenum<valueWidth/8; ++bytenum) {
        BaseSemantics::SValue::Ptr byteAddr = userOps->add(addr->get_subvalue(userDomainId_),
                                                         userOps->number_(addrWidth, bytenum));
        definers->insert(AbstractLocation(byteAddr));
    }
    result->set_subvalue(innerDomainId_, definers);
    return result;
}

BaseSemantics::SValue::Ptr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &addr,
                          const BaseSemantics::SValue::Ptr &dflt, const BaseSemantics::SValue::Ptr &cond) {
    if (cond->isFalse())
        return dflt;
    return readOrPeekMemory(segreg, addr, dflt, cond, true /*allow side effects*/);
}

BaseSemantics::SValue::Ptr
RiscOperators::peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &addr,
                          const BaseSemantics::SValue::Ptr &dflt) {
    return readOrPeekMemory(segreg, addr, dflt, undefined_(1), false /*noside effects allowed*/);
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValue::Ptr &addr_,
                           const BaseSemantics::SValue::Ptr &data_, const BaseSemantics::SValue::Ptr &cond) {
    if (cond->isFalse())
        return;
    TemporarilyDeactivate deactivate(this, innerDomainId_);
    MultiSemantics::SValue::Ptr addr = MultiSemantics::SValue::promote(addr_);
    MultiSemantics::SValue::Ptr data = MultiSemantics::SValue::promote(data_);
    Super::writeMemory(segreg, addr, data, cond);

    size_t addrWidth = addr->nBits();
    size_t valueWidth = data->nBits();
    ASSERT_require(0 == valueWidth % 8);

    BaseSemantics::RiscOperators::Ptr userOps = get_subdomain(userDomainId_);
    BaseSemantics::RiscOperators::Ptr innerOps = get_subdomain(innerDomainId_);

    for (size_t bytenum=0; bytenum<valueWidth/8; ++bytenum) {
        BaseSemantics::SValue::Ptr byteAddr = userOps->add(addr->get_subvalue(userDomainId_),
                                                         userOps->number_(addrWidth, bytenum));
        SValue::Ptr byte = SValue::promote(innerOps->extract(data->get_subvalue(innerDomainId_), 8*bytenum, 8*bytenum+8));
        insertDataFlowEdges(byte, AbstractLocation(byteAddr));
    }
}

} // namespace
} // namespace
} // namespace
} // namespace

#endif
