
#include "ppl.hh"

#include "FadaToPPL.hpp"

#include <iterator>

//#define __VERBOSE_
//#define __VERBOSE_PLUS_


namespace FadaToPPL {

/****************************************************************************************/
/*           Implementation of the Polyhedrique representation of dependence            */
/****************************************************************************************/

PolyhedricDependence::PolyhedricDependence(SgStatement * source, SgStatement * destination,
						std::vector<std::string *> * source_iterators,
						std::vector<std::string *> * destination_iterators,
						std::vector<std::string *> * globals):
	p_source(source),
	p_destination(destination),
	p_source_iterators(),
	p_destination_iterators(),
	p_globals(),
	p_minimized_constraints(NULL)
{
	int cpt = 0;
	std::vector<std::string *>::iterator it;

	for (it = source_iterators->begin(); it != source_iterators->end(); it++) {
		p_source_iterators.insert(std::pair<std::string, Parma_Polyhedra_Library::Variable>(
			**it,
			Parma_Polyhedra_Library::Variable(cpt++)
		));
	}

	for (it = destination_iterators->begin(); it != destination_iterators->end(); it++) {
		p_destination_iterators.insert(std::pair<std::string, Parma_Polyhedra_Library::Variable>(
			**it,
			Parma_Polyhedra_Library::Variable(cpt++)
		));
	}

	for (it = globals->begin(); it != globals->end(); it++) {
		p_globals.insert(std::pair<std::string, Parma_Polyhedra_Library::Variable>(
			**it,
			Parma_Polyhedra_Library::Variable(cpt++)
		));
	}

	p_polyhedron = new Parma_Polyhedra_Library::C_Polyhedron(cpt);
}

void PolyhedricDependence::addConstraint(Parma_Polyhedra_Library::Constraint * constraint) {
	p_polyhedron->add_constraint(*constraint);
}

Parma_Polyhedra_Library::Variable * PolyhedricDependence::getSourceIterator(std::string & var) {
	Parma_Polyhedra_Library::Variable * res;
	std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator it = p_source_iterators.find(var);
	if (it != p_source_iterators.end())
		res = &(it->second);
	else
		res = NULL;
	return res;
}

Parma_Polyhedra_Library::Variable * PolyhedricDependence::getDestinationIterator(std::string & var) {
	Parma_Polyhedra_Library::Variable * res;
	std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator it = p_destination_iterators.find(var);
	if (it != p_destination_iterators.end())
		res = &(it->second);
	else
		res = NULL;
	return res;
}

Parma_Polyhedra_Library::Variable * PolyhedricDependence::getGlobal(std::string & var) {
	Parma_Polyhedra_Library::Variable * res;
	std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator it = p_globals.find(var);
	if (it != p_globals.end())
		res = &(it->second);
	else
		res = NULL;
	return res;
}

SgStatement * PolyhedricDependence::getSource() { return p_source; }
SgStatement * PolyhedricDependence::getDestination() { return p_destination; }

int PolyhedricDependence::getNbrConstraints() {
	if (!p_minimized_constraints) {
		p_minimized_constraints = createConstraints();
	}
	return std::distance(p_minimized_constraints->begin(), p_minimized_constraints->end());
}

Parma_Polyhedra_Library::Constraint_System * PolyhedricDependence::getConstraints() {
	if (!p_minimized_constraints) {
		p_minimized_constraints = createConstraints();
	}
	return p_minimized_constraints;
}

Parma_Polyhedra_Library::Constraint_System * PolyhedricDependence::createConstraints() {

	Parma_Polyhedra_Library::Constraint_System * res = new Parma_Polyhedra_Library::Constraint_System();
	const Parma_Polyhedra_Library::Constraint_System & constraints = p_polyhedron->minimized_constraints();
	
	Parma_Polyhedra_Library::Constraint_System::const_iterator constraint_it;
	for (constraint_it = constraints.begin(); constraint_it != constraints.end(); constraint_it++) {
		if ((*constraint_it).is_equality()) {
			Parma_Polyhedra_Library::Linear_Expression exp;
			
			std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator it_var;
			for (it_var = p_source_iterators.begin(); it_var != p_source_iterators.end(); it_var++) {
				exp += constraint_it->coefficient(it_var->second) * it_var->second;
			}
			for (it_var = p_destination_iterators.begin(); it_var != p_destination_iterators.end(); it_var++) {
				exp += constraint_it->coefficient(it_var->second) * it_var->second;
			}
			for (it_var = p_globals.begin(); it_var != p_globals.end(); it_var++) {
				exp += constraint_it->coefficient(it_var->second) * it_var->second;
			}
			exp += constraint_it->inhomogeneous_term();
			
			res->insert(exp <= 0);
			res->insert(exp >= 0);
		}			
		else
			res->insert(*constraint_it);
	}
	
	return res;
}

/****************************************************************************************/
/*                        Implementation of the Quast Traversal                         */
/****************************************************************************************/


/*** Start with some parsing functions ***/

Parma_Polyhedra_Library::Linear_Expression * parseFadaExpressionToPPLLinExp(
	fada::Expression * expression,
	PolyhedricDependence * dependence,
	bool sourceORdestination
) {
#ifdef __VERBOSE_PLUS_
	std::cout << "Enter parseFadaExpressionToPPLLinExp." << std::endl;
#endif
	Parma_Polyhedra_Library::Linear_Expression * res = NULL;

	if(expression->IsLeaf()) {
		if (expression->IsValue()) {
			res = new Parma_Polyhedra_Library::Linear_Expression(expression->GetValue());
		}
		else if (expression->IsVariable()) {
			std::string var = expression->GetVariableName();
			Parma_Polyhedra_Library::Variable * ppl_var = NULL;
			
			bool var_found = (
				ppl_var = sourceORdestination ?
					dependence->getSourceIterator(var) :
					dependence->getDestinationIterator(var)
			) || (
				ppl_var = dependence->getGlobal(var)
			);
			
			if (!var_found) {
				if (var.size() == 1 && var[0] >= '0' && var[0] <= '9') {
					res = new Parma_Polyhedra_Library::Linear_Expression((int)(var[0] - '0'));
				}
				else {
					std::cerr << "Error in FadaToPPL::parseFadaExpressionToPPLLinExp(...) unrecognized Variable !" << std::endl;
					ROSE_ASSERT(false);
				}
			}
			else {
				res = new Parma_Polyhedra_Library::Linear_Expression(*ppl_var);
			}
		}
		else {
			std::cerr << "Error in FadaToPPL::parseFadaExpressionToPPLLinExp(...) unrecognized Quast's leaf !" << std::endl;
			ROSE_ASSERT(false);
		}
	}
	else {
		Parma_Polyhedra_Library::Linear_Expression * lhs;
		Parma_Polyhedra_Library::Linear_Expression * rhs;
	
		switch(expression->GetOperation()) {
			case FADA_ADD:
				lhs = parseFadaExpressionToPPLLinExp(
					expression->GetLeftChild(),
					dependence,
					sourceORdestination
				);
				rhs = parseFadaExpressionToPPLLinExp(
					expression->GetRightChild(),
					dependence,
					sourceORdestination
				);
				res = new Parma_Polyhedra_Library::Linear_Expression(*lhs + *rhs);
				delete rhs;
				delete lhs;
				break;
			case FADA_SUB:
				lhs = parseFadaExpressionToPPLLinExp(
					expression->GetLeftChild(),
					dependence,
					sourceORdestination
				);
				rhs = parseFadaExpressionToPPLLinExp(
					expression->GetRightChild(),
					dependence,
					sourceORdestination
				);
				res = new Parma_Polyhedra_Library::Linear_Expression(*lhs - *rhs);
				delete rhs;
				delete lhs;
				break;
			case FADA_MUL:
				// TODO (at least one operand need to be a constant)
			case FADA_DIV:
			case FADA_MOD:
			default:
				std::cerr << "Error in FadaToPPL::parseFadaExpressionToPPLLinExp(...) unsupported Operator !" << std::endl;
				ROSE_ASSERT(false);
		}
	}
	
	if (!res) {
		std::cerr << "Error in FadaToPPL::parseFadaExpressionToPPLLinExp(...) result value is NULL !" << std::endl;
		ROSE_ASSERT(false);
	}

#ifdef __VERBOSE_PLUS_
	std::cout << "Leave parseFadaExpression." << std::endl;
#endif
	
	return res;
}
	
void parseFadaCondition(
	fada::Condition * condition,
	PolyhedricDependence * dependence,
	bool sourceORdestination,
	bool negation = false
) {
#ifdef __VERBOSE_PLUS_
	std::cout << "Enter parseFadaCondition." << std::endl;
#endif
	if(condition->IsLeaf()) {
		Inequation * ineq = condition->GetInequation();
		if (!ineq->IsValue()) {

			Parma_Polyhedra_Library::Linear_Expression * rhs = parseFadaExpressionToPPLLinExp(
				ineq->GetRHS(),
				dependence,
				sourceORdestination
			);
			Parma_Polyhedra_Library::Linear_Expression * lhs = parseFadaExpressionToPPLLinExp(
				ineq->GetLHS(),
				dependence,
				sourceORdestination
			);
			
			Parma_Polyhedra_Library::Constraint * constraint;
			
			switch(ineq->GetPredicate()) {
				case FADA_LESS:
					constraint = new Parma_Polyhedra_Library::Constraint(negation ? *lhs >= *rhs : *lhs + 1 <= *rhs);
					break;
				case FADA_LESS_EQ:
					constraint = new Parma_Polyhedra_Library::Constraint(negation ? *lhs >= 1 + *rhs : *lhs <= *rhs);
					break;
				case FADA_GREATER:
					constraint = new Parma_Polyhedra_Library::Constraint(negation ? *lhs <= *rhs : *lhs >= 1 + *rhs);
					break;
				case FADA_GREATER_EQ:
					constraint = new Parma_Polyhedra_Library::Constraint(negation ? *lhs + 1 <= *rhs : *lhs >= *rhs);
					break;
				case FADA_EQ:
					if (!negation)
						constraint = new Parma_Polyhedra_Library::Constraint(*lhs == *rhs);
					else {
						std::cerr << "Error in FadaToPPL::parseFadaCondition(...) unsupported Predicate !" << std::endl;
						ROSE_ASSERT(false);
					}
					break;
				case FADA_NEQ:
					if (negation)
						constraint = new Parma_Polyhedra_Library::Constraint(*lhs == *rhs);
					else {
						std::cerr << "Error in FadaToPPL::parseFadaCondition(...) unsupported Predicate !" << std::endl;
						ROSE_ASSERT(false);
					}
					break;
				default: 
					std::cerr << "Error in FadaToPPL::parseFadaCondition(...) unsupported Predicate !" << std::endl;
					ROSE_ASSERT(false);
			}
			
			dependence->addConstraint(constraint);
			
			delete constraint;
			
			delete rhs;
			delete lhs;
		}
	}
	else {
		switch(condition->GetOperation()) {
			case FADA_AND:
				parseFadaCondition(
					condition->GetLeftChild(),
					dependence,
					sourceORdestination,
					negation
				);
				parseFadaCondition(
					condition->GetRightChild(),
					dependence,
					sourceORdestination,
					negation
				);
				break;
			case FADA_NOT :
				parseFadaCondition(
					condition->GetLeftChild(),
					dependence,
					sourceORdestination,
					!negation
				);
				// if an error come from here it can come from 'GetLeftChild' that need to be replace by 'GetRightChild'
				// other issue can come when (not-)equalities will be implemented
				break;
			case FADA_OR : // We work with convex spaces...
			default:
				std::cerr << "Error in FadaToPPL::parseFadaCondition(...) unsupported Operator !" << std::endl;
				ROSE_ASSERT(false);
		}
	}

#ifdef __VERBOSE_PLUS_
	std::cout << "Leave parseFadaCondition." << std::endl;
#endif
}

/*** And now the Traversal ***/

std::vector<PolyhedricDependence *> * traverseQuast(RoseToFada::FadaRoseCrossContext * ctx, SgStatement * source, fada::Quast * quast) {
#ifdef __VERBOSE_PLUS_
	std::cout << "Enter traverseQuast." << std::endl;
#endif
	if (!source) {
		std::cerr << "Error in FadaToPPL::traverseQuast(...) impossible to retrieve correspondant SgStatement for source !" << std::endl;
		ROSE_ASSERT(false);
	}

	std::vector<PolyhedricDependence *> * res = new std::vector<PolyhedricDependence *>();
	if (quast->IsLeaf()) {
#ifdef __VERBOSE_PLUS_
		std::cout << "\ttraverseQuast: isLeaf." << std::endl;
#endif
		if(!(quast->IsEmpty())) {
#ifdef __VERBOSE_PLUS_
			std::cout << "\t\ttraverseQuast: is not an empty Leaf." << std::endl;
#endif
			SgStatement * destination = ctx->getSgStatementByID(quast->GetAssignment());
	
			if (!destination) {
				std::cerr << "Error in FadaToPPL::traverseQuast(...) impossible to retrieve correspondant SgStatement for destination !" << std::endl;
				ROSE_ASSERT(false);
			}
		
			RoseToFada::FadaStatementAttribute * source_attribute = dynamic_cast<RoseToFada::FadaStatementAttribute *>(source->getAttribute("FadaStatement"));
			RoseToFada::FadaStatementAttribute * destination_attribute = dynamic_cast<RoseToFada::FadaStatementAttribute *>(destination->getAttribute("FadaStatement"));
		
			if (!source_attribute || !destination_attribute) {
				std::cerr << "Error in FadaToPPL::traverseQuast(...) wrong FadaStatementAttribute !" << std::endl;
				ROSE_ASSERT(false);
			}
			
			PolyhedricDependence * dependence = new PolyhedricDependence(
				source,
				destination,
				source_attribute->getIterators(),
				destination_attribute->getIterators(),
				ctx->getGlobals()
			);
			
			parseFadaCondition(
				source_attribute->getDomain(),
				dependence,
				true,
				false
			);
			
			parseFadaCondition(
				destination_attribute->getDomain(),
				dependence,
				false,
				false
			);
			
			std::vector<fada::Expression*> * index = quast->GetVertex()->GetIndex();

			std::vector<std::pair<std::string *, int> > * source_iterator_constraints;
			std::vector<std::pair<std::string *, int> > * destination_iterator_constraints;
			std::vector<std::pair<std::string *, int> > * globals_constraints;
			int constant;
			
			int cpt = 0;
			std::vector<fada::Expression *>::iterator it;
			for (it = index->begin(); it != index->end(); it++) {

				// Knowing implementation: Parma_Polyhedra_Library::Variable destination_iterator(SourceIterators.size() + cpt);
				Parma_Polyhedra_Library::Variable * destination_iterator = dependence->getDestinationIterator(*(*destination_attribute->getIterators())[cpt]);
				Parma_Polyhedra_Library::Linear_Expression * rhs = new Parma_Polyhedra_Library::Linear_Expression(*destination_iterator);
				Parma_Polyhedra_Library::Linear_Expression * lhs = parseFadaExpressionToPPLLinExp(
					*it,
					dependence,
					true
				);
				Parma_Polyhedra_Library::Constraint * constraint = new Parma_Polyhedra_Library::Constraint(*rhs == *lhs);
				dependence->addConstraint(constraint);
			
				cpt++;
				
				delete constraint;
				delete rhs;
				delete lhs;
			}
			res->push_back(dependence);
		}
	}
	else {	
#ifdef __VERBOSE_PLUS_	
		std::cout << "\ttraverseQuast: !isLeaf." << std::endl;
#endif
		std::vector<PolyhedricDependence *> * then_res = traverseQuast(ctx, source, quast->GetThenPart());
		std::vector<PolyhedricDependence *> * else_res = traverseQuast(ctx, source, quast->GetElsePart());
		
		fada::Condition * condition = quast->GetCondition();
		
		if (then_res->size() > 0 || else_res->size() > 0) {
					
			std::vector<PolyhedricDependence *>::iterator it;
			for (it = then_res->begin(); it != then_res->end(); it++) {
				parseFadaCondition(
					condition,
					*it,
					true,
					false
				);
				res->push_back(*it);
			}
			for (it = else_res->begin(); it != else_res->end(); it++) {
				parseFadaCondition(
					condition,
					*it,
					true,
					true
				);
				res->push_back(*it);
			}
		}
		
		delete then_res;
		delete else_res;
	}
#ifdef __VERBOSE_PLUS_
	std::cout << "Leave traverseQuast." << std::endl;
#endif
	return res;
}

/*** Thing that I don't want to see above: Printers !  ***/

void PolyhedricDependence::printMinimized(std::ostream & out) {
	Parma_Polyhedra_Library::Constraint_System::const_iterator it_constraint;
	std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator it_var;
	Parma_Polyhedra_Library::GMP_Integer coef;
	for (it_constraint = p_polyhedron->minimized_constraints().begin(); it_constraint != p_polyhedron->minimized_constraints().end(); it_constraint++) {
		for (it_var = p_source_iterators.begin(); it_var != p_source_iterators.end(); it_var++) {
			coef = it_constraint->coefficient(it_var->second);
			if (coef == 1)
				out << "+ S" << it_var->first << " ";
			if (coef == -1)
				out << "- S" << it_var->first << " ";
			if (coef > 1)
				out << "+ " << coef << " * S" << it_var->first << " ";
			if (coef < -1)
				out << "- " << -coef << " * S" << it_var->first << " ";
		}
		for (it_var = p_destination_iterators.begin(); it_var != p_destination_iterators.end(); it_var++) {
			coef = it_constraint->coefficient(it_var->second);
			if (coef == 1)
				out << "+ D" << it_var->first << " ";
			if (coef == -1)
				out << "- D" << it_var->first << " ";
			if (coef > 1)
				out << "+ " << coef << " * D" << it_var->first << " ";
			if (coef < -1)
				out << "- " << -coef << " * D" << it_var->first << " ";
		}
		for (it_var = p_globals.begin(); it_var != p_globals.end(); it_var++) {
			coef = it_constraint->coefficient(it_var->second);
			if (coef == 1)
				out << "+ " << it_var->first << " ";
			if (coef == -1)
				out << "- " << it_var->first << " ";
			if (coef > 1)
				out << "+ " << coef << " * " << it_var->first << " ";
			if (coef < -1)
				out << "- " << -coef << " * " << it_var->first << " ";
		}
		coef = it_constraint->inhomogeneous_term();
		if (coef > 0)
			out << "+ " << coef;
		if (coef < 0)
			out << "- " << -coef;
		out << (it_constraint->is_equality() ? " == 0" : (it_constraint->is_nonstrict_inequality() ? " >= 0" : " >  0"));
		out << std::endl;
	}
	out << std::endl;
}

void PolyhedricDependence::print(std::ostream & out) {
	Parma_Polyhedra_Library::Constraint_System::const_iterator it_constraint;
	std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator it_var;
	Parma_Polyhedra_Library::GMP_Integer coef;
	for (it_constraint = p_polyhedron->constraints().begin(); it_constraint != p_polyhedron->constraints().end(); it_constraint++) {
		for (it_var = p_source_iterators.begin(); it_var != p_source_iterators.end(); it_var++) {
			coef = it_constraint->coefficient(it_var->second);
			if (coef == 1)
				out << "+ S" << it_var->first << " ";
			if (coef == -1)
				out << "- S" << it_var->first << " ";
			if (coef > 1)
				out << "+ " << coef << " * S" << it_var->first << " ";
			if (coef < -1)
				out << "- " << -coef << " * S" << it_var->first << " ";
		}
		for (it_var = p_destination_iterators.begin(); it_var != p_destination_iterators.end(); it_var++) {
			coef = it_constraint->coefficient(it_var->second);
			if (coef == 1)
				out << "+ D" << it_var->first << " ";
			if (coef == -1)
				out << "- D" << it_var->first << " ";
			if (coef > 1)
				out << "+ " << coef << " * D" << it_var->first << " ";
			if (coef < -1)
				out << "- " << -coef << " * D" << it_var->first << " ";
		}
		for (it_var = p_globals.begin(); it_var != p_globals.end(); it_var++) {
			coef = it_constraint->coefficient(it_var->second);
			if (coef == 1)
				out << "+ " << it_var->first << " ";
			if (coef == -1)
				out << "- " << it_var->first << " ";
			if (coef > 1)
				out << "+ " << coef << " * " << it_var->first << " ";
			if (coef < -1)
				out << "- " << -coef << " * " << it_var->first << " ";
		}
		coef = it_constraint->inhomogeneous_term();
		if (coef > 0)
			out << "+ " << coef;
		if (coef < 0)
			out << "- " << -coef;
		out << (it_constraint->is_equality() ? " == 0" : (it_constraint->is_nonstrict_inequality() ? " >= 0" : " >  0"));
		out << std::endl;
	}
	out << std::endl;
}

}
