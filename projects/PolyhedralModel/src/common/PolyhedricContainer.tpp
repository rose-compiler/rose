/**
 * \file src/common/PolyhedricContainer.tpp
 * \brief Implementation of Polyhedric Containers (template).
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

template <class Function, class Expression, class VariableLBL>
void makeLinExp(
	PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program,
	Expression * expression,
	const std::vector<std::pair<VariableLBL, int> > & inequation,
	LinearExpression * lin_exp,
	size_t global_offset = 0
) {
	typename std::vector<std::pair<VariableLBL, int> >::const_iterator it;
	for (it = inequation.begin(); it != inequation.end(); it++) {
		if (!isConstant(it->first)) {
			size_t var;
			if (polyhedral_program.isIterator(it->first))
				var = polyhedral_program.getVariableID(expression, it->first);
			else
				var = global_offset + polyhedral_program.getVariableID(NULL, it->first);
			*lin_exp += it->second * VariableID(var);
		}
		else
			*lin_exp += it->second;
	}
}

/*********************/
/* PolyhedralProgram */
/*********************/

template <class Function, class Expression, class VariableLBL>
VariableLBL PolyhedralProgram<Function, Expression, VariableLBL>::getByID(size_t id, const std::map<VariableLBL, size_t> & map) {
	typename std::map<VariableLBL, size_t>::const_iterator it;
	for (it = map.begin(); it != map.end(); it++)
		if (it->second == id) break;
	if (it == map.end())
		throw Exception::UnknownVariableID();
	return it->first;
}

template <class Function, class Expression, class VariableLBL>
PolyhedralProgram<Function, Expression, VariableLBL>::PolyhedralProgram(Function * function) :
	p_function(function),
	p_variables(),
	p_expressions(),
	p_iterator_list(),
	p_var_map()
{
	p_var_map.insert(std::pair<Expression *, std::pair<size_t, std::map<VariableLBL, size_t> > >(
		NULL,
		std::pair<size_t, std::map<VariableLBL, size_t> >(0, std::map<VariableLBL, size_t>())
	));
}

		// Keep track of iterator and globals for aliasing (each Expression for is domain have is own scope)
		// For globals: "Expression global = NULL;" (a common scope to enforce aliasing for relation computation)
		
template <class Function, class Expression, class VariableLBL>
void PolyhedralProgram<Function, Expression, VariableLBL>::addIterator(VariableLBL it) {
	if (!isIterator(it)) p_iterator_list.push_back(it);
}

template <class Function, class Expression, class VariableLBL>
bool PolyhedralProgram<Function, Expression, VariableLBL>::isIterator(VariableLBL v) const {
	return find(p_iterator_list.begin(), p_iterator_list.end(), v) != p_iterator_list.end();
}

template <class Function, class Expression, class VariableLBL>
const std::vector<VariableLBL> & PolyhedralProgram<Function, Expression, VariableLBL>::getIterators() const {
	return p_iterator_list;
}

template <class Function, class Expression, class VariableLBL>
const size_t PolyhedralProgram<Function, Expression, VariableLBL>::getVariableID(Expression * e, VariableLBL v) {
	typename std::map<Expression *, std::pair<size_t, std::map<VariableLBL, size_t> > >::iterator exp_map = p_var_map.find(e);
	if (exp_map == p_var_map.end())
		throw Exception::UnknownExpression<Function, Expression>(p_function, e);
	typename std::map<VariableLBL, size_t>::iterator pair = exp_map->second.second.find(v);
	if (pair == exp_map->second.second.end()) {
		pair = exp_map->second.second.insert(exp_map->second.second.begin(), std::pair<VariableLBL, size_t>(v, exp_map->second.first));
		exp_map->second.first++;
	}
	return pair->second;
}

template <class Function, class Expression, class VariableLBL>
void PolyhedralProgram<Function, Expression, VariableLBL>::addExpression(Expression * e) {
	p_expressions.push_back(e);
	p_var_map.insert(std::pair<Expression *, std::pair<size_t, std::map<VariableLBL, size_t> > >(
		e,
		std::pair<size_t, std::map<VariableLBL, size_t> >(0, std::map<VariableLBL, size_t>())
	));
}

