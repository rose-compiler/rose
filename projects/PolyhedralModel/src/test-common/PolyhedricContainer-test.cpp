
#include "test-common/PolyhedricContainer-test.hpp"

namespace PolyhedricAnnotation {

template <>
TestPolyhedralProgram & getPolyhedralProgram<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(const TestCommon::TestFunction * function) {
	TestPolyhedralProgram * res = function->getPolyhedralProgram();
	return *res;
}

template <>
void setPolyhedralProgram<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(TestCommon::TestFunction * function) {
	function->setPolyhedralProgram(new TestPolyhedralProgram(function));
}

//

template <>
TestDomain & getDomain<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(const TestCommon::TestExpression * expression) {
	TestDomain * res = expression->getDomain();
	return *res;
}

template <>
void setDomain<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(
	TestPolyhedralProgram & polyhedral_program,
	TestCommon::TestExpression * expression,
	size_t nbr_surr_loop
) {
	expression->setDomain(new TestDomain(polyhedral_program, expression, nbr_surr_loop));
}

//

template <>
TestScattering & getScattering<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(const TestCommon::TestExpression * expression) {
	TestScattering * res = expression->getScattering();
	return *res;
}

template <>
void setScattering<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(
	TestPolyhedralProgram & polyhedral_program,
	TestCommon::TestExpression * expression,
	size_t nbr_surr_loop
) {
	expression->setScattering(new TestScattering(polyhedral_program, expression, nbr_surr_loop));
}

//

template <>
TestDataAccess & getDataAccess<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(const TestCommon::TestExpression * expression) {
	TestDataAccess * res = expression->getDataAccess();
	return *res;
}

template <>
void setDataAccess<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(
	TestPolyhedralProgram & polyhedral_program,
	TestCommon::TestExpression * expression,
	size_t nbr_surr_loop
) {
	expression->setDataAccess(new TestDataAccess(polyhedral_program, expression, nbr_surr_loop));
}

template <>
void makeAccessAnnotation<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *>(TestCommon::TestExpression * expression, TestDataAccess & data_access) {}

}

namespace TestCommon {

/****************/
/* TestFunction */
/****************/

size_t TestFunction::cnt = 0;

TestFunction::TestFunction(std::string name): p_id(cnt++), p_polyhedral_program(NULL), p_name(name) {}
		
size_t TestFunction::getID() { return p_id; }
	
void TestFunction::setPolyhedralProgram(PolyhedricAnnotation::TestPolyhedralProgram * polyhedral_program) { p_polyhedral_program = polyhedral_program; }
	
PolyhedricAnnotation::TestPolyhedralProgram * TestFunction::getPolyhedralProgram() const { return p_polyhedral_program; }

std::ostream & operator << (std::ostream & out, TestFunction & f) {
	return out << f.p_name;
}
std::ostream & operator << (std::ostream & out, const TestFunction & f) {
	return out << f.p_name;
}

/******************/
/* TestExpression */
/******************/

size_t TestExpression::cnt = 0;

TestExpression::TestExpression(std::string name) :
	p_id(cnt++),
	p_domain(NULL),
	p_scattering(NULL),
	p_data_acces(NULL),
	p_name(name)
{}
		
size_t TestExpression::getID() { return p_id; }
		
void TestExpression::setDomain(PolyhedricAnnotation::TestDomain * domain) { p_domain = domain; }
void TestExpression::setScattering(PolyhedricAnnotation::TestScattering * scattering) { p_scattering = scattering; }
void TestExpression::setDataAccess(PolyhedricAnnotation::TestDataAccess * data_acces) { p_data_acces = data_acces; }
		
PolyhedricAnnotation::TestDomain * TestExpression::getDomain() const { return p_domain; }
PolyhedricAnnotation::TestScattering * TestExpression::getScattering() const { return p_scattering; }
PolyhedricAnnotation::TestDataAccess * TestExpression::getDataAccess() const { return p_data_acces; }

std::ostream & operator << (std::ostream & out, TestExpression & e) {
	return out << e.p_name;
}
std::ostream & operator << (std::ostream & out, const TestExpression & e) {
	return out << e.p_name;
}

/*******************/
/* TestVariableLBL */
/*******************/

size_t TestVariableLBL::cnt = 0;

TestVariableLBL::TestVariableLBL(std::string name) : p_id(cnt++), p_name(name) {}
		
size_t TestVariableLBL::getID() { return p_id; }

std::ostream & operator << (std::ostream & out, TestVariableLBL & v) {
	return out << v.p_name;
}
std::ostream & operator << (std::ostream & out, const TestVariableLBL & v) {
	return out << v.p_name;
}

}

bool isConstant(const TestCommon::TestVariableLBL * v) {
	return v = NULL;
}

std::string toString(const TestCommon::TestVariableLBL * v) {
	return v->p_name;
}
/*
template <>
TestCommon::TestVariableLBL * constantLBL<TestCommon::TestVariableLBL *>() {
	return NULL;
}
*/

TestCommon::TestVariableLBL * constantLBL() {
	return NULL;
}

