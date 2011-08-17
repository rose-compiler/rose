
#define DEBUG_ROSE_MAX_COVER 1

#include "rose-max-cover/TDBUtraversal.hpp"

#if DEBUG_ROSE_MAX_COVER
#  define DEBUG_TRAVERSAL 1
#  define DEBUG_ATTACHED_RESULT 1
#  define DEBUG_SYMBOL_TABLE 1
#  define DEBUG_FOR_STMT 1
#  define DEBUG_LINEAR_EXPRESSION 1
#  define DEBUG_DEVELOP_EXPR 1
#  define DEBUG_DATA_ACCESS_COLLECTION 1
#  define DEBUG_TRAVERSAL_BOTTOM_UP 1
#  define DEBUG_DATA_ACCESS_GENERATION 1
//#  define DEBUG_TODO 1
#  define DEBUG_TODO 0
#  define DEBUG_PREDICATE 1
#  define DEBUG_HELPER 1
#else
#  define DEBUG_TODO 0
#  define DEBUG_ATTACHED_RESULT 1
#  define DEBUG_PREDICATE 1
#  define DEBUG_HELPER 1
#endif

#if DEBUG_SYMBOL_TABLE
#  define DEBUG_ATTACHED_RESULT 1
#endif

#if DEBUG_TRAVERSAL_BOTTOM_UP
#  define DEBUG_TRAVERSAL 1
#endif


SgExpression * develop(SgExpression * e) {
	if (isSgExpression(e) == NULL)
		return NULL;

	switch (e->variantT()) {
		case V_SgAddOp:
		{
			SgAddOp * add_op = isSgAddOp(e);
			SgExpression * lhs = develop(add_op->get_lhs_operand_i());
			SgExpression * rhs = develop(add_op->get_rhs_operand_i());
			
			add_op->set_lhs_operand_i(lhs);
			lhs->set_parent(add_op);
			add_op->set_rhs_operand_i(rhs);
			rhs->set_parent(add_op);
			
			return add_op;
		}
		case V_SgSubtractOp:
		{
			SgSubtractOp * sub_op = isSgSubtractOp(e);
			SgExpression * lhs = develop(sub_op->get_lhs_operand_i());
			SgExpression * rhs = sub_op->get_rhs_operand_i();

			SgAddOp * add_op = SageBuilder::buildAddOp(lhs, develop(SageBuilder::buildMinusOp(rhs)));

			return develop(add_op);
		}
		case V_SgMultiplyOp:
		{
			SgMultiplyOp * mul_op = isSgMultiplyOp(e);
			
			SgExpression * lhs = develop(mul_op->get_lhs_operand_i());
			SgExpression * rhs = develop(mul_op->get_rhs_operand_i());
			
			SgAddOp * add_lhs = isSgAddOp(lhs);
			SgAddOp * add_rhs = isSgAddOp(rhs);
			
			if (add_lhs) {
				SgExpression * lhs_lhs = develop(add_lhs->get_lhs_operand_i());
				SgExpression * lhs_rhs = develop(add_lhs->get_rhs_operand_i());
				
				delete mul_op;

				lhs_lhs = SageBuilder::buildMultiplyOp(lhs_lhs, rhs);
				lhs_rhs = SageBuilder::buildMultiplyOp(lhs_rhs, rhs);
				
				add_lhs->set_lhs_operand_i(lhs_lhs);
				add_lhs->set_rhs_operand_i(lhs_rhs);
				
				return develop(add_lhs);
			}
			
			if (add_rhs) {
				SgExpression * rhs_lhs = develop(add_rhs->get_lhs_operand_i());
				SgExpression * rhs_rhs = develop(add_rhs->get_rhs_operand_i());
				
				delete mul_op;

				rhs_lhs = SageBuilder::buildMultiplyOp(rhs_lhs, lhs);
				rhs_rhs = SageBuilder::buildMultiplyOp(rhs_rhs, lhs);
				
				add_rhs->set_lhs_operand_i(develop(rhs_lhs));
				add_rhs->set_rhs_operand_i(develop(rhs_rhs));
				
				return add_rhs;
			}
			
			mul_op->set_rhs_operand_i(rhs);
			rhs->set_parent(mul_op);
			mul_op->set_lhs_operand_i(lhs);
			lhs->set_parent(mul_op);
			return mul_op;
		}
		case V_SgCastExp:
		{
			// Just eliminate the cast as it is non-relevant...

			SgCastExp * cast_exp = isSgCastExp(e);
			SgExpression * casted = develop(cast_exp->get_operand_i());

			return casted;
		}
		case V_SgMinusOp:
		{
			SgMinusOp * minus_op = isSgMinusOp(e);
			SgExpression * expr = develop(minus_op->get_operand_i());

			if (!isSgAddOp(expr) && !isSgMultiplyOp(expr) && !isSgMinusOp(expr)) return e;
			
			if (isSgMinusOp(expr)) return isSgMinusOp(expr)->get_operand_i();

			std::stack<SgExpression *> stack_exp;
			stack_exp.push(expr);

			while (stack_exp.size() > 0) {
				SgExpression * current = stack_exp.top();
				stack_exp.pop();

				SgAddOp * add_op = isSgAddOp(current);
				SgMultiplyOp * mul_op = isSgMultiplyOp(current);
				SgMinusOp * minus_op = isSgMinusOp(current);

				if      (add_op != NULL)   { stack_exp.push(add_op->get_lhs_operand_i()); stack_exp.push(add_op->get_rhs_operand_i()); }
				else if (mul_op != NULL)   stack_exp.push(mul_op->get_rhs_operand_i());
				else if (minus_op != NULL) SageInterface::replaceExpression(current, minus_op->get_operand_i(), true);
				else                       SageInterface::replaceExpression(current, SageBuilder::buildMinusOp(current), true);
			}

			return expr;
		}
		case V_SgValueExp:
		case V_SgVarRefExp:
		case V_SgFunctionCallExp:
		  return e;
		case V_SgFunctionRefExp:
		  ROSE_ASSERT(DEBUG_TODO == 0); // TODO .
		case V_SgDivideOp:
		case V_SgModOp:
		  ROSE_ASSERT(DEBUG_TODO == 0); // TODO .
		default:
			if (isSgValueExp(e)) // SgValueExp is not a terminal of the grammar
				return e;
#if DEBUG_DEVELOP_EXPR
			std::cerr << "In develop(SgExpression * e = " << e << "): Non-handle node: " << e->class_name() << std::endl;
			ROSE_ASSERT(false);
#else /* DEBUG_DEVELOP_EXPR */
			return e;
#endif /* DEBUG_DEVELOP_EXPR */
	}
}

Expression<Symbol> * genExpression(SgExpression * sg_expr_, PolyhedralElement * elem, SymbolTable * st) {
	Expression<Symbol> * expr = NULL;

	SgExpression * sg_expr = develop(sg_expr_);

	// TODO

	return expr;
}

