#ifndef REVERSE_COMPUTATION_STATE_SAVER_H
#define REVERSE_COMPUTATION_STATE_SAVER_H

#include <rose.h>
#include <boost/tuple/tuple.hpp>

class StateSaver
{
    std::string event_name_;
    SgFunctionDeclaration* event_;
    SgInitializedName* model_obj_;
    SgClassDeclaration* storage_decl_;
    std::string storage_name_;
    SgVariableDeclaration* storage_obj_;
    SgFunctionDeclaration* state_saving_func_;
    SgFunctionDeclaration* rvs_event_;

public:
    StateSaver(SgFunctionDeclaration* func, SgInitializedName* model)
        : event_(func), model_obj_(model)
    {}

    boost::tuple<SgClassDeclaration*,
        SgVariableDeclaration*,
        SgFunctionDeclaration*,
        SgFunctionDeclaration*> 
    getOutput() const 
    { return boost::make_tuple(storage_decl_, storage_obj_, state_saving_func_, rvs_event_); }

    //typedef bool (*IsStateFunc)(SgExpression*);

    std::vector<SgExpression*> getAllModifiedVars();//IsStateFunc pred)
  
    void buildStorage();
    void buildStateSavingFunction();
    void buildReverseEvent();

    void storeVariable(SgExpression* var)
    {
        //SgType* var_type = var->get_type();
    }
};

#endif