template <class Function, class Expression, class VariableLBL>
const std::vector<Expression *> & PolyhedralProgram<Function, Expression, VariableLBL>::getExpressions() const {
	return p_expressions;
}

		
template <class Function, class Expression, class VariableLBL>
size_t PolyhedralProgram<Function, Expression, VariableLBL>::getVariableID(VariableLBL v) {
	for (size_t cnt = 0; cnt < p_variables.size(); cnt++)
		if (p_variables[cnt] == v) return cnt + 1;
	p_variables.push_back(v);
	return p_variables.size();
}
		
template <class Function, class Expression, class VariableLBL>
size_t PolyhedralProgram<Function, Expression, VariableLBL>::getVariableID(VariableLBL v) const {
	for (size_t cnt = 0; cnt < p_variables.size(); cnt++)
		if (p_variables[cnt] == v) return cnt + 1;
	throw Exception::UnknownVariable();
}

template <class Function, class Expression, class VariableLBL>
size_t PolyhedralProgram<Function, Expression, VariableLBL>::getNumberOfVariables() const {
	return p_variables.size();
}

template <class Function, class Expression, class VariableLBL>
VariableLBL PolyhedralProgram<Function, Expression, VariableLBL>::getVariableByID(size_t id) const {
	if (id > p_variables.size())	
		throw Exception::UnknownVariableID();
	return p_variables[id-1];
}

template <class Function, class Expression, class VariableLBL>
VariableLBL PolyhedralProgram<Function, Expression, VariableLBL>::getIteratorById(Expression * e, size_t id) const {
	typename std::map<Expression *, std::pair<size_t, std::map<VariableLBL, size_t> > >::const_iterator exp_map = p_var_map.find(e);
	if (exp_map == p_var_map.end())
		throw Exception::UnknownExpression<Function, Expression>(p_function, e);
	return getByID(id, exp_map->second.second);
}

template <class Function, class Expression, class VariableLBL>
VariableLBL PolyhedralProgram<Function, Expression, VariableLBL>::getGlobalById(size_t id) const {
	typename std::map<Expression *, std::pair<size_t, std::map<VariableLBL, size_t> > >::const_iterator exp_map = p_var_map.find(NULL);
	if (exp_map == p_var_map.end())
		throw Exception::UnknownExpression<Function, Expression>(p_function, NULL);
	return getByID(id, exp_map->second.second);
}

template <class Function, class Expression, class VariableLBL>
size_t PolyhedralProgram<Function, Expression, VariableLBL>::getNumberOfGlobals() const {
	typename std::map<Expression *, std::pair<size_t, std::map<VariableLBL, size_t> > >::const_iterator exp_map = p_var_map.find(NULL);
	if (exp_map == p_var_map.end())
		throw Exception::UnknownExpression<Function, Expression>(p_function, NULL);
	return exp_map->second.second.size();
}

template <class Function, class Expression, class VariableLBL>
size_t PolyhedralProgram<Function, Expression, VariableLBL>::getStatementID(Expression * e) const {
	for (size_t cnt = 0; cnt < p_expressions.size(); cnt++)
		if (p_expressions[cnt] == e) return cnt + 1;
	throw Exception::UnknownExpression<Function, Expression>(p_function, e);
	return 0;
}

template <class Function, class Expression, class VariableLBL>
Expression * PolyhedralProgram<Function, Expression, VariableLBL>::getStatementById(size_t id) const {
	return p_expressions[id-1];
}

template <class Function, class Expression, class VariableLBL>
size_t PolyhedralProgram<Function, Expression, VariableLBL>::getNumberOfStatement() const {
	return p_expressions.size();
}

template <class Function, class Expression, class VariableLBL>
Function * PolyhedralProgram<Function, Expression, VariableLBL>::getFunction() const { return p_function; }

template <class Function, class Expression, class VariableLBL>
Function * PolyhedralProgram<Function, Expression, VariableLBL>::getBase() const { return p_function; }

template <class Function, class Expression, class VariableLBL>
void PolyhedralProgram<Function, Expression, VariableLBL>::finalize() const {
	typename std::vector<Expression *>::const_iterator it;
	for (it = p_expressions.begin(); it != p_expressions.end(); it++)
		getDomain<Function, Expression, VariableLBL>(*it).finalize();
}

