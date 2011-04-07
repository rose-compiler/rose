#include "valueGraph.h"
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
//#include <boost/graph/graphviz.hpp>
#include <boost/graph/topological_sort.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>

namespace Backstroke
{

using namespace std;

#define foreach BOOST_FOREACH

#if 0
namespace
{

template <class Graph, class Vertex>
void writeValueGraphNode(const Graph& g, std::ostream& out, const Vertex& node)
{
    out << "[label=\"" << g[node]->toString() << "\"]";
}

template <class Graph, class Edge>
void writeValueGraphEdge(const Graph& g, std::ostream& out, const Edge& edge)
{
    out << "[label=\"" << g[edge]->toString() << "\"]";
}

template <class Graph>
void graphToDot(const Graph& g, const std::string& filename)
{
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;

    // Since the vetices are stored in a list, we have to give each vertex
    // a unique id here.
    int counter = 0;
    map<Vertex, int> vertexIDs;
    foreach (Vertex v, boost::vertices(g))
        vertexIDs[v] = counter++;

    // Turn a std::map into a property map.
    boost::associative_property_map<map<Vertex, int> > vertexIDMap(vertexIDs);

    ofstream ofile(filename.c_str(), std::ios::out);
    boost::write_graphviz(ofile, g,
            boost::bind(&writeValueGraphNode<Graph, Vertex>, g, ::_1, ::_2),
            boost::bind(&writeValueGraphEdge<Graph, Edge>, g, ::_1, ::_2),
            boost::default_writer(), vertexIDMap);
}
}
#endif


bool EventReverser::edgeBelongsToPath(const VGEdge& e, int dagIndex, int pathIndex) const
{
    ValueGraphEdge* edge = valueGraph_[e];
    
    // The ordered edge does not have path information, so trace its
    // corresponding normal edge.
    if (isOrderedEdge(edge))
    {
        VGEdge realEdge = *(boost::in_edges(
                boost::source(e, valueGraph_), valueGraph_).first);
        edge = valueGraph_[realEdge];
    }
    return edge->dagIndex == dagIndex && edge->paths[pathIndex];
}

void EventReverser::getSubGraph(int dagIndex, int pathIndex)
{
    //!!!
    dagIndex = 0;
    
    int pathNum = pathNumManager_.getPathNum(dagIndex);
    for (int i = 0; i < pathNum; ++i)
    {
        //PathEdgeSelector edgeSelector(&valueGraph_, dagIndex, i);
        //SubValueGraph subgraph(valueGraph_, edgeSelector);

        // First, get the subgraph for the path.

        pair<int, int> path = make_pair(dagIndex, i);
        set<VGVertex>& nodes = pathNodesAndEdges_[path].first;
        set<VGEdge>&   edges = pathNodesAndEdges_[path].second;
        foreach (const VGEdge& edge, boost::edges(valueGraph_))
        {
            if (edgeBelongsToPath(edge, dagIndex, pathIndex))
            {
                nodes.insert(boost::source(edge, valueGraph_));
                edges.insert(edge);
            }
        }
        nodes.insert(root_);

        // To resolve the problem of binding an overloaded function.
        set<VGVertex>::const_iterator (set<VGVertex>::*findNode)
                 (const set<VGVertex>::key_type&) const = &set<VGVertex>::find;
        set<VGEdge>::const_iterator (set<VGEdge>::*findEdge)
                 (const set<VGEdge>::key_type&) const = &set<VGEdge>::find;
        SubValueGraph subgraph(valueGraph_,
                               boost::bind(findEdge, &edges, ::_1) != edges.end(),
                               boost::bind(findNode, &nodes, ::_1) != nodes.end());

//        string filename = "VG" + boost::lexical_cast<string>(i);
//        //cout << subgraph << endl;
//        const char* name = "ABCDE";

        SubValueGraph route = getReversalRoute(dagIndex, i,
                                               subgraph, valuesToRestore_);

        generateReverseFunction(route);
        //graphToDot(subgraph, filename);
    }
}

void EventReverser::generateReverseFunction(
        //SgScopeStatement* scope,
        const SubValueGraph& route)
{

    // The following code is needed since the value graph has VertexList=ListS which
    // does not have a vertex_index property, which is needed by topological_sort.
    int counter = 0;
    map<VGVertex, int> vertexIDs;
    foreach (VGVertex v, boost::vertices(route))
        vertexIDs[v] = counter++;
    // Turn a std::map into a property map.
    boost::associative_property_map<map<VGVertex, int> > vertexIDMap(vertexIDs);
    
    vector<VGVertex> nodes;
    boost::topological_sort(route, back_inserter(nodes), vertex_index_map(vertexIDMap));

    foreach (VGVertex node, nodes)
    {
        if (node == root_)
        {
            foreach (const VGEdge& edge, boost::in_edges(node, route))
            {
                if (route[edge]->cost == 0) continue;
                VGVertex src = boost::source(edge, route);
                cout << "State saving: " << route[src]->toString() << endl;
            }
            continue;
        }

        if (OperatorNode* opNode = isOperatorNode(route[node]))
        {
//            VGEdge e = *(boost::in_edges(node, route).first);
//            VGVertex src = boost::source(e, route);
//            ValueNode* valNode = isValueNode(route[src]);
             cout << "Operation: " << opNode->toString() << endl;
        }
        else
        {
             ValueNode* valNode = isValueNode(route[node]);
             cout << valNode->vars[0] << endl;
        }
    }
}


namespace // anonymous namespace
{
    typedef EventReverser::VGVertex VGVertex;
    typedef EventReverser::VGEdge   VGEdge;
    typedef pair<vector<VGEdge>, int> RouteWithCost;


