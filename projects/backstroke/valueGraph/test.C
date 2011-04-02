#include "valueGraph.h"
#include "pathNumGenerator.h"
#include <slicing/backstrokeCFG.h>
#include <slicing/backstrokeCDG.h>
#include <slicing/backstrokeDDG.h>
#include <slicing/backstrokePDG.h>

//#include <boost/graph/graph_traits.hpp>
//#include <boost/graph/graphviz.hpp>
//#include <boost/graph/johnson_all_pairs_shortest.hpp>


using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH


#if 0

class MyGraph : public boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, 
		bool, property<edge_weight_t, int> >
{
};

typedef boost::graph_traits<MyGraph>::vertex_descriptor Vertex;
typedef boost::graph_traits<MyGraph>::edge_descriptor Edge;

typedef pair<Vertex, Vertex> VertexPair;
typedef set<pair<Vertex, Vertex> > SteinerTreeType;

// Generates all n combinations from m.
vector<vector<int> > GenerateCombination(int m, int n)
{
	ROSE_ASSERT(m >= n && m > 0 && n >= 0);
	vector<vector<int> > result;

	if (n == 0)
	{
		result.resize(1);
		return result;
	}

	if (n == 1)
	{
		for (int i = 0; i < m; ++i)
		{
			vector<int> r(1, i);
			result.push_back(r);
		}
		return result;
	}

	if (m == n)
	{
		vector<int> r;
		for (int i = 0; i < m; ++i)
			r.push_back(i);
		result.push_back(r);
		return result;
	}

	for (int i = m - 1; i >= n - 1; --i)
	{
		vector<vector<int> > r = GenerateCombination(i, n - 1);
		foreach (vector<int>& comb, r)
		{
			comb.push_back(i);
			result.push_back(comb);
		}
	}

	return result;
}

int SteinerTree(const MyGraph& g, Vertex r, vector<Vertex> terminals)
{
	// Get all-pair shortest path.
	int num = num_vertices(g);

	vector<vector<int> > distMatrix(num);
	foreach (vector<int>& vec, distMatrix)
		vec.resize(num);
	johnson_all_pairs_shortest_paths(g, distMatrix);

	vector<Vertex> allVertices;
	boost::graph_traits<MyGraph>::vertex_iterator v, w;
	for (tie(v, w) = boost::vertices(g); v != w; ++v)
		allVertices.push_back(*v);
	//sort(allVertices.begin(), allVertices.end());
	sort(terminals.begin(), terminals.end());

	map<pair<Vertex, vector<Vertex> >, int> S;
	foreach (Vertex t, terminals)
	{
		foreach (Vertex j, allVertices)
		{
			vector<Vertex> s(1, t);
			S[make_pair(j, s)] = distMatrix[j][t];
		}
	}

	for (int m = 2; m < terminals.size(); ++m)
	{
		vector<vector<int> > indices = GenerateCombination(terminals.size(), m);
		foreach (const vector<int>& idx, indices)
		{
			vector<Vertex> D;
			foreach (int i, idx)
				D.push_back(terminals[i]);
			
			foreach (Vertex v, allVertices)
			{
				S[make_pair(v, D)] = INT_MAX;

				int u = INT_MAX;

				// Produce all Es
				vector<vector<Vertex> > Es;
				for (int i = 0; i < m - 1; ++i)
				{
					vector<vector<int> > indices = GenerateCombination(m - 1, i);
					foreach (const vector<int> idx, indices)
					{
						vector<Vertex> vers(1, D[0]);
						foreach (int j, idx)
							vers.push_back(D[j + 1]);
						Es.push_back(vers);
					}
				}

				foreach (const vector<Vertex>& E, Es)
				{
					vector<Vertex> diff(D.size() - E.size());
					set_difference(D.begin(), D.end(), E.begin(), E.end(), diff.begin());

					int s1 = S[make_pair(v, E)];
					int s2 = S[make_pair(v, diff)];

					if (s1 != INT_MAX && s2 != INT_MAX)
						u = min(u, s1 + s2);
				}

				foreach (Vertex w, allVertices)
				{
					int d = distMatrix[v][w];
					if (d != INT_MAX && u != INT_MAX)
						S[make_pair(w, D)] = min(S[make_pair(w, D)], distMatrix[v][w] + u);
				}

			}
		}
	}

	typedef pair<pair<Vertex, vector<Vertex> >, int> P;
	foreach (const P& p, S)
		cout << "* " << p.second << endl;

	int result = INT_MAX;
	foreach (Vertex v, allVertices)
	{
		int u = INT_MAX;
		
		// Produce all Es
		vector<vector<Vertex> > Es;
		for (int i = 0; i < terminals.size() - 1; ++i)
		{
			vector<vector<int> > indices = GenerateCombination(terminals.size() - 1, i);
			foreach (const vector<int> idx, indices)
			{
				vector<Vertex> vers(1, terminals[0]);
				foreach (int j, idx)
					vers.push_back(terminals[j + 1]);
				Es.push_back(vers);
			}
		}

		foreach (const vector<Vertex>& E, Es)
		{
			vector<Vertex> diff(terminals.size() - E.size());
			set_difference(terminals.begin(), terminals.end(), E.begin(), E.end(), diff.begin());

			int s1 = S[make_pair(v, E)];
			int s2 = S[make_pair(v, diff)];

			if (s1 != INT_MAX && s2 != INT_MAX)
				u = min(u, s1 + s2);
		}

		int d = distMatrix[r][v];
		if (d != INT_MAX && u != INT_MAX)
			result = min(result, distMatrix[r][v] + u);
	}

	return result;
}

