
#include "rose-max-cover/TDBUtraversal.hpp"

#define DEBUG_TRAVERSAL 1
#define DEBUG_ATTACHED_RESULT 1
#define DEBUG_SYMBOL_TABLE 1
#if DEBUG_SYMBOL_TABLE
#  define DEBUG_ATTACHED_RESULT 1
#endif

bool genLinearExpression(SgExpression * e, LinearExpression & le, PolyhedralElement * elem, SymbolTable * st) {
	// TODO
	return false;
}

bool genLinearConstraint(SgExpression * e, std::vector<LinearExpression> & le_vect, bool inc, bool positive, PolyhedralElement * elem, SymbolTable * st) {
	// TODO
	return false;
}

bool checkIncrementAssign(Symbol * s, SgExpression * e, int * val, PolyhedralElement * elem, SymbolTable * st) {
	// TODO
	return false;
}

/**********/
/* Symbol */
/**********/

Symbol::Symbol(unsigned int vid) :
	sage_symbol(NULL),
	parent_symbol(NULL),
	relation_with_parent(nothing),
	scope(NULL),
	generated(false),
	iterator(false),
	data(false),
	function_parameter(false),
	dimension(0),
	id(vid),
	associated_access()
{}

void Symbol::print(std::ostream & out, std::string indent) {
	std::string name;
	if (isSgVariableSymbol(sage_symbol))
		name = isSgVariableSymbol(sage_symbol)->get_declaration()->get_name().getString();
	out << indent << "[ name=\"" << name << "\", sg-sym=" << sage_symbol << ", id=" << id << ", it=" << (iterator ? "true" : "false") << " ]" << std::endl;
}

/****************/
/* Symbol Table */
/****************/

SymbolTable::SymbolTable(SymbolTable * pt) :
	parent_table(pt),
	nb_child(0),
	symbol_set(),
	associated_access(),
	associated_control(),
	embedded_tables()
{
	if (parent_table != NULL) parent_table->nb_child++;
}

SymbolTable::~SymbolTable() {
	std::set<SymbolTable *>::iterator it;
	for (it = embedded_tables.begin(); it != embedded_tables.end(); it++)
		delete *it;
	
	ROSE_ASSERT(nb_child == 0);
	if (parent_table != NULL) parent_table->nb_child--;
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
		// TODO res->data; // saw as array or lvalue in an expression statement (or generated) - or not scalar type ? -
	}

	if (res == NULL && parent_table != NULL)
		res = parent_table->symbolFor(s, build, here);
	
	if (build && res == NULL && parent_table == NULL) {
		res = makeSymbol();
		res->sage_symbol = s;
		// TODO res->data; // saw as array or lvalue in an expression statement (or generated) - or not scalar type ? -
	}
	
	return res;
}

bool SymbolTable::haveSymbol(Symbol * s) const {
	return symbol_set.find(s) != symbol_set.end();
}

Symbol * SymbolTable::genSymbol(SgInitializedName * i) {
	SgSymbol * sage_symbol = i->get_scope()->get_symbol_table()->find(i);
	Symbol * res = symbolFor(sage_symbol, true, true);
	res->scope = i->get_scope();
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
//	res->scope = e->get_declaration()->get_scope();
	
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
	// TODO SymbolTable::genSymbol(SgPntrArrRefExp *)
	return res;
}

Symbol * SymbolTable::genSymbol(SgPointerDerefExp * e, bool build) {
	Symbol * res = NULL;
	// TODO SymbolTable::genSymbol(SgPointerDerefExp *)
	return res;
}

Symbol * SymbolTable::genSymbol(SgAddressOfOp * e, bool build) {
	Symbol * res = NULL;
	// TODO SymbolTable::genSymbol(SgAddressOfOp *)
	return res;
}

Symbol * SymbolTable::genIteratorSymbol(SgScopeStatement * e) {
	Symbol * res = NULL;
	// TODO SymbolTable::genIteratorSymbol(SgScopeStatement *)
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
	if (!haveSymbol(s)) {
		if (parent_table)
			return parent_table->getID(s);
		ROSE_ASSERT(parent_table && haveSymbol(s));
	}
	if (parent_table)
		return VariableID(s->id + parent_table->nbrReferencedVariables());
	return VariableID(s->id);
	
}
		
void SymbolTable::addControl(PolyhedralControl * v) {
	associated_control.insert(v);
}

