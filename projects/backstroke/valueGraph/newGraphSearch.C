#include "valueGraph.h"

namespace Backstroke
{
using namespace std;

#define foreach BOOST_FOREACH


void EventReverser::searchValueGraph()
{
    ValueGraph reversalGraph;
    
    // The search starts from each value which is needed to be restored.
    
    foreach (VGVertex valNode, valuesToRestore)
    {
        // For each of its out edges, search in depth first order, and propagate
        // predicates attached on edges.
    
    }
}

class Predicate
{
    ControlDependences pred_;
    
    BackstrokeCDG* cdg_;
    
public:
    Predicate(const ControlDependences& predicate) : pred_(predicate) {}
    
    void setIntersection(const Predicate& pred)
    {
        foreach (const ControlDependence& cd, pred_)
        {
            foreach ()
        }
    }
    
    ControlDependence getUnion(const ControlDependence& cd1, const ControlDependence& cd1)
    {
        
    }
    
    ControlDependence getIntersection(const ControlDependence& cd1, const ControlDependence& cd1)
    {
        
    }
    
    void setUnion(const Predicate& pred);
    
    bool empty() const;
    
    Predicate operator | (const Predicate& pred) const;
    
    Predicate operator & (const Predicate& pred) const;
};

Predicate searchNode(VGVertex node, 
            const Predicate& predicate, 
            set<VGVertex> searchedNodes,
            map<VGEdge, Predicate>& searchedEdges)
{
    vector<Predicate> predicates;
    
    foreach (const VGEdge& outEdge, boost::out_edges(node, valueGraph_))
    {
        const Predicate& predOnEdge = valueGraph_[outEdge]->controlDependences;
        
        // If the predicate does not match the predicate in arglist, continue.
        Predicate commonPred = getPredicateIntersection(predOnEdge, predicate);
        if (commonPred.empty())
            continue;
        
        VGVertex tgt = boost::target(outEdge, valueGraph_);
        
        // If the target exists in the searched node, a circle is formed so we stop
        // the search here.
        if (searchedNodes.count(tgt) > 0)
            continue;
        
        // If the target it root, we have reached the end of the route.
        if (tgt == root_)
        {
            predicates.push_back(commonPred);
            searchedEdges[outEdge] = commonPred;
            continue;
        }
            
        // Update the searched node set.
        searchedNodes.insert(node);
        
        map<VGEdge, Predicate> edgesAndPred;
        Predicate resultPred = searchNode(tgt, commonPred, searchedNodes, edgesAndPred);
        
        typedef map<VGEdge, Predicate>::value_type EdgeAndPred;
        foreach (const EdgeAndPred edgeAndPred, edgesAndPred)
            searchedEdges[edgeAndPred.first] |= edgeAndPred.second;
        predicates.push_back(resultPred);
        searchedEdges[outEdge] |= resultPred;
    }
    
    // The union of all predicates searched on this node is the result. Note that
    // if two predicates share the same route, we must pick one with less cost.
    
    // Sort all predicates to ease the union later.
    sort(predicates.begin(), predicates.end());
    Predicate resultPred;
    
    if (isOperatorNode(valueGraph_[tgt]))
    {
        resultPred = predicates[0];
        for (size_t i = 1, s = predicates.size(); i < s; ++i)
            resultPred.setIntersection(predicates[i]);
    }
    else
    {
        foreach (const Predicate& pred, predicates)
            resultPred.setUnion(pred);
    }
    return resultPred;
}



} // end of Backstroke
