#ifndef BACKSTROKE_EVENTPROCESSOR_H
#define	BACKSTROKE_EVENTPROCESSOR_H

#include <rose.h>
#include <utilities/types.h>
#include "variableVersionTable.h"


struct ExpressionObject
{
    ExpressionObject(SgExpression* exp1, SgExpression* exp2, const VariableVersionTable& table)
            : fwd_exp(exp1), rvs_exp(exp2), var_table(table)
    {}
    
    SgExpression* fwd_exp;
    SgExpression* rvs_exp;
    
    VariableVersionTable var_table;
    // Cost Model;
    // Symbol Table;
};

struct StatementObject
{
    StatementObject(SgStatement* stmt1, SgStatement* stmt2, const VariableVersionTable& table)
            : fwd_stmt(stmt1), rvs_stmt(stmt2), var_table(table)
    {}
    
    SgStatement* fwd_stmt;
    SgStatement* rvs_stmt;

    VariableVersionTable var_table;
    // Cost Model;
    // Symbol Table;
};

typedef std::vector<ExpressionObject> ExpressionObjectVec;
typedef std::vector<StatementObject> StatementObjectVec;

// Forward declaration of the class EventProcessor.
class EventProcessor;

class ExpressionProcessor
{
    EventProcessor* event_processor_;

protected:

    ExpressionObjectVec processExpression(SgExpression* exp, const VariableVersionTable& var_table);

    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);

public:

    ExpressionProcessor()
    : event_processor_(NULL) {}

    void setEventProcessor(EventProcessor* processor)
    {
        event_processor_ = processor;
    }

    virtual ExpressionObjectVec process(SgExpression* exp, const VariableVersionTable& var_table) = 0;
    //virtual void getCost() = 0;

};


class StatementProcessor
{
    EventProcessor* event_processor_;

protected:

    ExpressionObjectVec processExpression(SgExpression* exp, const VariableVersionTable& var_table);
    StatementObjectVec processStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);

public:

    StatementProcessor()
    : event_processor_(NULL) {}

    void setEventProcessor(EventProcessor* processor)
    {
        event_processor_ = processor;
    }

    virtual StatementObjectVec process(SgStatement* stmt, const VariableVersionTable& var_table) = 0;

    //virtual S

    //virtual void getCost() = 0;
};


class VariableRenaming;

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

    //! The variable renaming analysis object.
    VariableRenaming* var_renaming_;

    //! The variable version table which record final version of all variables in the event.

    //! Make those two classes the friends to let them use some private methods.
    friend class ExpressionProcessor;
    friend class StatementProcessor;

private:

    //! Given an expression, return all transformations using all expression processors.
    ExpressionObjectVec processExpression(SgExpression* exp, const VariableVersionTable& var_table);

    //! Given a statement, return all transformations using all statement processors.
    StatementObjectVec processStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    //! The following methods are for expression and statement processors for store and restore.
    SgExpression* getStackVar(SgType* type);
    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);


public:
    
    EventProcessor(SgFunctionDeclaration* func_decl = NULL, VariableRenaming* var_renaming = NULL)
    : event_(func_decl), var_renaming_(var_renaming) {}

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
    std::vector<SgVariableDeclaration*> getAllStackDeclarations() const;
};


#endif	/* BACKSTROKE_EVENTPROCESSOR_H */

