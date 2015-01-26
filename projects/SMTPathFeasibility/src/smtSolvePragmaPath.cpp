/*#include <iostream>
#include <fstream>
#include "rose.h"
#include <string>
#include <err.h>
#include "SgGraphTemplate.h"
#include "graphProcessing.h"
#include "staticCFG.h"
#include "smtQueryLib.h"
*//* Testing the graph traversal mechanism now implementing in AstProcessing.h (inside src/midend/astProcessing/)*/
/*#include <sys/time.h>
#include <sys/resource.h>
using namespace std;
using namespace boost;







typedef myGraph CFGforT;

myGraph* traversalMg;

SgIncidenceDirectedGraph* g;

StaticCFG::CFG* globalCFG;
*/
//#include "smtQueryLib.h"
#include "smtPathLibPragmaAware.h"
//#define CHECKENUMS 1

std::vector<std::string> pragma_suffix; 
class visitorTraversal : public SgGraphTraversal<CFGforT>
   {
     public:
          int tltnodes;
	  int paths;
	  int model;
          //std::vector<std::vector<VertexID> > pathstore;
	  void analyzePath(vector<VertexID>& pth);
   };

/*
std::map<SgIfStmt*, int> pathIfSt;

void parseBasicBlock(SgBasicBlock* bodypath) {
	SgStatementPtrList statList = bodypath->get_statements();
	for (SgStatementPtrList::iterator i = statList.begin(); i != statList.end(); i++) {
		if (isSgIfStmt(*i)) {
			ifst = isSgIfStmt(*i);
			std::cout << getSgIfStmt(ifst, pathIfSt[ifst]) << std::endl;
		}
		else if (isSgBasicBlock(*i)) {
			parseBasicBlock(isSgBasicBlock(*i));
		}
		else {
			std::cout << getSgStatement(*i) << std::endl;
		}
	}
}
*/
/*
std::string get_position(std::string prag_str) {
        if (prag_str.find(" begin ") != std::string::npos) {
                return "begin";
        }
        else if (prag_str.find(" end") != std::string::npos) {
                return "end";
        }
	else if (prag_str.find(" define ") != std::string::npos) {
		return "define";
	}
        ROSE_ASSERT(false);
        return "";
}
*/
/*
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
        //std::cout << "name: " << name << std::endl;

        return name;
}
*/
/*
std::vector<std::string> get_vars(std::string pstring) {
	std::vector<std::string> var_names;
	std::size_t begin_parse = pstring.find("[");
	std::size_t end_parse = pstring.find("]");
	bool begin_name = false;
	std::string white_space_string = pstring.substr(begin_parse,end_parse-begin_parse);
	std::string no_white_space_string = remove_white_space(white_space_string);
	int curr_int = 1;
	int begin_int = 1;
	ROSE_ASSERT(no_white_space_string[0] == "[");
	while (no_white_space_string[curr_int] != "]") {
		if (no_white_space_string[curr_int] != ",") {
			curr_int++;
		}
		else {
			std::string tmp_name = no_white_space_string.substr((size_t) begin_int, (size_t) curr_int-begin_int-1);
			var_names.push_back(tmp_name);
			begin_int = curr_int+1;
			curr_int++;
		}
	}
	std::string last_name = no_white_space_string.substr((size_t) begin_int,curr_int-begin_int-1);
	var_names.push_back(last_name);
	return (var_names);
}
*/	
		
		
/*
std::string get_pragma_string(SgPragmaDeclaration* prdecl) {
        ROSE_ASSERT(prdecl!=NULL);
        SgPragma* pr = prdecl->get_pragma();
        std::string prag = pr->get_pragma();
        return prag;
}
*/


	bool getCorrespondingPathNodeTruthValue(CFGNode cn1, CFGNode cn2) {
		vector<CFGEdge> ed = cn1.outEdges();
		CFGEdge ek;
		int k = 0;
		bool found = false;
		while (k < ed.size()) {
			if (ed[k].target() == cn2) {
				found = true;
				ek = ed[k];
				break;
			}
			k++;
		}
		if (!found) {
			std::cout << "Error: no edge found between two nodes" << std::endl;
			ROSE_ASSERT(false);
		}
		EdgeConditionKind kn = ek.condition();
		bool value = getConditionalValue(kn);
		return value;
	}

	bool getConditionalValue(EdgeConditionKind kn) {
		bool cond;
		if (kn == eckTrue) {
			cond = true;
		}
		else if (kn == eckFalse) {
			 cond = false;
		}
		else {
			std::cout << "unknown condition kind!" << std::endl;
			ROSE_ASSERT(false);
		}
		return cond;
	}

	
	/*			
		for (unsigned int k = 0; k < ed.size(); k++) {
			if (ed[k].target() == cn2) {
				ek = ed[k];
	void get_condition_edge_kind(SgGraphNode* pkk1, SgGraphNode* pkk2) {	
                                CFGNode cn1 = globalCFG->toCFGNode(pkk1);
                                CFGNode cn2 = globalCFG->toCFGNode(pkk2);
                                vector<CFGEdge> ed = cn1.outEdges();
                                CFGEdge ek;
                                bool found = false;
                                for (unsigned int k = 0; k < ed.size(); k++) {
                                        if (ed[k].target() == cn2) {
                                                found = true;
                                                ek = ed[k];
                                        }
                                }
                                ROSE_ASSERT(found);
                                EdgeConditionKind kn = ek.condition();
                                std::string SMTString;
                                ROSE_ASSERT(kn == eckTrue || kn == eckFalse);
                                if (kn == eckTrue) {
                                        SgIfStmt* st = isSgIfStmt(npth[i]->get_SgNode());
                                        setIf(st,1);
                                }
                                else if (kn == eckFalse) {
                                        SgIfStmt* st = isSgIfStmt(npth[i]->get_SgNode());
                                        setIf(st,0);
                                }
                }
        }
     	*/

