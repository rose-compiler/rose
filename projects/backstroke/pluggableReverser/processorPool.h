#include <rose.h>
#include <boost/function.hpp>
#include <boost/foreach.hpp>

#include "statementHandler.h"

#include "utilities/types.h"
#include "singleton.h"
#include "normalizations/expNormalization.h"

#define foreach BOOST_FOREACH

using namespace boost;

typedef boost::function<ExpPairs(SgExpression*)> ExpHandler;
typedef boost::function<StmtPairs(SgStatement*)> StmtHandler;
typedef std::pair<SgFunctionDeclaration*, SgFunctionDeclaration*> FuncDeclPair;

typedef singleton_default<std::vector<ExpHandler> > ExpHandlersPool;
typedef singleton_default<std::vector<StmtHandler> > StmtHandlersPool;

inline ExpPairs processExpression(SgExpression* exp)
{
    ExpPairs outputs;
    foreach (ExpHandler handler, ExpHandlersPool::instance())
    {
        ExpPairs result = handler(exp);
        outputs.insert(outputs.end(), result.begin(), result.end());
    }
    return outputs;
}

inline StmtPairs processStatement(SgStatement* stmt)
{
    StmtPairs outputs;
    foreach (StmtHandler handler, StmtHandlersPool::instance())
    {
        StmtPairs stmt_pairs = handler(stmt);
        outputs.insert(outputs.end(), stmt_pairs.begin(), stmt_pairs.end());
    }
    return outputs;
}

inline void addExpressionHandler(ExpHandler exp_handler)
{
    ExpHandlersPool::instance().push_back(exp_handler);
}

inline void addStatementHandler(StmtHandler stmt_handler)
{
    StmtHandlersPool::instance().push_back(stmt_handler);
}

#if 0
inline SgExpression* pushVal(SgExpression* exp)
{
    return SageInterface::copyExpression(exp);
}

inline SgExpression* popVal()
{
    return SageBuilder::buildIntVal(0);
}
#endif

class ProcessorPool
{
    //std::vector<ExpHandler> exp_handlers_;
    //std::vector<StmtHandler> stmt_handlers_;


    public:

#if 1
    std::vector<FuncDeclPair>
    processEvent(SgFunctionDeclaration* func_decl)
    {
        std::vector<FuncDeclPair> output;

        std::vector<StmtPair> func_decls = processFunctionDeclaration(func_decl);
        foreach (StmtPair stmt_pair, func_decls)
        {
            SgFunctionDeclaration* fwd_decl = isSgFunctionDeclaration(stmt_pair.first);
            SgFunctionDeclaration* rvs_decl = isSgFunctionDeclaration(stmt_pair.second);

            if (fwd_decl && rvs_decl)
                output.push_back(std::make_pair(fwd_decl, rvs_decl));
        }

        return output;
    }
#endif
};

class StatementProcessorPool
{
};
