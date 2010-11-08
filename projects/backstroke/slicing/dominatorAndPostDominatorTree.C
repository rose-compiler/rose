#include <rose.h>
#include "backstrokeCFG.h"
#include "backstrokeCDG.h"
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/bind.hpp>

using namespace std;
using namespace boost;


#define foreach BOOST_FOREACH

#define BACKSTROKE_DEBUG


class DominatorTreeBuilder
{
public:
    //typedef std::map<SgGraphNode*, set<SgGraphNode*> > NodeNodesMap;
    //typedef std::map<SgGraphNode*, SgGraphNode*> NodeNodeMap;

	//typedef VirtualCFG::CFGNode CFGNode;
	//typedef Backstroke::FullCFG::Vertex CFGVertex;
    typedef adjacency_list<vecS, vecS, directedS, CFGNode> DominatorTree;


private:
    //! The dominator tree represented by a boost::graph
    DominatorTree dominatorTree_;

    typedef graph_traits<DominatorTree>::vertex_descriptor Node;
    //typedef property_map<DominatorTree, vertex_name_t>::type NodeSgNodeMap;

	void writeTreeNode(std::ostream& out, const Node& node)
	{
		Backstroke::writeCFGNode(out, dominatorTree_[node]);
	}

public:
	template <class CFGType>
    void build(const CFGType& cfg);

    const DominatorTree& getDominatorTree() const { return dominatorTree_; }

    void toDot(const std::string& filename) const;
};

template <class DomTreeType>
void writeTreeNode(std::ostream& out, const typename boost::graph_traits<DomTreeType>::vertex_descriptor& node, const DomTreeType& domTree)
{
	Backstroke::writeCFGNode(out, domTree[node]);
}


template <class CFGType, class VertexT, class ContainerT>
void buildTree(const std::map<VertexT, ContainerT>& vertexMap, const CFGType& cfg, const string& filename)
{
#ifdef BACKSTROKE_DEBUG

	typedef adjacency_list<vecS, vecS, directedS, typename CFGType::CFGNodeType> Tree;
	//typedef typename boost::graph_traits<Tree>::vertex_descriptor VertexT;
    Tree tree;

    std::map<VertexT, VertexT> verticesAdded;

    typedef typename map<VertexT, ContainerT>::value_type VVS;
    foreach (const VVS& vertices, vertexMap)
    {
        VertexT from = vertices.first;
		
        VertexT src, tar;
        typename std::map<VertexT, VertexT>::iterator it;
        bool inserted;

        // Add the first node.
        tie(it, inserted) = verticesAdded.insert(make_pair(from, VertexT()));
        if (inserted)
        {
            src = add_vertex(tree);
            tree[src] = cfg[from];
            it->second = src;
        }
        else
            src = it->second;


		foreach (VertexT to, vertices.second)
		{
			// Add the second node.
			tie(it, inserted) = verticesAdded.insert(make_pair(to, VertexT()));
			if (inserted)
			{
				tar = add_vertex(tree);
				tree[tar] = cfg[to];
				it->second = tar;
			}
			else
				tar = it->second;

			// Add the edge.
			add_edge(src, tar, tree);
		}
    }

    ofstream out(filename.c_str());
    write_graphviz(out, tree, bind(writeTreeNode<Tree>, _1, _2, tree));

#endif
}

template <class CFGType, class VertexT>
void buildTree(const std::map<VertexT, VertexT>& vertexMap, const CFGType& cfg, const string& filename)
{
#ifdef BACKSTROKE_DEBUG

	map<VertexT, std::vector<VertexT> > vertexMap2;
	typedef typename std::map<VertexT, VertexT>::value_type VV;
	foreach (const VV& vv, vertexMap)
		vertexMap2[vv.first].push_back(vv.second);
	buildTree(vertexMap2, cfg, filename);

#endif
}