bool genLinearExpression(SgExpression * e, LinearExpression_ppl & le, PolyhedralElement * elem, SymbolTable * st) {
#if DEBUG_LINEAR_EXPRESSION
	std::cerr << "ENTER genLinearExpression(e=" << e << ", le=0, elem=" << elem << ", st=" << st << ")" << std::endl;
#endif /* DEBUG_LINEAR_EXPRESSION */

	SgExpression * exp = develop(e);
	
	std::vector<SgExpression *> term_exps;
	
	std::stack<SgExpression *> stack_exp;
	stack_exp.push(exp);
	
	while (!stack_exp.empty()) {
		SgExpression * current = stack_exp.top();
		stack_exp.pop();
		
		SgAddOp * add_op = isSgAddOp(current);
		
		if (add_op) {
			stack_exp.push(add_op->get_lhs_operand_i());
			stack_exp.push(add_op->get_rhs_operand_i());
			continue;
		}
		
		term_exps.push_back(current);
	}
	
	std::vector<SgExpression *>::iterator it_term;
	for (it_term = term_exps.begin(); it_term != term_exps.end(); it_term++) {
		if (isSgMultiplyOp(*it_term)) {
			std::vector<SgExpression *> factor_exps;
			stack_exp.push(*it_term);
			while (!stack_exp.empty()) {
				SgExpression * current = stack_exp.top();
				stack_exp.pop();
		
				SgMultiplyOp * mul_op = isSgMultiplyOp(current);
		
				if (mul_op) {
					stack_exp.push(mul_op->get_lhs_operand_i());
					stack_exp.push(mul_op->get_rhs_operand_i());
					continue;	
				}
		
				factor_exps.push_back(current);
			}
			
			int coef = 1;
			std::vector<SgExpression *>::iterator it_factor;
			std::vector<Symbol *> symbols_list;
			for (it_factor = factor_exps.begin(); it_factor != factor_exps.end(); it_factor++) {
				if (isSgValueExp(*it_factor)) {
					switch ((*it_factor)->variantT()) {
						case V_SgIntVal:                 coef *= isSgIntVal((*it_factor))->get_value();                 break;
						case V_SgLongIntVal:             coef *= isSgLongIntVal((*it_factor))->get_value();             break;
						case V_SgLongLongIntVal:         coef *= isSgLongLongIntVal((*it_factor))->get_value();         break;
						case V_SgShortVal:               coef *= isSgShortVal((*it_factor))->get_value();               break;
						case V_SgUnsignedIntVal:         coef *= isSgUnsignedIntVal((*it_factor))->get_value();         break;
						case V_SgUnsignedLongLongIntVal: coef *= isSgUnsignedLongLongIntVal((*it_factor))->get_value(); break;
						case V_SgUnsignedLongVal:        coef *= isSgUnsignedLongVal((*it_factor))->get_value();        break;
						case V_SgUnsignedCharVal:        coef *= isSgUnsignedCharVal((*it_factor))->get_value();        break;
						case V_SgCharVal:                coef *= isSgCharVal((*it_factor))->get_value();                break;
						case V_SgUnsignedShortVal:       coef *= isSgUnsignedShortVal((*it_factor))->get_value();       break;
						default: return false;
					}
				}
				else {
					Symbol * sym = st->genSymbol(exp);

					if (!sym) return false;
					else symbols_list.push_back(sym);
				}
			}
			if (symbols_list.size() == 0) {
				if (coef != 0)
					le += coef;
			}
			else if (symbols_list.size() == 1) {
				VariableID vid = elem->getID(symbols_list[0]);
				le += coef * vid;
			}
			else {
				ROSE_ASSERT(DEBUG_TODO == 0); // TODO if only global generate alias of "globals' product", if one iterator do pseudo multidimensional array analysis, if more than one iterator it's false.
				return false; 
			}
		}
		else if (isSgValueExp(*it_term)) {
			int coef = 0;
			switch ((*it_term)->variantT()) {
				case V_SgIntVal:                 coef = isSgIntVal((*it_term))->get_value();                 break;
				case V_SgLongIntVal:             coef = isSgLongIntVal((*it_term))->get_value();             break;
				case V_SgLongLongIntVal:         coef = isSgLongLongIntVal((*it_term))->get_value();         break;
				case V_SgShortVal:               coef = isSgShortVal((*it_term))->get_value();               break;
				case V_SgUnsignedIntVal:         coef = isSgUnsignedIntVal((*it_term))->get_value();         break;
				case V_SgUnsignedLongLongIntVal: coef = isSgUnsignedLongLongIntVal((*it_term))->get_value(); break;
				case V_SgUnsignedLongVal:        coef = isSgUnsignedLongVal((*it_term))->get_value();        break;
				case V_SgUnsignedCharVal:        coef = isSgUnsignedCharVal((*it_term))->get_value();        break;
				case V_SgCharVal:                coef = isSgCharVal((*it_term))->get_value();                break;
				case V_SgUnsignedShortVal:       coef = isSgUnsignedShortVal((*it_term))->get_value();       break;
				default: return false;
			}
			if (coef != 0)
				le += coef;
		}
		else {
			Symbol * sym = st->genSymbol(exp);

			if (!sym) return false;
			
			VariableID vid = elem->getID(sym);
			le += vid;
		}
	}
	
	return true;
}

bool genLinearConstraint(Symbol * iterator, SgExpression * e, std::vector<std::pair<LinearExpression_ppl, bool> > & le_vect, bool inc, bool positive, PolyhedralElement * elem, SymbolTable * st) {
#if DEBUG_LINEAR_EXPRESSION
	std::cerr << "ENTER genLinearConstraint(iterator=" << iterator << ", e=" << e << ", le_vect, inc=" << inc << ", positive=" << positive << ", elem=" << elem << ", st=" << st << ")" << std::endl;
#endif /* DEBUG_LINEAR_EXPRESSION */

	if (!positive) {
		std::cerr << "In genLinearConstraint(e=" << e << "): NYI: negative constraints." << std::endl;
		ROSE_ASSERT(false);
	}
	
	bool res = true;

	/* TODO support for boolean operators */ {
		SgLessThanOp * less_op = isSgLessThanOp(e);
		SgGreaterThanOp * greater_op = isSgGreaterThanOp(e);
		SgLessOrEqualOp * less_equal_op = isSgLessOrEqualOp(e);
		SgGreaterOrEqualOp * greater_equal_op = isSgGreaterOrEqualOp(e);

		if (less_op || greater_op || less_equal_op || greater_equal_op) {
		
			SgBinaryOp * bin_op = isSgBinaryOp(e);
			SgExpression * lhs = bin_op->get_lhs_operand_i();
			SgExpression * rhs = bin_op->get_rhs_operand_i();

			LinearExpression_ppl le_lhs;
			LinearExpression_ppl le_rhs;
			LinearExpression_ppl le;
			
			bool is_equality = false;

			if (genLinearExpression(lhs, le_lhs, elem, st) && genLinearExpression(rhs, le_rhs, elem, st)) {

				if (iterator != NULL) { // iterator is only specify when parsing the condition of a for-loop.
					Integer coef = le_lhs.coefficient(elem->getID(iterator)) - le_rhs.coefficient(elem->getID(iterator));
					if (coef == 0)
						return false;
				}

				bool lhs_rhs = true;
				bool minus_one = false;
				bool plus_one = false;
				switch (e->variantT()) {
					case V_SgLessThanOp:
					{
						lhs_rhs = false;
						minus_one = true;
						break;
					}
					case V_SgLessOrEqualOp:
					{
						lhs_rhs = false;
						break;
					}
					case V_SgGreaterThanOp:
					{
						lhs_rhs = true;
						minus_one = true;
						break;
					}
					case V_SgGreaterOrEqualOp:
					{
						lhs_rhs = true;
						break;
					}
					case V_SgEqualityOp:
					{
						is_equality = true;
						ROSE_ASSERT(DEBUG_TODO == 0); // TODO finnish equality handling in constraint parsing
						return false;
					}
					case V_SgNotEqualOp:
					{
						return false;
					}
					default:
						std::cerr << "Runtime error: unexpected node: " << e->class_name() << ", should have been filtered before..." << std::endl;
						ROSE_ASSERT(false);
				}
				if (lhs_rhs)
					le = le_lhs - le_rhs;
				else
					le = le_rhs - le_lhs;
				if (minus_one)
					le -= 1;
				if (plus_one)
					le += 1;
				
				le_vect.push_back(std::pair<LinearExpression_ppl, bool>(le, is_equality));
			}
			else res = false;
		}
		else res = false;
	}
	
	return res;
}

