/**
 * \file src/scoplib/Cloog.tpp
 * \brief Implementation of Cloog interface.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

template <class Function, class Expression, class VariableLBL>
size_t generateCloogProgramFromSchedule(
	const Scheduling::Schedule<Function, Expression, VariableLBL> & schedule,
	CloogState * state,
	CloogOptions * options,
	CloogProgram * program
) {
	const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program = schedule.getPolyhedralProgram();
	
	program->nb_scattdims = schedule.getDimension();
	
	size_t nb_parameters = polyhedral_program.getNumberOfGlobals();
	size_t cnt = 0;
	
	program->names = cloog_names_alloc();
	
	program->names->nb_scalars = 0;
	program->names->scalars = NULL;
	
	program->names->nb_scattering = 0;
	program->names->scattering = NULL;
	
	const std::vector<VariableLBL> & it_list = polyhedral_program.getIterators();
	program->names->nb_iterators = it_list.size();
	program->names->iterators = new char*[it_list.size()];
	for (typename std::vector<VariableLBL>::const_iterator it = it_list.begin(); it != it_list.end(); it++) {
		program->names->iterators[cnt] = varToCstr(*it);
		cnt++;
	}
	
	program->names->nb_parameters = polyhedral_program.getNumberOfGlobals();
	program->names->parameters = new char*[nb_parameters];
	for (size_t i = 0; i < nb_parameters; i++)
		program->names->parameters[i] = varToCstr(polyhedral_program.getGlobalById(i));
	
	CloogMatrix * ctx = cloog_matrix_alloc(0, nb_parameters + 2);
	program->context = cloog_domain_from_cloog_matrix(state, ctx, nb_parameters);

	size_t nb_statement = polyhedral_program.getNumberOfStatement();
	
	CloogLoop ** loop = &(program->loop);
	CloogBlockList ** block_list = &(program->blocklist);
	for (size_t i = 1; i <= nb_statement; i++) {
		*loop = cloog_loop_malloc(state);
		*block_list = cloog_block_list_malloc();
		(*block_list)->block = cloog_block_malloc(state);
		(*loop)->block = (*block_list)->block;
		
//		cloog_int_set_si((*loop)->stride, 1);
//		cloog_int_set_si((*loop)->offset, 0);
		
		(*loop)->block->statement = cloog_statement_malloc(state);
		(*loop)->block->statement->number = i;
		(*loop)->block->statement->usr = polyhedral_program.getStatementById(i);
		(*loop)->block->statement->next = NULL;
		
		(*loop)->block->nb_scaldims = 0;
		(*loop)->block->scaldims = NULL;
		
		PolyhedricAnnotation::Domain<Function, Expression, VariableLBL> domain =
			PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(polyhedral_program.getStatementById(i));
		(*loop)->block->depth = domain.getNumberOfIterators();
		
		const ConstraintSystem & cs = domain.getDomainPolyhedron().minimized_constraints();
		size_t cs_size = 0;
		for (ConstraintSystem::const_iterator it = cs.begin(); it != cs.end(); it++)
			cs_size++;

		CloogMatrix * matrix = cloog_matrix_alloc(cs_size, (*loop)->block->depth + nb_parameters + 2);
		
		cnt = 0;
		for (ConstraintSystem::const_iterator it = cs.begin(); it != cs.end(); it++) {
			cloog_int_set_si(matrix->p[cnt][0], it->is_equality() ? 0 : 1);
			for (size_t j = 0; j < it->space_dimension(); j++)
				cloog_int_set_si(matrix->p[cnt][j+1], it->coefficient(VariableID(j)).get_si());
			cloog_int_set_si(matrix->p[cnt][(*loop)->block->depth + nb_parameters + 1], it->inhomogeneous_term().get_si());
			cnt++;
		}
		
		(*loop)->domain = cloog_domain_from_cloog_matrix(state, matrix, nb_parameters);
		
		(*loop)->inner = NULL;
		
		loop = &((*loop)->next);
		block_list = &((*block_list)->next);
	}
	*loop = NULL;
	*block_list = NULL;
	
	size_t max_scat_size = 0;
	
	std::vector<std::vector<LinearExpression_ppl> > stmt_scat(nb_statement);
	
	for (size_t i = 1; i <= nb_statement; i++) {
		stmt_scat[i-1] = PolyhedricAnnotation::getScattering<Function, Expression, VariableLBL>(polyhedral_program.getStatementById(i)).getScattering();
		size_t scat_size = stmt_scat[i-1].size();
		if (scat_size > max_scat_size) max_scat_size = scat_size;
	}
	for (size_t i = 0; i < nb_statement; i++)
		stmt_scat[i].resize(max_scat_size);
	
	CloogScatteringList * scat_list;
	CloogScatteringList ** scat_it = &scat_list;
	for (size_t i = 1; i <= nb_statement; i++) {
		*scat_it = new CloogScatteringList();
		
		size_t nb_it = PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(polyhedral_program.getStatementById(i)).getNumberOfIterators();
		
		CloogMatrix * matrix = cloog_matrix_alloc(
			max_scat_size + schedule.getDimension(),
			max_scat_size + schedule.getDimension() + nb_it + nb_parameters + 2
		);
		cnt = 0;
		const std::vector<LinearExpression_ppl> & sched_list = schedule.getSchedule(i);
		for (std::vector<LinearExpression_ppl>::const_iterator it = sched_list.begin(); it != sched_list.end(); it++) {
			cloog_int_set_si(matrix->p[cnt][0], 0);
			for (size_t j = 0; j < schedule.getDimension(); j++)
				cloog_int_set_si(matrix->p[cnt][1 + j], (j == cnt) ? 1 : 0);
			for (size_t j = 0; j < max_scat_size; j++)
				cloog_int_set_si(matrix->p[cnt][1 + schedule.getDimension() + j], 0);
			for (size_t j = 0; j < nb_it + nb_parameters; j++)
				cloog_int_set_si(matrix->p[cnt][1 + schedule.getDimension() + max_scat_size + j], -it->coefficient(VariableID(j)).get_si());
			cloog_int_set_si(matrix->p[cnt][max_scat_size + schedule.getDimension() + nb_it + nb_parameters + 1], -it->inhomogeneous_term().get_si());
			cnt++;
		}
		for (std::vector<LinearExpression_ppl>::const_iterator it = stmt_scat[i-1].begin(); it != stmt_scat[i-1].end(); it ++) {
			cloog_int_set_si(matrix->p[cnt][0], 0);
			for (size_t j = 0; j < schedule.getDimension(); j++)
				cloog_int_set_si(matrix->p[cnt][1 + j], 0);
			for (size_t j = 0; j < max_scat_size; j++)
				cloog_int_set_si(matrix->p[cnt][1 + schedule.getDimension() + j], (schedule.getDimension() + j == cnt) ? 1 : 0);
			for (size_t j = 0; j < nb_it + nb_parameters; j++)
				cloog_int_set_si(matrix->p[cnt][1 + schedule.getDimension() + max_scat_size + j], -it->coefficient(VariableID(j)).get_si());
			cloog_int_set_si(matrix->p[cnt][max_scat_size + schedule.getDimension() + nb_it + nb_parameters + 1], -it->inhomogeneous_term().get_si());
			cnt++;
		}
		
		(*scat_it)->scatt = cloog_scattering_from_cloog_matrix(state, matrix, max_scat_size + schedule.getDimension(), nb_parameters);
		scat_it = &((*scat_it)->next);
	}
	(*scat_it) = NULL;
	
	program->names->nb_scattering = max_scat_size + schedule.getDimension();
	program->names->scattering = new char*[max_scat_size + schedule.getDimension()];
	for (size_t i = 0; i < max_scat_size + schedule.getDimension(); i++) {
		std::ostringstream oss;
		oss << 'c' << i;
		program->names->scattering[i] = new char [oss.str().size()+1];
		std::strcpy(program->names->scattering[i], oss.str().c_str());
	}

	cloog_program_scatter(program, scat_list, options);
	
	program->scaldims = new int[max_scat_size + schedule.getDimension()];
	for (size_t i = 0; i < max_scat_size + schedule.getDimension(); i++)
		program->scaldims[i] = 0 ;

	program = cloog_program_generate(program,options);

//	cloog_program_pprint(stdout,program,options);
	
	return max_scat_size + schedule.getDimension();
}

