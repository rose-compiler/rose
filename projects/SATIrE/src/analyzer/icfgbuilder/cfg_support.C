#include "CFGTraversal.h"
#include "cfg_support.h"

CallBlock::CallBlock(KFG_NODE_ID id_, KFG_NODE_TYPE type_, int procnum_,
        std::list<SgVariableSymbol *> *paramlist_, char *name_)
    : BasicBlock(id_, type_, procnum_), paramlist(paramlist_), name(name_)
{
    switch (node_type)
    {
    case X_FunctionEntry:
        statements.push_back(stmt = new FunctionEntry(node_type, name, this));
        break;
    case X_FunctionExit:
        statements.push_back(stmt = new FunctionExit(node_type, name, this));
        break;
    default:
        statements.push_back(stmt = new CallStmt(node_type, name, this));
    }
}

std::string DeclareStmt::unparseToString() const
{
    std::string label = "DeclareStmt(";
    label += var->get_name().str();
    label += ", ";
    label += type->unparseToString();
    label += ")";
    return label;
}

std::string UndeclareStmt::unparseToString() const
{
    std::string label = "UndeclareStmt([";
    if (vars != NULL)
    {
        std::list<SgVariableSymbol *>::const_iterator i;
        i = vars->begin();
        if (i != vars->end())
            label += (*i++)->get_name().str();
        while (i != vars->end())
        {
            label += ", ";
            label += (*i++)->get_name().str();
        }
    }
    label += "])";
    return label;
}

std::string CallBlock::print_paramlist() const
{
    if (paramlist == NULL)
    {
        // return std::string("null pointer in ") + (void *) this;
        std::stringstream s;
        s << "CallBlock " << (void *) this << " has null params";
        return s.str();
    }
    std::list<SgVariableSymbol *>::const_iterator i = paramlist->begin();
    if (i == paramlist->end())
        return std::string("");
    else
    {
        std::stringstream s;
        s << (*i)->get_name().str();
        for (++i; i != paramlist->end(); ++i)
            s << ", " << (*i)->get_name().str();
        return s.str();
    }
}

CallStmt::CallStmt(KFG_NODE_TYPE type_, char *name_, CallBlock *parent_)
    : type(type_), name(name_), parent(parent_)
{
    update_infolabel();
}

void CallStmt::update_infolabel()
{
    std::string s;
    switch (type)
    {
    case X_FunctionCall:
        s = std::string("Call(") + name +
            + ", [" + parent->print_paramlist() + "])";
        break;
    case X_FunctionReturn:
        s = std::string("Return(") + name +
            + ", [" + parent->print_paramlist() + "])";
        break;
    case X_FunctionEntry:
        s = std::string("Entry(") + name + ")";
        break;
    case X_FunctionExit:
        s = std::string("Exit(") + name
            + ", [" + parent->print_paramlist() + "])";
        break;
    default:
        s = "WEIRD NODE";
    }
    infolabel = s;
}

const char *expr_to_string(const SgExpression *expr)
{
    if (expr == NULL)
        return "null";
    SgTreeCopy treecopy;
    SgExpression *new_expr = isSgExpression(expr->copy(treecopy));
    new_expr->set_parent(NULL);
    const char *retval = strdup(new_expr->unparseToString().c_str());
    return retval;
}

bool ExprPtrComparator::operator()(const SgExpression *a,
        const SgExpression *b) const
{
    const char *sa = expr_to_string(a), *sb = expr_to_string(b);

    return strcmp(sa, sb) < 0;
}

bool TypePtrComparator::operator()(SgType *a, SgType *b) const
{
    while (isSgTypedefType(a))
        a = isSgTypedefType(a)->get_base_type();
    while (isSgTypedefType(b))
        b = isSgTypedefType(b)->get_base_type();
    const char *sa = strdup(a->unparseToString().c_str());
    const char *sb = strdup(b->unparseToString().c_str());

    return strcmp(sa, sb) < 0;
}

SgFunctionRefExp *find_called_func(SgExpression *call_expr)
{
    if (isSgFunctionRefExp(call_expr))
        return isSgFunctionRefExp(call_expr);
    else if (isSgDotExp(call_expr))
        return find_called_func(isSgDotExp(call_expr)->get_rhs_operand());
    else if (isSgArrowExp(call_expr))
        return find_called_func(isSgArrowExp(call_expr)->get_rhs_operand());
    else
        return NULL;
}

