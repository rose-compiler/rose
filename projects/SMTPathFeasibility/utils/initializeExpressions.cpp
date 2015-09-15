#include "utilHeader.h"

void replacePrefixPostfixStat(SgNode* n) {
	if (isSgExprStatement(n) && isSgUnaryOp(isSgExprStatement(n)->get_expression())) {
		std::cout << "replacing" << std::endl;	
		SgUnaryOp* op = isSgUnaryOp(isSgExprStatement(n)->get_expression());
		if (isSgMinusMinusOp(op) || isSgPlusPlusOp(op)) {
		SgVarRefExp* p_var = isSgVarRefExp(op->get_operand());
		SgExpression* var = SageInterface::copyExpression(p_var);
		SgIntVal* one = SageBuilder::buildIntVal(1);
		if (isSgMinusMinusOp(op)) {

                       SgSubtractOp* sub = SageBuilder::buildSubtractOp(var, one);
                        SgAssignOp* assign = SageBuilder::buildAssignOp(var, sub);
               
			SgExprStatement* subStatement = SageBuilder::buildExprStatement(isSgExpression(assign));
			SageInterface::replaceStatement(isSgStatement(n),subStatement);
		}
		else {
			SgAddOp* add = SageBuilder::buildAddOp(var, one);
                        SgAssignOp* assign = SageBuilder::buildAssignOp(var
, add);
                        
                        SgExprStatement* addStatement = SageBuilder::buildExprStatement(isSgExpression(assign));
                        SageInterface::replaceStatement(isSgStatement(n),addStatement);
		}
	}
	}
	}
void replacePrefixPostfixOp(SgNode* n, SgScopeStatement* scop) {
	if (isSgMinusMinusOp(n) || isSgPlusPlusOp(n)) {
		SgUnaryOp::Sgop_mode m = isSgUnaryOp(n)->get_mode();
		std::string val;
		if (m == 0) {
			val = "prefix";
		}
		else if (m == 1) {
			val = "postfix";
		}
		else {
			std::cout << "error, prefix or postfix not found" << std::endl;
			return;
		}
		SgExpression* exp_1 = isSgUnaryOp(n)->get_operand();
		ROSE_ASSERT(isSgVarRefExp(exp_1));
		SgVarRefExp* p_var = isSgVarRefExp(exp_1);
		SgExpression* var_exp = SageInterface::copyExpression(p_var);
		
		SgVarRefExp* var = isSgVarRefExp(var_exp);	
		
		SgIntVal* inc = SageBuilder::buildIntVal(1);
		if (isSgMinusMinusOp(n)) {
		
			SgSubtractOp* sub = SageBuilder::buildSubtractOp(var, inc);
			SgAssignOp* assign = SageBuilder::buildAssignOp(var, sub);
			SgStatement* enclose = SageInterface::getEnclosingStatement(n);
			SgExprStatement* subStatement = SageBuilder::buildExprStatement(isSgExpression(assign)); 
			if (val == "prefix") {
				SageInterface::insertStatementBefore(enclose,subStatement);
			}
			else {
				ROSE_ASSERT(val == "postfix");
				SageInterface::insertStatementAfter(enclose,subStatement);
			}
		}
		else {
			SgAddOp* add = SageBuilder::buildAddOp(var,inc);
			
			SgAssignOp* assign = SageBuilder::buildAssignOp(var,add);
			SgStatement* enclose = SageInterface::getEnclosingStatement(n);
			SgExprStatement* addStatement = SageBuilder::buildExprStatement(isSgExpression(assign));
			ROSE_ASSERT(enclose != NULL);
			if (val == "prefix") {
				SageInterface::insertStatementBefore(enclose,addStatement);	
			}
			else {
				ROSE_ASSERT(val == "postfix");
				SageInterface::insertStatementAfter(enclose,addStatement);
			}
		}
		ROSE_ASSERT(isSgExpression(var));
	//	SgExpression* var2 = SageInterface::copyExpression(var);
	//	ROSE_ASSERT(isSgExpression(n) && isSgExpression(var2));
		SageInterface::replaceExpression(isSgExpression(n),isSgExpression(var));
		//SageInterface::deepDelete(n);
	}
	return;
}

