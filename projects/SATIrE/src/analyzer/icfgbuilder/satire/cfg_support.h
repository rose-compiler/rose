// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: cfg_support.h,v 1.32 2009-02-11 10:03:44 gergo Exp $

#ifndef H_CFG_SUPPORT
#define H_CFG_SUPPORT

#include <utility>
#include <deque>
#include <typeinfo>

#include <cassert>
#include "AttributeMechanism.h"

// #include <config.h>

#include "kfg_types.h"
// #include "iface.h"

#include "EqualityTraversal.h"
#include "AnalyzerOptions.h"

enum
{
    X_FunctionCall = CALL,
    X_FunctionReturn = RETURN,
    X_FunctionEntry = START,
    X_FunctionExit = END
};

class BasicBlock;
class Procedure;
class CFG;

// GB (2008-11-11): Including "pointsto.h" here doesn't work; it leads to
// cycles and keeps certain type declarations from being seen. However, we
// don't really need anything here except the name of the PointsToAnalysis
// class to declare a pointer member in CFG. So we declare it directly.
// #include "pointsto.h"
namespace SATIrE { namespace Analyses { class PointsToAnalysis; } }

typedef std::pair<BasicBlock *, KFG_EDGE_TYPE> Edge;
// typedef std::deque<BasicBlock *> BlockList;
// GB (2008-05-30): BlockList is now a vector; this makes it easier to
// traverse it using pointers. All BlockLists must be NULL-terminated.
typedef std::vector<BasicBlock *> BlockList;

#if 0
// GB (2008-04-01): Replaced the typedef by a more complex class. The idea
// is to ease garbage collection by registering pointers to all instances of
// BlockListIterator in the corresponding CFG, and freeing them when the CFG
// is destructed.
// typedef std::pair<BlockList *, BlockList::iterator> BlockListIterator;
class BlockListIterator
{
public:
    enum DeletionFlag { DELETE_LIST, NO_DELETE_LIST };

    BlockListIterator(CFG *cfg, BlockList *blocks,
                      DeletionFlag deletionFlag = NO_DELETE_LIST);
    ~BlockListIterator();

    BasicBlock *head() const;
    BlockListIterator *tail() const;
    bool empty() const;
    int size() const;

private:
    CFG *cfg;
    BlockList *blocks;
    BlockList::iterator pos;
    DeletionFlag deletionFlag;
    BlockListIterator(CFG *cfg, BlockList *blocks,
                      BlockList::iterator pos,
                      DeletionFlag deletionFlag = NO_DELETE_LIST);
};
#endif

const char *expr_to_string(const SgExpression *);

class ExprPtrComparator
{
public:
    bool operator()(const SgExpression *a, const SgExpression *b) const;
};

class TypePtrComparator
{
public:
    bool operator()(SgType *a, SgType *b) const;
};

class CFG
{
public:
    BlockList nodes;
    BlockList calls, entries, exits, returns;
    std::deque<Procedure *> *procedures;
    std::multimap<std::string, Procedure *> proc_map;
    std::multimap<std::string, Procedure *> mangled_proc_map;
 // mappings from numbers to expressions and back
    std::vector<SgExpression *> numbers_exprs;
    std::map<SgExpression *, unsigned long> exprs_numbers;
 // mappings from numbers to types and back
    std::vector<SgType *> numbers_types;
    std::map<SgType *, unsigned long, TypePtrComparator> types_numbers;
 // mappings from numbers to variables and back; note that ids_varsyms
 // cannot be a vector because the ids are not consecutive
    std::map<SgVariableSymbol *, unsigned long> varsyms_ids;
    std::map<unsigned long, SgVariableSymbol *> ids_varsyms;

    std::map<std::string, SgVariableSymbol *> names_globals;
    std::map<std::string, SgExpression *> names_initializers;
    std::vector<SgVariableSymbol *> globals;
    std::map<SgVariableSymbol *, SgExpression *> globals_initializers;

