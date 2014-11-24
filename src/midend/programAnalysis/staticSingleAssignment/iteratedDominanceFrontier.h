#pragma once

// DQ (10/5/2014): This is more strict now that we include rose_config.h in the sage3basic.h.
// #include "rose.h"
// rose.h and sage3basic.h should not be included in librose header files. [Robb P. Matzke 2014-10-15]
// #include "sage3basic.h"

#include <boostGraphCFG.h>
#include <vector>
#include <set>
#include <map>
#include <iterator>
#include <boost/foreach.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/topological_sort.hpp>

namespace ssa_private
{
    using namespace std;
    using namespace boost;

    /** Given the dominance frontiers of each node and a set of start nodes, calculate the iterated dominance frontier
     * of the start nodes.          */
    template<class CfgNodeT>
    set<CfgNodeT> calculateIteratedDominanceFrontier(const map<CfgNodeT, set<CfgNodeT> >& dominanceFrontiers,
    const vector<CfgNodeT>& startNodes)
    {
        set<CfgNodeT> result;
        set<CfgNodeT> visitedNodes;
        vector<CfgNodeT> worklist;

        worklist.insert(worklist.end(), startNodes.begin(), startNodes.end());

        while (!worklist.empty())
        {
            CfgNodeT currentNode = worklist.back();
            worklist.pop_back();
            visitedNodes.insert(currentNode);

            //Get the dominance frontier of the node and add it to the results
            ROSE_ASSERT(dominanceFrontiers.count(currentNode) != 0);
            const set<CfgNodeT>& dominanceFrontier = dominanceFrontiers.find(currentNode)->second;

            //Add all the children to the result and to the worklist

            BOOST_FOREACH(CfgNodeT dfNode, dominanceFrontier)
            {
                if (visitedNodes.count(dfNode) > 0)
                    continue;

                result.insert(dfNode);
                worklist.push_back(dfNode);
            }
        }

        return result;
    }

