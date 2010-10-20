#include "testCodeBuilder.h"
#include "testExpressionBuilder.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

#define foreach BOOST_FOREACH

class BasicExpressionBuilder : public ExpressionBuilder
{
	std::vector<ExpressionBuilderPtr> exp_builders_;

public:
	virtual void build();
};

void BasicExpressionBuilder::build()
{
	
}

void BasicExpressionTest::build()
{
	// First build the state class.
	setStateClassName("State");
	addStateMember("b", buildBoolType());
	addStateMember("c", buildCharType());
	addStateMember("s", buildShortType());
	addStateMember("i", buildIntType());
	addStateMember("l", buildLongType());
	addStateMember("ll", buildLongLongType());
	addStateMember("f", buildFloatType());
	addStateMember("d", buildDoubleType());
	addStateMember("ld", buildLongDoubleType());
	//addStateMember("p", buildPointerType(buildVoidType()));
	
	buildStateClass();

	// Get all state member vars.
	vector<SgExpression*> vars = getAllStateMemberVars();

	// A expression builder pool uses its child builders to build expressions.
	ExpressionBuilderPool builders;
	foreach (SgExpression* var1, vars)
	{
		builders.addExpressionBuilder(new UnaryExpressionBuilder(var1));
		foreach (SgExpression* var2, vars)
			builders.addExpressionBuilder(new BinaryExpressionBuilder(var1, var2));
	}

	builders.build();

	vector<SgExpression*> exps = builders.getGeneratedExpressions();
	vector<SgBasicBlock*> bodies;
	foreach (SgExpression* exp, exps)
	{
		SgExprStatement* stmt = buildExprStatement(exp);
		SgBasicBlock* body = buildBasicBlock(stmt);
		bodies.push_back(body);
	}

	// Finally, build the test code.
	buildTestCode(bodies);
}


void ComplexExpressionTest::build()
{
	// First build the state class.
	setStateClassName("State");
	addStateMember("b", buildBoolType());
	addStateMember("c", buildCharType());
	addStateMember("s", buildShortType());
	addStateMember("i", buildIntType());
	addStateMember("l", buildLongType());
	addStateMember("ll", buildLongLongType());
	addStateMember("f", buildFloatType());
	addStateMember("d", buildDoubleType());
	addStateMember("ld", buildLongDoubleType());
	//addStateMember("p", buildPointerType(buildVoidType()));

	buildStateClass();

	// Get all state member vars.
	vector<SgExpression*> vars = getAllStateMemberVars();

	// A expression builder pool uses its child builders to build expressions.
	ExpressionBuilderPool builders;
	foreach (SgExpression* var1, vars)
	{
		builders.addExpressionBuilder(new UnaryExpressionBuilder(var1));
		foreach (SgExpression* var2, vars)
			;//builders.addExpressionBuilder(new BinaryExpressionBuilder(var1, var2));
	}

	builders.build();
#if 1
	vector<SgExpression*> all_exp = builders.getGeneratedExpressions();

	ExpressionBuilderPool builders2;
	foreach (SgExpression* var1, all_exp)
	{
		builders2.addExpressionBuilder(new UnaryExpressionBuilder(var1));
		//foreach (SgExpression* var2, all_exp)
			//builders2.addExpressionBuilder(new BinaryExpressionBuilder(var1, var2));
	}

	foreach (SgExpression* var, all_exp)
		deepDelete(var);

	builders2.build();
#endif
	vector<SgExpression*> exps = builders2.getGeneratedExpressions();
	vector<SgBasicBlock*> bodies;
	foreach (SgExpression* exp, exps)
	{
		SgExprStatement* stmt = buildExprStatement(exp);
		SgBasicBlock* body = buildBasicBlock(stmt);
		bodies.push_back(body);
	}

	// Finally, build the test code.
	buildTestCode(bodies);
}