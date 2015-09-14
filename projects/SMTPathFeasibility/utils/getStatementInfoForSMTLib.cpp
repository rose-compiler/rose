//#include <string>
//#include "rose.h"
#include "utilHeader.h"
/*
std::string convertVarRefExp(SgVarRefExp*);
std::string getSgCompoundAssignOp(SgCompoundAssignOp*,SgExpression*,SgExpression*);
std::string getSgStatement(SgStatement* stat);
std::string getNewVar(SgInitializedName*);
std::string getNewVar(SgVarRefExp*);
std::string getVarName(SgVarRefExp*);
std::string getSgBinaryOper(SgBinaryOp* op);
std::string getSgBinaryOp(SgBinaryOp*);
std::string getAssignOp(SgBinaryOp* binOp,SgExpression* lhs, SgExpression* rhs);
std::string getSgUnaryOp(SgUnaryOp* unaryOp);
std::string getSgValueExp(SgValueExp*);
void getSgBasicBlock(SgBasicBlock*);
*/
bool isLogicOp(SgBinaryOp* bin_op);
int rulenum = 0;
std::string getSgInitializedName(SgInitializedName* initName);
std::map<std::string, std::string> enums_defined;
std::set<SgVariableSymbol*> variablesAssigned;
std::map<SgIfStmt*,std::string> ifcondmap;
/*
//int initializeScopeInformation(SgProject*);
std::string getSExpr(SgExpression* expNode);
std::string convertAssignOp(SgBinaryOp* binOp, SgExpression* lhs, SgExpression* rhs);
std::string initializeVariable(SgInitializedName* initName);
std::map<SgSymbol*, std::string> SymbolToZ3;
std::map<SgSymbol*, int> SymbolToInstances;

std::map<SgVariableSymbol*,int> variableSymbolToInstanceIdentifier;
std::map<SgScopeStatement*,int> scopeStatementToInteger;
*/
std::map<int, SgScopeStatement*> scopeIntegerToStatement;
void getSgIfStmt(SgIfStmt* node);
/*
std::map<SgVariableSymbol*, std::string> varExpressionValue;
std::set<SgVariableSymbol*> varSymbolList;
std::map<SgVariableSymbol*, std::string> symbolToConvertedString;
*/
/*
std::vector<SgNode*> solved;

void reducePath(std::vector<SgGraphNode*>& path, StaticCFG::CFG* cfg) {
	int i = 0;
	
	while (i < path.size()) {
		if (isSgStatement(path[i]->get_SgNode())) {
			SgGraphNode* end = cfg->cfgForEnd(path[i]->get_SgNode());
			while (path[i] != end) {
				if (i >= path.size()) {
					break;
				}
				i++;
			}
		}
		i++;
	
	}
}
*/


bool isLogicOp(SgBinaryOp* bin_op) {
	if (isSgAndOp(bin_op) || isSgEqualityOp(bin_op) || isSgGreaterOrEqualOp(bin_op) || isSgGreaterThanOp(bin_op) || isSgLessOrEqualOp(bin_op) || isSgLessThanOp(bin_op) || isSgNotEqualOp(bin_op) || isSgOrOp(bin_op)) {
	return true;
	}
	return false;
}

std::map<SgIfStmt*,std::string> pathValue;	

std::set<SgStatement*> pathStatements;

int findLast(SgNode* n, std::vector<SgGraphNode*> path, int begin) {
	int last;
	int i = begin;
	while (i < path.size()) {
		if (path[i]->get_SgNode() == n) {
			last = i;
		}
		i++;
	}
	return last;
}


 			
void  evaluatePath(std::vector<SgGraphNode*> path, std::map<SgNode*,std::string> mapNodeToPathTruthValue, SgExpression*  bounding_condition, std::set<SgVariableSymbol*> vars, std::string& vardecls,std::string& rule, std::string& initrule) {
	

	pathStatements.clear();	
	for (int i = 0; i < path.size(); i++) {
	//	std::cout << "path["<<i<<"] = " << path[i]->get_SgNode()->class_name() << std::endl;
		if (isSgStatement(path[i]->get_SgNode())) {
			pathStatements.insert(isSgStatement(path[i]->get_SgNode()));
	//	 	std::cout << "path["<<i<<"] = " << path[i]->get_SgNode()->class_name() << std::endl;
		}
	}
	//std::cout << "pathStatements.size(): " << pathStatements.size() << std::endl;
	/*	if (isSgVarRefExp(path[i]->get_SgNode())) {
			variablesAssigned.insert(isSgVarRefExp(path[i]->get_SgNode())->get_symbol());
		}
	}*/
	
	pathValue.clear();
    	varExpressionValue.clear(); 
	variablesAssigned = vars;
	std::set<SgVariableSymbol*>::iterator v = variablesAssigned.begin();
	for (; v != variablesAssigned.end(); v++) {
		getVarName(*v);
	}

	
	//currentPathPtr = &path;
       //std::cout << "\n************* PATH ***************\n" << std::endl;
        std::vector<SgNode*> found;
	int i = 0;
	for (int j = 0; j < path.size(); j++) {
		if (isSgIfStmt(path[j]->get_SgNode()) && pathValue.find(isSgIfStmt(path[j]->get_SgNode())) == pathValue.end()) {
			ROSE_ASSERT(mapNodeToPathTruthValue.find(path[j]->get_SgNode()) != mapNodeToPathTruthValue.end());
			if (mapNodeToPathTruthValue[path[j]->get_SgNode()] == "true") {
			pathValue[isSgIfStmt(path[j]->get_SgNode())] = "true";
			}
			else if (mapNodeToPathTruthValue[path[j]->get_SgNode()] == "false") {
			pathValue[isSgIfStmt(path[j]->get_SgNode())] = "false";
			}
			else {
			 std::cout << "bad mapNodeToPathValue : " << mapNodeToPathTruthValue[path[j]->get_SgNode()] << std::endl;
			ROSE_ASSERT(false);
			}
		//std::cout << "pathValue at stmt: " << pathValue[isSgIfStmt(path[j]->get_SgNode())] << std::endl;
		}
		
	}
        while(i < path.size()) {
	    //   	std::cout << path[i]->get_SgNode()->class_name() << std::endl; 
		if (isSgFunctionDefinition(path[i]->get_SgNode())) {
                	getSgFunctionDefinition(isSgFunctionDefinition(path[i]->get_SgNode()));	
		}
		else if (isSgInitializedName(path[i]->get_SgNode())) {
			std::string initname = getSgInitializedName(isSgInitializedName(path[i]->get_SgNode()));
        //		std::cout << initname << std::endl;
		}
		else if (isSgScopeStatement(path[i]->get_SgNode())) {
		//else if (isSgScopeStatement(path[i]->get_SgNode()) && !isSgIfStmt(path[i]->get_SgNode())) {
			getSgScopeStatement(isSgScopeStatement(path[i]->get_SgNode()));
		}
		//else if (isSgIfStmt(path[i]->get_SgNode())) {
	//		getSgIfStmt(isSgIfStmt(path[i]->get_SgNode()));
	//	}
		/*else if (isSgBasicBlock(path[i]->get_SgNode())) {
			getSgBasicBlock(isSgBasicBlock(path[i]->get_SgNode()));
		}*/
		found.push_back(path[i]->get_SgNode());
		//std::cout << path[i]->get_SgNode()->class_name() << std::endl;
		i = findLast(path[i]->get_SgNode(),path,i);
		i = i + 1;
		
	}
	//std::cout << "results: " << std::endl;
	std::set<SgVariableSymbol*>::iterator k = variablesAssigned.begin();
	std::stringstream variables;
	for (;k != variablesAssigned.end(); k++) {
		
		variables << "(declare-var " << symbolToConvertedString[*k] << " " << get_type_identifier((*k)->get_type()) << ")\n";
	}
	vardecls = variables.str();	
	k = variablesAssigned.begin();
	std::string total = "(rule (=> ";
	std::stringstream init;
	std::stringstream relations;
	std::stringstream conditions;
	std::stringstream precedingAnds;
/*	for (;c!=ifcondmap.end();c++) {
		if (pathValue[c->first] == "true") {
			conditions << " " << c->second << std::endl;
		}
		else if (pathValue[c->first] == "false") {
			conditions <<  "(not " << c->second << std::endl;
		}
		else {
			ROSE_ASSERT(false);
		}
	}
*/
/*
	bool usecond = false;
	for (int q = 0; q < path.size(); q++) {
		if (isSgIfStmt(path[q]->get_SgNode())) {
			usecond = true;
		}
	}
	if (!usecond) {
		ifcondmap.clear();
	}
*/
	relations << "(declare-rel rule" << rulenum << "("; 
	init <<  "(rule" << rulenum << " "; 
	std::stringstream final;
	final << "(rule" << rulenum << " ";
	//rulenum += 1;
	for (;k != variablesAssigned.end(); k++) {
		relations << get_type_identifier((*k)->get_type()) << " "; 
		init << symbolToConvertedString[*k] + " ";
		//std::cout << symbolToConvertedString[*k] << std::endl;
		//std::cout << (*k)->get_name().getString() << std::endl;
		final << varExpressionValue[*k] + " ";
		//std::cout << varExpressionValue[*k] << std::endl;
	}
	relations << "))";
	init << ")";
	std::map<SgIfStmt*,std::string>::iterator c = ifcondmap.begin();
	precedingAnds << "(and ";
	for (;c!=ifcondmap.end();c++) {
                if (pathValue[c->first] == "true") {
			precedingAnds << "( and ";
                        conditions << c->second << ")" << std::endl;
                	//final << ")";
		}
                else if (pathValue[c->first] == "false") {
			precedingAnds << "( and ";
                        conditions <<  "(not " << c->second << "))" << std::endl;
			//final << ")";
                }
                else {
                        ROSE_ASSERT(false);
                }
        }
	varExpressionValue.clear();
	std::string bounding_condition_string = getSExpr(bounding_condition);
	conditions << bounding_condition_string << ")";	
	final << "))";
/*
	std::stringstream prelude;
       prelude << "(define-fun absolute ((x Int)) Int" << std::endl;
        prelude << " (ite (>= x 0) x (- x)))" << std::endl;
        prelude << "(define-fun cdiv ((x Int) (y Int)) Int" << std::endl;
        prelude << "(ite (or (and (< x 0) (< y 0)) (and (> x 0) (> y 0))) (div (absolute x) (absolute y)) (- 0 (div (absolute x) (absolute y)))))" << std::endl;
        prelude << "(define-fun cmod ((x Int) (y Int)) Int" << std::endl;
        prelude << "(ite (< x 0) (- (mod (absolute x) (absolute y))) (mod (absolute x) (absolute y))))" << std::endl;
        prelude << "(define-fun cbool ((b Bool)) Int" << std::endl;
        prelude << "(ite false 0 1))" << std::endl;
        prelude << "(define-fun cand ((b1 Bool) (b2 Bool)) Int" << std::endl;
        prelude << "(ite (or (= (cbool b1) 0) (= (cbool b2) 0)) 0 1))" << std::endl;
        prelude << "(define-fun cor ((b1 Bool) (b2 Bool)) Int" << std::endl;
        prelude << "(ite (and (= (cbool b1) 0) (= (cbool b2) 0)) 0 1))" << std::endl;
        prelude << "(define-fun cnot ((b1 Bool)) Int" << std::endl;
        prelude << "(ite false 1 0))" << std::endl;



	std::cout << prelude.str() << std::endl;
*/	
//std::cout << variables.str() << std::endl;
	vardecls = vardecls + relations.str();
	//std::cout << relations.str() << std::endl;
	std::stringstream rules;
	initrule = init.str();
	rules << total << "\n " << precedingAnds.str() << init.str() << conditions.str() << "\n" << final.str() << ")\n";
        //std::cout << "\n************* END PATH ****************\n" << std::endl;
	rule = rules.str();
	ifcondmap.clear();
        return;

}