    /** Calculates the dominance frontier for each node in the control flow graph of the given function.
     * @param iDominatorMap map from each node to its immediate dominator
     * @param iPostDominatorMap map from each node to its immediate postdominator */
    template<class CfgNodeT, class CfgEdgeT>
    map<CfgNodeT, set<CfgNodeT> > calculateDominanceFrontiers(SgFunctionDefinition* func, map<CfgNodeT, CfgNodeT>* iDominatorMap,
    map<CfgNodeT, CfgNodeT>* iPostDominatorMap)
    {
        typedef CFG<CfgNodeT, CfgEdgeT> ControlFlowGraph;

        //Build a CFG first
        ControlFlowGraph functionCfg(func);

        //Build the dominator tree
        typename ControlFlowGraph::VertexVertexMap dominatorTreeMap = functionCfg.getDominatorTree();

        //TODO: This code converts a VertexVertex Map to a  boost graph. Should be factored out
        typedef adjacency_list<vecS, vecS, bidirectionalS, CfgNodeT> TreeType;
        TreeType domTree;
        typedef typename graph_traits<TreeType>::vertex_descriptor TreeVertex;

        set<CfgNodeT> addedNodes;
        map<CfgNodeT, TreeVertex> cfgNodeToVertex;

        BOOST_FOREACH(typename ControlFlowGraph::VertexVertexMap::value_type& nodeDominatorPair, dominatorTreeMap)
        {
            CfgNodeT node = *functionCfg[nodeDominatorPair.first];
            CfgNodeT dominator = *functionCfg[nodeDominatorPair.second];

            if (addedNodes.count(dominator) == 0)
            {
                TreeVertex newVertex = add_vertex(domTree);
                cfgNodeToVertex[dominator] = newVertex;
                domTree[newVertex] = dominator;
                addedNodes.insert(dominator);
            }

            if (addedNodes.count(node) == 0)
            {
                TreeVertex newVertex = add_vertex(domTree);
                cfgNodeToVertex[node] = newVertex;
                domTree[newVertex] = node;
                addedNodes.insert(node);
            }

            //Add the edge from dominator to node
            add_edge(cfgNodeToVertex[dominator], cfgNodeToVertex[node], domTree);

            if (iDominatorMap != NULL)
            {
                ROSE_ASSERT(iDominatorMap->count(node) == 0);
                iDominatorMap->insert(make_pair(node, dominator));
            }
        }

        //Get a topological ordering of the vertices
        vector<TreeVertex> reverseTopological;
        topological_sort(domTree, back_inserter(reverseTopological));

        //Calculate all the dominance frontiers. This algorithm is from figure 10, Cytron et. al 1991
        map<CfgNodeT, set<CfgNodeT> > dominanceFrontiers;

        BOOST_FOREACH(TreeVertex v, reverseTopological)
        {
            CfgNodeT currentNode = domTree[v];
            set<CfgNodeT>& currentDominanceFrontier = dominanceFrontiers[currentNode];

            //Local contribution: Iterate over all the successors of v in the control flow graph

            BOOST_FOREACH(CfgEdgeT outEdge, currentNode.outEdges())
            {
                CfgNodeT successor = outEdge.target();

                //Get the immediate dominator of the successor
                typename ControlFlowGraph::Vertex successorVertex = functionCfg.getVertexForNode(successor);
#if !USE_ROSE
             // DQ (11/3/2011): EDG compilains about this (but GNU allowed it, I think that EDG might be correct.
             // since it might be a private variable.  But since we are only trying to compile ROSE with ROSE (using the
             // new EDG 4.3 front-end as a tests) we can just skip this case for now.
                ROSE_ASSERT(successorVertex != ControlFlowGraph::GraphTraits::null_vertex());
#endif
                ROSE_ASSERT(dominatorTreeMap.count(successorVertex) == 1);
                typename ControlFlowGraph::Vertex iDominatorVertex = dominatorTreeMap[successorVertex];
                CfgNodeT iDominator = *functionCfg[iDominatorVertex];

                //If we have a successor that we don't dominate, that successor is in our dominance frontier
                if (iDominator != currentNode)
                {
                    currentDominanceFrontier.insert(successor);
                }
            }

            //"Up" contribution. Iterate over all children in the dominator tree
            typename graph_traits<TreeType>::adjacency_iterator currentIter, lastIter;
            for (tie(currentIter, lastIter) = adjacent_vertices(v, domTree); currentIter != lastIter; currentIter++)
            {
                CfgNodeT childNode = domTree[*currentIter];
                const set<CfgNodeT>& childDominanceFrontier = dominanceFrontiers[childNode];

                BOOST_FOREACH(CfgNodeT childDFNode, childDominanceFrontier)
                {
                    //Get the immediate dominator of the child DF node
                    typename ControlFlowGraph::Vertex childDFVertex = functionCfg.getVertexForNode(childDFNode);
#if !USE_ROSE
                 // DQ (11/3/2011): EDG compilains about this (but GNU allowed it, I think that EDG might be correct.
                 // since it might be a private variable.  But since we are only trying to compile ROSE with ROSE (using the
                 // new EDG 4.3 front-end as a tests) we can just skip this case for now.
                    ROSE_ASSERT(childDFVertex != ControlFlowGraph::GraphTraits::null_vertex());
#endif
                    ROSE_ASSERT(dominatorTreeMap.count(childDFVertex) == 1);
                    typename ControlFlowGraph::Vertex iDominatorVertex = dominatorTreeMap[childDFVertex];
                    CfgNodeT iDominator = *functionCfg[iDominatorVertex];

                    if (iDominator != currentNode)
                    {
                        currentDominanceFrontier.insert(childDFNode);
                    }
                }
            }
        }

        //While we're at it, calculate the postdominator tree
        if (iPostDominatorMap != NULL)
        {
            typename ControlFlowGraph::VertexVertexMap postDominatorTreeMap = functionCfg.getPostdominatorTree();

            BOOST_FOREACH(typename ControlFlowGraph::VertexVertexMap::value_type& nodePostDominatorPair, postDominatorTreeMap)
            {
                CfgNodeT node = *functionCfg[nodePostDominatorPair.first];
                CfgNodeT postDominator = *functionCfg[nodePostDominatorPair.second];

                ROSE_ASSERT(iPostDominatorMap->count(node) == 0);
                iPostDominatorMap->insert(make_pair(node, postDominator));
            }
        }

        return dominanceFrontiers;
    }

}
