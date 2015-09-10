#include "../utils/utilHeader.h"


// pathCollector will be filled with paths after this function
void collectPaths(SgGraphNode* begin, SgGraphNode* end, PathCollector* pathCollector) {
	std::vector<SgGraphNode*> visited;
	GraphAlgorithm::startNode = begin;
	visited.push_back(begin);
	GraphAlgorithm::DepthFirst(pathCollector,visited,end,-1,-1);	
}


void fixAllPrefixPostfix(SgProject* proj) {
/*	std::vector<SgNode*> stat_exps = NodeQuery::querySubTree(proj,V_SgExprStatement);
	for (int i = 0; i < stat_exps.size(); i++) {
		replacePrefixPostfixStat(stat_exps[i]);
	}
*/
	std::vector<SgNode*> unaries = NodeQuery::querySubTree(proj,V_SgUnaryOp);
//	std::cout << "unaries.size(): " << unaries.size() << std::endl;
	for (int i = 0; i < unaries.size(); i++) {
		//std::cout << "replacing... " << std::endl;
		SgScopeStatement* scop = SageInterface::getScope(unaries[i]);
		replacePrefixPostfixOp(unaries[i], scop);
		//std::cout << "replaced" << std::endl;
			
	}
	
	return;
}
			
// pragma lists are 
std::set<SgPragmaDeclaration*> collectPragmas(SgNode* queriedNode) {
	SgScopeStatement* enclosingScope = SageInterface::getEnclosingScope(queriedNode);
	//get pragmas
	SgStatementPtrList statementsInScope = enclosingScope->getStatementList();
	std::set<SgPragmaDeclaration*> pragmaDeclarations;
	SgStatementPtrList::iterator i = statementsInScope.begin();
	for (; i != statementsInScope.end(); i++) {
		if (isSgPragmaDeclaration(*i)) {
			pragmaDeclarations.insert(isSgPragmaDeclaration(*i));
		}
	}
	return 	pragmaDeclarations;	

}

std::string getWhileRule(PathCollector* pathCollector, std::vector<SgGraphNode*> path, std::set<SgVariableSymbol*> vars, SgWhileStmt* whileStmt,std::string& vardecls,std::string& initrule) {
	std::map<SgNode*,std::string> eckAssociations = pathCollector->getAssociationsForPath(path);
	//SgExpression* boundingCondition = getBoundingCondition(whileStmt);
	SgExprStatement* boundingConditionStatement = isSgExprStatement(whileStmt->get_condition());
	SgExpression* boundingCondition = boundingConditionStatement->get_expression();	
	std::string rule; 
	evaluatePath(path,eckAssociations,boundingCondition,vars,vardecls,rule,initrule);
	return rule;
}	

std::vector<std::string> getWhileRules(PathCollector* pathCollector,SgWhileStmt* whileStat,std::set<SgVariableSymbol*> vars, std::string& vardecls, std::string& initrule) {
	std::vector<std::vector<SgGraphNode*> > paths = pathCollector->getPaths();
	std::vector<std::string> rules;
	for (int i = 0; i < paths.size(); i++) {
		std::vector<SgGraphNode*> path = paths[i];
		std::string rule = getWhileRule(pathCollector,path,vars,whileStat,vardecls,initrule);
		rules.push_back(rule);
	}
	return rules;
}
std::vector<std::string> getRules(SgNode* nodeForRuleType,PathCollector* pathCollector,std::set<SgVariableSymbol*> vars, std::string& vardecls,std::string& initrule) {
	std::vector<std::string> rules;
	if (isSgWhileStmt(nodeForRuleType)) {
		rules = getWhileRules(pathCollector,isSgWhileStmt(nodeForRuleType),vars,vardecls,initrule);
	}
	else {
		std::cout << "currently can only support while statements" << std::endl;
	}
	return rules;
}




std::string queryPragma(std::vector<std::string> s_expressions, std::string initrule) {
	std::stringstream queryStringStream;
	std::stringstream queryEndStream;
	queryStringStream << "(and " << initrule << std::endl;
	
	for (unsigned int i = 0; i < s_expressions.size()-1; i++) {
		queryStringStream << "\t(and " << s_expressions[i] << std::endl;
		queryEndStream << ")";
	}
	queryStringStream << s_expressions[s_expressions.size()-1] << std::endl;
	queryStringStream << "\t" << queryEndStream.str() << "\n)" << std::endl;
	std::string beginString = "query";
	std::string preludeString = "; querying against user input\n";
	std::string queryResult = preludeString + "\n(" + beginString + queryStringStream.str() + ":print-certificate true)";
	return queryResult;
}	
	