void listSgSwitchStatements(SgSwitchStatement* switchStat) {
        std::cout << "unsure of what this should return..." << std::endl;
        listBodyStatements(switchStat);
        return;
}

bool isScopeWithBasicBlock(SgScopeStatement* scop) {
        return (isSgFunctionDefinition(scop));
}


bool isScopeWithBody(SgScopeStatement* scop) {
        return (isSgDoWhileStmt(scop) || isSgForStatement(scop) || isSgWhileStmt(scop));
}

void listBodyStatements(SgScopeStatement* scop) {
        SgStatement* body;
        if (isSgDoWhileStmt(scop)) {
                SgDoWhileStmt* scopWhile = isSgDoWhileStmt(scop);
                std::cout << "Do While Statement: " << std::endl;
               std::cout << "Condition: " << std::endl;
                SgStatement* condition = scopWhile->get_condition();
                if (condition != NULL) {
                if (isSgExprStatement(condition)) {
                        std::string s_expr_string = getSExpr(isSgExprStatement(condition)->get_expression());
                        std::cout << "condition expression: " << s_expr_string << std::endl;
                }
                else {
                        std::cout << "do while condition is not an SgExprStatement as expected, it is an " << condition->class_name() << std::endl;
                }
                }
                else {
                        std::cout << "condition is null" << std::endl;
                }

                body = isSgDoWhileStmt(scop)->get_body();
        }
        else if (isSgForStatement(scop)) {
                std::cout << "for statement: " << std::endl;
                std::cout << " init stmt " << std::endl;
                SgStatementPtrList statList = isSgForStatement(scop)->get_init_stmt();
                listStatementPtrList(statList);
                SgExpression* test_expression = isSgForStatement(scop)->get_test_expr();
                std::cout << "test expression" << std::endl;

                if (test_expression) {
                        std::string test_expr = getSExpr(test_expression);
                        std::cout << test_expr << std::endl;
                }
                else {
                        std::cout << "test expression is null" << std::endl;
                }
                body = (isSgForStatement(scop))->get_loop_body();
        }
        else if (isSgWhileStmt(scop)) {
                std::cout << "While Stmt" << std::endl;
                SgStatement* condition = isSgWhileStmt(scop)->get_condition();
                std::cout << "condition" << std::endl;
                if (condition) {
                        if (isSgExprStatement(condition)) {
                                std::string condstring = getSExpr(isSgExprStatement(condition)->get_expression());
                                std::cout << condstring << std::endl;
                        }
                        else {
                                std::cout << "the condition of the while loop is not an SgExprStatement as expected, it is an: " << condition->class_name() << std::endl;
                        
			}
                }
                else {
                        std::cout << "condition is null" << std::endl;
                }
                body = (isSgWhileStmt(scop))->get_body();
 }
        else {
                std::cout << "ERROR: listBodyStatements given a statement without a body: " << scop->class_name() << std::endl;
        }

        std::cout << "body statements" << std::endl;
                if (body) {
                        if (isSgBasicBlock(body)) {
                                listBasicBlockStats(isSgBasicBlock(body));
                        }
                        else {
                                std::cout << "body is not a basic block, please revise code to address this." << std::endl;
                                ROSE_ASSERT(false);
                        }
                }
                else {
                        std::cout << "body is null" << std::endl;
                }
        return;
}

void listBasicBlockStats(SgBasicBlock* bl) {
        SgStatementPtrList statList = bl->get_statements();
        listStatementPtrList(statList);
        return;
}
void listStatementPtrList(SgStatementPtrList statList) {
        SgStatementPtrList::iterator i = statList.begin();
        for (; i != statList.end(); i++) {
                if (isSgScopeStatement(*i)) {
                        std::cout << "INTERNAL SCOPE STAT" << std::endl;
                        listStatements(isSgScopeStatement(*i));
                        std::cout << "INTERNAL SCOPE STAT COMPLETE" << std::endl;
                }
                else {
			std::string statStr = getSgStatement(*i);
                        std::cout << "STATEMENT: " << statStr << std::endl;
                        std::cout << (*i)->class_name() << std::endl;
                }
        }
}

	

void listStatements(SgScopeStatement* currentScopeStat) {
        if (isSgBasicBlock(currentScopeStat)) {
                listBasicBlockStats(isSgBasicBlock(currentScopeStat));
        }
        else if (isScopeWithBody(currentScopeStat)) {
                listBodyStatements(currentScopeStat);
        }
        else if (isScopeWithBasicBlock(currentScopeStat)) {
                if (isSgFunctionDefinition(currentScopeStat)) {
                        SgFunctionDefinition* functionDefinition = isSgFunctionDefinition(currentScopeStat);
                        SgBasicBlock* body = functionDefinition->get_body();
                        listBasicBlockStats(body);
                }
                else {
                        std::cout << "scope with basic block is admitting a node without a way to translate it!" << std::endl;
                        ROSE_ASSERT(false);
                }

        }
        else if (isSgIfStmt(currentScopeStat)) {
                listSgIfStmts(isSgIfStmt(currentScopeStat));
        }
        else if (isSgSwitchStatement(currentScopeStat)) {
                listSgSwitchStatements(isSgSwitchStatement(currentScopeStat));
        }
        else {
                std::cout << "scope statement not yet covered" << std::endl;
        }
}



