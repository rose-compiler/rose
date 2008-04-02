// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: cfg_support.h,v 1.11 2008-04-02 09:28:58 gergo Exp $

#ifndef H_CFG_SUPPORT
#define H_CFG_SUPPORT

#include <utility>
#include <deque>
#include <typeinfo>

#include "AttributeMechanism.h"

#include "iface.h"

enum edgetypes
{
    LOCAL,
    BB_INTERN,
    TRUE_EDGE,
    FALSE_EDGE,
    NORMAL_EDGE,
    CALL_EDGE,
    RETURN_EDGE,
    //NO_EDGE,
    EDGE_TYPE_MAX /* whatever you do, leave this dummy as the last constant */
};

/* in analogy with:
typedef enum
{
    CALL,
    RETURN,
    START,
    END,
    INNER
} KFG_NODE_TYPE;
*/
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

typedef std::pair<BasicBlock *, KFG_EDGE_TYPE> Edge;
typedef std::deque<BasicBlock *> BlockList;

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
    std::map<int, SgExpression *> numbers_exprs;
    std::map<SgExpression *, int, ExprPtrComparator> exprs_numbers;
    std::map<int, SgType *> numbers_types;
    std::map<SgType *, int, TypePtrComparator> types_numbers;

    std::map<std::string, SgVariableSymbol *> names_globals;
    std::map<std::string, SgExpression *> names_initializers;
    std::vector<SgVariableSymbol *> globals;
    std::map<SgVariableSymbol *, SgExpression *> globals_initializers;

    ~CFG();

 // This method is used to register all dynamically allocated
 // BlockListIterators. They can be freed when the CFG is destructed.
    void add_iteratorToDelete(BlockListIterator *i);
 // This method duplicates a C string. It keeps the allocated memory blocks
 // in a list and frees them when the CFG itself is destructed.
    char *dupstr(const char *str);

private:
    std::vector<BlockListIterator *> iteratorsToDelete;
    std::vector<char *> cStringsToDelete;
};

class BasicBlock
{
public:
    BasicBlock(KFG_NODE_ID id_, KFG_NODE_TYPE type_, int procnum_)
        : id(id_), node_type(type_), procnum(procnum_), reachable(true)
    {
    }

    KFG_NODE_ID id;
    KFG_NODE_TYPE node_type;
    int procnum;
    std::deque<SgStatement *> statements;
    std::vector<Edge> successors;
    std::vector<Edge> predecessors;
 // GB (2008-03-10): Reachability flag, used for cleaning up the CFG.
    bool reachable;

    virtual ~BasicBlock();
};

class CallStmt;

class CallBlock : public BasicBlock
{
public:
    CallBlock(KFG_NODE_ID id_, KFG_NODE_TYPE type_, int procnum_,
            std::vector<SgVariableSymbol *> *paramlist_, std::string name_);
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

class IcfgStmt : public SgStatement
{
public:
  // MS: we make this function virtual (although it is not virtual in ROSE)
  virtual std::string unparseToString() const;
  IcfgStmt() {}
};

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

protected:
    std::string infolabel;
};

typedef CallStmt FunctionCall, FunctionReturn;

class FunctionEntry : public CallStmt
{
public:
    FunctionEntry(KFG_NODE_TYPE type, std::string func, CallBlock *parent)
        : CallStmt(type, func, parent)
    {
    }

    // const char *get_funcname() const { return funcname; }

private:
    std::string funcname;
};

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

protected:
    SgVariableSymbol *var;
    SgType *type;
};

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

protected:
    std::vector<SgVariableSymbol *> *vars;
};

class RetvalAttribute : public AstAttribute
{
public:
    RetvalAttribute(std::string s) : str(s)
    {
    }
    std::string get_str() const
    {
        return str;
    }

private:
    RetvalAttribute();
    std::string str;
};

// GB (2007-20-23): Added members for the expression that refers to the
// external function and for its parameter list.
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
    
private:
    ExternalCall();
    SgExpression *function;
    std::vector<SgVariableSymbol *> *params;
    SgType *type;
};

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

private:
    ConstructorCall();
    std::string name;
    SgType *type;
};

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

private:
    DestructorCall();
    std::string name;
    SgType *type;
};

class ArgumentAssignment : public IcfgStmt
{
public:
    ArgumentAssignment(SgVariableSymbol *l, SgExpression *r);
    ArgumentAssignment(SgExpression *l, SgExpression *r);
    SgExpression *get_lhs() const;
    SgExpression *get_rhs() const;
    void set_rhs(SgExpression *r);
    std::string unparseToString() const;

private:
    void init(SgExpression *l, SgExpression *r);
    SgExpression *lhs;
    SgExpression *rhs;
  //ArgumentAssignment();
};

class MyAssignment : public IcfgStmt
{
public:
  SgVariableSymbol *get_lhs() const;
  SgVariableSymbol *get_rhs() const;
  MyAssignment(SgVariableSymbol *l, SgVariableSymbol *r);
  SgVarRefExp *get_lhsVarRefExp() const;
  SgVarRefExp *get_rhsVarRefExp() const;
  
protected:
  SgVariableSymbol *lhs;
  SgVariableSymbol *rhs;
  SgVarRefExp *lhsVarRefExp;
  SgVarRefExp *rhsVarRefExp;
};

class ReturnAssignment : public MyAssignment
{
public:
    ReturnAssignment(SgVariableSymbol *l, SgVariableSymbol *r)
        : MyAssignment(l, r)
    {
    }
    std::string unparseToString() const;
};

class ParamAssignment : public MyAssignment
{
public:
    ParamAssignment(SgVariableSymbol *l, SgVariableSymbol *r)
        : MyAssignment(l, r)
    {
    }
    std::string unparseToString() const;
};

class LogicalIf : public IcfgStmt
{
public:
    LogicalIf(SgExpression *e) : expr(e)
    {
     // expr->set_parent(NULL);
    }
    SgExpression *get_condition() const { return expr; }
    std::string unparseToString() const;

private:
    SgExpression *expr;
};

class IfJoin : public IcfgStmt
{
public:
    std::string unparseToString() const;
    std::string get_funcname() const { return "<none>"; }
};

class WhileJoin : public IcfgStmt
{
public:
    std::string unparseToString() const;
    std::string get_funcname() const { return "<none>"; }
};

class FunctionExit : public CallStmt
{
public:
    FunctionExit(KFG_NODE_TYPE type, std::string func, CallBlock *parent)
        : CallStmt(type, func, parent)
    {
    }

    // const char *get_funcname() const { return funcname; }

private:
    std::string funcname;
};

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
    SgFunctionParameterList *params;
    SgFunctionDeclaration *decl;

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

#endif
