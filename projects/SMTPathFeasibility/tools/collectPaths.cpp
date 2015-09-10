//#include "collectPaths.h" 
#include "../utils/utilHeader.h"
//#include <vector>
//#include <map>
//#include <string>
//#include <utility>
//#include "rose.h"
int recurses;
/*
typedef std::pair<SgGraphNode*, SgGraphNode*> Link;
typedef std::vector<SgGraphNode*> Path;
typedef std::vector<Path> PathSet;
typedef std::vector<Path>::const_iterator Paths;
*/

/*
class PathCollector
{
private:
      
    SgIncidenceDirectedGraph* graph;
    std::vector<std::vector<SgGraphNode*> > pathSet;        
 
public:
 
    PathCollector(SgIncidenceDirectedGraph* g) {self.graph = g;}
    PathCollector( const std::vector<Link>& links, 
             const bool& bi = false );
    ~PathCollector();
    
    void addPath( const std::vector<SgGraphNode*>& p );
    void clearPaths()
    void displayPath(std::vector<SgGraphNode*> p);
    void displayAllPaths()  
    //void traverseAndCollect(SgGraphNode* beginning, SgGraphNode* end) 
};
*/

StaticCFG::CFG* PathCollector::getStaticCFG() {
	return cfg;
}

CFGNode PathCollector::getEquivCFGNode(SgGraphNode* node) {
	return (cfg->toCFGNode(node));
}

int PathCollector::getEquivCFGNodeIndex(SgGraphNode* node) {
	return (cfg->toCFGNode(node).getIndex());
}

std::map<SgNode*,std::string> PathCollector::getAssociationsForPath(std::vector<SgGraphNode*> path) {
	std::map<SgNode*,std::string> conditionValueForPath;

	for (int i = 0; i < path.size(); i++) {
		//std::vector<SgGraphNode*> successors;
		//pathCollector->getGraph()->getSuccessors(path[i],successors);
		
		CFGNode node = cfg->toCFGNode(path[i]);
		std::vector<CFGEdge> edges = node.outEdges();
		if (edges.size() > 1) {
			//std::cout << "associated node: " << path[i]->get_SgNode()->class_name() << std::endl;
                	CFGNode eck_result = cfg->toCFGNode(path[i+1]);
			CFGEdge conditionEdge;
			int k = 0;
			int found = -1;
			while (k < edges.size()) {
				if (edges[k].target() == eck_result) {
					conditionEdge = edges[k];
					found = k;
					break;
				}
			k++;
			}
				
			EdgeConditionKind edgeKind = conditionEdge.condition();
			if (edgeKind == eckTrue) {
				conditionValueForPath[path[i]->get_SgNode()] = "true";
			}
			else if (edgeKind == eckFalse) {
				conditionValueForPath[path[i]->get_SgNode()] = "false";
			}
			else {
				std::cout << "edge kind not handled! exiting..." << std::endl;
				ROSE_ASSERT(false);
					
			
			}
		//	std::cout << "edge value: " << conditionValueForPath[path[i]->get_SgNode()] << std::endl;
		}
	}
	return conditionValueForPath;
	}		

void PathCollector::clearPaths() {
	pathSet.clear();
}



void PathCollector::addPath(const std::vector<SgGraphNode*>& p) {
	pathSet.push_back(p);
}

void PathCollector::displayPath(std::vector<SgGraphNode*> p) {
	std::vector<SgGraphNode*>::iterator i = p.begin();
	for (;i != p.end(); i++) {
		std::cout << ((*i)->get_SgNode()->class_name());
		std::cout << ", index " << (*i)->get_index() << std::endl;	
	}
	return;
}	

void PathCollector::displayAllPaths() {
	std::cout << "**********\n DISPLAYING PATHS ***********\n" << std::endl;
	std::cout << "pathSet.size() " << pathSet.size() << std::endl;
	std::vector<std::vector<SgGraphNode*> >::iterator i = pathSet.begin();
	int pathnum = 1;
	for(;i!=pathSet.end();i++) {
		std::cout << "path " << pathnum << std::endl;
		displayPath(*i);
		pathnum++;
	}
	std::cout << "\n******** PATHS DISPLAYED **********\n" << std::endl;
	return;
}
namespace GraphAlgorithm {
SgGraphNode* startNode;
}

bool GraphAlgorithm::containsNode(SgGraphNode* node, std::vector<SgGraphNode*> path) {
	std::vector<SgGraphNode*>::iterator i = path.begin();
	for(;i!=path.end(); i++) {
		if ((*i) == node) {
			return true;
		}
	}
	return false;
}



void GraphAlgorithm::DepthFirst(PathCollector* pathCollector, std::vector<SgGraphNode*>& visited, SgGraphNode* end, const int& max_hops, const int& min_hops) {
	//std::cout << "recurses: " << recurses << std::endl;
	recurses++;
	SgGraphNode* back = visited.back();
	std::vector<SgGraphNode*> successors;
	pathCollector->getGraph()->getSuccessors(back,successors) ;
	//std::cout << "successors: " << successors.size() << std::endl;  
	std::vector<SgGraphNode*>::iterator i = successors.begin();
	for (;i!=successors.end();i++) {
		SgGraphNode* node = (*i);
		bool startEqualsTarget = containsNode(node,visited) && startNode == end && node == startNode;
		if (containsNode(node,visited) && !startEqualsTarget) {
			continue;
		}
		if (node == end)
		{
			visited.push_back(*i);
			const int size = (int) visited.size();
			const int hops = size-1;
			if ( ( max_hops < 1 || hops <= max_hops) && hops >= min_hops) {
				std::vector<SgGraphNode*> path(visited.begin(),visited.begin()+size);
				pathCollector->addPath(path);
			}
			visited.erase(visited.begin()+hops);
			break;
		}
	}
	std::vector<SgGraphNode*>::iterator j = successors.begin();
	
	for(;j != successors.end(); j++) {
		SgGraphNode* node = (*j);
		if ( containsNode (node, visited) || node == end) {
			continue;
		}
		visited.push_back(node);
		DepthFirst(pathCollector,visited,end,max_hops,min_hops);
		int n = (int) visited.size()-1;
		visited.erase(visited.begin() + n);
	}
}


	