void listSgIfStmts(SgIfStmt* ifstmt) {
                        SgStatement* ifconditional = ifstmt->get_conditional();
                        if (isSgExprStatement(ifconditional)) {
                                std::string ifresult = getSExpr(isSgExprStatement(ifconditional)->get_expression());
                                std::cout << "if_result: " << ifresult;
				ifcondmap[ifstmt] = ifresult;
                        }
                        else {
                                std::cout << "ifconditional is not an SgExprStatement, it's a :" << ifconditional->class_name() << std::endl;
                        }
			if (pathValue[ifstmt] == "true") {
                        std::cout << "true body" << std::endl;
                        SgStatement* trueBody = ifstmt->get_true_body();
                        if (isSgBasicBlock(trueBody)) {
                                listBasicBlockStats(isSgBasicBlock(trueBody));
                        }
                        else if (isSgScopeStatement(trueBody)) {
                                listStatements(isSgScopeStatement(trueBody));
                        }
                        else {
                                std::cout << "true body is not a basic block or a scope statement, it is a: " << trueBody->class_name() << std::endl;
                        }
			}
			else if (pathValue[ifstmt] == "false") {
                        SgStatement* falseBody = ifstmt->get_false_body();
                        std::cout << "false body" << std::endl;
                        if (falseBody != NULL) {
                        if (isSgScopeStatement(falseBody)) {
                                listStatements(isSgScopeStatement(falseBody));
                        }
                        else {
                                std::cout << "false body is neither NULL nor scope statement, it is a : " << falseBody->class_name() << std::endl;
                                ROSE_ASSERT(false);
                        }
                        }
                        else {
                                std::cout << "false body is null" << std::endl;
                        }
			}
        return;
}




std::vector<std::string> evaluateBody(std::vector<SgStatement*> listOfStatements) {
        varExpressionValue.clear();
        std::vector<std::string> returnValue;
        for (int i = 0; i < listOfStatements.size(); i++) {
                SgStatement* stat = isSgStatement(listOfStatements[i]);
                if (isSgExprStatement(stat)) {
                        SgExpression* exp = (isSgExprStatement(stat))->get_expression();

                        std::string expString = "\n";
                        if (isSgFunctionCallExp(exp)) {
                                expString += getFunctionCallExpInfo(isSgFunctionCallExp(exp));
                        }
                        else {
                                expString += "SgExprStatement\n";
                                expString += getSExpr(exp);
                        }
                        returnValue.push_back(expString);
                        //expString = "\n/*\n " + expString + " \n*/\n";
                        SageBuilder::buildComment(stat,expString,PreprocessingInfo::after);

                }
                if (isSgVariableDeclaration(stat)) {
                        SgVariableDeclaration* varDecl = isSgVariableDeclaration(stat);
                        SgInitializedNamePtrList nameList = varDecl->get_variables();
                        std::string names;
                        for (SgInitializedNamePtrList::iterator q = nameList.begin(); q != nameList.end(); q++) {
                                SgType* nameType = (*q)->get_typeptr();
                                SgName nameName = (*q)->get_name();
                                names += "\ntype: " + nameType->class_name() + ", name: " + nameName.getString();

                                if ((*q)->get_initptr()) {
                                        SgInitializer* ip = (*q)->get_initptr();
                                        //names += "\nhas initptr of type: " + ip->class_name() + "\n";
                                        if (isSgAssignInitializer(ip)) {
                                                //names += "operand type: " + (isSgAssignInitializer(ip))->get_operand()->class_name();
                                                std::string oIf =  operandInfo(isSgAssignInitializer(ip)->get_operand());
                                                std::string var = getNewVar(*q);
                                                //std::string var2 = getNewVar(*q,false);

}
                                }
                                else {
                                        //names += ", has no initptr";
                                }
                                names += "\n";
                        }
                        returnValue.push_back(names);
                        SageBuilder::buildComment(stat,names,PreprocessingInfo::after);
                }
                if (isSgWhileStmt(stat)) {
                        std::string condition = writeWhileStatement(isSgWhileStmt(stat));
                        //while_conditions[isSgWhileStmt(stat)] = condition;
                        returnValue.push_back(condition);
                        SageBuilder::buildComment(stat->get_scope(),condition,PreprocessingInfo::before);
                        std::stringstream whilefunc;
                        //whileSMTFunctionName[isSgWhileStmt(stat)] = whilefunc.str();
                }
                if (isSgIfStmt(stat))
{
                        std::string retStat = writeIfStmt(isSgIfStmt(stat));
                        returnValue.push_back(retStat);
                        //return;
                }
/*
                        SgStatement* condstat = isSgIfStmt(stat)->get_conditional();
                        SgStatement* trueBody = isSgIfStmt(stat)->get_true_body();
                        SgStatement* falseBody = isSgIfStmt(stat)->get_false_body();
        
*/

                }

        return returnValue;
}

std::string writeWhileStatement(SgWhileStmt* whileStat) {
        std::cout << "in write while statement" << std::endl;
	SgStatement* condition = whileStat->get_condition();
        std::string conditionString = "";
        if (isSgExprStatement(condition)) {
                SgExpression* exp = (isSgExprStatement(condition))->get_expression();
                conditionString = constructConditional(exp);
        }
        else {
                std::cout << "while condition is not an SgExprStatement it is an: " << condition->class_name() << std::endl;
        }       
        return conditionString;
}         




std::string writeIfStmt(SgIfStmt* solve_if) {
        SgStatement* trueBody = solve_if->get_true_body();
        SgStatement* falseBody = solve_if->get_false_body();
        SgStatement* conditional = solve_if->get_conditional();
        std::string conditional_sexpr;
        std::vector<SgStatement*> true_body_stat_list = getStatementList(trueBody);
        std::vector<SgStatement*> false_body_stat_list = getStatementList(falseBody);
        ROSE_ASSERT(isSgExprStatement(conditional) || conditional == NULL);
        if (isSgExprStatement(conditional)) {
                conditional_sexpr = getSExpr(isSgExprStatement(conditional)->get_expression());
        }
        else {
                std::cout << "conditional is not an Expression Statement, rather it is: " << conditional->class_name() << std::endl;
        }
        std::map<SgVariableSymbol*, std::string> prevVarExpressionValue = varExpressionValue;
        std::vector<std::string> false_body_stats = evaluateBody(false_body_stat_list);
        std::map<SgVariableSymbol*, std::string> falseBodyVarExpressionValue = varExpressionValue;
        varExpressionValue = prevVarExpressionValue;
        std::vector<std::string> true_body_stats = evaluateBody(true_body_stat_list);
        std::map<SgVariableSymbol*, std::string> trueBodyVarExpressionValue = varExpressionValue;
        //merge_if_bodies(falseBodyVarExpressionValue,trueBodyVarExpressionValue, conditional_sexpr);
        //varExpressionValue = prevVarExpressionValue;
        std::cout << "conditional s-expression: " << conditional_sexpr << std::endl;

        std::cout << "false body" << std::endl;
        for (int i = 0; i < false_body_stats.size(); i++) {
                std::cout << i << ": " << false_body_stats[i] << std::endl;
        }
        std::cout << std::endl << "true body" << std::endl;
        for (int j = 0; j < true_body_stats.size(); j++) {
                std::cout << j << ": " << true_body_stats[j] << std::endl;
        }
        return "";
}

