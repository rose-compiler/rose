/**
 * \file src/scoplib/ScopLib.tpp
 * \brief Implementation of ScopLib interface.
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

using namespace PolyhedricAnnotation;

template <class Function, class Expression, class VariableLBL>
scoplib_scop_p PolyhedricAnnotationToScopLib(const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program) {
	scoplib_scop_p res = scoplib_scop_malloc();
	
	res->usr = polyhedral_program.getFunction();
	
	res->nb_parameters = polyhedral_program.getNumberOfGlobals();
	
	res->parameters = new char *[res->nb_parameters];
	
	for (size_t i = 0; i < res->nb_parameters; i++)
		res->parameters[i] = varToCstr(polyhedral_program.getGlobalById(i));
	
	res->context = scoplib_matrix_malloc(0, res->nb_parameters+2);
	
	res->nb_arrays = polyhedral_program.getNumberOfVariables();
	
	res->arrays = new char *[res->nb_arrays];
	
	for (size_t i = 0; i < res->nb_arrays; i++)
		res->arrays[i] = varToCstr(polyhedral_program.getVariableByID(i+1));
	
	scoplib_statement_p * statement = &(res->statement);
	
	
	for (int i = 1; i <= polyhedral_program.getNumberOfStatement(); i++) {
		*statement = scoplib_statement_malloc();
		
		std::ostringstream oss_body;
		oss_body << *(polyhedral_program.getStatementById(i));
		(*statement)->body = new char [oss_body.str().size()+1];
		strcpy ((*statement)->body, oss_body.str().c_str());
	
		try {
			Domain<Function, Expression, VariableLBL> & domain = getDomain<Function, Expression, VariableLBL>(polyhedral_program.getStatementById(i));
			
			(*statement)->nb_iterators = domain.getNumberOfIterators();
			
			(*statement)->domain = scoplib_matrix_list_malloc();
			
			const ConstraintSystem & cs = domain.getDomainPolyhedron().constraints();
			ConstraintSystem::const_iterator it;
			size_t cs_size = 0;
			for (it = cs.begin(); it != cs.end(); it++) cs_size++;
			
			(*statement)->domain->elt = scoplib_matrix_malloc(cs_size, (*statement)->nb_iterators + res->nb_parameters + 2);
			
			size_t row = 0;
			for (it = cs.begin(); it != cs.end(); it++) {
				if ((*it).is_equality())
					SCOPVAL_set_si((*statement)->domain->elt->p[row][0], 0);
				else
					SCOPVAL_set_si((*statement)->domain->elt->p[row][0], 1);
				for (int j = 1; j <= (*statement)->nb_iterators; j++)
					if (j <= (*it).space_dimension())
						SCOPVAL_set_si((*statement)->domain->elt->p[row][j], (*it).coefficient(VariableID(j - 1)).get_si());
					else
						SCOPVAL_set_si((*statement)->domain->elt->p[row][j], 0);
				for (int j = (*statement)->nb_iterators + 1; j <= (*statement)->nb_iterators + res->nb_parameters; j++)
					if (j <= (*it).space_dimension())
						SCOPVAL_set_si((*statement)->domain->elt->p[row][j], (*it).coefficient(VariableID(j - 1)).get_si());
					else
						SCOPVAL_set_si((*statement)->domain->elt->p[row][j], 0);
				SCOPVAL_set_si((*statement)->domain->elt->p[row][(*statement)->nb_iterators + res->nb_parameters + 1], (*it).inhomogeneous_term().get_si());
				
				row++;
			}
		}
		catch (Exception::ExceptionFrontend & e) {
			std::cerr << "Error ! Domain of " << *(polyhedral_program.getStatementById(i)) << " is missing." << std::endl;
			scoplib_matrix_list_free((*statement)->domain);
			(*statement)->domain = scoplib_matrix_list_malloc();
			(*statement)->domain->elt = scoplib_matrix_malloc(0, (*statement)->nb_iterators + res->nb_parameters + 2);
		}
		
		try {
			Scattering<Function, Expression, VariableLBL> & scattering = getScattering<Function, Expression, VariableLBL>(polyhedral_program.getStatementById(i));
			
			const std::vector<LinearExpression> & le_l = scattering.getScattering();
			std::vector<LinearExpression>::const_iterator it;
			
			(*statement)->schedule = scoplib_matrix_malloc(le_l.size(), (*statement)->nb_iterators + res->nb_parameters + 2);

			size_t it_cnt = 0;
			size_t row = 0;
			for (it = le_l.begin(); it != le_l.end(); it++) {
				SCOPVAL_set_si((*statement)->schedule->p[row][0], 0);
				for (int j = 1; j <= (*statement)->nb_iterators; j++)
					SCOPVAL_set_si((*statement)->schedule->p[row][j], 0);
				if (!((*it).all_homogeneous_terms_are_zero())) {
					if ((*it).coefficient(VariableID(it_cnt)) < 0)
						throw Exception::OutOfScopLibScope("ScopLib does not accept negative increments.");
					SCOPVAL_set_si((*statement)->schedule->p[row][it_cnt+1], (*it).coefficient(VariableID(it_cnt)).get_si());
					it_cnt++;
				}
				for (int j = (*statement)->nb_iterators + 1; j <= (*statement)->nb_iterators + res->nb_parameters; j++)
					SCOPVAL_set_si((*statement)->schedule->p[row][j], 0);
					
				if ((*it).all_homogeneous_terms_are_zero())
					SCOPVAL_set_si((*statement)->schedule->p[row][(*statement)->nb_iterators + res->nb_parameters + 1], (*it).inhomogeneous_term().get_si());
				else
					SCOPVAL_set_si((*statement)->schedule->p[row][(*statement)->nb_iterators + res->nb_parameters + 1], 0);
				row++;
			}
		}
		catch (Exception::ExceptionFrontend & e) {
			std::cerr << "Error ! Scattering of " << polyhedral_program.getStatementById(i) << " is missing." << std::endl;
			scoplib_matrix_free((*statement)->schedule);
			(*statement)->schedule = scoplib_matrix_malloc(0, (*statement)->nb_iterators + res->nb_parameters + 2);
		}
		
		try {
			DataAccess<Function, Expression, VariableLBL> & data_access = getDataAccess<Function, Expression, VariableLBL>(polyhedral_program.getStatementById(i));

	
			size_t row = 0;
			size_t nbr_line = 0;
			std::vector<std::pair<size_t, std::vector<LinearExpression> > >::const_iterator it1;
			std::vector<LinearExpression>::const_iterator it2;
			for (it1 = data_access.getRead().begin(); it1 != data_access.getRead().end(); it1++)
				nbr_line += (*it1).second.size();
		
			(*statement)->read = scoplib_matrix_malloc(nbr_line, (*statement)->nb_iterators + res->nb_parameters + 2);
			for (it1 = data_access.getRead().begin(); it1 != data_access.getRead().end(); it1++) {
				for (it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++) {
					if (it2 == (*it1).second.begin())
						SCOPVAL_set_si((*statement)->read->p[row][0], (*it1).first);
					else
						SCOPVAL_set_si((*statement)->read->p[row][0], 0);
					for (int j = 1; j <= (*statement)->nb_iterators; j++)
						if (j <= (*it2).space_dimension())
							SCOPVAL_set_si((*statement)->read->p[row][j], (*it2).coefficient(VariableID(j - 1)).get_si());
						else
							SCOPVAL_set_si((*statement)->read->p[row][j], 0);
					for (int j = (*statement)->nb_iterators + 1; j <= (*statement)->nb_iterators + res->nb_parameters; j++)
						if (j <= (*it2).space_dimension())
							SCOPVAL_set_si((*statement)->read->p[row][j], (*it2).coefficient(VariableID(j - 1)).get_si());
						else
							SCOPVAL_set_si((*statement)->read->p[row][j], 0);
					SCOPVAL_set_si((*statement)->read->p[row][(*statement)->nb_iterators + res->nb_parameters + 1], (*it2).inhomogeneous_term().get_si());
					row++;
				}
			}
			
			row = 0;
			nbr_line = 0;
			for (it1 = data_access.getWrite().begin(); it1 != data_access.getWrite().end(); it1++)
				nbr_line += (*it1).second.size();
				
			(*statement)->write = scoplib_matrix_malloc(nbr_line, (*statement)->nb_iterators + res->nb_parameters + 2);
			
			for (it1 = data_access.getWrite().begin(); it1 != data_access.getWrite().end(); it1++) {
				for (it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++) {
					if (it2 == (*it1).second.begin())
						SCOPVAL_set_si((*statement)->write->p[row][0], (*it1).first);
					else
						SCOPVAL_set_si((*statement)->write->p[row][0], 0);
					for (int j = 1; j <= (*statement)->nb_iterators; j++)
						if (j <= (*it2).space_dimension())
							SCOPVAL_set_si((*statement)->write->p[row][j], (*it2).coefficient(VariableID(j - 1)).get_si());
						else
							SCOPVAL_set_si((*statement)->write->p[row][j], 0);
					for (int j = (*statement)->nb_iterators + 1; j <= (*statement)->nb_iterators + res->nb_parameters; j++)
						if (j <= (*it2).space_dimension())
							SCOPVAL_set_si((*statement)->write->p[row][j], (*it2).coefficient(VariableID(j - 1)).get_si());
						else
							SCOPVAL_set_si((*statement)->write->p[row][j], 0);
					SCOPVAL_set_si((*statement)->write->p[row][(*statement)->nb_iterators + res->nb_parameters + 1], (*it2).inhomogeneous_term().get_si());
					row++;
				}
			}
		}
		catch (Exception::ExceptionFrontend & e) {
			std::cerr << "Error ! Access of " << polyhedral_program.getStatementById(i) << " is missing." << std::endl;
			scoplib_matrix_free((*statement)->read);
			(*statement)->read = scoplib_matrix_malloc(0, (*statement)->nb_iterators + res->nb_parameters + 2);
			scoplib_matrix_free((*statement)->write);
			(*statement)->write = scoplib_matrix_malloc(0, (*statement)->nb_iterators + res->nb_parameters + 2);
		}
		
		(*statement)->iterators = new char *[(*statement)->nb_iterators];
		for (size_t j = 0; j < (*statement)->nb_iterators; j++)
			(*statement)->iterators[j] = varToCstr(polyhedral_program.getIteratorById(polyhedral_program.getStatementById(i), j));
		
		statement = &((*statement)->next);
	}
	*statement = NULL;
	
	return res;
}
  
  
  
  
  