SgMemberFunctionRefExp *find_called_memberfunc(SgExpression *call_expr)
{
    if (isSgMemberFunctionRefExp(call_expr))
        return isSgMemberFunctionRefExp(call_expr);
    else if (isSgDotExp(call_expr))
        return find_called_memberfunc(
                isSgDotExp(call_expr)->get_rhs_operand());
    else if (isSgArrowExp(call_expr))
        return find_called_memberfunc(
                isSgArrowExp(call_expr)->get_rhs_operand());
    else
        return NULL;
}

SgExpression *calling_object_address(SgExpression *call_expr)
{
    SgDotExp *dot_exp = isSgDotExp(call_expr);
    SgArrowExp *arrow_exp = isSgArrowExp(call_expr);

    if (dot_exp)
        if (isSgMemberFunctionRefExp(dot_exp->get_rhs_operand()))
            return new SgAddressOfOp(dot_exp->get_file_info(),
                    dot_exp->get_lhs_operand(), dot_exp->get_type());
        else
            return calling_object_address(dot_exp->get_rhs_operand());
    else if (arrow_exp)
        if (isSgMemberFunctionRefExp(arrow_exp->get_rhs_operand()))
            return arrow_exp->get_lhs_operand();
        else
            return calling_object_address(arrow_exp->get_rhs_operand());
    else
        return NULL;
}

SgName find_func_name(SgFunctionCallExp *call)
{
    SgFunctionRefExp *fr = find_called_func(call->get_function());
    SgMemberFunctionRefExp *mfr = find_called_memberfunc(call->get_function());
    
    if (fr)
        return fr->get_symbol_i()->get_name();
    else if (mfr)
        return SgName(mfr->get_symbol_i()->get_name().str());
    else
        return SgName("unknown_func");
}

std::string CallStmt::unparseToString() const
{
    return infolabel;
}

char *CallStmt::get_funcname() const
{
    return name;
}

ArgumentAssignment::ArgumentAssignment(SgVariableSymbol *l, SgExpression *r)
{
    init(new SgVarRefExp(new Sg_File_Info(), l), r);
}

ArgumentAssignment::ArgumentAssignment(SgExpression *l, SgExpression *r)
{
    init(l, r);
}

void ArgumentAssignment::init(SgExpression *l, SgExpression *r)
{
    lhs = l;
    if (isSgFunctionCallExp(r))
    {
        RetvalAttribute *varnameattr
            = (RetvalAttribute *) r->getAttribute("return variable");
        SgVariableSymbol *var = new SgVariableSymbol(
                new SgInitializedName(SgName(varnameattr->get_str()),
                    r->get_type()));
        rhs = new SgVarRefExp(r->get_file_info(), var);
    }
    else if (isSgConstructorInitializer(r)
            && r->attributeExists("anonymous variable"))
    {
        RetvalAttribute *varnameattr
            = (RetvalAttribute *) r->getAttribute("anonymous variable");
        SgVariableSymbol *var = new SgVariableSymbol(
                new SgInitializedName(SgName(varnameattr->get_str()),
                    r->get_type()));
        rhs = new SgVarRefExp(r->get_file_info(), var);
    }
    else
        rhs = r;
    if (rhs != NULL)
        rhs->set_parent(NULL);
}

SgExpression *ArgumentAssignment::get_lhs() const
{
    return lhs;
}

SgExpression *ArgumentAssignment::get_rhs() const
{
    return rhs;
}

void ArgumentAssignment::set_rhs(SgExpression *r)
{
    rhs = r;
}

std::string ArgumentAssignment::unparseToString() const
{
    std::string buf = "ArgumentAssignment(";
    buf += expr_to_string(lhs);
    buf += ", ";
    buf += expr_to_string(rhs);
    buf += ")";
    return buf;
}

std::string ReturnAssignment::unparseToString() const
{   
    std::string buf = "ReturnAssignment(";
    if (lhs->get_declaration()->get_scope() != NULL)
        buf += lhs->get_declaration()->get_qualified_name().str();
    else
        buf += lhs->get_name().str();
    buf += ", ";
    if (rhs->get_declaration()->get_scope() != NULL)
        buf += rhs->get_declaration()->get_qualified_name().str();
    else
        buf += rhs->get_name().str();
    buf += ")";
    return buf;
} 