template <class CFGType>
void buildDominatorTree(const CFGType& cfg, bool postDom = false)
{
	typedef typename CFGType::Vertex VertexT;

	map<VertexT, VertexT> iDom;
	if (postDom)
		iDom = cfg.buildPostdominatorTree();
	else
		iDom = cfg.buildDominatorTree();

	cout << "Draw the graph." << endl;
	buildTree(iDom, cfg, "dominatorTree.dot");

	//cout << "Draw the graph." << endl;
	//buildDominanceFrontiers(iDom, cfg);
    // Start to build the dominator tree.
    //NodeSgNodeMap node_to_sgnode = get(vertex_name, dominatorTree_);
}


template <class VertexT>
void appendSuccessors(
		VertexT v,
		std::vector<VertexT>& vertices,
		const std::map<VertexT, std::set<VertexT> >& iSucc)
{
	vertices.push_back(v);

	typename map<VertexT, std::set<VertexT> >::const_iterator
	iter = iSucc.find(v);
	if (iter != iSucc.end())
	{
		ROSE_ASSERT(!iter->second.empty());

		foreach (VertexT succ, iter->second)
			appendSuccessors(succ, vertices, iSucc);
	}
}

template <class VertexT, class CFGType>
std::map<VertexT, std::set<VertexT> >
buildDominanceFrontiers(const std::map<VertexT, VertexT>& iDom, const CFGType& cfg)
{
	typedef typename map<VertexT, VertexT>::value_type VV;
	typedef typename map<VertexT, std::set<VertexT> >::value_type VVS;
	
	// Find immediate children in the dominator tree for each node.
	std::map<VertexT, std::set<VertexT> > children;
	foreach (const VV& vv, iDom)
	{
		children[vv.second].insert(vv.first);
	}
	
	VertexT entry = cfg.getEntry();
	ROSE_ASSERT(children.find(entry) != children.end());

	// Use a stack to make a bottom-up traversal of the dominator tree.
	std::vector<VertexT> vertices;
	appendSuccessors(entry, vertices, children);

	buildTree(children, cfg, "immediateChildren.dot");

#if 0
	// Find children nodes for each node.
	std::map<VertexT, std::vector<VertexT> > children;
	foreach (VertexT v, vertices)
	{
		std::vector<VertexT>& vs = children[v];
		foreach (VertexT succ, children[v])
			appendSuccessors(succ, vs, children);
	}

	buildTree(children, cfg, "children.dot");
#endif

	map<VertexT, std::set<VertexT> > domFrontiers;

	// Start to build the dominance frontiers.
	while (!vertices.empty())
	{
		VertexT v = vertices.back();
		vertices.pop_back();

		typename boost::graph_traits<CFGType>::adjacency_iterator i, j;
		for (tie(i, j) = adjacent_vertices(v, cfg); i != j; ++i)
		{
			ROSE_ASSERT(iDom.count(*i) > 0);

			if (iDom.find(*i)->second != v)
				domFrontiers[v].insert(*i);
		}

		foreach (VertexT child, children[v])
		{
			foreach (VertexT u, domFrontiers[child])
			{
				ROSE_ASSERT(iDom.count(u) > 0);

				if (iDom.find(u)->second != v)
					domFrontiers[v].insert(u);
			}
		}
	}
	// End build.
	
	cout << "Build complete!" << endl;

	buildTree(domFrontiers, cfg, "dominanceFrontiers.dot");
	return domFrontiers;
}

int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* sageProject = frontend(argc,argv);

  // Process all function definition bodies for static control flow graph generation
  Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(sageProject, V_SgFunctionDefinition);
  for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
  {
    SgFunctionDefinition* proc = isSgFunctionDefinition(*i);
    ROSE_ASSERT (proc != NULL);

	//Backstroke::FullCFG cfg(proc);
	Backstroke::FilteredCFG cfg(proc);
	Backstroke::FilteredCFG rvsCfg = cfg.makeReverseCopy();
	rvsCfg.toDot("CFG.dot");
	buildDominatorTree(rvsCfg);

	Backstroke::CDG<Backstroke::FilteredCFG> cdg(cfg);
	cdg.toDot("CDG.dot");

	//Backstroke::FilteredCFG rvsCfg = cfg.makeReverseCopy();
	//rvsCfg.toDot("temp.dot");

	//cout << "CFG is built." << endl;

    //buildDominatorTree(rvsCfg);

	return 0;
  }

  return 0;
}