template <class Function, class Expression, class VariableLBL>
void PolyhedralProgram<Function, Expression, VariableLBL>::print(std::ostream & out) {
	out << "# [File generated from ROSE Polyhedric Annotations]" << std::endl;
	out << std::endl;
	
	out << "SCoP" << std::endl;
	out << std::endl;
	
	out << "# ==================================================== Global" << std::endl;
	out << "# Language" << std::endl;
	out << "C" << std::endl;
	out << std::endl;
	
	out << "# Context" << std::endl;
	{
		typename std::map<Expression *, std::pair<size_t, std::map<VariableLBL, size_t> > >::iterator exp_map = p_var_map.find(NULL);
		if (exp_map == p_var_map.end())
			throw Exception::UnknownExpression<Function, Expression>(p_function, NULL);
		out << "0 " << exp_map->second.second.size() + 2 << std::endl;
		out << std::endl;
		out << "# Parameter names are provided" << std::endl;
		out << "1" << std::endl;
		out << "# Parameter names" << std::endl;
		typename std::map<VariableLBL, size_t>::iterator it;
		for (it = exp_map->second.second.begin(); it != exp_map->second.second.end(); it++)
			out  << toString(it->first) << " ";
		out << std::endl;
	}
	out << std::endl;
	
	out << "# Number of statements" << std::endl;
	out << p_expressions.size() << std::endl;
	out << std::endl;
	
	out << "# Some specific comments" << std::endl;
	out << "# " << std::endl;
	out << "# SCoP descriptor: " << *p_function << std::endl;
	out << "# " << std::endl;
	out << "# Variable IDs: " << std::endl;
	for (size_t cnt = 0; cnt < p_variables.size(); cnt++)
		out << "#\t" << cnt+1 << " -> " << toString(p_variables[cnt]) << std::endl;
	out << "# " << std::endl;
	out << "# Statemant IDs: " << std::endl;
	for (size_t cnt = 0; cnt < p_expressions.size(); cnt++)
		out << "#\t" << cnt + 1 << " -> \"" << *(p_expressions[cnt]) << "\"" << std::endl;
	out << "# " << std::endl;
	out << std::endl;
	
	for (int i = 0; i < p_expressions.size(); i++) {
		out << "# ==================================================== Statement " << i + 1 << std::endl;
		out << "# ---------------------------------------------------  " << i + 1 << ".1 Domain" << std::endl;
		out << "# Iteration domain is provided" << std::endl;
		try {
			getDomain<Function, Expression, VariableLBL>(p_expressions[i]).print(out);
		}
		catch (...) {
			out << "0 # error !" << std::endl;
		}
		
		out << "# ---------------------------------------------------  " << i + 1 << ".2 Scattering" << std::endl;
		out << "# Scattering function is provided" << std::endl;
		try {
			getScattering<Function, Expression, VariableLBL>(p_expressions[i]).print(out);
		}
		catch (...) {
			out << "0 # error !" << std::endl;
		}
		
		out << "# ---------------------------------------------------  " << i + 1 << ".3 Access" << std::endl;
		out << "# Access informations are provided" << std::endl;
		try {
			getDataAccess<Function, Expression, VariableLBL>(p_expressions[i]).print(out);
		}
		catch (...) {
			out << "0 # Front-end have not been able to process data access." << std::endl;
		}
		
		out << "# ---------------------------------------------------  " << i + 1 << ".4 Body" << std::endl;
		out << "# Statement body is provided" << std::endl;
		out << "1" << std::endl;
		out << "# Original iterator names" << std::endl;
		typename std::map<Expression *, std::pair<size_t, std::map<VariableLBL, size_t> > >::iterator exp_map = p_var_map.find(p_expressions[i]);
		if (exp_map == p_var_map.end())
			throw Exception::UnknownExpression<Function, Expression>(p_function, p_expressions[i]);
		typename std::map<VariableLBL, size_t>::reverse_iterator it;
		for (it = exp_map->second.second.rbegin(); it != exp_map->second.second.rend(); it++)
			out  << toString(it->first) << " ";
		out << std::endl;
		out << "# Statement body" << std::endl;
		out << *(p_expressions[i]) << std::endl;
		out << std::endl;
		out << std::endl;
	}
}

template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program) {
	polyhedral_program.print(out);
	return out;
}