struct SteinerTreeFinder
{
	const MyGraph& g;
	const set<Vertex>& terminals;
	const vector<vector<int> >& distMatrix;
	int iteration;
	Vertex root;

	SteinerTreeFinder(
		const MyGraph& graph,
		Vertex r,
		const set<Vertex>& terms,
		int iter,
		const vector<vector<int> >& distMat)
			: g(graph), root(r), terminals(terms), iteration(iter), distMatrix(distMat)
	{}


	float getDensity(const SteinerTreeType& t, const set<Vertex>& terms) const
	{
		int dist = 0;
		set<Vertex> reached;
		foreach (const VertexPair& vertices, t)
		{
			if (terms.count(vertices.second) > 0)
				reached.insert(vertices.second);
			dist += distMatrix[vertices.first][vertices.second];
		}

		if (reached.empty())
			return INT_MAX;

		return ((float)dist) / reached.size();
	}

	SteinerTreeType GetSteinerTree() const
	{
		return SteinerTree(iteration, terminals, terminals.size(), root);
	}

	SteinerTreeType SteinerTree(int iter, set<Vertex> terms, int k, Vertex r) const
	{
		SteinerTreeType t;

		int num = 0;
		foreach (Vertex v, terms)
		{
			if (distMatrix[r][v] < INT_MAX)
				++num;
		}

	#if 1
		foreach (Vertex v, terms)
		cout << v << ' ';
		cout << endl;
		cout << "k:" << k << " num:" << num << " r:" << r << " iter:" << iter << endl;
		//getchar();
	#endif

		if (num < k)
			return t;

		if (iter == 1)
		{
			vector<pair<int, Vertex> > distVec;
			foreach (Vertex v, terms)
				distVec.push_back(make_pair(distMatrix[r][v], v));
			sort(distVec.begin(), distVec.end());
			for (int i = 0; i < k; ++i)
			{
				if (distVec[i].first < INT_MAX)//r != distVec[i].second &&
					t.insert(make_pair(r, distVec[i].second));
			}
			return t;
		}

		while (k > 0)
		{
			SteinerTreeType tBest;

			boost::graph_traits<MyGraph>::vertex_iterator v, w;
			for (tie(v, w) = boost::vertices(g); v != w; ++v)
			{
				if (/**v == r || */distMatrix[r][*v] == INT_MAX)
					continue;

				for (int l = 1; l <= k; ++l)
				{
					cout << "l:" << l << " k:" << k << " r:" << r << " iter:" << iter<< endl;
					
					SteinerTreeType tNew = SteinerTree(iter - 1, terms, l, *v);
					tNew.insert(make_pair(r, *v));

					
					cout << getDensity(tBest, terms) << ' ' << getDensity(tNew, terms) << endl;
					
					if (getDensity(tBest, terms) > getDensity(tNew, terms))
					{
						cout << "!!!!!!" << r << ',' << *v << endl;
						tBest = (tNew);//tBest.swap(tNew);
					}
				}
			}

				foreach (const VertexPair& vertices, tBest)
				{
					cout << vertices.first << "," << vertices.second << endl;
				}

			cout << "Update k: " << k << ' ';

			foreach (const VertexPair& vertices, tBest)
			{
				set<Vertex>::iterator iter = terms.find(vertices.second);
				if (iter != terms.end())
				{
					--k;
					terms.erase(iter);
				}
				t.insert(vertices);
			}

			cout << k << endl;
		}
		return t;
	}

};

