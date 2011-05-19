/**
 * \file src/common/Schedule.tpp
 * \brief Implementation of Schedule (template).
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

template <class Function, class Expression, class VariableLBL>
Schedule<Function, Expression, VariableLBL>::Schedule(PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & polyhedral_program, size_t dim) :
	p_polyhedral_program(polyhedral_program),
	p_dim(dim),
	p_schedule(),
	p_memory_size(sizeof(Schedule<Function, Expression, VariableLBL>))
{
	p_memory_size += p_polyhedral_program.getNumberOfStatement() * (sizeof(std::pair<Expression *, std::vector<LinearExpression> >) + p_dim * sizeof(LinearExpression));
	for (size_t i = 1; i <= p_polyhedral_program.getNumberOfStatement(); i++) {
		p_schedule.insert(std::pair<Expression *, std::vector<LinearExpression> >(
			p_polyhedral_program.getStatementById(i),
			std::vector<LinearExpression>(p_dim)
		));
		
	}
}
		
template <class Function, class Expression, class VariableLBL>
size_t Schedule<Function, Expression, VariableLBL>::getDimension() const {
	return p_dim;
}

template <class Function, class Expression, class VariableLBL>
std::vector<LinearExpression> & Schedule<Function, Expression, VariableLBL>::setSchedule(size_t expression) {
	return p_schedule[p_polyhedral_program.getStatementById(expression)];
}
		
template <class Function, class Expression, class VariableLBL>
const std::vector<LinearExpression> & Schedule<Function, Expression, VariableLBL>::getSchedule(size_t expression) const {
	typename std::map<Expression *, std::vector<LinearExpression> >::const_iterator sched = p_schedule.find(p_polyhedral_program.getStatementById(expression));
	if (sched == p_schedule.end())
		throw Exception::UnknownExpression<Function, Expression>(p_polyhedral_program.getFunction(), p_polyhedral_program.getStatementById(expression));
	return sched->second;
}
		
template <class Function, class Expression, class VariableLBL>
Function * Schedule<Function, Expression, VariableLBL>::getFunction() const {
	return p_polyhedral_program.getFunction();
}

template <class Function, class Expression, class VariableLBL>
const PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & Schedule<Function, Expression, VariableLBL>::getPolyhedralProgram() const {
	return p_polyhedral_program;
}

template <class Function, class Expression, class VariableLBL>
PolyhedricAnnotation::PolyhedralProgram<Function, Expression, VariableLBL> & Schedule<Function, Expression, VariableLBL>::getPolyhedralProgram() {
	return p_polyhedral_program;
}

template <class Function, class Expression, class VariableLBL>
void Schedule<Function, Expression, VariableLBL>::print(std::ostream & out) const {
	print(out, "");
}

template <class Function, class Expression, class VariableLBL>
void Schedule<Function, Expression, VariableLBL>::print(std::ostream & out, std::string indent) const {
	for (size_t i = 1; i <= p_polyhedral_program.getNumberOfStatement(); i++) {
		out << indent << "S" << i << ": " << std::endl;
		std::vector<LinearExpression>::const_iterator it;
		const std::vector<LinearExpression> & vect = p_schedule.find(p_polyhedral_program.getStatementById(i))->second;
		for (it = vect.begin(); it != vect.end(); it++) {
			out << indent << "\t";
			for (size_t j = 0; j < it->space_dimension(); j++) {
				out << it->coefficient(VariableID(j)) << " ";
			}
			out << it->inhomogeneous_term() << std::endl;
		}
	}
}

template <class Function, class Expression, class VariableLBL>
size_t Schedule<Function, Expression, VariableLBL>::memory_size() const {
	return p_memory_size;
}
		
template <class Function, class Expression, class VariableLBL>
Schedule<Function, Expression, VariableLBL> & Schedule<Function, Expression, VariableLBL>::operator = (const Schedule<Function, Expression, VariableLBL> & s) {
//	p_polyhedral_program = s.p_polyhedral_program;
	p_dim = s.p_dim;
	p_schedule = s.p_schedule;
	p_memory_size = s.p_memory_size;
}

template <class Function, class Expression, class VariableLBL>
std::ostream & operator << (std::ostream & out, Schedule<Function, Expression, VariableLBL> & sched) {
	sched.print(out);
	return out;
}

