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
    
    getReversalRoute(dagIndex, valuesToRestore_[dagIndex]);

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
    typedef pair<vector<VGEdge>, PathInfo> RouteWithPaths;


    // A local structure to help find all routes.
    struct Route
    {
        Route() : cost(0) {}
        Route& operator=(Route& route)
        {
            edges.swap(route.edges);
            nodes.swap(route.nodes);
            cost = route.cost;
            paths = route.paths;
            return *this;
        }

        vector<VGEdge>   edges;
        vector<pair<VGVertex, VGVertex> >  nodes;
        int cost;
        PathInfo paths;
    };
    
    inline bool operator<(const Route& r1, const Route& r2)
    { return r1.paths < r2.paths; }

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


map<EventReverser::VGEdge, PathInfo> EventReverser::getReversalRoute(
        int dagIndex,
        const set<VGVertex>& valsToRestore)
{
    map<VGVertex, vector<Route> > allRoutes;
    
    set<VGVertex> valuesToRestore = valsToRestore;
    // Find all reverse function call node and put them into valuesToRestore set.
    foreach (VGVertex node, boost::vertices(valueGraph_))
    {
        FunctionCallNode* funcCallNode = isFunctionCallNode(valueGraph_[node]);
        // Note that now only reverse function call node will be add to route graph.
        if (funcCallNode && funcCallNode->isReverse)
            valuesToRestore.insert(node);
    }
    
    foreach (VGVertex valToRestore, valuesToRestore)
    {
        //// A flag for mu node search.
        //bool firstNode = true;
        
#if 0
        // For dummy nodes.
        if (ValueNode* v = isValueNode(valueGraph_[valToRestore]))
        {
            if (v->isTemp())
            {
                PathInfos paths = valueGraph_[*(boost::out_edges(valToRestore, valueGraph_).first)]->paths;
                if (paths.count(dagIndex) == 0 || !paths[dagIndex][pathIndex])
                {
                    continue;
                }
            }
        }
#endif
        
        // This stack stores all possible routes for the state variable.
        stack<Route> unfinishedRoutes;
        
        // Initialize the stack.
        foreach (const VGEdge& edge, boost::out_edges(valToRestore, valueGraph_))
        {
            VGVertex tar = boost::target(edge, valueGraph_);

            Route route;
            route.edges.push_back(edge);
            route.nodes.push_back(make_pair(tar, valToRestore));
            route.paths = valueGraph_[edge]->paths[dagIndex];
            if (!route.paths.isEmpty())
                unfinishedRoutes.push(route);
        }

        while (!unfinishedRoutes.empty())
        {
            // Fetch a route.
            Route unfinishedRoute = unfinishedRoutes.top();
            unfinishedRoutes.pop();

            if (unfinishedRoute.nodes.empty())
                continue;
            // To prevent long time search, limit the nodes in the route no more
            // than 10.
            //if (unfinishedRoute.nodes.size() > 10)
            //    continue;

            // Get the node on the top, and find it out edges.
            VGVertex node = unfinishedRoute.nodes.back().first;
            
            
            // Currently, we forbid the route graph includes any function call nodes
            // which are not reverse ones. This will be modified in the future.
            FunctionCallNode* funcCallNode = isFunctionCallNode(valueGraph_[node]);
            if (funcCallNode && !funcCallNode->isReverse)
                continue;

            
            //if (availableNodes.count(node) > 0)
            // For a function call node, if its target is itself, keep searching.
            if ((node == root_ || funcCallNode) && node != valToRestore)
            {                
                //cout << "AVAILABLE: " << valueGraph_[node]->toString() << endl;
                
                Route& newRoute = unfinishedRoute;

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
                    newRoute.cost += valueGraph_[edge]->cost;

                vector<Route>& routes = allRoutes[valToRestore];
                routes.push_back(Route());
                // Swap instead of copy for performance.
                routes.back().edges.swap(newRoute.edges);
                routes.back().paths = newRoute.paths;

                continue;
            }

            // If this node is an operator node or function call node, add all its operands.
            if (isOperatorNode(valueGraph_[node]) || isFunctionCallNode(valueGraph_[node]))
            {
                Route& newRoute = unfinishedRoute;
                foreach (const VGEdge& edge, boost::out_edges(node, valueGraph_))
                {
                    VGVertex tar = boost::target(edge, valueGraph_);
                    
                    //cout << "OPERAND: " << valueGraph_[tar]->toString() << endl;

                    // The the following function returns true if adding
                    // this edge will form a circle.
                    if (containsVertex(unfinishedRoute.nodes, tar))
                        goto NEXT;
                }
                
                foreach (const VGEdge& edge, boost::out_edges(node, valueGraph_))
                {
                    VGVertex tar = boost::target(edge, valueGraph_);
                    newRoute.edges.push_back(edge);
                    newRoute.nodes.push_back(make_pair(tar, node));
                }
                unfinishedRoutes.push(newRoute);
                continue;
            }

            foreach (const VGEdge& edge, boost::out_edges(node, valueGraph_))
            {
                VGVertex tar = boost::target(edge, valueGraph_);
                
                // The the following function returns true if adding
                // this edge will form a circle.
                if (containsVertex(unfinishedRoute.nodes, tar))
                    continue;

                Route newRoute = unfinishedRoute;
                newRoute.edges.push_back(edge);
                newRoute.nodes.push_back(make_pair(tar, node));
                newRoute.paths &= valueGraph_[edge]->paths[dagIndex];
                if (!newRoute.paths.isEmpty())
                    unfinishedRoutes.push(newRoute);
            } // end of foreach (const VGEdge& edge, boost::out_edges(node, valueGraph_))
NEXT:
            ;//firstNode = false;
        } // end of while (!unfinishedRoutes.empty())
    } // end of foreach (VGVertex valNode, valuesToRestore)
    
    //map<VGVertex, vector<RouteWithCost> > allRoutes;
    typedef map<VGVertex, vector<Route> >::value_type VertexWithRoute;
    foreach (const VertexWithRoute& nodeWithRoutes, allRoutes)
    {
        cout << "\n\n\n" << valueGraph_[nodeWithRoutes.first]->toString() << "\n\n";
        foreach (const Route& route, nodeWithRoutes.second)
        {
            cout << '\n' << route.paths << "\n";
            foreach (const VGEdge& edge, route.edges)
            cout << edge << " ==> ";
            cout << "\n";
        }
    }

    
    
    /**************************************************************************/
    // Now get the route for all state variables.
    
    // map<VGVertex, vector<RouteWithCost> > allRoutes;
    //pair<int, int> path = make_pair(dagIndex, pathIndex);
    //set<VGVertex>& nodesInRoute = routeNodesAndEdges_[path].first;
    //set<VGEdge>&   edgesInRoute = routeNodesAndEdges_[path].second;
    map<VGEdge, PathInfo> edgesInRoute;
    
    // The following map stores the cost of each edge and how many times it's shared
    // by different to-store values.
    map<VGEdge, pair<int, int> > costForEdges;
    
    // Collect cost information.
    foreach (const VGEdge& edge, boost::edges(valueGraph_))
        costForEdges[edge] = make_pair(valueGraph_[edge]->cost, 0);
    
    // Make stats how many times an edge is shared by different to-store values.
    foreach (VertexWithRoute& nodeWithRoute, allRoutes)
    {
        set<VGEdge> edges;
        foreach (const Route& route, nodeWithRoute.second)
        {
            foreach (const VGEdge& edge, route.edges)
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
        set<Route> routes;
        
        
        map<PathInfo, int> pathToCost;
        
        // For each path, find the route with minimum cost. 
        foreach (Route& route, nodeWithRoute.second)
        {
            float cost = 0;
            
            //RouteWithPaths& route = nodeWithRoute.second[i];
            foreach (const VGEdge& edge, route.edges)
            {
                //route.second += subgraph[edge]->cost;
                pair<int, int> costWithCounter = costForEdges[edge];
                // In this way we make an approximation of the real cost in the
                // final route graph.
                cost += float(costWithCounter.first) / costWithCounter.second;
            }
            
            route.cost = cost;
            
            
            
            
            bool toInsert = true;
            vector<Route> toRemove;
            vector<Route> toAdd;
            
            foreach (const Route& route2, routes)
            {
                PathSet p = route2.paths & route.paths;
                if (p.any())
                {
                    if (cost < route2.cost)
                    {
                        toRemove.push_back(route2);
                        toAdd.push_back(route);
                        //pathToCost.erase(pathCost.first);
                        //routes.insert(route);
                    }
                    toInsert = false;
                }
            }
            if (toInsert)
                routes.insert(route);
            
            // Add and remove routes.
            foreach (const Route& route, toRemove)
                routes.erase(route);
            foreach (const Route& route, toAdd)
                routes.insert(route);
            
        }
        
        
        // Make sure the union of all paths is all path set.
        PathSet paths;
        foreach (const Route& route, routes)
        {
            if (paths.empty())
                paths = route.paths;
            else
                paths |= route.paths;
        }
        ROSE_ASSERT(!paths.flip().any());
        
        foreach (const Route& route, routes)
        {
            foreach (const VGEdge& edge, route.edges)
            {
                PathInfo& paths = edgesInRoute[edge];
                if (paths.empty())
                    paths = route.paths;
                else
                    paths |= route.paths;
                
                const PathInfo& origPaths = valueGraph_[edge]->paths[dagIndex];
                if (paths == origPaths)
                    paths = origPaths;
            }
            
        }
    }
    
    
    typedef map<EventReverser::VGEdge, PathInfo>::value_type T;
    cout << "\n\n";
    foreach (const T& edgeWithPaths, edgesInRoute)
    {
        cout << edgeWithPaths.first << " : " << edgeWithPaths.second << "\n";
    }
    cout << "\n\n";

    return edgesInRoute;
}

set<EventReverser::VGEdge> EventReverser::getReversalRoute(
        int dagIndex,
        int pathIndex,
        const SubValueGraph& subgraph,
        const set<VGVertex>& valsToRestore,
        const set<VGVertex>& availableNodes)
{
    map<VGVertex, vector<Route> > allRoutes;
    
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
                PathInfos paths = valueGraph_[*(boost::out_edges(valToRestore, valueGraph_).first)]->paths;
                if (paths.count(dagIndex) == 0 || !paths[dagIndex][pathIndex])
                {
                    continue;
                }
            }
        }
        
        
        Route route;
        route.nodes.push_back(make_pair(valToRestore, valToRestore));

        vector<Route> routes(1, route);

        // This stack stores all possible routes for the state variable.
        stack<Route> unfinishedRoutes;
        unfinishedRoutes.push(route);

        while (!unfinishedRoutes.empty())
        {
            // Fetch a route.
            Route unfinishedRoute = unfinishedRoutes.top();
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
                
                Route& newRoute = unfinishedRoute;

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

                vector<Route>& routes = allRoutes[valToRestore];
                routes.push_back(Route());
                // Swap instead of copy for performance.
                routes.back().edges.swap(newRoute.edges);

                continue;
            }

            // If this node is an operator node or function call node, add all its operands.
            if (isOperatorNode(subgraph[node]) || isFunctionCallNode(subgraph[node]))
            {
                Route& newRoute = unfinishedRoute;
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

                Route newRoute = unfinishedRoute;
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
    
    typedef map<VGVertex, vector<Route> >::value_type VertexWithRoute;
    
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
        foreach (const Route& route, nodeWithRoute.second)
        {
            foreach (const VGEdge& edge, route.edges)
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
            
            Route& route = nodeWithRoute.second[i];
            foreach (const VGEdge& edge, route.edges)
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

        foreach (const VGEdge& edge, nodeWithRoute.second[minIndex].edges)
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