/**********/
/* Domain */
/**********/

template <class Function, class Expression, class VariableLBL>
Domain<Function, Expression, VariableLBL>::Domain(PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, Expression * expression, size_t nbr_surr_loop) :
	p_polyhedral_program(polyhedral_program),
	p_expression(expression),
	p_nbr_surr_loop(nbr_surr_loop),
	p_domain(nbr_surr_loop + p_polyhedral_program.getNumberOfGlobals())
{}

template <class Function, class Expression, class VariableLBL>
PolyhedralProgram<Function, Expression, VariableLBL> & Domain<Function, Expression, VariableLBL>::getPolyhedralProgram() const { return p_polyhedral_program; }

template <class Function, class Expression, class VariableLBL>
void Domain<Function, Expression, VariableLBL>::addInequation(const std::vector<std::pair<VariableLBL, int> > & inequation, bool positive) {
	LinearExpression lin_exp(0);
	makeLinExp(p_polyhedral_program, p_expression, inequation, &lin_exp, p_nbr_surr_loop);
	if (lin_exp.space_dimension() > p_domain.space_dimension())
		p_domain.add_space_dimensions_and_embed(lin_exp.space_dimension() - p_domain.space_dimension());
	p_domain.refine_with_constraint(positive ? lin_exp >= 0 : lin_exp <= 0);
}

template <class Function, class Expression, class VariableLBL>
const Polyhedron & Domain<Function, Expression, VariableLBL>::getDomainPolyhedron() const {
	return p_domain;
}

template <class Function, class Expression, class VariableLBL>
size_t Domain<Function, Expression, VariableLBL>::getNumberOfIterators() const {
	return p_nbr_surr_loop;
}

template <class Function, class Expression, class VariableLBL>
size_t Domain<Function, Expression, VariableLBL>::getNumberOfGlobals() const {
	return p_polyhedral_program.getNumberOfGlobals();
}
		
template <class Function, class Expression, class VariableLBL>
void Domain<Function, Expression, VariableLBL>::finalize() {
	p_domain.add_space_dimensions_and_embed(p_nbr_surr_loop + p_polyhedral_program.getNumberOfGlobals() - p_domain.space_dimension());
}

template <class Function, class Expression, class VariableLBL>
void Domain<Function, Expression, VariableLBL>::print(std::ostream & out) {
	out << "1" << std::endl;
	
	const ConstraintSystem & cs = p_domain.constraints();
	ConstraintSystem::const_iterator it;
	size_t cs_size = 0;
	for (it = cs.begin(); it != cs.end(); it++) cs_size++;
	out << cs_size << " " << p_domain.space_dimension() + 2 << std::endl;
	
	out << "#eq/in";
	for (int i = 0; i < p_domain.space_dimension(); i++)
		if (i < p_nbr_surr_loop)
			out << std::setw(6) << toString(p_polyhedral_program.getIteratorById(p_expression, i));
		else
			out << std::setw(6) << toString(p_polyhedral_program.getGlobalById(i - p_nbr_surr_loop));
	out << std::setw(6) << "1" << std::endl;
	
	for (it = cs.begin(); it != cs.end(); it++) {
		if ((*it).is_equality())
			out << std::setw(6) << "0";
		else
			out << std::setw(6) << "1";
		for (int i = 0; i < p_domain.space_dimension(); i++)
			out << std::setw(6) << (*it).coefficient(VariableID(i));
		out << std::setw(6) << (*it).inhomogeneous_term() << std::endl;
	}
	out << std::endl;
}

template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, Domain<Function, Expression, VariableLBL> & domain) {
	domain.print(out);
	return out;
}

/**************/
/* Scattering */
/**************/

template <class Function, class Expression, class VariableLBL>
Scattering<Function, Expression, VariableLBL>::Scattering(PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, Expression * expression, size_t nbr_surr_loop) :
	p_polyhedral_program(polyhedral_program),
	p_expression(expression),
	p_nbr_surr_loop(nbr_surr_loop),
	p_equations()
{}

template <class Function, class Expression, class VariableLBL>
PolyhedralProgram<Function, Expression, VariableLBL> & Scattering<Function, Expression, VariableLBL>::getPolyhedralProgram() const { return p_polyhedral_program; }

