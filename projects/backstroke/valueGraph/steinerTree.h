#ifndef BACKSTROKE_STEINERTREE_H
#define	BACKSTROKE_STEINERTREE_H

#include <rose.h>
#include <boost/foreach.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#include "valueGraph.h"
//#include <boost/timer.hpp>

#define foreach BOOST_FOREACH

namespace Backstroke
{


// Generates all n combinations from m.
std::vector<std::vector<int> > generateCombination(int m, int n);


#if 1


struct SteinerTreeBuilder
{
	typedef boost::graph_traits<ValueGraph>::vertex_descriptor Vertex;
	typedef boost::graph_traits<ValueGraph>::edge_descriptor Edge;
	typedef boost::associative_property_map<std::map<Edge, int> > WeightMapT;
	
	typedef std::map<std::pair<Vertex, std::vector<Vertex> >, int> Table;

	Table S, U;
	std::vector<std::vector<int> > distMatrix;

	Vertex root;
	//Graph valueGraph;
	//std::set<Vertex> terminals;

	static const int INTMAX = INT_MAX / 3;

	int& get(Table& t, Vertex v, const std::vector<Vertex>& vertices)
	{
		return t[std::make_pair(v, vertices)];
	}

	int buildSteinerTree(const ValueGraph& valueGraph, Vertex r, const std::set<Vertex>& terminals);

	void preBuild(const ValueGraph& g, const std::set<Vertex>& terminals, const WeightMapT& weightMap);

	int build(const ValueGraph& g, const std::set<Vertex>& terminals, const WeightMapT& weightMap);

	void getPartitions(int m, const std::vector<Vertex>& vertices,
		std::vector<std::vector<Vertex> >& Es, std::vector<std::vector<Vertex> >& D_Es);
};
#endif

template <class Graph, class Vertex, class Map>
int SteinerTree(const Graph& g, Vertex r, const std::set<Vertex>& terms, const Map& weightMap)
{
	const int INTMAX = INT_MAX / 3;

	// Get all-pair shortest path.
	int num = boost::num_vertices(g);
	std::vector<Vertex> terminals(terms.begin(), terms.end());

	std::vector<std::vector<int> > distMatrix(num);
	foreach (std::vector<int>& vec, distMatrix)
		vec.resize(num);
	boost::johnson_all_pairs_shortest_paths(g, distMatrix, boost::weight_map(weightMap));

	foreach (std::vector<int>& vec, distMatrix)
	{
		foreach (int& d, vec)
		{
			if (d == INT_MAX)
				d = INTMAX;
		}
	}

	std::vector<Vertex> allVertices;
	typename boost::graph_traits<Graph>::vertex_iterator v, w;
	for (boost::tie(v, w) = boost::vertices(g); v != w; ++v)
	{
		if (distMatrix[r][*v] < INTMAX)
			allVertices.push_back(*v);
	}

	//std::cout << "allVertices:" << allVertices.size() << std::endl;

	std::map<std::pair<Vertex, std::vector<Vertex> >, int> S;
	foreach (Vertex t, terminals)
	{
		foreach (Vertex j, allVertices)
		{
			std::vector<Vertex> s(1, t);
			S[std::make_pair(j, s)] = distMatrix[j][t];
		}
	}

	for (size_t m = 2; m < terminals.size(); ++m)
	{
		std::vector<std::vector<int> > indices = generateCombination(terminals.size(), m);
		foreach (const std::vector<int>& idx, indices)
		{
			std::vector<Vertex> D;
			foreach (int i, idx)
				D.push_back(terminals[i]);

			// Produce all Es
			int size = (1 << (m - 1)) - 1;
			std::vector<std::vector<Vertex> > Es(size), D_Es(size);
			int x = 0;

			for (size_t i = 0; i < m - 1; ++i)
			{
				std::vector<std::vector<int> > indices = generateCombination(m - 1, i);
				foreach (const std::vector<int>& idx, indices)
				{
					std::vector<Vertex> E(idx.size() + 1);
					E[0] = D[0];
					for (int j = 0; j < idx.size(); ++j)
						E[j+1] = D[idx[j]+1];

					std::vector<Vertex> D_E(D.size() - E.size());
					std::set_difference(D.begin(), D.end(), E.begin(), E.end(), D_E.begin());

					Es[x].swap(E);
					D_Es[x].swap(D_E);

					++x;
				}
			}

			ROSE_ASSERT(x == size);

			foreach (Vertex v, allVertices)
				S[std::make_pair(v, D)] = INTMAX;
			
			foreach (Vertex v, allVertices)
			{

				int u = INTMAX;

				for (int i = 0; i < Es.size(); ++i)
				//foreach (const std::vector<Vertex>& E, Es)
				{
					//std::vector<Vertex> diff(D.size() - E.size());
					//std::set_difference(D.begin(), D.end(), E.begin(), E.end(), diff.begin());
					u = std::min(u, S[std::make_pair(v, Es[i])] + S[std::make_pair(v, D_Es[i])]);
				}

				foreach (Vertex w, allVertices)
				{
					int& s = S[std::make_pair(w, D)];
					s = std::min(s, distMatrix[w][v] + u);
				}

			}
		}
	}

//	typedef pair<pair<Vertex, vector<Vertex> >, int> P;
//	foreach (const P& p, S)
//		cout << "* " << p.second << endl;

	int result = INTMAX;
	foreach (Vertex v, allVertices)
	{
		int u = INTMAX;

		// Produce all Es
		std::vector<std::vector<Vertex> > Es;
		for (size_t i = 0; i < terminals.size() - 1; ++i)
		{
			std::vector<std::vector<int> > indices = generateCombination(terminals.size() - 1, i);
			foreach (const std::vector<int> idx, indices)
			{
				std::vector<Vertex> vers(1, terminals[0]);
				foreach (int j, idx)
					vers.push_back(terminals[j + 1]);
				Es.push_back(vers);
			}
		}

		foreach (const std::vector<Vertex>& E, Es)
		{
			std::vector<Vertex> diff(terminals.size() - E.size());
			std::set_difference(terminals.begin(), terminals.end(), E.begin(), E.end(), diff.begin());
			u = std::min(u, S[std::make_pair(v, E)] + S[std::make_pair(v, diff)]);
		}

		result = std::min(result, distMatrix[r][v] + u);
	}

	return result;
}


template <class Graph, class WeightMap>
struct SteinerTreeFinder
{
	typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
	typedef std::set<std::pair<Vertex, Vertex> > SteinerTreeType;
	typedef std::pair<Vertex, Vertex> VertexPair;

