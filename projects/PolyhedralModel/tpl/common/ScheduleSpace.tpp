/**
 * \file src/common/ScheduleSpace.tpp
 * \brief Implementation of Schedule Space (template).
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

template <class Function, class Expression, class VariableLBL>
void ScheduleSpace<Function, Expression, VariableLBL>::generateSchedule(
	std::vector<size_t> * selection,
	std::vector<std::vector<int*> *> & raw_sched,
	std::vector<Schedule<Function, Expression, VariableLBL> > & res) {
	size_t depth = selection == NULL ? 0 : selection->size();
	if (depth < p_schedule_spaces.size()) {
		for (size_t i = 0; i < raw_sched[depth]->size(); i++) {
			std::vector<size_t> * new_selection = (selection == NULL) ? new std::vector<size_t>() : new std::vector<size_t>(*selection);
			new_selection->push_back(i);
			generateSchedule(new_selection, raw_sched, res);
			delete new_selection;
		}
	}
	else if (depth != 0) {
		Schedule<Function, Expression, VariableLBL> sched(p_polyhedral_program, depth);
		for (size_t i = 0; i < depth; i++) {
			int * selected_sched = (*raw_sched[i])[(*selection)[i]];
			for (size_t j = 0; j < p_dim_space; j++) {
				if (p_coef_info[j].type != Constant)
					sched.setSchedule(p_coef_info[j].statement)[i] += selected_sched[j] * VariableID(p_coef_info[j].local_coef);
				else
					sched.setSchedule(p_coef_info[j].statement)[i] += selected_sched[j];
			}
		}
		res.push_back(sched);
	}
}

template <class Function, class Expression, class VariableLBL>
void ScheduleSpace<Function, Expression, VariableLBL>::computePBCC() {
	size_t nb_statements = p_polyhedral_program.getNumberOfStatement();
	size_t nb_globals = p_polyhedral_program.getNumberOfGlobals();
	
	std::vector<size_t> nb_iterators(nb_statements);
	std::vector<size_t> first_coef(nb_statements);
	p_dim_space = (nb_globals + 1) * nb_statements;
	
	Expression * stmt = p_polyhedral_program.getStatementById(1);
	nb_iterators[0] = PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(stmt).getNumberOfIterators();
	p_dim_space += nb_iterators[0];
	first_coef[0] = 0;
	for (size_t j = 0; j < nb_iterators[0]; j++)
		p_coef_info.push_back(CoefInfo(j, 1, Iterator, j, j));
	for (size_t j = 0; j < nb_globals; j++)
		p_coef_info.push_back(CoefInfo(nb_iterators[0] + j, 1, Global, j, nb_iterators[0] + j));
	p_coef_info.push_back(CoefInfo(nb_iterators[0] + nb_globals, 1, Constant, 0, nb_iterators[0] + nb_globals));
	
	for (size_t i = 1; i < nb_statements; i++) {
		stmt = p_polyhedral_program.getStatementById(i+1);
		nb_iterators[i] = PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(stmt).getNumberOfIterators();
		p_dim_space += nb_iterators[i];
		first_coef[i] = first_coef[i-1] + nb_iterators[i-1] + nb_globals + 1;
		
		for (size_t j = 0; j < nb_iterators[i]; j++)
			p_coef_info.push_back(CoefInfo(first_coef[i] + j, i+1, Iterator, j, j));
		for (size_t j = 0; j < nb_globals; j++)
			p_coef_info.push_back(CoefInfo(first_coef[i] + nb_iterators[i] + j, i+1, Global, j, nb_iterators[i] + j));
		p_coef_info.push_back(CoefInfo(first_coef[i] + nb_iterators[i] + nb_globals, i+1, Constant, 0, nb_iterators[i] + nb_globals));
	}
	
	std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *> * deps_list = new std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *>();
	typename std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *>::const_iterator it;
	for (it = p_dependencies.begin(); it != p_dependencies.end(); it++)
		if ((*it)->getType() == PolyhedricDependency::RaW)
			deps_list->push_back(*it);
	
	Polyhedron * current;
	
	while (deps_list->size() != 0) {
		
		p_schedule_spaces.push_back(Polyhedron(p_dim_space));
		
		std::vector<Polyhedron*> strongly_list;
		
		typename std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *>::iterator it_dep;
		for (it_dep = deps_list->begin(); it_dep != deps_list->end(); it_dep++) {
			Polyhedron * weakly;
			Polyhedron * strongly;
			
			size_t from = (*it_dep)->getFrom() - 1;
			size_t to = (*it_dep)->getTo() - 1;
			
			biLinearExpression sched_from(p_dim_space, nb_iterators[from] + nb_iterators[to] + nb_globals);
			biLinearExpression sched_to(p_dim_space, nb_iterators[from] + nb_iterators[to] + nb_globals);

			for (size_t i = 0; i < nb_iterators[from]; i++)
				sched_from.add(first_coef[from] + i, nb_iterators[to] + i, 1);
			for (size_t i = 0; i < nb_globals; i++)
				sched_from.add(first_coef[from] + nb_iterators[from] + i, nb_iterators[from] + nb_iterators[to] + i, 1);

			for (size_t i = 0; i < nb_iterators[to]; i++)
				sched_to.add(first_coef[to] + i, i, 1);
			for (size_t i = 0; i < nb_globals; i++)
				sched_to.add(first_coef[to] + nb_iterators[to] + i, nb_iterators[from] + nb_iterators[to] + i, 1);

			FarkasWS(sched_from - sched_to, (*it_dep)->getPolyhedron(), &weakly, &strongly);
			
			p_schedule_spaces.back().intersection_assign(*weakly);
			
			strongly_list.push_back(strongly);
			
			delete weakly;
		}
		std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *> * tmp_deps = new std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *>();
		size_t cnt = 0;
		for (it_dep = deps_list->begin(); it_dep != deps_list->end(); it_dep++) {
			Polyhedron tmp(p_schedule_spaces.back());
			
			tmp.intersection_assign(*(strongly_list[cnt]));
		
			const ConstraintSystem & cs = tmp.minimized_constraints(); //! < \bug seem to avoid following bug !!! (ScheduleSpace.tpp:83)
			
			if (tmp.contains_integer_point()) //! < \bug run out of memory for large/complex polyhedron... (Seem to be due to an infinite recursion in PPL::MIP_Problem)
				p_schedule_spaces.back() = tmp;
			else
				tmp_deps->push_back(*it_dep);
				
			delete strongly_list[cnt];
				
			cnt++;
		}
		delete deps_list;
		deps_list = tmp_deps;
		
	}
}

template <class Function, class Expression, class VariableLBL>
bool ScheduleSpace<Function, Expression, VariableLBL>::isValid(int ** sched) {
	for (size_t i = 0; i < p_schedule_spaces.size(); i++) {
		LinearExpression le;
		for (size_t j = 0; j < p_dim_space; j++)
			le += sched[i][j] * VariableID(j);
		if (p_schedule_spaces[i].relation_with(Generator::point(le)) == Parma_Polyhedra_Library::Poly_Gen_Relation::subsumes())
			return false;
	}
	return true;
}

template <class Function, class Expression, class VariableLBL>
ScheduleSpace<Function, Expression, VariableLBL>::ScheduleSpace(
	PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program,
	const std::vector<PolyhedricDependency::Dependency<Function, Expression, VariableLBL> *> & dependencies,
	ScheduleSpaceAlgorithm method
) :
	p_polyhedral_program(polyhedral_program),
	p_dependencies(dependencies),
	p_schedule_spaces(),
	p_coef_info(),
	p_dim_space(0)
{
	switch (method) {
		case PBCC:
			computePBCC();
			break;
		case Feautrier:
			throw Exception::NotImplementedYet("Feautrier schedule space generation algorithm");
			break;
	}
}

template <class Function, class Expression, class VariableLBL>
std::vector<Schedule<Function, Expression, VariableLBL> > * ScheduleSpace<Function, Expression, VariableLBL>::generateScheduleList(int ilb, int iub, int glb, int gub, int clb, int cub) {
	std::vector<Schedule<Function, Expression, VariableLBL> > * res = NULL;
	
	std::vector<int> lower_bounds(p_dim_space);
	std::vector<int> upper_bounds(p_dim_space);
	
	for (size_t i = 0; i < p_dim_space; i++) {
		switch (p_coef_info[i].type) {
			case Iterator:
				lower_bounds[i] = ilb;
				upper_bounds[i] = iub;
				break;
			case Global:
				lower_bounds[i] = glb;
				upper_bounds[i] = gub;
				break;
			case Constant:
				lower_bounds[i] = clb;
				upper_bounds[i] = cub;
		}
	}
	std::vector<std::vector<int*> *> tmp(p_schedule_spaces.size());
	size_t nb_sched = 1;
	for (size_t i = 0; i < p_schedule_spaces.size(); i++) {
		tmp[i] = exhaustiveExploration(p_schedule_spaces[i], lower_bounds, upper_bounds);
		nb_sched *= tmp[i]->size();
	}
	Schedule<Function, Expression, VariableLBL> empty_sched(p_polyhedral_program, p_schedule_spaces.size());
	size_t res_size = nb_sched * empty_sched.memory_size();
	if (res_size < getTotalSystemMemory() - 1000000000) { // 1 G for system
		try {
			res = new std::vector<Schedule<Function, Expression, VariableLBL> >();
			generateSchedule(NULL, tmp, *res);
		}
		catch (std::bad_alloc) {
			std::cerr << nb_sched << " schedules have been found (need " << res_size << " bytes on " << getTotalSystemMemory() << 
						" bytes of system memory) but allocation fail. Consider reducing the bounds." << std::endl;
			res = new std::vector<Schedule<Function, Expression, VariableLBL> >();
		}
	}
	else {
		std::cerr << nb_sched << " schedules have been found (need " << res_size << " bytes on " << getTotalSystemMemory() << 
						" bytes of system memory) but it is not possible to allocate enough memory to store them." << 
						" Consider reducing the bounds." << std::endl;
		res = new std::vector<Schedule<Function, Expression, VariableLBL> >();
	}
	
	return res;
}

template <class Function, class Expression, class VariableLBL>		
bool ScheduleSpace<Function, Expression, VariableLBL>::isValid(Schedule<Function, Expression, VariableLBL> & sched_) {
	if (sched_.getFunction() != p_polyhedral_program.getFunction())
		return false; // maybe an exception is better.
	
	size_t dim_sched = p_schedule_spaces.size();
	int * data = new int[dim_sched * p_dim_space];
	int ** sched = new int*[dim_sched];
	for (size_t i = 0; i < dim_sched; i++) {
		sched[i] = &(data[i * p_dim_space]);
		for (size_t j = 0; j < p_dim_space; j++) {
			const std::vector<LinearExpression> & sched_j = sched_.getSchedule(p_coef_info[j].statement+1);
			if (sched_j.size() < i)
				sched[i][j] = 0;
			else
				sched[i][j] = sched_j[i].coefficient(VariableID(p_coef_info[j].local_coef)).get_si();
		}
		
	}
	
	bool res = isValid(sched);
	
	delete [] data;
	delete [] sched;
	
	return res;
}

template <class Function, class Expression, class VariableLBL>
size_t ScheduleSpace<Function, Expression, VariableLBL>::getDimension() const {
	return p_schedule_spaces.size();
}

template <class Function, class Expression, class VariableLBL>
size_t ScheduleSpace<Function, Expression, VariableLBL>::getSize() const {
	return p_dim_space;
}
		
template <class Function, class Expression, class VariableLBL>
void ScheduleSpace<Function, Expression, VariableLBL>::print(std::ostream & out) const {
	std::vector<Polyhedron>::const_iterator it;
	for (it = p_schedule_spaces.begin(); it != p_schedule_spaces.end(); it++)
		this->print(out, &(*it));
}

template <class Function, class Expression, class VariableLBL>
void ScheduleSpace<Function, Expression, VariableLBL>::print(std::ostream & out, const Polyhedron * p) const {
	size_t nb_statements = p_polyhedral_program.getNumberOfGlobals();
	size_t nb_globals = p_polyhedral_program.getNumberOfGlobals();
	
	std::vector<size_t> nb_iterators(nb_statements);
	
	out << "        |";
	for (size_t i = 0; i < nb_statements; i++) {
		nb_iterators[i] = PolyhedricAnnotation::getDomain<Function, Expression, VariableLBL>(p_polyhedral_program.getStatementById(i)).getNumberOfIterators();
		out << std::setw((nb_iterators[i] + nb_globals + 1)*6) << *(p_polyhedral_program.getStatementById(i)) << std::setw(nb_iterators[i] + nb_globals + 1) << "|";
	}
	out << std::setw(7) << "|" << std::endl;
	
	out << "eq/ineq |";
	for (size_t i = 0; i < nb_statements; i++) {
		for (size_t j = 0; j < nb_iterators[i]; j++)
			out << std::setw(4) << toString(p_polyhedral_program.getIteratorById(p_polyhedral_program.getStatementById(i), j)) << std::setw(3) << "|";
		for (size_t j = 0; j < nb_globals; j++)
			out << std::setw(4) << toString(p_polyhedral_program.getGlobalById(j)) << std::setw(3) << "|";
		out << std::setw(4) << "K" << std::setw(3) << "|";
	}
	out << std::setw(4) << "1" << std::setw(3) << "|" << std::endl;
	
	const ConstraintSystem & cs = p->minimized_constraints();
	ConstraintSystem::const_iterator it;
	for (it = cs.begin(); it != cs.end(); it++) {
		if (it->is_equality())
			out << "    0   |";
		else
			out << "    1   |";
		for (size_t i = 0; i < p_dim_space; i++) {
			out << std::setw(4) << it->coefficient(VariableID(i)) << std::setw(3) << "|";
		}
		out << std::setw(4) << it->inhomogeneous_term() << std::setw(3) << "|" << std::endl;
	}
		
	
}

template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, ScheduleSpace<Function, Expression, VariableLBL> & sched_space) {
	sched_space.print(out);
	return out;
}

