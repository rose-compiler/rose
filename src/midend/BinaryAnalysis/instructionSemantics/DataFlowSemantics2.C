#include "sage3basic.h"
#include "DataFlowSemantics2.h"
#include "Diagnostics.h"

namespace Rose {
namespace BinaryAnalysis {
namespace InstructionSemantics2 {
namespace DataFlowSemantics {

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                      SValue
//
// The semantic value used by the inner dataflow semantics layer.  These values only carry information about definers: each
// SValue contains the set of abstract locations which define the value.
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef Sawyer::SharedPointer<class SValue> SValuePtr;

class SValue: public BaseSemantics::SValue {
    std::vector<AbstractLocation> sources_;             // the locations that defined this value

    // The normal C++ constructors; same arguments as the base class
protected:
    explicit SValue(size_t nbits): BaseSemantics::SValue(nbits) {}
    SValue(size_t nbits, uint64_t number): BaseSemantics::SValue(nbits) {}

public:
    /** Instantiate a new prototypical value. Prototypical values are only used for their virtual constructors. */
    static SValuePtr instance() {
        return SValuePtr(new SValue(1));
    }
    
    /** Instantiate a new undefined value of specified width. */
    static SValuePtr instance(size_t nbits) {
        return SValuePtr(new SValue(nbits));
    }

    /** Instantiate a new concrete value. */
    static SValuePtr instance(size_t nbits, uint64_t value) {
        return SValuePtr(new SValue(nbits, value));
    }

    // Virtual allocating constructors
public:
    virtual BaseSemantics::SValuePtr bottom_(size_t nbits) const ROSE_OVERRIDE {
        return instance(nbits);
    }
    virtual BaseSemantics::SValuePtr undefined_(size_t nbits) const ROSE_OVERRIDE {
        return instance(nbits);
    }
    virtual BaseSemantics::SValuePtr unspecified_(size_t nbits) const ROSE_OVERRIDE {
        return instance(nbits);
    }
    virtual BaseSemantics::SValuePtr number_(size_t nbits, uint64_t value) const ROSE_OVERRIDE {
        return instance(nbits, value);
    }
    virtual BaseSemantics::SValuePtr copy(size_t new_width=0) const ROSE_OVERRIDE {
        SValuePtr retval(new SValue(*this));
        if (new_width!=0 && new_width!=retval->get_width())
            retval->set_width(new_width);
        retval->sources_ = sources_;
        return retval;
    }

public:
    /** Promote a base value to a value of this type.  The value @p v must have an appropriate dynamic type. */
    static SValuePtr promote(const BaseSemantics::SValuePtr &v) { // hot
        SValuePtr retval = v.dynamicCast<SValue>();
        ASSERT_not_null(retval);
        return retval;
    }

    // Override virtual methods...
public:
    virtual Sawyer::Optional<BaseSemantics::SValuePtr>
    createOptionalMerge(const BaseSemantics::SValuePtr &other, const BaseSemantics::MergerPtr&,
                        const SmtSolverPtr&) const ROSE_OVERRIDE {
        TODO("[Robb P. Matzke 2015-08-10]");
    }

    virtual bool may_equal(const BaseSemantics::SValuePtr &other,
                           const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return true;
    }

    virtual bool must_equal(const BaseSemantics::SValuePtr &other,
                            const SmtSolverPtr &solver = SmtSolverPtr()) const ROSE_OVERRIDE {
        return false;
    }

    virtual bool isBottom() const ROSE_OVERRIDE {
        return false;
    }

    virtual bool is_number() const ROSE_OVERRIDE {
        return false;
    }

    virtual uint64_t get_number() const ROSE_OVERRIDE {
        ASSERT_not_reachable("DataFlowSementics::SValue are never concrete");
#ifdef _MSC_VER
        return 0;
#endif
    }