bool checkIncrementAssign(Symbol * s, SgExpression * e, int * val, PolyhedralElement * elem, SymbolTable * st) {
	switch (e->variantT()) {
		case V_SgPlusPlusOp:
		{
			Symbol * sym = st->genSymbol(isSgPlusPlusOp(e)->get_operand_i());
			if (sym != s)
				return false;
			*val = 1;
			return true;
		}
		case V_SgMinusMinusOp:
		{
			Symbol * sym = st->genSymbol(isSgMinusMinusOp(e)->get_operand_i());
			if (sym != s)
				return false;
			*val = -1;
			return true;
		}
		case V_SgPlusAssignOp:
		{
			Symbol * sym = st->genSymbol(isSgPlusAssignOp(e)->get_lhs_operand_i());
			if (sym != s)
				return false;
			SgExpression * rhs = isSgPlusAssignOp(e)->get_rhs_operand_i();
			switch (rhs->variantT()) {
				case V_SgIntVal:                 *val = isSgIntVal(rhs)->get_value();                 return true;
				case V_SgLongIntVal:             *val = isSgLongIntVal(rhs)->get_value();             return true;
				case V_SgLongLongIntVal:         *val = isSgLongLongIntVal(rhs)->get_value();         return true;
				case V_SgShortVal:               *val = isSgShortVal(rhs)->get_value();               return true;
				case V_SgUnsignedIntVal:         *val = isSgUnsignedIntVal(rhs)->get_value();         return true;
				case V_SgUnsignedLongLongIntVal: *val = isSgUnsignedLongLongIntVal(rhs)->get_value(); return true;
				case V_SgUnsignedLongVal:        *val = isSgUnsignedLongVal(rhs)->get_value();        return true;
				case V_SgUnsignedCharVal:        *val = isSgUnsignedCharVal(rhs)->get_value();        return true;
				case V_SgCharVal:                *val = isSgCharVal(rhs)->get_value();                return true;
				case V_SgUnsignedShortVal:       *val = isSgUnsignedShortVal(rhs)->get_value();       return true;
				default:                                                                              return false;
			}
		}
		case V_SgMinusAssignOp:
		{
			Symbol * sym = st->genSymbol(isSgMinusAssignOp(e)->get_lhs_operand_i());
			if (sym != s)
				return false;
			SgExpression * rhs = isSgMinusAssignOp(e)->get_rhs_operand_i();
			switch (rhs->variantT()) {
				case V_SgIntVal:                 *val = -isSgIntVal(rhs)->get_value();                 return true;
				case V_SgLongIntVal:             *val = -isSgLongIntVal(rhs)->get_value();             return true;
				case V_SgLongLongIntVal:         *val = -isSgLongLongIntVal(rhs)->get_value();         return true;
				case V_SgShortVal:               *val = -isSgShortVal(rhs)->get_value();               return true;
				case V_SgUnsignedIntVal:         *val = -isSgUnsignedIntVal(rhs)->get_value();         return true;
				case V_SgUnsignedLongLongIntVal: *val = -isSgUnsignedLongLongIntVal(rhs)->get_value(); return true;
				case V_SgUnsignedLongVal:        *val = -isSgUnsignedLongVal(rhs)->get_value();        return true;
				case V_SgUnsignedCharVal:        *val = -isSgUnsignedCharVal(rhs)->get_value();        return true;
				case V_SgCharVal:                *val = -isSgCharVal(rhs)->get_value();                return true;
				case V_SgUnsignedShortVal:       *val = -isSgUnsignedShortVal(rhs)->get_value();       return true;
				default:                                                                               return false;
			}
		}
		default: return false;
	}
}

Access * generateAccess(SgExpression * e, PolyhedralElement * elem, SymbolTable * st, bool assign_lhs = false) {
	Access * res = NULL;

	SgVarRefExp * var_ref_exp         = isSgVarRefExp(e);
	SgPntrArrRefExp * arr_ref_exp     = isSgPntrArrRefExp(e);
	SgArrowExp * arrow_exp            = isSgArrowExp(e);
	SgDotExp * dot_exp                = isSgDotExp(e);

	SgAddressOfOp * addr_of_op        = isSgAddressOfOp(e);
	SgPointerDerefExp * deref_op      = isSgPointerDerefExp(e);

	SgFunctionCallExp * func_call_exp = isSgFunctionCallExp(e);

	if (func_call_exp != NULL) {
		std::cerr << "Warning: generateAccess(SgExpression * e=" << e << "): function call are not supported yet... (return NULL)" << std::endl;

		SgFunctionRefExp * func_ref_exp = isSgFunctionRefExp(func_call_exp->get_function());
		if (func_ref_exp != NULL) {

			SgFunctionDeclaration * func_decl = func_ref_exp->get_symbol_i()->get_declaration();

			PolyhedralFunction * poly_func = st->function_table->get(func_decl);

			ROSE_ASSERT(poly_func != NULL);

			res = new ParametrizedAccess(elem, poly_func);

			// TODO associate parameters to the symbols
		}
	}
	else if (var_ref_exp != NULL || arr_ref_exp != NULL || arrow_exp != NULL || dot_exp != NULL || addr_of_op != NULL || deref_op != NULL) {
		Symbol * sym = st->genSymbol(e);
		
		if (sym == NULL) std::cerr << "Error: generateAccess(SgExpression * e=" << e << "(" << e->class_name() << ") ): Cannot find the associated symbol." << std::endl;;
		ROSE_ASSERT(sym != NULL);
		
		DataAccess * data_access = new DataAccess(elem, assign_lhs ? DataAccess::write : DataAccess::read, sym);
		res = data_access;
		
		if (arrow_exp != NULL || dot_exp != NULL || addr_of_op != NULL || deref_op != NULL) {
#if DEBUG_DATA_ACCESS_GENERATION
			std::cerr << "Error: generateAccess(SgExpression * e=" << e << "(" << e->class_name() << ") ) are not supported yet." << std::endl;
			ROSE_ASSERT(false);
#endif /* DEBUG_DATA_ACCESS_GENERATION */
		}
		else if (var_ref_exp != NULL) {
			ROSE_ASSERT(DEBUG_TODO == 0); // TODO
		}
		else if (arr_ref_exp != NULL) {
			
			unsigned int dim_cnt = 0;
			do {
				LinearExpression_ppl subscript_le;
				SgExpression * subscript_exp = arr_ref_exp->get_rhs_operand_i();

				// TODO Expression<Symbol> * genExpression(SgExpression *, PolyhedralElement *, SymbolTable *)
				if (genLinearExpression(subscript_exp, subscript_le, elem, st)) {
					data_access->restrainAccessedDomain(subscript_le, dim_cnt, equality);
				}
				else {
					ROSE_ASSERT(DEBUG_TODO == 0); // TODO I am not sure that something have to be done here
				}

				dim_cnt++;
				arr_ref_exp = isSgPntrArrRefExp(arr_ref_exp->get_lhs_operand_i());
			} while (arr_ref_exp);
		}
		// no else (all cases are handle)
	}
#if DEBUG_DATA_ACCESS_GENERATION
	else {
		std::cerr << "Error: generateAccess(SgExpression * e=" << e << "(" << e->class_name() << ") ): unsupported expression type." << std::endl;
		ROSE_ASSERT(false);
	}
#endif /* DEBUG_DATA_ACCESS_GENERATION */

	return res;
}

bool collectAccess(SgExpression * e, std::set<Access *> & access_vect, PolyhedralElement * elem, SymbolTable * st) {
	std::vector<std::pair<SgExpression *, bool> > sub_exp;
	
	std::stack<std::pair<SgExpression *, bool> > stack_exp;
	stack_exp.push(std::pair<SgExpression *, bool>(e, false));
	
	while (!stack_exp.empty()) {
		std::pair<SgExpression *, bool> current = stack_exp.top();
		stack_exp.pop();
		
		SgVarRefExp * var_ref_exp         = isSgVarRefExp(current.first);
		SgPntrArrRefExp * arr_ref_exp     = isSgPntrArrRefExp(current.first);
		SgArrowExp * arrow_exp            = isSgArrowExp(current.first);
		SgDotExp * dot_exp                = isSgDotExp(current.first);

		SgAddressOfOp * addr_of_op        = isSgAddressOfOp(current.first);
		SgPointerDerefExp * deref_op      = isSgPointerDerefExp(current.first);
		
		SgFunctionCallExp * func_call_exp = isSgFunctionCallExp(current.first);

		SgValueExp * val_exp              = isSgValueExp(current.first);
		
		bool terminal = var_ref_exp   != NULL
					 || arr_ref_exp   != NULL
					 || arrow_exp     != NULL
					 || dot_exp       != NULL
					 || addr_of_op    != NULL
					 || deref_op      != NULL
					 || func_call_exp != NULL
					;
		
		bool non_access_terminal = val_exp != NULL;
		
		if (!terminal && !non_access_terminal) {
			SgBinaryOp * bin_op = isSgBinaryOp(current.first);
			SgUnaryOp * una_op  = isSgUnaryOp(current.first);

			bool is_reduc_op  = isSgPlusAssignOp   (current.first)
							 || isSgMinusAssignOp  (current.first)
							 || isSgMultAssignOp   (current.first)
							 || isSgDivAssignOp    (current.first)
							 || isSgModAssignOp    (current.first)
							 || isSgAndAssignOp    (current.first)
							 || isSgIorAssignOp    (current.first)
							 || isSgXorAssignOp    (current.first)
							 || isSgLshiftAssignOp (current.first)
							 || isSgRshiftAssignOp (current.first)
//							 || isSgPointerAssignOp(current.first)
							;
			bool is_assign_op = isSgAssignOp(current.first) || is_reduc_op;
			
			bool is_crement_op = isSgPlusPlusOp(current.first) || isSgMinusMinusOp(current.first);
			
			if (current.second == true) {
				ROSE_ASSERT(DEBUG_TODO == 0); // TODO figure out the 'def' composition law
				return false;
			}

			if (is_assign_op) {
				if (is_reduc_op) {
					stack_exp.push(std::pair<SgExpression *, bool>(bin_op->get_lhs_operand_i(), false));
				}
				stack_exp.push(std::pair<SgExpression *, bool>(bin_op->get_lhs_operand_i(), true));
				stack_exp.push(std::pair<SgExpression *, bool>(bin_op->get_rhs_operand_i(), false));
			}
			else if (bin_op != NULL) {
				stack_exp.push(std::pair<SgExpression *, bool>(bin_op->get_lhs_operand_i(), false));
				stack_exp.push(std::pair<SgExpression *, bool>(bin_op->get_rhs_operand_i(), false));
			}
			else if (una_op != NULL) {
				stack_exp.push(std::pair<SgExpression *, bool>(una_op->get_operand_i(), false));
				if (is_crement_op)
					stack_exp.push(std::pair<SgExpression *, bool>(una_op->get_operand_i(), true));
			}
			else {
#if DEBUG_DATA_ACCESS_COLLECTION
				std::cerr << "Error: collectAccess(SgExpression * e=" << e << "): unsupported node: " << current.first->class_name() << std::endl;
				ROSE_ASSERT(false);
#else /* DEBUG_DATA_ACCESS_COLLECTION */
				std::cerr << "Warning: collectAccess(SgExpression * e=" << e << "): unsupported node: " << current.first->class_name() << std::endl;
				return false;
#endif /* DEBUG_DATA_ACCESS_COLLECTION */
			}
			continue;	
		}
		
		if (!non_access_terminal) {
			sub_exp.push_back(current);
		}
	}
	
	bool all_translated = true;
	
	std::vector<std::pair<SgExpression *, bool> >::iterator it;
	for (it = sub_exp.begin(); it != sub_exp.end(); it++) {
		Access * access = generateAccess(it->first, elem, st, it->second);
		if (access != NULL) {
			access_vect.insert(access);
		}
		else {
#if DEBUG_DATA_ACCESS_COLLECTION
			std::cerr << "Warning: collectAccess(SgExpression * e=" << e << "): call to generateAccess(SgExpression * e = " << it->first << " (" << it->first->class_name() << "), ...) return NULL." << std::endl;
#endif /* DEBUG_DATA_ACCESS_COLLECTION */
			all_translated = false;
		}
	}
	
	return all_translated;
}