std::string assumptionPragma(std::vector<std::string> s_expressions, std::string initrule) {
        std::stringstream queryStringStream;
        std::stringstream queryEndStream;
        for (unsigned int i = 0; i < s_expressions.size()-1;i++) {
                queryStringStream << "\t(and " << s_expressions[i] << std::endl;
                queryEndStream << ")";
        }
        queryStringStream << "\t";
        //queryStringStream << "(and ";
        queryStringStream << s_expressions[s_expressions.size()-1];// << std::endl;
        std::string preludeString = "; adding an initial condition\n";
        std::string beginString = "rule(=> \n";
        queryStringStream << "\t" << queryEndStream.str() << "\n";
        queryStringStream << "\t" << initrule << "\n\t)\n)" << std::endl;
        std::string queryResult = preludeString + "\n(" + beginString + queryStringStream.str();
	return queryResult; 
}




std::set<SgVariableSymbol*> getVars(PathCollector* pathCollector) {
	std::vector<std::vector<SgGraphNode*> > paths = pathCollector->getPaths();
	std::set<SgVariableSymbol*> vars;
	for (unsigned int i = 0; i < paths.size(); i++) {
		std::vector<SgGraphNode*> path = paths[i];
		for (unsigned int j = 0; j < path.size(); j++) {
		if (isSgVarRefExp(path[j]->get_SgNode())) {
			vars.insert(isSgVarRefExp(path[j]->get_SgNode())->get_symbol());
		}
		}
	}
	return vars;
}	



SgGraphNode* getWhileEndNode(SgWhileStmt* whileNode, PathCollector* pathCollector) {
	SgGraphNode* whileEnd;
	bool complete = false;
	std::vector<std::vector<SgGraphNode*> > paths = pathCollector->getPaths();
	for (unsigned int i = 0; i < paths.size(); i++) {
		std::vector<SgGraphNode*> path = paths[i];
		for (unsigned int j = 0; j < paths[i].size(); j++) {
			if (isSgWhileStmt(path[j]->get_SgNode()) == whileNode) {
				int index = pathCollector->getEquivCFGNodeIndex(path[j]);
				if (index == 2) {
					whileEnd = path[j];
					complete = true;
					break;
				}
			}
		}
		if (complete) {
			break;
		}
	}
	if (!complete) {
		std::cout << "couldn't find exit for SgWhileStmt!" << std::endl;
		ROSE_ASSERT(false);
	}
	return whileEnd;
}



std::string getPrelude() {
	std::string prelude = "(define-fun absolute ((x Int)) Int\n  (ite (>= x 0) x (- x)))\n (define-fun cdiv ((x Int) (y Int)) Int\n (ite (or (and (< x 0) (< y 0)) (and (> x 0) (> y 0))) (div (absolute x) (absolute y)) (- 0 (div (absolute x) (absolute y)))))\n (define-fun cmod ((x Int) (y Int)) Int\n (ite (< x 0) (- (mod (absolute x) (absolute y))) (mod (absolute x) (absolute y))))\n (define-fun cbool ((b Bool)) Int \n (ite false 0 1))\n (define-fun cand ((b1 Bool) (b2 Bool)) Int\n (ite (or (= (cbool b1) 0) (= (cbool b2) 0)) 0 1))\n (define-fun cor ((b1 Bool) (b2 Bool)) Int\n (ite (and (= (cbool b1) 0) (= (cbool b2) 0)) 0 1))\n (define-fun cnot ((b1 Bool)) Int\n (ite false 1 0))\n(define-fun cgeq ((a Int) (b Int)) Int (ite (>= a b) 1 0))\n(define-fun cgt ((a Int) (b Int)) Int (ite (> a b) 1 0))\n(define-fun cleq ((a Int) (b Int)) Int (ite (<= a b) 1 0))\n(define-fun clt ((a Int) (b Int)) Int (ite (< a b) 1 0))\n (define-fun ceq ((a Int) (b Int)) Int (ite (= a b) 1 0))\n(define-fun cneq ((a Int) (b Int)) Int (ite (distinct a b) 1 0))\n";
return prelude;
} 

