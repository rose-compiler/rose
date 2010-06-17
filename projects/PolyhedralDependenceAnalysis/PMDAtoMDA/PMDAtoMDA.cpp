
#include "PMDAtoMDA.hpp"

#include <ostream>

namespace MultiDimArrays {

/** class PseudoMultiDimensionalArrayAttribute ;) **/

class PseudoMultiDimensionalArrayAttribute::factorIR {
	protected:
		std::vector<SgInitializedName *> p_globals;
		SgInitializedName * p_iterator;
		int p_constant;
	public:
		factorIR(factors fact) :
			p_globals(),
			p_iterator(NULL),
			p_constant(fact.first)
		{
			std::vector<SgInitializedName *>::iterator it;
			for (it = fact.second->begin(); it != fact.second->end(); it++) {
				if ((*it)->attributeExists("IteratorAttribute"))
					if (!p_iterator)
						p_iterator = *it;
					else {
						ROSE_ASSERT(false);
					}
				else
					p_globals.push_back(*it);
			}
		}
				
		int dimension() { return p_globals.size(); }
		
	friend bool operator == (factorIR & v1, factorIR & v2);
	friend bool operator < (factorIR & v1, factorIR & v2);
	friend void proccessAnalysis();
			
	friend class AccessVectorComponentIR;
};
		
class PseudoMultiDimensionalArrayAttribute::AccessVectorComponentIR {
	protected:
		std::vector<SgInitializedName *> p_globals;
		std::vector<std::pair<SgInitializedName *, int> > p_iterators;
	public:
		AccessVectorComponentIR(factorIR * fact) :
			p_globals(fact->p_globals),
			p_iterators()
		{
			p_iterators.push_back(std::pair<SgInitializedName *, int>(fact->p_iterator, fact->p_constant));
		}
				
		void addTerm(factorIR * fact) {
			p_iterators.push_back(std::pair<SgInitializedName *, int>(fact->p_iterator, fact->p_constant));
			
			// TODO check equal of globals ?
		}
		