/**********************/
/* PolyhedralFunction */
/**********************/

PolyhedralFunction::PolyhedralFunction(SgFunctionDeclaration * decl, FunctionTable * func_table) :
	function_table(func_table),
	p_decls_set(),
	p_first_decl(NULL),
	p_defn(NULL),
	p_parameters(),
	p_top_symbol_table(new SymbolTable(this)),
	p_return_symbol(p_top_symbol_table->genReturnSymbol())
{
	ROSE_ASSERT(function_table != NULL);

	p_first_decl = isSgFunctionDeclaration(decl->get_firstNondefiningDeclaration());
	if (p_first_decl == NULL) p_first_decl = decl;
	
	ROSE_ASSERT(p_first_decl != NULL);
	
	// Look for definition
	
	SgFunctionDeclaration * defn_decl = isSgFunctionDeclaration(p_first_decl->get_definingDeclaration());
	
	if (defn_decl) {
		p_decls_set.insert(defn_decl);
		p_defn = defn_decl->get_definition();
	}
	
	// Analyse paramaters
	
	SgFunctionParameterList * sg_params_list = NULL;
	if (defn_decl)
		sg_params_list = defn_decl->get_parameterList();
	else
		sg_params_list = p_first_decl->get_parameterList();

	ROSE_ASSERT(sg_params_list != NULL);
	
	const SgInitializedNamePtrList & params_list = sg_params_list->get_args();
	SgInitializedNamePtrList::const_iterator it;
	for (it = params_list.begin(); it != params_list.end(); it++) {
		if (!isSgInitializedName(*it)) continue;
	
		Symbol* sym = p_top_symbol_table->genSymbol(*it);
		
		sym->function_parameter = true;
		
		ROSE_ASSERT(DEBUG_TODO == 0); // TODO caracterize the symbol (function of his type: data or scalar, dimension for array, ...)
		
		p_parameters.push_back(sym);
	}
}

bool PolyhedralFunction::isSameFunction(SgFunctionDeclaration * decl) {
	ROSE_ASSERT(decl != NULL);

	if (decl->get_firstNondefiningDeclaration() == NULL)
		return decl == p_first_decl;
	else if (decl->get_firstNondefiningDeclaration() == p_first_decl) {
		p_decls_set.insert(decl);
		return true;
	}
	else
		return false;
}

bool PolyhedralFunction::isDefined() const {
	return p_defn != NULL;
}

SgFunctionDefinition * PolyhedralFunction::getDefinition() {
	return p_defn;
}

SgFunctionDeclaration * PolyhedralFunction::getDefDecl() {
	ROSE_ASSERT(p_defn != NULL);
	return isSgFunctionDeclaration(p_defn->get_declaration());
}

SymbolTable * PolyhedralFunction::getSymbolTable() {
	return p_top_symbol_table;
}

Symbol * PolyhedralFunction::getReturnSymbol() {
	return p_return_symbol;
}

/*****************/
/* FunctionTable */
/*****************/

FunctionTable::FunctionTable() :
	p_function_set()
{}

FunctionTable::~FunctionTable() {}
		
void FunctionTable::add(SgFunctionDeclaration * func_decl) {
	std::set<PolyhedralFunction *>::iterator it;
	for (it = p_function_set.begin(); it != p_function_set.end(); it++) {
		if ((*it)->isSameFunction(func_decl)) return;
	}
	p_function_set.insert(new PolyhedralFunction(func_decl, this));
}

PolyhedralFunction * FunctionTable::get(SgFunctionDeclaration * func_decl) {
	std::set<PolyhedralFunction *>::const_iterator it;
	for (it = p_function_set.begin(); it != p_function_set.end(); it++) {
		if ((*it)->isSameFunction(func_decl)) return *it;
	}
	return NULL;
}

const std::set<PolyhedralFunction *> & FunctionTable::get() const { return p_function_set; }
std::set<PolyhedralFunction *> & FunctionTable::get() { return p_function_set; }

/**********/
/* Symbol */
/**********/

Symbol::Symbol(unsigned int vid) :
	sage_symbol(NULL),
	parent_symbol(NULL),
	relation_with_parent(nothing),
	function_return(false),
	associated_function(NULL),
	original_function_call(NULL),
	scope(NULL),
	generated(false),
	iterator(false),
	data(false),
	function_parameter(false),
	dimension(0),
	id(vid)
{}

void Symbol::print(std::ostream & out, std::string indent) {
	std::string name;
	if (isSgVariableSymbol(sage_symbol))
		name = isSgVariableSymbol(sage_symbol)->get_declaration()->get_name().getString();
	out << indent << "[ name=\"" << name << "\", sg-sym=" << sage_symbol << ", id=" << id << ", it=" << (iterator ? "true" : "false") << " ]" << std::endl;
}

/***************/
/* SymbolTable */
/***************/

SymbolTable::SymbolTable(PolyhedralFunction * curr_func) :
	parent_table(NULL),
	function_table(NULL),
	current_function(curr_func),
	nb_child(0),
	symbol_set()
{
	ROSE_ASSERT(curr_func != NULL);
	
	function_table = current_function->function_table;
}

SymbolTable::SymbolTable(SymbolTable * pt) :
	parent_table(pt),
	function_table(NULL),
	current_function(NULL),
	nb_child(0),
	symbol_set()
{
	ROSE_ASSERT(parent_table != NULL);
	
	current_function = parent_table->current_function;
	function_table   = parent_table->function_table;

	parent_table->nb_child++;
}

SymbolTable::~SymbolTable() {
	ROSE_ASSERT(nb_child == 0);
	if (parent_table != NULL) parent_table->nb_child--;
	
	ROSE_ASSERT(DEBUG_TODO == 0); // TODO delete symbol
}

Symbol * SymbolTable::genSymbol(SgExpression * e, bool build) {
	switch (e->variantT()) {
		case V_SgVarRefExp:       return genSymbol(isSgVarRefExp(e), build);
		case V_SgArrowExp:        return genSymbol(isSgArrowExp(e), build);
		case V_SgDotExp:          return genSymbol(isSgDotExp(e), build);
		case V_SgPntrArrRefExp:   return genSymbol(isSgPntrArrRefExp(e), build);
		case V_SgPointerDerefExp: return genSymbol(isSgPointerDerefExp(e), build);
		case V_SgAddressOfOp:     return genSymbol(isSgAddressOfOp(e), build);
		default:                  return NULL;
	}
}

