#ifndef BACKSTROKE_PATHNUMGENERATOR_H
#define	BACKSTROKE_PATHNUMGENERATOR_H

#include "types.h"

namespace Backstroke
{
    
//typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
//    BackstrokeCFG::Vertex, BackstrokeCFG::Edge> DAG;

typedef BackstrokeCFG DAG;
    
    
class PathNumManager;

//! Given a DAG with its entry and exit, this class generates path numbers on its edges and vertices.
class PathNumGenerator
{
	//typedef boost::graph_traits<DAG>::vertex_descriptor Vertex;
	//typedef boost::graph_traits<DAG>::edge_descriptor Edge;
    
    typedef BackstrokeCFG::Vertex Vertex;
    typedef BackstrokeCFG::Edge   Edge;
    
    typedef BackstrokeCFG::Vertex CFGVertex;
    typedef BackstrokeCFG::Edge   CFGEdge;

    typedef std::vector<Edge> Path;

    // The path set on each vertex contains what path number this vertex can see
    // and the corresponding path set of this number. The union of all path sets
    // for all numbers is also given.
    struct PathSetOnVertex
    {
        // Each visible incomplete path number and its corresponding path set.
        std::map<int, PathSet> numToPath;

        // The path set on this vertex, which should be the union of all path set above.
        PathSet allPath;
    };

    //! The DAG on which we'll add path numbers.
    const DAG& dag_;
    
    //! The CFG.
    const BackstrokeCFG* cfg_;

    //! A table mapping each vertex to the number of paths on it.
    std::map<Vertex, size_t> pathNumbersOnVertices_;

    //! A table mapping each edge to the number attached to it.
    std::map<Edge, size_t> edgeValues_;

    //! All paths in this DAG. The i_th path has the path number i.
    std::vector<Path> paths_;
    
    //! All paths numbers for the paths above.
    std::vector<size_t> pathNumbers_;

    //std::map<Vertex, PathSetOnVertex> pathsForNode_;
    std::map<Vertex, PathSet> pathsForNode_;
    std::map<Edge,   PathSet> pathsForEdge_;
    //std::map<Vertex, std::set<int> > pathsForNode_;


    //! For each CFG node, this table stores all its control dependent nodes and 
    //! corresponding edges.
    std::map<Vertex, std::map<Vertex, std::vector<Edge> > > 
    controlDependences_;
        
        
    //! Get access to edgeValues_.
    friend class PathNumManager;

public:
    PathNumGenerator(const DAG& dag, const BackstrokeCFG* cfg)
        : dag_(dag), cfg_(cfg)
    {}

    //! Returns the visible incomplete path numbers and their corresponding complete 
    //! paths set of the given DAG node.
    std::map<int, PathInfos> getVisibleNumAndPaths(Vertex v) const
    {
        return std::map<int, PathInfos>();
        //ROSE_ASSERT(pathsForNode_.count(v) > 0);
        //return pathsForNode_.find(v)->second.numToPath;
    }

    PathSet getPaths(Vertex v) const
    {
        ROSE_ASSERT(pathsForNode_.count(v) > 0);
        //return pathsForNode_.find(v)->second.allPath;
        return pathsForNode_.find(v)->second;
    }

    PathSet getPaths(const Edge& e) const
    {
        ROSE_ASSERT(pathsForEdge_.count(e) > 0);
        return pathsForEdge_.find(e)->second;
    }
    
    //! Given a path index, return the value of this path.
    size_t getPathNumber(size_t idx) const
    { 
        ROSE_ASSERT(idx < pathNumbers_.size());
        return pathNumbers_[idx]; 
    }
    
    void generatePathNumbers()
    {
        generateEdgeValues();
        generateControlDependences();
        generateAllPaths();
        generateAllPathNumForNodesAndEdges();
    }
    
    size_t getNumberOfPath() const { return paths_.size(); }

private:
    //! Assign values to edges then each path has a unique number.
    void generateEdgeValues();
    
    //! Generate control dependences for all DAG nodes.
    void generateControlDependences();

    //! Find all paths in this DAG.
    void generateAllPaths();

    //! For each vertex in the DAG, find all paths which contain it.
    void generateAllPathNumForNodesAndEdges();
};

class PathNumManager
{
    //typedef boost::graph_traits<DAG>::vertex_descriptor DAGVertex;
    //typedef boost::graph_traits<DAG>::edge_descriptor   DAGEdge;
    
    typedef BackstrokeCFG::Vertex DAGVertex;
    typedef BackstrokeCFG::Edge   DAGEdge;
    
    typedef BackstrokeCFG::Vertex CFGVertex;
    typedef BackstrokeCFG::Edge   CFGEdge;

    const BackstrokeCFG* cfg_;
    
    Backstroke::FullCFG fullCfg_;

    //! The first DAG is about the function, and others are all loops.
    std::vector<DAG> dags_;
    
    std::vector<FullCFG> auxDags_;

    //! A DAG including all CFG nodes and is used to get the path numbers.
    DAG superDag_;
    
    //! All back edges in the CFG.
    std::set<CFGEdge> backEdges_;
    
    //! All loops in the CFG.
    std::map<CFGVertex, std::set<CFGVertex> > loops_;
    
    //! A table from each CFG vertex to the index of the DAG and the DAG vertex.
    std::map<CFGVertex, std::map<int, DAGVertex> > vertexToDagIndex_;

    //! A table from each CFG edge to the index of the DAG and the DAG vertex.
    std::map<CFGEdge, std::map<int, DAGEdge> > edgeToDagIndex_;

    std::vector<PathNumGenerator*> pathNumGenerators_;

