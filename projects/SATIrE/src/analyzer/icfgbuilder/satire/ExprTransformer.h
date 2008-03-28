// Copyright 2005,2006,2007,2008 Markus Schordan, Gergo Barany
// $Id: ExprTransformer.h,v 1.5 2008-03-28 15:55:39 gergo Exp $

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

    std::string find_mangled_func_name(SgFunctionRefExp *) const;
  //SgName find_mangled_memberf_name(SgMemberFunctionRefExp *) const;
    CallBlock *find_entry(SgFunctionCallExp *);
    const std::vector<CallBlock *> *find_entries(SgFunctionCallExp *);
    const std::vector<CallBlock *> *find_destructor_entries(SgClassType *);
    std::vector<SgVariableSymbol *> *evaluate_arguments(std::string,
            SgExpressionPtrList &, BasicBlock *, bool);
    void assign_retval(std::string, SgFunctionCallExp *, BasicBlock *);
    std::vector<std::string> *find_destructor_names(SgClassType *);
    std::vector<std::string> *find_destructor_this_names(SgClassType *);
};

// GB (2008-03-10): Added this function as wrapper around ROSE's
// replaceChild. The problem with that function is that it tries to replace
// the originalExpressionTrees in SgValueExps, but that action is not
// implemented in those classes. This wrapper does nothing in such cases and
// simply calls replaceChild otherwise.
void satireReplaceChild(SgNode *parent, SgNode *from, SgNode *to);

#endif