template <class Function, class Expression, class VariableLBL>
void Scattering<Function, Expression, VariableLBL>::addEquation(std::vector<std::pair<VariableLBL, int> > & equation) {
	LinearExpression lin_exp(0);
	makeLinExp(p_polyhedral_program, p_expression, equation, &lin_exp, p_nbr_surr_loop);
	p_equations.push_back(lin_exp);
}

template <class Function, class Expression, class VariableLBL>
const std::vector<LinearExpression> & Scattering<Function, Expression, VariableLBL>::getScattering() const {
	return p_equations;
}
		
template <class Function, class Expression, class VariableLBL>
void Scattering<Function, Expression, VariableLBL>::print(std::ostream & out) {
	out << "1" << std::endl;
	size_t dim_max = p_nbr_surr_loop + p_polyhedral_program.getNumberOfGlobals();
	out << p_equations.size() << " " << dim_max + 2 << std::endl;
	out << "#eq/in";
	for (int i = 0; i < dim_max; i++)
		if (i < p_nbr_surr_loop)
			out << std::setw(6) << toString(p_polyhedral_program.getIteratorById(p_expression, i));
		else
			out << std::setw(6) << toString(p_polyhedral_program.getGlobalById(i - p_nbr_surr_loop));
	out << std::setw(6) << "1" << std::endl;
	std::vector<LinearExpression>::iterator it;
	for (it = p_equations.begin(); it != p_equations.end(); it++) {
		out << std::setw(6) << "0";
		for (int i = 0; i < dim_max; i++)
			out << std::setw(6) << (*it).coefficient(VariableID(i));
		out << std::setw(6) << (*it).inhomogeneous_term() << std::endl;
	}
}

template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, Scattering<Function, Expression, VariableLBL> & scattering) {
	scattering.print(out);
	return out;
}

/**************/
/* DataAccess */
/**************/

template <class Function, class Expression, class VariableLBL>
DataAccess<Function, Expression, VariableLBL>::DataAccess(PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, Expression * expression, size_t nbr_surr_loop):
	p_polyhedral_program(polyhedral_program),
	p_expression(expression),
	p_nbr_surr_loop(nbr_surr_loop),
	p_read(),
	p_write()
{}

template <class Function, class Expression, class VariableLBL>
PolyhedralProgram<Function, Expression, VariableLBL> & DataAccess<Function, Expression, VariableLBL>::getPolyhedralProgram() const { return p_polyhedral_program; }

template <class Function, class Expression, class VariableLBL>
void DataAccess<Function, Expression, VariableLBL>::addRead(VariableLBL v, const std::vector<std::vector<std::pair<VariableLBL, int> > > & access_vector) {
	std::vector<LinearExpression> vect;
	
	typename std::vector<std::vector<std::pair<VariableLBL, int> > >::const_iterator it;
	for (it = access_vector.begin(); it != access_vector.end(); it++) {
		LinearExpression lin_exp(0);
		makeLinExp(p_polyhedral_program, p_expression, *it, &lin_exp, p_nbr_surr_loop);
		vect.push_back(lin_exp);
	}
	p_read.push_back(std::pair<size_t, std::vector<LinearExpression> >(p_polyhedral_program.getVariableID(v), vect));
}

template <class Function, class Expression, class VariableLBL>
std::vector<std::vector<LinearExpression> > * DataAccess<Function, Expression, VariableLBL>::getRead(VariableLBL v) const {
	std::vector<std::vector<LinearExpression> > * res = new std::vector<std::vector<LinearExpression> >();
	
	std::vector<std::pair<size_t, std::vector<LinearExpression> > >::const_iterator it;
	for (it = p_read.begin(); it != p_read.end(); it++)
		if (it->first == p_polyhedral_program.getVariableID(v))
			res->push_back(it->second);
			
	return res;
}

template <class Function, class Expression, class VariableLBL>
void DataAccess<Function, Expression, VariableLBL>::addWrite(VariableLBL v, const std::vector<std::vector<std::pair<VariableLBL, int> > > & access_vector) {
	std::vector<LinearExpression> vect;
	
	typename std::vector<std::vector<std::pair<VariableLBL, int> > >::const_iterator it;
	for (it = access_vector.begin(); it != access_vector.end(); it++) {
		LinearExpression lin_exp(0);
		makeLinExp(p_polyhedral_program, p_expression, *it, &lin_exp, p_nbr_surr_loop);
		vect.push_back(lin_exp);
	}
	p_write.push_back(std::pair<size_t, std::vector<LinearExpression> >(p_polyhedral_program.getVariableID(v), vect));
}