		SgExpression * buildSgExpression() {
			SgExpression * res = NULL;
			if (p_iterators.size() > 0) {
				if (p_iterators.begin()->first)
					res = SageBuilder::buildMultiplyOp(
						SageBuilder::buildVarRefExp(p_iterators.begin()->first , p_iterators.begin()->first->get_scope()),
						SageBuilder::buildIntVal(p_iterators.begin()->second)
					);
				else
					res = SageBuilder::buildIntVal(p_iterators.begin()->second);
				std::vector<std::pair<SgInitializedName *, int> >::iterator it_it;
				for (it_it = p_iterators.begin()+1; it_it != p_iterators.end(); it_it++) {
					if (it_it->first)
						res = SageBuilder::buildAddOp(
							res,
							SageBuilder::buildMultiplyOp(
								SageBuilder::buildVarRefExp(it_it->first , it_it->first->get_scope()),
								SageBuilder::buildIntVal(it_it->second)
							)
						);
					else
						res = SageBuilder::buildAddOp(
							res,
							SageBuilder::buildIntVal(it_it->second)
						);
				}
			}
			if (!res) {
				std::cerr << "This access vector component is empty : you have a problem !" << std::endl;
				ROSE_ASSERT(false);				
			}
			return res;
		}
};

bool PseudoMultiDimensionalArrayAttribute::sort_factorIR_pntr(factorIR * a, factorIR * b) { return *a < *b; }
		
void PseudoMultiDimensionalArrayAttribute::proccessAnalysis() {
	std::sort(p_factors.begin(), p_factors.end(), &MultiDimArrays::PseudoMultiDimensionalArrayAttribute::sort_factorIR_pntr);
					
	while (p_factors.size() > 0) {
		factorIR * current = *p_factors.begin();
		AccessVectorComponentIR * comp = new AccessVectorComponentIR(current);
				
		std::vector<factorIR *>::iterator it = p_factors.erase(p_factors.begin());
		while (it != p_factors.end() && **it == *current) {
			comp->addTerm(*it);
			it = p_factors.erase(it);
		}
		
		p_components.push_back(comp);
	}			
}

PseudoMultiDimensionalArrayAttribute::PseudoMultiDimensionalArrayAttribute(std::vector<factors> * factors_vect) :
	p_factors()
{
	std::vector<factors>::iterator it;
	for (it = factors_vect->begin(); it != factors_vect->end(); it++) {
		p_factors.push_back(new factorIR(*it));
	}
	delete factors_vect;
	
	proccessAnalysis();
}

PseudoMultiDimensionalArrayAttribute::~PseudoMultiDimensionalArrayAttribute() {
	std::vector<factorIR *>::iterator it;
	for (it = p_factors.begin(); it != p_factors.end(); it++)
		delete *it;
}
		
bool PseudoMultiDimensionalArrayAttribute::isPseudoMultiDimArray() { return p_components.size() > 1; }
int PseudoMultiDimensionalArrayAttribute::nbrComponent() { return p_components.size(); }
SgExpression * PseudoMultiDimensionalArrayAttribute::buildComponent(int idx) { return p_components[idx]->buildSgExpression(); }

/** Functions that create attributes for SgPntrArrRef **/

/*** Some usefull functions and definitions ***/

void print(std::ostream & out, set_of_factors * set) {
	set_of_factors::iterator fact_it;
	std::vector<SgInitializedName *>::iterator init_name_it;
	for (fact_it = set->begin(); fact_it != set->end(); fact_it++) {
		out << "< " << fact_it->first << ", ( ";
		for (init_name_it = fact_it->second->begin(); init_name_it != fact_it->second->end(); init_name_it++) {
			out << (*init_name_it)->get_name().getString();
			if (init_name_it != fact_it->second->end() - 1)
				out << ", ";
		}
		out << " ) >" << std::endl;
	}
	out << std::endl;
}

void invert(set_of_factors * set) {
	set_of_factors::iterator it;
	for (it = set->begin(); it != set->end(); it++) {
		it->first = -it->first;
	}
}

set_of_factors * somme(set_of_factors * set0, set_of_factors * set1) {
	set0->insert(set0->end(), set1->begin(), set1->end());
	delete set1;
	return set0;
}

set_of_factors * product(set_of_factors * set0, set_of_factors * set1) {
	set_of_factors * res = new set_of_factors();
/*
	std::cout << "----------------" << std::endl;
	std::cout << "Product:" << std::endl;
	print(std::cout, set0);
	std::cout << "****************" << std::endl;
	print(std::cout, set1);
	std::cout << "================" << std::endl;
*/
	set_of_factors::iterator it0;
	set_of_factors::iterator it1;
	for (it0 = set0->begin(); it0 != set0->end(); it0++) {
		for (it1 = set1->begin(); it1 != set1->end(); it1++) {
			factors product;
			product.first = it0->first * it1->first;
			product.second = new std::vector<SgInitializedName *>();
			product.second->insert(product.second->end(), it0->second->begin(), it0->second->end());
			product.second->insert(product.second->end(), it1->second->begin(), it1->second->end());
			res->push_back(product);
		}
	}
	delete set0;
	delete set1;
/*
	print(std::cout, res);
	std::cout << "----------------" << std::endl;
*/
	return res;
}

/*** Do a devellopment of the access function of the array ***/
set_of_factors * traverseArrayAccessExpression(SgExpression * exp) {
//	std::cout << "traverseArrayAccessExpression..." << std::endl;
	set_of_factors * res;	
	switch (exp->variantT()) {
		case V_SgMultiplyOp:
		{
//			std::cout << "\tSgMultiplyOp..." << std::endl;
			SgMultiplyOp * op = isSgMultiplyOp(exp);
			set_of_factors * lhs = traverseArrayAccessExpression(op->get_lhs_operand_i());
			set_of_factors * rhs = traverseArrayAccessExpression(op->get_rhs_operand_i());
			
			res = product(lhs, rhs);
			break;
		}
		case V_SgAddOp:
		{
//			std::cout << "\tSgAddOp..." << std::endl;
			SgAddOp * op = isSgAddOp(exp);
			set_of_factors * lhs = traverseArrayAccessExpression(op->get_lhs_operand_i());
			set_of_factors * rhs = traverseArrayAccessExpression(op->get_rhs_operand_i());
			
			res = somme(lhs, rhs);
			break;
		}
		case V_SgSubtractOp:
		{
//			std::cout << "\tSgSubtractOp..." << std::endl;
			SgSubtractOp * op = isSgSubtractOp(exp);
			set_of_factors * lhs = traverseArrayAccessExpression(op->get_lhs_operand_i());
			set_of_factors * rhs = traverseArrayAccessExpression(op->get_rhs_operand_i());
			invert(rhs);
			
			res = somme(lhs, rhs);
			break;
		}
		case V_SgMinusOp:
		{
//			std::cout << "\tSgMinusOp..." << std::endl;
			SgMinusOp * op = isSgMinusOp(exp);
			res = traverseArrayAccessExpression(op->get_operand_i());
			invert(res);
			break;
		}
		case V_SgVarRefExp:
		{
//			std::cout << "\tSgVarRefExp..." << std::endl;
			SgVarRefExp * var_ref_exp = isSgVarRefExp(exp);
			std::vector<SgInitializedName *> * fact = new std::vector<SgInitializedName *>();
			fact->push_back(var_ref_exp->get_symbol()->get_declaration());
			res = new set_of_factors();
			res->push_back(factors(1, fact));
			break;
		}
		case V_SgIntVal:
		{
//			std::cout << "\tSgIntVal..." << std::endl;
			SgIntVal * int_exp = isSgIntVal(exp);
			std::vector<SgInitializedName *> * fact = new std::vector<SgInitializedName *>();
			res = new set_of_factors();
			res->push_back(factors(int_exp->get_value(), fact));
			break;
		}
		default:{
//			std::cerr << "Unsupported SgExpression: " << exp->class_name() << std::endl;
			ROSE_ASSERT(false);
		}
	}
	
//	print(std::cout, res);
	
	return res;
}

/*** Look for SgPntrArrRef and create annotation if needed ***/
void traverseSgExpression(SgExpression * exp) {
//	std::cout << "traverseSgExpression..." << std::endl;
	if (!isSgPntrArrRefExp(exp)) {
//		std::cout << "\t!isSgPntrArrRefExp()..." << std::endl;
		SgBinaryOp * bop;
		SgUnaryOp * uop;
		if (bop = isSgBinaryOp(exp)) {
//			std::cout << "\t\tSgBinaryOp..." << std::endl;
			traverseSgExpression(bop->get_lhs_operand_i());
			traverseSgExpression(bop->get_rhs_operand_i());
		}
		else if (uop = isSgUnaryOp(exp)) {
//			std::cout << "\t\tSgUnaryOp..." << std::endl;
			traverseSgExpression(uop->get_operand_i());
		}
		else switch (exp->variantT()) {
			case V_SgVarRefExp:
			case V_SgIntVal:
//				std::cout << "\t\tNothing to do..." << std::endl;
				break;
			default:
			{
//				std::cerr << "Unsupported SgExpression: " << exp->class_name() << std::endl;
				ROSE_ASSERT(false);
			}
		}
	}
	else {
//		std::cout << "\tisSgPntrArrRefExp()..." << std::endl;
		set_of_factors * dvlp_expr = traverseArrayAccessExpression(isSgPntrArrRefExp(exp)->get_rhs_operand_i());

		PseudoMultiDimensionalArrayAttribute * attr = new PseudoMultiDimensionalArrayAttribute(dvlp_expr);
		
		if (attr->isPseudoMultiDimArray()) {
			exp->setAttribute("PseudoMultiDimensionalArray", attr);
//			std::cout << "PseudoMultiDimensionalArrayAttribute added..." << std::endl << "\t";
//			for (int i = 0; i < attr->nbrComponent(); i++)
//				std::cout << "[ " << attr->buildComponent(i)->unparseToString() << " ]";
//			std::cout << std::endl;
		}
		else {
			delete attr;
//			std::cout << "\t\tPseudoMultiDimensionalArrayAttribute removed..." << std::endl;
		}
		traverseSgExpression(isSgPntrArrRefExp(exp)->get_lhs_operand_i());
	}
		
}

/** Functions that traverse the ast   **/

void traverse(SgFunctionDeclaration * func_decl) {
	SgFunctionDefinition * func_def = isSgFunctionDefinition(func_decl->get_definition());
	ROSE_ASSERT(func_def != NULL);

	SgBasicBlock * bb = isSgBasicBlock(func_def->get_body());
	ROSE_ASSERT(bb != NULL);
	
	traverse(bb);
}

void traverseSgForStatement(SgForStatement * for_stmt) {
	// For Initialisation
	{
		SgForInitStatement * init_stmt = for_stmt->get_for_init_stmt();
		SgExprStatement * init_exp_stmt = isSgExprStatement(init_stmt->get_init_stmt()[0]);
		if (!init_exp_stmt) {
			std::cerr << "in traverseSgForStatement: initialisation statement is not an expression." << std::endl;
			ROSE_ASSERT(false);
		}
		SgExpression * init_exp =  init_exp_stmt->get_expression();
	
		SgAssignOp * assign_op = isSgAssignOp(init_exp);
		if (!assign_op) {
			std::cerr << "in traverseSgForStatement: initialisation statement is not an assignment." << std::endl;
			ROSE_ASSERT(false);
		}
	
		SgVarRefExp * var_ref_exp = isSgVarRefExp(assign_op->get_lhs_operand_i());
		if (!var_ref_exp) {
			std::cerr << "in traverseSgForStatement: initialisation statement lhs is not an variable." << std::endl;
			ROSE_ASSERT(false);
		}
		
		SgInitializedName * init_name = var_ref_exp->get_symbol()->get_declaration();
		IteratorAttribute * attribute;
		if (init_name->attributeExists("IteratorAttribute"))
			attribute = dynamic_cast<IteratorAttribute*>(init_name->getAttribute("IteratorAttribute"));
		else
			attribute = new IteratorAttribute();
		ROSE_ASSERT(attribute);
		
		attribute->setIterator(true);
		
		init_name->setAttribute("IteratorAttribute", attribute);
		
//		std::cout << "IteratorAttribute added..." << std::endl;
	}
	
	traverse(for_stmt->get_loop_body());
}

void traverseSgVariableDeclaration(SgVariableDeclaration * var_decl) {/**/}

void traverseSgExprStatement(SgExprStatement * expr_stmt) {
	traverseSgExpression(expr_stmt->get_expression());
}

void traverseSgBasicBlock(SgBasicBlock * bb_stmt) {
	SgStatementPtrList & stmt_list = bb_stmt->get_statements();
	SgStatementPtrList::iterator stmt_it;
	for (stmt_it = stmt_list.begin(); stmt_it != stmt_list.end(); stmt_it++) {
		traverse(*stmt_it);
	}
}

void traverse(SgStatement * stmt) {
	switch (stmt->variantT()) {
		case V_SgBasicBlock:
			traverseSgBasicBlock(isSgBasicBlock(stmt));
			break;
		case V_SgForStatement:
			traverseSgForStatement(isSgForStatement(stmt));
			break;
		case V_SgVariableDeclaration:
			traverseSgVariableDeclaration(isSgVariableDeclaration(stmt));
			break;
		case V_SgExprStatement:
			traverseSgExprStatement(isSgExprStatement(stmt));
			break;
		default:
			std::cerr << "Unsupported statement :" << stmt->class_name() << std::endl;
			ROSE_ASSERT(false);
	}
}
		
bool operator == (PseudoMultiDimensionalArrayAttribute::factorIR & v1, PseudoMultiDimensionalArrayAttribute::factorIR & v2) {
	if (v1.p_globals.size() == v2.p_globals.size()) {
		std::vector<SgInitializedName *>::iterator it1, it2;
		bool find_all = true;
		for (it1 = v1.p_globals.begin(); it1 != v1.p_globals.end(); it1++) {
			bool find_one = false;
			for (it2 = v2.p_globals.begin(); it2 != v2.p_globals.end(); it2++)
				find_one |= (*it1 == *it2);
			find_all &= find_one;
		}
		return find_all;
	}
	else
		return false;
}
		
bool operator < (PseudoMultiDimensionalArrayAttribute::factorIR & v1, PseudoMultiDimensionalArrayAttribute::factorIR & v2) {
	if (v1.p_globals.size() == v2.p_globals.size()) {
		if (v1 == v2)
			return false;
		else {
			std::cerr << "Try to compare two factorIR of same size, wich is an indecedable problem !" << std::endl;
			ROSE_ASSERT(false);
		}
	}
	else
		return (v1.p_globals.size() < v2.p_globals.size());
}

}
