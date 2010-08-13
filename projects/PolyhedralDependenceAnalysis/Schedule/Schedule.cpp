
#include "ppl.hh"

#include "Schedule.hpp"
#include "ConvexHullInterior.hpp"

#include <utility>
#include <iomanip>

namespace PolyhedralSchedule {

/***/

ScheduleVariablesMappingPPL::ScheduleVariablesMappingPPL(
	FadaToPPL::PolyhedricContext * ctx,
	SgStatement * statement,
	ValidScheduleSpacePPL * schedule_space
):
	p_iterators_coeficiant(),
	p_globals_coeficiant()
{
	
	ROSE_ASSERT(statement);
	
	std::vector<std::string> * it_list = ctx->getDomain(statement)->getIteratorsList();
	
	std::vector<std::string>::iterator it;
	for (it = it_list->begin(); it != it_list->end(); it++) {
		p_iterators_coeficiant.insert(std::pair<std::string, Parma_Polyhedra_Library::Variable>(
			*it,
			Parma_Polyhedra_Library::Variable(schedule_space->nextVariable())
		));
	}
	
	for (it = ctx->getGlobals()->begin(); it != ctx->getGlobals()->end(); it++) {
		p_globals_coeficiant.insert(std::pair<std::string, Parma_Polyhedra_Library::Variable>(
			*it,
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
	FadaToPPL::PolyhedricContext * ctx,
	std::vector<FadaToPPL::PolyhedricDependence *> * dependences
):
	p_map_statement_variables_mapping(),
	p_context(ctx),
	p_variables_counter(0),
	p_lambdas_counter(0)
{
	
	std::vector<SgStatement *> * stmts = ctx->getStatements();
	std::vector<SgStatement *>::iterator stmt_it;
	for (stmt_it = stmts->begin(); stmt_it != stmts->end(); stmt_it++) {
		p_map_statement_variables_mapping.insert(std::pair<SgStatement *, ScheduleVariablesMappingPPL *>(
			*stmt_it,
			new ScheduleVariablesMappingPPL(p_context, *stmt_it, this)
		));
	}
	
//	std::vector<FadaToPPL::PolyhedricDependence *>::iterator dep_it;
//	for (dep_it = dependences->begin(); dep_it != dependences->end(); dep_it++)
//		p_lambdas_counter += (*dep_it)->getNbrConstraints() + 1;
	
	delete stmts;
	
	p_polyhedron = new Parma_Polyhedra_Library::C_Polyhedron(p_variables_counter/* + p_lambdas_counter*/);
	
	FillPolyhedron(dependences);
}

void ValidScheduleSpacePPL::FillPolyhedron(std::vector<FadaToPPL::PolyhedricDependence *> * dependences) {
	
	std::vector<FadaToPPL::PolyhedricDependence *>::iterator dep_it;
	for (dep_it = dependences->begin(); dep_it != dependences->end(); dep_it++) {
//		std::cerr << "." << std::endl;
		AddDependenceToPolyhedron(*dep_it);
	}
}

void ValidScheduleSpacePPL::AddDependenceToPolyhedron(FadaToPPL::PolyhedricDependence * dependence) {
	
	SgStatement * source = dependence->getSource();
	SgStatement * destination = dependence->getDestination();
	
	ROSE_ASSERT(source && destination);
	
	std::vector<std::string> * source_it_list = p_context->getDomain(source)->getIteratorsList();
	std::vector<std::string> * destination_it_list = p_context->getDomain(destination)->getIteratorsList();
		
	if (!source_it_list || !destination_it_list) {
		std::cerr << "Error in PolyhedralSchedule::ValidScheduleSpacePPL::AddDependenceToPolyhedron(...) wrong SgStatement non-mapped in PPL context !" << std::endl;
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
	
	/**/
	Parma_Polyhedra_Library::C_Polyhedron * polyhedron = 
		new Parma_Polyhedra_Library::C_Polyhedron(p_variables_counter + dependence->getNbrConstraints() + 1);
	/**/
	
	std::vector<std::string>::iterator str_it;
	Parma_Polyhedra_Library::Constraint_System::const_iterator constraint_it;
	
	for (str_it = source_it_list->begin(); str_it != source_it_list->end(); str_it++) {
		int counter = p_variables_counter + 1;
		Parma_Polyhedra_Library::Linear_Expression exp;
		Parma_Polyhedra_Library::Variable * dependence_var = dependence->getSourceIterator(*str_it);
		for (constraint_it = constraints->begin(); constraint_it != constraints->end(); constraint_it++) {
			Parma_Polyhedra_Library::Variable lambda = Parma_Polyhedra_Library::Variable(counter++);
			exp += (*constraint_it).coefficient(*dependence_var) * lambda;
		}
		Parma_Polyhedra_Library::Variable * schedule_var = source_var_map->getIteratorCoeficiant(*str_it);
		ROSE_ASSERT(schedule_var);
		/*p_*/polyhedron->add_constraint(*schedule_var == exp);
	}
	
	for (str_it = destination_it_list->begin(); str_it != destination_it_list->end(); str_it++) {
		int counter = p_variables_counter + 1;
		Parma_Polyhedra_Library::Linear_Expression exp;
		Parma_Polyhedra_Library::Variable * dependence_var = dependence->getDestinationIterator(*str_it);
		for (constraint_it = constraints->begin(); constraint_it != constraints->end(); constraint_it++) {
			Parma_Polyhedra_Library::Variable lambda = Parma_Polyhedra_Library::Variable(counter++);
			exp += (*constraint_it).coefficient(*dependence_var) * lambda;
		}
		Parma_Polyhedra_Library::Variable * schedule_var = destination_var_map->getIteratorCoeficiant(*str_it);
		ROSE_ASSERT(schedule_var);
		/*p_*/polyhedron->add_constraint(-1 * *schedule_var == exp);
	}
	for (str_it = p_context->getGlobals()->begin(); str_it != p_context->getGlobals()->end(); str_it++) {
		int counter = p_variables_counter + 1;
		Parma_Polyhedra_Library::Linear_Expression exp;
		Parma_Polyhedra_Library::Variable * dependence_var = dependence->getGlobal(*str_it);
		for (constraint_it = constraints->begin(); constraint_it != constraints->end(); constraint_it++) {
			Parma_Polyhedra_Library::Variable lambda = Parma_Polyhedra_Library::Variable(counter++);
			exp += (*constraint_it).coefficient(*dependence_var) * lambda;
		}
		Parma_Polyhedra_Library::Variable * schedule_source_var = source_var_map->getGlobalCoeficiant(*str_it);
		Parma_Polyhedra_Library::Variable * schedule_destination_var = destination_var_map->getGlobalCoeficiant(*str_it);
		ROSE_ASSERT(schedule_source_var || schedule_destination_var);
		/*p_*/polyhedron->add_constraint(*schedule_source_var - *schedule_destination_var == exp);
	}
	
	int counter = p_variables_counter;
	
	int delta = 1; // 'delta' is the latency associate to the execution of destination statement (Assume to be 1 in case of software code generation)
	
	Parma_Polyhedra_Library::Linear_Expression lhs_exp =
		*(source_var_map->getConstantCoeficiant()) - *(destination_var_map->getConstantCoeficiant()) - delta;
	Parma_Polyhedra_Library::Variable lambda0 = Parma_Polyhedra_Library::Variable(counter++);
	Parma_Polyhedra_Library::Linear_Expression rhs_exp;
	rhs_exp += lambda0;
	for (constraint_it = constraints->begin(); constraint_it != constraints->end(); constraint_it++) {
		Parma_Polyhedra_Library::Variable lambda = Parma_Polyhedra_Library::Variable(counter++);
		rhs_exp += (*constraint_it).inhomogeneous_term() * lambda;
	}
	/*p_*/polyhedron->add_constraint(lhs_exp == rhs_exp);
	
	/*for (; p_variables_counter < counter; p_variables_counter++) {
		p_polyhedron->add_constraint( Parma_Polyhedra_Library::Variable(p_variables_counter) >= 0);
	}*/
	for (int i = p_variables_counter; i < counter; i++) {
		polyhedron->add_constraint( Parma_Polyhedra_Library::Variable(i) >= 0);
	}
	
	/**/
	polyhedron->remove_higher_space_dimensions(p_variables_counter);
	p_polyhedron->intersection_assign_and_minimize(*polyhedron);
	delete polyhedron;
	/**/
}

std::vector<ProgramSchedule *> * ValidScheduleSpacePPL::generateAllValidSchedules() {
	std::map<SgStatement *, ScheduleVariablesMappingPPL *>::iterator var_map_it;
	std::map<std::string, Parma_Polyhedra_Library::Variable> * var_map;
	std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator var_it;

	std::vector<ProgramSchedule *> * all_valid_schedule = new std::vector<ProgramSchedule *>();
	std::vector<ProgramSchedule *>::iterator valid_schedule_it;
	ProgramSchedule * program_schedule;
	StatementSchedule * statement_schedule;
	int cnt;
		
	std::cout << "Dim: " << p_polyhedron->space_dimension() << std::endl;
	
	if (true || p_polyhedron->is_bounded()) {
	
//		std::cerr << "\t." << std::endl;
	
		std::vector<int * > * all_points = PolyhedralToolsPPL::allPointsInPolyhedron(p_polyhedron);
		std::vector<int * >::iterator it;
	
//		std::cerr << "\t.." << std::endl;
		
		for (it = all_points->begin(); it != all_points->end(); it++) {
		
//			std::cerr << "\t\t." << std::endl;

			/*if ((*it)[p_polyhedron->space_dimension()] != 1) {
				std::cerr << ".";
				continue;
			}*/
			
			program_schedule = new ProgramSchedule();
			for (var_map_it = p_map_statement_variables_mapping.begin(); var_map_it != p_map_statement_variables_mapping.end(); var_map_it++) {
			
//				std::cerr << "\t\t\t." << std::endl;
				
				statement_schedule = program_schedule->addStatement((*var_map_it).first);
				
//				std::cerr << "\t\t\t.." << std::endl;
				
				var_map = &((*var_map_it).second->p_iterators_coeficiant);
				for (var_it = var_map->begin(); var_it != var_map->end(); var_it++) {
					statement_schedule->setIteratorCoef((*it)[var_it->second.id()]);
				}
				
//				std::cerr << "\t\t\t..." << std::endl;

				var_map = &((*var_map_it).second->p_globals_coeficiant);
				for (var_it = var_map->begin(); var_it != var_map->end(); var_it++) {
					statement_schedule->setGlobalCoef((*it)[var_it->second.id()]);
				}
				
//				std::cerr << "\t\t\t...." << std::endl;
				
				statement_schedule->setConstant((*it)[var_map_it->second->getConstantCoeficiant()->id()]);
				
//				std::cerr << "\t\t\t....." << std::endl;
			}
			
//			std::cerr << "\t\t.." << std::endl;
			
			for (valid_schedule_it = all_valid_schedule->begin(); valid_schedule_it != all_valid_schedule->end(); valid_schedule_it++)
				if (**valid_schedule_it == *program_schedule)
					break;
			
//			std::cerr << "\t\t..." << std::endl;
			
			if (valid_schedule_it == all_valid_schedule->end()) {
				all_valid_schedule->push_back(program_schedule);
			}
			else {
				delete program_schedule;
			}
			
//			std::cerr << "\t\t...." << std::endl;
			
		}
//		std::cerr << "\t..." << std::endl;
	}
	else { 
		std::cerr << "Error in PolyhedralSchedule::ValidScheduleSpacePPL::generateAllValidSchedules(...): polyhedron isn't bounded !" << std::endl;
		ROSE_ASSERT(false);
	}
	
	return 	all_valid_schedule;	
}


int ValidScheduleSpacePPL::nextVariable() { return p_variables_counter++; }

void ValidScheduleSpacePPL::bounding(int min_param_bound, int max_param_bound, int lambda_bound) {
	int i;
	/*for (i = 0; i < p_variables_counter - p_lambdas_counter; i++) {
		p_polyhedron->add_constraint( Parma_Polyhedra_Library::Variable(i) >= min_param_bound);
		p_polyhedron->add_constraint( Parma_Polyhedra_Library::Variable(i) <= max_param_bound);
	}
	for (; i < p_variables_counter; i++) {
		p_polyhedron->add_constraint( Parma_Polyhedra_Library::Variable(i) <= lambda_bound);
	}*/
	for (i = 0; i < p_variables_counter; i++) {
		p_polyhedron->add_constraint( Parma_Polyhedra_Library::Variable(i) >= min_param_bound);
		p_polyhedron->add_constraint( Parma_Polyhedra_Library::Variable(i) <= max_param_bound);
	}
}

void ValidScheduleSpacePPL::playWithGenerator() {
	Parma_Polyhedra_Library::Generator_System generators = p_polyhedron->minimized_generators();
	std::map<SgStatement *, ScheduleVariablesMappingPPL *>::iterator var_map_it;
	std::map<std::string, Parma_Polyhedra_Library::Variable> * var_map;
	std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator var_it;
	Parma_Polyhedra_Library::Generator_System::const_iterator it_generator;
	int statement_cnt = 0;
	

	if (p_polyhedron->is_bounded()) {
		
		for (var_map_it = p_map_statement_variables_mapping.begin(); var_map_it != p_map_statement_variables_mapping.end(); var_map_it++) {
			var_map = &((*var_map_it).second->p_iterators_coeficiant);
			for (var_it = var_map->begin(); var_it != var_map->end(); var_it++) {
				std::cout << "| A_" << statement_cnt << "_" << var_it->first << " ";
			}
			var_map = &((*var_map_it).second->p_globals_coeficiant);
			for (var_it = var_map->begin(); var_it != var_map->end(); var_it++) {
				std::cout << "| B_" << statement_cnt << "_" << var_it->first << " ";
			}
			std::cout << "|  K_" << statement_cnt << "  ";
			statement_cnt++;
		}
		std::cout << "|" << std::endl;
		for (var_map_it = p_map_statement_variables_mapping.begin(); var_map_it != p_map_statement_variables_mapping.end(); var_map_it++) {
			var_map = &((*var_map_it).second->p_iterators_coeficiant);
			for (var_it = var_map->begin(); var_it != var_map->end(); var_it++) std::cout << "|-------";
			var_map = &((*var_map_it).second->p_globals_coeficiant);
			for (var_it = var_map->begin(); var_it != var_map->end(); var_it++) std::cout << "|-------";
			std::cout << "|-------";
		}
		std::cout << "|" << std::endl;
		for (it_generator = generators.begin(); it_generator != generators.end(); it_generator++) {
			for (var_map_it = p_map_statement_variables_mapping.begin(); var_map_it != p_map_statement_variables_mapping.end(); var_map_it++) {
				var_map = &((*var_map_it).second->p_iterators_coeficiant);
				for (var_it = var_map->begin(); var_it != var_map->end(); var_it++) {
					std::cout << "| " << std::setw(5) << it_generator->coefficient(var_it->second) << " ";
				}
				var_map = &((*var_map_it).second->p_globals_coeficiant);
				for (var_it = var_map->begin(); var_it != var_map->end(); var_it++) {
					std::cout << "| " << std::setw(5) << it_generator->coefficient(var_it->second) << " ";
				}
				std::cout << "| " << std::setw(5) << it_generator->coefficient(*((*var_map_it).second->getConstantCoeficiant())) << " ";
			}
			std::cout << "|" << std::endl;
		}
	}
	else { 
		std::cerr << "Error in PolyhedralSchedule::ValidScheduleSpacePPL::playWithGenerator(...): polyhedron isn't bounded !" << std::endl;
		int cnt = 1;
		std::cout << "| #### | l | p | r | c |" << std::endl;
		std::cout << "----------------------" << std::endl;
		for (it_generator = generators.begin(); it_generator != generators.end(); it_generator++) {
			std::cout << "| " << std::setw(4) << cnt++ << " | ";
			std::cout << (*it_generator).is_line() << " | ";
			std::cout << (*it_generator).is_point() << " | ";
			std::cout << (*it_generator).is_ray()  << " | ";
			std::cout << (*it_generator).is_closure_point()  << " | ";
			std::cout << std::endl;
		}
	}
	std::cout << std::endl;
	
}

void ValidScheduleSpacePPL::print(std::ostream & out, bool not_minimized) {
	out << "Dim: " << p_polyhedron->space_dimension() << std::endl;

	Parma_Polyhedra_Library::Constraint_System constraints = not_minimized ? p_polyhedron->constraints() : p_polyhedron->minimized_constraints();

	Parma_Polyhedra_Library::Constraint_System::const_iterator it_constraint;
	std::map<SgStatement *, ScheduleVariablesMappingPPL *>::iterator var_map_it;
	std::map<std::string, Parma_Polyhedra_Library::Variable> * var_map;
	std::map<std::string, Parma_Polyhedra_Library::Variable>::iterator var_it;
	Parma_Polyhedra_Library::GMP_Integer coef;
	int line = 1;
	
	for (it_constraint = constraints.begin(); it_constraint != constraints.end(); it_constraint++) {
		out << line++ << ":  ";
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
	
	out << "Has integer points inside: " << p_polyhedron->contains_integer_point() << std::endl;
	out << "Is bounded: " << p_polyhedron->is_bounded() << std::endl;
}

std::vector<int> p_iterators_coeficiants;
std::vector<int> p_globals_coeficiants;
int p_constant;
bool operator == (StatementSchedule & s1, StatementSchedule & s2) {
	if (s1.p_iterators_coeficiants.size() != s2.p_iterators_coeficiants.size())
		return false;
	if (s1.p_globals_coeficiants.size() != s2.p_globals_coeficiants.size())
		return false;
	std::vector<int>::iterator it1 = s1.p_iterators_coeficiants.begin();
	std::vector<int>::iterator it2 = s2.p_iterators_coeficiants.begin();
	while (it1 != s1.p_iterators_coeficiants.end() && it2 != s2.p_iterators_coeficiants.end()) {
		if (*it1 != *it2) return false;
		it1++;
		it2++;
	}
	it1 = s1.p_globals_coeficiants.begin();
	it2 = s2.p_globals_coeficiants.begin();
	while (it1 != s1.p_globals_coeficiants.end() && it2 != s2.p_globals_coeficiants.end()) {
		if (*it1 != *it2) return false;
		it1++;
		it2++;
	}
	if (s1.p_constant != s2.p_constant) return false;
	return true;
}

bool operator == (ProgramSchedule & s1, ProgramSchedule & s2) {
	if (s1.p_statement_schedules.size() != s2.p_statement_schedules.size())
		return false;
	std::map<SgStatement *, StatementSchedule *>::iterator it1, it2;
	for (it1 = s1.p_statement_schedules.begin(); it1 != s1.p_statement_schedules.end(); it1++) {
		it2 = s2.p_statement_schedules.find(it1->first);
		if (it2 != s2.p_statement_schedules.end() && *it1->second == *it2->second)
			continue;
		return false;
	}
	return true;
}

/***/

}
