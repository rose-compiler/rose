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

class EventHandler
{
    SgFunctionDeclaration* event_;
    std::vector<ExpressionHandler*> exp_handlers_;
    std::vector<StatementHandler*> stmt_handlers_;

    friend class ExpressionHandler;
    friend class StatementHandler;

private:

    ExpPairs handleExpression(SgExpression* exp)
    {
        ExpPairs output;
        foreach (ExpressionHandler* exp_handler, exp_handlers_)
        {
            ExpPairs result = exp_handler->handleExpression(exp);
            output.insert(output.end(), result.begin(), result.end());
        }
        return output;
    }

    StmtPairs handleStatement(SgStatement* stmt)
    {
        StmtPairs output;
        foreach (StatementHandler* stmt_handler, stmt_handlers_)
        {
            StmtPairs result = stmt_handler->handleStatement(stmt);
            output.insert(output.end(), result.begin(), result.end());
        }
        return output;
    }

    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);

public:
    EventHandler(SgFunctionDeclaration* func_decl)
    : event_(func_decl) {}

    void addExpressionHandler(ExpressionHandler* exp_handler)
    {
        exp_handlers_.push_back(exp_handler);
    }

    void addStatementHandler(StatementHandler* stmt_handler)
    {
        stmt_handlers_.push_back(stmt_handler);
    }

    std::vector<FuncDeclPair> handleEvent()
    {
        SgBasicBlock* body = event_->get_definition()->get_body();
        StmtPairs outputs;

        static int ctr = 0;

        StmtPairs bodies = handleStatement(body);
        foreach (StmtPair stmt_pair, bodies)
        {
            SgStatement *fwd_body, *rvs_body;
            tie(fwd_body, rvs_body) = stmt_pair;

            string ctr_str = lexical_cast<string > (ctr++);

            SgName fwd_func_name = event_->get_name() + "_forward" + ctr_str;
            SgFunctionDeclaration* fwd_func_decl =
                    SageBuilder::buildDefiningFunctionDeclaration(
                        fwd_func_name, event_->get_orig_return_type(),
                    isSgFunctionParameterList(
                        SageInterface::copyStatement(event_->get_parameterList())));
            SgFunctionDefinition* fwd_func_def = fwd_func_decl->get_definition();
            fwd_func_def->set_body(isSgBasicBlock(fwd_body));
            fwd_body->set_parent(fwd_func_def);

            SgName rvs_func_name = event_->get_name() + "_reverse" + ctr_str;
            SgFunctionDeclaration* rvs_func_decl =
                    SageBuilder::buildDefiningFunctionDeclaration(
                        rvs_func_name, event_->get_orig_return_type(),
                    isSgFunctionParameterList(
                        SageInterface::copyStatement(event_->get_parameterList())));
            SgFunctionDefinition* rvs_func_def = rvs_func_decl->get_definition();
            rvs_func_def->set_body(isSgBasicBlock(rvs_body));
            rvs_body->set_parent(rvs_func_def);

            outputs.push_back(StmtPair(fwd_func_decl, rvs_func_decl));
        }

        return outputs;
    }


};

class ExpressionHandler
{
    EventHandler* event_handler_;

protected:
    SgExpression* pushVal(SgExpression* exp, SgType* type)
    {
        return event_handler_->pushVal(exp, type);
    }

    SgExpression* popVal(SgType* type)
    {
        return event_handler_->popVal(type);
    }

public:
    ExpressionHandler(EventHandler* handler)
    : event_handler_(handler) {}


    virtual ExpPairs handleExpression(SgExpression* exp) = 0;
    //virtual void getCost() = 0;

};


class StatementHandler
{
    EventHandler* event_handler_;

protected:
    ExpPairs handleExpression(SgExpression* exp)
    {
        return event_handler_->handleExpression(exp);
    }

    SgExpression* pushVal(SgExpression* exp, SgType* type)
    {
        return event_handler_->pushVal(exp, type);
    }

    SgExpression* popVal(SgType* type)
    {
        return event_handler_->popVal(type);
    }

public:
    StatementHandler(EventHandler* handler)
    : event_handler_(handler) {}

    virtual StmtPairs handleStatement(SgStatement* stmt) = 0;
    //virtual void getCost() = 0;
};