    virtual void print(std::ostream &out, BaseSemantics::Formatter &fmt) const ROSE_OVERRIDE {
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
    void insert(const SValuePtr &other) {
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

    // The normal C++ constructors; protected because this object is reference counted
protected:
    explicit InnerRiscOperators(const SValuePtr &protoval, const SmtSolverPtr &solver = SmtSolverPtr())
        : BaseSemantics::RiscOperators(protoval, solver) {
        name("DataFlow(Inner)");
    }

    // Static allocating constructor; no state since register and memory I/O methods are no-ops
public:
    static InnerRiscOperatorsPtr instance(const SmtSolverPtr &solver = SmtSolverPtr()) {
        return InnerRiscOperatorsPtr(new InnerRiscOperators(SValue::instance(), solver));
    }

    // Virtual constructors
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
    static InnerRiscOperatorsPtr promote(const BaseSemantics::RiscOperatorsPtr &x) {
        InnerRiscOperatorsPtr retval = boost::dynamic_pointer_cast<InnerRiscOperators>(x);
        ASSERT_not_null(retval);
        return retval;
    }

    // Create a new SValue whose source abstract locations are the union of the source abstract locations of the arguments.
protected:
    SValuePtr mergeSources(size_t nbits, const BaseSemantics::SValuePtr &a) {
        return SValue::promote(a->copy(nbits));
    }
    SValuePtr mergeSources(size_t nbits, const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) {
        SValuePtr result = SValue::promote(a->copy(nbits));
        result->insert(SValue::promote(b)->sources());
        return result;
    }
    SValuePtr mergeSources(size_t nbits, const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                           const BaseSemantics::SValuePtr &c) {
        SValuePtr result = SValue::promote(a->copy(nbits));
        result->insert(SValue::promote(b)->sources());
        result->insert(SValue::promote(c)->sources());
        return result;
    }

    // Inherited RISC operations
public:
    virtual void interrupt(int majr, int minr) ROSE_OVERRIDE {
        ASSERT_not_implemented("[Robb P. Matzke 2014-05-19]");
    }

    virtual BaseSemantics::SValuePtr and_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a, b);
    }

    virtual BaseSemantics::SValuePtr or_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a, b);
    }

    virtual BaseSemantics::SValuePtr xor_(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a, b);
    }

    virtual BaseSemantics::SValuePtr invert(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a);
    }

    virtual BaseSemantics::SValuePtr extract(const BaseSemantics::SValuePtr &a,
                                             size_t begin_bit, size_t end_bit) ROSE_OVERRIDE {
        return mergeSources(end_bit-begin_bit, a);
    }

    virtual BaseSemantics::SValuePtr concat(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(a->get_width() + b->get_width(), a, b);
    }

    virtual BaseSemantics::SValuePtr leastSignificantSetBit(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a);
    }

    virtual BaseSemantics::SValuePtr mostSignificantSetBit(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a);
    }

    virtual BaseSemantics::SValuePtr rotateLeft(const BaseSemantics::SValuePtr &a,
                                                const BaseSemantics::SValuePtr &sa) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a, sa);
    }

    virtual BaseSemantics::SValuePtr rotateRight(const BaseSemantics::SValuePtr &a,
                                                 const BaseSemantics::SValuePtr &sa) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a, sa);
    }

    virtual BaseSemantics::SValuePtr shiftLeft(const BaseSemantics::SValuePtr &a,
                                               const BaseSemantics::SValuePtr &sa) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a, sa);
    }

    virtual BaseSemantics::SValuePtr shiftRight(const BaseSemantics::SValuePtr &a,
                                                const BaseSemantics::SValuePtr &sa) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a, sa);
    }

    virtual BaseSemantics::SValuePtr shiftRightArithmetic(const BaseSemantics::SValuePtr &a,
                                                          const BaseSemantics::SValuePtr &sa) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a, sa);
    }

    virtual BaseSemantics::SValuePtr equalToZero(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE {
        return mergeSources(1, a);
    }

    virtual BaseSemantics::SValuePtr ite(const BaseSemantics::SValuePtr &sel, const BaseSemantics::SValuePtr &a,
                                         const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), sel, a, b);
    }

    virtual BaseSemantics::SValuePtr unsignedExtend(const BaseSemantics::SValuePtr &a, size_t new_width) ROSE_OVERRIDE {
        return mergeSources(new_width, a);
    }

    virtual BaseSemantics::SValuePtr signExtend(const BaseSemantics::SValuePtr &a, size_t new_width) ROSE_OVERRIDE {
        return mergeSources(new_width, a);
    }

    virtual BaseSemantics::SValuePtr add(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a, b);
    }

    virtual BaseSemantics::SValuePtr addWithCarries(const BaseSemantics::SValuePtr &a, const BaseSemantics::SValuePtr &b,
                                                    const BaseSemantics::SValuePtr &c,
                                                    BaseSemantics::SValuePtr &carry_out/*out*/)ROSE_OVERRIDE {
        carry_out = mergeSources(a->get_width(), a, b, c);
        return mergeSources(a->get_width(), a, b, c);
    }

    virtual BaseSemantics::SValuePtr negate(const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a);
    }
    
    virtual BaseSemantics::SValuePtr signedDivide(const BaseSemantics::SValuePtr &a,
                                                  const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a, b);
    }

    virtual BaseSemantics::SValuePtr signedModulo(const BaseSemantics::SValuePtr &a,
                                                  const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(b->get_width(), a, b);
    }

    virtual BaseSemantics::SValuePtr signedMultiply(const BaseSemantics::SValuePtr &a,
                                                    const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(a->get_width() + b->get_width(), a, b);
    }

    virtual BaseSemantics::SValuePtr unsignedDivide(const BaseSemantics::SValuePtr &a,
                                                    const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(a->get_width(), a, b);
    }

    virtual BaseSemantics::SValuePtr unsignedModulo(const BaseSemantics::SValuePtr &a,
                                                    const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(b->get_width(), a, b);
    }

    virtual BaseSemantics::SValuePtr unsignedMultiply(const BaseSemantics::SValuePtr &a,
                                                      const BaseSemantics::SValuePtr &b) ROSE_OVERRIDE {
        return mergeSources(a->get_width() + b->get_width(), a, b);
    }

    virtual BaseSemantics::SValuePtr readRegister(RegisterDescriptor reg,
                                                  const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE {
        ASSERT_not_reachable("readRegister is not possible for this semantic domain");
#ifdef _MSC_VER
        return BaseSemantics::SValuePtr();
#endif
    }

    virtual void writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &a) ROSE_OVERRIDE {
        ASSERT_not_reachable("writeRegister is not possible for this semantic domain");
    }

    virtual BaseSemantics::SValuePtr readMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt,
                                                const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        ASSERT_not_reachable("readMemory is not possible for this semantic domain");