std::vector<SgStatement*> getStatementList(SgStatement* node) {
        std::vector<SgStatement*> statementList;
        Rose_STL_Container<SgNode*> nodes;
        if (isSgWhileStmt(node)) {
                nodes = NodeQuery::querySubTree(isSgWhileStmt(node),V_SgStatement);

        for (int i = 0; i < nodes.size(); i++) {
                ROSE_ASSERT(isSgStatement(nodes[i]));
                statementList.push_back(isSgStatement(nodes[i]));
        }
        }
        else if (isSgIfStmt(node)) {
                std::cout << "ifstmt" << std::endl;
                std::string res = writeIfStmt(isSgIfStmt(node));
                std::cout << res << std::endl;
        }
        //this should happen when you send a true/false body of an if statement
        else if (isSgBasicBlock(node)) {
                SgStatementPtrList lst = (isSgBasicBlock(node))->get_statements();
                for (SgStatementPtrList::iterator i = lst.begin(); i != lst.end(); i++) {
                        statementList.push_back(isSgStatement(*i));
                }
        }
        else if (isSgFunctionParameterList(node)) {
                std::cout << "no current evaluation for Function Parameter Lists, the parameters are:\n";
                SgInitializedNamePtrList args = isSgFunctionParameterList(node)->get_args();
                int argnum = 0;
                for (SgInitializedNamePtrList::iterator i = args.begin(); i != args.end(); ++i) {
                        std::string currentarg = (*i)->get_qualified_name().getString();
                        std::cout << "arg["<<argnum<<"] = " << currentarg << std::endl;
                }
        }

        else if (isSgFunctionDeclaration(node)) {
                std::cout << "no current evaluation for Function Declarations, currently at: " << isSgFunctionDeclaration(node)->get_name().getString() << std::endl;
        }

        else {

                std::cout << "no known evaluation for " << node->class_name() <<"!" << std::endl;
        }
        return statementList;
}



int initializeScopeInformation(SgProject* proj) {
        int scope_num = 0;
        std::string scopeInfoString = "";
        std::vector<SgNode*> scopeList = NodeQuery::querySubTree(proj,V_SgScopeStatement);
        for (std::vector<SgNode*>::iterator i = scopeList.begin(); i != scopeList.end(); i++) {
                SgScopeStatement* scopeStatement = isSgScopeStatement(*i);

                scopeInfoString += "Scope Statement Type: " + scopeStatement->class_name() + ", ";
                std::stringstream scopestr;
                scopestr << scope_num;
                scopeInfoString += "Scope Statement Integer Representation: " + scopestr.str();
                ROSE_ASSERT(scopeStatementToInteger.find(scopeStatement) == scopeStatementToInteger.end());
                scopeStatementToInteger[scopeStatement] = scope_num;
                scopeIntegerToStatement[scope_num] = scopeStatement;
                scope_num++;
        }
        return 0;
}


std::string get_type_identifier(SgType* symbolType) {
        std::string retstr;
        if (symbolType->isIntegerType()) {
                retstr += "Int";
        }
        else if (symbolType->isFloatType()) {
                retstr += "Real";
        }
        else if (isSgArrayType(symbolType)) {
                retstr += "arr_" + get_type_identifier(isSgArrayType(symbolType)->get_base_type());
        }
        else if (isSgPointerType(symbolType)) {
                retstr += "ptr_" + get_type_identifier(isSgPointerType(symbolType)->get_base_type());
        }
        else if (isSgEnumType(symbolType)) {
                retstr += "enum_" + isSgEnumType(symbolType)->get_name().getString();
        }
        else if (isSgFunctionType(symbolType)) {
                retstr += "func";
        }
        else {
                retstr += "unknown";
        }
        return retstr;
}


/*
std::string getAssignOp(SgBinaryOp* binOp,SgExpression* lhs,SgExpression* rhs) {
        std::string assignString = "";
        if (isSgVarRefExp(lhs)) {
                //std::string varName = getVarName(vAssign);

                std::string rhs_expr = getSExpr(rhs);
                convertAssignOp(binOp,lhs,rhs);
                std::string varName = getNewVar(isSgVarRefExp(lhs));
                assignString += "(= " + varName + " " + rhs_expr + ")";
        }
        else if (isSgPntrArrRefExp(lhs)) {
                assignString += "Currently cannot handle Pointer Array Reference Expressions";
        }
        else {
                assignString += "Currently can't handle left hand side of type: " + lhs->class_name();
        }
        return assignString;
}
*/


/*std::string getSExpr(SgExpression* expNode) {

        std::string expString = "";
        std::string convertedString = "";
        VariantT var = expNode->variantT();
        if (isSgBinaryOp(expNode)) {
                //expString += writeSgBinaryOp(isSgBinaryOp(expNode)); 
                SgBinaryOp* bin_op = isSgBinaryOp(expNode);
                SgExpression* lhs = bin_op->get_lhs_operand();
                SgExpression* rhs = bin_op->get_rhs_operand();
                if (isSgAssignOp(bin_op)) {
                        expString = getAssignOp(bin_op,lhs,rhs);
        //              convertedString = convertAssignOp(bin_op, lhs, rhs);
                }
                else if (isSgCompoundAssignOp(bin_op)) {
                        expString = getSgCompoundAssignOp(isSgCompoundAssignOp(bin_op),lhs,rhs);
                }
                else {
                        std::string bin_op_str = getSgBinaryOp(bin_op);
                        std::string lhs_exp_str = getSExpr(lhs);
                        std::string rhs_exp_str = getSExpr(rhs);
                        expString = "(" + bin_op_str + " " + lhs_exp_str + " " + rhs_exp_str + ")";
                }
        }

        else if (isSgUnaryOp(expNode)) {
                expString = getSgUnaryOp(isSgUnaryOp(expNode));
        }
        else if (isSgValueExp(expNode)) {
                expString = getSgValueExp(isSgValueExp(expNode));
        }
        else {
              switch (var) {
                case V_SgCallExpression:
                        expString="CALLEXP";
                        break;
                case V_SgClassNameRefExp:
                        expString="CLASSNAMEREF";
                        break;
                case V_SgConditionalExp:
                        expString="SGCONDITIONALEXP";
                        break;
                case V_SgExprListExp:
                        expString="SGEXPRLISTEXPR";
                        break;
          case V_SgFunctionRefExp:
                        expString="SGFUNCTIONREFEXP";
                        break;
                case V_SgDeleteExp:
                        expString="DELETE";
                        break;
                case V_SgInitializer:
                        expString="INITIALIZER";
                        break;
                case V_SgNaryOp:
                        expString="NARYOP";
                        break;
                case V_SgNewExp:
                        expString="NEWOP";
                        break;
                case V_SgNullExpression:
                        expString="NULL";
                        break;
                case V_SgRefExp:
                        expString="REFEXP";
                        break;
                case V_SgSizeOfOp:
                        expString="sizeOf";
                        break;
                case V_SgStatementExpression:
                        expString="STATEMENTEXPRESSION";
                        break;
                case V_SgValueExp:
                        std::cout << "V_SgValueExp should never be encountered" << std::endl;
                        ROSE_ASSERT(false);
                        expString = "";
                        break;
            case V_SgVarRefExp:
                        expString = convertVarRefExp(isSgVarRefExp(expNode));
                        break;
                default:
                        expString = expNode->class_name(); //+ " is not being considered for implementation";
                        break;
                }
                }
                return expString;
        }
*/




std::string getSgExprStatement(SgExprStatement* exprStat) {
        SgExpression* exprFromExprStat = exprStat->get_expression();
        std::string exprStr = getSExpr(exprFromExprStat);
        return exprStr;
}


