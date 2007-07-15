// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: ExprTransformer.h,v 1.3 2007-07-15 02:02:27 markus Exp $

#ifndef H_EXPRTRANSFORMER
#define H_EXPRTRANSFORMER

#include <config.h>
#include <rose.h>

#include <list>

#include "cfg_support.h"

// this must be used postorder
class ExprTransformer : public AstSimpleProcessing
{
public:
    ExprTransformer(int node_id_, int procnum_, int expnum_, CFG *cfg_,
            BasicBlock *after);
    int get_node_id() const;
    int get_expnum() const;
    BasicBlock *get_after() const;
    BasicBlock *get_last() const;
    BasicBlock *get_retval() const;
    SgVariableSymbol *get_root_var() const;

protected:
    void visit(SgNode *);

private:
    ExprTransformer();
    int node_id;
    int procnum;
    int expnum;
    CFG *cfg;
    BasicBlock *after, *last;
    BasicBlock *retval;
    SgVariableSymbol *root_var;

    SgName find_mangled_func_name(SgFunctionRefExp *) const;
  //SgName find_mangled_memberf_name(SgMemberFunctionRefExp *) const;
    CallBlock *find_entry(SgFunctionCallExp *);
    const std::list<CallBlock *> *find_entries(SgFunctionCallExp *);
    const std::list<CallBlock *> *find_destructor_entries(SgClassType *);
    std::list<SgVariableSymbol *> *evaluate_arguments(SgName,
            SgExpressionPtrList &, BasicBlock *, bool);
    void assign_retval(SgName, SgFunctionCallExp *, BasicBlock *);
    std::list<std::string> *find_destructor_names(SgClassType *);
    std::list<std::string> *find_destructor_this_names(SgClassType *);
};

#endif
