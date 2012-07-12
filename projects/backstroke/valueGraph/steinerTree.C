#include "steinerTree.h"

#include <boost/graph/dijkstra_shortest_paths.hpp>

namespace Backstroke
{

using namespace std;
using namespace boost;

#define foreach BOOST_FOREACH


// Generates all n combinations from m.
vector<vector<int> > generateCombination(int m, int n)
{
	ROSE_ASSERT(m >= n && m >= 0 && n >= 0);
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
		vector<vector<int> > r = generateCombination(i, n - 1);
		foreach (vector<int>& comb, r)
		{
			comb.push_back(i);
			result.push_back(comb);
		}
	}

	return result;
}

const int SteinerTreeBuilder::INTMAX = INT_MAX / 3;

int SteinerTreeBuilder::buildSteinerTree(
	const ValueGraph& valueGraph,
	SteinerTreeBuilder::Vertex r,
	const set<SteinerTreeBuilder::Vertex>& terminals)
{
	root = r;

	int num = num_vertices(valueGraph);
	distMatrix.resize(num);
	foreach (vector<int>& vec, distMatrix)
		vec.resize(num);

	// First, collect all operator nodes.
	vector<Vertex> opNodes;
	map<Vertex, pair<Vertex, set<Vertex> > > dependencesMap;
	graph_traits<ValueGraph>::vertex_iterator v, w;
	for (tie(v, w) = vertices(valueGraph); v != w; ++v)
	{
		if (isOperatorNode(valueGraph[*v]))
		{
			opNodes.push_back(*v);

			// Find the dependence.
			dependencesMap[*v].first = target(
					*(out_edges(*v, valueGraph).first), valueGraph);
			graph_traits<ValueGraph>::in_edge_iterator e, f;
			for (tie(e, f) = in_edges(*v, valueGraph); e != f; ++e)
				dependencesMap[*v].second.insert(source(*e, valueGraph));
		}
	}


	// Prebuild some necessary structures.
	ValueGraph vg = valueGraph;

	set<Vertex> terms = terminals;
	foreach (Vertex v, opNodes)
	{
		graph_traits<ValueGraph>::in_edge_iterator e, f;
		for (tie(e, f) = in_edges(v, vg); e != f; ++e)
			terms.insert(source(*e, vg));

		Edge edge = *(out_edges(v, vg).first);
		vg[add_edge(root, target(edge, vg), vg).first] = vg[edge];

		clear_vertex(v, vg);
	}

	map<Edge, int> weightMap;
	graph_traits<ValueGraph>::edge_iterator e, f;
	for (tie(e, f) = edges(vg); e != f; ++e)
		weightMap[*e] = vg[*e]->cost;
	preBuild(vg, terms, weightMap);

	cout << "Prebuild complete!" << endl;


	// Then start to traverse all combinations to get the optimal result.

	for (int i = opNodes.size(); i >= 0 ; --i)
	//for (size_t i = 0; i <= opNodes.size(); ++i)
	{
		vector<vector<int> > combs = generateCombination(opNodes.size(), i);

		// for each combination
		foreach (vector<int>& comb, combs)
		{
			// First, check if this combination is valid (no cross dependence).
			map<Vertex, set<Vertex> > dependences;
			bool flag = false;
			foreach (int j, comb)
			{
				pair<Vertex, set<Vertex> >& vp = dependencesMap[opNodes[j]];

				// A value can only be restored by one operator.
				if (dependences.count(vp.first) > 0)
				{
					flag = true;
					break;
				}

				//cout << "::" << vp.first << ':';
				foreach (Vertex v, vp.second)
				{
					//cout << v << ' ';
					if (dependences[v].count(vp.first) > 0)
					{
						flag = true;
						//cout << "\n\n\n";
						goto NEXT;
					}
				}
				//cout << endl;
				dependences.insert(vp);
			}
NEXT:
			if (flag)
				continue;

			ValueGraph vg = valueGraph;

			set<Vertex> terms = terminals;
			foreach (int k, comb)
			{
				graph_traits<ValueGraph>::in_edge_iterator e, f;
				for (tie(e, f) = in_edges(opNodes[k], vg); e != f; ++e)
					terms.insert(source(*e, vg));
			}

			for (size_t j = 0; j < opNodes.size(); ++j)
			{
				if (find(comb.begin(), comb.end(), j) != comb.end())
				{
					Edge e = *(out_edges(opNodes[j], vg).first);
					vg[add_edge(root, target(e, vg), vg).first] = vg[e];
				}
				clear_vertex(opNodes[j], vg);
			}
			
			// Start the search.
			map<Edge, int> weightMap;
			graph_traits<ValueGraph>::edge_iterator e, f;
			for (tie(e, f) = edges(vg); e != f; ++e)
				weightMap[*e] = vg[*e]->cost;

			int res = build(vg, terms, WeightMapT(weightMap));

			cout << i << ' ' << comb.size() << ' ' << res << endl;
			//getchar();
		}
	}
}

