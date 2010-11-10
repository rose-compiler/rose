#include "testStatementBuilder.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace SageBuilder;
using namespace SageInterface;

void ExpressionStatementBuilder::build()
{
	results_.push_back(buildExprStatement(exp_));
}
