#ifndef BACKSTROKE_PATHNUMGENERATOR_H
#define	BACKSTROKE_PATHNUMGENERATOR_H

#include "CFGFilter.h"
#include <slicing/backstrokeCFG.h>

namespace Backstroke
{

typedef CFG<BackstrokeCFGNodeFilter> BackstrokeCFG;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		BackstrokeCFG::Vertex, BackstrokeCFG::Edge> DAG;

class PathNumGenerator;

class PathNumManager
{
    typedef boost::graph_traits<DAG> GraphTraits;
	typedef GraphTraits::vertex_descriptor DAGVertex;
	typedef GraphTraits::edge_descriptor DAGEdge;
    typedef GraphTraits::vertex_iterator DAGVertexIter;
    typedef GraphTraits::edge_iterator DAGEdgeIter;
    typedef GraphTraits::out_edge_iterator DAGOutEdgeIter;

    const BackstrokeCFG& cfg_;

    //! The first DAG is about the function, and others are all loops.
    std::vector<DAG> dags_;

    //! A table from each CFG vertex to the index of the DAG and the DAG vertex.
    std::map<BackstrokeCFG::Vertex, std::pair<int, DAGVertex> > vertexToDagIndex_;

    std::vector<PathNumGenerator*> pathNumGenerators_;

    std::map<SgNode*, BackstrokeCFG::Vertex> nodeCFGVertexMap_;
    
    //std::map<SgNode*, std::pair<int, Vertex> > sgNodeToVertexMap_;

    //std::map<SgNode*, std::pair<int, DAGVertex> > sgNodeToVertexMap_;
    
public:
    PathNumManager(const BackstrokeCFG& cfg);
    ~PathNumManager();

    std::pair<int, std::set<int> > getPathNumbers(SgNode* node) const;

private:
    //! Use path number generator to generate path numbers.
    void generatePathNumbers();

    //! Build a table from each SgNode to its belonging CFG vertex.
    void buildNodeCFGVertexMap();
};



//! Given a DAG with its entry and exit, this class generates path numbers on its edges and vertices.
class PathNumGenerator
{
    typedef boost::graph_traits<DAG> GraphTraits;
	typedef GraphTraits::vertex_descriptor Vertex;
	typedef GraphTraits::edge_descriptor Edge;
    typedef GraphTraits::out_edge_iterator OutEdgeIter;
    
    
    typedef std::vector<Edge> Path;

    //! The DAG on which we'll add path numbers.
    const DAG& dag_;

    //! The entry of the dag. This may be the entry of the CFG, or the header of a loop.
    Vertex entry_;

    //! The exit of the dag. This may be the exit of the CFG, or the exit of a loop.
    //! An exit of a loop is not a node in the CFG, but a virtual node representing outside the loop.
    Vertex exit_;

    //! A table mapping each vertex to the number of paths on it.
    std::map<Vertex, int> pathNumbers_;

    //! A table mapping each edge to the number attached to it.
    std::map<Edge, int> edgeValues_;

    //! All paths in this DAG. The i_th path has the path number i.
    std::vector<Path> paths_;

    std::map<Vertex, std::set<int> > pathsForNode_;

public:
    PathNumGenerator(const DAG& dag, Vertex entry, Vertex exit)
            : dag_(dag), entry_(entry), exit_(exit) {}

    //! Assign values to edges then each path has a unique number.
    void getEdgeValues();

    //! Find all paths in this DAG.
    void getAllPaths();

    //! For each vertex in the DAG, find all paths which contain it.
    void getAllPathNumbersForEachNode();

    std::set<int> getPaths(Vertex v) const
    {
        ROSE_ASSERT(pathsForNode_.count(v) > 0);
        return pathsForNode_.find(v)->second;
    }
};

} // end of Backstroke

#endif	/* BACKSTROKE_PATHNUMGENERATOR_H */

