
#include "RoseToFadaAST.hpp"
#include "RoseToFadaCommon.hpp"
#include "PMDAtoMDA.hpp"

using namespace fada;

namespace RoseToFada {

/***/

int statement_counter;

inline std::pair<std::string, std::vector<Expression*> *> parseSgPntrArrRefExp(SgPntrArrRefExp * array_ref_exp) {
	std::pair<std::string, std::vector<Expression*> *> res;
	res.second = new std::vector<Expression*>();
	
	SgPntrArrRefExp * tmp = array_ref_exp;
	do {
		if (tmp->attributeExists("PseudoMultiDimensionalArray")) {
			MultiDimArrays::PseudoMultiDimensionalArrayAttribute * attribute = 
				dynamic_cast<MultiDimArrays::PseudoMultiDimensionalArrayAttribute *>
					(tmp->getAttribute("PseudoMultiDimensionalArray"));

			if (!attribute) {
				std::cerr << "Error with PseudoMultiDimensionalArray in RoseToFadaAST !" << std::endl;
				ROSE_ASSERT(false);
			}

			for (int i = 0; i < attribute->nbrComponent(); i++)
				res.second->insert(res.second->begin(), parseSgExpressionToFadaExpression(attribute->buildComponent(i)));
		}
		else {
			res.second->insert(res.second->begin(), parseSgExpressionToFadaExpression(tmp->get_rhs_operand_i()));
		}
		array_ref_exp = tmp;
	} while (tmp = isSgPntrArrRefExp(tmp->get_lhs_operand_i()));			
					
	SgVarRefExp * var_ref_exp = isSgVarRefExp(array_ref_exp->get_lhs_operand_i());
	if (var_ref_exp == NULL) {
		Sg_File_Info * info = array_ref_exp->get_file_info();
		std::cerr << "Error in SgPntrArrRefExp: " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
	
	res.first = var_ref_exp->get_symbol()->get_name().getString();
	
	return res;
}

/***/

Statement * parseSgFunctionDeclarationToFadaAST(SgFunctionDeclaration * n) {
	statement_counter = 0;

	SgFunctionDefinition * func_def = isSgFunctionDefinition(n->get_definition());
	ROSE_ASSERT(func_def != NULL);

	SgBasicBlock * bb = isSgBasicBlock(func_def->get_body());
	ROSE_ASSERT(bb != NULL);
	
	Statement * ast = new Statement();
	
	parseSgStatementToFadaAST(bb, ast);
	
	return ast;
}

void parseSgStatementToFadaAST(SgStatement * stmt, Statement * parent) {
	switch (stmt->variantT()) {
		case V_SgBasicBlock:
		{
			SgBasicBlock * bb_stmt = isSgBasicBlock(stmt);
			ROSE_ASSERT(bb_stmt != NULL);
			SgStatementPtrList & stmt_list = bb_stmt->get_statements();
			SgStatementPtrList::iterator stmt_it;
			for (stmt_it = stmt_list.begin(); stmt_it != stmt_list.end(); stmt_it++) {
				parseSgStatementToFadaAST(*stmt_it, parent);
			}
			break;
		}
//		case V_SgVariableDeclaration:
//		{
//			parseDeclarationToFadaAST((SgVariableDeclaration*)node, ast);
//			break;
//		}
		case V_SgForStatement:
		{
			int current = statement_counter++;
			SgForStatement * for_stmt = isSgForStatement(stmt);
			ROSE_ASSERT(for_stmt != NULL);
			Statement * sub_ast = new Statement(parseSgForStatementToFadaControl(for_stmt));
			parseSgStatementToFadaAST(for_stmt->get_loop_body (), sub_ast);
			parent->Enclose(sub_ast);
			sub_ast->SetID(current);
			stmt->setAttribute("FadaStatement", new FadaStatementAttribute(current));
			break;
		}
		case V_SgExprStatement:
		{
			int current = statement_counter++;
			Statement * sub_ast = new Statement(parseSgExpressionToFadaAssignment(isSgExprStatement(stmt)->get_expression()));
			parent->Enclose(sub_ast);
			sub_ast->SetID(current);
			stmt->setAttribute("FadaStatement", new FadaStatementAttribute(current, true));
			break;
		}
		default:{}
	}
}

Control * parseSgForStatementToFadaControl(SgForStatement * for_stmt) {
	Control * ctrl;
	if (checkSgForStatementIsNormalized(for_stmt)) {
		ctrl = new Control(
			getSgForStatementIterator(for_stmt),
			getSgForStatementInitExp(for_stmt),
			getSgForStatementLimitExp(for_stmt)
		);
	}
	else {
		Sg_File_Info * info = for_stmt->get_file_info();
		std::cerr << "Error in SgForStatement isn't Normalized: " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
	return ctrl;
}
		
Assignment * parseSgExpressionToFadaAssignment(SgExpression * exp) {
	Assignment * assign;
	switch(exp->variantT()) {
		case V_SgAssignOp:
		{
			SgAssignOp * assign_op = isSgAssignOp(exp);
			
			SgExpression * lhs_exp = assign_op->get_lhs_operand_i();
			
			switch(lhs_exp->variantT()) {
				case V_SgPntrArrRefExp:
				{
					SgPntrArrRefExp * array_ref_exp = isSgPntrArrRefExp(lhs_exp);
					
					std::pair<std::string, std::vector<Expression*> *> res_parse = parseSgPntrArrRefExp(array_ref_exp);
					
					assign = new Assignment(
						res_parse.first,
						(FADA_Index*)res_parse.second,
						parseSgExpressionToFadaExpression(assign_op->get_rhs_operand_i())
					);
					
					break;
				}
				case V_SgVarRefExp:
				{
					SgVarRefExp * var_ref_exp = isSgVarRefExp(lhs_exp);
					assign = new Assignment(
						var_ref_exp->get_symbol()->get_name().getString(),
						parseSgExpressionToFadaExpression(assign_op->get_rhs_operand_i())
					);
					break;
				}
				default:
				{
					Sg_File_Info * info = lhs_exp->get_file_info();
					std::cerr << "Error unsupported operand " << lhs_exp->class_name() << " at " << info->get_filenameString() << " l." << info->get_line() << std::endl;
					ROSE_ASSERT(false);
				}
			}
			break;
		}
		case V_SgPlusPlusOp:
		{
			SgPlusPlusOp * plusplus_op = isSgPlusPlusOp(exp);
		
			SgExpression * operand_exp = plusplus_op->get_operand_i();
			
			switch(operand_exp->variantT()) {
				case V_SgPntrArrRefExp:
				{
					SgPntrArrRefExp * array_ref_exp = isSgPntrArrRefExp(operand_exp);
					
					std::pair<std::string, std::vector<Expression*> *> res_parse = parseSgPntrArrRefExp(array_ref_exp);
										
					assign = new Assignment(
						res_parse.first,
						(FADA_Index*)res_parse.second,
						new Expression(
							new Expression(
								FADA_array,
								res_parse.first,
								res_parse.second
							),
							FADA_ADD,
							new Expression("1")
						)
					);
					break;
				}
				case V_SgVarRefExp:
				{
					SgVarRefExp * var_ref_exp = isSgVarRefExp(operand_exp);
					assign = new Assignment(
						var_ref_exp->get_symbol()->get_name().getString(),
						new Expression(
							new Expression(var_ref_exp->get_symbol()->get_name().getString()),
							FADA_ADD,
							new Expression("1")
						)
					);
					break;
				}
				default:
				{
					Sg_File_Info * info = operand_exp->get_file_info();
					std::cerr << "Error unsupported operand " << operand_exp->class_name() << " at " << info->get_filenameString() << " l." << info->get_line() << std::endl;
					ROSE_ASSERT(false);
				}
			}
			break;
		}
		case V_SgMinusMinusOp:
		{
			SgMinusMinusOp * minusminus_op = isSgMinusMinusOp(exp);
		
			SgExpression * operand_exp = minusminus_op->get_operand_i();
			
			switch(operand_exp->variantT()) {
				case V_SgPntrArrRefExp:
				{
					SgPntrArrRefExp * array_ref_exp = isSgPntrArrRefExp(operand_exp);
					
					std::pair<std::string, std::vector<Expression*> *> res_parse = parseSgPntrArrRefExp(array_ref_exp);
										
					assign = new Assignment(
						res_parse.first,
						(FADA_Index*)res_parse.second,
						new Expression(
							new Expression(
								FADA_array,
								res_parse.first,
								res_parse.second
							),
							FADA_SUB,
							new Expression("1")
						)
					);
					break;
				}
				case V_SgVarRefExp:
				{
					SgVarRefExp * var_ref_exp = isSgVarRefExp(operand_exp);
					assign = new Assignment(
						var_ref_exp->get_symbol()->get_name().getString(),
						new Expression(
							new Expression(var_ref_exp->get_symbol()->get_name().getString()),
							FADA_SUB,
							new Expression("1")
						)
					);
					break;
				}
				default:
				{
					Sg_File_Info * info = operand_exp->get_file_info();
					std::cerr << "Error unsupported operand " << operand_exp->class_name() << " at " << info->get_filenameString() << " l." << info->get_line() << std::endl;
					ROSE_ASSERT(false);
				}
			}
			break;
		}
		default:
		{
			Sg_File_Info * info = exp->get_file_info();
			std::cerr << "Error unsupported operator " << exp->class_name() << " at " << info->get_filenameString() << " l." << info->get_line() << std::endl;
			ROSE_ASSERT(false);
		}
	}
	return assign;
}
		
Expression * parseSgExpressionToFadaExpression(SgExpression * exp) {
	Expression * res_exp;
	switch(exp->variantT()) {
		case V_SgMinusOp:
		{
			SgMinusOp * minus_op = isSgMinusOp(exp);
			res_exp = new Expression(
				new Expression("0"),
				FADA_SUB,
				parseSgExpressionToFadaExpression(minus_op->get_operand_i())
			);
			break;
		}
		case V_SgAddOp:
		{
			SgAddOp * add_op = isSgAddOp(exp);
			res_exp = new Expression(
				parseSgExpressionToFadaExpression(add_op->get_lhs_operand_i()),
				FADA_ADD,
				parseSgExpressionToFadaExpression(add_op->get_rhs_operand_i())
			);
			break;
		}
		case V_SgSubtractOp:
		{
			SgSubtractOp * substract_op = isSgSubtractOp(exp);
			res_exp = new Expression(
				parseSgExpressionToFadaExpression(substract_op->get_lhs_operand_i()),
				FADA_SUB,
				parseSgExpressionToFadaExpression(substract_op->get_rhs_operand_i())
			);
			break;
		}
		case V_SgMultiplyOp:
		{
			SgMultiplyOp * multiply_op = isSgMultiplyOp(exp);
			res_exp = new Expression(
				parseSgExpressionToFadaExpression(multiply_op->get_lhs_operand_i()),
				FADA_MUL,
				parseSgExpressionToFadaExpression(multiply_op->get_rhs_operand_i())
			);
			break;
		}
		case V_SgDivideOp:
		{
			SgDivideOp * divide_op = isSgDivideOp(exp);
			res_exp = new Expression(
				parseSgExpressionToFadaExpression(divide_op->get_lhs_operand_i()),
				FADA_DIV,
				parseSgExpressionToFadaExpression(divide_op->get_rhs_operand_i())
			);
			break;
		}
		case V_SgPntrArrRefExp:
		{
			SgPntrArrRefExp * array_ref_exp = isSgPntrArrRefExp(exp);
					
			std::pair<std::string, std::vector<Expression*> *> res_parse = parseSgPntrArrRefExp(array_ref_exp);
			
			res_exp = new Expression(
				FADA_array,
				res_parse.first,
				res_parse.second
			);
			break;
		}
		case V_SgVarRefExp:
		{
			SgVarRefExp * var_ref_exp = isSgVarRefExp(exp);
			res_exp = new Expression(var_ref_exp->get_symbol()->get_name().getString());
			break;
		}
		case V_SgIntVal:
		{
			SgIntVal * int_val = isSgIntVal(exp);
			res_exp = new Expression(int_val->get_valueString());
			break;
		}
		
		default:
		{
			Sg_File_Info * info = exp->get_file_info();
			std::cerr << "Error unsupported expression " << exp->class_name() << " at " << info->get_filenameString() << " l." << info->get_line() << std::endl;
			ROSE_ASSERT(false);
		}
	}
	return res_exp;
}

std::string getSgForStatementIterator(SgForStatement * for_stmt) {
	SgForInitStatement * init_stmt = for_stmt->get_for_init_stmt();
	SgExprStatement * exp_stmt = isSgExprStatement(init_stmt->get_init_stmt()[0]);
	if (exp_stmt == NULL) {
		Sg_File_Info * info = for_stmt->get_file_info();
		std::cerr << "Error in SgForInitStatement: " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
	SgAssignOp * assign_op = isSgAssignOp(exp_stmt->get_expression());
	if (assign_op == NULL) {
		Sg_File_Info * info = for_stmt->get_file_info();
		std::cerr << "Error in SgForInitStatement: " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
	SgVarRefExp * var_ref_exp = isSgVarRefExp(assign_op->get_lhs_operand_i());
	if (var_ref_exp == NULL) {
		Sg_File_Info * info = for_stmt->get_file_info();
		std::cerr << "Error in SgForInitStatement: " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
	return var_ref_exp->get_symbol()->get_name().getString();
}

fada::Expression * getSgForStatementInitExp(SgForStatement * for_stmt) {
	SgForInitStatement * init_stmt = for_stmt->get_for_init_stmt();
	SgExprStatement * exp_stmt = isSgExprStatement(init_stmt->get_init_stmt()[0]);
	if (exp_stmt == NULL) {
		Sg_File_Info * info = for_stmt->get_file_info();
		std::cerr << "Error in SgForInitStatement: " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
	SgAssignOp * assign_op = isSgAssignOp(exp_stmt->get_expression());
	if (assign_op == NULL) {
		Sg_File_Info * info = for_stmt->get_file_info();
		std::cerr << "Error in SgForInitStatement: " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
	SgExpression * exp = isSgExpression(assign_op->get_rhs_operand_i());
	if (exp == NULL) {
		Sg_File_Info * info = for_stmt->get_file_info();
		std::cerr << "Error in SgForInitStatement: " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
	return parseSgExpressionToFadaExpression(exp);
}

fada::Expression * getSgForStatementLimitExp(SgForStatement * for_stmt) {
	SgStatement * cond_stmt = for_stmt->get_test();
	SgExprStatement * exp_stmt = isSgExprStatement(cond_stmt);
	if (exp_stmt == NULL) {
		Sg_File_Info * info = for_stmt->get_file_info();
		std::cerr << "Error in SgForStatement->get_test(): " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
	SgBinaryOp * bin_op = isSgBinaryOp(exp_stmt->get_expression());
	if (bin_op == NULL) {
		Sg_File_Info * info = for_stmt->get_file_info();
		std::cerr << "Error in SgForInitStatement->get_test(): " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
	SgExpression * exp = isSgExpression(bin_op->get_rhs_operand_i());
	if (exp == NULL) {
		Sg_File_Info * info = for_stmt->get_file_info();
		std::cerr << "Error in SgForInitStatement->get_test(): " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
	if (isSgLessThanOp(bin_op))
		return new Expression(parseSgExpressionToFadaExpression(exp), FADA_SUB, new Expression("1"));
	else if (isSgLessOrEqualOp(bin_op))
		return parseSgExpressionToFadaExpression(exp);
	else {
		Sg_File_Info * info = for_stmt->get_file_info();
		std::cerr << "Error in SgForInitStatement->get_test(): " << info->get_filenameString() << " l." << info->get_line() << std::endl;
		ROSE_ASSERT(false);
	}
}

bool checkSgForStatementIsNormalized(SgForStatement * for_stmt) {
	// TODO
	return true;
}

}
