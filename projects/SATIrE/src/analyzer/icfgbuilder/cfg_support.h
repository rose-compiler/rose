// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: cfg_support.h,v 1.2 2007-03-08 15:36:49 markus Exp $

#ifndef H_CFG_SUPPORT
#define H_CFG_SUPPORT

#include <utility>
#include <deque>

#include <AttributeMechanism.h>

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

typedef std::pair<BasicBlock *, KFG_EDGE_TYPE> Edge;
typedef std::deque<BasicBlock *> BlockList;
typedef std::pair<BlockList *, BlockList::iterator> BlockListIterator;

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
};

class BasicBlock
{
public:
    BasicBlock(KFG_NODE_ID id_, KFG_NODE_TYPE type_, int procnum_)
        : id(id_), node_type(type_), procnum(procnum_)
    {
    }

    KFG_NODE_ID id;
    KFG_NODE_TYPE node_type;
    int procnum;
    std::deque<SgStatement *> statements;
    std::vector<Edge> successors;
    std::vector<Edge> predecessors;
};

class CallStmt;

class CallBlock : public BasicBlock
{
public:
    CallBlock(KFG_NODE_ID id_, KFG_NODE_TYPE type_, int procnum_,
            std::list<SgVariableSymbol *> *paramlist_, char *name_);
    CallBlock *partner;
    std::string print_paramlist() const;
    std::list<SgVariableSymbol *> *paramlist;
    CallStmt *stmt;

protected:
    char *name;
};

class CallStmt : public SgStatement
{
public:
    CallStmt(KFG_NODE_TYPE node_type, char *name, CallBlock *parent);

    std::string unparseToString() const;
    char *get_funcname() const;

    KFG_NODE_TYPE type;
    char *name;
    CallBlock *parent;
    void update_infolabel();

protected:
    std::string infolabel;
};

typedef CallStmt FunctionCall, FunctionReturn;

class FunctionEntry : public CallStmt
{
public:
    FunctionEntry(KFG_NODE_TYPE type, char *func, CallBlock *parent)
        : CallStmt(type, func, parent)
    {
    }

    // const char *get_funcname() const { return funcname; }

private:
    char *funcname;
};

class DeclareStmt : public SgStatement
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

class UndeclareStmt : public SgStatement
{
public:
    UndeclareStmt(std::list<SgVariableSymbol *> *v)
        : vars(v)
    {
    }

    std::string unparseToString() const;
    std::list<SgVariableSymbol *> *get_vars() const { return vars; }

protected:
    std::list<SgVariableSymbol *> *vars;
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

class ExternalCall : public SgStatement
{
public:
    SgType *get_type() const { return type; }
    ExternalCall(SgType *type_) : type(type_) {}
    
private:
    ExternalCall();
    SgType *type;
};

class ConstructorCall : public SgStatement
{
public:
    char *get_name() const { return name; }
    SgType *get_type() const { return type; }
    ConstructorCall(char *name_, SgType *type_)
        : name(name_), type(type_)
    {
    }

private:
    ConstructorCall();
    char *name;
    SgType *type;
};

class DestructorCall : public SgStatement
{
public:
    char *get_name() const { return name; }
    SgType *get_type() const { return type; }
    DestructorCall(char *name_, SgType *type_)
        : name(name_), type(type_)
    {
    }

private:
    DestructorCall();
    char *name;
    SgType *type;
};

class ArgumentAssignment : public SgStatement
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
    ArgumentAssignment();
};

class MyAssignment : public SgStatement
{
public:
    MyAssignment(SgVariableSymbol *l, SgVariableSymbol *r) : lhs(l), rhs(r)
    {
    }
    SgVariableSymbol *get_lhs() const { return lhs; }
    SgVariableSymbol *get_rhs() const { return rhs; }
    std::string unparseToString() const;

protected:
    SgVariableSymbol *lhs;
    SgVariableSymbol *rhs;
    MyAssignment();
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

class LogicalIf : public SgStatement
{
public:
    LogicalIf(SgExpression *e) : expr(e)
    {
        expr->set_parent(NULL);
    }
    SgExpression *get_condition() const { return expr; }
    std::string unparseToString() const;

private:
    SgExpression *expr;
};

class IfJoin : public SgStatement
{
public:
    std::string unparseToString() const;
    char *get_funcname() const { return "<none>"; }
};

class WhileJoin : public SgStatement
{
public:
    std::string unparseToString() const;
    char *get_funcname() const { return "<none>"; }
};

class FunctionExit : public CallStmt
{
public:
    FunctionExit(KFG_NODE_TYPE type, char *func, CallBlock *parent)
        : CallStmt(type, func, parent)
    {
    }

    // const char *get_funcname() const { return funcname; }

private:
    char *funcname;
};

class Procedure
{
public:
    int procnum;
    char *name, *mangled_name, *memberf_name, *mangled_memberf_name;
    SgClassDefinition *class_type;
    CallBlock *entry;
    CallBlock *exit;
    BasicBlock *arg_block, *first_arg_block, *last_arg_block, *this_assignment;
    SgVariableSymbol *returnvar;
    std::map<std::string, BasicBlock *> goto_targets;
    std::multimap<std::string, BasicBlock *> goto_blocks;
    SgFunctionParameterList *params;
    SgFunctionDeclaration *decl;
};

void add_link(BasicBlock *from, BasicBlock *to, KFG_EDGE_TYPE type);
SgFunctionRefExp *find_called_func(SgExpression *);
SgMemberFunctionRefExp *find_called_memberfunc(SgExpression *);
SgExpression *calling_object_address(SgExpression *);
SgName find_func_name(SgFunctionCallExp *);
BasicBlock *call_destructor(SgInitializedName *in, CFG *cfg,
        int procnum, BasicBlock *after, int *node_id);
bool subtype_of(SgClassDefinition *, SgClassDefinition *);

#endif
