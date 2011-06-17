/**
 * \file src/rose/CodeGeneration.cpp
 * \brief Implementation of Code Generator.
 * \author Tristan Vanderbruggen
 * \version 0.1
 *
 * \todo l.161-169: Figure out the use case of this kind of stmt, for VarRef creation (can var be a struct ?)
 *
 * \note VarRefExp built between l194 and l211 refering to generated iterator => no risk of struct construction
 */

namespace CodeGeneration {

template <class TplStatement>
SgBasicBlock * generateBasicBlockFromSchedule(
	PolyhedricAnnotation::PolyhedralProgram<TplStatement, SgExprStatement, RoseVariable> & polyhedral_program,
	Scheduling::Schedule<TplStatement, SgExprStatement, RoseVariable> & schedule,
	SgScopeStatement * scope
) {
	CloogState * state = cloog_state_malloc();
	CloogOptions * options = cloog_options_malloc(state);
	CloogProgram * program = cloog_program_malloc();
	
	size_t nb_scat_dim = ScopLib::generateCloogProgramFromSchedule<TplStatement, SgExprStatement, RoseVariable>(schedule, state, options, program);
	
	struct clast_stmt * clast = cloog_clast_create(program, options);

	Sg_File_Info* sourceLocation = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
	SgBasicBlock * block = new SgBasicBlock(sourceLocation, NULL);
	
	std::ostringstream str_schedule;
	str_schedule << std::endl << " * This block is scheduled with:" << std::endl << std::endl;
	schedule.print(str_schedule, " * ");
	SageBuilder::buildComment(block, str_schedule.str());
	
	for (size_t i = 0; i < nb_scat_dim; i++) {
		std::ostringstream oss;
		oss << 'c' << i;
		SageInterface::appendStatement(
			SageBuilder::buildVariableDeclaration(
				oss.str(),
				SageBuilder::buildIntType(),
				NULL,
				block
			),
			block
		);
	}
	
	generateSageFromClast(clast, block, polyhedral_program);
	
	return block;
}

template <class TplStatement>
void generateSageFromClast(
	struct clast_stmt * s,
	SgScopeStatement * scope,
	PolyhedricAnnotation::PolyhedralProgram<TplStatement, SgExprStatement, RoseVariable> & polyhedral_program
) {
	
	for ( ; s != NULL; s = s->next) {
		if (CLAST_STMT_IS_A(s, stmt_root)) {
			continue;
		}
		if (CLAST_STMT_IS_A(s, stmt_ass)) { // Figure out the use case of this kind of stmt, for VarRef creation (can var be a struct ?)
			SageInterface::appendStatement(
				SageBuilder::buildAssignStatement(
					SageBuilder::buildVarRefExp(std::string(((struct clast_assignment *)s)->LHS), scope), 
					ClastExprToSageExpr(((struct clast_assignment *)s)->RHS, scope, polyhedral_program)
				),
				scope
			);
		}
		else if (CLAST_STMT_IS_A(s, stmt_user)) {
			SgExprStatement * stmt = polyhedral_program.getStatementById(((struct clast_user_stmt *)s)->statement->number);
			PolyhedricAnnotation::Domain<TplStatement, SgExprStatement, RoseVariable> & domain =
				PolyhedricAnnotation::getDomain<TplStatement, SgExprStatement, RoseVariable>(stmt);
			struct clast_stmt *t = ((struct clast_user_stmt *)s)->substitutions;
			for (size_t i = 0; i < domain.getNumberOfIterators(); i++) {
				SageInterface::appendStatement(
					SageBuilder::buildAssignStatement(
						polyhedral_program.getIteratorById(stmt, i).generate(scope),
						ClastExprToSageExpr(((struct clast_assignment *)t)->RHS, scope, polyhedral_program)
					),
					scope
				);
				t = t->next;
			}
			SageInterface::appendStatement(stmt, scope);
		}
		else if (CLAST_STMT_IS_A(s, stmt_for)) {
			Sg_File_Info* sourceLocation = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
			SgBasicBlock * block = new SgBasicBlock(sourceLocation, NULL);
			
			generateSageFromClast(((struct clast_for *)s)->body, block, polyhedral_program);
			
			// VarRefExp built here refering to generated iterator => no risk of struct construction
			SageInterface::appendStatement(
				SageBuilder::buildForStatement(
					SageBuilder::buildAssignStatement(
						SageBuilder::buildVarRefExp(std::string(((struct clast_for *)s)->iterator), scope), 
						ClastExprToSageExpr(((struct clast_for *)s)->LB, scope, polyhedral_program)
					),
					SageBuilder::buildExprStatement(SageBuilder::buildLessOrEqualOp(
						SageBuilder::buildVarRefExp(std::string(((struct clast_for *)s)->iterator), scope),
						ClastExprToSageExpr(((struct clast_for *)s)->UB, scope, polyhedral_program)
					)),
					SageBuilder::buildPlusAssignOp(
						SageBuilder::buildVarRefExp(std::string(((struct clast_for *)s)->iterator), scope),
						SageBuilder::buildIntVal(mpz_get_si(((struct clast_for *)s)->stride))
					),
					block
				),
				scope
			);
		}
		else if (CLAST_STMT_IS_A(s, stmt_guard)) {
			SgExpression * condition;
			size_t i = 0;
			if (((struct clast_guard *)s)->eq[i].sign == 0)
				condition = SageBuilder::buildEqualityOp(
					ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].LHS, scope, polyhedral_program),
					ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].RHS, scope, polyhedral_program)
				);
			else if (((struct clast_guard *)s)->eq[i].sign > 0)
				condition = SageBuilder::buildGreaterOrEqualOp(
					ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].LHS, scope, polyhedral_program),
					ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].RHS, scope, polyhedral_program)
				);
			else if (((struct clast_guard *)s)->eq[i].sign < 0)
				condition = SageBuilder::buildLessOrEqualOp(
					ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].LHS, scope, polyhedral_program),
					ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].RHS, scope, polyhedral_program)
				);
			for (i = 1; i < ((struct clast_guard *)s)->n; i++) {
				if (((struct clast_guard *)s)->eq[i].sign == 0)
					condition = SageBuilder::buildAndOp(condition, SageBuilder::buildEqualityOp(
						ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].LHS, scope, polyhedral_program),
						ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].RHS, scope, polyhedral_program)
					));
				else if (((struct clast_guard *)s)->eq[i].sign > 0)
					condition = SageBuilder::buildAndOp(condition, SageBuilder::buildGreaterOrEqualOp(
						ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].LHS, scope, polyhedral_program),
						ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].RHS, scope, polyhedral_program)
					));
				else if (((struct clast_guard *)s)->eq[i].sign < 0)
					condition = SageBuilder::buildAndOp(condition, SageBuilder::buildLessOrEqualOp(
						ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].LHS, scope, polyhedral_program),
						ClastExprToSageExpr(((struct clast_guard *)s)->eq[i].RHS, scope, polyhedral_program)
					));
			}
			
			Sg_File_Info* sourceLocation = Sg_File_Info::generateDefaultFileInfoForTransformationNode();
			SgBasicBlock * block = new SgBasicBlock(sourceLocation, NULL);
				
			generateSageFromClast(((struct clast_guard *)s)->then, block, polyhedral_program);
			
			SageInterface::appendStatement(
				SageBuilder::buildIfStmt(
					SageBuilder::buildExprStatement(condition),
					block,
					NULL
				),
				scope
			);
		}
		else if (CLAST_STMT_IS_A(s, stmt_block)) {
			generateSageFromClast(((struct clast_block *)s)->body, scope, polyhedral_program);
		}
		else {
			std::cout << "Error CLAST_STMT_IS nothing" << std::endl;
		}
	}
}