 // access to statement labels
    bool statementHasLabels(SgStatement *);
    std::pair<int, int> statementEntryExitLabels(SgStatement *);
    std::set<int> statementAllLabels(SgStatement *);
    void registerStatementLabel(int label, SgStatement *stmt);
 // debugging
    void print_map() const;

 // a single symbol for all return variables in the program
    SgVariableSymbol *global_return_variable_symbol;
 // a single set of argument variables for all function calls
    std::vector<SgVariableSymbol *> global_argument_variable_symbols;
 // a single symbol for all "this" return variables and arguments
    SgVariableSymbol *global_this_variable_symbol;
 // the type to assign to these global variables
    SgType *global_unknown_type;

    EqualityTraversal equalityTraversal;

    AnalyzerOptions *analyzerOptions;

    SATIrE::Analyses::PointsToAnalysis *pointsToAnalysis;

    ~CFG();

#if 0
 // This method is used to register all dynamically allocated
 // BlockListIterators. They can be freed when the CFG is destructed.
    void add_iteratorToDelete(BlockListIterator *i);
#endif
 // This method duplicates a C string. It keeps the allocated memory blocks
 // in a list and frees them when the CFG itself is destructed.
    char *dupstr(const char *str);

private:
#if 0
    std::vector<BlockListIterator *> iteratorsToDelete;
#endif
    std::vector<char *> cStringsToDelete;

    std::map<int, SgStatement *> block_stmt_map;
    std::map<SgStatement *, std::set<int> > stmt_blocks_map;
};

class BasicBlock
{
public:
    BasicBlock(KFG_NODE_ID id_, KFG_NODE_TYPE type_, int procnum_)
        : id(id_), node_type(type_), procnum(procnum_), call_target(NULL),
          call_index(0), reachable(true), in_edge_mask(-1), out_edge_mask(-1)
    {
    }

    KFG_NODE_ID id;
    KFG_NODE_TYPE node_type;
    int procnum;
    std::deque<SgStatement *> statements;
    std::vector<Edge> successors;
    std::vector<Edge> predecessors;
 // GB (2008-10-16): Recording which function call, if any, this block was
 // created for. This is the expression that identifies the function being
 // called (NULL if this block did not arise from a function call).
 // Additionally, for argument and param assignments (at least), the
 // call_index numbers the nodes from 0 to n so we know which param is
 // affected by each such statement.
    SgExpression *call_target;
    int call_index;
 // GB (2008-05-30): Starting a move towards keeping successor/predecessor
 // blocks and the corresponding edges separated. This makes it easier to
 // iterate over block lists using pointers.
    BlockList successor_blocks, predecessor_blocks;
 // GB (2008-03-10): Reachability flag, used for cleaning up the CFG.
    bool reachable;
 // GB (2008-05-26): Store neighbor masks once computed.
    int in_edge_mask, out_edge_mask;

    virtual ~BasicBlock();
};

class CallStmt;

class CallBlock : public BasicBlock
{
public:
    CallBlock(KFG_NODE_ID id_, KFG_NODE_TYPE type_, int procnum_,
            std::vector<SgVariableSymbol *> *paramlist_, std::string name_,
            bool add_call_stmt = true);
    CallBlock *partner;
    std::string print_paramlist() const;
    CallStmt *stmt;

    std::vector<SgVariableSymbol *> *get_params();
    void set_params(std::vector<SgVariableSymbol *> *params);

    virtual ~CallBlock();

protected:
    std::string name;
    std::vector<SgVariableSymbol *> *paramlist;
};

// GB (2008-10-21): Added this enumeration to allow us to switch on ICFG
// statement types just as we can switch on ROSE AST node variants.
enum SatireVariant
{
    V_IcfgStmt,
    V_CallStmt,
    V_FunctionCall,
    V_FunctionReturn,
    V_FunctionEntry,
    V_DeclareStmt,
    V_UndeclareStmt,
    V_ExternalCall,
    V_ExternalReturn,
    V_ConstructorCall,
    V_DestructorCall,
    V_ArgumentAssignment,
    V_MyAssignment,
    V_ReturnAssignment,
    V_ParamAssignment,
    V_LogicalIf,
    V_IfJoin,
    V_WhileJoin,
    V_FunctionExit,
    V_SATIrE_max_variants
};