    // A local structure to help find all routes.
    struct RouteWithNodes
    {
        RouteWithNodes() : cost(0) {}

        vector<VGEdge>   edges;
        vector<pair<VGVertex, VGVertex> >  nodes;
        int cost;
    };

    // Returns if the vector contains the second parameter
    // at the first member of each element.
    bool containsVertex(const vector<pair<VGVertex, VGVertex> >& nodes,
                        VGVertex node)
    {
        typedef pair<VGVertex, VGVertex> VertexPair;
        foreach (const VertexPair& vpair, nodes)
        {
            if (vpair.first == node)
                return true;
        }
        return false;
    }
} // end of anonymous


EventReverser::SubValueGraph EventReverser::getReversalRoute(
        int dagIndex, int pathIndex,
        const SubValueGraph& subgraph,
        const vector<VGVertex>& valuesToRestore)
{
    map<VGVertex, vector<RouteWithCost> > allRoutes;

    foreach (VGVertex valNode, valuesToRestore)
    {
        RouteWithNodes route;
        route.nodes.push_back(make_pair(valNode, valNode));

        vector<RouteWithNodes> routes(1, route);

        // This stack stores all possible routes for the state variable.
        stack<RouteWithNodes> unfinishedRoutes;
        unfinishedRoutes.push(route);

        while (!unfinishedRoutes.empty())
        {
            // Fetch a route.
            RouteWithNodes unfinishedRoute = unfinishedRoutes.top();
            unfinishedRoutes.pop();

            if (unfinishedRoute.nodes.empty())
                continue;

            // Get the node on the top, and find it out edges.
            VGVertex node = unfinishedRoute.nodes.back().first;

            // If this node is an operatot node, add all its operands.
            if (isOperatorNode(subgraph[node]))
            {
                RouteWithNodes newRoute = unfinishedRoute;
                foreach (const VGEdge& edge, boost::out_edges(node, subgraph))
                {
                    VGVertex tar = boost::target(edge, subgraph);

                    // The the following function returns true if adding
                    // this edge will form a circle.
                    if (containsVertex(unfinishedRoute.nodes, tar))
                        goto NEXT;

                    newRoute.edges.push_back(edge);
                    newRoute.nodes.push_back(make_pair(tar, node));
                }
                unfinishedRoutes.push(newRoute);
                continue;
            }

            foreach (const VGEdge& edge, boost::out_edges(node, subgraph))
            {
                VGVertex tar = boost::target(edge, subgraph);

                // If the target is the root, go back in the stack.
                if (tar == root_)
                {
                    RouteWithNodes newRoute = unfinishedRoute;
                    newRoute.edges.push_back(edge);

                    // Keep removing nodes from the stack if the node before this
                    // node is its parent node.
                    VGVertex parent;
                    do
                    {
                        parent = newRoute.nodes.back().second;
                        newRoute.nodes.pop_back();
                        if (newRoute.nodes.empty()) break;
                    } while (parent == newRoute.nodes.back().first);

                    if (!newRoute.nodes.empty())
                    {
                        unfinishedRoutes.push(newRoute);
                        continue;
                    }

#if 0
                    foreach (const VGEdge& e, newRoute.edges)
                        cout << subgraph[e]->toString() << " ==> ";
                    cout << "  cost: " << newRoute.cost << "\n";
#endif
              
                    // If there is no nodes in this route, this route is finished.
                    foreach (const VGEdge& edge, newRoute.edges)
                        newRoute.cost += subgraph[edge]->cost;

                    vector<RouteWithCost>& routeWithCost = allRoutes[valNode];
                    routeWithCost.push_back(RouteWithCost());
                    // Swap instead of copy to improve performance.
                    routeWithCost.back().first.swap(newRoute.edges);
      
                    continue;
                }

                // The the following function returns true if adding
                // this edge will form a circle.
                if (containsVertex(unfinishedRoute.nodes, tar))
                    continue;

                RouteWithNodes newRoute = unfinishedRoute;
                newRoute.edges.push_back(edge);
                newRoute.nodes.push_back(make_pair(tar, node));
                unfinishedRoutes.push(newRoute);
            } // end of foreach (const VGEdge& edge, boost::out_edges(node, subgraph))
NEXT:
            ;
        } // end of while (!unfinishedRoutes.empty())
    } // end of foreach (VGVertex valNode, valuesToRestore)

    /**************************************************************************/
    // Now get the route for all state variables.
    // map<VGVertex, vector<RouteWithCost> > allRoutes;
    pair<int, int> path = make_pair(dagIndex, pathIndex);
    set<VGVertex>& nodesInRoute = routeNodesAndEdges_[path].first;
    set<VGEdge>&   edgesInRoute = routeNodesAndEdges_[path].second;

    typedef map<VGVertex, vector<RouteWithCost> >::value_type VertexWithRoute;
    foreach (VertexWithRoute& nodeWithRoute, allRoutes)
    {
        int minCost = INT_MAX;
        size_t minIndex;

        for (size_t i = 0, m = nodeWithRoute.second.size(); i < m; ++i)
        {
            RouteWithCost& route = nodeWithRoute.second[i];
            foreach (const VGEdge& edge, route.first)
                route.second += subgraph[edge]->cost;
            if (route.second < minCost)
            {
                minCost = route.second;
                minIndex = i;
            }
        }

        foreach (const VGEdge& edge, nodeWithRoute.second[minIndex].first)
        {
            nodesInRoute.insert(boost::source(edge, subgraph));
            edgesInRoute.insert(edge);
        }
        nodesInRoute.insert(root_);
    }

    // End.
    /**************************************************************************/

    // To resolve the problem of binding an overloaded function.
    set<VGVertex>::const_iterator (set<VGVertex>::*findNode)
             (const set<VGVertex>::key_type&) const = &set<VGVertex>::find;
    set<VGEdge>::const_iterator (set<VGEdge>::*findEdge)
             (const set<VGEdge>::key_type&) const = &set<VGEdge>::find;
    return SubValueGraph(valueGraph_,
                         boost::bind(findEdge, &edgesInRoute, ::_1) != edgesInRoute.end(),
                         boost::bind(findNode, &nodesInRoute, ::_1) != nodesInRoute.end());
}



} // end of Backstroke