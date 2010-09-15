#include "whileStatementProcessor.h"
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
#define foreach BOOST_FOREACH

vector<EvaluationResult> IfStatementProcessor::evaluate(SgStatement* stmt, const VariableVersionTable& var_table)
{
    vector<EvaluationResult> results;
    SgWhileStmt* while_stmt = isSgWhileStmt(stmt);
    if (while_stmt == NULL)
        return results;

	
}





