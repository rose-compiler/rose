#ifndef BACKSTROKE_EVENTPROCESSOR_H
#define	BACKSTROKE_EVENTPROCESSOR_H

#include <rose.h>
#include <utilities/types.h>
#include "variableVersionTable.h"
#include "costModel.h"
#include "VariableRenaming.h"

/** Stores the results of reversing an expression. These include the forward and reverse expressions,
  * along with the cost of the reversal. */
struct ExpressionReversal
{
    ExpressionReversal() {}
    ExpressionReversal(
        SgExpression* exp1,
        SgExpression* exp2,
        const VariableVersionTable& table,
        const SimpleCostModel& cst = SimpleCostModel())
            : fwd_exp(exp1), rvs_exp(exp2), var_table(table), cost(cst)
    {}

    ExpressionReversal clone()
    {
        return ExpressionReversal(
                SageInterface::copyExpression(fwd_exp),
                SageInterface::copyExpression(rvs_exp),
                var_table, cost);
    }
    
    SgExpression* fwd_exp;
    SgExpression* rvs_exp;
    
    // Variable version table
    VariableVersionTable var_table;
    // Cost model
    SimpleCostModel cost;
};

/** Stores the result of reversing a statement. These include the instrumented forward statement
 * as well as the reverse statement and cost of the reversal. */
struct StatementReversal
{
    StatementReversal() {}
    StatementReversal(
        SgStatement* stmt1,
        SgStatement* stmt2,
        const VariableVersionTable& table,
        const SimpleCostModel& cst = SimpleCostModel())
            : fwd_stmt(stmt1), rvs_stmt(stmt2), var_table(table), cost(cst)
    {}

    StatementReversal clone()
    {
        return StatementReversal(
                SageInterface::copyStatement(fwd_stmt),
                SageInterface::copyStatement(rvs_stmt),
                var_table, cost);
    }
    
    SgStatement* fwd_stmt;
    SgStatement* rvs_stmt;

    // Variable version table
    VariableVersionTable var_table;
    // Cost model;
    SimpleCostModel cost;
};

//! Comparison functions for structure ExpressionReversal and StatementReversal.
inline bool operator < (const ExpressionReversal& e1, const ExpressionReversal& e2)
{ return e1.cost.getCost() < e2.cost.getCost(); }

//! Comparison functions for structure ExpressionReversal and StatementReversal.
inline bool operator < (const StatementReversal& s1, const StatementReversal& s2)
{ return s1.cost.getCost() < s2.cost.getCost(); }

typedef std::vector<ExpressionReversal> ExpressionReversalVec;
typedef std::vector<StatementReversal> StatementReversalVec;


// Forward declaration of the class EventProcessor.
class EventProcessor;

class ProcessorBase
{
    EventProcessor* event_processor_;

protected:

    ExpressionReversalVec processExpression(SgExpression* exp, const VariableVersionTable& table, bool isReverseValueUsed);
    StatementReversalVec processStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    /**
     * Given a variable and a version, returns an expression evaluating to the value of the variable
     * at the given version.
     *
     * @param variable name of the variable to be restored
     * @param availableVariables variables whos values are currently available
     * @return definitions the version of the variable which should be restored
     */
    std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
                                               VariableRenaming::NumNodeRenameEntry definitions);

    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);

    //! Return if the given variable is a state variable (currently, it should be the parameter of event function).
    bool isStateVariable(SgExpression* exp);

    VariableRenaming* getVariableRenaming();

public:

    ProcessorBase() : event_processor_(NULL) {}
    
    void setEventProcessor(EventProcessor* processor)
    {
        event_processor_ = processor;
    }
};

class ExpressionProcessor : public ProcessorBase
{
public:

    virtual ExpressionReversalVec process(SgExpression* exp, const VariableVersionTable& table, bool isReverseValueUsed) = 0;

};


class StatementProcessor : public ProcessorBase
{
public:

    virtual StatementReversalVec process(SgStatement* stmt, const VariableVersionTable& var_table) = 0;
};


/** These types of reverse handlers recalculate a specific value of a variable at a different point
  * in the program. */
class VariableValueRestorer
{
public:

    /**
     * Given a variable and a version, returns an expression evaluating to the value of the variable
     * at the given version.
     *
     * @param variable name of the variable to be restored
     * @param availableVariables variables whos values are currently available
     * @return definitions the version of the variable which should be restored
     */
    virtual std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
                                                       VariableRenaming::NumNodeRenameEntry definitions) = 0;

    VariableValueRestorer() : eventProcessor(NULL)
    {
    }

    void setEventProcessor(EventProcessor* eventProcessor)
    {
        this->eventProcessor = eventProcessor;
    }

    EventProcessor* getEventProcessor()
    {
        return eventProcessor;
    }

private:

    EventProcessor* eventProcessor;
};

class EventProcessor
{
    //! This is the current event function to handle.
    SgFunctionDeclaration* event_;

    //! All expression processors which are added by the user.
    std::vector<ExpressionProcessor*> exp_processors_;

    //! All statement processors which are added by the user.
    std::vector<StatementProcessor*> stmt_processors_;

    /** Handlers which can restore a variable value without state saving. */
    std::vector<VariableValueRestorer*> variableValueRestorers;

    //! All declarations of stacks which store values of different types.
    std::map<std::string, SgVariableDeclaration*> stack_decls_;

    //! The variable renaming analysis object.
    VariableRenaming* var_renaming_;

    //! The variable version table which record final version of all variables in the event.

    //! Make those two classes the friends to let them use some private methods.
    friend class ProcessorBase;

private:

    //! Given an expression, return all transformations using all expression processors.
    ExpressionReversalVec processExpression(SgExpression* exp, const VariableVersionTable& table, bool isReverseValueUsed);

    //! Given a statement, return all transformations using all statement processors.
    StatementReversalVec processStatement(SgStatement* stmt, const VariableVersionTable& var_table);

    //! The following methods are for expression and statement processors for store and restore.
    SgExpression* getStackVar(SgType* type);
    SgExpression* pushVal(SgExpression* exp, SgType* type);
    SgExpression* popVal(SgType* type);


public:
    
    EventProcessor(SgFunctionDeclaration* func_decl = NULL, VariableRenaming* var_renaming = NULL)
    : event_(func_decl), var_renaming_(var_renaming) {}

    /** Add a new expression processor to the pool. Expression processor objects can only be used
     * with one event processor at a time. */
    void addExpressionProcessor(ExpressionProcessor* exp_processor)
    {
        exp_processor->setEventProcessor(this);
        exp_processors_.push_back(exp_processor);
    }

    /** Add a new statement processor to the pool. Statement processor objects can only be used with one
     * event processor at a time. */
    void addStatementProcessor(StatementProcessor* stmt_processor)
    {
        stmt_processor->setEventProcessor(this);
        stmt_processors_.push_back(stmt_processor);
    }

    void addVariableValueRestorer(VariableValueRestorer* restorer)
    {
        restorer->setEventProcessor(this);
        variableValueRestorers.push_back(restorer);
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

    VariableRenaming* getVariableRenaming()
    {
        return var_renaming_;
    }

    /**
     * Given a variable and a version, returns an expression evaluating to the value of the variable
     * at the given version.
     *
     * @param variable name of the variable to be restored
     * @param availableVariables variables whos values are currently available
     * @return definitions the version of the variable which should be restored
     */
    std::vector<SgExpression*> restoreVariable(VariableRenaming::VarName variable, const VariableVersionTable& availableVariables,
                                               VariableRenaming::NumNodeRenameEntry definitions);
};


#endif	/* BACKSTROKE_EVENTPROCESSOR_H */

