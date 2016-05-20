//#include "rose.h"
#include "../utils/utilHeader.h"
//#include "analyzePath.h"
//#include "../tools/collectPaths.h"

//int initializeScopeInformation(SgProject*);
//void getSgFunctionDefinition(SgFunctionDefinition*);
/*
void evalSgStatement(SgStatement* stat) {
	VariantT var = stat->variantT();
	switch (var) {
		case V_SgFunctionDefinition:
		{
			std::cout << "SGFUNCTIONDEFINITION" << std::endl;
			break;
		}
		case V_SgFunctionParameterList:
		{	
			std::cout << "SGFUNCTIONPARAMETERLIST" << std::endl;
			break;
		}
		case V_SgBasicBlock:
		{
			std::cout << "SGBASICBLOCK" << std::endl;
			break;
		}
		case V_SgVariableDeclaration:
		{
			SgVariableDeclaration* varDecl = isSgVariableDeclaration(declStat);
			SgInitializedName* nam = SageInterface::getFirstInitializedName(varDecl);
				
			std::cout << "SGVARIABLEDECLARATION" << std::endl;
			break;
		}
		case V_SgForStatement:
		{
			std::cout << "SGFORSTATEMENT" << std::endl;
			break;
		}
		case V_SgForInitStatement:
		{	
			std::cout << "SGFORINITSTATEMENT" << std::endl;
			break;
		}
		case V_SgExprStatement:
		{
			std::cout << "SGEXPRSTATEMENT" << std::endl;
			break;
		}
		case V_SgReturnStmt:
		{
			std::cout << "SGRETURNSTATEMENT" << std::endl;
			break;
		}
		default:
		{
			std::cout << "NOT YET COVERED, " << stat->class_name() << std::endl;
			break;
		}
	}
	return;

}
*/
/*
void evaluatePath(std::vector<SgGraphNode*> path) {

	

	std::cout << "\n************* PATH ***************\n" << std::endl;
	std::vector<SgNode*> found;
	for (int i = 0; i < path.size(); i++) {
		if (isSgFunctionDefinition(path[i]->get_SgNode())) {
			getSgFunctionDefinition(isSgFunctionDefinition(path[i]->get_SgNode()));
		}
	}
*/
/*
	if (find(found.begin(), found.end(), path[i]->get_SgNode()) == found.end()) {
//			if (isSgStatement(path[i]->get_SgNode())) {
//				std::cout << "Statement" << std::endl;
	//			evalSgStatement(isSgStatement(path[i]->get_SgNode()));
//			}
//			else {
//				std::cout << "Not a statement" << std::endl;
//			}
			std::stringstream indstr;
			indstr << "index " << path[i]->get_index() << ": " << path[i]->get_SgNode()->class_name() << std::endl;
			if (isSgStatement(path[i]->get_SgNode())) {	
			SageBuilder::buildComment(isSgStatement(path[i]->get_SgNode()),indstr.str(),PreprocessingInfo::after);
			}
			std::cout << "index " << path[i]->get_index() << ": ";
			std::cout << path[i]->get_SgNode()->class_name() << std::endl;

*//*			if (isSgStatement(path[i]->get_SgNode())) {
				std::cout << "found SgStatement:" << std::endl;
				std::string info = getSgStatement(isSgStatement(path[i]->get_SgNode()));
				std::cout << "info: " << info << std::endl;
			}
*//*
			found.push_back(path[i]->get_SgNode());
		}
	}
*/
/*
	std::cout << "\n************* END PATH ****************\n" << std::endl;

	return;

}
*/

int main(int argc,char** argv) {
	SgProject* proj = frontend(argc,argv);
	fixAllPrefixPostfix(proj);
	initializeScopeInformation(proj);	
	SgFunctionDeclaration* mainDecl = SageInterface::findMain(proj);

	SgFunctionDefinition* mainDef = mainDecl->get_definition();
	
	//SageInterface::rebuildSymbolTable(mainDef);
	StaticCFG::CFG cfg(mainDef);
	SgIncidenceDirectedGraph *g = cfg.getGraph();
	PathCollector* pathCollector = new PathCollector(g,&cfg);	

	std::vector<SgNode*> scopeNodes = NodeQuery::querySubTree(mainDef,V_SgScopeStatement);
	
	std::vector<SgGraphNode*> visited;
	std::vector<SgNode*> nodes = NodeQuery::querySubTree(mainDef,V_SgWhileStmt);
	std::vector<SgNode*>::iterator node = nodes.begin();
	for (; node != nodes.end(); node++) {
	SgScopeStatement* scopeOfWhile = SageInterface::getEnclosingScope(*node);
	SgStatementPtrList statementsInScope = scopeOfWhile->getStatementList();
	SgStatementPtrList::iterator statPtr = statementsInScope.begin();
	std::set<SgPragmaDeclaration*> prdecls;

	for (; statPtr!=statementsInScope.end();statPtr++) {
		if (isSgPragmaDeclaration(*statPtr)) {
			prdecls.insert(isSgPragmaDeclaration(*statPtr));
		}
	}
	//SgExprStatement* boundingConditionStatement = isSgExprStatement(isSgWhileStmt(*node)->get_condition()); 	
	//SgExpression* boundingCondition = boundingConditionStatement->get_expression();
	SgStatement* body = (isSgWhileStmt(*node)->get_body());
	std::vector<std::vector<SgGraphNode*> > paths = pathCollector->getPaths();

	
	std::cout << getPrelude() << std::endl;

		

	SgGraphNode* whileStart = cfg.cfgForBeginning(isSgWhileStmt(*node));		
	SgGraphNode* whileEnd = cfg.cfgForEnd(isSgWhileStmt(*node));
	collectPaths(whileStart,whileEnd, pathCollector);
	SgGraphNode* whileOut = getWhileEndNode(isSgWhileStmt(*node),pathCollector);
	SgGraphNode* bodyStart = cfg.cfgForBeginning(isSgWhileStmt(*node)->get_body());
	SgGraphNode* bodyEnd = cfg.cfgForEnd(isSgWhileStmt(*node)->get_body());

        pathCollector->clearPaths();
	
	collectPaths(bodyStart,whileOut,pathCollector);	
	paths.clear();
	paths = pathCollector->getPaths();
	std::vector<std::vector<SgGraphNode*> >::iterator i  = paths.begin();
	std::set<SgVariableSymbol*> vars = getVars(pathCollector);
	std::string vardecls;
	std::string initrule;
	std::vector<std::string> rules= getRules(*node,pathCollector, vars, vardecls,initrule);
	std::cout << vardecls << std::endl;
	for (int i = 0; i < rules.size(); i++) {
		std::cout << rules[i] << std::endl;
	}
	std::set<SgPragmaDeclaration*>::iterator pr = prdecls.begin();
	for (; pr != prdecls.end(); pr++) {
		std::set<std::string> variables;
		std::vector<std::string> s_expressions;
		std::string prag_str = get_pragma_string(*pr);
		bool initPrag;
		/*std::string rhs =*/ translateToS_Expr(prag_str,variables,s_expressions,initPrag);
		if (s_expressions.size() > 0) {
		std::string queryResult;
		if (initPrag) {
			queryResult = assumptionPragma(s_expressions,initrule);
		}
		else {
			queryResult = queryPragma(s_expressions,initrule);
		}	
		std::cout << queryResult << std::endl;
		}
	}
	}
	backend(proj);
	return 0;
	}