Symbol * SymbolTable::makeSymbol() {
	unsigned int id = nbrReferencedVariables();
	Symbol * res = new Symbol(id);
	symbol_set.insert(res);

	ROSE_ASSERT(DEBUG_TODO == 0); 
	// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO
	// TODO TODO TODO TODO  update Polyhedron  TODO TODO TODO TODO
	// TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO TODO

	return res;
}

Symbol * SymbolTable::symbolFor(SgSymbol * s, bool build, bool here) {
	if (s == NULL)
		return NULL;

	Symbol * res = NULL;
	
	std::set<Symbol *>::iterator it;
	for (it = symbol_set.begin(); it != symbol_set.end(); it++) {
		if ((*it)->sage_symbol == s) {
			res = *it;
			break;
		}
	}
	
	if (build && here && res == NULL) {
		res = makeSymbol();
		res->sage_symbol = s;
		ROSE_ASSERT(DEBUG_TODO == 0); // TODO res->data; // saw as array or lvalue in an expression statement (or generated) - or not scalar type ? -
	}

	if (res == NULL && parent_table != NULL)
		res = parent_table->symbolFor(s, build, here);
	
	if (build && res == NULL && parent_table == NULL) {
		res = makeSymbol();
		res->sage_symbol = s;
		ROSE_ASSERT(DEBUG_TODO == 0); // TODO res->data; // saw as array or lvalue in an expression statement (or generated) - or not scalar type ? -
	}
	
	return res;
}

bool SymbolTable::haveSymbol(Symbol * s) const {
	return symbol_set.find(s) != symbol_set.end();
}

Symbol * SymbolTable::genSymbol(SgInitializedName * i) {
	Symbol * res = NULL;
	SgSymbol * sage_symbol = i->get_scope()->get_symbol_table()->find(i);
	if (sage_symbol != NULL) {
		res = symbolFor(sage_symbol, true, true);
		res->scope = i->get_scope();
	}
	else {
		// TODO It's a special case for non-defining function declarations (happens only when no definition exist)
		ROSE_ASSERT(DEBUG_TODO == 0); // TODO It needs to be check !
		res = makeSymbol();
	}
	return res;
}

Symbol * SymbolTable::genSymbol(SgVarRefExp * e, bool build) {
	Symbol * res = symbolFor(e->get_symbol(), build, false);
	res->scope = e->get_symbol()->get_declaration()->get_scope();
	return res;
}

Symbol * SymbolTable::genSymbol(SgArrowExp * e, bool build) {
	Symbol * lhs = genSymbol(e->get_lhs_operand_i(), build);
	if (lhs == NULL) return NULL;

	Symbol * rhs = genSymbol(e->get_rhs_operand_i(), build);
	if (rhs == NULL) return NULL;
	
	rhs->parent_symbol = lhs;
	rhs->relation_with_parent = arrow;

	ROSE_ASSERT(DEBUG_TODO == 0); // TODO res->scope = e->get_declaration()->get_scope();
	
	return rhs;
}

Symbol * SymbolTable::genSymbol(SgDotExp * e, bool build) {
	Symbol * lhs = genSymbol(e->get_lhs_operand_i(), build);
	if (lhs == NULL) return NULL;

	Symbol * rhs = genSymbol(e->get_rhs_operand_i(), build);
	if (rhs == NULL) return NULL;
	
	rhs->parent_symbol = lhs;
	rhs->relation_with_parent = dot;
//	res->scope = e->get_declaration()->get_scope();
	
	return rhs;
}

Symbol * SymbolTable::genSymbol(SgPntrArrRefExp * e, bool build) {
	Symbol * res = NULL;
	
	unsigned int dim = 1;
	SgPntrArrRefExp * lhs = e;
	while (isSgPntrArrRefExp(lhs->get_lhs_operand_i())) {
		lhs = isSgPntrArrRefExp(lhs->get_lhs_operand_i());
		dim++;
	}
	
	res = genSymbol(lhs->get_lhs_operand_i(), build);
	
	if (res != NULL) {
		res->dimension = dim;
		res->data = true;
	}
	return res;
}

Symbol * SymbolTable::genSymbol(SgPointerDerefExp * e, bool build) {
	Symbol * res = NULL;
	
	ROSE_ASSERT(DEBUG_TODO == 0); // TODO SymbolTable::genSymbol(SgPointerDerefExp *)

	return res;
}

Symbol * SymbolTable::genSymbol(SgAddressOfOp * e, bool build) {
	Symbol * res = NULL;

	ROSE_ASSERT(DEBUG_TODO == 0); // TODO SymbolTable::genSymbol(SgAddressOfOp *)

	return res;
}

Symbol * SymbolTable::genSymbol(SgFunctionCallExp * e, bool build) {
	Symbol * res = makeSymbol();
	
	res->function_return = true;
	res->associated_function = NULL; ROSE_ASSERT(DEBUG_TODO == 0); // TODO ask to the function table
	res->original_function_call = e;
		
	return res;
}

Symbol * SymbolTable::genIteratorSymbol(SgScopeStatement * e) {
	Symbol * res = makeSymbol();

	res->scope = e;
	res->generated = true;
	res->iterator = true;
	
	return res;
}

Symbol * SymbolTable::genPredicate(SgScopeStatement * e) {
	Symbol * res = makeSymbol();

	res->scope = e;
	res->generated = true;
	res->data = true;
	
	return res;
}

Symbol * SymbolTable::genReturnSymbol() {
	Symbol * res = makeSymbol();

	res->generated = true;
	res->data = true;
	
	return res;
}

bool SymbolTable::isSymbol(SgExpression * e, Symbol * s) {
	return genSymbol(e, false) == s;
}

unsigned int SymbolTable::nbrReferencedVariables() const {
	unsigned int res = symbol_set.size();
	if (parent_table)
		res += parent_table->nbrReferencedVariables();
	return res;
}

VariableID SymbolTable::getID(Symbol * s) const {
	ROSE_ASSERT(false); // NEVER USED
	if (!haveSymbol(s)) {
		if (parent_table)
			return parent_table->getID(s);
		ROSE_ASSERT(parent_table && haveSymbol(s));
	}
	if (parent_table)
		return VariableID(s->id + parent_table->nbrReferencedVariables());
	return VariableID(s->id);
	
}

void SymbolTable::print(std::ostream & out, std::string indent) {
	out << indent << "Symbol Table: " << this << std::endl;
	out << indent << "\tparent table:    " << parent_table << std::endl;
	out << indent << "\tnumber children: " << nb_child << std::endl;
	
	std::set<Symbol *>::iterator it0;
	for (it0 = symbol_set.begin(); it0 != symbol_set.end(); it0++) {
		(*it0)->print(out, indent + "\t");
	}
}

void SymbolTable::embedSymbolTable(SymbolTable * st) {
	ROSE_ASSERT(st != NULL);
	symbol_set.insert(st->symbol_set.begin(), st->symbol_set.end());
	nb_child += st->nb_child;
	delete st;
}

/*********************/
/* PolyhedralElement */
/*********************/

PolyhedralElement::PolyhedralElement(PolyhedralControl * p) :
	parent(p),
	predicate_map(),
	symbol_id_map(),
	domain()
{}

PolyhedralElement::PolyhedralElement(const PolyhedralElement & elem) :
	parent(elem.parent),
	predicate_map(elem.predicate_map),
	symbol_id_map(elem.symbol_id_map),
	domain()
{}

PolyhedralElement::~PolyhedralElement() {}

void PolyhedralElement::refineDomain(Constraint c) {
	if (c.space_dimension() > domain.space_dimension()) {
		domain.add_space_dimensions_and_embed(c.space_dimension() - domain.space_dimension());
	}
	domain.refine_with_constraint(c);
}

VariableID PolyhedralElement::getID(Symbol * s) {
	std::map<Symbol *, VariableID>::iterator it = symbol_id_map.find(s);
	if (it == symbol_id_map.end()) {
		it = symbol_id_map.insert(std::pair<Symbol *, VariableID>(s, VariableID(symbol_id_map.size()))).first;
	}
	return it->second;
}

void PolyhedralElement::addPredicate(PolyhedralPredicate * pred, bool value) {
	predicate_map.insert(std::pair<PolyhedralPredicate *, bool>(pred, value));
}

