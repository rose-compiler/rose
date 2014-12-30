#include "SMTPragmaBackend.h"
class visitorTraversal : public SgGraphTraversal<CFGforT>
	{
		public:
			int paths;
			void analyzePath(vector<VertexID>& pth);
			std::string vis_pragma;
			std::string vis_pragma_type;
	};


void disp_header() {
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
       return;
}

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

int check_pragma(std::vector<SgGraphNode*> path, std::string pragmaname) {
	int begin = -1;
	int end = -1;
	int type_flag = -1;
	for (int i = 0; i < path.size(); i++) {
		if (isSgPragmaDeclaration(path[i]->get_SgNode())) {
			std::string prag_string = get_pragma_string(isSgPragmaDeclaration(path[i]->get_SgNode()));
			std::string name = get_name(prag_string);
			if (name == pragmaname) {
				std::string position = get_position(prag_string);
				if (position == "begin") {
					begin = i;
					std::string type = get_path_type(prag_string);
					if (type == "infeasible") {
						type_flag = 0;
					}
					else if (type == "feasible") {
						type_flag = 1;
					}
					else {
						std::cout << "unknown type: " << type << std::endl;
					} 
				}
				else if (position == "end") {
					end = i;
				}
				else {
					std::cout << "unknown position value: " << position << std::endl;
					ROSE_ASSERT(false);
				}
			}
		}
	}
	if (end == -1 || begin == -1 || type_flag == -1) { 
		return -1;
	}
                        std::cout << "(declare-fun pragma_" << pragmaname << " () Int)\n (assert (= pragma_"<<  pragmaname << " " << type_flag << "))" << std::endl;
	return 0;
                        //pragma_suffix.push_back(containedPragma[j].first);
                }

				


