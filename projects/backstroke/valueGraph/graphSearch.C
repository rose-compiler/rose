#include "valueGraph.h"
#include "pathNumGenerator.h"
#include <sageBuilder.h>
#include <boost/bind.hpp>
#include <boost/foreach.hpp>
//#include <boost/graph/graphviz.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>

namespace Backstroke
{

using namespace std;

#define foreach BOOST_FOREACH


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
    
    return edge->paths.hasPath(dagIndex, pathIndex);
}

set<EventReverser::VGEdge> EventReverser::getRouteFromSubGraph(int dagIndex, int pathIndex)
{
    //PathEdgeSelector edgeSelector(&valueGraph_, dagIndex, i);
    //SubValueGraph subgraph(valueGraph_, edgeSelector);

    // First, get the subgraph for the path.

    pair<int, int> path(dagIndex, pathIndex);
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
    
    //set<VGVertex> availableNodes;
    //availableNodes.insert(root_);

    // Get the route for this path.
    return getReversalRoute(dagIndex, pathIndex, subgraph, 
            valuesToRestore_[dagIndex], availableValues_[dagIndex]);
    
    // Generate the reverse function for this route.
    //generateReverseFunction(scope, route);

    // Fix all variable references.
    //SageInterface::fixVariableReferences(scope);
    //graphToDot(subgraph, filename);
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
        RouteWithNodes& operator=(RouteWithNodes& route)
        {
            edges.swap(route.edges);
            nodes.swap(route.nodes);
            cost = route.cost;
            return *this;
        }

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
    
    // Returns if the given value graph node is a mu node and its DAG index is
    // the same as the given one.
    bool isMuNode(ValueGraphNode* node, int dagIndex)
    {
        if (MuNode* muNode = isMuNode(node))
        {
            if (dagIndex == muNode->dagIndex)
                return true;
        }
        return false;
    }
} // end of anonymous

set<EventReverser::VGEdge> EventReverser::getReversalRoute(
        int dagIndex,
        //const SubValueGraph& subgraph,
        const set<VGVertex>& valsToRestore,
        const set<VGVertex>& availableNodes)
{
    map<VGVertex, vector<RouteWithCost> > allRoutes;
    
    set<VGVertex> valuesToRestore = valsToRestore;
    // Find all reverse function call node and put them into valuesToRestore set.
    foreach (VGVertex node, boost::vertices(subgraph))
    {
        FunctionCallNode* funcCallNode = isFunctionCallNode(subgraph[node]);
        // Note that now only reverse function call node will be add to route graph.
        if (funcCallNode && funcCallNode->isReverse)
            valuesToRestore.insert(node);
    }
    
    foreach (VGVertex valToRestore, valuesToRestore)
    {
        //// A flag for mu node search.
        //bool firstNode = true;
        
        // For dummy nodes.
        if (ValueNode* v = isValueNode(valueGraph_[valToRestore]))
        {
            if (v->isTemp())
            {
                PathInfo paths = valueGraph_[*(boost::out_edges(valToRestore, valueGraph_).first)]->paths;
                if (paths.count(dagIndex) == 0 || !paths[dagIndex][pathIndex])
                {
                    continue;
                }
            }
        }
        
        
        RouteWithNodes route;
        route.nodes.push_back(make_pair(valToRestore, valToRestore));

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
            // To prevent long time search, limit the nodes in the router no more
            // than 10.
            //if (unfinishedRoute.nodes.size() > 10)
            //    continue;

            // Get the node on the top, and find it out edges.
            VGVertex node = unfinishedRoute.nodes.back().first;
            
            
            // Currently, we forbid the route graph includes any function call nodes
            // which are not reverse ones. This will be modified in the future.
            FunctionCallNode* funcCallNode = isFunctionCallNode(subgraph[node]);
            if (funcCallNode && !funcCallNode->isReverse)
                continue;

            
            //if (availableNodes.count(node) > 0)
            // For a function call node, if its target is itself, keep searching.
            if ((node == root_ || funcCallNode) && node != valToRestore)
            {                
                //cout << "AVAILABLE: " << valueGraph_[node]->toString() << endl;
                
                RouteWithNodes& newRoute = unfinishedRoute;

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

                // If there is no nodes in this route, this route is finished.
                foreach (const VGEdge& edge, newRoute.edges)
                    newRoute.cost += subgraph[edge]->cost;

                vector<RouteWithCost>& routeWithCost = allRoutes[valToRestore];
                routeWithCost.push_back(RouteWithCost());
                // Swap instead of copy for performance.
                routeWithCost.back().first.swap(newRoute.edges);

                continue;
            }

            // If this node is an operator node or function call node, add all its operands.
            if (isOperatorNode(subgraph[node]) || isFunctionCallNode(subgraph[node]))
            {
                RouteWithNodes& newRoute = unfinishedRoute;
                foreach (const VGEdge& edge, boost::out_edges(node, valueGraph_))
                {
                    VGVertex tar = boost::target(edge, subgraph);
                    
                    //cout << "OPERAND: " << valueGraph_[tar]->toString() << endl;

                    // The the following function returns true if adding
                    // this edge will form a circle.
                    if (containsVertex(unfinishedRoute.nodes, tar))
                        goto NEXT;
                }
                
                foreach (const VGEdge& edge, boost::out_edges(node, valueGraph_))
                {
                    VGVertex tar = boost::target(edge, subgraph);
                    newRoute.edges.push_back(edge);
                    newRoute.nodes.push_back(make_pair(tar, node));
                }
                unfinishedRoutes.push(newRoute);
                continue;
            }

            foreach (const VGEdge& edge, boost::out_edges(node, subgraph))
            {
                VGVertex tar = boost::target(edge, subgraph);
                
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
            ;//firstNode = false;
        } // end of while (!unfinishedRoutes.empty())
    } // end of foreach (VGVertex valNode, valuesToRestore)

}

