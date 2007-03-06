// Copyright 2005,2006,2007,2008 Markus Schordan, Gergo Barany
// $Id: ExprTransformer.h,v 1.10 2009-02-11 10:03:44 gergo Exp $

#ifndef H_EXPRTRANSFORMER
#define H_EXPRTRANSFORMER

#include <satire_rose.h>

#include <list>

#include "cfg_support.h"
#include "ExprLabeler.h"
#include "analysis_info.h"

// this must be used postorder
class ExprTransformer : public AstSimpleProcessing
{
public:
#if 0
    ExprTransformer(int node_id_, int procnum_, int expnum_, CFG *cfg_,
            BasicBlock *after);
#endif
    int get_node_id() const;
    int get_expnum() const;
    BasicBlock *get_after() const;
    BasicBlock *get_last() const;
    BasicBlock *get_retval() const;
    SgVariableSymbol *get_root_var() const;
 // GB (2008-05-05): New interface to roll expression labeling and numbering
 // into one operation.
    ExprTransformer(int node_id, int procnum, int expnum, CFG *cfg,
            BasicBlock *after, SgStatement *stmt);
    SgExpression *labelAndTransformExpression(SgExpression *expr);

protected:
    void visit(SgNode *);

private:
    int node_id;
    int procnum;
    int expnum;
    CFG *cfg;
    BasicBlock *after, *last;
    BasicBlock *retval;
    SgVariableSymbol *root_var;
    SgStatement *stmt;
    ExprLabeler el;
    StatementAttribute *stmt_start, *stmt_end;

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
    bool definitelyNotTheSameType(SgType*, SgType*) const;
};

// GB (2008-03-10): Added this function as wrapper around ROSE's
// replaceChild. The problem with that function is that it tries to replace
// the originalExpressionTrees in SgValueExps, but that action is not
// implemented in those classes. This wrapper does nothing in such cases and
// simply calls replaceChild otherwise.
void satireReplaceChild(SgNode *parent, SgNode *from, SgNode *to);

#endif