template <class TplStatement>
SgExpression * ClastExprToSageExpr(
	struct clast_expr * e,
	SgScopeStatement * scope,
	PolyhedricAnnotation::PolyhedralProgram<TplStatement, SgExprStatement, RoseVariable> & polyhedral_program
) {
	SgExpression * res = NULL;
	if (!e)
		return res;
	switch (e->type) {
		case clast_expr_name:
			for (size_t i = 0; i < polyhedral_program.getNumberOfGlobals(); i++)
				if (polyhedral_program.getGlobalById(i).getString() == ((struct clast_name *)e)->name)
					return polyhedral_program.getGlobalById(i).generate(scope);
			return SageBuilder::buildVarRefExp(std::string(((struct clast_name *)e)->name), scope);
		case clast_expr_term:
			res = SageBuilder::buildIntVal(mpz_get_si(((struct clast_term *)e)->val));
			if (((struct clast_term *)e)->var) {
				res = SageBuilder::buildMultiplyOp(res, ClastExprToSageExpr(((struct clast_term *)e)->var, scope, polyhedral_program));
			}
			return res;
		case clast_expr_red:
			switch (((struct clast_reduction*)e)->type) {
				case clast_red_sum:
					res = ClastExprToSageExpr(((struct clast_reduction*)e)->elts[0], scope, polyhedral_program);
					for (size_t i = 1; i < ((struct clast_reduction*)e)->n; i++) {
						res = SageBuilder::buildAddOp(res, ClastExprToSageExpr(((struct clast_reduction*)e)->elts[i], scope, polyhedral_program));
					}
					return res;
					
				case clast_red_min:
					res = ClastExprToSageExpr(((struct clast_reduction*)e)->elts[0], scope, polyhedral_program);
					for (size_t i = 1; i < ((struct clast_reduction*)e)->n; i++) {
						SgExpression * tmp = ClastExprToSageExpr(((struct clast_reduction*)e)->elts[i], scope, polyhedral_program);
						res = SageBuilder::buildConditionalExp(SageBuilder::buildLessThanOp(res, tmp), res, tmp);	
					}
					return res;
					
				case clast_red_max:
					res = ClastExprToSageExpr(((struct clast_reduction*)e)->elts[0], scope, polyhedral_program);
					for (size_t i = 1; i < ((struct clast_reduction*)e)->n; i++) {
						SgExpression * tmp = ClastExprToSageExpr(((struct clast_reduction*)e)->elts[i], scope, polyhedral_program);
						res = SageBuilder::buildConditionalExp(SageBuilder::buildGreaterThanOp(res, tmp), res, tmp);
					}
					return res;
					
				default:
					std::cout << "Error clast_expr_red" << std::endl;
			}
			break;
		case clast_expr_bin:
			switch (((struct clast_binary *)e)->type) {
				case clast_bin_fdiv:
				{
					SgExpression * n_exp = ClastExprToSageExpr(((struct clast_binary *)e)->LHS, scope, polyhedral_program);
					SgExpression * d_exp = SageBuilder::buildIntVal(mpz_get_si(((struct clast_binary *)e)->RHS));
					return SageBuilder::buildConditionalExp(
						SageBuilder::buildLessThanOp(
							n_exp,
							SageBuilder::buildIntVal(0)
						),
						SageBuilder::buildIntegerDivideOp(SageBuilder::buildAddOp(SageBuilder::buildSubtractOp(n_exp, d_exp), SageBuilder::buildIntVal(1)), d_exp),
						SageBuilder::buildIntegerDivideOp(n_exp, d_exp)
					);
				}
				case clast_bin_cdiv:
				{
					SgExpression * n_exp = ClastExprToSageExpr(((struct clast_binary *)e)->LHS, scope, polyhedral_program);
					SgExpression * d_exp = SageBuilder::buildIntVal(mpz_get_si(((struct clast_binary *)e)->RHS));
					return SageBuilder::buildConditionalExp(
						SageBuilder::buildLessThanOp(
							n_exp,
							SageBuilder::buildIntVal(0)
						),
						SageBuilder::buildIntegerDivideOp(n_exp, d_exp),
						SageBuilder::buildIntegerDivideOp(SageBuilder::buildAddOp(SageBuilder::buildAddOp(n_exp, d_exp), SageBuilder::buildIntVal(1)), d_exp)
					);
				}
				case clast_bin_div:
					return SageBuilder::buildIntegerDivideOp(
						ClastExprToSageExpr(((struct clast_binary *)e)->LHS, scope, polyhedral_program),
						SageBuilder::buildIntVal(mpz_get_si(((struct clast_binary *)e)->RHS))
					);
					
				case clast_bin_mod:
					return SageBuilder::buildModOp(
						ClastExprToSageExpr(((struct clast_binary *)e)->LHS, scope, polyhedral_program),
						SageBuilder::buildIntVal(mpz_get_si(((struct clast_binary *)e)->RHS))
					);
			}
			break;
		default:
			std::cout << "Error clast_expr_bin" << std::endl;
	}
	if (!res)
		std::cout << "Error res=NULL" << std::endl;
    	return res;
}

}