void SymbolTable::addDataAccess(DataAccess * v) {
	associated_access.insert(v);
}

void SymbolTable::print(std::ostream & out, std::string indent) {
	out << indent << "Symbol Table: " << this << std::endl;
	out << indent << "\tparent table:    " << parent_table << std::endl;
	out << indent << "\tnumber children: " << nb_child << std::endl;
	
	std::set<Symbol *>::iterator it;
	for (it = symbol_set.begin(); it != symbol_set.end(); it++) {
		(*it)->print(out, indent + "\t");
	}
}

/*********************/
/* PolyhedralElement */
/*********************/

PolyhedralElement::PolyhedralElement(PolyhedralControl * p) :
	parent(p),
	predicate_set(),
	symbol_id_map(),
	domain()
{}

PolyhedralElement::~PolyhedralElement() {}

VariableID PolyhedralElement::getID(Symbol *s) {
	std::map<Symbol *, VariableID>::iterator it = symbol_id_map.find(s);
	if (it != symbol_id_map.end()) {
		it = symbol_id_map.insert(std::pair<Symbol *, VariableID>(s, VariableID(symbol_id_map.size()))).first;
	}
	return it->second;
}

/***********************/
/* PolyhedralStatement */
/***********************/

PolyhedralStatement::PolyhedralStatement(SgStatement * stmt) : PolyhedralElement(NULL), statement(stmt) {}
PolyhedralStatement::~PolyhedralStatement() {}

void PolyhedralStatement::aggregate(PolyhedralElement * elem) {
	std::cerr << "Runtime error cannot call virtual function PolyhedralStatement::aggregate, only defined for PolyhedralControl." << std::endl;
	ROSE_ASSERT(false);
}

void PolyhedralStatement::print(std::ostream & out, std::string indent) {
	out << indent << "Statement " << statement->class_name() << " " << statement << std::endl;
}

/*********************/
/* PolyhedralControl */
/*********************/

PolyhedralControl::PolyhedralControl(SgNode * n, SymbolTable * st) :
	PolyhedralElement(NULL),
	associated_node(n),
	symbol_table(new SymbolTable(st)),
	elements_set()
{
	symbol_table->addControl(this);
}

PolyhedralControl::~PolyhedralControl() {
	if (symbol_table)
		delete symbol_table;
}

PolyhedralControl * PolyhedralControl::genNext(SgNode * n) const {
	PolyhedralControl * new_control = new PolyhedralControl(n, symbol_table);
	new_control->parent = this;
	// TODO propagate predicate ? domains ?
	return new_control;
}

void PolyhedralControl::aggregate(PolyhedralElement * elem) {
	bool elem_have_domain_constraint = false;
	PolyhedralControl * control = dynamic_cast<PolyhedralControl *>(elem);
	if (control)
		elem_have_domain_constraint = !isSgBasicBlock(control->associated_node);
	
	if (control == NULL || elem_have_domain_constraint) {
		elements_set.push_back(elem);
	}
	else {
		std::vector<PolyhedralElement *>::iterator it;
		for (it = control->elements_set.begin(); it != control->elements_set.end(); it++) {
			aggregate(*it);
		}
		symbol_table->embedded_tables.insert(control->symbol_table);
		control->symbol_table = NULL;
		delete elem;
	}
}

void PolyhedralControl::print(std::ostream & out, std::string indent) {
	out << indent << "Control   ";
	if (associated_node)
		out << associated_node->class_name() << " " << associated_node;
	out << std::endl;
	
#if DEBUG_SYMBOL_TABLE
	symbol_table->print(out, indent + "|\t");
#endif /* DEBUG_SYMBOL_TABLE */

	std::vector<PolyhedralElement *>::iterator it;
	for (it = elements_set.begin(); it != elements_set.end(); it++) {
		(*it)->print(out, indent + "\t");
	}
}

/**************/
/* DataAccess */
/**************/

DataAccess::DataAccess(PolyhedralElement * elem, AccessType type, Symbol * s) :
	associated_element(elem),
	access_type(type),
	symbol(s),
	accessed_domain(),
	dimension(0)
{
	ROSE_ASSERT(s != NULL);
	
	dimension = s->dimension;
}
		
void DataAccess::restrainAccessedDomain(LinearExpression & le, unsigned int dim, constraint_type_e type) {
	// TODO DataAccess::restrainAccessedDomain
}
		
