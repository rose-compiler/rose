
namespace PolyhedricAnnotation {

template <class TplStatement>
size_t Parse(TplStatement * attach_to, SgStatement * stmt) throw (Exception::ExceptionBase) {
	ScopTree_::ScopTree<TplStatement, SgExprStatement, RoseVariable> * root =
		new ScopTree_::ScopRoot<TplStatement, SgExprStatement, RoseVariable>(attach_to);

	Traverse<TplStatement>(stmt, root, 0, attach_to);

	root->Traverse();
	
	PolyhedricAnnotation::PolyhedralProgram<TplStatement, SgExprStatement, RoseVariable> & polyhedral_program =
		PolyhedricAnnotation::getPolyhedralProgram<TplStatement, SgExprStatement, RoseVariable>(attach_to);
	
	polyhedral_program.finalize();
	
	const std::vector<SgExprStatement *> & exps = polyhedral_program.getExpressions();
	std::vector<SgExprStatement *>::const_iterator it;
	for (it = exps.begin(); it != exps.end(); it++) {
		PolyhedricAnnotation::DataAccess<TplStatement, SgExprStatement, RoseVariable> & data_access =
			PolyhedricAnnotation::getDataAccess<TplStatement, SgExprStatement, RoseVariable>(*it);
		PolyhedricAnnotation::makeAccessAnnotation<TplStatement, SgExprStatement, RoseVariable>(*it, data_access);
	}
}

template <class TplStatement>
void setBounds(
	SgForStatement * for_stmt,
	ScopTree_::ScopLoop<TplStatement, SgExprStatement, RoseVariable> * loop_node,
	const RoseVariable & iterator
);

template <class TplStatement>
void setCondition(
	SgIfStmt * if_stmt,
	ScopTree_::ScopConditionnal<TplStatement, SgExprStatement, RoseVariable> * cond_node,
	bool is_else
);

template <class TplStatement>
void parseCondition(
	SgBinaryOp * bin_op,
	ScopTree_::ScopConditionnal<TplStatement, SgExprStatement, RoseVariable> * cond_node,
	bool negative
);

template <class TplStatement>
size_t Traverse(
	SgStatement * stmt,
	ScopTree_::ScopTree<TplStatement, SgExprStatement, RoseVariable> * tree,
	size_t pos,
	TplStatement * base
) throw (Exception::ExceptionBase) {
	switch (stmt->variantT()) {
		case V_SgBasicBlock:
		{
			SgBasicBlock * bb_stmt = isSgBasicBlock(stmt);
			size_t cnt = 0;

			const SgStatementPtrList & stmt_list = bb_stmt->get_statements();
			SgStatementPtrList::const_iterator it;
			for (it = stmt_list.begin(); it != stmt_list.end(); it++)
				cnt += Traverse(*it, tree, pos + cnt, base);

			return cnt;
		}
		case V_SgForStatement:
		{
			SgForStatement * for_stmt = isSgForStatement(stmt);

			RoseVariable iterator = getIterator(for_stmt);

			ScopTree_::ScopLoop<TplStatement, SgExprStatement, RoseVariable> * loop_node =
				new ScopTree_::ScopLoop<TplStatement, SgExprStatement, RoseVariable>(pos, tree, iterator);

			setBounds<TplStatement>(for_stmt, loop_node, iterator);

			loop_node->setIncrement(getIncrement(for_stmt, iterator));

			Traverse(for_stmt->get_loop_body(), loop_node, 0, base);

			return 1;
		}
		case V_SgIfStmt:
		{
			SgIfStmt * if_stmt = isSgIfStmt(stmt);

			ScopTree_::ScopConditionnal<TplStatement, SgExprStatement, RoseVariable> * then_node =
				new ScopTree_::ScopConditionnal<TplStatement, SgExprStatement, RoseVariable>(pos, tree);

			setCondition<TplStatement>(if_stmt, then_node, false);

			Traverse(if_stmt->get_true_body(), then_node, 0, base);

			if (if_stmt->get_false_body() != NULL) {
				ScopTree_::ScopConditionnal<TplStatement, SgExprStatement, RoseVariable> * else_node =
					new ScopTree_::ScopConditionnal<TplStatement, SgExprStatement, RoseVariable>(pos+1, tree);

				setCondition<TplStatement>(if_stmt, else_node, true);

				Traverse(if_stmt->get_false_body(), else_node, 0, base);

				return 2;
			}

			return 1;
		}
		case V_SgExprStatement:
		{
			SgExprStatement * exp_stmt = isSgExprStatement(stmt);

			ScopTree_::ScopStatement<TplStatement, SgExprStatement, RoseVariable> * stmt_node =
				new ScopTree_::ScopStatement<TplStatement, SgExprStatement, RoseVariable>(pos, tree, exp_stmt);

			return 1;
		}
		case V_SgVariableDeclaration:
		case V_SgPragmaDeclaration:
		case V_SgReturnStmt:
			throw Exception::MisplacedNode(stmt);
		default:
		{
			throw Exception::RoseUnknownNode("SCoP traversal", stmt);
		}
	}
}

template <class TplStatement>
void setBounds(
	SgForStatement * for_stmt,
	ScopTree_::ScopLoop<TplStatement, SgExprStatement, RoseVariable> * loop_node,
	const RoseVariable & iterator
) {
	std::vector<std::pair<RoseVariable, int> >::iterator it;
	std::vector<std::pair<RoseVariable, int> > lb;
	std::vector<std::pair<RoseVariable, int> > ub;
	
	SgForInitStatement * init_stmt;
	SgStatement * cond_stmt;
	SgExprStatement * exp_stmt;
	SgAssignOp * assign_op;
	SgBinaryOp * bin_op;
	SgExpression * exp;
	
	// Lower Bound
	init_stmt = for_stmt->get_for_init_stmt();
	exp_stmt = isSgExprStatement(init_stmt->get_init_stmt()[0]);
	if (exp_stmt == NULL)
		throw Exception::ExceptionForLoopTranslation(for_stmt, "Initialisation statement is not an Expression statement.");
	assign_op = isSgAssignOp(exp_stmt->get_expression());
	if (assign_op == NULL)
		throw Exception::ExceptionForLoopTranslation(for_stmt, "Initialisation expression is not an assignation.");
	exp = assign_op->get_rhs_operand_i();

	translateLinearExpression(exp, lb);
	for (it = lb.begin(); it != lb.end(); it++)
		loop_node->addLowerBoundTerm(it->first, it->second);
	
	// Upper Bound
	cond_stmt = for_stmt->get_test();
	exp_stmt = isSgExprStatement(cond_stmt);
	if (exp_stmt == NULL)
		throw Exception::ExceptionForLoopTranslation(for_stmt, "Condition statement is not an Expression statement.");
	bin_op = isSgBinaryOp(exp_stmt->get_expression());
	if (bin_op == NULL || (!isSgLessThanOp(bin_op) && !isSgLessOrEqualOp(bin_op) && !isSgGreaterThanOp(bin_op) && !isSgGreaterOrEqualOp(bin_op)))
		throw Exception::ExceptionForLoopTranslation(for_stmt, "Condition expression is not a \"less\" or \"less or equal\" operation.");
	if (!isSgVarRefExp(bin_op->get_lhs_operand_i()) || !iterator.is(isSgVarRefExp(bin_op->get_lhs_operand_i())->get_symbol()->get_declaration()))
		throw Exception::ExceptionForLoopTranslation(for_stmt, "Left operand of the condition need to be the loop iterator.");
	exp = bin_op->get_rhs_operand_i();
	
	translateLinearExpression(exp, ub);
	for (it = ub.begin(); it != ub.end(); it++)
		loop_node->addUpperBoundTerm(it->first, it->second);
	
	if (isSgLessThanOp(bin_op)) {
		loop_node->addUpperBoundTerm(RoseVariable(), -1);
	}
}

template <class TplStatement>
void parseCondition(
	SgBinaryOp * bin_op,
	ScopTree_::ScopConditionnal<TplStatement, SgExprStatement, RoseVariable> * cond_node,
	bool negative
) {
	SgBinaryOp * lhs_bin_op = isSgBinaryOp(bin_op->get_lhs_operand_i());
	SgBinaryOp * rhs_bin_op = isSgBinaryOp(bin_op->get_rhs_operand_i());
	
	SgNotOp * lhs_not = isSgNotOp(bin_op->get_lhs_operand_i());
	SgNotOp * rhs_not = isSgNotOp(bin_op->get_rhs_operand_i());
	
	std::vector<std::pair<RoseVariable, int> > lin_exp;
	
	switch (bin_op->variantT()) {
		case V_SgAndOp:
			if (negative)
				throw Exception::ExceptionConditionnalTranslation(bin_op, "Condition need to be convex. Caused by \"!(a & b)\". \"!\" (not) cant come from an \"else\" statement.");

			if (lhs_bin_op)
				parseCondition<TplStatement>(lhs_bin_op, cond_node, negative);
			else if (lhs_not) {
				lhs_bin_op = isSgBinaryOp(lhs_not->get_operand_i());
				parseCondition<TplStatement>(lhs_bin_op, cond_node, !negative);
			}
			else
				throw Exception::RoseUnknownNode("Condition expression", bin_op->get_lhs_operand_i());

			if (rhs_bin_op)
				parseCondition<TplStatement>(rhs_bin_op, cond_node, negative);
			else if (rhs_not) {
				rhs_bin_op = isSgBinaryOp(rhs_not->get_operand_i());
				parseCondition<TplStatement>(rhs_bin_op, cond_node, !negative);
			}
			else
				throw Exception::RoseUnknownNode("Condition expression", bin_op->get_rhs_operand_i());
			break;
		case V_SgOrOp:
			if (!negative)
				throw Exception::ExceptionConditionnalTranslation(bin_op, "Condition need to be convex. Caused by \"a | b\".");
				
			if (lhs_bin_op)
				parseCondition<TplStatement>(lhs_bin_op, cond_node, negative);
			else if (lhs_not) {
				lhs_bin_op = isSgBinaryOp(lhs_not->get_operand_i());
				parseCondition<TplStatement>(lhs_bin_op, cond_node, !negative);
			}
			else
				throw Exception::RoseUnknownNode("Condition expression", bin_op->get_lhs_operand_i());
				
			if (rhs_bin_op)
				parseCondition<TplStatement>(rhs_bin_op, cond_node, negative);
			else if (rhs_not) {
				rhs_bin_op = isSgBinaryOp(rhs_not->get_operand_i());
				parseCondition<TplStatement>(rhs_bin_op, cond_node, !negative);
			}
			else
				throw Exception::RoseUnknownNode("Condition expression", bin_op->get_rhs_operand_i());
			break;
		case V_SgEqualityOp:
			if (negative)
				throw Exception::ExceptionConditionnalTranslation(bin_op, "Condition need to be convex. Caused by \"!(a == b)\". \"!\" (not) cant come from an \"else\" statement.");
			translateLinearExpression(bin_op->get_lhs_operand_i(), lin_exp, false);
			translateLinearExpression(bin_op->get_rhs_operand_i(), lin_exp, true);
			cond_node->addCondition(lin_exp, true);
			break;
		case V_SgNotEqualOp:
			if (!negative)
				throw Exception::ExceptionConditionnalTranslation(bin_op, "Condition need to be convex. Caused by \"a != b\".");
			translateLinearExpression(bin_op->get_lhs_operand_i(), lin_exp, false);
			translateLinearExpression(bin_op->get_rhs_operand_i(), lin_exp, true);
			cond_node->addCondition(lin_exp, true);
			break;
		case V_SgGreaterOrEqualOp:
			if (!negative) {
				translateLinearExpression(bin_op->get_lhs_operand_i(), lin_exp, false);
				translateLinearExpression(bin_op->get_rhs_operand_i(), lin_exp, true);
			}
			else {
				translateLinearExpression(bin_op->get_rhs_operand_i(), lin_exp, false);
				translateLinearExpression(bin_op->get_lhs_operand_i(), lin_exp, true);
				lin_exp.push_back(std::pair<RoseVariable, int>(RoseVariable(), -1));
			}
			cond_node->addCondition(lin_exp, false);
			break;
		case V_SgGreaterThanOp:
			if (!negative) {
				translateLinearExpression(bin_op->get_lhs_operand_i(), lin_exp, false);
				translateLinearExpression(bin_op->get_rhs_operand_i(), lin_exp, true);
				lin_exp.push_back(std::pair<RoseVariable, int>(RoseVariable(), -1));
			}
			else {
				translateLinearExpression(bin_op->get_rhs_operand_i(), lin_exp, false);
				translateLinearExpression(bin_op->get_lhs_operand_i(), lin_exp, true);
			}
			cond_node->addCondition(lin_exp, false);
			break;
		case V_SgLessOrEqualOp:
			if (!negative) {
				translateLinearExpression(bin_op->get_rhs_operand_i(), lin_exp, false);
				translateLinearExpression(bin_op->get_lhs_operand_i(), lin_exp, true);
			}
			else {
				translateLinearExpression(bin_op->get_lhs_operand_i(), lin_exp, false);
				translateLinearExpression(bin_op->get_rhs_operand_i(), lin_exp, true);
				lin_exp.push_back(std::pair<RoseVariable, int>(RoseVariable(), -1));
			}
			cond_node->addCondition(lin_exp, false);
			break;
		case V_SgLessThanOp:
			if (!negative) {
				translateLinearExpression(bin_op->get_rhs_operand_i(), lin_exp, false);
				translateLinearExpression(bin_op->get_lhs_operand_i(), lin_exp, true);
				lin_exp.push_back(std::pair<RoseVariable, int>(RoseVariable(), -1));
			}
			else {
				translateLinearExpression(bin_op->get_lhs_operand_i(), lin_exp, false);
				translateLinearExpression(bin_op->get_rhs_operand_i(), lin_exp, true);
			}
			cond_node->addCondition(lin_exp, false);
			break;
		default:
			throw Exception::RoseUnknownNode("Condition expression", bin_op->get_rhs_operand_i());
	}
}

template <class TplStatement>
void setCondition(
	SgIfStmt * if_stmt,
	ScopTree_::ScopConditionnal<TplStatement, SgExprStatement, RoseVariable> * cond_node,
	bool is_else
) {
	SgExprStatement * exp_stmt = isSgExprStatement(if_stmt->get_conditional());
	if (!exp_stmt)
		throw Exception::RoseUnknownNode("Condition parser", if_stmt->get_conditional());
	SgBinaryOp * bin_op = isSgBinaryOp(exp_stmt->get_expression());
	SgNotOp * not_op = isSgNotOp(exp_stmt->get_expression());
	if (bin_op)
		parseCondition<TplStatement>(bin_op, cond_node, is_else);
	else if (not_op) {
		bin_op = isSgBinaryOp(not_op->get_operand_i());
		if (!bin_op)
			throw Exception::RoseUnknownNode("Condition expression", exp_stmt->get_expression());
		parseCondition<TplStatement>(bin_op, cond_node, !is_else);
	}
	else
		throw Exception::RoseUnknownNode("Condition expression", exp_stmt->get_expression());
}

template <class TplStatement>
void makeAccessAnnotationSage(
	SgExprStatement * expression,
	PolyhedricAnnotation::DataAccess<TplStatement, SgExprStatement, RoseVariable> & data_access
) {
	SgExpression * exp = expression->get_expression();
	switch (exp->variantT()) {
		case V_SgPlusAssignOp:
		case V_SgMinusAssignOp:
		case V_SgMultAssignOp:
		case V_SgDivAssignOp:
		case V_SgModAssignOp:
		case V_SgAndAssignOp:
		case V_SgIorAssignOp:
		case V_SgXorAssignOp:
		case V_SgRshiftAssignOp:
		case V_SgLshiftAssignOp:
		case V_SgPointerAssignOp:
			setRead(isSgBinaryOp(exp)->get_lhs_operand_i(), data_access);
		case V_SgAssignOp:
			setWrite(isSgBinaryOp(exp)->get_lhs_operand_i(), data_access);
			setRead(isSgBinaryOp(exp)->get_rhs_operand_i(), data_access);
			break;
		default:
			throw Exception::ExceptionAccessRetrieval(exp, "Look for an assignation.");
	}
}

template <class TplStatement>
void setRead(SgExpression * exp, DataAccess<TplStatement, SgExprStatement, RoseVariable> & data_access) {
	std::vector<std::vector<std::pair<RoseVariable, int> > > av;
	if (isSgValueExp(exp)) return;
	RoseVariable * var;
	switch (exp->variantT()) {
		case V_SgPntrArrRefExp:
			var = parseArray(isSgPntrArrRefExp(exp), av);
			data_access.addRead(*var, av);
			break;
		case V_SgVarRefExp:
		case V_SgDotExp:
		case V_SgArrowExp:
			data_access.addRead(*translateVarRef(exp), av);
			break;
		case V_SgAddOp:
		case V_SgSubtractOp:
		case V_SgMultiplyOp:
		case V_SgDivideOp:
		case V_SgIntegerDivideOp:
		case V_SgModOp:
		case V_SgAndOp:
		case V_SgOrOp:
		case V_SgBitAndOp:
		case V_SgBitOrOp:
		case V_SgBitXorOp:
		case V_SgLshiftOp:
		case V_SgRshiftOp:
			setRead(isSgBinaryOp(exp)->get_lhs_operand_i(), data_access);
			setRead(isSgBinaryOp(exp)->get_rhs_operand_i(), data_access);
			break;
		case V_SgCastExp:
		case V_SgMinusOp:
		case V_SgNotOp:
			setRead(isSgUnaryOp(exp)->get_operand_i(), data_access);
			break;
		case V_SgFunctionCallExp:
		{
			SgFunctionCallExp * func_call = isSgFunctionCallExp(exp);
			SgFunctionRefExp * func_ref = isSgFunctionRefExp(func_call->get_function());
			if (!func_ref)
				throw Exception::ExceptionAccessRetrieval(exp, "Only simple function call are handle for now. (and not \"" + func_call->get_function()->class_name() + "\")");
			SgFunctionType * func_type = isSgFunctionType(func_ref->get_type());
			if (!func_type)
				throw Exception::ExceptionAccessRetrieval(exp, "Only simple function call are handle for now. (problem with funtion type \"" + func_ref->get_type()->class_name() + "\")");
			const SgTypePtrList & param_type_list = func_type->get_argument_list()->get_arguments();
			SgTypePtrList::const_iterator param_type;
			bool all_param_are_value_type = true;
			for (param_type = param_type_list.begin(); param_type != param_type_list.end(); param_type++) {
				switch ((*param_type)->variantT()) {
					case V_SgTypeChar:
					case V_SgTypeSignedChar:
					case V_SgTypeUnsignedChar:
					case V_SgTypeShort:
					case V_SgTypeSignedShort:
					case V_SgTypeUnsignedShort:
					case V_SgTypeInt:
					case V_SgTypeSignedInt:
					case V_SgTypeUnsignedInt:
					case V_SgTypeLong:
					case V_SgTypeSignedLong:
					case V_SgTypeFloat:
					case V_SgTypeDouble:
					case V_SgTypeLongLong:
					case V_SgTypeSignedLongLong:
					case V_SgTypeUnsignedLongLong:
					case V_SgTypeBool:
					case V_SgEnumType:
						break;
					case V_SgPointerType:
					case V_SgClassType:
					default:
						all_param_are_value_type = false;
						break;
				}
				if (!all_param_are_value_type)
					break;
			}
			if (!all_param_are_value_type)
				throw Exception::ExceptionAccessRetrieval(exp, "Only accept call to function with value parameters.");
			const SgExpressionPtrList & param_list = func_call->get_args()->get_expressions();
			SgExpressionPtrList::const_iterator param;
			for (param = param_list.begin(); param != param_list.end(); param++)
				setRead(*param, data_access);
			
			break;
		}
		default:
			throw Exception::RoseUnknownNode("Expression data access analysis", exp);
	}
}

template <class TplStatement>
void setWrite(SgExpression * exp, DataAccess<TplStatement, SgExprStatement, RoseVariable> & data_access) {
	std::vector<std::vector<std::pair<RoseVariable, int> > > av;
	RoseVariable * var;
	switch (exp->variantT()) {
		case V_SgPntrArrRefExp:
			var = parseArray(isSgPntrArrRefExp(exp), av);
			data_access.addWrite(*var, av);
			break;
		case V_SgVarRefExp:
		case V_SgDotExp:
		case V_SgArrowExp:
			data_access.addWrite(*translateVarRef(exp), av);
			break;
		default:
			throw Exception::ExceptionAccessRetrieval(exp, "Look for variable/array reference in left side of an assignation.");
	}
}

}
