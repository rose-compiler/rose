#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS
#include <Rose/BinaryAnalysis/DataDependence.h>

#include <Rose/Affirm.h>
#include <Rose/As.h>
#include <Rose/BinaryAnalysis/Architecture/Base.h>
#include <Rose/BinaryAnalysis/InstructionSemantics/SymbolicSemantics.h>
#include <Rose/BinaryAnalysis/Partitioner2/BasicBlock.h>
#include <Rose/BinaryAnalysis/Partitioner2/Function.h>
#include <Rose/BinaryAnalysis/Partitioner2/Partitioner.h>
#include <Rose/StringUtility/Diagnostics.h>
#include <Rose/StringUtility/Escape.h>
#include <Rose/StringUtility/NumberToString.h>

#include <SgAsmInstruction.h>                           // rose

namespace BS = Rose::BinaryAnalysis::InstructionSemantics::BaseSemantics;
namespace P2 = Rose::BinaryAnalysis::Partitioner2;

using Rose::StringUtility::addrToString;
using Rose::StringUtility::htmlEscape;
using Rose::StringUtility::plural;

using namespace Sawyer::Message::Common;

namespace Rose {
namespace BinaryAnalysis {
namespace DataDependence {

Sawyer::Message::Facility mlog;

void initDiagnostics() {
    static bool initialized = false;
    if (!initialized) {
        initialized = true;
        Rose::Diagnostics::initAndRegister(&mlog, "Rose::BinaryAnalysis::DataDependence");
        mlog.comment("computing data dependence graphs");
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Context
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Context {
public:
    P2::Partitioner::ConstPtr partitioner;
    BS::Dispatcher::Ptr cpu;
public:
    Context(const P2::Partitioner::ConstPtr &partitioner, const BS::Dispatcher::Ptr &cpu)
        : partitioner(notnull(partitioner)), cpu(notnull(cpu)) {}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Vertex::~Vertex() {}

Vertex::Vertex(const Type type)
    : type_(type) {
    ASSERT_require2(type != Type::NORMAL, "use Vertex(const AbstractLocation&) instead");
}

Vertex::Vertex(const AbstractLocation &location)
    : location_(location) {}

Vertex::Vertex(const RegisterDescriptor reg, const RegisterDictionary::Ptr &regdict)
    : location_(reg, regdict) {}

Vertex::Vertex(const BS::SValue::Ptr &addr, const size_t nBitsInValue)
    : location_(addr, nBitsInValue / 8) {}

Vertex::Vertex(const RegisterDescriptor reg, const RegisterDictionary::Ptr &regdict,
               const BS::SValue::Ptr &offset, const size_t nBitsInValue)
    : location_(reg, regdict, offset, nBitsInValue / 8) {}

Vertex
Vertex::emptySource() {
    return Vertex(Type::EMPTY_SOURCE);
}

Vertex
Vertex::emptyTarget() {
    return Vertex(Type::EMPTY_TARGET);
}

Vertex::Type
Vertex::type() const {
    return type_;
}

const AbstractLocation&
Vertex::location() const {
    return location_;
}

std::string
Vertex::toString() const {
    if (!location_.isValid()) {
        return "∅";
    } else if (location_.isRelativeAddress() && location_.getOffset()) {
        if (const auto offset = location_.getOffset()->toSigned()) {
            std::string retval = (boost::format("mem[%s%+d]")
                                  % RegisterNames()(location_.getRegister(), location_.registerDictionary())
                                  % *offset).str();
            if (location_.nBytes() > 0)
                retval += " " + plural(location_.nBytes(), "bytes");
            return retval;
        }
    }

    return location_.toString();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// VertexKey
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VertexKey::VertexKey(const Vertex &vertex) {
    switch (vertex.type()) {
        case Vertex::Type::NORMAL:
            hash_ = vertex.location().hash();
            break;
        case Vertex::Type::EMPTY_SOURCE:
            hash_ = 0;
            break;
        case Vertex::Type::EMPTY_TARGET:
            hash_ = 1;
            break;
    }
}

bool
VertexKey::operator<(const VertexKey &other) const {
    return hash_ < other.hash_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Edge
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Edge::Edge(SgAsmInstruction *insn)
    : insn_(notnull(insn)) {}

SgAsmInstruction*
Edge::instruction() const {
    return insn_;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Graph
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Graph::~Graph() {}

Graph::Graph(const P2::Partitioner::ConstPtr &partitioner)
    : partitioner_(notnull(partitioner)),
      emptySource_(insertVertex(VertexValue::emptySource())),
      emptyTarget_(insertVertex(VertexValue::emptyTarget())) {}

Graph::Graph(const Graph &other)
    : Super(other) {
    partitioner_ = other.partitioner_;
    emptySource_ = findVertexValue(VertexValue::emptySource());
    emptyTarget_ = findVertexValue(VertexValue::emptyTarget());
}

Graph&
Graph::operator=(const Graph &other) {
    if (this != &other) {
        Super::operator=(other);
        partitioner_ = other.partitioner_;
        emptySource_ = findVertexValue(VertexValue::emptySource());
        emptyTarget_ = findVertexValue(VertexValue::emptyTarget());
    }
    return *this;
}

Graph::ConstVertexIterator
Graph::emptySource() const {
    ASSERT_require(isValidVertex(emptySource_));
    return emptySource_;
}

Graph::ConstVertexIterator
Graph::emptyTarget() const {
    ASSERT_require(isValidVertex(emptyTarget_));
    return emptyTarget_;
}

Architecture::Base::ConstPtr
Graph::architecture() const {
    return partitioner_->architecture();
}

bool
Graph::edgeExists(VertexIterator src, VertexIterator dst, SgAsmInstruction *insn) const {
    ASSERT_require(isValidVertex(src));
    ASSERT_require(isValidVertex(dst));
    ASSERT_not_null(insn);
    for (const auto &edge: src->outEdges()) {
        if (edge.target() == dst && edge.value().instruction() == insn)
            return true;
    }
    return false;
}

// Given an instruction address, return the control flow successor instruction addresses. The successor addresses may or may not be
// represented in this data dependence graph. The partitioner's CFG vertices are basic blocks instead of instructions, so we have a
// little work to do here.
std::set<Address>
Graph::controlFlowSuccessors(const Address srcAddr) const {
    std::set<Address> retval;
    if (const P2::BasicBlock::Ptr bb = partitioner_->basicBlockContainingInstruction(srcAddr)) {
        const size_t i = *bb->instructionIndex(srcAddr);
        if (i + 1 < bb->nInstructions()) {
            retval.insert(bb->instructions()[i+1]->get_address());
        } else {
            for (const auto &cfgEdge: partitioner_->findPlaceholder(bb->address())->outEdges()) {
                if (const auto dstAddr = cfgEdge.target()->value().optionalAddress())
                    retval.insert(*dstAddr);
            }
        }
    }
    return retval;
}

Graph::Subgraphs
Graph::makeGraphvizSubgraphs() const {
    Subgraphs subgraphs;
    for (auto edge = edges().begin(); edge != edges().end(); ++edge)
        subgraphs[edge->value().instruction()->get_address()].push_back(edge);
    return subgraphs;
}

void
Graph::emitGraphvizLocationVertices(std::ostream &out) const {
    out <<"\n  # Location vertices (registers and memory)\n";
    for (const auto &vertex: vertices()) {
        out <<"  v" <<vertex.id()
            <<" [shape=oval"
            <<" label=<" <<vertex.id() <<": " <<htmlEscape(vertex.value().toString()) <<">"
            <<" style=filled";

        // Color based on purpose
        if (vertex.nInEdges() == 0 && vertex.nOutEdges() == 0) {
            out <<" fillcolor=crimson";                 // no access
        } else if (vertex.nInEdges() == 0) {
            out <<" fillcolor=cadetblue1";              // read-only
        } else if (vertex.nOutEdges() == 0) {
            out <<" fillcolor=darkgoldenrod1";          // write-only
        } else {
            out <<" fillcolor=darkolivegreen1";         // read-write
        }

        out <<"];\n";
    }

}

void
Graph::emitGraphvizInstructionVertices(std::ostream &out, const Subgraphs &subgraphs) const {
    out <<"\n  # Instruction subgraphs, each containing one or more data flows\n";
    for (const auto &subgraph: subgraphs) {
        out <<"  subgraph cluster_" <<addrToString(subgraph.first).substr(2) <<" {\n"
            <<"    graph [style=filled fillcolor=gray75];\n"
            <<"    label = <" <<htmlEscape(subgraph.second[0]->value().instruction()->toStringNoColor()) <<">;\n";
        for (const auto &edge: subgraph.second) {
            out <<"    e" <<edge->id()
                <<" [shape=box"
                <<" label=<" <<edge->id() <<"<br/>" <<edge->source()->id() <<"→" <<edge->target()->id() <<">"
                <<" style=filled fillcolor=white"
                <<"];\n";
        }
        out <<"  }\n";
    }
}

void
Graph::emitGraphvizDataFlowEdges(std::ostream &out) const {
    out <<"\n  # Edges for the data flows (location to instruction to location)\n";
    for (const auto &edge: edges()) {
        out <<"  v" <<edge.source()->id() <<" -> e" <<edge.id() <<";\n";
        out <<"  e" <<edge.id() <<" -> v" <<edge.target()->id() <<";\n";
    }
}

void
Graph::emitGraphvizControlFlowEdges(std::ostream &out, const Subgraphs &subgraphs) const {
    out <<"\n  # Edges for the control flows (instruction to instruction)\n";
    for (const auto &addrEdges: subgraphs) {
        const auto cfgSuccessorAddrs = controlFlowSuccessors(addrEdges.first);
        for (const Address dstAddr: cfgSuccessorAddrs) {
            const auto found = subgraphs.find(dstAddr);
            if (found != subgraphs.end()) {
                // Create a Graphviz edge between two subgraphs. Graphviz cannot do this directly, so we need to create the graphviz
                // edge between two vertices within the subgraphs and tell Graphviz where to put the source and destination ends of
                // the edge.
                ASSERT_forbid(addrEdges.second.empty()); // the source subgraph address and vertices
                ASSERT_forbid(found->second.empty());    // the destination subgraph address and vertices
                out <<"  e" <<addrEdges.second.back()->id() <<" -> e" <<found->second[0]->id()
                    <<" [style=dashed color=gray60"
                    <<" ltail=cluster_" <<addrToString(addrEdges.first).substr(2)
                    <<" lhead=cluster_" <<addrToString(found->first).substr(2)
                    <<"];\n";
            }
        }
    }
}

void
Graph::toGraphviz(std::ostream &out) const {
    const Subgraphs subgraphs = makeGraphvizSubgraphs();
    out <<"digraph ddg {\n"
        <<"  compound = true;\n";
    emitGraphvizLocationVertices(out);
    emitGraphvizInstructionVertices(out, subgraphs);
    emitGraphvizDataFlowEdges(out);
    emitGraphvizControlFlowEdges(out, subgraphs);
    out <<"}\n";
}

std::ostream&
operator<<(std::ostream &out, const Graph &ddg) {
    ddg.toGraphviz(out);
    return out;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EdgeCreator
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class EdgeCreator: public SymbolicExpression::Visitor {
private:
    Graph &ddg;
    Graph::VertexIterator dstVertex;
    SgAsmInstruction *insn;
    size_t nEdgesAdded = 0;
    std::set<size_t> &unusedReads;

public:
    EdgeCreator(Graph &ddg, Graph::VertexIterator dstVertex, SgAsmInstruction *insn, std::set<size_t> &unusedReads)
        : ddg(ddg), dstVertex(dstVertex), insn(notnull(insn)), unusedReads(unusedReads) {
        ASSERT_require(ddg.isValidVertex(dstVertex));
    }

    SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Node *node) override {
        if (!node->userData().empty()) {
            auto srcVertex = boost::any_cast<Graph::VertexIterator>(node->userData());
            ASSERT_require(ddg.isValidVertex(srcVertex));
            if (!ddg.edgeExists(srcVertex, dstVertex, insn)) {
                unusedReads.erase(srcVertex->id());
                ddg.insertEdge(srcVertex, dstVertex, Edge(insn));
                ++nEdgesAdded;
            }
            return SymbolicExpression::TRUNCATE;
        } else {
            return SymbolicExpression::CONTINUE;
        }
    }

    void finish() {
        if (0 == nEdgesAdded)
            ddg.insertEdge(ddg.emptySource(), dstVertex, Edge(insn));
    }
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Semantics
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
namespace Semantics {

namespace SymSem = Rose::BinaryAnalysis::InstructionSemantics::SymbolicSemantics;

using Merger = SymSem::Merger;
using Formatter = SymSem::Formatter;
using SValue = SymSem::SValue;
using RegisterState = SymSem::RegisterState;
using MemoryState = SymSem::MemoryListState;
using State = SymSem::State;

class RiscOperators: public SymSem::RiscOperators {
public:
    using Ptr = boost::shared_ptr<RiscOperators>;

private:
    Graph ddg_;
    SymbolicExpression::Ptr initialSp_, initialFp_;
    std::set<size_t> unusedReads_;                      // locations read but not used for anything
    P2::Function::Ptr function_;                        // function being analyzed

protected:
    RiscOperators(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function, const BS::State::Ptr &state,
                  const SmtSolver::Ptr &solver)
        : SymSem::RiscOperators(state, solver), ddg_(partitioner), function_(function) {}

public:
    static Ptr instance(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function,
                        const SmtSolver::Ptr &solver = SmtSolver::Ptr()) {
        auto protoval = SValue::instance();
        auto regState = RegisterState::instance(protoval, partitioner->architecture()->registerDictionary());
        auto memState = MemoryState::instance(protoval, protoval);
        auto state = State::instance(regState, memState);
        return Ptr((new RiscOperators(partitioner, function, state, solver))->config());
    }

    BS::RiscOperators::Ptr create(const BS::SValue::Ptr &protoval, const SmtSolver::Ptr &solver = SmtSolver::Ptr()) const override {
        ASSERT_not_implemented("unsupported");
    }

    BS::RiscOperators::Ptr create(const BS::State::Ptr &state, const SmtSolver::Ptr &solver = SmtSolver::Ptr()) const override {
        ASSERT_not_implemented("unsupported");
    }

    static Ptr promote(const BS::RiscOperators::Ptr &ops) {
        return notnull(as<RiscOperators>(ops));
    }

    const Graph& dataDependencyGraph() const {
        return ddg_;
    }

    void startInstruction(SgAsmInstruction *insn) override {
        unusedReads_.clear();

        if (const RegisterDescriptor SP = currentState()->registerState()->registerDictionary()->stackPointerRegister()) {
            const auto dflt = undefined_(SP.nBits());
            initialSp_ = SValue::promote(readRegister(SP, dflt))->get_expression();
            ASSERT_require(initialSp_->isVariable2());
        }

        if (const RegisterDescriptor FP = currentState()->registerState()->registerDictionary()->stackFrameRegister()) {
            const auto dflt = undefined_(FP.nBits());
            initialFp_ = SValue::promote(readRegister(FP, dflt))->get_expression();
            ASSERT_require(initialFp_->isVariable2());
        }

        SymSem::RiscOperators::startInstruction(insn);
    }

    void finishInstruction(SgAsmInstruction *insn) override {
        for (const size_t id: unusedReads_) {
            const auto srcVertex = ddg_.findVertex(id);
            ASSERT_require(ddg_.isValidVertex(srcVertex));
            ddg_.insertEdge(srcVertex, ddg_.emptyTarget(), Edge(insn));
        }

        SymSem::RiscOperators::finishInstruction(insn);
    }

    BS::SValue::Ptr readRegister(const RegisterDescriptor reg, const BS::SValue::Ptr &dflt) override {
        SValue::Ptr retval = SValue::promote(SymSem::RiscOperators::readRegister(reg, dflt));
        if (currentInstruction()) {
            const auto srcVertex = insertVertexMaybe(reg);
            unusedReads_.insert(srcVertex->id());
            notnull(retval->get_expression())->userData(srcVertex);
        }
        return retval;
    }

    void writeRegister(const RegisterDescriptor reg, const BS::SValue::Ptr &value_) override {
        if (SgAsmInstruction *insn = currentInstruction()) {
            const auto dstVertex = insertVertexMaybe(reg);
            EdgeCreator edgeCreator(ddg_, dstVertex, insn, unusedReads_);
            notnull(SValue::promote(value_)->get_expression())->depthFirstTraversal(edgeCreator);
            edgeCreator.finish();
        }
        return SymSem::RiscOperators::writeRegister(reg, value_);
    }

    BS::SValue::Ptr readMemory(const RegisterDescriptor segreg, const BS::SValue::Ptr &addr, const BS::SValue::Ptr &dflt,
                               const BS::SValue::Ptr &cond) override {
        SValue::Ptr retval = SValue::promote(SymSem::RiscOperators::readMemory(segreg, addr, dflt, cond));
        if (currentInstruction()) {
            const auto srcVertex = insertVertexMaybe(addr, dflt->nBits());
            unusedReads_.insert(srcVertex->id());
            notnull(retval->get_expression())->userData(srcVertex);
        }
        return retval;
    }

    void writeMemory(const RegisterDescriptor segreg, const BS::SValue::Ptr &addr, const BS::SValue::Ptr &value,
                     const BS::SValue::Ptr &cond) override {
        if (SgAsmInstruction *insn = currentInstruction()) {
            const auto dstVertex = insertVertexMaybe(addr, value->nBits());
            EdgeCreator edgeCreator(ddg_, dstVertex, insn, unusedReads_);
            notnull(SValue::promote(value)->get_expression())->depthFirstTraversal(edgeCreator);
            edgeCreator.finish();
        }
        return SymSem::RiscOperators::writeMemory(segreg, addr, value, cond);
    }

private:
    RiscOperators* config() {
        ASSERT_not_null(currentState());
        ASSERT_not_null(currentState()->registerState());
        return this;
    }

    // Insert a vertex into the data dependence graph if it doesn't exist yet, and return either the new vertex or the existing
    // vertex.
    Graph::VertexIterator insertVertexMaybe(const RegisterDescriptor reg) {
        const RegisterDictionary::Ptr regdict = currentState()->registerState()->registerDictionary();
        return ddg_.insertVertexMaybe(Vertex(reg, regdict));
    }

    Graph::VertexIterator insertVertexMaybe(const BS::SValue::Ptr &addr, const size_t nBitsRead) {
        ASSERT_not_null(addr);
        if (const BS::SValue::Ptr offset = stackOffset(addr)) {
            const RegisterDictionary::Ptr regdict = currentState()->registerState()->registerDictionary();
            const RegisterDescriptor SP = regdict->stackPointerRegister();
            return ddg_.insertVertexMaybe(Vertex(SP, regdict, offset, nBitsRead));
        } else {
            return ddg_.insertVertexMaybe(Vertex(addr, nBitsRead));
        }
    }

    // Test whether an expression contains another expression as a subexpression. The needle is compared to the haystack using
    // structural equivalence and the first match is returned. Returns null if no match is found.
    SymbolicExpression::Ptr findSubexpression(const SymbolicExpression::Ptr &haystack, const SymbolicExpression::Ptr &needle) {
        struct Finder: SymbolicExpression::Visitor {
            SymbolicExpression::Ptr needle;
            SymbolicExpression::Ptr found;

            explicit Finder(const SymbolicExpression::Ptr &needle)
                : needle(needle) {}

            SymbolicExpression::VisitAction preVisit(const SymbolicExpression::Ptr &node) override {
                if (node->isEquivalentTo(needle)) {
                    found = node;
                    return SymbolicExpression::TERMINATE;
                } else {
                    return SymbolicExpression::CONTINUE;
                }
            }
        } finder(needle);
        haystack->depthFirstTraversal(finder);
        return finder.found;
    }

    // If an expression is relative to the stack pointer, then return the offset from the stack pointer, otherwise return null.
    BS::SValue::Ptr stackOffset(const BS::SValue::Ptr &value_) {
        const SymbolicExpression::Ptr value = SValue::promote(value_)->get_expression();
        ASSERT_require(initialSp_->isVariable2());

        Sawyer::Message::Stream debug(mlog[DEBUG]);

        SAWYER_MESG(debug) <<"  stackOffset for address " <<*value <<"\n"
                           <<"    with sp = " <<*initialSp_ <<"\n";

        if (findSubexpression(value, initialSp_)) {
            if (const auto spDelta = function_->stackDeltaAnalysis().instructionInputStackDeltaWrtFunction(currentInstruction())) {
                SAWYER_MESG(debug) <<"    sp delta = " <<*spDelta <<"\n";
                const auto curSp = SymbolicExpression::makeAdd(initialSp_,
                                                               SymSem::SValue::promote(spDelta)->get_expression());
                SAWYER_MESG(debug) <<"    cur sp = " <<*curSp <<"\n";
                const SymbolicExpression::Ptr offset = SymbolicExpression::makeSubtract(value, curSp);
                SAWYER_MESG(debug) <<"    found; offset = " <<*offset <<"\n";
                if (!findSubexpression(offset, initialSp_))
                    return svalueExpr(offset);
            }
        }

        if (findSubexpression(value, initialFp_)) {
            const auto spDelta = function_->stackDeltaAnalysis().instructionInputStackDeltaWrtFunction(currentInstruction());
            const auto fpDelta = function_->stackDeltaAnalysis().instructionInputFrameDelta(currentInstruction());
            if (spDelta && fpDelta) {
                SAWYER_MESG(debug) <<"    fp delta = " <<*fpDelta <<"\n";
                using namespace SymbolicExpression;
                const auto curFp = makeAdd(initialSp_,
                                           makeAdd(SymSem::SValue::promote(spDelta)->get_expression(),
                                                   SymSem::SValue::promote(fpDelta)->get_expression()));
                const auto offset = makeSubtract(value, curFp);
                SAWYER_MESG(debug) <<"    found; offset = " <<*offset <<"\n";
                if (!findSubexpression(offset, initialFp_))
                    return svalueExpr(offset);
            }
        }

        return BS::SValue::Ptr();

#if 0 // OLD STUFF
        if (!findSubexpression(value, initialSp_))
            return BS::SValue::Ptr();                   // value doesn't depend on the initial stack pointer

        const SymbolicExpression::Ptr offset = SymbolicExpression::makeSubtract(value, initialSp_);
        if (findSubexpression(offset, initialSp_))
            return BS::SValue::Ptr();                   // offset still depends on the initial stack pointer

        return svalueExpr(offset);
#endif
    }
};

} // namespace
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void
insertInstructionDependencies(SgAsmInstruction *insn, const Context &ctx) {
    ASSERT_not_null(ctx.cpu);
    SAWYER_MESG(mlog[DEBUG]) <<"inserting " <<insn->toString() <<"\n";
    ctx.cpu->currentState()->clear();
    ctx.cpu->processInstruction(insn);
}

Graph
createFromFunction(const P2::Partitioner::ConstPtr &partitioner, const P2::Function::Ptr &function) {
    ASSERT_not_null(partitioner);
    ASSERT_not_null(function);

    auto ops = Semantics::RiscOperators::instance(partitioner, function);
    auto cpu = partitioner->architecture()->newInstructionDispatcher(ops);
    Context ctx(partitioner, cpu);

    for (const Address bbAddr: function->basicBlockAddresses()) {
        if (P2::BasicBlock::Ptr bb = partitioner->basicBlockExists(bbAddr)) {
            for (SgAsmInstruction *insn: bb->instructions())
                insertInstructionDependencies(insn, ctx);
        }
    }

    return ops->dataDependencyGraph();
}

} // namespace
} // namespace
} // namespace

#endif