class IcfgStmt : public SgStatement
{
public:
  // MS: we make this function virtual (although it is not virtual in ROSE)
  virtual std::string unparseToString() const;
  IcfgStmt() {}
// GB (2008-04-07): Implementing traversal interface.
  std::vector<SgNode *> get_traversalSuccessorContainer();
  size_t get_numberOfTraversalSuccessors();
  SgNode *get_traversalSuccessorByIndex(size_t idx);
  std::vector<std::string> get_traversalSuccessorNamesContainer();
// GB (2008-04-07): Implementing is... type tests.
  friend IcfgStmt *isIcfgStmt(SgNode *);
  friend const IcfgStmt *isIcfgStmt(const SgNode *);
  std::string class_name() const;
  virtual SatireVariant satireVariant() const;
};

IcfgStmt *isIcfgStmt(SgNode *);
const IcfgStmt *isIcfgStmt(const SgNode *);

class CallStmt : public IcfgStmt
{
public:
    CallStmt(KFG_NODE_TYPE node_type, std::string name, CallBlock *parent);

    std::string unparseToString() const;
    std::string get_funcname() const;

    KFG_NODE_TYPE type;
    std::string name;
    CallBlock *parent;
    void update_infolabel();

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend CallStmt *isCallStmt(SgNode *);
    friend const CallStmt *isCallStmt(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;

protected:
    std::string infolabel;
};

CallStmt *isCallStmt(SgNode *);
const CallStmt *isCallStmt(const SgNode *);

// typedef CallStmt FunctionCall, FunctionReturn;
// GB (2008-04-07): Replaced the typedefs by a full-fledged implementation.
// This is cleaner when it comes to the traversal and type test stuff.
class FunctionCall: public CallStmt
{
public:
    FunctionCall(KFG_NODE_TYPE node_type, std::string name, CallBlock *parent);

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend FunctionCall *isFunctionCall(SgNode *);
    friend const FunctionCall *isFunctionCall(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;
};

FunctionCall *isFunctionCall(SgNode *);
const FunctionCall *isFunctionCall(const SgNode *);

class FunctionReturn: public CallStmt
{
public:
    FunctionReturn(KFG_NODE_TYPE node_type, std::string name, CallBlock *parent);

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend FunctionReturn *isFunctionReturn(SgNode *);
    friend const FunctionReturn *isFunctionReturn(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;
};

FunctionReturn *isFunctionReturn(SgNode *);
const FunctionReturn *isFunctionReturn(const SgNode *);

class FunctionEntry : public CallStmt
{
public:
    FunctionEntry(KFG_NODE_TYPE type, std::string func, CallBlock *parent)
        : CallStmt(type, func, parent)
    {
    }

    // const char *get_funcname() const { return funcname; }

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend FunctionEntry *isFunctionEntry(SgNode *);
    friend const FunctionEntry *isFunctionEntry(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;

private:
    std::string funcname;
};

FunctionEntry *isFunctionEntry(SgNode *);
const FunctionEntry *isFunctionEntry(const SgNode *);

class DeclareStmt : public IcfgStmt
{
public:
    DeclareStmt(SgVariableSymbol *v, SgType *t)
        : var(v), type(t)
    {
    }

    std::string unparseToString() const;
    SgVariableSymbol *get_var() const { return var; }
    SgType *get_type() const { return type; }

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend DeclareStmt *isDeclareStmt(SgNode *);
    friend const DeclareStmt *isDeclareStmt(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;

protected:
    SgVariableSymbol *var;
    SgType *type;
};

DeclareStmt *isDeclareStmt(SgNode *);
const DeclareStmt *isDeclareStmt(const SgNode *);

class UndeclareStmt : public IcfgStmt
{
public:
    UndeclareStmt(std::vector<SgVariableSymbol *> *v)
        : vars(v)
    {
    }

