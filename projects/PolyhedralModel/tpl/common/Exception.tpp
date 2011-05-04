/**
 * \file src/common/Exception.tpp
 * \brief Implementation of Exception (template).
 * \author Tristan Vanderbruggen
 * \version 0.1
 */

template <class Function, class Expression>
UnknownExpression<Function, Expression>::UnknownExpression(Function * function, Expression * expression) :
	ContainerException(""),
	p_function(function),
	p_expression(expression)
{}

template <class Function, class Expression>
UnknownExpression<Function, Expression>::~UnknownExpression() {}

template <class Function, class Expression>	
void UnknownExpression<Function, Expression>::print(std::ostream & out) {
	out << "The expression " << *p_expression << " is unknown in the function " << *p_function << " scope." << std::endl;
}

template <class Expression, class VariableLBL>
AmbiguousVariableDim<Expression, VariableLBL>::AmbiguousVariableDim(Expression * source, Expression * target, VariableLBL variable) :
	ContainerException(""),
	p_source(source),
	p_target(target),
	p_variable(variable)
{}

template <class Expression, class VariableLBL>
AmbiguousVariableDim<Expression, VariableLBL>::~AmbiguousVariableDim() {}

template <class Expression, class VariableLBL>
void AmbiguousVariableDim<Expression, VariableLBL>::print(std::ostream & out) {
	out << "The variable " << *p_variable << " has different dimension in expressions " << *p_source << " and " << *p_target << "." << std::endl;
}

