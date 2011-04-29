// tps (1/14/2010) : Switching from rose.h to sage3 changed size from 19,9 MB to 9,5MB
#include "sage3basic.h"

#include "DependenceGraph.h"
#include <set>
using namespace std;

DependenceNode *MergedDependenceGraph::_importNode(DependenceNode * node)
{/*
    if (node->isInterproc())
    {
        // if it's interprocedural, we want to use the pointer from
        // _interproc, not the imported pointer
        return createNode(node->getCopiedFrom());
    }
    else if (node->getType() == DependenceNode::SGNODE)
    {
        return createNode(node->getNode());
    }
    else
    {
        return createNode(node);
    }*/
                return NULL;
}

void MergedDependenceGraph::mergeGraph(DependenceGraph * graph)
{
        set < SimpleDirectedGraphNode * >nodes = graph->getNodes();
        // for all nodes in this graph...
        set < SimpleDirectedGraphNode * >::iterator i;
        DependenceNode * srcDepParent;
        SgNode * parentSgNode,*childSgNode;
        DependenceNode::NodeType parentType,childType;
        for (i = nodes.begin(); i != nodes.end(); i++)
        {
                // get the parent in the source graph
                srcDepParent = dynamic_cast < DependenceNode * >(*i);
                parentSgNode= srcDepParent->getSgNode();
                parentType=srcDepParent->getType();
//              cout <<"Merging ParentNode: "<<parentSgNode->unparseToString()<<endl;
                
                set < SimpleDirectedGraphNode * >::iterator j;
                set < SimpleDirectedGraphNode * > srcDepChildren = srcDepParent->getSuccessors(); 
                for (j = srcDepChildren.begin(); j != srcDepChildren.end(); j++)
                {
                        // get the child node in the src-graph  
                        DependenceNode * srcDepChild=dynamic_cast < DependenceNode * >(*j);
                        childSgNode=srcDepChild->getSgNode();
                        childType=srcDepChild->getType();
//                      cout <<"Merging ChildNode: "<<childSgNode->unparseToString()<<endl;
                        // get all edges between parent and child
                        set < EdgeType > edgeTypes = graph->edgeType(srcDepParent,srcDepChild);
                        for (set < EdgeType >::iterator k = edgeTypes.begin(); k != edgeTypes.end(); k++)
                        {
                                // for each edge-type establish an edge
                                // if the nodes for parent and child are not yet created, getNode will automatically create that node
                                establishEdge(getNode(parentType,parentSgNode),getNode(childType,childSgNode),*k);
                        }
                }
        }
}

void MergedDependenceGraph::_mergeGraph(DependenceGraph * graph)
{
    set < SimpleDirectedGraphNode * >nodes = graph->getNodes();

    // now go through and set up the edges
    set < SimpleDirectedGraphNode * >::iterator i;
    for (i = nodes.begin(); i != nodes.end(); i++)
    {
        DependenceNode *orig = dynamic_cast < DependenceNode * >(*i);
        DependenceNode *node = _importNode(orig);

        set < SimpleDirectedGraphNode * >::iterator j;
        // go through all of orig's successors and link orig to them
        set < SimpleDirectedGraphNode * >succs = orig->getSuccessors();
        for (j = succs.begin(); j != succs.end(); j++)
        {
            DependenceNode *orig_succ = dynamic_cast < DependenceNode * >(*j);
            DependenceNode *new_succ = _importNode(orig_succ);

            // get the types of edges from orig to orig_succ
            set < EdgeType > edgeTypes = graph->edgeType(orig, orig_succ);
            // go through the various edge types and "establish" that edge in
            // the current graph
            for (set < EdgeType >::iterator k = edgeTypes.begin(); k != edgeTypes.end(); k++)
            {
                establishEdge(node, new_succ, *k);
            }
            // _establishEdge(node, new_succ, CONTROL);
            // addLink(node, new_succ); //this also sets the predecessors
            // correctly
        }
    }
}

set < SgNode * >MergedDependenceGraph::slice(SgNode * node)
{/*TODO
    set < DependenceNode * >reached = getSlice(getNode(node));

    set < SgNode * >retval;
    for (set < DependenceNode * >::iterator i = reached.begin(); i != reached.end(); i++)
    {
        DependenceNode *dnode = *i;

        if (dnode->getType() == DependenceNode::SGNODE)
            retval.insert(dnode->getNode());
    }

    return retval;*/ return set<SgNode * >();
}

set < DependenceNode * >MergedDependenceGraph::_getReachable(set <
                                                             DependenceNode *
                                                             >start, int edgeTypesToFollow)
{

    set < DependenceNode * >reachables;
    stack < DependenceNode * >remaining;

    for (set < DependenceNode * >::iterator i = start.begin(); i != start.end(); i++)
    {
        remaining.push(*i);
    }

    // Simple DFS on graph to find reachable nodes
    while (remaining.size() != 0)
    {
        // get the first node on the stack
        DependenceNode *curr = remaining.top();

        remaining.pop();

        // if we haven't seen it, add it to the return list and push its
        // children shared accros appropriate edges onto the list
        if (reachables.count(curr) == 0)
        {
            reachables.insert(curr);

            set < SimpleDirectedGraphNode * >preds = curr->getPredecessors();
            // go through the children and only keep the children which have
            // the right sort of edge
            for (set < SimpleDirectedGraphNode * >::iterator i = preds.begin();
                 i != preds.end(); i++)
            {
                DependenceNode *pred = dynamic_cast < DependenceNode * >(*i);

                // get the types of edges that go from pred to curr
                set < EdgeType > eds = edgeType(pred, curr);
                // if any of the edges are in edgeTypesToFollow, then we add
                // this element to the list
                bool addToRemaining = false;

                for (set < EdgeType >::iterator j = eds.begin(); j != eds.end(); j++)
                {
                    EdgeType edge = *j;

                    if (edge & edgeTypesToFollow  && !(edge & DO_NOT_FOLLOW))
                    {           // it's a bit vector
                        addToRemaining = true;
                    }
                }
                if (addToRemaining)
                {
                    remaining.push(pred);
                }
            }
        }
        else
        {
            // do nothing - we've already seen this node
        }
    }

    return reachables;
}