void check_pragmas(std::vector<SgGraphNode*> path) {
        std::set<std::string> names;
        std::vector<std::pair<std::string, std::string> > containedPragma;  
        std::map<std::string, std::string> pair_position;
        std::map<std::string,std::string> path_type_map;
        //std::cout << ";; debugging path" << std::endl;
        for (int i = 0; i < path.size(); i++) {
        //      std::cout << ";; " << path[i]->get_SgNode()->class_name() << std::endl;
                if (isSgPragmaDeclaration(path[i]->get_SgNode())) {

                        std::string prag_string;
                        prag_string = get_pragma_string(isSgPragmaDeclaration(path[i]->get_SgNode()));
                        //if (isSMTGeneral(prag_string)) {
                        //              parseSMTGeneral(prag_string);
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
        //              std::cout << ";;path type is: " << path_type << " with name: " << name <<  std::endl;   

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



void printSMT(std::string name);

bool bool_val(SgGraphNode* node) {
        VariantT var_t = node->get_SgNode()->variantT();
        if ((var_t == V_SgOrOp || var_t == V_SgAndOp || var_t == V_SgIfStmt) && globalCFG->toCFGNode(node).getIndex() == 1) {
                return true;
        }
        else {
                return false;
        }
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


void process_branches(std::vector<SgGraphNode*> path) {
        for (int i = 0; i < path.size(); i++) {
                SgGraphNode* curr_node = path[i];
                if (bool_val(curr_node)) {
                        CFGNode cn1 = globalCFG->toCFGNode(curr_node);
                        if ((i+1) > path.size()) {
                                std::cout << "index out of bounds" << std::endl;
                        }
                        SgGraphNode* next_node = path[i+1];
                        CFGNode cn2 = globalCFG->toCFGNode(next_node);
                        bool path_val = getCorrespondingPathNodeTruthValue(cn1,cn2);
                        setTruthValue(curr_node->get_SgNode(), path_val);
                }
        }
        return;
}


void visitorTraversal::analyzePath(vector<VertexID>& pathR) {
        std::cout << "path: " << paths << std::endl;
        paths++;
       std::vector<SgGraphNode*> path;
        for (int j = 0; j < pathR.size(); j++) {
                SgGraphNode* graphNode = (*orig)[pathR[j]].sg;
                //std::cout << "node num: " << j << ": " << (graphNode->get_SgNode())->class_name() << std::endl;
                path.push_back(graphNode);
        }
        process_branches(path);
        int i = 0;
        //int j;
	std::cout << "checking pragma" << std::endl;
	int ret = check_pragma(path,vis_pragma);
	std::cout << "pragma checked" << std::endl;
       if (ret != -1) {
	while (i < path.size()) {
                if (globalCFG->toCFGNode(path[i]).getIndex() == 0 && isSgStatement(path[i]->get_SgNode())) {
                        //std::cout << "getSgStatement: " << isSgStatement(path[i]->get_SgNode())->class_name() << std::endl;
                        getSgStatement(isSgStatement(path[i]->get_SgNode()));
                        SgGraphNode* endNode = globalCFG->cfgForEnd(path[i]->get_SgNode());
                        if (endNode == globalCFG->cfgForBeginning(path[i]->get_SgNode())) {
				i++;
				ROSE_ASSERT(i < path.size());
                        }
                        else {
                        int j = i;
			ROSE_ASSERT(j < path.size());
                        while (path[j] != endNode) {
				if (j >= path.size()) {
					break;
				}
                                j++;
                        }
                        i = j;
                        }
                }

	               else {
                        i++;
                }

        }
	printSMT(vis_pragma);
	}
	else {
		std::cout << "path does not contain both beginning and end of pragma" << std::endl;
	}
}

void printSMT(std::string pragma_name) {
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
	std::cout << "(get-value (pragma_" << pragma_name << "))" << std::endl;	
}
/*
void initGraph(SgProject* proj) {
	SgFunctionDeclaration* mainDeclaration = SageInterface::findMain(proj);
	SgFunctionDefinition* mainDefinition = mainDeclaration->get_definition();
	StaticCFG::CFG cfg(mainDefinition);
	SgIncidenceDirectedGraph* g = cfg.getGraph();
	mg = new myGraph();

mg = instantiateGraph(g,cfg);
	orig = mg;
	globalCFG = &cfg;
	return;
}
*/
int findPragmaPaths(pragma_pair my_pair,SgProject* proj) {
	SgFunctionDeclaration* mainDeclaration = SageInterface::findMain(proj);
	SgFunctionDefinition* mainDefinition = mainDeclaration->get_definition();
	std::cout << "pragma_pair 1: " << my_pair.first->get_pragma()->get_pragma() << std::endl;
	std::cout << "pragma_pair 2: " << my_pair.second->get_pragma()->get_pragma() << std::endl;
	StaticCFG::CFG cfg(mainDefinition);
	SgIncidenceDirectedGraph* g = cfg.getGraph();
	mg = new myGraph();
	mg = instantiateGraph(g,cfg);
	orig = mg;
	globalCFG = &cfg;
	
	visitorTraversal* vis = new visitorTraversal();
	vis->paths = 0;
	std::string pstr = get_pragma_string(my_pair.first);
	std::string name = get_name(pstr);
	std::string type = get_path_type(pstr);
	vis->vis_pragma = name;//get_my_pair.first->get_pragma()->get_pragma()
	vis->vis_pragma_type = type;
	vis->constructPathAnalyzer(mg,false,VSlink[cfg.cfgForBeginning(isSgNode(my_pair.first))], VSlink[cfg.cfgForEnd(isSgNode(my_pair.second))]);
	std::cout << "path analyzed" << std::endl;
//	std::cout << "deletes begin" << std::endl;
//	delete g;
//	orig = NULL;
//	g = NULL;
//	globalCFG = NULL;
//	delete mg;
//	mg = NULL;
////	vis = NULL;	
       //delete vis;
//	std::cout << "deletes complete" << std::endl;	
	return 0;
}

pragma_pair get_named_pragma(SgProject* proj, std::string pragma_name) {

        pragma_pair my_pragma_pair;
        Rose_STL_Container<SgNode*> pragmaList = NodeQuery::querySubTree(proj,V_SgPragmaDeclaration);
        for (Rose_STL_Container<SgNode*>::iterator i = pragmaList.begin(); i != pragmaList.end(); i++) {
                SgPragmaDeclaration* prg = isSgPragmaDeclaration(*i);
                std::string prag_string = get_pragma_string(prg);
                std::string name = get_name(prag_string);
                if (name == pragma_name) {
                        std::string position = get_position(prag_string);
                        if (position == "begin") {
                                my_pragma_pair.first = prg;
                        }
                        else if (position == "end") {
                                my_pragma_pair.second = prg;
                        }
                        else {
                                std::cout << "invalid position value: " << position << std::endl;
                                ROSE_ASSERT(false);
                        }
                }
        }
	return my_pragma_pair;
}