	const Graph& g;
	const std::set<Vertex>& terminals;
	std::vector<std::vector<int> > distMatrix;
	int iteration;
	Vertex root;

	SteinerTreeFinder(
		const Graph& graph,
		Vertex r,
		const std::set<Vertex>& terms,
		int iter,
		const WeightMap& weightMap)
			: g(graph), root(r), terminals(terms), iteration(iter)
	{
		int num = boost::num_vertices(g);
		distMatrix.resize(num);
		foreach (std::vector<int>& vec, distMatrix)
			vec.resize(num);
		boost::johnson_all_pairs_shortest_paths(g, distMatrix, boost::weight_map(weightMap));

		// Transpose the distance matrix since we don't want to reverse the graph.
		for (int i = 0; i < num; ++i)
			for (int j = 0; j < i; ++j)
				std::swap(distMatrix[i][j], distMatrix[j][i]);
	}


	float getDensity(const SteinerTreeType& t, const std::set<Vertex>& terms) const
	{
		int dist = 0;
		std::set<Vertex> reached;
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

	std::pair<SteinerTreeType, int> GetSteinerTree() const
	{
		SteinerTreeType steinerTree = SteinerTree(iteration, terminals, terminals.size(), root);
		
		int dist = 0;
		foreach (const VertexPair& vertices, steinerTree)
		{
			//cout << vertices.first << "," << vertices.second << endl;
			dist += distMatrix[vertices.first][vertices.second];
		}
		return std::make_pair(steinerTree, dist);
	}

	SteinerTreeType SteinerTree(int iter, std::set<Vertex> terms, int k, Vertex r) const
	{
		SteinerTreeType t;

		int num = 0;
		foreach (Vertex v, terms)
		{
			if (distMatrix[r][v] < INT_MAX)
				++num;
		}

	#if 0
		foreach (Vertex v, terms)
		std::cout << v << ' ';
		std::cout << std::endl;
		std::cout << "k:" << k << " num:" << num << " r:" << r << " iter:" << iter << std::endl;
		//getchar();
	#endif

		if (num < k)
			return t;

		if (iter == 1)
		{
			std::vector<std::pair<int, Vertex> > distVec;
			foreach (Vertex v, terms)
				distVec.push_back(std::make_pair(distMatrix[r][v], v));
			std::sort(distVec.begin(), distVec.end());
			for (int i = 0; i < k; ++i)
			{
				if (distVec[i].first < INT_MAX)//r != distVec[i].second &&
					t.insert(std::make_pair(r, distVec[i].second));
			}
			return t;
		}

		while (k > 0)
		{
			SteinerTreeType tBest;

			typename boost::graph_traits<Graph>::vertex_iterator v, w;
			for (boost::tie(v, w) = boost::vertices(g); v != w; ++v)
			{
				if (/**v == r || */distMatrix[r][*v] == INT_MAX)
					continue;

				for (int l = 1; l <= k; ++l)
				{
					//std::cout << "l:" << l << " k:" << k << " r:" << r << " iter:" << iter<< std::endl;

					SteinerTreeType tNew = SteinerTree(iter - 1, terms, l, *v);
					tNew.insert(std::make_pair(r, *v));


					//std::cout << getDensity(tBest, terms) << ' ' << getDensity(tNew, terms) << std::endl;

					if (getDensity(tBest, terms) > getDensity(tNew, terms))
					{
						//std::cout << "!!!!!!" << r << ',' << *v << std::endl;
						tBest = (tNew);//tBest.swap(tNew);
					}
				}
			}

//				foreach (const VertexPair& vertices, tBest)
//				{
//					std::cout << vertices.first << "," << vertices.second << std::endl;
//				}

			//std::cout << "Update k: " << k << ' ';

			foreach (const VertexPair& vertices, tBest)
			{
				typename std::set<Vertex>::iterator iter = terms.find(vertices.second);
				if (iter != terms.end())
				{
					--k;
					terms.erase(iter);
				}
				t.insert(vertices);
			}

			//std::cout << k << std::endl;
		}
		return t;
	}

};


} // End of namespace Backstroke


#endif	/* BACKSTROKE_STEINERTREE_H */