std::string get_path_type(std::string prag_str) {
	if (prag_str.find("infeasible") != std::string::npos) {
		return "infeasible";
	}
	else if (prag_str.find("feasible") != std::string::npos) {
		return "feasible";
	}
	else {
	ROSE_ASSERT(false);
	return "";
	}
}

void check_pragmas(std::vector<SgGraphNode*> path) {
	std::set<std::string> names;
	std::vector<std::pair<std::string, std::string> > containedPragma;	
	std::map<std::string, std::string> pair_position;
	std::map<std::string,std::string> path_type_map;
	//std::cout << ";; debugging path" << std::endl;
	for (int i = 0; i < path.size(); i++) {
	//	std::cout << ";; " << path[i]->get_SgNode()->class_name() << std::endl;
		if (isSgPragmaDeclaration(path[i]->get_SgNode())) {
			
			std::string prag_string;
			prag_string = get_pragma_string(isSgPragmaDeclaration(path[i]->get_SgNode()));
			//if (isSMTGeneral(prag_string)) {
			//		parseSMTGeneral(prag_string);
			//}
			//else {
			/*
			if (isSMTGeneral(prag_string)) {
				if (isSMTGeneralLoop(prag_string)) {
					std::string unroll_number = parseSMTGeneral(prag_string);
					int unroll_literal = atoi(unroll_number.c_str());
					if (isSgForStatement(path[i]->get_SgNode())) {
						
						
			}
			*/
			std::string name;
			name = get_name(prag_string);
			std::string position;
			position = get_position(prag_string);
			
		if (position == "begin") {
			std::string path_type = get_path_type(prag_string);
	//		std::cout << ";;path type is: " << path_type << " with name: " << name <<  std::endl;	
			
			path_type_map[name] = path_type;
		}
		else if (position == "define") {
			std::vector<std::string> value = get_vars(prag_string);
			if (value.size() == 0) {
				std::cout << "define failed on string: " << prag_string << std::endl;
			}
		}	
		if (names.find(name) == names.end()) {
				pair_position[name] = position;
				names.insert(name);
			}
			else {
				if (pair_position.find(name) != pair_position.end() && pair_position[name] != position) {
					std::pair<std::string,std::string> name_and_type;	
					
					name_and_type.first = name;
					name_and_type.second = path_type_map[name];
					containedPragma.push_back(name_and_type);
				}
				else {
					std::cout << "pragma has two " << pair_position[name] << " values, we need both a begin and end" << std::endl;
				ROSE_ASSERT(false);
				}
			}
		}
	//}
	}
	if (containedPragma.size() != 0) {
		for (int j = 0; j < containedPragma.size(); j++) {
			std::string type = containedPragma[j].second;
			int flag;
			if (type == "infeasible") {
				flag = 0;
			}
			else {
				flag = 1;
			}
			std::string name = containedPragma[j].first;
			std::cout << ";; should be: " << type << " for pragma " << name << std::endl;
			std::cout << "(declare-fun pragma_" << containedPragma[j].first << " () Int)\n (assert (= pragma_"<<  containedPragma[j].first << " " << flag << "))" << std::endl;
			pragma_suffix.push_back(containedPragma[j].first);
		}
		containedPragma.clear();
	}
	return;
}
		
 void visitorTraversal::analyzePath(vector<VertexID>& pth) {
	bool incomplete = true;
	paths++;
	std::cout << "(push)" << std::endl;
	std::cout << ";;Path Number: " << paths << std::endl;
	std::cout << ";; path size: " << pth.size() << std::endl;
	tltnodes += pth.size();
	vector<SgGraphNode*> npth;

	for (int i = 0; i < pth.size(); i++) {
		npth.push_back((*traversalMg)[pth[i]].sg);
	//	std::cout << ";; npth " << i << ", " << npth.back()->get_SgNode()->class_name() << std::endl;
	}
	
	
	int currentPathPosition = 0;
	int i = 0;
	check_pragmas(npth);

	for (i = 0; i < npth.size(); i++) {
		SgGraphNode* curr_node = npth[i];
		VariantT n_var = curr_node->get_SgNode()->variantT();
		if (( n_var == V_SgOrOp || n_var == V_SgAndOp || n_var == V_SgIfStmt) && globalCFG->toCFGNode(npth[i]).getIndex() == 1) {
			CFGNode cn1 = globalCFG->toCFGNode(curr_node);
			if ((i+1) > npth.size()) {
				std::cout << "npth out of nodes at an index 1 conditional node of type: " << curr_node->get_SgNode()->class_name() << std::endl;
				ROSE_ASSERT(false);
			}
			SgGraphNode* next_node = npth[i+1];
			CFGNode cn2 = globalCFG->toCFGNode(next_node);
			bool n_path_val = getCorrespondingPathNodeTruthValue(cn1, cn2);
			setTruthValue(curr_node->get_SgNode(), n_path_val);
		}
		
	}			
			
		
	i = 0;
	while (i < npth.size()) {
        //for (int i = 0; i < npth.size(); i++) {
		if (globalCFG->toCFGNode(npth[i]).getIndex() == 0 && isSgStatement(npth[i]->get_SgNode())) {
			std::cout << getSgStatement(isSgStatement(npth[i]->get_SgNode())) << std::endl;
                        SgGraphNode* endNode = globalCFG->cfgForEnd(npth[i]->get_SgNode());
                        int j = i;
                        while (npth[j] != endNode) {
                                j++;
                        }
                        i = j;
                }

		
		else {
			i++;
		}
		
	}
/*
	std::cout << ";; header " << std::endl;
	std::cout << "(define-fun absolute ((x Int)) Int" << std::endl;
	std::cout << " (ite (>= x 0) x (- x)))" << std::endl;
	std::cout << "(define-fun cdiv ((x Int) (y Int)) Int" << std::endl;
	std::cout << "(ite (or (and (< x 0) (< y 0)) (and (> x 0) (> y 0))) (div (absolute x) (absolute y)) (- 0 (div (absolute x) (absolute y)))))" << std::endl;
	std::cout << "(define-fun cmod ((x Int) (y Int)) Int" << std::endl; 	   std::cout << "(ite (< x 0) (- (mod (absolute x) (absolute y))) (mod (absolute x) (absolute y))))" << std::endl;
*/
	std::cout << ";; variables" << std::endl;
	for (int i = 0; i < variables.size(); i++) {
		std::cout << variables[i] << std::endl;
	}
	std::cout << ";; declarations" << std::endl;
	for (int j = 0; j < declarations.size(); j++) {
		std::cout << declarations[j] << std::endl;
	}
	std::cout << ";; expressions" << std::endl;
	for (int k = 0; k < expressions.size(); k++) {
		std::cout << expressions[k] << std::endl;
	}
	std::cout << "( get-assertions ) " << std::endl;
	std::cout << "(check-sat)" << std::endl;
	for (int i = 0; i < pragma_suffix.size(); i++) {
		std::cout << "(get-value (pragma_" << pragma_suffix[i] << "))" << std::endl;
	}
	pragma_suffix.clear();
	#ifdef USE_MODEL
	if (model==1) {
	std::cout << "(get-model)" << std::endl;
	}
	#endif
	std::cout << "(pop)" << std::endl;			
	expressions.clear();
	declarations.clear();
	variables.clear();	
}
/*
void annotateScopeStatements(SgFunctionDefinition* mainDef) {
	Rose_STL_Container<SgNode*> scopeStatementsList = NodeQuery::querySubTree(mainDef, V_SgScopeStatement);
	for (Rose_STL_Container<SgNode*>::iterator i = scopeStatementsList.begin(); i != scopeStatementsList.end(); i++) {
		std::string scopeString = getSgScopeStatement(isSgScopeStatement(*i));
		std::cout << "scopeString on " << (*i)->class_name()  << scopeString << std::endl;
	}
}
*/
double timeDifference(const struct timeval& end, const struct timeval& begin)
{
    return (end.tv_sec + end.tv_usec / 1.0e6) - (begin.tv_sec + begin.tv_usec / 1.0e6);
}