void DataAccess::addAccessDimension() {
	// TODO DataAccess::addAccessDimension
	// TODO update symbol if needed
	// TODO update polyhedron
}

unsigned int DataAccess::getAccessDimension() const {
	return dimension;
}

/*************/
/* Traversal */
/*************/

PolyhedralModelisation::PolyhedralModelisation() {}

void PolyhedralModelisation::traverse(SgNode * n) {
	evaluate(n, new PolyhedralControl());
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
				syn_attr_list.push_back(evaluate(*it, inh_control->genNext(bb_stmt), indent + "|\t"));
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
			LinearExpression lower_bound;
			LinearExpression upper_bound;
			int increment;
			
			if (for_init_stmt_list.size() != 1) is_static_control = false;
			else {
				LinearExpression lower_bound;
				LinearExpression upper_bound;
			
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
				
#if 0
				SgExprStatement * test_stmt_ = isSgExprStatement(test_stmt);
				SgExpression * test_rhs_expr;
				bool exclusive_bound = true;
				
				if (is_static_control && test_stmt_ != NULL) {
					SgBinaryOp * bin_op = isSgBinaryOp(test_stmt_->get_expression());
					if (bin_op != NULL) {
						bool lhs_is_it = isSymbol(bin_op->get_lhs_operand_i(), iterator);
						bool rhs_is_it = isSymbol(bin_op->get_rhs_operand_i(), iterator);
						if (!(lhs_is_it xor rhs_is_it)) is_static_control = false;
						if (is_static_control) {
							switch (bin_op->variantT()) {
								case V_SgGreaterOrEqualOp:
									exclusive_bound = false;
									if (increment > 0 && lhs_is_it) is_static_control = false;
									if (increment < 0 && rhs_is_it) is_static_control = false;
									break;
								case V_SgLessOrEqualOp:
									exclusive_bound = false;
									if (increment < 0 && lhs_is_it) is_static_control = false;
									if (increment > 0 && rhs_is_it) is_static_control = false;
									break;
								case V_SgGreaterThanOp:
									if (increment > 0 && lhs_is_it) is_static_control = false;
									if (increment < 0 && rhs_is_it) is_static_control = false;
									break;
								case V_SgLessThanOp:
									if (increment < 0 && lhs_is_it) is_static_control = false;
									if (increment > 0 && rhs_is_it) is_static_control = false;
									break;
								case V_SgNotEqualOp:
									break;
								default:
									is_static_control = false;
							}
						}
						if (is_static_control) {
							if (lhs_is_it)
								
						}
					}
					else is_static_control = false;
				}
				else is_static_control = false;

				is_static_control = is_static_control && genLinearExpression(
										test_rhs_expr,
										increment > 0 ? upper_bound : lower_bound,
										inh_control->symbol_id_map,
										inh_control->symbol_table
							);

				if (is_static_control && exclusive_bound) {
					if (increment > 0)
						upper_bound = upper_bound - 1;
					else
						lower_bound = lower_bound + 1;
				}

				if (is_static_control) {
					lower_bound = iterator->id - lower_bound;
					upper_bound = upper_bound - iterator->id;
				}
#else
				if (is_static_control) {
					if (increment > 0)
						lower_bound = iterator->id - lower_bound;
					else
						upper_bound = upper_bound - iterator->id;
				}

				SgExprStatement * test_stmt_ = isSgExprStatement(test_stmt);
				
				is_static_control = is_static_control && test_stmt_ != NULL;

				std::vector<LinearExpression> constraint_set;
				is_static_control = is_static_control && genLinearConstraint(
										test_stmt_->get_expression(),
										constraint_set,
										increment > 0,
										true,
										inh_control,
										inh_control->symbol_table
							);

				is_static_control = is_static_control && constraint_set.size() == 1;
				
				if (is_static_control) {
					if (increment > 0)
						upper_bound = constraint_set[0];
					else
						lower_bound = constraint_set[0];
				}

				Integer it_val = is_static_control ? (increment > 0 ? upper_bound : lower_bound).coefficient(inh_control->symbol_table->getID(iterator)) : 0;
				is_static_control = is_static_control && it_val != 0 && !(increment > 0 xor it_val > 0);
#endif
			}
			
			if (is_static_control) {
				inh_control->domain.refine_with_constraint(lower_bound >= 0);
				inh_control->domain.refine_with_constraint(upper_bound >= 0);
			}
			else {
				// TODO add 'for_init_stmt_list' to parent
				// TODO gen predicate stmt and iterator
				// TODO update 'inh_control' with predicate and open domain
			}
			
			SgStatement * body_stmt = for_stmt->get_loop_body();
			syn_attr_list.push_back(evaluate(body_stmt, inh_control->genNext(for_stmt), indent + "|\t"));
			
			if (!is_static_control) {
				// TODO add increment stmt ("evaluate(new SgExprStatement(inc_expr), inh_control->genNext(for_stmt), indent + "|\t")")
			}
			
			break;
		}
		case V_SgWhileStmt:
		case V_SgDoWhileStmt:
		case V_SgIfStmt:
		{
			ROSE_ASSERT(false);
		}
		
		case V_SgVariableDeclaration:
		{
			PolyhedralStatement * res = new PolyhedralStatement(isSgStatement(n));
			
			SgInitializedNamePtrList var_init_list = isSgVariableDeclaration(n)->get_variables();
			SgInitializedNamePtrList::iterator it;
			for (it = var_init_list.begin(); it != var_init_list.end(); it++) {
				SgInitializedName * var_init = *it;
				Symbol * symbol = inh_control->symbol_table->parent_table->genSymbol(var_init);
				if (var_init->get_initptr() != NULL) {
					// TODO generate DataAccess
				}
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
			
			// TODO ExprStatement
			
			delete inh_control;
			
#if DEBUG_TRAVERSAL
			std::cerr << indent << "Leave                " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */
			
			return res;
		}
		case V_SgBreakStmt:
		case V_SgContinueStmt:
		case V_SgNullStatement:
		case V_SgReturnStmt:
		{
			PolyhedralStatement * res = new PolyhedralStatement(isSgStatement(n));
			
			// TODO Special Statement ?
			
			delete inh_control;
			
#if DEBUG_TRAVERSAL
			std::cerr << indent << "Leave                " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */
			
			return res;
		}
		
		case V_SgProject:
		{
			SgProject * project = isSgProject(n);
			SgFilePtrList::iterator it;
			for (it = project->get_fileList_ptr()->get_listOfFiles().begin(); it != project->get_fileList_ptr()->get_listOfFiles().end(); it++)
				evaluate(*it, new PolyhedralControl(), indent + "|\t");
			delete inh_control;
			
#if DEBUG_TRAVERSAL
			std::cerr << indent << "Leave                " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */
			
			return NULL;
		}
		case V_SgSourceFile:
		{
			evaluate(isSgSourceFile(n)->get_globalScope(), new PolyhedralControl(), indent + "|\t");
			delete inh_control;
			
#if DEBUG_TRAVERSAL
			std::cerr << indent << "Leave                " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */
			
			return NULL;
		}
		case V_SgGlobal:
		{
			SgGlobal * global_scope = isSgGlobal(n);
			SgDeclarationStatementPtrList::iterator it;
			for (it = global_scope->get_declarations().begin(); it != global_scope->get_declarations().end(); it++)
				evaluate(*it, new PolyhedralControl(), indent + "|\t");
			
#if DEBUG_TRAVERSAL
			std::cerr << indent << "Leave                " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */
			
			return NULL;
		}
		case V_SgFunctionDeclaration:
		{
			SgFunctionDeclaration * func_decl = isSgFunctionDeclaration(n);
			SgFunctionDefinition * func_defn  = isSgFunctionDefinition(func_decl->get_definition());
			if (func_defn != NULL)
				evaluate(func_defn, new PolyhedralControl(), indent + "|\t");
			// TODO function data base
			delete inh_control;
			
#if DEBUG_TRAVERSAL
			std::cerr << indent << "Leave                " << n->class_name() << " " << n << std::endl;
#endif /* DEBUG_TRAVERSAL */
			
			return NULL;
		}
		case V_SgFunctionDefinition:
		{
			PolyhedralControl * control = new PolyhedralControl(n);

			// TODO 0 add function decl symbols

			syn_attr_list.push_back(evaluate(isSgFunctionDefinition(n)->get_body(), control, indent + "|\t"));
			res = NULL; // force the bottom-up attribute to be attach to the current node
			break;
		}
		default:
			ROSE_ASSERT(false);
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
				res->aggregate(*it);
			}
			else if (*it != NULL) {
				// TODO finalyse -> refactor PolyhedralElement to use privatized var and id from symbol 
				// TODO attach PolyhedralElement = **it && more verbose

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