std::string getSgInitializedName(SgInitializedName* initName) {
        std::string exprStr;
        SgSymbol* initNameSym = initName->search_for_symbol_from_symbol_table();
        std::string varInit = initializeVariable(initName);
        std::string retString;
        if (initName->get_initptr() != NULL) {
                SgInitializer* nameInitializer = initName->get_initializer();

                VariantT var = nameInitializer->variantT();

                switch (var) {
                        case V_SgAggregateInitializer:
                        {
                                SgAggregateInitializer* aggInit = isSgAggregateInitializer(nameInitializer);
                                if (!isSgArrayType(aggInit->get_type())) {
                                        std::cout << "currently only arrays use aggregate initializers, you are using " << aggInit->class_name() << std::endl;
                                        ROSE_ASSERT(false);
                                }
                                SgExprListExp* members = aggInit->get_initializers();
                                SgExpressionPtrList member_expressions = members->get_expressions();
                                std::string symName = SymbolToZ3[initNameSym];
                                ROSE_ASSERT(SymbolToInstances[initNameSym] == 0);
                                int arrmem = 0;
                                std::stringstream exprStream;
                                for (SgExpressionPtrList::iterator i = member_expressions.begin(); i != member_expressions.end(); i++) {

                        exprStream << "\n(assert (= (select " << symName << "_0 "  << arrmem << ") " << getSExpr((isSgAssignInitializer((*i))->get_operand())) << ")";
                                arrmem = arrmem+1;
                                }
                                retString = varInit + "\n" + exprStream.str();
                                #ifdef ARRAY_TEST
                                std::cout << "retString: " << retString << std::endl;
                                #endif

                        break;
                        }
                        case V_SgCompoundInitializer:
                        {
                        std::cout << "SgCompoundInitializer not yet supported" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }
                        case V_SgConstructorInitializer:
                        {
                        std::cout << "SgConstructorInitializer is not yet supported" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }
                        case V_SgDesignatedInitializer:
                        {
                        std::cout << "SgDesignatedInitializer is not yet supported" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }
                        case V_SgAssignInitializer:
                        {
                        SgAssignInitializer* assignInit = isSgAssignInitializer(nameInitializer);
                      std::string symName = SymbolToZ3[initNameSym];
                        ROSE_ASSERT(SymbolToInstances[initNameSym] == 0);
                        exprStr = "(assert (= " + symName + "_0 " + getSExpr(assignInit->get_operand()) + "))";
                        retString = varInit + "\n" + exprStr;
                        break;
                        }
                        default:
                        {
                        std::cout << "unknown initializer of type: " << nameInitializer->class_name() << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }
        }


        }
        else {
                retString = varInit;
        }

        return retString;
}


std::string getSgFunctionParameterList(SgFunctionParameterList* funcParamList) {
        SgInitializedNamePtrList pList = funcParamList->get_args();
        std::string paramReturnString;
        for (SgInitializedNamePtrList::iterator i = pList.begin(); i != pList.end(); i++) {
                std::string param = getSgInitializedName(*i);
                paramReturnString = paramReturnString + "\n" + param;
        }
        return paramReturnString;
}


void getSgFunctionDefinition(SgFunctionDefinition* funcDef) {
        SgFunctionDeclaration* funcDefDecl = funcDef->get_declaration();
        SgFunctionParameterList* funcParamList = funcDefDecl->get_parameterList();
        std::string disp = getSgFunctionParameterList(funcParamList);
        std::cout << "function parameter list result" << std::endl;
	std::cout << disp << std::endl;
	std::cout << "function parameter list result end" << std::endl;	
	//might need to do more later, for now all it does is instantiate its parameters
        SgBasicBlock* f_body = funcDef->get_body();
        getSgBasicBlock(f_body);
        return;

}


void getSgIfStmt(SgIfStmt* ifstat) {
        //fix truthbool
	bool truthBool=false;
       // scopeStatNum++;

        std::stringstream ifstatindexstream;
       // ifstatindexstream << scopeStatNum;
        std::string ifstatindex = ifstatindexstream.str();
        SgStatement* conditional = ifstat->get_conditional();
        ROSE_ASSERT(isSgStatement(conditional));
        std::string conditionalString = getSgStatement(conditional);
	ifcondmap[ifstat] =  conditionalString;
        if (conditionalString == "") {
                std::cout << "empty conditional string!" << std::endl;
                ROSE_ASSERT(false);
        }
        SgStatement* ifBody;

        std::string ifBodyString;
//	std::cout << "current path value: " << pathValue[ifstat] << std::endl;
	if (pathValue[ifstat] == "true") { truthBool = true; }
	else if (pathValue[ifstat] == "false") { truthBool = false; }
        else { ROSE_ASSERT(false); }
	if (!truthBool) { ifBody = ifstat->get_false_body(); }
        else { ifBody = ifstat->get_true_body(); }
        if (ifBody == NULL) {
        ifBodyString = "; empty body\n";
        }
        else {
                ROSE_ASSERT(isSgStatement(ifBody));
                ifBodyString =  getSgStatement(ifBody);
        }
        /*std::cout << "should be basic block..." << ifBody->class_name() << std::endl;
        ROSE_ASSERT(isSgBasicBlock(ifBody));
        getSgBasicBlock(isSgBasicBlock(ifBody));*/



        std::string fullIfStat;
        std::string assrt1, assrt2, assrt3;
        if (truthBool) {
                assrt3 = "(assert (= " + conditionalString + " true))\n";
        }
        else {
                assrt3 = "(assert (= " + conditionalString + " false))\n";
        }
        //expressions.push_back(assrt3);

        return;
}


void getSgBasicBlock(SgBasicBlock* basicBlock) {
        std::string basicBlockString;
        std::vector<std::string> basicBlockStrings;
        SgStatementPtrList statements = basicBlock->get_statements();
        for (SgStatementPtrList::iterator i = statements.begin(); i != statements.end(); i++) {
		if (pathStatements.find(*i) != pathStatements.end()) {
                std::string ithStatement = getSgStatement(*i);
                basicBlockStrings.push_back(ithStatement);
		}
        }
        if (basicBlockStrings.size() == 0) {
                basicBlockString = "";
        }
        else if (basicBlockStrings.size() == 1) {
                basicBlockString = "\n"  + basicBlockStrings[0];
        }
        else {
                basicBlockString = "\n";
        }
        return;
}


