#include "util.h"
#include <boost/foreach.hpp>
#include <boost/graph/strong_components.hpp>

#define foreach BOOST_FOREACH

namespace SystemDependenceGraph
{

namespace
{
    void appendSuccessors(
            ControlFlowGraph::Vertex v,
            std::vector<ControlFlowGraph::Vertex>& vertices,
            const std::map<ControlFlowGraph::Vertex, std::set<ControlFlowGraph::Vertex> >& iSucc)
    {
        vertices.push_back(v);

        std::map<ControlFlowGraph::Vertex, std::set<ControlFlowGraph::Vertex> >::const_iterator
            iter = iSucc.find(v);
        if (iter != iSucc.end())
        {
            ROSE_ASSERT(!iter->second.empty());

            foreach (ControlFlowGraph::Vertex succ, iter->second)
                appendSuccessors(succ, vertices, iSucc);
        }
    }
    
    bool checkCycle(const ControlFlowGraph& cfg)
    {
        // Add an edge from the exit to entry in the CFG, then count the number of
        // strongly connected components. If the number is greater than 1, there is
        // a cycle in the CFG which has no exit.
        ControlFlowGraph cfgCopy = cfg;
        boost::add_edge(cfgCopy.getExit(), cfgCopy.getEntry(), cfgCopy);

        std::vector<int> component(num_vertices(cfgCopy));
        int num = boost::strong_components(cfgCopy, &component[0]);

        return num == 1;
    }
}


//! Build dominance frontiers for all nodes in the given CFG.

DominanceFrontiersT buildDominanceFrontiers(const ControlFlowGraph& cfg)
{
	typedef ControlFlowGraph::Vertex CFGVertexT;
	typedef ControlFlowGraph::Edge   CFGEdgeT;
    
    
    if (!checkCycle(cfg))
	{
		std::cerr << "This function may contain an infinite loop "
				"inside so that its CDG cannot be built" << std::endl;
		throw;
	}
    
    
    std::map<CFGVertexT, CFGVertexT> iDom = cfg.getDominatorTree();

	typedef std::map<CFGVertexT, CFGVertexT>::value_type VertexVertexMap;
	// Find immediate children in the dominator tree for each node.
	std::map<CFGVertexT, std::set<CFGVertexT> > children;
	foreach (const VertexVertexMap& vv, iDom)
		children[vv.second].insert(vv.first);

	CFGVertexT entry = cfg.getEntry();
	ROSE_ASSERT(children.find(entry) != children.end());

	// Use a stack to make a bottom-up traversal of the dominator tree.
	std::vector<CFGVertexT> vertices;
	appendSuccessors(entry, vertices, children);

	//buildTree(children, cfg, "immediateChildren.dot");

	// Map each node to its control dependence node together with the edge from which we can get the
	// control dependence type (true, false or case).
	DominanceFrontiersT domFrontiers;

	// Start to build the dominance frontiers.
	while (!vertices.empty())
	{
		CFGVertexT v = vertices.back();
		vertices.pop_back();

        foreach (const CFGEdgeT& e, boost::out_edges(v, cfg))
		{
			CFGVertexT succ = boost::target(e, cfg);

			//if (iDom.count(*i) == 0) std::cout << cfg[*i].getNode()->class_name() << std::endl;
			ROSE_ASSERT(iDom.count(succ) > 0);

			if (iDom.find(succ)->second != v)
				domFrontiers[v][succ].push_back(e);
		}

		foreach (CFGVertexT child, children[v])
		{
			typedef std::map<CFGVertexT, std::vector<CFGEdgeT> > VertexEdgesMap;
			foreach (const VertexEdgesMap::value_type& vertexEdges, domFrontiers[child])
			{
				ROSE_ASSERT(iDom.count(vertexEdges.first) > 0);

				if (iDom.find(vertexEdges.first)->second != v)
				{
					foreach (CFGEdgeT edge, vertexEdges.second)
						domFrontiers[v][vertexEdges.first].push_back(edge);
				}
			}
		}
	}
	// End build.

	//cout << "Build complete!" << endl;

	//buildTree(domFrontiers, cfg, "dominanceFrontiers.dot");
	return domFrontiers;
}

}