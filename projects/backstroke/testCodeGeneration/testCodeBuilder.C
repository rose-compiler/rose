#include "testCodeBuilder.h"
#include "testExpressionBuilder.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

#define foreach BOOST_FOREACH

void BasicExpressionTest::build()
{
	// First build the state class.
	setStateClassName("State");
	addStateMember("i", buildIntType());
	addStateMember("f", buildFloatType());
	buildStateClass();

	SgExpression* int_var = buildStateMemberExpression("i");
	SgExpression* float_var = buildStateMemberExpression("f");


	// A expression builder pool uses its child builders to build expressions.
	ExpressionBuilderPool builders;
	builders.addExpressionBuilder(new UnaryExpressionBuilder(int_var));
	builders.addExpressionBuilder(new UnaryExpressionBuilder(float_var));
	builders.addExpressionBuilder(new BinaryExpressionBuilder(int_var, float_var));
	builders.addExpressionBuilder(new BinaryExpressionBuilder(float_var, int_var));
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