/**
 * \file src/rose/Annoter.cpp
 * \brief Implementation of Sage to SCoP translation mecanism.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

#include "rose/Parser.hpp"

#include <iostream>
#include <sstream>
#include <string>

namespace PolyhedricAnnotation {

RoseVariable getIterator(SgForStatement * for_stmt) {
	SgForInitStatement * init_stmt = for_stmt->get_for_init_stmt();
	SgExprStatement * exp_stmt = isSgExprStatement(init_stmt->get_init_stmt()[0]);
	if (exp_stmt == NULL)
		throw Exception::ExceptionForLoopTranslation(for_stmt, "Initialisation statement is not an Expression statement.");
	SgAssignOp * assign_op = isSgAssignOp(exp_stmt->get_expression());
	if (assign_op == NULL)
		throw Exception::ExceptionForLoopTranslation(for_stmt, "Initialisation expression is not an assignation.");
	SgVarRefExp * var_ref_exp = isSgVarRefExp(assign_op->get_lhs_operand_i());
	if (var_ref_exp == NULL)
		throw Exception::ExceptionForLoopTranslation(for_stmt, "Left operand of the initialisation assignation is not a variable reference.");
	
	return RoseVariable(var_ref_exp->get_symbol()->get_declaration());
}

int getIncrement(SgForStatement * for_stmt, const RoseVariable & iterator) {
	SgExpression * exp = isSgExpression(for_stmt->get_increment());
	if (exp == NULL)
		throw Exception::ExceptionForLoopTranslation(for_stmt, "Increment is not an expression.");
	int res = 0;
	SgValueExp * val = NULL;
	switch (exp->variantT()) {
		case V_SgAssignOp:
		{
			if (!isSgVarRefExp(isSgAssignOp(exp)->get_lhs_operand_i()) || !iterator.is(isSgVarRefExp(isSgAssignOp(exp)->get_lhs_operand_i())->get_symbol()->get_declaration()))
				throw Exception::ExceptionForLoopTranslation(for_stmt, "Wrong variable reference for assignation. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
			SgAssignOp * op = isSgAssignOp(exp);
			
			if (!isSgVarRefExp(op->get_lhs_operand_i()) || !iterator.is(isSgVarRefExp(op->get_lhs_operand_i())->get_symbol()->get_declaration()))
				throw Exception::ExceptionForLoopTranslation(for_stmt, "Wrong variable reference for assignation. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
			if (isSgAddOp(op->get_rhs_operand_i())) {
				SgAddOp * add = isSgAddOp(op->get_rhs_operand_i());
				res = 1;
				if (!isSgVarRefExp(add->get_lhs_operand_i()) || !iterator.is(isSgVarRefExp(add->get_lhs_operand_i())->get_symbol()->get_declaration()))
					throw Exception::ExceptionForLoopTranslation(for_stmt, "Wrong variable reference for assignation. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
				switch (add->get_rhs_operand_i()->variantT()) {
					case V_SgIntVal:
						res *= isSgIntVal(val)->get_value();
						break;
					case V_SgLongIntVal:
						res *= isSgLongIntVal(val)->get_value();
						break;
					case V_SgLongLongIntVal:
						res *= isSgLongLongIntVal(val)->get_value();
						break;
					case V_SgShortVal:
						res *= isSgShortVal(val)->get_value();
						break;
					case V_SgUnsignedIntVal:
						res *= isSgUnsignedIntVal(val)->get_value();
						break;
					case V_SgUnsignedLongLongIntVal:
						res *= isSgUnsignedLongLongIntVal(val)->get_value();
						break;
					case V_SgUnsignedLongVal:
						res *= isSgUnsignedLongVal(val)->get_value();
						break;
					case V_SgUnsignedCharVal:
						res *= isSgUnsignedCharVal(val)->get_value();
						break;
					case V_SgCharVal:
						res *= isSgCharVal(val)->get_value();
						break;
					case V_SgUnsignedShortVal:
						res *= isSgUnsignedShortVal(val)->get_value();
						break;
					default:
						throw Exception::ExceptionForLoopTranslation(for_stmt, "Value need to be an integer type. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
				}
			}
			else if (isSgMinusOp(op->get_rhs_operand_i())) {
				SgSubtractOp * minus = isSgSubtractOp(op->get_rhs_operand_i());
				res = -1;
				if (!isSgVarRefExp(minus->get_lhs_operand_i()) || !iterator.is(isSgVarRefExp(minus->get_lhs_operand_i())->get_symbol()->get_declaration()))
					throw Exception::ExceptionForLoopTranslation(for_stmt, "Wrong variable reference for assignation. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
				switch (minus->get_rhs_operand_i()->variantT()) {
					case V_SgIntVal:
						res *= isSgIntVal(val)->get_value();
						break;
					case V_SgLongIntVal:
						res *= isSgLongIntVal(val)->get_value();
						break;
					case V_SgLongLongIntVal:
						res *= isSgLongLongIntVal(val)->get_value();
						break;
					case V_SgShortVal:
						res *= isSgShortVal(val)->get_value();
						break;
					case V_SgUnsignedIntVal:
						res *= isSgUnsignedIntVal(val)->get_value();
						break;
					case V_SgUnsignedLongLongIntVal:
						res *= isSgUnsignedLongLongIntVal(val)->get_value();
						break;
					case V_SgUnsignedLongVal:
						res *= isSgUnsignedLongVal(val)->get_value();
						break;
					case V_SgUnsignedCharVal:
						res *= isSgUnsignedCharVal(val)->get_value();
						break;
					case V_SgCharVal:
						res *= isSgCharVal(val)->get_value();
						break;
					case V_SgUnsignedShortVal:
						res *= isSgUnsignedShortVal(val)->get_value();
						break;
					default:
						throw Exception::ExceptionForLoopTranslation(for_stmt, "Value need to be an integer type. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
				}
			}
			else
				throw Exception::ExceptionForLoopTranslation(for_stmt, "Unsupported operator. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
			break;
		}
		case V_SgPlusAssignOp:
		{
			if (!isSgVarRefExp(isSgAssignOp(exp)) || !iterator.is(isSgVarRefExp(isSgPlusAssignOp(exp)->get_lhs_operand_i())->get_symbol()->get_declaration()))
				throw Exception::ExceptionForLoopTranslation(for_stmt, "Wrong variable reference for assignation. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
			val = isSgValueExp(isSgPlusAssignOp(exp)->get_rhs_operand_i());
			if (!val)
				throw Exception::ExceptionForLoopTranslation(for_stmt, "Look for a value in += right side. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
			res = 1;
			break;
		}
		case V_SgMinusAssignOp:
		{
			if (!isSgVarRefExp(isSgMinusAssignOp(exp)->get_lhs_operand_i()) || !iterator.is(isSgVarRefExp(isSgMinusAssignOp(exp)->get_lhs_operand_i())->get_symbol()->get_declaration()))
				throw Exception::ExceptionForLoopTranslation(for_stmt, "Wrong variable reference for assignation. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
			val = isSgValueExp(isSgPlusAssignOp(exp)->get_rhs_operand_i());
			if (!val)
				throw Exception::ExceptionForLoopTranslation(for_stmt, "Look for a value in -= right side. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
			res = -1;
			break;
		}
		case V_SgPlusPlusOp:
		{
			if (!isSgVarRefExp(isSgPlusPlusOp(exp)->get_operand_i()) || !iterator.is(isSgVarRefExp(isSgPlusPlusOp(exp)->get_operand_i())->get_symbol()->get_declaration()))
				throw Exception::ExceptionForLoopTranslation(for_stmt, "Wrong variable reference for ++ operator. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
			res = 1;
			break;
		}
		case V_SgMinusMinusOp:
		{
			if (!isSgVarRefExp(isSgMinusMinusOp(exp)->get_operand_i()) || !iterator.is(isSgVarRefExp(isSgMinusMinusOp(exp)->get_operand_i())->get_symbol()->get_declaration()))
				throw Exception::ExceptionForLoopTranslation(for_stmt, "Wrong variable reference for -- operator. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
			res = -1;
			break;
		}
		default:
			throw Exception::ExceptionForLoopTranslation(for_stmt, "Unsupported operator. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
	}
	if (val != NULL) {
		 switch (val->variantT()) {
			case V_SgIntVal:
				res *= isSgIntVal(val)->get_value();
				break;
			case V_SgLongIntVal:
				res *= isSgLongIntVal(val)->get_value();
				break;
			case V_SgLongLongIntVal:
				res *= isSgLongLongIntVal(val)->get_value();
				break;
			case V_SgShortVal:
				res *= isSgShortVal(val)->get_value();
				break;
			case V_SgUnsignedIntVal:
				res *= isSgUnsignedIntVal(val)->get_value();
				break;
			case V_SgUnsignedLongLongIntVal:
				res *= isSgUnsignedLongLongIntVal(val)->get_value();
				break;
			case V_SgUnsignedLongVal:
				res *= isSgUnsignedLongVal(val)->get_value();
				break;
			case V_SgUnsignedCharVal:
				res *= isSgUnsignedCharVal(val)->get_value();
				break;
			case V_SgCharVal:
				res *= isSgCharVal(val)->get_value();
				break;
			case V_SgUnsignedShortVal:
				res *= isSgUnsignedShortVal(val)->get_value();
				break;
			default:
				throw Exception::ExceptionForLoopTranslation(for_stmt, "Value need to be an integer type. Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
		}
	}

	if (res != 1 && res != -1)
		throw Exception::ExceptionForLoopTranslation(for_stmt, "Assume \"i = i + a or i += a or i++\" and substraction equivalent, where \"i\" is the iterator associated to the loop (and a = 1).");
	
	return res;
}

RoseVariable * translateVarRef(SgExpression * var_ref) {
	SgDotExp * dot = isSgDotExp(var_ref);
	if (dot) {
		SgVarRefExp * rhs = isSgVarRefExp(dot->get_rhs_operand_i());
		if (!rhs)
			throw Exception::ExceptionLinearExpressionTranslation(dot, "RHS of a '.' operand need to be a variable reference.");
		return new RoseVariable(rhs->get_symbol()->get_declaration(), translateVarRef(dot->get_lhs_operand_i()), false);
	}
	SgArrowExp * arrow = isSgArrowExp(var_ref);
	if (arrow) {
		SgVarRefExp * rhs = isSgVarRefExp(arrow->get_rhs_operand_i());
		if (!rhs)
			throw Exception::ExceptionLinearExpressionTranslation(arrow, "RHS of a '->' operand need to be a variable reference.");
		return new RoseVariable(rhs->get_symbol()->get_declaration(), translateVarRef(arrow->get_lhs_operand_i()), true);
	}
	SgVarRefExp * var = isSgVarRefExp(var_ref);
	if (var) {
		return new RoseVariable(var->get_symbol()->get_declaration());
	}	
	throw Exception::ExceptionLinearExpressionTranslation(var_ref, "In a linear expression, variable can only be struct or classic variable (SgDotExp, SgArrowExp, SgVarRefExp).");
}

void translateLinearExpression(SgExpression * lin_exp, std::map<RoseVariable, int> & res, int coef_) {	
	int coef;
	switch (lin_exp->variantT()) {
		case V_SgMultiplyOp:
		{
			std::map<RoseVariable, int> lhs;
			std::map<RoseVariable, int> rhs;
			
			translateLinearExpression(isSgBinaryOp(lin_exp)->get_lhs_operand_i(), lhs, coef_ );
			translateLinearExpression(isSgBinaryOp(lin_exp)->get_rhs_operand_i(), rhs, coef_ );
			
			std::map<RoseVariable, int>::iterator it;
			if (lhs.size() == 1 && isConstant(lhs.begin()->first)) {
				int coef = lhs.begin()->second;
				for (it = rhs.begin(); it != rhs.end(); it++)
					res.insert(std::pair<RoseVariable, int>(it->first, coef_ * coef * it->second));
			}
			else if (rhs.size() == 1 && isConstant(rhs.begin()->first)) {
				int coef = rhs.begin()->second;
				for (it = lhs.begin(); it != lhs.end(); it++)
					res.insert(std::pair<RoseVariable, int>(it->first, coef_ * coef * it->second));
			}
			else
				throw Exception::ExceptionLinearExpressionTranslation(lin_exp, "Not linear.");
			break;
		}
		case V_SgAddOp:
		{
			translateLinearExpression(isSgBinaryOp(lin_exp)->get_lhs_operand_i(), res, coef_);
			translateLinearExpression(isSgBinaryOp(lin_exp)->get_rhs_operand_i(), res, coef_);
			break;
		}
		case V_SgSubtractOp:
		{
			translateLinearExpression(isSgBinaryOp(lin_exp)->get_lhs_operand_i(), res, coef_);
			translateLinearExpression(isSgBinaryOp(lin_exp)->get_rhs_operand_i(), res, -coef_);
			break;
		}
		case V_SgDotExp:
		case V_SgArrowExp:
		case V_SgVarRefExp:
		{
			res.insert(std::pair<RoseVariable, int>(*translateVarRef(lin_exp), coef_));
			break;
		}
		case V_SgIntVal:
			coef = isSgIntVal(lin_exp)->get_value();
			res.insert(std::pair<RoseVariable, int>(RoseVariable(), coef_ * coef));
			break;
		case V_SgLongIntVal:
			coef = isSgLongIntVal(lin_exp)->get_value();
			res.insert(std::pair<RoseVariable, int>(RoseVariable(), coef_ * coef));
			break;
		case V_SgLongLongIntVal:
			coef = isSgLongLongIntVal(lin_exp)->get_value();
			res.insert(std::pair<RoseVariable, int>(RoseVariable(), coef_ * coef));
			break;
		case V_SgShortVal:
			coef = isSgShortVal(lin_exp)->get_value();
			res.insert(std::pair<RoseVariable, int>(RoseVariable(), coef_ * coef));
			break;
		case V_SgUnsignedIntVal:
			coef = isSgUnsignedIntVal(lin_exp)->get_value();
			res.insert(std::pair<RoseVariable, int>(RoseVariable(), coef_ * coef));
			break;
		case V_SgUnsignedLongLongIntVal:
			coef = isSgUnsignedLongLongIntVal(lin_exp)->get_value();
			res.insert(std::pair<RoseVariable, int>(RoseVariable(), coef_ * coef));
			break;
		case V_SgUnsignedLongVal:
			coef = isSgUnsignedLongVal(lin_exp)->get_value();
			res.insert(std::pair<RoseVariable, int>(RoseVariable(), coef_ * coef));
			break;
		case V_SgUnsignedCharVal:
			coef = isSgUnsignedCharVal(lin_exp)->get_value();
			res.insert(std::pair<RoseVariable, int>(RoseVariable(), coef_ * coef));
			break;
		case V_SgCharVal:
			coef = isSgCharVal(lin_exp)->get_value();
			res.insert(std::pair<RoseVariable, int>(RoseVariable(), coef_ * coef));
			break;
		case V_SgUnsignedShortVal:
			coef = isSgUnsignedShortVal(lin_exp)->get_value();
			res.insert(std::pair<RoseVariable, int>(RoseVariable(), coef_ * coef));
			break;
		default:
			std::cerr << lin_exp->class_name() << std::endl;
			throw Exception::RoseUnknownNode("Linear expression", lin_exp);
	}
}

void translateLinearExpression(SgExpression * lin_exp, std::vector<std::pair<RoseVariable, int> > & res, bool neg) {
	std::map<RoseVariable, int> map;
	translateLinearExpression(lin_exp, map, neg ? -1 : 1);
	res.insert(res.begin(), map.begin(), map.end());
}

RoseVariable * parseArray(SgPntrArrRefExp * array, std::vector<std::vector<std::pair<RoseVariable, int> > > & av) {

	av.insert(av.begin(), std::vector<std::pair<RoseVariable, int> >());
	PolyhedricAnnotation::translateLinearExpression(array->get_rhs_operand_i(), av.front());
	
	switch (array->get_lhs_operand_i()->variantT()) {
		case V_SgPntrArrRefExp:
			return parseArray(isSgPntrArrRefExp(array->get_lhs_operand_i()), av);
		case V_SgVarRefExp:
		case V_SgDotExp:
		case V_SgArrowExp:
			return translateVarRef(array->get_lhs_operand_i());
		default:
			throw Exception::ExceptionAccessRetrieval(array, "Array reference need to be variable or struct.");
	}
}

}