    std::map<SgNode*, BackstrokeCFG::Vertex> nodeCFGVertexMap_;
    
    //! Each element consists of the path index of the parent 
    //! region, and the number of paths.
    std::vector<std::pair<int, int> > pathInfo_;
    
    //! A table mapping each loop header to its DAG index.
    std::map<CFGVertex, int> headerToDagIndex_;
    
    //std::map<SgNode*, std::pair<int, Vertex> > sgNodeToVertexMap_;

    //std::map<SgNode*, std::pair<int, DAGVertex> > sgNodeToVertexMap_;
    
public:
    PathNumManager(const BackstrokeCFG* cfg);
    ~PathNumManager();

    void buildSuperDAG();
    
    //! A work around.
    void buildAuxiliaryDags();
    
    //! A recursive function to get all control dependences related information.
    void getPathNumsAndEdgeValues(
        int dagIndex, DAGVertex dagNode, 
        const std::map<int, int>& values,
        std::vector<std::map<int, int> >& results) const;
    
    //! Get path numbers from a AST node.
    PathInfos getPathNumbers(SgNode* node) const;

    //! Get path numbers from two AST nodes, which form a CFG edge.
    PathInfos getPathNumbers(SgNode* node1, SgNode* node2) const;

    //! Get visible incomplete path numbers and their cooresponding complete path numbers.
    //! The first member in the returned pair is the DAG index, and the second is a table
    //! from each incomplete visible path number to its corresponding path set.
    std::pair<int, std::map<int, PathSet> >
    getVisiblePathNumbers(SgNode* node) const;

    //! Given a DAG index, return how many paths it has.
    size_t getNumberOfPath(size_t index) const
    { return pathInfo_[index].second; }
    
    //! Given a DAG index and a path index, return the path number.
    size_t getPathNumber(size_t dagIndex, size_t pathIndex) const
    { return pathNumGenerators_[dagIndex]->getPathNumber(pathIndex); }
    
    //! Given a DAG index, return a path set with all 1 (all paths).
    PathSet getAllPaths(size_t index) const
    {
        PathSet paths(getNumberOfPath(index));
        return paths.flip();
    }
    
    //! For all DAGs, return a path set with all 1 (all paths).
    PathInfos getAllPaths() const;
    
    size_t getDagNum() const { return dags_.size(); }
    
    int getLoopDagIndex(SgNode* header) const
    { return headerToDagIndex_.find(getCFGNode(header))->second; }
    
    size_t getNumberOfDags() const { return dags_.size(); }
    
    //! Get a table from each AST node to a number representing the topological order
    //! in the CFG.
    void getAstNodeIndices(size_t index, std::map<SgNode*, int>& nodeIndicesTable) const;
    
    //! Get a table from each path set to a number representing the topological order
    //! in the CFG.
    //std::map<PathSet, int> getPathsIndices(size_t index) const;

    void insertPathNumToFwdFunc();
    void insertLoopCounterToFwdFunc(const std::set<SgNode*>& loopHeaders);
    
    //! Build a graph in dot file representing the given DAG.
    void dagToDot(const DAG& dag, const std::string& filename);

private:
    //! Use path number generator to generate path numbers.
    void generatePathNumbers();

    //! Build a table from each SgNode to its belonging CFG vertex.
    void buildNodeCFGVertexMap();

    //! Given a AST node, find its belonged CFG node.
    CFGVertex getCFGNode(SgNode* node) const;

    //! Check if the given node is a data member declaration.
    bool isDataMember(SgNode* node) const;

    //! Insert a path number update statement on the given CFG edge.
    void insertPathNumberOnEdge(
            SgNode* src, SgNode* tgt,
            const std::string& pathNumName,
            int val);
    
    //! For a loop counter, insert ++counter on the given CFG edge.
    void insertLoopCounterIncrOnEdge(
            SgNode* src, SgNode* tgt,
            const std::string& pathNumName,
            const std::string& counterName);
    
    //! For a loop counter, insert push(counter) on the given CFG edge.
    void insertLoopCounterPushOnEdge(
            SgNode* src, SgNode* tgt,
            const std::string& pathNumName);
};

#if 0
class PredicateManager
{
    typedef boost::graph_traits<DAG>::vertex_descriptor DAGVertex;
    typedef boost::graph_traits<DAG>::edge_descriptor   DAGEdge;
    typedef BackstrokeCFG::Vertex                       CFGVertex;
    typedef BackstrokeCFG::Edge                         CFGEdge;

    const BackstrokeCFG* cfg_;
    const BackstrokeCDG* cdg_;

    //! The first DAG is about the function, and others are all loops.
    std::vector<DAG> dags_;

    //! A table from each CFG vertex to the index of the DAG and the DAG vertex.
    std::map<CFGVertex, std::pair<int, DAGVertex> > vertexToDagIndex_;

    //! A table from each CFG edge to the index of the DAG and the DAG vertex.
    std::map<CFGEdge, std::pair<int, DAGEdge> > edgeToDagIndex_;

    std::map<SgNode*, BackstrokeCFG::Vertex> nodeCFGVertexMap_;
    
    //! Each element consists of the path index of the parent 
    //! region, and the number of paths.
    std::vector<std::pair<int, int> > pathInfo_;
    
    //std::map<SgNode*, std::pair<int, Vertex> > sgNodeToVertexMap_;

    //std::map<SgNode*, std::pair<int, DAGVertex> > sgNodeToVertexMap_;
    
public:
    PredicateManager(const BackstrokeCFG* cfg);
    ~PredicateManager(); 
};
#endif

} // end of Backstroke

#endif	/* BACKSTROKE_PATHNUMGENERATOR_H */