#ifdef _MSC_VER
        return BaseSemantics::SValuePtr();
#endif
    }
    
    virtual BaseSemantics::SValuePtr peekMemory(RegisterDescriptor segreg,
                                                const BaseSemantics::SValuePtr &addr,
                                                const BaseSemantics::SValuePtr &dflt) ROSE_OVERRIDE {
        ASSERT_not_reachable("peekMemory is not possible for this semantic domain");
#ifdef _MSC_VER
        return BaseSemantics::SValuePtr();
#endif
    }
    
    virtual void writeMemory(RegisterDescriptor segreg,
                             const BaseSemantics::SValuePtr &addr,
                             const BaseSemantics::SValuePtr &data,
                             const BaseSemantics::SValuePtr &cond) ROSE_OVERRIDE {
        ASSERT_not_reachable("writeMemory is not possible for this semantic domain");
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                              Risc Operators
//
// The (outer) RISC operators are a multi-domain defined over the inner RISC operators and a user-specified domain.  The user-
// specified domain is used to compute memory addresses that become some of the abstract locations in the result.  The inner
// RISC operators and semantic values only carry information about what abstract locations are used to define other abstract
// locations (i.e., the data flow).
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
RiscOperators::init(const BaseSemantics::RiscOperatorsPtr &userDomain) {
    name("DataFlow(Outer)");
    regdict_ = userDomain->currentState()->registerState()->get_register_dictionary();
    InnerRiscOperatorsPtr innerDomain = InnerRiscOperators::instance(userDomain->solver());
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
RiscOperators::insertDataFlowEdges(const BaseSemantics::SValuePtr &svalue_, const AbstractLocation &target) {
    SValuePtr svalue = SValue::promote(svalue_);
    DataFlowEdge::EdgeType edgeType = DataFlowEdge::CLOBBER;
    if (svalue->sources().empty()) {
        insertDataFlowEdge(AbstractLocation(), target, edgeType);
    } else {
        BOOST_FOREACH (const AbstractLocation &source, svalue->sources()) {
            insertDataFlowEdge(source, target, edgeType);
            edgeType = DataFlowEdge::AUGMENT;
        }
    }
}

BaseSemantics::SValuePtr
RiscOperators::readRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt) {
    TemporarilyDeactivate deactivate(this, innerDomainId_);
    MultiSemantics::SValuePtr result = MultiSemantics::SValue::promote(Super::readRegister(reg, dflt));
    BaseSemantics::RiscOperatorsPtr innerDomain = get_subdomain(innerDomainId_);
    SValuePtr value = SValue::promote(innerDomain->protoval()->undefined_(reg.nBits()));
    value->insert(AbstractLocation(reg, regdict_));
    result->set_subvalue(innerDomainId_, value);
    return result;
}

BaseSemantics::SValuePtr
RiscOperators::peekRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &dflt) {
    TemporarilyDeactivate deactivate(this, innerDomainId_);
    MultiSemantics::SValuePtr result = MultiSemantics::SValue::promote(Super::peekRegister(reg, dflt));
    BaseSemantics::RiscOperatorsPtr innerDomain = get_subdomain(innerDomainId_);
    SValuePtr value = SValue::promote(innerDomain->protoval()->undefined_(reg.nBits()));
    value->insert(AbstractLocation(reg, regdict_));
    result->set_subvalue(innerDomainId_, value);
    return result;
}