set<EventReverser::VGEdge> EventReverser::getReversalRoute(
        int dagIndex,
        int pathIndex,
        const SubValueGraph& subgraph,
        const set<VGVertex>& valsToRestore,
        const set<VGVertex>& availableNodes)
{
    map<VGVertex, vector<RouteWithCost> > allRoutes;
    
    set<VGVertex> valuesToRestore = valsToRestore;
    // Find all reverse function call node and put them into valuesToRestore set.
    foreach (VGVertex node, boost::vertices(subgraph))
    {
        FunctionCallNode* funcCallNode = isFunctionCallNode(subgraph[node]);
        // Note that now only reverse function call node will be add to route graph.
        if (funcCallNode && funcCallNode->isReverse)
            valuesToRestore.insert(node);
    }
    
    foreach (VGVertex valToRestore, valuesToRestore)
    {
        //// A flag for mu node search.
        //bool firstNode = true;
        
        // For dummy nodes.
        if (ValueNode* v = isValueNode(valueGraph_[valToRestore]))
        {
            if (v->isTemp())
            {
                PathInfo paths = valueGraph_[*(boost::out_edges(valToRestore, valueGraph_).first)]->paths;
                if (paths.count(dagIndex) == 0 || !paths[dagIndex][pathIndex])
                {
                    continue;
                }
            }
        }
        
        
        RouteWithNodes route;
        route.nodes.push_back(make_pair(valToRestore, valToRestore));

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
            // To prevent long time search, limit the nodes in the router no more
            // than 10.
            //if (unfinishedRoute.nodes.size() > 10)
            //    continue;

            // Get the node on the top, and find it out edges.
            VGVertex node = unfinishedRoute.nodes.back().first;
            
            
            // Currently, we forbid the route graph includes any function call nodes
            // which are not reverse ones. This will be modified in the future.
            FunctionCallNode* funcCallNode = isFunctionCallNode(subgraph[node]);
            if (funcCallNode && !funcCallNode->isReverse)
                continue;

            
            //if (availableNodes.count(node) > 0)
            // For a function call node, if its target is itself, keep searching.
            if ((node == root_ || funcCallNode) && node != valToRestore)
            {                
                //cout << "AVAILABLE: " << valueGraph_[node]->toString() << endl;
                
                RouteWithNodes& newRoute = unfinishedRoute;

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

                vector<RouteWithCost>& routeWithCost = allRoutes[valToRestore];
                routeWithCost.push_back(RouteWithCost());
                // Swap instead of copy for performance.
                routeWithCost.back().first.swap(newRoute.edges);

                continue;
            }

            // If this node is an operator node or function call node, add all its operands.
            if (isOperatorNode(subgraph[node]) || isFunctionCallNode(subgraph[node]))
            {
                RouteWithNodes& newRoute = unfinishedRoute;
                foreach (const VGEdge& edge, boost::out_edges(node, valueGraph_))
                {
                    VGVertex tar = boost::target(edge, subgraph);
                    
                    //cout << "OPERAND: " << valueGraph_[tar]->toString() << endl;

                    // The the following function returns true if adding
                    // this edge will form a circle.
                    if (containsVertex(unfinishedRoute.nodes, tar))
                        goto NEXT;
                }
                
                foreach (const VGEdge& edge, boost::out_edges(node, valueGraph_))
                {
                    VGVertex tar = boost::target(edge, subgraph);
                    newRoute.edges.push_back(edge);
                    newRoute.nodes.push_back(make_pair(tar, node));
                }
                unfinishedRoutes.push(newRoute);
                continue;
            }

            foreach (const VGEdge& edge, boost::out_edges(node, subgraph))
            {
                VGVertex tar = boost::target(edge, subgraph);
                
#if 0
                // For loop search, if the source is the mu node and the target
                // is the root, and this is the first time to traverse the mu 
                // node, then the root should not be reached.
                if (tar == root_ && firstNode)
                {
                    if (PhiNode* phiNode = isPhiNode(valueGraph_[node]))
                        if (phiNode->mu)
                            continue;
                }
#endif
                
#if 0
                // If the target is an available node, go back in the stack.
                if (availableNodes.count(tar) > 0)
                //if (tar == root_ || )
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

                    vector<RouteWithCost>& routeWithCost = allRoutes[valToRestore];
                    routeWithCost.push_back(RouteWithCost());
                    // Swap instead of copy for performance.
                    routeWithCost.back().first.swap(newRoute.edges);
      
                    continue;
                }
#endif

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
            ;//firstNode = false;
        } // end of while (!unfinishedRoutes.empty())
    } // end of foreach (VGVertex valNode, valuesToRestore)

    
    /**************************************************************************/
    // Now get the route for all state variables.
    
    // map<VGVertex, vector<RouteWithCost> > allRoutes;
    pair<int, int> path = make_pair(dagIndex, pathIndex);
    set<VGVertex>& nodesInRoute = routeNodesAndEdges_[path].first;
    //set<VGEdge>&   edgesInRoute = routeNodesAndEdges_[path].second;
    set<VGEdge> edgesInRoute;
    
    typedef map<VGVertex, vector<RouteWithCost> >::value_type VertexWithRoute;
    
    // The following map stores the cost of each edge and how many times it's shared
    // by different to-store values.
    map<VGEdge, pair<int, int> > costForEdges;
    
    // Collect cost information.
    foreach (const VGEdge& edge, boost::edges(subgraph))
        costForEdges[edge] = make_pair(subgraph[edge]->cost, 0);
    
    // Make stats how many times an edge is shared by different to-store values.
    foreach (VertexWithRoute& nodeWithRoute, allRoutes)
    {
        set<VGEdge> edges;
        foreach (const RouteWithCost& routeWithCost, nodeWithRoute.second)
        {
            foreach (const VGEdge& edge, routeWithCost.first)
                edges.insert(edge);
        }
        
        foreach (const VGEdge& edge, edges)
        {
            //// A way to force the inverse of each function call is used.
            //if (isSgFunctionCallExp(subgraph[nodeWithRoute.first]->astNode))
            //    costForEdges[edge].second += 100;
            //else
                ++costForEdges[edge].second;
        }
    }
    

    foreach (VertexWithRoute& nodeWithRoute, allRoutes)
    {
        float minCost = std::numeric_limits<float>::max();
        size_t minIndex = 0;

        // Find the route with the minimum cost.
        for (size_t i = 0, m = nodeWithRoute.second.size(); i < m; ++i)
        {
            float cost = 0;
            
            RouteWithCost& route = nodeWithRoute.second[i];
            foreach (const VGEdge& edge, route.first)
            {
                //route.second += subgraph[edge]->cost;
                pair<int, int> costWithCounter = costForEdges[edge];
                // In this way we make an approximation of the real cost in the
                // final route graph.
                cost += float(costWithCounter.first) / costWithCounter.second;
            }
            
            if (cost < minCost)
            {
                minCost = cost;
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
    
    // Check if there is root node in edges set.
    bool hasRoot = false;
    foreach (const VGEdge& edge, edgesInRoute)
    {
        if (root_ == boost::target(edge, subgraph))
        {
            hasRoot = true;
            break;
        }
    }
    ROSE_ASSERT(edgesInRoute.empty() || hasRoot);

    return edgesInRoute;
    
#if 0

    // To resolve the problem of binding an overloaded function.
    set<VGVertex>::const_iterator (set<VGVertex>::*findNode)
             (const set<VGVertex>::key_type&) const = &set<VGVertex>::find;
    set<VGEdge>::const_iterator (set<VGEdge>::*findEdge)
             (const set<VGEdge>::key_type&) const = &set<VGEdge>::find;
    return SubValueGraph(valueGraph_,
                         boost::bind(findEdge, &edgesInRoute, ::_1) != edgesInRoute.end(),
                         boost::bind(findNode, &nodesInRoute, ::_1) != nodesInRoute.end());
#endif
}

} // end of Backstroke
