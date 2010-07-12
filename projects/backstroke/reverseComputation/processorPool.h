#include <rose.h>
#include <boost/function.hpp>

using namespace boost;

typedef boost::function<ExpPair(SgExpression*)> ExpHandler;

class ProcessorPool
{
    vector<ExpHandler> exp_handlers_;
    vector<StmtHandler> stmt_handlers_;

    vector<ExpPair> processExpression(SgExpression* exp)
    {
        vector<ExpPair> outputs;
        foreach (ExpHandler handler, exp_handlers_)
            outputs.push_back(handler(exp));
        return outputs;
    }

    vector<StmtPair> processStatement(SgStatement* stmt)
    {
    }


};

class StatementProcessorPool
{
};
