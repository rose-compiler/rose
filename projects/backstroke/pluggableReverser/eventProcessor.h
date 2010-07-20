#ifndef BACKSTROKE_EVENTPROCESSOR_H
#define	BACKSTROKE_EVENTPROCESSOR_H

#include <rose.h>
#include <utilities/types.h>


// Forward declaration of the class EventProcessor.
class EventProcessor;

class ExpressionProcessor
{
    EventProcessor* event_processor_;

protected:

    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);

public:

    ExpressionProcessor()
    : event_processor_(NULL) {}

    void setEventProcessor(EventProcessor* processor)
    {
        event_processor_ = processor;
    }

    virtual ExpPairs process(SgExpression* exp) = 0;
    //virtual void getCost() = 0;

};


class StatementProcessor
{
    EventProcessor* event_processor_;

protected:

    ExpPairs processExpression(SgExpression* exp);
    StmtPairs processStatement(SgStatement* stmt);

    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);

public:

    StatementProcessor()
    : event_processor_(NULL) {}

    void setEventProcessor(EventProcessor* processor)
    {
        event_processor_ = processor;
    }

    virtual StmtPairs process(SgStatement* stmt) = 0;
    //virtual void getCost() = 0;
};



class EventProcessor
{
    //! This is the current event function to handle.
    SgFunctionDeclaration* event_;

    //! All expression processors which are added by the user.
    std::vector<ExpressionProcessor*> exp_processors_;

    //! All statement processors which are added by the user.
    std::vector<StatementProcessor*> stmt_processors_;

    //! All declarations of stacks which store values of different types.
    std::map<std::string, SgVariableDeclaration*> stack_decls_;

    //! Make those two classes the friends to let them use some private methods.
    friend class ExpressionProcessor;
    friend class StatementProcessor;

private:

    //! Given an expression, return all transformations using all expression processors.
    ExpPairs processExpression(SgExpression* exp);

    //! Given a statement, return all transformations using all statement processors.
    StmtPairs processStatement(SgStatement* stmt);

    //! The following methods are for expression and statement processors for store and restore.
    SgExpression* getStackVar(SgType* type);
    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);


public:
    
    EventProcessor(SgFunctionDeclaration* func_decl = NULL)
    : event_(func_decl) {}

    void addExpressionProcessor(ExpressionProcessor* exp_processor)
    {
        exp_processor->setEventProcessor(this);
        exp_processors_.push_back(exp_processor);
    }

    void addStatementProcessor(StatementProcessor* stmt_processor)
    {
        stmt_processor->setEventProcessor(this);
        stmt_processors_.push_back(stmt_processor);
    }

    FuncDeclPairs processEvent();

    FuncDeclPairs processEvent(SgFunctionDeclaration* event)
    {
        event_ = event;
        //stack_decls_.clear();
        return processEvent();
    }

    //! Get all declarations of stacks which store values of different types.
    std::vector<SgVariableDeclaration*> getAllStackDeclarations();
};


#endif	/* BACKSTROKE_EVENTPROCESSOR_H */