static inline timeval getCPUTime() {
  rusage ru;
  getrusage(RUSAGE_SELF, &ru);
  return ru.ru_utime;
}

/*
int main(int argc, char *argv[]) {
*/

void runSMTQuery(int argc, char* argv[], int model=0) {  
  if_statement_count = 0;
  expression_count = 0;
  struct timeval t1, t2;
  SgProject* proj = frontend(argc,argv);
  ROSE_ASSERT (proj != NULL); 
  //stats = annotateTree(proj);
  SgFunctionDeclaration* mainDefDecl = SageInterface::findMain(proj);
  /*std::vector<SgFunctionDefinition*> functions =  SageInterface::querySubTree<SgFunctionDefinition> (proj);
*/	
	SgFunctionDefinition* mainDef = mainDefDecl->get_definition();

//	std::cout << "functions size " << functions.size() << std::endl;
//	if (mainDefDecl != NULL) {
 //  mainDef = mainDefDecl->get_definition();
//	}
//	else {
//	return;
//	}
/*
   for (std::vector<SgFunctionDefinition*>::iterator fdefdot = functions.begin(); fdefdot != functions.end(); fdefdot++) {
	SgFunctionDefinition* funDef = *fdefdot;
*/
	   string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";
    cfgToDot(mainDef,dotFileName1);
//	}
   //for (std::vector<SgFunctionDefinition*>::iterator fdef = functions.begin(); fdef != functions.end(); fdef++) {
   //SgFunctionDefinition* funDef = *fdef; 
   //visitorTraversal* vis = new visitorTraversal();
        #ifdef CHECKENUMS
        std::vector<SgEnumDeclaration*> enum_decls = SageInterface::querySubTree<SgEnumDeclaration> (proj);
        if (enum_decls.size() == 0) {
                std::cout << "undefine CHECKENUMS in smtPathLib.cpp" << std::endl;
                ROSE_ASSERT(false);
        }
        else {
                for (int i = 0; i < enum_decls.size(); i++) {
                        SgEnumDeclaration* enum_decl = enum_decls[i];

                        SgInitializedNamePtrList enumerated_values = enum_decl->get_enumerators();
			std::cout << "enumerated values number: " << enumerated_values.size() << std::endl;	
                        for (SgInitializedNamePtrList::iterator j = enumerated_values.begin(); j != enumerated_values.end(); j++) {
                                SgInitializedName* enum_decl_name = *j;
                                SgName enum_value_name = (*j)->get_name();
                                std::cout << "enumerated value: " << enum_value_name.getString() << std::endl;
                        }
                }
        }

#endif
    

ROSE_ASSERT(mainDef != NULL);
    //StaticCFG::CFG* cfg = new StaticCFG::CFG(mainDef);
    StaticCFG::CFG cfg(mainDef);
  
    SgIncidenceDirectedGraph* g = new SgIncidenceDirectedGraph();
    visitorTraversal* vis = new visitorTraversal();
    g = cfg.getGraph();
    traversalMg = new myGraph();   
    traversalMg = instantiateGraph(g,cfg);
    globalCFG = &cfg;
//cfg.buildFullCFG();
    stringstream ss;
    //string fileName= StringUtility::stripPathFromFileName(mainDef->get_file_info()->get_filenameString());
    //string dotFileName1=fileName+"."+ mainDef->get_declaration()->get_name() +".dot";

//    cfgToDot(funDef,dotFileName1); 
    //cfg->buildFullCFG();
/*
    g = new SgIncidenceDirectedGraph();
    g = cfg.getGraph();
    traversalMg = new myGraph();
    traversalMg = instantiateGraph(g, cfg);
*/
    vis->tltnodes = 0;
    vis->paths = 0;
    vis->model = model; 
    //vis->firstPrepGraph(constcfg);
    t1 = getCPUTime();
    //annotateScopeStatements(mainDef);
    
           std::cout << ";; header " << std::endl;
        std::cout << "(set-option :produce-models true)" << std::endl;
	std::cout << "(set-option :interactive-mode true)" << std::endl;
	std::cout << "(set-option :produce-assignments true)" << std::endl;
	std::cout << "(define-fun absolute ((x Int)) Int" << std::endl;
        std::cout << " (ite (>= x 0) x (- x)))" << std::endl;
        std::cout << "(define-fun cdiv ((x Int) (y Int)) Int" << std::endl;
        std::cout << "(ite (or (and (< x 0) (< y 0)) (and (> x 0) (> y 0))) (div (absolute x) (absolute y)) (- 0 (div (absolute x) (absolute y)))))" << std::endl;
        std::cout << "(define-fun cmod ((x Int) (y Int)) Int" << std::endl;        
	std::cout << "(ite (< x 0) (- (mod (absolute x) (absolute y))) (mod (absolute x) (absolute y))))" << std::endl;
	std::cout << "(define-fun cbool ((b Bool)) Int" << std::endl;
	std::cout << "(ite false 0 1))" << std::endl;
	std::cout << "(define-fun cand ((b1 Bool) (b2 Bool)) Int" << std::endl;
	std::cout << "(ite (or (= (cbool b1) 0) (= (cbool b2) 0)) 0 1))" << std::endl;
	std::cout << "(define-fun cor ((b1 Bool) (b2 Bool)) Int" << std::endl;
	std::cout << "(ite (and (= (cbool b1) 0) (= (cbool b2) 0)) 0 1))" << std::endl;
	std::cout << "(define-fun cnot ((b1 Bool)) Int" << std::endl;
	std::cout << "(ite false 1 0))" << std::endl; 
    vis->constructPathAnalyzer(traversalMg, true, 0, 0, true);
    t2 = getCPUTime();
    std::cout << ";took: " << timeDifference(t2, t1) << std::endl;
    //cfg.clearNodesAndEdges();
    std::cout << ";finished" << mainDef->get_declaration()->get_name() << std::endl;
    std::cout << ";tltnodes: " << vis->tltnodes << " paths: " << vis->paths << std::endl;
//	delete vis;
//	delete cfg;
//	delete g;
//	delete traversalMg;
	//delete g;
	//delete traversalMg;
	//delete globalCFG;
//	delete vis;	
	
//	}
//    delete vis;
    //return 0;
}

