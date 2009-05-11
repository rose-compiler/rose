// Copyright 2005,2006,2007,2008 Markus Schordan, Gergo Barany
// $Id: ExprTransformer.h,v 1.10 2009-02-11 10:03:44 gergo Exp $

#ifndef H_EXPRTRANSFORMER
#define H_EXPRTRANSFORMER

#include <satire_rose.h>

#include <list>

class ExprTransformer;

#include "cfg_support.h"
#include "ExprLabeler.h"
#include "analysis_info.h"

// The ExprInfo class represents an expression in the ICFG. This may be a
// simple expression pointer, but it may also represent a range of ICFG
// nodes that are needed to evaluate the expression (as is the case for
// function calls, for instance).
class ExprInfo
{
public:
 // An expression tree giving the value of the expression represented by
 // this ExprInfo. This may be an original AST fragment, a reference to a
 // temporary variable, or some other transformed expression.
    SgExpression *value;
 // If the expression represented by this ExprInfo is not "pure", i.e., it
 // must be represented by actual code in the form of ICFG statements, then
 // these pointers refer to the entry and exit blocks of that code segment.
 // Otherwise, they are both NULL.
    BasicBlock *entry, *exit;

 // Is this ExprInfo "pure", i.e., are no ICFG nodes needed to evaluate it?
    bool isPure() const;

    ExprInfo(SgExpression *value = NULL);
};

ExprInfo merge(ExprInfo a, ExprInfo b, SgExpression *newValue = NULL);
// Is this needed?
ExprInfo merge(std::vector<ExprInfo>);

class ExprTransformer: public AstBottomUpProcessing<ExprInfo>
{
public:
    int get_node_id() const;
    int get_expnum() const;
    BasicBlock *get_after() const;
    BasicBlock *get_last() const;
    BasicBlock *get_retval() const;

    ExprTransformer(int node_id, int procnum, int expnum, CFG *cfg,
                    BasicBlock *after, SgStatement *stmt);
    SgExpression *transformExpression(SgExpression *expr,
                                      SgExpression *original_expr);

protected:
    ExprInfo defaultSynthesizedAttribute();
    ExprInfo evaluateSynthesizedAttribute(SgNode *node,
                                          SynthesizedAttributesList synList);


private:
 // from old implementation
    int node_id;
    int procnum;
    int expnum;
    CFG *cfg;
    BasicBlock *after, *last;
    BasicBlock *retval;
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

    std::vector<BasicBlock *> callSites;

 // new support stuff
    BasicBlock *newBasicBlock();
    CallBlock *newCallBlock();
    CallBlock *newReturnBlock();
    SgVariableSymbol *newReturnVariable(std::string funcname);
    SgVariableSymbol *newLogicalVariable();
    SgVariableSymbol *icfgArgumentVarSym(unsigned int i);
    SgVarRefExp *icfgArgumentVarRef(unsigned int i);
    BasicBlock *newAssignBlock(SgExpression *lhs, SgExpression *rhs);
    BasicBlock *newArgumentAssignmentBlock(unsigned int i, SgExpression *e);
    std::vector<SgVariableSymbol *> *newArgumentSymbolList(
            SgFunctionCallExp *call);
};


#if OLD_AND_UNUSED
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
    SgExpression *labelAndTransformExpression(SgExpression *expr,
                                              SgExpression *original_expr);

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

    std::vector<BasicBlock *> callSites;
};
#endif

// GB (2008-03-10): Added this function as wrapper around ROSE's
// replaceChild. The problem with that function is that it tries to replace
// the originalExpressionTrees in SgValueExps, but that action is not
// implemented in those classes. This wrapper does nothing in such cases and
// simply calls replaceChild otherwise.
void satireReplaceChild(SgNode *parent, SgNode *from, SgNode *to);

class CallSiteAnnotator: public AstSimpleProcessing
{
public:
    CallSiteAnnotator(std::vector<BasicBlock *> &callSites);

protected:
    void visit(SgNode *);

private:
    std::vector<BasicBlock *> &callSites;
    std::vector<BasicBlock *>::iterator callSite;
};

#endif
