#ifndef ROSE_BinaryAnalysis_DataDependence_H
#define ROSE_BinaryAnalysis_DataDependence_H
#include <featureTests.h>
#ifdef ROSE_ENABLE_BINARY_ANALYSIS

#include <Rose/BinaryAnalysis/AbstractLocation.h>
#include <Rose/BinaryAnalysis/BasicTypes.h>

#include <Sawyer/Graph.h>

class SgAsmInstruction;

namespace Rose {
namespace BinaryAnalysis {
namespace DataDependence {

extern Sawyer::Message::Facility mlog;
void initDiagnostics();

class Vertex {
public:
    enum class Type {
        NORMAL,
        EMPTY_SOURCE,
        EMPTY_TARGET,
    };

private:
    Type type_ = Type::NORMAL;
    AbstractLocation location_;
public:
    ~Vertex();
    explicit Vertex(Type);
    explicit Vertex(const AbstractLocation&);
    Vertex(RegisterDescriptor, const RegisterDictionaryPtr&);
    Vertex(const InstructionSemantics::BaseSemantics::SValuePtr &address, size_t nBitsInValue);
    Vertex(RegisterDescriptor baseRegister, const RegisterDictionaryPtr&,
           const InstructionSemantics::BaseSemantics::SValuePtr &registerOffset, size_t nBitsInValue);
    static Vertex emptyTarget();
    static Vertex emptySource();
    Type type() const;
    const AbstractLocation& location() const;
    std::string toString() const;
};

class VertexKey {
    uint64_t hash_ = 0;
public:
    VertexKey() = delete;
    explicit VertexKey(const Vertex&);
    bool operator<(const VertexKey&) const;
};

class Edge {
    SgAsmInstruction *insn_ = nullptr;
public:
    Edge() = delete;
    explicit Edge(SgAsmInstruction*);
    SgAsmInstruction *instruction() const;
};

class Graph: public Sawyer::Container::Graph<Vertex, Edge, VertexKey> {
private:
    using Super = Sawyer::Container::Graph<DataDependence::Vertex, DataDependence::Edge, DataDependence::VertexKey>;
public:
    using Subgraphs = std::map<Address, std::vector<Graph::ConstEdgeIterator>>;

private:
    Partitioner2::PartitionerConstPtr partitioner_;
    ConstVertexIterator emptySource_, emptyTarget_;

public:
    ~Graph();
    Graph() = delete;
    Graph(const Graph&);
    Graph& operator=(const Graph&);
    explicit Graph(const Partitioner2::PartitionerConstPtr&);

public:
    bool edgeExists(VertexIterator, VertexIterator, SgAsmInstruction*) const;
    void toGraphviz(std::ostream&) const;
    ConstVertexIterator emptySource() const;
    ConstVertexIterator emptyTarget() const;
    Architecture::BaseConstPtr architecture() const;

private:
    std::set<Address> controlFlowSuccessors(Address) const;
    Subgraphs makeGraphvizSubgraphs() const;
    void emitGraphvizLocationVertices(std::ostream&) const;
    void emitGraphvizInstructionVertices(std::ostream&, const Subgraphs&) const;
    void emitGraphvizDataFlowEdges(std::ostream&) const;
    void emitGraphvizControlFlowEdges(std::ostream&, const Subgraphs&) const;
};

Graph createFromFunction(const Partitioner2::PartitionerConstPtr&, const Partitioner2::FunctionPtr&);


std::ostream& operator<<(std::ostream&, const Graph&);
    
} // namespace
} // namespace
} // namespace

#endif
#endif