std::string ParamAssignment::unparseToString() const
{   
    std::string buf = "ParamAssignment(";
    if (lhs->get_declaration()->get_scope() != NULL)
        buf += lhs->get_declaration()->get_qualified_name().str();
    else
        buf += lhs->get_name().str();
    buf += ", ";
    if (rhs->get_declaration()->get_scope() != NULL)
        buf += rhs->get_declaration()->get_qualified_name().str();
    else
        buf += rhs->get_name().str();
    buf += ")";
    return buf;
} 

std::string LogicalIf::unparseToString() const
{
    return std::string("LogicalIf(") + expr->unparseToString() + ')';
}

std::string IfJoin::unparseToString() const
{
    return std::string("IfJoin");
}

std::string WhileJoin::unparseToString() const
{
    return std::string("WhileJoin");
}

BasicBlock *call_destructor(SgInitializedName *in, CFG *cfg,
        int procnum, BasicBlock *after, int *node_id)
{
    SgClassType *ct = isSgClassType(in->get_type());
    std::string class_name(ct->get_name().str());
    std::string destructor_name = class_name + "::~" + class_name;
    std::string this_var_name = std::string("$~") + class_name + "$this";
    std::deque<Procedure *>::const_iterator i;
    for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i)
    {
        /* we want member functions */
        if ((*i)->memberf_name == NULL)
            break;
        if (destructor_name == (*i)->memberf_name)
        {
            SgInitializedName *this_var = new SgInitializedName(
                    this_var_name, in->get_type());
            SgVariableSymbol *this_var_sym = new SgVariableSymbol(this_var);
            CallBlock *entry = (*i)->entry;
            CallBlock *call_block = new CallBlock((*node_id)++, CALL,
                    procnum, NULL, strdup(destructor_name.c_str()));
            CallBlock *return_block = new CallBlock((*node_id)++, RETURN,
                    procnum, NULL, strdup(destructor_name.c_str()));
            cfg->nodes.push_back(call_block);
            cfg->calls.push_back(call_block);
            cfg->nodes.push_back(return_block);
            cfg->returns.push_back(return_block);
            call_block->partner = return_block;
            return_block->partner = call_block;
            BasicBlock *this_block
                = new BasicBlock((*node_id)++, INNER, procnum);
            cfg->nodes.push_back(this_block);
            this_block->statements.push_back(new ArgumentAssignment(
                        new SgVarRefExp(new Sg_File_Info(), this_var_sym),
                        new SgAddressOfOp(new Sg_File_Info(),
                            new SgVarRefExp(new Sg_File_Info(),
                                new SgVariableSymbol(in)),
                            new SgPointerType(in->get_type()))));
            /* set links */
            add_link(this_block, call_block, NORMAL_EDGE);
            add_link(call_block, entry, CALL_EDGE);
            add_link(call_block, return_block, LOCAL);
            add_link(entry->partner, return_block, RETURN_EDGE);
            add_link(return_block, after, NORMAL_EDGE);
            after = this_block;
            /* This was the only implementation of this
             * destructor; we don't need to traverse the rest of
             * the procedures. */
            break;
        }
    }
    /* If no destructor was found, it is external -- or does not
     * exist, this should be improved in the future. */
    if (i == cfg->procedures->end())
    {
        BasicBlock *b = new BasicBlock((*node_id)++, INNER, procnum);
        cfg->nodes.push_back(b);
        b->statements.push_back(
                new DestructorCall(strdup(class_name.c_str()), ct));
        add_link(b, after, NORMAL_EDGE);
        after = b;
    }
    return after;
}

bool subtype_of(SgClassDefinition *a, SgClassDefinition *b)
{
    const std::list<SgBaseClass *> &base_classes = a->get_inheritances();
    std::list<SgBaseClass *>::const_iterator i;
    for (i = base_classes.begin(); i != base_classes.end(); ++i)
    {
        SgClassDefinition *base = (*i)->get_base_class()->get_definition();
        if (base == b || subtype_of(base, b))
            return true;
    }
    return false;
}
