
#include "common/PolyhedricContainer.hpp"

#ifndef _ANALYSES_RESULTS_TEST_HPP_
#define _ANALYSES_RESULTS_TEST_HPP_

namespace TestCommon {

class TestFunction;
class TestExpression;
class TestVariableLBL;

}

namespace PolyhedricAnnotation {

typedef PolyhedricAnnotation::PolyhedralProgram<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> TestPolyhedralProgram;
typedef PolyhedricAnnotation::Domain<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> TestDomain;
typedef PolyhedricAnnotation::Scattering<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> TestScattering;
typedef PolyhedricAnnotation::DataAccess<TestCommon::TestFunction, TestCommon::TestExpression, TestCommon::TestVariableLBL *> TestDataAccess;

}

namespace TestCommon {

class TestFunction {
	public:
		static size_t cnt;
		
	protected:
		size_t p_id;
		
		PolyhedricAnnotation::TestPolyhedralProgram * p_polyhedral_program;
	
		std::string p_name;
		
	public:
		TestFunction(std::string name);
		
		size_t getID();
	
		void setPolyhedralProgram(PolyhedricAnnotation::TestPolyhedralProgram * polyhedral_program);
	
		PolyhedricAnnotation::TestPolyhedralProgram * getPolyhedralProgram() const;
		
	friend std::ostream & operator << (std::ostream & out, TestFunction & f);
	friend std::ostream & operator << (std::ostream & out, const TestFunction & f);
};

std::ostream & operator << (std::ostream & out, TestFunction & f);
std::ostream & operator << (std::ostream & out, const TestFunction & f);

class TestExpression {
	public:
		static size_t cnt;
		
	protected:
		size_t p_id;
		
		PolyhedricAnnotation::TestDomain * p_domain;
		PolyhedricAnnotation::TestScattering * p_scattering;
		PolyhedricAnnotation::TestDataAccess * p_data_acces;
	
		std::string p_name;
	
	public:
		TestExpression(std::string name);
		
		size_t getID();
		
		void setDomain(PolyhedricAnnotation::TestDomain * domain);
		void setScattering(PolyhedricAnnotation::TestScattering * scattering);
		void setDataAccess(PolyhedricAnnotation::TestDataAccess * data_acces);
		
		PolyhedricAnnotation::TestDomain * getDomain() const;
		PolyhedricAnnotation::TestScattering * getScattering() const;
		PolyhedricAnnotation::TestDataAccess * getDataAccess() const;
		
	friend std::ostream & operator << (std::ostream & out, TestExpression & e);
	friend std::ostream & operator << (std::ostream & out, const TestExpression & e);
};

std::ostream & operator << (std::ostream & out, TestExpression & e);
std::ostream & operator << (std::ostream & out, const TestExpression & e);

class TestVariableLBL {
	protected:
		static size_t cnt;
		size_t p_id;
	
		
	
	public:
		TestVariableLBL(std::string name);
		
		size_t getID();
		
		std::string p_name;
		
	friend std::ostream & operator << (std::ostream & out, TestVariableLBL & v);
	friend std::ostream & operator << (std::ostream & out, const TestVariableLBL & v);
};

std::ostream & operator << (std::ostream & out, TestVariableLBL & v);
std::ostream & operator << (std::ostream & out, const TestVariableLBL & v);

}

bool isConstant(const TestCommon::TestVariableLBL * v);

std::string toString(const TestCommon::TestVariableLBL * v);

TestCommon::TestVariableLBL * constantLBL();

#endif /* _ANALYSES_RESULTS_TEST_HPP_ */