/***********************/
/* PolyhedralStatement */
/***********************/

PolyhedralStatement::PolyhedralStatement(SgStatement * stmt) :
	PolyhedralElement((PolyhedralControl *)NULL),
	statement(stmt),
	p_access_set()
{}

PolyhedralStatement::~PolyhedralStatement() {}

void PolyhedralStatement::print(std::ostream & out, std::string indent) {
	out << indent << "Statement " << statement->class_name() << " " << statement << std::endl;

#if DEBUG_PREDICATE
	std::map<PolyhedralPredicate *, bool>::iterator it_pred;
	for (it_pred = predicate_map.begin(); it_pred != predicate_map.end(); it_pred++) {
		out << indent + ">>>\t" << "Need to be " << (it_pred->second ? std::string("true") : std::string("false")) << ": ";
		it_pred->first->print(out, "");
	}
#endif /* DEBUG_PREDICATE */
}

void PolyhedralStatement::addAccess(Access * access) {
	p_access_set.insert(access);
}

void PolyhedralStatement::addAccess(const std::set<Access *> & access) {
	p_access_set.insert(access.begin(), access.end());
}

/***********************/
/* PolyhedralPredicate */
/***********************/

PolyhedralPredicate::PolyhedralPredicate(SgStatement * test_stmt, SgScopeStatement * stmt, SymbolTable * st) :
	PolyhedralStatement(test_stmt),
	predicate(st->genPredicate(stmt))
{
	ROSE_ASSERT(DEBUG_TODO == 0); // TODO add write to the predicate var
}

void PolyhedralPredicate::print(std::ostream & out, std::string indent) {
	out << indent << "Predicate " << statement->class_name() << " " << statement << ", predicate=" << predicate << std::endl;

#if DEBUG_PREDICATE
	std::map<PolyhedralPredicate *, bool>::iterator it_pred;
	for (it_pred = predicate_map.begin(); it_pred != predicate_map.end(); it_pred++) {
		out << indent + ">>>\t" << "Need to be " << (it_pred->second ? std::string("true") : std::string("false")) << ": ";
		it_pred->first->print(out, "");
	}
#endif /* DEBUG_PREDICATE */
}

/*********************/
/* PolyhedralControl */
/*********************/

PolyhedralControl::PolyhedralControl(SgNode * n, SymbolTable * st) :
	PolyhedralElement((PolyhedralControl *)NULL),
	associated_node(n),
	symbol_table(new SymbolTable(st)),
	elements_set(),
	previous_statements(),
	stride(0),
	natural_order(false),
	false_statement(NULL)
{}

PolyhedralControl::PolyhedralControl(PolyhedralControl * control) :
	PolyhedralElement(*control),
	associated_node(control->associated_node),
	symbol_table(new SymbolTable(*(control->symbol_table))),
	elements_set(),
	previous_statements(),
	stride(0),
	natural_order(false),
	false_statement(NULL)
{}

PolyhedralControl::~PolyhedralControl() {
	if (symbol_table)
		delete symbol_table;
}

PolyhedralControl * PolyhedralControl::genNext(SgNode * n) const {
	PolyhedralControl * new_control = new PolyhedralControl(n, symbol_table);
	new_control->parent = this;
	ROSE_ASSERT(DEBUG_TODO == 0); // TODO propagate predicate ? domains ?
	return new_control;
}

void PolyhedralControl::collectPredicate(PolyhedralStatement * stmt) const {
	if (parent != NULL) parent->collectPredicate(stmt);
	
	std::map<PolyhedralPredicate *, bool>::const_iterator it_pred;
	for (it_pred = predicate_map.begin(); it_pred != predicate_map.end(); it_pred++)
		stmt->addPredicate(it_pred->first, it_pred->second);
}

void PolyhedralControl::print(std::ostream & out, std::string indent) {
	out << indent << "Control   ";
	if (associated_node)
		out << associated_node->class_name() << " " << associated_node;
	out << std::endl;
	
#if DEBUG_SYMBOL_TABLE
	symbol_table->print(out, indent + "|\t");
#endif /* DEBUG_SYMBOL_TABLE */

	std::vector<PolyhedralElement *>::iterator it_elem;
	for (it_elem = elements_set.begin(); it_elem != elements_set.end(); it_elem++) {
		(*it_elem)->print(out, indent + "\t");
	}
}

/**********/
/* Access */
/**********/

Access::Access(PolyhedralElement * elem) :
	associated_element(elem)
{}

Access::~Access() {}

/**************/
/* DataAccess */
/**************/

DataAccess::DataAccess(PolyhedralElement * elem, AccessType type, Symbol * s) :
	Access(elem),
	access_type(type),
	symbol(s),
	accessed_domain(),
	dimension(0)
{
	ROSE_ASSERT(symbol != NULL);

	dimension = symbol->dimension;

	if (type == write) symbol->data = true;
}

DataAccess::~DataAccess() {}
	
void DataAccess::restrainAccessedDomain(LinearExpression_ppl & le, unsigned int dim, constraint_type_e type) {
	ROSE_ASSERT(DEBUG_TODO == 0); // TODO DataAccess::restrainAccessedDomain
}

void DataAccess::addAccessDimension() {
	ROSE_ASSERT(DEBUG_TODO == 0); // TODO DataAccess::addAccessDimension
	ROSE_ASSERT(DEBUG_TODO == 0); // TODO update symbol if needed
	ROSE_ASSERT(DEBUG_TODO == 0); // TODO update polyhedron
}

unsigned int DataAccess::getAccessDimension() const {
	return dimension;
}

void DataAccess::print(std::ostream & out, std::string indent) {
	ROSE_ASSERT(DEBUG_TODO == 0); // TODO DataAccess::print
}

/**************************/
/* ParametrizedAccess */
/**************************/

ParametrizedAccess::ParametrizedAccess(PolyhedralElement * elem, PolyhedralFunction * func) :
	Access(elem),
	associated_function(func),
	p_params_args_map()
{
	ROSE_ASSERT(DEBUG_TODO == 0); // TODO fill 'p_params_args_map' using 'associated_element->associated_node' and 'associated_function'
}

ParametrizedAccess::~ParametrizedAccess() {}

void ParametrizedAccess::print(std::ostream & out, std::string indent) {
	ROSE_ASSERT(DEBUG_TODO == 0); // TODO ParametrizedAccess::print
}

/*************/
/* Traversal */
/*************/

PolyhedralModelisation::PolyhedralModelisation() : p_results() {}

FunctionTable * PolyhedralModelisation::traverse(SgProject * project) {
	FunctionTable * func_tab = new FunctionTable();

	std::vector<SgNode*> func_decls = NodeQuery::querySubTree(project, V_SgFunctionDeclaration);
	std::vector<SgNode*>::iterator it0;
	for (it0 = func_decls.begin(); it0 != func_decls.end(); it0++) {
		SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(*it0);
		func_tab->add(func_decl);
	}
	
	std::set<PolyhedralFunction *> & func_set = func_tab->get();
	std::set<PolyhedralFunction *>::iterator it1;
	for (it1 = func_set.begin(); it1 != func_set.end(); it1++) {
		if ((*it1)->isDefined()) {
			std::cerr << "PolyhedralFunction: " << (*it1)->getDefDecl()->get_name().getString() << std::endl << std::endl;
			
			PolyhedralControl * control = new PolyhedralControl((*it1)->getDefinition(), (*it1)->getSymbolTable());
			
			PolyhedralElement * elem = evaluate((*it1)->getDefinition(), control);
			if (elem != NULL) {
				ROSE_ASSERT(DEBUG_TODO == 0); // TODO finalyse -> refactor PolyhedralElement to use privatized var and id from symbol 
				ROSE_ASSERT(DEBUG_TODO == 0); // TODO attach PolyhedralElement = elem

				p_results.insert(elem);

#if DEBUG_ATTACHED_RESULT
				std::cerr << "Attach:" << std::endl;
				elem->print(std::cerr, ">\t\t");
#endif /* DEBUG_ATTACHED_RESULT */
			
			}
			
			(*it1)->getSymbolTable()->print(std::cerr);
			
#if DEBUG_HELPER
			std::cerr << std::endl << std::endl << std::endl;
#endif /* DEBUG_ROSE_MAX_COVER */
		}
	}
	
	return func_tab;
}