    std::string unparseToString() const;
    std::vector<SgVariableSymbol *> *get_vars() const { return vars; }
    virtual ~UndeclareStmt();

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend UndeclareStmt *isUndeclareStmt(SgNode *);
    friend const UndeclareStmt *isUndeclareStmt(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;

protected:
    std::vector<SgVariableSymbol *> *vars;
};

UndeclareStmt *isUndeclareStmt(SgNode *);
const UndeclareStmt *isUndeclareStmt(const SgNode *);

class RetvalAttribute : public AstAttribute
{
public:
    RetvalAttribute(SgVariableSymbol *sym) : sym(sym) { }
    SgVariableSymbol *get_variable_symbol() const { return sym; }

private:
 // GB (2008-06-25): Strings are deprecated! We only use variable symbols
 // now. That's more efficient and more correct (variable ids...).
#if 0
    RetvalAttribute(std::string s) : str(s)
    {
    }
    std::string get_str() const
    {
        return str;
    }
    RetvalAttribute();
    std::string str;
#endif
    SgVariableSymbol *sym;
};

// GB (2008-10-17): Annotating statements and expressions in the ICFG that
// were generated for function calls with an attribute that specifies the
// call's target(s).
class CallAttribute: public AstAttribute
{
public:
    CallAttribute(SgExpression *call_target): call_target(call_target) { }
    SgExpression *call_target;
};

// GB (2007-10-23): Added members for the expression that refers to the
// external function and for its parameter list.
// GB (2008-04-23): Added the ExternalReturn class.
class ExternalCall : public IcfgStmt
{
public:
    SgExpression *get_function() const { return function; }
    std::vector<SgVariableSymbol *> *get_params() const { return params; }
    SgType *get_type() const { return type; }

    ExternalCall(SgExpression *function_, std::vector<SgVariableSymbol *> *params_, SgType *type_);

    void set_params(std::vector<SgVariableSymbol *> *params_);
    std::string unparseToString() const;

    ~ExternalCall();
    
 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend ExternalCall *isExternalCall(SgNode *);
    friend const ExternalCall *isExternalCall(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;

private:
    SgExpression *function; // TRAVERSED
    std::vector<SgVariableSymbol *> *params;
    SgType *type;
};

ExternalCall *isExternalCall(SgNode *);
const ExternalCall *isExternalCall(const SgNode *);

class ExternalReturn : public IcfgStmt
{
public:
    SgExpression *get_function() const { return function; }
    std::vector<SgVariableSymbol *> *get_params() const { return params; }
    SgType *get_type() const { return type; }

    ExternalReturn(SgExpression *function_, std::vector<SgVariableSymbol *> *params_, SgType *type_);

    void set_params(std::vector<SgVariableSymbol *> *params_);
    std::string unparseToString() const;

    ~ExternalReturn();
    
 // Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // Implementing is... type tests.
    friend ExternalReturn *isExternalReturn(SgNode *);
    friend const ExternalReturn *isExternalReturn(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;

private:
    SgExpression *function; // TRAVERSED
    std::vector<SgVariableSymbol *> *params;
    SgType *type;
};

ExternalReturn *isExternalReturn(SgNode *);
const ExternalReturn *isExternalReturn(const SgNode *);

class ConstructorCall : public IcfgStmt
{
public:
    std::string get_name() const { return name; }
    SgType *get_type() const { return type; }
    ConstructorCall(std::string name_, SgType *type_)
        : name(name_), type(type_)
    {
    }
    std::string unparseToString() const;

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend ConstructorCall *isConstructorCall(SgNode *);
    friend const ConstructorCall *isConstructorCall(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;

private:
    ConstructorCall();
    std::string name;
    SgType *type;
};

ConstructorCall *isConstructorCall(SgNode *);
const ConstructorCall *isConstructorCall(const SgNode *);

class DestructorCall : public IcfgStmt
{
public:
    std::string get_name() const { return name; }
    SgType *get_type() const { return type; }
    DestructorCall(std::string name_, SgType *type_)
        : name(name_), type(type_)
    {
    }
    std::string unparseToString() const;

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend DestructorCall *isDestructorCall(SgNode *);
    friend const DestructorCall *isDestructorCall(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;

private:
    DestructorCall();
    std::string name;
    SgType *type;
};

DestructorCall *isDestructorCall(SgNode *);
const DestructorCall *isDestructorCall(const SgNode *);

class ArgumentAssignment : public IcfgStmt
{
public:
    ArgumentAssignment(SgVariableSymbol *l, SgExpression *r);
    ArgumentAssignment(SgExpression *l, SgExpression *r);
    SgExpression *get_lhs() const;
    SgExpression *get_rhs() const;
    void set_rhs(SgExpression *r);
    std::string unparseToString() const;

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend ArgumentAssignment *isArgumentAssignment(SgNode *);
    friend const ArgumentAssignment *isArgumentAssignment(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;

private:
    void init(SgExpression *l, SgExpression *r);
    SgExpression *lhs; // TRAVERSED
    SgExpression *rhs; // TRAVERSED
  //ArgumentAssignment();
};

ArgumentAssignment *isArgumentAssignment(SgNode *);
const ArgumentAssignment *isArgumentAssignment(const SgNode *);

class MyAssignment : public IcfgStmt
{
public:
  SgVariableSymbol *get_lhs() const;
  SgVariableSymbol *get_rhs() const;
  MyAssignment(SgVariableSymbol *l, SgVariableSymbol *r);
  SgVarRefExp *get_lhsVarRefExp() const;
  SgVarRefExp *get_rhsVarRefExp() const;
// GB (2008-04-07): Implementing traversal interface.
  std::vector<SgNode *> get_traversalSuccessorContainer();
  size_t get_numberOfTraversalSuccessors();
  SgNode *get_traversalSuccessorByIndex(size_t idx);
  std::vector<std::string> get_traversalSuccessorNamesContainer();
// GB (2008-04-07): Implementing is... type tests.
  friend MyAssignment *isMyAssignment(SgNode *);
  friend const MyAssignment *isMyAssignment(const SgNode *);
  std::string class_name() const;
  virtual SatireVariant satireVariant() const;
  
protected:
  SgVariableSymbol *lhs;
  SgVariableSymbol *rhs;
  SgVarRefExp *lhsVarRefExp; // TRAVERSED
  SgVarRefExp *rhsVarRefExp; // TRAVERSED
};

MyAssignment *isMyAssignment(SgNode *);
const MyAssignment *isMyAssignment(const SgNode *);

class ReturnAssignment : public MyAssignment
{
public:
    ReturnAssignment(SgVariableSymbol *l, SgVariableSymbol *r)
        : MyAssignment(l, r)
    {
    }
    std::string unparseToString() const;

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend ReturnAssignment *isReturnAssignment(SgNode *);
    friend const ReturnAssignment *isReturnAssignment(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;
};

ReturnAssignment *isReturnAssignment(SgNode *);
const ReturnAssignment *isReturnAssignment(const SgNode *);

class ParamAssignment : public MyAssignment
{
public:
    ParamAssignment(SgVariableSymbol *l, SgVariableSymbol *r)
        : MyAssignment(l, r)
    {
    }
    std::string unparseToString() const;

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend ParamAssignment *isParamAssignment(SgNode *);
    friend const ParamAssignment *isParamAssignment(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;
};

ParamAssignment *isParamAssignment(SgNode *);
const ParamAssignment *isParamAssignment(const SgNode *);

class LogicalIf : public IcfgStmt
{
public:
    LogicalIf(SgExpression *e) : expr(e)
    {
     // expr->set_parent(NULL);
    }
    SgExpression *get_condition() const { return expr; }
    std::string unparseToString() const;

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend LogicalIf *isLogicalIf(SgNode *);
    friend const LogicalIf *isLogicalIf(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;

private:
    SgExpression *expr; // TRAVERSED
};

LogicalIf *isLogicalIf(SgNode *);
const LogicalIf *isLogicalIf(const SgNode *);

class IfJoin : public IcfgStmt
{
public:
    std::string unparseToString() const;
    std::string get_funcname() const { return "<none>"; }

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend IfJoin *isIfJoin(SgNode *);
    friend const IfJoin *isIfJoin(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;
};

IfJoin *isIfJoin(SgNode *);
const IfJoin *isIfJoin(const SgNode *);

class WhileJoin : public IcfgStmt
{
public:
    std::string unparseToString() const;
    std::string get_funcname() const { return "<none>"; }

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend WhileJoin *isWhileJoin(SgNode *);
    friend const WhileJoin *isWhileJoin(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;
};

WhileJoin *isWhileJoin(SgNode *);
const WhileJoin *isWhileJoin(const SgNode *);

class FunctionExit : public CallStmt
{
public:
    FunctionExit(KFG_NODE_TYPE type, std::string func, CallBlock *parent)
        : CallStmt(type, func, parent)
    {
    }

