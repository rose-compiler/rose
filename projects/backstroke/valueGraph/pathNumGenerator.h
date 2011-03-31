#ifndef BACKSTROKE_PATHNUMGENERATOR_H
#define	BACKSTROKE_PATHNUMGENERATOR_H

#include "CFGFilter.h"
#include <slicing/backstrokeCFG.h>

namespace Backstroke
{

typedef CFG<BackstrokeCFGNodeFilter> BackstrokeCFG;

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS,
		BackstrokeCFG::Vertex, BackstrokeCFG::Edge> DAG;

class PathNum
{
    typedef boost::graph_traits<DAG> GraphTraits;
	typedef GraphTraits::vertex_descriptor Vertex;
	typedef GraphTraits::edge_descriptor Edge;
    typedef GraphTraits::vertex_iterator VertexIter;
    typedef GraphTraits::edge_iterator EdgeIter;
    typedef GraphTraits::out_edge_iterator OutEdgeIter;
    
public:
    PathNum(const BackstrokeCFG& cfg);
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

    std::map<Vertex, std::vector<int> > pathsForNode_;

public:
    PathNumGenerator(const DAG& dag, Vertex entry, Vertex exit)
            : dag_(dag), entry_(entry), exit_(exit) {}

    //! Assign values to edges then each path has a unique number.
    void getEdgeValues();

    //! Find all paths in this DAG.
    void getAllPaths();

    //! For each vertex in the DAG, find all paths which contain it.
    void getAllPathNumbersForEachNode();

};

} // end of Backstroke

#endif	/* BACKSTROKE_PATHNUMGENERATOR_H */