void getSgScopeStatement(SgScopeStatement* scopeStat) {
	//if (isSgIfStmt(scopeStat)) {
	//	listSgIfStmts(isSgIfStmt(scopeStat));
	//}
	//listStatements(scopeStat);	
        VariantT var = scopeStat->variantT();
        std::string scopeStatStr = "";
	std::vector<SgNode*> assignList = NodeQuery::querySubTree(scopeStat,V_SgAssignOp);
	/*for (int i = 0; i < assignList.size(); i++) {
		SgAssignOp* currAssign = isSgAssignOp(assignList[i]);
		SgStatement* currAssignStat = SageInterface::getEnclosingStatement(assignList[i]);
		if (pathStatements.find(currAssignStat) != pathStatements.end()) {
			SgBinaryOp* bin_op = isSgBinaryOp(currAssign);
			SgExpression* lhs = bin_op->get_lhs_operand();
			if (isSgVarRefExp(lhs)) {
				variablesAssigned.insert(isSgVarRefExp(lhs)->get_symbol());
			}
		}
	}*/		
        switch (var) {
                case V_SgBasicBlock:
                        {
                        getSgBasicBlock(isSgBasicBlock(scopeStat));
                        break;
                        }

                case V_SgCatchOptionStmt:
                        {
                        std::cout << "SgCatchOptionStmt is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgDoWhileStmt:
                        {
                        std::cout << "SgDoWhileStmt is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgForStatement:
                        {
                        std::cout << "SgForStatement is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgGlobal:
                        {
                        std::cout << "SgGlobal is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgIfStmt:
                        {
                        getSgIfStmt(isSgIfStmt(scopeStat));
                        break;
                        }

                case V_SgSwitchStatement:
                        {
                        std::cout << "SgSwitchStatement is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                      }

                case V_SgWhileStmt:
                        {
			std::vector<SgStatement*> stat= getStatementList(isSgWhileStmt(scopeStat));
                        //std::cout << "SgWhileStmt is not yet implemented" << std::endl;
                        //ROSE_ASSERT(false);
                        break;
                        }

                case V_SgForAllStatement:

                        {
                        std::cout << "SgForAllStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgAssociateStatement:

                        {
                        std::cout << "SgAssociateStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgBlockDataStatement:

                        {
                        std::cout << "SgBlockDataStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgNamespaceDefinitionStatement:

                        {
                        std::cout << "SgNamespaceDefinitionStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgClassDefinition:
                        {
                        std::cout << "SgClassDefinition should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgFunctionDefinition:
                        {
                        getSgFunctionDefinition(isSgFunctionDefinition(scopeStat));
                        break;
                        }

                case V_SgCAFWithTeamStatement:
                        {
                        std::cout << "SgCAFWithTeamStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                       break;
                        }

                case V_SgFortranDo:
                        {
                        std::cout << "SgFortranDo should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgFortranNonblockedDo:
                        {
                        std::cout << "SgFortranNonblockedDo should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgJavaForEachStatement:
                        {
                        std::cout << "SgJavaForEachStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgJavaLabelStatement:
                        {
                        std::cout << "SgJavaLabelStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgUpcForAllStatement:
                        {
                        std::cout << "SgUpcForAllStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                default:
                        {
                        std::cout << " Unknown node type!: " << scopeStat->class_name() << std::endl;
                        ROSE_ASSERT(false);
                        }
        }
        return;
}


std::string getSgIOStatement(SgIOStatement* ioStat) {
        std::cout << "io statements are not implemented yet" << std::endl;
        ROSE_ASSERT(false);
        return "";
}


void getSgDeclarationStatement(SgDeclarationStatement* declStat) {
        VariantT var = declStat->variantT();
        std::string declStatStr = "";
        switch (var) {
                case V_SgEnumDeclaration:
                        {
                        SgEnumDeclaration* enum_decl = isSgEnumDeclaration(declStat);
                        std::string enum_str_name = enum_decl->get_name().getString();
                        std::cout << ";enum_str_name = " << enum_str_name << std::endl;
                        if (enums_defined.find(enum_str_name) == enums_defined.end()) {
                                std::cout << ";enum not yet defined" << std::endl;
                                std::string enum_name = enum_decl->get_name().getString();
                                std::stringstream enum_z3;
                                enum_z3 << "(declare-datatypes () ((" << enum_name;
                                SgInitializedNamePtrList enumerated_values = enum_decl->get_enumerators();
                                for (SgInitializedNamePtrList::iterator j = enumerated_values.begin(); j != enumerated_values.end(); j++) {
                                        SgName enum_value_name = (*j)->get_name();
                                        enum_z3 << " " << enum_value_name.getString();
                                }
                                enum_z3 << ")))";
                                std::cout << enum_z3.str() << std::endl;
                                enums_defined[enum_str_name] = enum_z3.str();
                        }
                        break;
                        }

                case V_SgFunctionParameterList:
                        {
                        std::cout << "SgFunctionParameterList is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgTypedefDeclaration:
                        {
                        std::cout << "SgTypedefDeclaration is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgVariableDeclaration:
                        {
                        SgVariableDeclaration* varDecl = isSgVariableDeclaration(declStat);
                        SgInitializedName* nam = SageInterface::getFirstInitializedName(varDecl);
                /*      SgInitializedNamePtrList list = varDecl->get_variables();
                        std::stringstream declStatStrStream;
                        int checkOne = 0;
                        for (SgInitializedNamePtrList::iterator i = list.begin(); i != list.end(); i++) {
                                if (checkOne == 1) {
                                        std::cout << "InitializedNamePtrList should have only one InitializedName" << std::endl;
                                        ROSE_ASSERT(false);
                                }
                                SgInitializedName* ithName = isSgInitializedName(*i);
                                declStatStrStream << getSgInitializedName(*i) << "\n";
                                checkOne++;
                      }
                */
                        declStatStr = getSgInitializedName(nam) + "\n";

                        break;
                        }

                case V_SgVariableDefinition:
                        {
                        std::cout << "SgVariableDefinition is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgAttributeSpecificationStatement:

                        {
                        std::cout << "SgAttributeSpecificationStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgCommonBlock:

                        {
                        std::cout << "SgCommonBlock is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgEquivalenceStatement:

                        {
                        std::cout << "SgEquivalenceStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgImplicitStatement:

                        {
                        std::cout << "SgImplicitStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgNamelistStatement:

                        {
                        std::cout << "SgNamelistStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgNamespaceDeclarationStatement:

                        {
                        std::cout << "SgNamespaceDeclarationStatement is not yet implemented" << std::endl;
                       ROSE_ASSERT(false);
                        break;
                        }

                case V_SgTemplateInstantiationDirectiveStatement:

                        {
                        std::cout << "SgTemplateInstantiationDirectiveStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgUsingDeclarationStatement:

                        {
                        std::cout << "SgUsingDeclarationStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgUsingDirectiveStatement:

                        {
                        std::cout << "SgUsingDirectiveStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgClassDeclaration:
                        {
                        std::cout << "SgClassDeclaration should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgContainsStatement:
                        {
                        std::cout << "SgContainsStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgFunctionDeclaration:
                        {
                        std::cout << "SgFunctionDeclaration should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgTemplateDeclaration:
                        {
                        std::cout << "SgTemplateDeclaration should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgAsmStmt:
                        {
                        std::cout << "SgAsmStmt should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgC_PreprocessorDirectiveStatement:
                        {
                        std::cout << "SgC_PreprocessorDirectiveStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgClinkageDeclarationStatement:
                        {
                        std::cout << "SgClinkageDeclarationStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgCtorInitializerList:
                        {
                        std::cout << "SgCtorInitializerList should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgFormatStatement:
                        {
                        std::cout << "SgFormatStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgFortranIncludeLine:
                        {
                        std::cout << "SgFortranIncludeLine should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgImportStatement:
                        {
                        std::cout << "SgImportStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgInterfaceStatement:
                        {
                        std::cout << "SgInterfaceStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgJavaImportStatement:
                        {
                        std::cout << "SgJavaImportStatement should not be found here! " << std::endl;
                       ROSE_ASSERT(false);
                        break;
                        }

                case V_SgNamespaceAliasDeclarationStatement:
                        {
                        std::cout << "SgNamespaceAliasDeclarationStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgStmtDeclarationStatement:
                        {
                        std::cout << "SgStmtDeclarationStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgUseStatement:
                        {
                        std::cout << "SgUseStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgPragmaDeclaration:
                        {

                        //std::cout << "pragmas skipped" << std::endl;
                //      declStatStr=";;pragma\n";
                        break;
                        }
                default:
                        {
                        std::cout << " Unknown node type!: " << declStat->class_name() << std::endl;
                        ROSE_ASSERT(false);
                        }
        }
        //declarations.push_back(declStatStr);
        return;
}



std::string getSgStatement(SgStatement* stat) {
        VariantT var = stat->variantT();
        std::string statStr = "";
        if (isSgDeclarationStatement(stat)) {
                getSgDeclarationStatement(isSgDeclarationStatement(stat));
        }
	else if (isSgIfStmt(stat)) {
		getSgIfStmt(isSgIfStmt(stat));
	}	
        else if (isSgScopeStatement(stat)) {
                getSgScopeStatement(isSgScopeStatement(stat));
        }
        else if (isSgIOStatement(stat)) {
                statStr = getSgIOStatement(isSgIOStatement(stat));
        }
        else {
        switch (var) {
                case V_SgAllocateStatement:
                        {
                        std::cout << "SgAllocateStatement is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgArithmeticIfStatement:
                        {
                        std::cout << "SgArithmeticIfStatement is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgAssertStmt:
                        {
                        std::cout << "SgAssertStmt is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgAssignedGotoStatement:
                        {
                        std::cout << "SgAssignedGotoStatement is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgAssignStatement:
		            {
                        std::cout << "SgAssignStatement is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgBreakStmt:
                        {
                //        std::cout << "SgBreakStmt is not yet implemented" << std::endl;
                        //ROSE_ASSERT(false);
                        break;
                        }

                case V_SgCaseOptionStmt:
                        {
                        std::cout << "SgCaseOptionStmt is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgCatchStatementSeq:
                        {
                        std::cout << "SgCatchStatementSeq is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgComputedGotoStatement:
                        {
                        std::cout << "SgComputedGotoStatement is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgContinueStmt:
                        {
                        std::cout << "SgContinueStmt is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgDeallocateStatement:
                        {
             std::cout << "SgDeallocateStatement is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgDefaultOptionStmt:
                        {
                        std::cout << "SgDefaultOptionStmt is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgExprStatement:
                        {
                        statStr = getSgExprStatement(isSgExprStatement(stat));
                        break;
                        }

                case V_SgForInitStatement:
                        {
                        std::cout << "SgForInitStatement is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgFunctionTypeTable:
                        {
                        std::cout << "SgFunctionTypeTable is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgGotoStatement:
                        {
                        std::cout << "SgGotoStatement is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }
     case V_SgLabelStatement:
                        {
                        std::cout << "SgLabelStatement is not yet implemented" << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgReturnStmt:
                        {
                        SgReturnStmt* ret = isSgReturnStmt(stat);
                        SgExpression* retExp = ret->get_expression();
                        std::string retExpStr = getSExpr(retExp);
                        statStr = "; return statement not yet linked to function\n ;" + retExpStr + "\n";
                        break;
                        }

                case V_SgSequenceStatement:

                        {
                        std::cout << "SgSequenceStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgPassStatement:

                        {
                        std::cout << "SgPassStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgNullStatement:

                        {
                        std::cout << "SgNullStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgNullifyStatement:

                        {

                 std::cout << "SgNullifyStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgExecStatement:

                        {
                        std::cout << "SgExecStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgElseWhereStatement:

                        {
                        std::cout << "SgElseWhereStatement is not yet implemented" << std::endl; ;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgScopeStatement:
                        {
                        std::cout << "SgScopeStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgDeclarationStatement:
                        {
                        std::cout << "SgDeclarationStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgIOStatement:
                        {
			statStr = getSgIOStatement(isSgIOStatement(stat));	
                        break;
			}
  case V_SgJavaThrowStatement:
                        {
                        std::cout << "SgJavaThrowStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgJavaSynchronizedStatement:
                        {
                        std::cout << "SgJavaSynchronizedStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgOmpBarrierStatement:
                        {
                        std::cout << "SgOmpBarrierStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgOmpBodyStatement:
                        {
                        std::cout << "SgOmpBodyStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgOmpFlushStatement:
                        {
                        std::cout << "SgOmpFlushStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
                        }

                case V_SgOmpTaskwaitStatement:
                        {
                        std::cout << "SgOmpTaskwaitStatement should not be found here! " << std::endl;
                        ROSE_ASSERT(false);
                        break;
		    }

                default:
                        std::cout << "unknown SgStatement type!: " << stat->class_name() << std::endl;
                        ROSE_ASSERT(false);
        }
        }
        return statStr;
}



std::string getSgUnaryOp(SgUnaryOp* unaryOp) {
        std::string unaryOpString;
        VariantT var = unaryOp->variantT();
        switch (var) {
                case V_SgAddressOfOp:
                        unaryOpString = "ADDRESSOF";
                        break;
                case V_SgBitComplementOp:
                        unaryOpString = "BITCOMPLEMENT";
                        break;
                case V_SgCastExp:
                        {
                        SgExpression* casted = unaryOp->get_operand();
                        unaryOpString = getSExpr(casted);
                        break;
                        }

                case V_SgConjugateOp:
                        unaryOpString = "CONJUGATE";
                        break;
                case V_SgExpressionRoot:

                        unaryOpString = "EXPRESSIONROOT";
                        break;
                case V_SgImagPartOp:
                        unaryOpString = "IMAG";
                        break;
                case V_SgMinusMinusOp:
		{
			SgExpression* operand = unaryOp->get_operand();
			std::string toMinus = getSExpr(operand);
                        unaryOpString = "(- " + toMinus + " 1)";
                        break;
                }
		case V_SgMinusOp:
                        {
                        SgExpression* operand = unaryOp->get_operand();
                        std::string toMinus = getSExpr(operand);
                        std::string minusedExp;
                        minusedExp = "(- " + toMinus + ")";
                        unaryOpString = minusedExp;
                        break;
                        }
                case V_SgNotOp:
                        unaryOpString = "NOT";
                        break;
                                                                                      
              case V_SgPlusPlusOp:
              {
		          SgExpression* operand = unaryOp->get_operand();
			std::string toAdd = getSExpr(operand);
			unaryOpString = "(+ " + toAdd + " 1)";
                        break;
        	}       
	 case V_SgPointerDerefExp:
                        unaryOpString = "DEREF";
                        break;
                case V_SgRealPartOp:
                        unaryOpString = "REAL";
                        break;
                case V_SgThrowOp:
                        unaryOpString = "THROW";
                        break;
                case V_SgUnaryAddOp:
                        unaryOpString = "UNARYADD";
                        break;
                case V_SgUserDefinedUnaryOp:
                        unaryOpString = "USERDEFUNARYOP";
                        break;
                default:

                        std::cout << "Unknown unary op! " << unaryOp->class_name() << std::endl;
                        ROSE_ASSERT(false);
                        break;
                }
        return unaryOpString;
}

std::string getSgBinaryOp(SgBinaryOp* bin_op) {
	SgExpression* lhs = bin_op->get_lhs_operand();
	SgExpression* rhs = bin_op->get_rhs_operand();
	std::string expString = "";
	if (isSgAssignOp(bin_op)) {
		expString = getAssignOp(bin_op,lhs,rhs);
	}
	else if (isSgCompoundAssignOp(bin_op)) {
		expString = getCompoundAssignOp(isSgCompoundAssignOp(bin_op),lhs,rhs);
	}
	else {
		std::string bin_op_str = getSgBinaryOper(bin_op);
		std::string lhs_exp_str = getSExpr(lhs);
		std::string rhs_exp_str = getSExpr(rhs); 
		if (isLogicOp(bin_op)) {
			expString = "(= (" + bin_op_str + " " + lhs_exp_str + " " + rhs_exp_str + ") 1)";
		}
		else {
		expString = "(" + bin_op_str + " " + lhs_exp_str + " " + rhs_exp_str + ")";
		}
	}
	return expString;
}


std::string getSgBinaryOper(SgBinaryOp* op) {
        VariantT binOpVariant = op->variantT();
        std::string retStr;
        switch (binOpVariant) {
                case V_SgAddOp:
                        retStr = "+";
                        break;
                case V_SgSubtractOp:
                        retStr = "-";
                        break;
                case V_SgMultiplyOp:
                        retStr = "*";
                        break;
                case V_SgDivideOp:
                        retStr = "/";
                        break;
                case V_SgEqualityOp:
                        //retStr = "=";
			retStr = "ceq";
                        break;
                case V_SgGreaterThanOp:
                        //retStr = ">";
			retStr = "cgt";
                        break;
                case V_SgGreaterOrEqualOp:
                        //retStr = ">=";
			retStr = "cgeq";
                        break;
                case V_SgLessThanOp:
                        //retStr = "<";
			retStr = "clt";
                        break;
                case V_SgLessOrEqualOp:
                        //retStr = "<=";
			retStr = "cleq";
                        break;
                case V_SgIntegerDivideOp:
                        retStr = "cdiv";
                        break;
                case V_SgAssignOp:
                        retStr = "assign";
                        break;
                case V_SgNotEqualOp:
                        retStr = "cneq";
                        break;
                case V_SgModOp:
                        retStr = "cmod";
                        break;
                case V_SgPntrArrRefExp:
                        retStr = "select";
                        break;
                case V_SgAndOp:
                        retStr = "cand";
                        break;
                case V_SgOrOp:
                        retStr = "cor";
                        break;
                default:
                        retStr = "unknown";


                
        }
        return retStr;
}


std::string getVariableFinalState(SgVarRefExp* vRef) {
        
        std::vector<SgStatement*> vardefuse = analyzeVariableDefUse(vRef);
        std::cout << "past var def use" << std::endl;
        std::stringstream sExprs;
        std::vector<std::string> result = evaluateBody(vardefuse);
        for (int i = 0; i < result.size(); i++) {
                std::cout << "result[" << i << "] of evaluateBody: " << result[i] << std::endl; 
        }
        for (int i = 0; i < vardefuse.size(); i++) {
                if (isSgExprStatement(vardefuse[i])) {
                        SgExpression* variableExp = isSgExprStatement(vardefuse[i])->get_expression();
                sExprs << getSExpr(variableExp) << "\n";
                }
                else {
                        std::cout << "not a statement expresion: " << vardefuse[i]->class_name() << std::endl;
                }
        }
        return sExprs.str();
        
        return "";
} 


std::string getSgValueExp(SgValueExp* valExp) {
        VariantT var = valExp->variantT();
        std::stringstream val;
        switch (var) {
                case V_SgDoubleVal:
                        val << isSgDoubleVal(valExp)->get_value();
                        break;
                case V_SgFloatVal:
                        val << isSgFloatVal(valExp)->get_value();
                        break;
                case V_SgShortVal:
                        val << isSgShortVal(valExp)->get_value();
                        break;
                case V_SgLongIntVal:
                        val << isSgLongIntVal(valExp)->get_value();
                        break;
                case V_SgLongLongIntVal:
                        val << isSgLongLongIntVal(valExp)->get_value();
                        break;
                case V_SgIntVal:
                        val << isSgIntVal(valExp)->get_value();
                        break;
                case V_SgLongDoubleVal:
                        val << isSgLongDoubleVal(valExp)->get_value();
                        break;
                case V_SgEnumVal:
                      {
                        SgEnumVal* enumValExp = isSgEnumVal(valExp);
                        val << enumValExp->get_name().getString();
                        break;
                        }


                default:
                        std::cout << "SgValueExp : " << valExp->class_name() << std::endl;
                        ROSE_ASSERT(false);
                }
                return val.str();
        }

/*
std::string getSgBinaryOp(SgBinaryOp* op) {
        VariantT binOpVariant = op->variantT();
        std::string retStr;
        switch (binOpVariant) {
                case V_SgAddOp:
                        retStr = "+";
                        break;
                case V_SgSubtractOp:
                        retStr = "-";
                        break;
                case V_SgMultiplyOp:
                        retStr = "*";
                        break;
                case V_SgDivideOp:
                        retStr = "/";
                        break;
                case V_SgEqualityOp:
                        retStr = "=";
                        break;
                case V_SgGreaterThanOp:
                        retStr = ">";
                        break;
                case V_SgGreaterOrEqualOp:
                 retStr = ">=";
                        break;
                case V_SgLessThanOp:
                        retStr = "<";
                        break;
                case V_SgLessOrEqualOp:
                        retStr = "<=";
                        break;
                case V_SgIntegerDivideOp:
                        retStr = "cdiv";
                        break;
                case V_SgAssignOp:
                        retStr = "assign";
                        break;
                case V_SgNotEqualOp:
                        retStr = "neq";
                        break;
                case V_SgModOp:
                        retStr = "cmod";
                        break;
                case V_SgPntrArrRefExp:
                        retStr = "select";
                        break;
            case V_SgAndOp:
                        retStr = "and";
                        break;
                case V_SgOrOp:
                        retStr = "or";
                        break;
                default:
                        retStr = "unknown";
	}
	return retStr;
}
*/	
std::string getSExpr(SgExpression* expNode) {
        std::string expString = "";
        std::string convertedString = "";
	if (isSgBinaryOp(expNode)) {
		//std::cout << "found binaryop in s expression" << std::endl;
		std::string opstr = getSgBinaryOp(isSgBinaryOp(expNode));
		return opstr;
	}
	else if (isSgValueExp(expNode)) {
		//std::cout << "found value exp in s expression" << std::endl;
		std::string opstr = getSgValueExp(isSgValueExp(expNode));
		return opstr;
	}
	
        VariantT var = expNode->variantT();
        if (isSgBinaryOp(expNode)) {
		expString = getSgBinaryOp(isSgBinaryOp(expNode));
	}
	else if (isSgUnaryOp(expNode)) {
		expString = getSgUnaryOp(isSgUnaryOp(expNode));
	}
	else {
	switch (var) {
		case V_SgValueExp:
			expString = getSgValueExp(isSgValueExp(expNode));
			break;
                case V_SgCallExpression:
                        expString="CALLEXP";
			break;
		case V_SgClassNameRefExp:
                        expString="CLASSNAMEREF";
                        break;
                case V_SgConditionalExp:
                        expString="SGCONDITIONALEXP";
                        break;
                case V_SgExprListExp:
                        expString="SGEXPRLISTEXPR";
                        break;
                case V_SgFunctionRefExp:
                        expString="SGFUNCTIONREFEXP";
                        break;
                case V_SgDeleteExp:
                        expString="DELETE";
                        break;
                case V_SgInitializer:
                        expString="INITIALIZER";
                        break;
                case V_SgNaryOp:
                        expString="NARYOP";
                        break;
                case V_SgNewExp:
                        expString="NEWOP";
                        break;
                case V_SgNullExpression:
                        expString="NULL";
                        break;
                case V_SgRefExp:
                        expString="REFEXP";
                        break;
                case V_SgSizeOfOp:
                        expString="sizeOf";
                        break;
          	case V_SgStatementExpression:
                        expString="STATEMENTEXPRESSION";
                        break;
                case V_SgVarRefExp:
                        expString = convertVarRefExp(isSgVarRefExp(expNode));
                        break;
                default:
                        expString = "UNKNOWNVAL_" + expNode->class_name(); //+ " is not being considered for implementation";
                        break;
                }
	}
                
	return expString;
}

std::string getArgumentInfo(SgExpression* arg) {
        std::string argument_s_exp = getSExpr(arg);
        return argument_s_exp;
}

std::string getFunctionCallExpInfo(SgFunctionCallExp* fcall) {
        std::string retstr;
        std::string expString;
        if (fcall->getAssociatedFunctionDeclaration()) {
                SgFunctionDeclaration* functionDecl = fcall->getAssociatedFunctionDeclaration();
                SgName functionName = functionDecl->get_qualified_name();
                expString += "\nFunction Call To: "  + functionName.getString() + "\n";
                expString += "Arg Info:\n";
                SgExprListExp* args_list = fcall->get_args();
                SgExpressionPtrList call_args_list = args_list->get_expressions();
                for (SgExpressionPtrList::iterator expPtr = call_args_list.begin(); expPtr != call_args_list.end(); expPtr++) {
                        expString += getArgumentInfo(*expPtr) + "\n";
                }
                SgInitializedNamePtrList function_args = functionDecl->get_args();
                std::string argstr = "";
                int argnum = 0;
                for (SgInitializedNamePtrList::iterator i = function_args.begin(); i != function_args.end(); i++) {
                        SgInitializedName* ithArg = *i;
                        SgType* ithArgType = ithArg->get_type();
                        SgName ithArgName = ithArg->get_name();
                        argnum++;
                        argstr += "arg type: " + ithArgType->class_name() + ", arg name: " + ithArgName.getString() + "\n";
                }
                if (argnum == 0) {
                       argstr = "nullary op";
                }
                retstr = expString + "\n" + argstr + "\n" ;
        }
        else {
                retstr = " \nFunction Call Expression has no static resolution to Function Declaration\n";
        }
        return retstr;
}

std::string operandInfo(SgExpression* operand) {
        std::string opinfo = "";
        if (isSgFunctionCallExp(operand)) {
        opinfo += getFunctionCallExpInfo(isSgFunctionCallExp(operand));
        }
        else {
        opinfo += /*"Operand: " + operand->class_name() + "\n" +*/ getSExpr(operand);
        }
        return opinfo;
}



std::string getSgCompoundAssignOp(SgCompoundAssignOp* binOp,SgExpression* lhs, SgExpression* rhs) {
        std::string rhsString = getSExpr(rhs);
        VariantT var = binOp->variantT();
        std::string opStr = "";
        std::string retstr = "";
        switch (var) {
                case V_SgAndAssignOp:
                        {
                        opStr = "and";
                        break;
                        }

                case V_SgDivAssignOp:
                        {
                        opStr = "/";
                        break;
                        }

                case V_SgIntegerDivideAssignOp:
                        {
                        opStr = "cdiv";
                        break;
                        }
               case V_SgMinusAssignOp:
                        {
                        opStr = "-";
                        break;
                        }

                case V_SgModAssignOp:
                        {
                        opStr = "cmod";
                        break;
                        }

                case V_SgMultAssignOp:
                        {
                        opStr = "*";
                        break;
                        }

                case V_SgPlusAssignOp:
                        {
                     opStr = "+";
                        break;
                        }



                case V_SgExponentiationAssignOp:

                        {
                        opStr = "expontentiate";
                        break;
                        }

                case V_SgIorAssignOp:

                        {
                        opStr = "Ior";
                        break;
                        }

                case V_SgLshiftAssignOp:

                        {
                        opStr = "Lshift";
                        break;
                        }

                case V_SgRshiftAssignOp:

                        {
                        opStr = "Rshift";
                        break;
                        }

                case V_SgXorAssignOp:

                        {
                        opStr = "Xor";
                        break;
		}
              case V_SgJavaUnsignedRshiftAssignOp:
                        {
                        opStr = "JavaUnsignedRshiftAssignOp";
                        break;
                        }

                default:
                        {
                        opStr = "UnknownOperator" + binOp->class_name();
                        break;
                        }
        }
        if (isSgVarRefExp(lhs)) {
                SgVarRefExp* varRefExp = isSgVarRefExp(lhs);
                std::string varStr = getVarName(varRefExp);
                std::string varNew = getNewVar(varRefExp);
                retstr += "(= " + varNew + "(" + opStr + " " + varStr + " " + rhsString + "))";
        	std::cout << "retstr: " << retstr << std::endl;
	}
        else {
                retstr += "(= newVarVersion (" + opStr + " newVarVersion " + rhsString + "))";
        }
        return(retstr);
}

