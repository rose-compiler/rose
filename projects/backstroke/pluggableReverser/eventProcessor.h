#ifndef BACKSTROKE_EVENTPROCESSOR_H
#define	BACKSTROKE_EVENTPROCESSOR_H

#include <rose.h>
#include <utilities/types.h>
#include "variableVersionTable.h"


struct ExpressionObject
{
    ExpressionObject() {}
    ExpressionObject(SgExpression* exp1, SgExpression* exp2, const VariableVersionTable& table)
            : fwd_exp(exp1), rvs_exp(exp2), var_table(table)
    {}

    ExpressionObject clone()
    {
        ExpressionObject obj;
        obj.fwd_exp = SageInterface::copyExpression(fwd_exp);
        obj.rvs_exp = SageInterface::copyExpression(rvs_exp);
        obj.var_table = var_table;
        return obj;
    }
    
    SgExpression* fwd_exp;
    SgExpression* rvs_exp;
    
    VariableVersionTable var_table;
    // Cost Model;
    // Symbol Table;
};

struct StatementObject
{
    StatementObject() {}
    StatementObject(SgStatement* stmt1, SgStatement* stmt2, const VariableVersionTable& table)
            : fwd_stmt(stmt1), rvs_stmt(stmt2), var_table(table)
    {}

    StatementObject clone()
    {
        StatementObject obj;
        obj.fwd_stmt = SageInterface::copyStatement(fwd_stmt);
        obj.rvs_stmt = SageInterface::copyStatement(rvs_stmt);
        obj.var_table = var_table;
        return obj;
    }
    
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

class ProcessorBasis
{
    EventProcessor* event_processor_;

protected:

    ExpressionObjectVec processExpression(SgExpression* exp, const VariableVersionTable& var_table);
    StatementObjectVec processStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);

    //! Return if the given variable is a state variable (currently, it should be the parameter of event function).
    bool isStateVariable(SgExpression* exp);

public:

    ProcessorBasis() : event_processor_(NULL) {}
    
    void setEventProcessor(EventProcessor* processor)
    {
        event_processor_ = processor;
    }
};

class ExpressionProcessor : public ProcessorBasis
{
public:

    virtual ExpressionObjectVec process(SgExpression* exp, const VariableVersionTable& var_table) = 0;
    //virtual void getCost() = 0;

};


class StatementProcessor : public ProcessorBasis
{
public:

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
    friend class ProcessorBasis;

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

    //! Return if the given variable is a state variable (currently, it should be the parameter of event function).
    bool isStateVariable(SgExpression* exp);

    //! Get all declarations of stacks which store values of different types.
    std::vector<SgVariableDeclaration*> getAllStackDeclarations() const;
};


#endif	/* BACKSTROKE_EVENTPROCESSOR_H */

