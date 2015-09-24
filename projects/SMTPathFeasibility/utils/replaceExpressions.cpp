#include "rose.h"
#include "utilHeader.h"



std::vector<SgExpression*> prefixes;
std::vector<SgExpression*> postfixes;

void print_tab_count(int count) {
	for (int i = 0; i < count; i++) {
		std::cout << "\t";
	}
}

void replaceExps(SgExpression* exp, SgVariableDeclaration* vardecl) {
	 if (isSgVarRefExp(exp)) {
                return;
        }
        else if (isSgUnaryOp(exp)) {
                if (isSgMinusMinusOp(exp) || isSgPlusPlusOp(exp)) {
                        SgExpression* operand = (isSgUnaryOp(exp))->get_operand();
			SgExpression* operand_cpy = SageInterface::copyExpression(operand);
			std::cout << "operand: " << operand->class_name() << std::endl;
			SageInterface::replaceExpression(exp,operand_cpy);
		}



                return;
        }
        else if (isSgBinaryOp(exp)) {
                replaceExps(isSgBinaryOp(exp)->get_lhs_operand(), vardecl);
                replaceExps(isSgBinaryOp(exp)->get_rhs_operand(), vardecl);
                return;
        }
        else {
                return;
        }
        return;
}




void getExps(SgExpression* exp, SgInitializedName* prevPost, std::vector<SgExpression*>& result, int tabcount) {
	
	if (isSgVarRefExp(exp)) {
		print_tab_count(tabcount);
		std::cout << "Var: " << SageInterface::get_name(isSgVarRefExp(exp)->get_symbol()) << std::endl;
		return;
	}
	else if (isSgUnaryOp(exp)) {
		print_tab_count(tabcount);
		std::cout << "un_op: " << exp->class_name() << std::endl;
		if (isSgMinusMinusOp(exp) || isSgPlusPlusOp(exp)) {
			SgExpression* operand = (isSgUnaryOp(exp))->get_operand();
		/*	if (isSgVarRefExp(operand)) {
				SageInterface::replaceExpression(exp,operand);
			}
			else {
				ROSE_ASSERT(false);
			}
		*/
			
			bool prefix = (isSgUnaryOp(exp)->get_mode() == SgUnaryOp::prefix);
				if (prefix) {
					SgExpression* exp_cpy = SageInterface::copyExpression(exp);
					prefixes.push_back(exp_cpy);
				}
				else {
					SgExpression* exp_cpy = SageInterface::copyExpression(exp);
					postfixes.push_back(exp_cpy);
				}
			
			
		}
		
		return;
	}
	else if (isSgBinaryOp(exp)) {
		result.push_back(exp);
		print_tab_count(tabcount);
		std::cout << "bin_op: " << exp->class_name() << ", precedence: " << exp->precedence() << std::endl;
		print_tab_count(tabcount);
		std::cout << "rhs:\n";
		getExps(isSgBinaryOp(exp)->get_lhs_operand(), prevPost, result,tabcount+1);
		print_tab_count(tabcount);
		std::cout << "lhs:\n";
		getExps(isSgBinaryOp(exp)->get_rhs_operand(), prevPost, result,tabcount+1);
		std::cout << std::endl;
		return;
	}
	else {
		result.push_back(exp);
		print_tab_count(tabcount);
		std::cout << "unknown node type: " << exp->class_name() << std::endl;
		return;
	}
	return;
}

int main(int argc, char** argv) {
	SgProject* proj = frontend(argc,argv);
	SgFunctionDeclaration* mainDecl = SageInterface::findMain(proj);
	SgFunctionDefinition* mainDef = mainDecl->get_definition();
	std::vector<SgNode*> assignNodes = NodeQuery::querySubTree(mainDef, V_SgVariableDeclaration);
	std::cout << assignNodes.size() << " nodes found" << std::endl;
	std::vector<SgBinaryOp*> bin_ops;
	std::vector<SgUnaryOp*> un_ops;
	std::vector<SgNode*> other;
	std::vector<SgExpression*> results;
	for (std::vector<SgNode*>::iterator i = assignNodes.begin(); i != assignNodes.end(); i++) {
	
		SgVariableDeclaration* vdecl = isSgVariableDeclaration(*i);
		SgInitializedNamePtrList vlst = vdecl->get_variables();
		SgInitializedName* initName = isSgInitializedName((*(vlst.begin())));
		SgExpression* exp = isSgAssignInitializer(initName->get_initializer())->get_operand();
		std::cout << exp->class_name() << std::endl;
		if (!isSgFunctionCallExp(exp)) {
			getExps(exp, isSgInitializedName(*i), results, 0);
		
	
	std::cout << "prefixes" << std::endl;
	for (int j = 0; j < prefixes.size(); j++) {
	        SgExprStatement* expSt = SageBuilder::buildExprStatement_nfi(prefixes[j]);
		SageInterface::insertStatement(isSgVariableDeclaration(*i),expSt,true);
		
		std::cout << prefixes[j]->class_name() << std::endl;
	}
	std::cout << "results" << std::endl;
	for (int j = 0; j < results.size(); j++) {
		std::cout << results[j]->class_name() << std::endl;
	}
	std::cout << "postfixes" << std::endl;
	for (int j = 0; j < postfixes.size(); j++) {
		SgExprStatement* expSt = SageBuilder::buildExprStatement_nfi(postfixes[j]);
                SageInterface::insertStatement(isSgVariableDeclaration(*i),expSt,false);
		std::cout << postfixes[j]->class_name() << std::endl;
	}
	
	replaceExps(exp,vdecl);
	}
		
	}
			
	backend(proj);
	return 0;
}
	