    // const char *get_funcname() const { return funcname; }

 // GB (2008-04-07): Implementing traversal interface.
    std::vector<SgNode *> get_traversalSuccessorContainer();
    size_t get_numberOfTraversalSuccessors();
    SgNode *get_traversalSuccessorByIndex(size_t idx);
    std::vector<std::string> get_traversalSuccessorNamesContainer();
 // GB (2008-04-07): Implementing is... type tests.
    friend FunctionExit *isFunctionExit(SgNode *);
    friend const FunctionExit *isFunctionExit(const SgNode *);
    std::string class_name() const;
    virtual SatireVariant satireVariant() const;

private:
    std::string funcname;
};

FunctionExit *isFunctionExit(SgNode *);
const FunctionExit *isFunctionExit(const SgNode *);

class Procedure
{
public:
    Procedure();

    int procnum;
    std::string name, mangled_name, memberf_name, mangled_memberf_name;
    SgClassDefinition *class_type;
    CallBlock *entry;
    CallBlock *exit;
    BasicBlock *arg_block, *first_arg_block, *last_arg_block, *this_assignment;
    SgVariableSymbol *returnvar;
    std::map<std::string, BasicBlock *> goto_targets;
    std::multimap<std::string, BasicBlock *> goto_blocks;
    SgFunctionParameterList *params, *default_params;
    SgFunctionDeclaration *decl;
    SgFunctionSymbol *funcsym;
 // GB (2008-05-26): Type, symbol and representative expression for the this
 // pointer.
    SgType *this_type;
    SgVariableSymbol *this_sym;
    SgVarRefExp *this_exp;

 // GB (2008-07-01): Used for resolution of calls to static functions. For
 // static functions, this points to the file containing the function's
 // definition. It is NULL otherwise.
    SgFile *static_file;

    ~Procedure();
};

void add_link(BasicBlock *from, BasicBlock *to, KFG_EDGE_TYPE type);
SgFunctionRefExp *find_called_func(SgExpression *);
SgMemberFunctionRefExp *find_called_memberfunc(SgExpression *);
SgExpression *calling_object_address(SgExpression *);
std::string *find_func_name(SgFunctionCallExp *);
BasicBlock *call_destructor(SgInitializedName *in, CFG *cfg,
        int procnum, BasicBlock *after, int *node_id);
bool subtype_of(SgClassDefinition *, SgClassDefinition *);
void dumpTreeFragment(SgNode *, std::ostream &);
std::string dumpTreeFragmentToString(SgNode *);

// Helper for printing sub-ASTs in a format like:
// SgAddOp( SgIntVal( ) SgVarRefExp( ) )
class TreeFragmentDumper: private AstPrePostProcessing
{
public:
    TreeFragmentDumper(std::ostream &stream)
      : stream(stream)
    {
    }

    void run(SgNode *n)
    {
        traverse(n);
    }

protected:
    void preOrderVisit(SgNode *n)
    {
        stream << n->class_name() << "( ";
    }

    void postOrderVisit(SgNode *n)
    {
        stream << ") ";
    }

private:
    std::ostream &stream;
};

// GB (2008-05-14): This function is used in some places where we need to
// access data specific to the current ICFG, but where the ICFG is not
// passed in as a parameter (namely, in PAG support functions).
CFG *get_global_cfg();

#endif
