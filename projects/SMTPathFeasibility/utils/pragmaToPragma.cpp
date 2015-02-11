#include <iostream>
#include <fstream>
//#include <rose.h>
#include <string>
#ifndef _MSC_VER
#include <err.h>
#endif
#include "SgGraphTemplate.h"
#include "graphProcessing.h"
#include "staticCFG.h"
#include <set>
/* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/
#include <utility>
#include <map>

using namespace std;
using namespace boost;

typedef myGraph CFGforT;

std::vector<SgNode*> pragmas;
std::map<std::string, std::pair<SgNode*, SgNode*> > stringToPragmas;

class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
          int tltnodes;
	  int paths;
	  void analyzePath(vector<VertexID>& pth);
   };

CFGforT* orig;

void visitorTraversal::analyzePath(vector<VertexID>& pathR) {
        tltnodes += pathR.size();
        paths++;
        std::vector<SgGraphNode*> path;
        int pR = (int) pathR.size();
      	std::set<SgNode*> nodes_seen;
	std::cout << "new path number: " << paths << std::endl;
       for (int j = 0; j < pR; j++) {
		SgGraphNode* pRNode = (*orig)[pathR[j]].sg;
        	SgNode* pRNodeNode = pRNode->get_SgNode();
		if (nodes_seen.find(pRNodeNode) == nodes_seen.end()) {
			nodes_seen.insert(pRNodeNode);
			std::cout << "node num: " << j << ": " <<  (pRNode->get_SgNode())->class_name() << std::endl;
		}
	}
	std::cout << "new path end" << std::endl;
}

std::string get_pragma_string(SgPragmaDeclaration* prdecl) {
	ROSE_ASSERT(prdecl!=NULL);
	SgPragma* pr = prdecl->get_pragma();
	std::string prag = pr->get_pragma();
	return prag;
}
std::string get_name(std::string pstring) {
	std::string name;
	//std::size_t begin_parse = pstring.find("pragma") + 6;
	//std::size_t curr_pos = pstring.begin();//egin_parse+1;
	int curr_pos = 0;
	if (pstring[curr_pos] == ' ') {
		while (pstring[curr_pos] == ' ') {
			curr_pos+=1;
		}
	}
	size_t begin_pos = (size_t) curr_pos;
	while (pstring[curr_pos] != ' ') {
		curr_pos++;
	}
	size_t end_pos = (size_t) curr_pos;
	name = pstring.substr(begin_pos, end_pos-begin_pos);
	std::cout << "name: " << name << std::endl;
	
	return name;
}	

std::string get_position(std::string prag_str) {
	if (prag_str.find("begin") != std::string::npos) {
		return "begin";
	}
	else if (prag_str.find("end") != std::string::npos) {
		return "end";
	}
	ROSE_ASSERT(false);
	return "";
}

int main(int argc, char *argv[]) {
  
 std::set<std::string> names;
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 

  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);

 SgFunctionDefinition* mainDef = mainDefDecl->get_definition(); 
 StaticCFG::CFG cfg(mainDef);
 Rose_STL_Container<SgNode*> pragmaList = NodeQuery::querySubTree(proj,V_SgPragmaDeclaration);
  for (Rose_STL_Container<SgNode*>::iterator i = pragmaList.begin(); i != pragmaList.end(); i++) {
	  SgPragmaDeclaration* prg = isSgPragmaDeclaration(*i);
	  std::string prag_string = get_pragma_string(prg);
	  std::string name = get_name(prag_string);
	  if (names.find(name) == names.end()) {
		names.insert(name);
	  }
	  std::string position = get_position(prag_string);				     if (stringToPragmas.find(name) == stringToPragmas.end()) {
			std::pair<SgNode*,SgNode*> pragma_pair;
			if (position == "begin") {
			pragma_pair.first = prg;
			}
			else if (position == "end") {
			pragma_pair.second =prg;
			}
			else {
			std::cout << "position: " << position << " is not a legal position variable" << std::endl;
			ROSE_ASSERT(false);
			}
			stringToPragmas[name] = pragma_pair;		
	}
	else {
		if (position == "begin") {
			stringToPragmas[name].first = prg;
		}
		else if (position == "end") {
			stringToPragmas[name].second = prg;
		}
		else {
			std::cout << "position: " << position << " is not a legal position variable" << std::endl;
			ROSE_ASSERT(false);
		}
	}
}

SgIncidenceDirectedGraph* g = cfg.getGraph();
myGraph *mg = new myGraph();
mg = instantiateGraph(g,cfg);
orig = mg;
for (std::set<std::string>::iterator i = names.begin(); i != names.end(); i++) {
	std::pair<SgNode*,SgNode*> beginEnd = stringToPragmas[*i];
		


		
  visitorTraversal* visif = new visitorTraversal();
	


      visif->tltnodes = 0;
      visif->paths = 0;
      visif->constructPathAnalyzer(mg, false,VSlink[cfg.cfgForBeginning(isSgPragmaDeclaration(beginEnd.first))],VSlink[cfg.cfgForEnd(isSgPragmaDeclaration(beginEnd.second))]);
      std::cout << "there are " << visif->paths << " paths." << std::endl;
     delete visif;
     
     }
  }
 
