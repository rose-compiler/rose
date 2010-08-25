// tps : Switching from rose.h to sage3 changed size from 20 MB to 9,8MB
#include "sage3basic.h"

#include "CreateSliceSet.h"
#include <set>

using namespace std;

CreateSliceSet::CreateSliceSet(SystemDependenceGraph *program,std::list<SgNode*> targets)
{
	sdg=program;
	sliceTargetNodes=targets;
}
std::set<SgNode*> CreateSliceSet::computeSliceSet()
{
	set < SgNode * >totalSlicingSet;
	set < SgNode * >currentSlicingSet, tmp;
	for (list < SgNode * >::iterator i = sliceTargetNodes.begin();i != sliceTargetNodes.end();i++)
	{
		currentSlicingSet=computeSliceSet(dynamic_cast<SgNode*>(*i));
		set_union(totalSlicingSet.begin(), totalSlicingSet.end(),
				currentSlicingSet.begin(), currentSlicingSet.end(),
				inserter(tmp, tmp.begin()));
		totalSlicingSet.swap(tmp);	
	}
	return totalSlicingSet;
}

std::set<SgNode*> CreateSliceSet::computeSliceSet(SgNode * node)
{
	std::set<SgNode*> returnSet;
	
	set<DependenceGraph::EdgeType> allowedEdges1,allowedEdges2;
	
	allowedEdges1.insert(DependenceGraph::CONTROL);
	allowedEdges1.insert(DependenceGraph::DATA);
	allowedEdges1.insert(DependenceGraph::SUMMARY);
	allowedEdges1.insert(DependenceGraph::PARAMETER_IN);
	// if we are within a function, the call helps us to reach the main scope
	allowedEdges1.insert(DependenceGraph::CALL);
	allowedEdges1.insert(DependenceGraph::GLOBALVAR_HELPER);
//	allowedEdges1.insert(DependenceGraph::CALL_RETURN);
	
	allowedEdges2.insert(DependenceGraph::PARAMETER_OUT);
	allowedEdges2.insert(DependenceGraph::CONTROL);
	allowedEdges2.insert(DependenceGraph::DATA);
	allowedEdges2.insert(DependenceGraph::SUMMARY);
	allowedEdges2.insert(DependenceGraph::GLOBALVAR_HELPER);
	allowedEdges2.insert(DependenceGraph::CALL);
	
	
	set<DependenceNode*> searchSet,returnedDepSet;
  
	searchSet.insert(sdg->getNode(node));
	
	returnedDepSet=getSliceDepNodes(searchSet,allowedEdges1);
	searchSet.insert(returnedDepSet.begin(),returnedDepSet.end());
	sdg->writeDot((char*)"stage1.slice.dot");
	returnedDepSet=getSliceDepNodes(searchSet,allowedEdges2);

	// transform the depset to sgSet
	for (set<DependenceNode*>::iterator i=returnedDepSet.begin();i!=returnedDepSet.end();i++)
	{
		// do not transform OUTNODES since the are spseudonodes!
		if ((*i)->isDummyNode()) continue;
//		if ((*i)->getType()==DependenceNode::ACTUALOUT) continue;
		returnSet.insert((*i)->getSgNode());
	}
	
	return returnSet;
}

                                                                                               
std::set<DependenceNode*> CreateSliceSet::getSliceDepNodes(set <DependenceNode*> searchSet,set<DependenceGraph::EdgeType> allowedEdges)
{
//	std::set<SgNode*> reachableSgNodes;
	set <DependenceNode*> reachableNodes;
	
	std::vector <DependenceNode*> workList;
	set <DependenceNode*> visitedSet;
	// add all DepNodes in the list to the work-set
	for (set<DependenceNode*>::iterator i=searchSet.begin();searchSet.end()!=i;i++)
	{
		workList.push_back(*i);
	}
	// initialize process loop
	DependenceNode *current=NULL;	
	while(workList.size())
	{
	//	cout <<"processing DepNode \"";
		current=workList.back();
//		current->writeOut(cout);
//		cout <<"\""<<endl;
		workList.pop_back();
		//
		if (visitedSet.count(current))
			continue;
		visitedSet.insert(current);		
//		SgNode * currentSgNode=current->getSgNode();
		current->highlightNode();
		// since a set contains only one copy for each node it is safe to insert the sgNode
//		reachableSgNodes.insert(currentSgNode);
		reachableNodes.insert(current);		
	
		// get all predecessors for this node
		set <SimpleDirectedGraphNode *> preds=current->getPredecessors();
		for (set < SimpleDirectedGraphNode * >::iterator i = preds.begin();
                     i != preds.end(); i++)
                {
			DependenceNode *pred = dynamic_cast < DependenceNode * >(*i);
			set < DependenceGraph::EdgeType > connectingSet=sdg->edgeType(pred,current);
			for (set < DependenceGraph::EdgeType >::iterator j=connectingSet.begin();j!=connectingSet.end();j++)
			{
				if (allowedEdges.count(*j))
				{
					workList.push_back(pred);
					break;
				}
			}
		}
	}
	return reachableNodes;
}
																																															 
