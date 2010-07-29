
#include "ppl.hh"

#include "Schedule.hpp"

#include <utility>

namespace PolyhedralSchedule {

/***/

ScheduleVariablesMappingPPL::ScheduleVariablesMappingPPL(
	RoseToFada::FadaRoseCrossContext * ctx,
	SgStatement * statement,
	ValidScheduleSpacePPL * schedule_space
):
	p_iterators_coeficiant(),
	p_globals_coeficiant()
{
	ROSE_ASSERT(statement);
		
	RoseToFada::FadaStatementAttribute * statement_attribute = dynamic_cast<RoseToFada::FadaStatementAttribute *>(statement->getAttribute("FadaStatement"));
	
	if (!statement_attribute) {
		std::cerr << "Error in PolyhedralSchedule::ScheduleVariablesMappingPPL::ScheduleVariablesMappingPPL(...) wrong FadaStatementAttribute !" << std::endl;
		ROSE_ASSERT(false);
	}
	
	std::vector<std::string *>::iterator it;
	for (it = statement_attribute->getIterators()->begin(); it != statement_attribute->getIterators()->end(); it++) {
		p_iterators_coeficiant.insert(std::pair<std::string, Parma_Polyhedra_Library::Variable>(
			**it,
			Parma_Polyhedra_Library::Variable(schedule_space->nextVariable())
		));
	}
	for (it = ctx->getGlobals()->begin(); it != ctx->getGlobals()->end(); it++) {
		p_globals_coeficiant.insert(std::pair<std::string, Parma_Polyhedra_Library::Variable>(
			**it,
			Parma_Polyhedra_Library::Variable(schedule_space->nextVariable())
		));
	}
	p_constant_coeficiant = schedule_space->nextVariable();
}

Parma_Polyhedra_Library::Variable * ScheduleVariablesMappingPPL::getIteratorCoeficiant(std::string & var) {
	Parma_Polyhedra_Library::Variable * res;
	std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator it = p_iterators_coeficiant.find(var);
	if (it != p_iterators_coeficiant.end())
		res = &(it->second);
	else
		res = NULL;
	return res;
}

Parma_Polyhedra_Library::Variable * ScheduleVariablesMappingPPL::getGlobalCoeficiant(std::string & var) {
	Parma_Polyhedra_Library::Variable * res;
	std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator it = p_globals_coeficiant.find(var);
	if (it != p_globals_coeficiant.end())
		res = &(it->second);
	else
		res = NULL;
	return res;
}

Parma_Polyhedra_Library::Variable * ScheduleVariablesMappingPPL::getConstantCoeficiant() { return new Parma_Polyhedra_Library::Variable(p_constant_coeficiant); }

/***/

ValidScheduleSpacePPL::ValidScheduleSpacePPL(
	RoseToFada::FadaRoseCrossContext * ctx,
	std::vector<FadaToPPL::PolyhedricDependence *> & dependences
):
	p_map_statement_variables_mapping(),
	p_context(ctx),
	p_variables_counter(0)
{
	
	int nbr_lambda = 0;
	
	std::vector<FadaToPPL::PolyhedricDependence *>::iterator dep_it;
	for (dep_it = dependences.begin(); dep_it != dependences.end(); dep_it++) {
		p_map_statement_variables_mapping.insert(std::pair<SgStatement *, ScheduleVariablesMappingPPL *>((*dep_it)->getSource(), NULL));
		p_map_statement_variables_mapping.insert(std::pair<SgStatement *, ScheduleVariablesMappingPPL *>((*dep_it)->getDestination(), NULL));
		nbr_lambda += (*dep_it)->getNbrConstraints() + 1;
	}
	
	std::map<SgStatement *, ScheduleVariablesMappingPPL *>::iterator stmt_it;
	for (stmt_it = p_map_statement_variables_mapping.begin(); stmt_it != p_map_statement_variables_mapping.end(); stmt_it++) {
		(*stmt_it).second = new ScheduleVariablesMappingPPL(p_context, (*stmt_it).first, this);
	}
	
	p_polyhedron = new Parma_Polyhedra_Library::C_Polyhedron(p_variables_counter + nbr_lambda);
	
	FillPolyhedron(dependences);
}

void ValidScheduleSpacePPL::FillPolyhedron(std::vector<FadaToPPL::PolyhedricDependence *> & dependences) {
	
	std::vector<FadaToPPL::PolyhedricDependence *>::iterator dep_it;
	for (dep_it = dependences.begin(); dep_it != dependences.end(); dep_it++) {
		AddDependenceToPolyhedron(*dep_it);
	}
}

void ValidScheduleSpacePPL::AddDependenceToPolyhedron(FadaToPPL::PolyhedricDependence * dependence) {
	
	SgStatement * source = dependence->getSource();
	SgStatement * destination = dependence->getDestination();
	
	ROSE_ASSERT(source && destination);
	
	RoseToFada::FadaStatementAttribute * source_attribute = dynamic_cast<RoseToFada::FadaStatementAttribute *>(source->getAttribute("FadaStatement"));
	RoseToFada::FadaStatementAttribute * destination_attribute = dynamic_cast<RoseToFada::FadaStatementAttribute *>(destination->getAttribute("FadaStatement"));
		
	if (!source_attribute || !destination_attribute) {
		std::cerr << "Error in PolyhedralSchedule::ValidScheduleSpacePPL::AddDependenceToPolyhedron(...) wrong FadaStatementAttribute !" << std::endl;
		ROSE_ASSERT(false);
	}
	
	ScheduleVariablesMappingPPL * source_var_map = NULL;
	ScheduleVariablesMappingPPL * destination_var_map = NULL;
	
	std::map<SgStatement *, ScheduleVariablesMappingPPL *>::iterator var_map_it = p_map_statement_variables_mapping.find(source);
	if (var_map_it == p_map_statement_variables_mapping.end()) {
		std::cerr << "Error in PolyhedralSchedule::ValidScheduleSpacePPL::AddDependenceToPolyhedron(...) non-mapped source statement !" << std::endl;
		ROSE_ASSERT(false);
	}
	source_var_map = var_map_it->second;
	if (source_var_map == NULL) {
		std::cerr << "Error in PolyhedralSchedule::ValidScheduleSpacePPL::AddDependenceToPolyhedron(...) source statement variable map doesn't exist !" << std::endl;
		ROSE_ASSERT(false);
	}	
	
	var_map_it = p_map_statement_variables_mapping.find(destination);
	if (var_map_it == p_map_statement_variables_mapping.end()) {
		std::cerr << "Error in PolyhedralSchedule::ValidScheduleSpacePPL::AddDependenceToPolyhedron(...) non-mapped destination statement !" << std::endl;
		ROSE_ASSERT(false);
	}
	destination_var_map = var_map_it->second;
	if (destination_var_map == NULL) {
		std::cerr << "Error in PolyhedralSchedule::ValidScheduleSpacePPL::AddDependenceToPolyhedron(...) destination statement variable map doesn't exist !" << std::endl;
		ROSE_ASSERT(false);
	}
	
	Parma_Polyhedra_Library::Constraint_System * constraints = dependence->getConstraints();
	
	ROSE_ASSERT(constraints);
	
	std::vector<std::string *>::iterator str_it;
	Parma_Polyhedra_Library::Constraint_System::const_iterator constraint_it;
	
	for (str_it = source_attribute->getIterators()->begin(); str_it != source_attribute->getIterators()->end(); str_it++) {
		int counter = p_variables_counter + 1;
		Parma_Polyhedra_Library::Linear_Expression exp;
		Parma_Polyhedra_Library::Variable * dependence_var = dependence->getSourceIterator(**str_it);
		for (constraint_it = constraints->begin(); constraint_it != constraints->end(); constraint_it++) {
			Parma_Polyhedra_Library::Variable lambda = Parma_Polyhedra_Library::Variable(counter++);
			exp += ((*constraint_it).coefficient(*dependence_var) + (*constraint_it).inhomogeneous_term()) * lambda;
		}
		Parma_Polyhedra_Library::Variable * schedule_var = source_var_map->getIteratorCoeficiant(**str_it);
		ROSE_ASSERT(schedule_var);
		p_polyhedron->add_constraint_and_minimize(*schedule_var == exp);
	}
	
	for (str_it = destination_attribute->getIterators()->begin(); str_it != destination_attribute->getIterators()->end(); str_it++) {
		int counter = p_variables_counter + 1;
		Parma_Polyhedra_Library::Linear_Expression exp;
		Parma_Polyhedra_Library::Variable * dependence_var = dependence->getDestinationIterator(**str_it);
		for (constraint_it = constraints->begin(); constraint_it != constraints->end(); constraint_it++) {
			Parma_Polyhedra_Library::Variable lambda = Parma_Polyhedra_Library::Variable(counter++);
			exp += ((*constraint_it).coefficient(*dependence_var) + (*constraint_it).inhomogeneous_term()) * lambda;
		}
		Parma_Polyhedra_Library::Variable * schedule_var = destination_var_map->getIteratorCoeficiant(**str_it);
		ROSE_ASSERT(schedule_var);
		p_polyhedron->add_constraint_and_minimize(-1 * *schedule_var == exp);
	}
	
	for (str_it = p_context->getGlobals()->begin(); str_it != p_context->getGlobals()->end(); str_it++) {
		int counter = p_variables_counter + 1;
		Parma_Polyhedra_Library::Linear_Expression exp;
		Parma_Polyhedra_Library::Variable * dependence_var = dependence->getGlobal(**str_it);
		for (constraint_it = constraints->begin(); constraint_it != constraints->end(); constraint_it++) {
			Parma_Polyhedra_Library::Variable lambda = Parma_Polyhedra_Library::Variable(counter++);
			exp += ((*constraint_it).coefficient(*dependence_var) + (*constraint_it).inhomogeneous_term()) * lambda;
		}
		Parma_Polyhedra_Library::Variable * schedule_source_var = source_var_map->getGlobalCoeficiant(**str_it);
		Parma_Polyhedra_Library::Variable * schedule_destination_var = destination_var_map->getGlobalCoeficiant(**str_it);
		ROSE_ASSERT(schedule_source_var || schedule_destination_var);
		p_polyhedron->add_constraint_and_minimize(*schedule_source_var - *schedule_destination_var == exp);
	}
	
	int counter = p_variables_counter;
	Parma_Polyhedra_Library::Linear_Expression lhs_exp =
		*(source_var_map->getConstantCoeficiant()) - *(destination_var_map->getConstantCoeficiant()) - 1;
	Parma_Polyhedra_Library::Variable lambda0 = Parma_Polyhedra_Library::Variable(counter++);
	Parma_Polyhedra_Library::Linear_Expression rhs_exp;
	rhs_exp += lambda0;
	for (constraint_it = constraints->begin(); constraint_it != constraints->end(); constraint_it++) {
		Parma_Polyhedra_Library::Variable lambda = Parma_Polyhedra_Library::Variable(counter++);
		rhs_exp += (*constraint_it).inhomogeneous_term() * lambda;
	}
	p_polyhedron->add_constraint_and_minimize(lhs_exp == rhs_exp);
	
	for (; p_variables_counter < counter; p_variables_counter++) {
		p_polyhedron->add_constraint_and_minimize( Parma_Polyhedra_Library::Variable(p_variables_counter) >= 0);
//		p_polyhedron->add_constraint_and_minimize( Parma_Polyhedra_Library::Variable(p_variables_counter) <= 10);
	}
}

int ValidScheduleSpacePPL::nextVariable() { return p_variables_counter++; }



void ValidScheduleSpacePPL::print(std::ostream & out) {
	out << "Dim: " << p_polyhedron->space_dimension() << std::endl;

	Parma_Polyhedra_Library::Constraint_System::const_iterator it_constraint;
	std::map<SgStatement *, ScheduleVariablesMappingPPL *>::iterator var_map_it;
	std::map<std::string, Parma_Polyhedra_Library::Variable> * var_map;
	std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator var_it;
	Parma_Polyhedra_Library::GMP_Integer coef;
	
	for (it_constraint = p_polyhedron->minimized_constraints().begin(); it_constraint != p_polyhedron->minimized_constraints().end(); it_constraint++) {
		int cnt = 0;
		for (var_map_it = p_map_statement_variables_mapping.begin(); var_map_it != p_map_statement_variables_mapping.end(); var_map_it++) {
			out << "[ ";
			var_map = &((*var_map_it).second->p_iterators_coeficiant);
			for (var_it = var_map->begin(); var_it != var_map->end(); var_it++) {
				coef = it_constraint->coefficient(var_it->second);
				if (coef == 1)
					out << "+ A" << var_it->first << " ";
				if (coef == -1)
					out << "- A" << var_it->first << " ";
				if (coef > 1)
					out << "+ " << coef << " * A" << var_it->first << " ";
				if (coef < -1)
					out << "- " << -coef << " * A" << var_it->first << " ";
				cnt++;
			}
			var_map = &((*var_map_it).second->p_globals_coeficiant);
			for (var_it = var_map->begin(); var_it != var_map->end(); var_it++) {
				coef = it_constraint->coefficient(var_it->second);
				if (coef == 1)
					out << "+ B" << var_it->first << " ";
				if (coef == -1)
					out << "- B" << var_it->first << " ";
				if (coef > 1)
					out << "+ " << coef << " * B" << var_it->first << " ";
				if (coef < -1)
					out << "- " << -coef << " * B" << var_it->first << " ";
				cnt++;
			}
			coef = it_constraint->coefficient(*((*var_map_it).second->getConstantCoeficiant()));
			if (coef == 1)
				out << "+ C ";
			if (coef == -1)
				out << "- C ";
			if (coef > 1)
				out << "+ " << coef << " * C ";
			if (coef < -1)
				out << "- " << -coef << " * C ";
			out << "] ";
			cnt++;
		}
		out << "[ ";
		for (int i = cnt; i < p_variables_counter; i++) {
			coef = it_constraint->coefficient(Parma_Polyhedra_Library::Variable(i));
			if (coef == 1)
				out << "+ l" << i << " ";
			if (coef == -1)
				out << "- l" << i << " ";
			if (coef > 1)
				out << "+ " << coef << " * l" << i << " ";
			if (coef < -1)
				out << "- " << -coef << " * l" << i << " ";
		}
		out << "] ";
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

/***/

}