PolyhedralElement * PolyhedralModelisation::evaluate(SgNode * n, PolyhedralControl * inh_control, std::string indent) {
	/*****************/
	/* Top-Down part */
	/*****************/
	
	if (isSgLocatedNode(n) && isSgLocatedNode(n)->get_startOfConstruct()->isCompilerGenerated()) {
#if DEBUG_TRAVERSAL
		std::cerr << indent << "Skipping compiler generated node " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */
		delete inh_control;
		return NULL;
	}
	
#if DEBUG_TRAVERSAL
	std::cerr << indent << "Traverse (Top-Down ) " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */

	std::vector<PolyhedralElement *> syn_attr_list; // Attribute synthetized by the bottom-up traversal
	PolyhedralElement * res = inh_control; // Attribute generated in the bottom-up part, place here for short circuit in function definition case

	switch (n->variantT()) {
		case V_SgBasicBlock:
		{
			SgBasicBlock * bb_stmt = isSgBasicBlock(n);
			SgStatementPtrList::iterator it;
			for (it = bb_stmt->get_statements().begin(); it != bb_stmt->get_statements().end(); it++)
				syn_attr_list.push_back(evaluate(*it, inh_control->genNext(*it), indent + "|\t"));
			break;
		}

		case V_SgForStatement:
		{
			SgForStatement * for_stmt = isSgForStatement(n);
			
			SgForInitStatement * for_init_stmt = for_stmt->get_for_init_stmt();
			SgStatement * test_stmt = for_stmt->get_test();
			SgExpression * inc_expr = for_stmt->get_increment();
			
			bool is_static_control = true;
			
			SgStatementPtrList & for_init_stmt_list = for_init_stmt->get_init_stmt();
			
			Symbol * iterator;
			LinearExpression_ppl lower_bound;
			LinearExpression_ppl upper_bound;
			int increment;
			
			if (for_init_stmt_list.size() != 1) is_static_control = false;
			else {
				LinearExpression_ppl lower_bound;
				LinearExpression_ppl upper_bound;
			
				SgExpression * init_rhs_expr;
				SgStatement * init_stmt = for_init_stmt_list[0];
				if (isSgExprStatement(init_stmt)) {
					SgExpression * init_expr = isSgExprStatement(init_stmt)->get_expression();
					SgAssignOp * assign_expr = isSgAssignOp(init_expr);
					if (assign_expr) {
						SgVarRefExp * it_var_ref = isSgVarRefExp(assign_expr->get_lhs_operand_i());
						if (it_var_ref) {
							iterator = inh_control->symbol_table->genSymbol(it_var_ref);
							init_rhs_expr = assign_expr->get_rhs_operand_i();
						}
						else is_static_control = false;
					}
					else is_static_control = false;
				}
				else if (isSgVariableDeclaration(init_stmt)) {
					SgInitializedNamePtrList var_init_list = isSgVariableDeclaration(init_stmt)->get_variables();
					if (var_init_list.size() == 1) {
						SgInitializedName * var_init = var_init_list[0];
						if (isSgAssignInitializer(var_init->get_initptr())) {
							iterator = inh_control->symbol_table->genSymbol(var_init);
							init_rhs_expr = isSgAssignInitializer(var_init->get_initptr())->get_operand_i();
						}
						else is_static_control = false;
					}
					else is_static_control = false;
				}
				else is_static_control = false;
				
				if (is_static_control)
					iterator->iterator = true;
				
				// Check increment before bounds
				is_static_control = is_static_control && checkIncrementAssign(
										iterator,
										inc_expr,
										&increment,
										inh_control,
										inh_control->symbol_table
							);
				
				is_static_control = is_static_control && genLinearExpression(
										init_rhs_expr,
										increment > 0 ? lower_bound : upper_bound,
										inh_control,
										inh_control->symbol_table
							);

				if (is_static_control) {
					if (increment > 0)
						lower_bound = iterator->id - lower_bound;
					else
						upper_bound = upper_bound - iterator->id;
				}

				SgExprStatement * test_stmt_ = isSgExprStatement(test_stmt);
				
				is_static_control = is_static_control && test_stmt_ != NULL;

				std::vector<std::pair<LinearExpression_ppl, bool> > constraint_set;
				is_static_control = is_static_control && genLinearConstraint(
										iterator,
										test_stmt_->get_expression(),
										constraint_set,
										increment > 0,
										true,
										inh_control,
										inh_control->symbol_table
							);

				is_static_control = is_static_control && constraint_set.size() == 1 && constraint_set[0].second == false;
				
				if (is_static_control) {
					if (increment > 0)
						upper_bound = constraint_set[0].first;
					else
						lower_bound = constraint_set[0].first;
				}

				Integer it_val = is_static_control ? (increment > 0 ? upper_bound : lower_bound).coefficient(inh_control->getID(iterator)) : 0;
				is_static_control = is_static_control && it_val != 0 && (increment > 0 xor it_val > 0);
			}
			
			SgStatement * body_stmt = for_stmt->get_loop_body();
			
			if (is_static_control) {
				inh_control->refineDomain(lower_bound >= 0);
				inh_control->refineDomain(upper_bound >= 0);
				inh_control->stride = increment > 0 ? increment : -increment;
				inh_control->natural_order = increment > 0;

				syn_attr_list.push_back(evaluate(body_stmt, inh_control->genNext(body_stmt), indent + "|\t"));
			}
			else {
				// Add 'for_init_stmt_list' to parent
				std::vector<SgStatement *>::iterator it;
				for (it = for_init_stmt_list.begin(); it != for_init_stmt_list.end(); it++)
					inh_control->previous_statements.push_back(evaluate(*it , inh_control->genNext(*it), indent + "|\t"));
				
				// Gen predicate stmt and iterator
				PolyhedralPredicate * predicate = new PolyhedralPredicate(test_stmt, for_stmt, inh_control->symbol_table);
				syn_attr_list.push_back(predicate);
				Symbol * iterator = inh_control->symbol_table->genIteratorSymbol(for_stmt);
				
				// Update 'inh_control' with predicate and open domain
				inh_control->refineDomain(inh_control->getID(iterator) >= 0);
				inh_control->stride = 1;
				inh_control->natural_order = true;

				SgBreakStmt * break_stmt = SageBuilder::buildBreakStmt();

				PolyhedralControl * body_control  =  inh_control->genNext(body_stmt);
				body_control->addPredicate(predicate, true);
				PolyhedralControl * break_control = inh_control->genNext(break_stmt);
				break_control->addPredicate(predicate, false);
				
				syn_attr_list.push_back(evaluate(body_stmt,  body_control,  indent + "|\t"));
				syn_attr_list.push_back(evaluate(break_stmt, break_control, indent + "|\t"));

				SgExprStatement * expr_stmt = SageBuilder::buildExprStatement(inc_expr);
				syn_attr_list.push_back(evaluate(expr_stmt, inh_control->genNext(expr_stmt), indent + "|\t"));
			}

#if DEBUG_FOR_STMT
			if (is_static_control) 
				std::cerr << indent << "# Statically controlled FOR statement found: " << for_stmt << ", " << syn_attr_list.size() << " attributes generated." << std::endl;
			else
				std::cerr << indent << "# Non-statically controlled FOR statement found: " << for_stmt << ", " << syn_attr_list.size() << " attributes generated." << std::endl;
#endif /* DEBUG_FOR_STMT */

			break;
		}
		case V_SgIfStmt:
		{
			SgIfStmt * if_stmt = isSgIfStmt(n);

			SgStatement * cond_stmt  = if_stmt->get_conditional();
			SgStatement * true_stmt  = if_stmt->get_true_body();
			SgStatement * false_stmt = if_stmt->get_false_body();

			SgExprStatement * cond_expr_stmt = isSgExprStatement(cond_stmt);
			SgExpression * cond_expr = NULL;
			if (cond_expr_stmt != NULL) cond_expr = cond_expr_stmt->get_expression();

			std::vector<std::pair<LinearExpression_ppl, bool> > constraint_set;
			bool is_static_control = cond_expr != NULL && genLinearConstraint(
						NULL,
						cond_expr,
						constraint_set,
						true,
						true,
						inh_control,
						inh_control->symbol_table
			);
			
			is_static_control = is_static_control
							 && (
								   (constraint_set.size() == 1 && constraint_set[0].second == false)
							 	|| (constraint_set.size() == 1 && false_stmt == NULL)
							 	|| (constraint_set.size()  > 1 && false_stmt == NULL) // TODO this case is never reach
							 );
									//	Simplified version, do not expose the 3 different cases.
									//	 	   (constraint_set.size() >= 1 && false_stmt == NULL)
									//	 	|| (constraint_set.size() == 1 && constraint_set[0].is_equality == false)

			if (is_static_control) {
				inh_control->false_statement = new PolyhedralControl(inh_control); // before all other modification (it's a copy)
				
				std::vector<std::pair<LinearExpression_ppl, bool> >::iterator it;
				for (it = constraint_set.begin(); it != constraint_set.end(); it++)
					if (it->second == false)
						inh_control->refineDomain(it->first >= 0);
					else
						inh_control->refineDomain(it->first == 0);

				syn_attr_list.push_back(evaluate(true_stmt, inh_control->genNext(true_stmt), indent + "|\t"));

				if (false_stmt != NULL) { // (false_stmt != NULL) => (constraint_set.size() == 1 && constraint_set[0].second == false) => only one inequality
					inh_control->false_statement->refineDomain(constraint_set[0].first <= -1);
					syn_attr_list.push_back(evaluate(false_stmt, inh_control->false_statement->genNext(false_stmt), indent + "|\t"));
				}
			}
			else {
				PolyhedralPredicate * predicate = new PolyhedralPredicate(cond_stmt, if_stmt, inh_control->symbol_table);
				syn_attr_list.push_back(predicate);
				

				PolyhedralControl * true_control  =  inh_control->genNext(true_stmt);
				true_control->addPredicate(predicate, true);
				syn_attr_list.push_back(evaluate(true_stmt,  true_control,  indent + "|\t"));

				if (false_stmt != NULL) {
					PolyhedralControl * false_control = inh_control->genNext(false_stmt);
					false_control->addPredicate(predicate, false);
					syn_attr_list.push_back(evaluate(false_stmt, false_control, indent + "|\t"));
				}
			}

			break;
		}
		case V_SgWhileStmt:
		case V_SgDoWhileStmt:
		{
			std::cerr << "Error: PolyhedralModelisation::evaluate(SgNode * n = " << n << ", PolyhedralControl * inh_control = " << inh_control << "): " << n->class_name() << ": NYI" << std::endl;
			ROSE_ASSERT(false);
		}
		
		case V_SgVariableDeclaration:
		{
			PolyhedralStatement * res = new PolyhedralStatement(isSgStatement(n));
			std::set<Access *> access_vect;
			bool success = true;
			
			SgInitializedNamePtrList var_init_list = isSgVariableDeclaration(n)->get_variables();
			SgInitializedNamePtrList::iterator it;
			for (it = var_init_list.begin(); it != var_init_list.end(); it++) {
				SgInitializedName * var_init = *it;
				Symbol * symbol = inh_control->symbol_table->parent_table->genSymbol(var_init);
				
				access_vect.insert(new DataAccess(res, DataAccess::write, symbol));
				
				SgInitializer * init = isSgInitializer(var_init->get_initptr());
				if (init != NULL) {
					switch (init->variantT()) {
						case V_SgAssignInitializer:
						{
							SgExpression * assign_init_expr = isSgExpression(isSgAssignInitializer(init)->get_operand_i());

							if (assign_init_expr) {
								success = collectAccess(assign_init_expr, access_vect, inh_control, inh_control->symbol_table);
							}
							else {
								success = true;
							}

							break;
						}
						case V_SgConstructorInitializer:
							ROSE_ASSERT(DEBUG_TODO == 0); // TODO see params
						case V_SgDesignatedInitializer:
						case V_SgAggregateInitializer:
							ROSE_ASSERT(DEBUG_TODO == 0); // TODO figure out what needs to be done !
							break;
						default:
							std::cerr << "Runtime Error: PolyhedralModelisation::evaluate(SgNode * n = " << n << ", PolyhedralControl * inh_control = " << inh_control << "): impossible case..." << std::endl;
							ROSE_ASSERT(false);
					}
				}
			}
			
			inh_control->collectPredicate(res);
			
			if (success) {
				res->addAccess(access_vect);
			}
			else {
				delete res;
				res = NULL;
			}
			
			delete inh_control;
			
#if DEBUG_TRAVERSAL
			std::cerr << indent << "Leave                " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */
			
			return res;
		}
		case V_SgExprStatement:
		{
			PolyhedralStatement * res = new PolyhedralStatement(isSgStatement(n));
			
			SgExprStatement * expr_stmt = isSgExprStatement(n);
			SgExpression * expr = isSgExpression(expr_stmt->get_expression());
			
			std::set<Access *> access_vect;
			
			bool success = false;
			if (expr) success = collectAccess(expr, access_vect, inh_control, inh_control->symbol_table);
			else success = true;// TODO -> empty expression: need to be replace by SgNullStatement (ROSE/EDG lvl)
			
			inh_control->collectPredicate(res);
			
			if (success) {
				res->addAccess(access_vect);
			}
			else {
				delete res;
				res = NULL;
			}
			
			delete inh_control;
			
#if DEBUG_TRAVERSAL
			std::cerr << indent << "Leave                " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */
			
			return res;
		}
		case V_SgReturnStmt:
		{
			PolyhedralStatement * res = new PolyhedralStatement(isSgReturnStmt(n));
			std::set<Access *> access_vect;

			Symbol * ret_sym = inh_control->symbol_table->current_function->getReturnSymbol();

			access_vect.insert(new DataAccess(res, DataAccess::write, ret_sym));
			
			SgExpression * expr = isSgExpression(isSgReturnStmt(n)->get_expression());
			
			bool success = false;
			if (expr) success = collectAccess(expr, access_vect, inh_control, inh_control->symbol_table);
			else success = true;

			inh_control->collectPredicate(res);

			if (success) {
				res->addAccess(access_vect);
			}
			else {
				delete res;
				res = NULL;
			}
			
			delete inh_control;
			
#if DEBUG_TRAVERSAL
			std::cerr << indent << "Leave                " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */

			return res;
		}
		case V_SgNullStatement: 
		{
			ROSE_ASSERT(DEBUG_TODO == 0); // TODO Remove during aggregation phase
		}
		case V_SgBreakStmt:
		case V_SgContinueStmt:
		{
			PolyhedralStatement * res = new PolyhedralStatement(isSgStatement(n));
			
			ROSE_ASSERT(DEBUG_TODO == 0); // TODO Special Statement ?
			
			inh_control->collectPredicate(res);
			
			delete inh_control;
			
#if DEBUG_TRAVERSAL
			std::cerr << indent << "Leave                " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */
			
			return res;
		}

		case V_SgFunctionDefinition:
		{
			syn_attr_list.push_back(evaluate(isSgFunctionDefinition(n)->get_body(), inh_control->genNext(isSgFunctionDefinition(n)->get_body()), indent + "|\t"));
			break;
		}
		default:
		{
#if DEBUG_TRAVERSAL
			std::cerr << "In PolyhedralModelisation::evaluate(SgNode * n = " << n << ", PolyhedralControl * inh_control = " << inh_control << "): Non-handled node: " << n->class_name() << std::endl;
			ROSE_ASSERT(DEBUG_TODO == 0);
#endif /* DEBUG_TRAVERSAL */
			delete inh_control;
			return NULL;
		}
	}

	/******************/
	/* Bottom-Up part */
	/******************/
	
#if DEBUG_TRAVERSAL
	std::cerr << indent << "Traverse (Bottom-Up) " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */

	{
		std::vector<PolyhedralElement *>::iterator it;
		if (res != NULL)
			for (it = syn_attr_list.begin(); it != syn_attr_list.end(); it++)
				if (*it == NULL)
					res = NULL; // one child node is outside Polyhedral Model scope

		for (it = syn_attr_list.begin(); it != syn_attr_list.end(); it++) {
			if (res != NULL) {
				inh_control->elements_set.push_back(*it);
			}
			else if (*it != NULL) {
				ROSE_ASSERT(DEBUG_TODO == 0); // TODO finalyse -> refactor PolyhedralElement to use privatized var and id from symbol 
				ROSE_ASSERT(DEBUG_TODO == 0); // TODO attach PolyhedralElement = **it

				p_results.insert(*it);

#if DEBUG_ATTACHED_RESULT
				std::cerr << "Attach:" << std::endl;
				(*it)->print(std::cerr, ">\t\t");
#endif /* DEBUG_ATTACHED_RESULT */
			}
		}

		if (res != NULL && inh_control->associated_node == NULL) // parent node is outside Polyhedral Model scope
			res = NULL;

		if (res == NULL)
			delete inh_control;
	}
	
#if DEBUG_TRAVERSAL
	std::cerr << indent << "Leave                " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */

	return res;
}
