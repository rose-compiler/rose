// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: CFGTraversal.h,v 1.15 2009-02-11 10:03:44 gergo Exp $

#ifndef H_CFGTRAVERSAL
#define H_CFGTRAVERSAL

#include <satire_rose.h>

#include <map>

#include "cfg_support.h"
#include "ProcTraversal.h"

// this must be used preorder
class CFGTraversal : public AstSimpleProcessing
{
public:
 // CFGTraversal(std::deque<Procedure *> *);
    CFGTraversal(ProcTraversal &, AnalyzerOptions *options = NULL);
    CFG *getCFG();

 // Can be called before the traversal to choose whether the expressions and
 // types in the program should be numbered. Default is yes. If the
 // expressions are not numbered, but an analysis tries to access them, no
 // attempt is made to fail gracefully (i.e., you will get a segfault).
    void numberExpressions(bool flag);

protected:
    void visit(SgNode *);
    void atTraversalStart();
    void atTraversalEnd();

private:
    CFGTraversal();
 // BasicBlock *transform_block(SgBasicBlock *block, BasicBlock *after,
    BasicBlock *transform_block(SgStatement *ast_statement, BasicBlock *after,
            BasicBlock *break_target, BasicBlock *continue_target,
            BasicBlock *enclosing_switch);
#if 0
 // GB (2008-08-22): Deprecated this function.
    BlockList *do_switch_body(SgBasicBlock *block, BasicBlock *after,
            BasicBlock *continue_target);
#endif
    int find_procnum(std::string funcname) const;
    BasicBlock *allocate_new_block(BasicBlock *, BasicBlock *);
    BasicBlock *allocate_block_without_successor(BasicBlock *);
    SgStatement *rewrite_statement(const SgStatement *,
            std::vector<SgVariableSymbol *> *);
    void perform_goto_backpatching();
 // GB (2008-05-30): Unused method, commented it out.
 // void kill_unreachable_nodes();
    void number_exprs();
    bool is_destructor_decl(SgFunctionDeclaration *) const;
    BasicBlock *call_base_destructors(Procedure *, BasicBlock *);
    void setProcedureEndNodes();
    void processProcedureArgBlocks();
    void processGlobalVariableDeclarations(SgGlobal*);
    void processFunctionDeclarations(SgFunctionDeclaration *decl);
    std::pair<BasicBlock *, BasicBlock *>
        introduceUndeclareStatements(SgBasicBlock *block, BasicBlock *after);
    int node_id;
    int procnum;
    CFG *cfg, *real_cfg;
    Procedure *proc;
    int call_num;
    int lognum;
    int expnum;
    SgStatement *current_statement;
    TimingPerformance *traversalTimer;

    bool flag_numberExpressions;
};

// GB (2008-04-04): This class can be used to traverse the SATIrE ICFG. The
// design is similar to the ROSE AST traversals: You subclass this class and
// override the icfgVisit method; if you wish, you can also override
// atIcfgTraversalStart and atIcfgTraversalEnd. The traversal is started
// using the traverse method. There is no support for passing attributes.
// Right now, this traversal proceeds as follows:
//  - visit each initializer expression for global variables in no
//    particular order
//  - visit each statement in the ICFG in no particular order
// The traversal does NOT DESCEND into expressions or statements. If that is
// what you want, you will need to implement an additional traversal that
// you start at appropriate times.
// Variable symbols are not visited; this is because the ROSE traversal
// doesn't seem to visit them either.
class IcfgTraversal
{
public:
 // Start the traversal.
    void traverse(CFG *icfg);

    virtual ~IcfgTraversal();

protected:
 // This mirrors the ROSE AST traversal interface.
    virtual void atIcfgTraversalStart();
    virtual void icfgVisit(SgNode *node) = 0;
    virtual void atIcfgTraversalEnd();

 // This method can be called from within the traversal to get a pointer to
 // the ICFG being traversed.
    CFG *get_icfg() const;
 // These methods can be called from within the traversal to find out where
 // it is at the moment. is_icfg_statement returns true iff the node that is
 // being visited is a statement in the ICFG (as opposed to an initializer
 // expression, for instance).
    bool is_icfg_statement() const;
 // The following methods may only be called when is_icfg_statement returned
 // true. They return the current basic block's number, its type, the number
 // of the procedure that contains it, and the index of the statent in the
 // basic block, respectively. Since we currently use single-statement basic
 // blocks, the index will always be 0.
 // One might argue that all of this information should be passed as
 // arguments to the visit function. But: I don't know whether we will
 // add more such information, and I don't know who will need what. Which
 // leads us to Epigram 11: "If you have a procedure with 10 parameters, you
 // probably missed some." (Perlis, A. J. 1982. Special Feature: Epigrams on
 // programming. SIGPLAN Not. 17, 9 (Sep. 1982), 7-13. DOI=
 // http://doi.acm.org/10.1145/947955.1083808)
    int get_node_id() const;
    KFG_NODE_TYPE get_node_type() const;
    int get_node_procnum() const;
    int get_statement_index() const;

private:
    CFG *icfg;
    bool icfg_statement;
    int node_id;
    KFG_NODE_TYPE node_type;
    int node_procnum;
    int statement_index;
};

// GB (2008-11-14): This class is responsible for resolving call targets for
// external calls according to the results of points-to analysis.
class IcfgExternalCallResolver: private IcfgTraversal
{
public:
    void run(CFG *icfg);

private:
    void atTraversalStart();
    void icfgVisit(SgNode *node);
    void atTraversalEnd();

    TimingPerformance *timer;
};

#endif
