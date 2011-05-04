/**
 * \file src/scoplib/Candl.tpp
 * \brief Implementation of Candl interface.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

template <class Function, class Expression, class VariableLBL>
std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *> * ComputeDependenciesWithCandl(
	const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program
) {
	std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *> * res =
		new std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *>();
	
	scoplib_scop_p scop = PolyhedricAnnotationToScopLib<Function, Expression, VariableLBL>(polyhedral_program);
	
	scoplib_scop_print(stdout, scop);
	
	candl_program_p prog = candl_program_convert_scop(scop, NULL);
	
	candl_program_print(stdout, prog);
	
	candl_dependence_p deps = candl_dependence(prog, candl_options_malloc());
	
	while (deps != NULL) {
		size_t from = deps->target->label + 1;
		size_t to = deps->source->label + 1;
		size_t from_pos = 0;
		size_t to_pos = 0;
		
		PolyhedricDependency::DependencyType type;
		size_t variable;
		
		switch (deps->type) {
			case CANDL_RAR:
				type = PolyhedricDependency::RaR;
				variable = CANDL_get_si(deps->source->read->p[deps->ref_source][0]);
				for (size_t i = 0; i < deps->ref_target; i++)
					if (CANDL_get_si(deps->target->read->p[i][0]) != 0)
						from_pos++;
				for (size_t i = 0; i < deps->ref_source; i++)
					if (CANDL_get_si(deps->source->read->p[i][0]) != 0)
						to_pos++;
				break;
			case CANDL_RAW:
				type = PolyhedricDependency::RaW;
				variable = CANDL_get_si(deps->source->written->p[deps->ref_source][0]);
				for (size_t i = 0; i < deps->ref_target; i++)
					if (CANDL_get_si(deps->target->read->p[i][0]) != 0)
						from_pos++;
				for (size_t i = 0; i < deps->ref_source; i++)
					if (CANDL_get_si(deps->source->written->p[i][0]) != 0)
						to_pos++;
				break;
			case CANDL_WAR:
				type = PolyhedricDependency::WaR;
				variable = CANDL_get_si(deps->source->read->p[deps->ref_source][0]);
				for (size_t i = 0; i < deps->ref_target; i++)
					if (CANDL_get_si(deps->target->written->p[i][0]) != 0)
						from_pos++;
				for (size_t i = 0; i < deps->ref_source; i++)
					if (CANDL_get_si(deps->source->read->p[i][0]) != 0)
						to_pos++;
				break;
			case CANDL_WAW:
				type = PolyhedricDependency::WaW;
				variable = CANDL_get_si(deps->source->written->p[deps->ref_source][0]);
				for (size_t i = 0; i < deps->ref_target; i++)
					if (CANDL_get_si(deps->target->written->p[i][0]) != 0)
						from_pos++;
				for (size_t i = 0; i < deps->ref_source; i++)
					if (CANDL_get_si(deps->source->written->p[i][0]) != 0)
						to_pos++;
				break;
			case CANDL_UNSET:
			default:
				throw Exception::WrongDependencyType();
		}
		
		Polyhedron dependency(deps->domain->NbColumns - 2);
		
		for (size_t i = 0; i < deps->domain->NbRows; i++) {
			LinearExpression le;
			for (size_t j = 1; j < deps->domain->NbColumns-1; j++) {
				le += Integer((long int)deps->domain->p[i][j]) * VariableID(j-1);
			}
			le += Integer((long int)deps->domain->p[i][deps->domain->NbColumns-1]);
			if (CANDL_get_si(deps->domain->p[i][0]) == 0)
				dependency.refine_with_constraint(le == 0);
			else if (CANDL_get_si(deps->domain->p[i][0]) == 1)
				dependency.refine_with_constraint(le >= 0);
			else
				throw Exception::ExceptionScopLib("First column of a matrix need to be 0/1 for eq/ineq selection.");
		}
		
		res->push_back(new PolyhedricDependency::Dependency<Function, Expression, VariableLBL>(
			polyhedral_program,
			from,
			to,
			from_pos,
			to_pos,
			type,
			variable,
			dependency
		));
		
		deps = deps->next;
	}
	
	return res;
}