void SteinerTreeBuilder::preBuild(
	const ValueGraph& g,
	const set<SteinerTreeBuilder::Vertex>& terms,
	const SteinerTreeBuilder::WeightMapT& weightMap)
{
	// Get all-pair shortest path.
	johnson_all_pairs_shortest_paths(g, distMatrix,
			weight_map(weightMap).distance_inf(INTMAX));

	vector<Vertex> allVertices;
	graph_traits<ValueGraph>::vertex_iterator v, w;
	for (tie(v, w) = vertices(g); v != w; ++v)
	{
		if (distMatrix[root][*v] < INTMAX)
			allVertices.push_back(*v);
	}

	//cout << "allVertices:" << allVertices.size() << endl;

	vector<Vertex> terminals(terms.begin(), terms.end());
	foreach (Vertex t, terminals)
	{
		foreach (Vertex j, allVertices)
		{
			vector<Vertex> s(1, t);
			get(S, j, s) = distMatrix[j][t];
		}
	}

	for (size_t m = 2; m < terminals.size(); ++m)
	{
		vector<vector<int> > indices = generateCombination(terminals.size(), m);
		foreach (const vector<int>& idx, indices)
		{
			vector<Vertex> D(idx.size());
			for (size_t i = 0; i < idx.size(); ++i)
				D[i] = terminals[idx[i]];

			// Produce all Es
			vector<vector<Vertex> > Es, D_Es;
			getPartitions(m, D, Es, D_Es);

			foreach (Vertex v, allVertices)
				get(S, v, D) = INTMAX;

			foreach (Vertex v, allVertices)
			{
				int u = INTMAX;

				for (size_t i = 0; i < Es.size(); ++i)
					u = min(u, get(S, v, Es[i]) + get(S, v, D_Es[i]));

				get(U, v, D) = u;

				foreach (Vertex w, allVertices)
				{
					int& s = get(S, w, D);
					s = min(s, distMatrix[w][v] + u);
				}
			}

			get(U, root, D) = INTMAX;
		}
	}
}

int SteinerTreeBuilder::build(
	const ValueGraph& g,
	const set<SteinerTreeBuilder::Vertex>& terms,
	const SteinerTreeBuilder::WeightMapT& weightMap)
{
	// For each selection of operator nodes, first calculate the shortest path from the root to every other
	// node in the graph, and update the distance matrix.
	dijkstra_shortest_paths(g, root,
			weight_map(weightMap).distance_map(&(distMatrix[root][0])).distance_inf(INTMAX));

	vector<Vertex> allVertices;
	graph_traits<ValueGraph>::vertex_iterator v, w;
	for (tie(v, w) = vertices(g); v != w; ++v)
	{
		if (distMatrix[root][*v] < INTMAX)
			allVertices.push_back(*v);
	}
	
	// Update S.

	vector<Vertex> terminals(terms.begin(), terms.end());
	foreach (Vertex t, terminals)
	{
		vector<Vertex> s(1, t);
		get(S, root, s) = distMatrix[root][t];
	}
	
	for (size_t m = 2; m < terminals.size(); ++m)
	{
		vector<vector<int> > indices = generateCombination(terminals.size(), m);
		foreach (const vector<int>& idx, indices)
		{
			vector<Vertex> D(idx.size());
			for (size_t i = 0; i < idx.size(); ++i)
				D[i] = terminals[idx[i]];

			// Produce all Es
			vector<vector<Vertex> > Es, D_Es;
			getPartitions(m, D, Es, D_Es);

			int s = INTMAX;
			foreach (Vertex v, allVertices)
				s = min(s, distMatrix[root][v] + get(U, v, D));
			for (size_t i = 0; i < Es.size(); ++i)
				s = min(s, get(S, root, Es[i]) + get(S, root, D_Es[i]));

			get(S, root, D) = s;
		}
	}

	vector<vector<Vertex> > Es, D_Es;
	getPartitions(terminals.size(), terminals, Es, D_Es);

	int result = INTMAX;
	foreach (Vertex v, allVertices)
	{
		int u = INTMAX;

		for (size_t i = 0; i < Es.size(); ++i)
			u = min(u, get(S, v, Es[i]) + get(S, v, D_Es[i]));

		result = min(result, distMatrix[root][v] + u);
	}

	return result;
}

void SteinerTreeBuilder::getPartitions(
	int m,
	const vector<SteinerTreeBuilder::Vertex>& vertices,
	vector<vector<SteinerTreeBuilder::Vertex> >& Es,
	vector<vector<SteinerTreeBuilder::Vertex> >& D_Es)
{
	int size = (1 << (m - 1)) - 1;
	Es.resize(size);
	D_Es.resize(size);
	int x = 0;

	for (int i = 0; i < m - 1; ++i)
	{
		vector<vector<int> > indices = generateCombination(m - 1, i);
		foreach (const vector<int>& idx, indices)
		{
			vector<Vertex> E(idx.size() + 1);
			E[0] = vertices[0];
			for (size_t j = 0; j < idx.size(); ++j)
				E[j+1] = vertices[idx[j]+1];

			vector<Vertex> D_E(vertices.size() - E.size());
			set_difference(vertices.begin(), vertices.end(), E.begin(), E.end(), D_E.begin());

			Es[x].swap(E);
			D_Es[x].swap(D_E);

			++x;
		}
	}
	ROSE_ASSERT(x == size);
}

} // End of namespace Backstroke