template <class Function, class Expression, class VariableLBL>
std::vector<std::vector<LinearExpression> > * DataAccess<Function, Expression, VariableLBL>::getWrite(VariableLBL v) const {
	std::vector<std::vector<LinearExpression> > * res = new std::vector<std::vector<LinearExpression> >();
	
	std::vector<std::pair<size_t, std::vector<LinearExpression> > >::const_iterator it;
	for (it = p_write.begin(); it != p_write.end(); it++)
		if (it->first == p_polyhedral_program.getVariableID(v))
			res->push_back(it->second);
			
	return res;
}
		
template <class Function, class Expression, class VariableLBL>
void DataAccess<Function, Expression, VariableLBL>::print(std::ostream & out) {
	out << "1" << std::endl;
	
	size_t dim_max = p_nbr_surr_loop + p_polyhedral_program.getNumberOfGlobals();
	
	size_t nbr_line = 0;
	std::vector<std::pair<size_t, std::vector<LinearExpression> > >::iterator it1;
	std::vector<LinearExpression>::iterator it2;
	
	out << "# Read acces information" << std::endl;
	for (it1 = p_read.begin(); it1 != p_read.end(); it1++)
		nbr_line += (*it1).second.size();
	out << nbr_line << " " << dim_max + 2 << std::endl;
	
	out << "#  Var";
	for (int i = 0; i < dim_max; i++)
		if (i < p_nbr_surr_loop)
			out << std::setw(6) << toString(p_polyhedral_program.getIteratorById(p_expression, i));
		else
			out << std::setw(6) << toString(p_polyhedral_program.getGlobalById(i - p_nbr_surr_loop));
	out << std::setw(6) << "1" << std::endl;
	
	for (it1 = p_read.begin(); it1 != p_read.end(); it1++) {
		for (it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++) {
			if (it2 == (*it1).second.begin())
				out << std::setw(6) << (*it1).first;
			else
				out << std::setw(6) << "0";
			for (int i = 0; i < dim_max; i++)
				out << std::setw(6) << (*it2).coefficient(VariableID(i));
			out << std::setw(6) << (*it2).inhomogeneous_term() << std::endl;
		}
	}
	
	nbr_line = 0;
	out << "# Write acces information" << std::endl;
	for (it1 = p_write.begin(); it1 != p_write.end(); it1++)
		nbr_line += (*it1).second.size();
	out << nbr_line << " " << dim_max + 2 << std::endl;
	
	out << "#  Var";
	for (int i = 0; i < dim_max; i++)
		if (i < p_nbr_surr_loop)
			out << std::setw(6) << toString(p_polyhedral_program.getIteratorById(p_expression, i));
		else
			out << std::setw(6) << toString(p_polyhedral_program.getGlobalById(i - p_nbr_surr_loop));
	out << std::setw(6) << "1" << std::endl;
	
	for (it1 = p_write.begin(); it1 != p_write.end(); it1++) {
		for (it2 = (*it1).second.begin(); it2 != (*it1).second.end(); it2++) {
			if (it2 == (*it1).second.begin())
				out << std::setw(6) << (*it1).first;
			else
				out << std::setw(6) << "0";
			for (int i = 0; i < dim_max; i++)
				out << std::setw(6) << (*it2).coefficient(VariableID(i));
			out << std::setw(6) << (*it2).inhomogeneous_term() << std::endl;
		}
	}
	out << std::endl;

}

template <class Function, class Expression, class VariableLBL>
const std::vector<std::pair<size_t, std::vector<LinearExpression> > > & DataAccess<Function, Expression, VariableLBL>::getRead() const { return p_read; }
template <class Function, class Expression, class VariableLBL>
const std::vector<std::pair<size_t, std::vector<LinearExpression> > > & DataAccess<Function, Expression, VariableLBL>::getWrite() const { return p_write; }

template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, DataAccess<Function, Expression, VariableLBL> & data_access) {
	data_access.print(out);
	return out;
}