void
RiscOperators::writeRegister(RegisterDescriptor reg, const BaseSemantics::SValuePtr &a_) {
    TemporarilyDeactivate deactivate(this, innerDomainId_);
    MultiSemantics::SValuePtr a = MultiSemantics::SValue::promote(a_);
    Super::writeRegister(reg, a);
    SValuePtr innerVal = SValue::promote(a->get_subvalue(innerDomainId_));
    insertDataFlowEdges(innerVal, AbstractLocation(reg, regdict_));
}

BaseSemantics::SValuePtr
RiscOperators::readOrPeekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr_,
                                const BaseSemantics::SValuePtr &dflt_, const BaseSemantics::SValuePtr &cond,
                                bool allowSideEffects) {
    TemporarilyDeactivate deactivate(this, innerDomainId_);
    MultiSemantics::SValuePtr addr = MultiSemantics::SValue::promote(addr_);
    MultiSemantics::SValuePtr dflt = MultiSemantics::SValue::promote(dflt_);
    MultiSemantics::SValuePtr result;
    if (allowSideEffects) {
        result = MultiSemantics::SValue::promote(Super::readMemory(segreg, addr, dflt, cond));
    } else {
        result = MultiSemantics::SValue::promote(Super::peekMemory(segreg, addr, dflt));
    }

    size_t addrWidth = addr->get_width();
    size_t valueWidth = dflt->get_width();
    ASSERT_require(0 == valueWidth % 8);

    BaseSemantics::RiscOperatorsPtr userOps = get_subdomain(userDomainId_);
    BaseSemantics::RiscOperatorsPtr innerOps = get_subdomain(innerDomainId_);

    SValuePtr definers = SValue::promote(dflt->get_subvalue(innerDomainId_)->copy());
    for (size_t bytenum=0; bytenum<valueWidth/8; ++bytenum) {
        BaseSemantics::SValuePtr byteAddr = userOps->add(addr->get_subvalue(userDomainId_),
                                                         userOps->number_(addrWidth, bytenum));
        definers->insert(AbstractLocation(byteAddr));
    }
    result->set_subvalue(innerDomainId_, definers);
    return result;
}

BaseSemantics::SValuePtr
RiscOperators::readMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                          const BaseSemantics::SValuePtr &dflt, const BaseSemantics::SValuePtr &cond) {
    if (cond->is_number() && !cond->get_number())
        return dflt;
    return readOrPeekMemory(segreg, addr, dflt, cond, true /*allow side effects*/);
}

BaseSemantics::SValuePtr
RiscOperators::peekMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr,
                          const BaseSemantics::SValuePtr &dflt) {
    return readOrPeekMemory(segreg, addr, dflt, undefined_(1), false /*noside effects allowed*/);
}

void
RiscOperators::writeMemory(RegisterDescriptor segreg, const BaseSemantics::SValuePtr &addr_,
                           const BaseSemantics::SValuePtr &data_, const BaseSemantics::SValuePtr &cond) {
    if (cond->is_number() && !cond->get_number())
        return;
    TemporarilyDeactivate deactivate(this, innerDomainId_);
    MultiSemantics::SValuePtr addr = MultiSemantics::SValue::promote(addr_);
    MultiSemantics::SValuePtr data = MultiSemantics::SValue::promote(data_);
    Super::writeMemory(segreg, addr, data, cond);

    size_t addrWidth = addr->get_width();
    size_t valueWidth = data->get_width();
    ASSERT_require(0 == valueWidth % 8);

    BaseSemantics::RiscOperatorsPtr userOps = get_subdomain(userDomainId_);
    BaseSemantics::RiscOperatorsPtr innerOps = get_subdomain(innerDomainId_);

    for (size_t bytenum=0; bytenum<valueWidth/8; ++bytenum) {
        BaseSemantics::SValuePtr byteAddr = userOps->add(addr->get_subvalue(userDomainId_),
                                                         userOps->number_(addrWidth, bytenum));
        SValuePtr byte = SValue::promote(innerOps->extract(data->get_subvalue(innerDomainId_), 8*bytenum, 8*bytenum+8));
        insertDataFlowEdges(byte, AbstractLocation(byteAddr));
    }
}

} // namespace
} // namespace
} // namespace
} // namespace