int main()
{
    // declare a graph object
    MyGraph g;

	property_map<MyGraph, edge_weight_t>::type weightmap = get(edge_weight, g);

	weightmap[add_edge(0, 1, g).first] = 3;
	weightmap[add_edge(0, 2, g).first] = 3;
	weightmap[add_edge(1, 2, g).first] = 1;
	weightmap[add_edge(1, 3, g).first] = 5;
	weightmap[add_edge(1, 6, g).first] = 3;
	weightmap[add_edge(2, 3, g).first] = 3;
	weightmap[add_edge(2, 4, g).first] = 3;
	weightmap[add_edge(3, 5, g).first] = 2;
	weightmap[add_edge(3, 4, g).first] = 3;
	weightmap[add_edge(3, 7, g).first] = 4;
	weightmap[add_edge(6, 7, g).first] = 3;
	weightmap[add_edge(4, 5, g).first] = 5;
	weightmap[add_edge(5, 7, g).first] = 1;

	int num = num_vertices(g);
	vector<vector<int> > distMatrix(num);
	foreach (vector<int>& vec, distMatrix)
		vec.resize(num);
	johnson_all_pairs_shortest_paths(g, distMatrix);

	for (int i = 0; i < num; ++i)
	{
		for (int j = 0; j < num; ++j)
			cout << distMatrix[i][j] << '\t';
		cout << endl;
	}

	vector<Vertex> terminals;
	terminals.push_back(4);
	terminals.push_back(5);
	terminals.push_back(6);

	cout << ">>>" << SteinerTree(g, 0, terminals) << endl;
#if 0

	SteinerTreeFinder finder(g, 0, terminals, 3, distMatrix);
	SteinerTreeType steinerTree = finder.GetSteinerTree();

	int dist = 0;
	foreach (const VertexPair& vertices, steinerTree)
	{
		cout << vertices.first << "," << vertices.second << endl;
		dist += distMatrix[vertices.first][vertices.second];
	}
	cout << dist << endl;
#endif

	ofstream ofile("g.dot", std::ios::out);
	write_graphviz(ofile, g);
}


#else

int main(int argc, char *argv[])
{
  // Build the AST used by ROSE
  SgProject* project = frontend(argc,argv);
  SgSourceFile* sourceFile = isSgSourceFile((*project)[0]);

  // Process all function definition bodies for static control flow graph generation
  Rose_STL_Container<SgNode*> functions = NodeQuery::querySubTree(project, V_SgFunctionDefinition);
  for (Rose_STL_Container<SgNode*>::const_iterator i = functions.begin(); i != functions.end(); ++i)
  {
    SgFunctionDefinition* funcDef = isSgFunctionDefinition(*i);
    ROSE_ASSERT (funcDef != NULL);

	if (!funcDef->get_file_info()->isSameFile(sourceFile))
		continue;

    Backstroke::BackstrokeCFG cfg(funcDef);
	cfg.toDot("CFG.dot");
    
	Backstroke::EventReverser reverser(funcDef);
	reverser.buildValueGraph();
//	//reverser.searchValueGraph();
//	reverser.shortestPath();
//	reverser.buildForwardAndReverseEvent();
//	reverser.getPath();

//	Backstroke::ValueGraph reverseVg;
//	// The following function makes a reverse CFG copy.
//	boost::transpose_graph(vg, reverseVg);

	reverser.valueGraphToDot("VG.dot");

//
//    Backstroke::FilteredCFG fullCfg(funcDef);
//	fullCfg.toDot("FullCFG.dot");
//
//    Backstroke::PathNumManager pathNum(cfg);

//	Backstroke::CDG<CFG> cdg(cfg);
//	cdg.toDot("CDG.dot");
//
//	Backstroke::DDG<CFG> ddg(cfg);
//	ddg.toDot("DDG.dot");
//
//	Backstroke::PDG<CFG> pdg(cfg);
//	pdg.toDot("PDG.dot");

	break;
  }

  return backend(project);
}

#endif