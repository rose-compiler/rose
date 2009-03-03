// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: cfg_support.C,v 1.31 2009-02-11 10:03:44 gergo Exp $

#include "CFGTraversal.h"
#include "cfg_support.h"
#include "IrCreation.h"
#include <cstring>

CFG::CFG()
  : global_return_variable_symbol(NULL), global_this_variable_symbol(NULL),
    global_unknown_type(NULL), analyzerOptions(NULL),
    pointsToAnalysis(NULL), contextInformation(NULL)
{
}

CFG::~CFG()
{
    std::deque<Procedure *>::iterator p;
    for (p = procedures->begin(); p != procedures->end(); ++p)
        delete *p;
    delete procedures;
    BlockList::iterator b;
    for (b = nodes.begin(); b != nodes.end(); ++b)
        delete *b;
#if 0
    std::vector<BlockListIterator *>::iterator i;
    for (i = iteratorsToDelete.begin(); i != iteratorsToDelete.end(); ++i)
        delete *i;
#endif
    std::vector<char *>::iterator s;
    for (s = cStringsToDelete.begin(); s != cStringsToDelete.end(); ++s)
        free(*s);
}

CallBlock::CallBlock(KFG_NODE_ID id_, KFG_NODE_TYPE type_, int procnum_,
                     std::vector<SgVariableSymbol *> *paramlist_,
                     std::string name_, bool add_call_stmt)
  : BasicBlock(id_, type_, procnum_), paramlist(paramlist_), name(name_)
{
  if (add_call_stmt) {
    switch (node_type) {
    case X_FunctionCall:
      statements.push_back(
              stmt = Ir::createFunctionCall(node_type, name, this));
      break;
    case X_FunctionReturn:
      statements.push_back(
              stmt = Ir::createFunctionReturn(node_type, name, this));
      break;
    case X_FunctionEntry:
      statements.push_back(
              stmt = Ir::createFunctionEntry(node_type, name, this));
      break;
    case X_FunctionExit:
      statements.push_back(
              stmt = Ir::createFunctionExit(node_type, name, this));
      break;
    default:
   // statements.push_back(stmt = Ir::createCallStmt(node_type, name, this));
      std::cerr
          << "ICFG builder error: " << __FILE__ << ":" << __LINE__
          << ": reached default case in switch" << std::endl;
      abort();
    }
  }
  else
    stmt = NULL;
}

std::vector<SgVariableSymbol *> *
CallBlock::get_params()
{
    return paramlist;
}

void
CallBlock::set_params(std::vector<SgVariableSymbol *> *params)
{
    if (paramlist != NULL)
        delete paramlist;
    paramlist = params;
}

CallBlock::~CallBlock()
{
    if (paramlist != NULL)
        delete paramlist;
}

std::string DeclareStmt::unparseToString() const 
{
  std::string label = "DeclareStmt(";
  label += var->get_name().str();
  label += ", ";
  label += Ir::fragmentToString(type);
  label += ")";
  return label;
}

std::string UndeclareStmt::unparseToString() const 
{
  std::string label = "UndeclareStmt([";
  if (vars != NULL) {
    std::vector<SgVariableSymbol *>::const_iterator i;
    i = vars->begin();
    if (i != vars->end()) {
      label += (*i++)->get_name().str();
    }
    while (i != vars->end()) {
      label += ", ";
      label += (*i++)->get_name().str();
    }
  }
  label += "])";
  return label;
}

UndeclareStmt::~UndeclareStmt()
{
    delete vars;
}

// GB (2007-10-23): Added this function to unparse the new members of the
// ExternalCall node (the function expression and the list of parameter
// variables).
std::string ExternalCall::unparseToString() const
{
    std::stringstream label;
    label << "ExternalCall(" << Ir::fragmentToString(function) << ", [";
    assert(params != NULL);
    std::vector<SgVariableSymbol *>::const_iterator i = params->begin();
    if (i != params->end())
    {
        label << (*i)->get_name().str();
        for (++i; i != params->end(); ++i)
            label << ", " << (*i)->get_name().str();
    }
    label << "])";
    return label.str();
}

ExternalCall::ExternalCall(SgExpression *function_,
        std::vector<SgVariableSymbol *> *params_, SgType *type_)
  : function(function_), params(params_), type(type_)
{
}

void ExternalCall::set_params(std::vector<SgVariableSymbol *> *params_)
{
    if (params != NULL)
        delete params;
    params = params_;
}

ExternalCall::~ExternalCall()
{
    if (params != NULL)
        delete params;
}

std::string ExternalReturn::unparseToString() const
{
    std::stringstream label;
    label << "ExternalReturn(" << Ir::fragmentToString(function) << ", [";
    assert(params != NULL);
    std::vector<SgVariableSymbol *>::const_iterator i = params->begin();
    if (i != params->end())
    {
        label << (*i)->get_name().str();
        for (++i; i != params->end(); ++i)
            label << ", " << (*i)->get_name().str();
    }
    label << "])";
    return label.str();
}

ExternalReturn::ExternalReturn(SgExpression *function_,
        std::vector<SgVariableSymbol *> *params_, SgType *type_)
  : function(function_), params(params_), type(type_)
{
}

void ExternalReturn::set_params(std::vector<SgVariableSymbol *> *params_)
{
    if (params != NULL)
        delete params;
    params = params_;
}

ExternalReturn::~ExternalReturn()
{
    if (params != NULL)
        delete params;
}

std::string CallBlock::print_paramlist() const 
{
  if (paramlist == NULL) {
    // return std::string("null pointer in ") + (void *) this;
    std::stringstream s;
    s << "CallBlock " << (void *) this << " has null params";
    return s.str();
  }
  std::vector<SgVariableSymbol *>::const_iterator i = paramlist->begin();
  if (i == paramlist->end()) {
    return std::string("");
  } else {
    std::stringstream s;
    s << (*i)->get_name().str();
    for (++i; i != paramlist->end(); ++i)
      s << ", " << (*i)->get_name().str();
    return s.str();
  }
}

CallStmt::CallStmt(KFG_NODE_TYPE type_, std::string name_, CallBlock *parent_)
  : type(type_), name(name_), parent(parent_) 
{
  update_infolabel();
}

void 
CallStmt::update_infolabel() 
{
  if (this == NULL)
  {
      infolabel = "<none>";
      return;
  }
  std::string s;
  switch (type) {
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

FunctionCall::FunctionCall(KFG_NODE_TYPE node_type, std::string name, CallBlock *parent)
  : CallStmt(node_type, name, parent)
{
}

FunctionReturn::FunctionReturn(KFG_NODE_TYPE node_type, std::string name, CallBlock *parent)
  : CallStmt(node_type, name, parent)
{
}

const char*
expr_to_string(const SgExpression *expr) 
{
  if (expr == NULL)
    return "null";

  // MS: deactivate code because new function fragmentToString allow
  // to unparse without duplicating code
  //SgTreeCopy treecopy;
  //SgExpression *new_expr = isSgExpression(expr->copy(treecopy));
  //new_expr->set_parent(NULL);
  // const char* retval = strdup(Ir::fragmentToString(new_expr).c_str());

  const char* retval = Ir::getCharPtr(Ir::fragmentToString(expr));
  return retval;
}

bool ExprPtrComparator::operator()(const SgExpression *a,
                                   const SgExpression *b) const 
{
  if (a == b)
      return false;
// GB (2008-03-26): This was really horribly slow with all the string
// operations. Looking at the variants first should help...
  VariantT va = a->variantT();
  VariantT vb = b->variantT();
  if (va < vb)
      return true;
  if (va > vb)
      return false;

// Simple "profiling" showed that SgVarRefExp is by far the most frequent
// expression type in our ICFG. At least in the test program. Be that as it
// may, optimize that case aggressively.
  if (const SgVarRefExp *ra = isSgVarRefExp(a))
  {
      if (const SgVarRefExp *rb = isSgVarRefExp(b))
      {
       // SgSymbols are supposedly always shared in the AST. But that's not
       // something that should be trusted.
          SgSymbol *syma = ra->get_symbol();
          SgSymbol *symb = rb->get_symbol();
          if (syma == symb)
              return false;
       // SgNames are supposedly also shared...
          SgName na = syma->get_name();
          SgName nb = symb->get_name();
          const char *nastr = na.str();
          const char *nbstr = nb.str();
          if (nastr == nbstr)
              return false;
       // strcmp here should be faster than constructing a std::string and
       // comparing using <
          return std::strcmp(nastr, nbstr) < 0;
      }
  }

// SgAssignOp, SgCastExp are also very frequent. As are other binary and
// unary operators, or so I would think. Recall that we *know* that a and b
// are the same operator because the variants matched above!
  if (const SgBinaryOp *ba = isSgBinaryOp(a))
  {
      if (const SgBinaryOp *bb = isSgBinaryOp(b))
      {
       // The root of the two expressions is the same. Thus:
       // if a->left < b->left then a < b;
       // if b->left < a->left then a !< b.
          SgExpression *bal = ba->get_lhs_operand();
          SgExpression *bbl = bb->get_lhs_operand();
          if (operator()(bal, bbl))
              return true;
          if (operator()(bbl, bal))
              return false;
       // If we got here, neither a->left < b->left nor b->left < a->left.
       // Thus a->left == b->left. So we look at the right operands like we
       // did above.
          SgExpression *bar = ba->get_rhs_operand();
          SgExpression *bbr = bb->get_rhs_operand();
          if (operator()(bar, bbr))
              return true;
          if (operator()(bbr, bar))
              return false;
      }
  }
  if (const SgUnaryOp *ua = isSgUnaryOp(a))
  {
      if (const SgUnaryOp *ub = isSgUnaryOp(b))
      {
          return operator()(ua->get_operand(), ub->get_operand());
      }
  }

// GB (2008-03-26): Started stuffing the most obvious memory leaks. This
// should also be a little faster than with strdup.
  bool result = Ir::fragmentToString(a) < Ir::fragmentToString(b);
  return result;
}

bool TypePtrComparator::operator()(SgType *a, SgType *b) const 
{
  if (a == b)
      return false;
#if 1
// This way of comparing types is much much faster! Still, we should move to
// a hash table approach soon.
// grato: 12 sec
  std::string ma = a->get_mangled().str();
  std::string mb = b->get_mangled().str();
  return ma < mb;
#endif

#if 0
// 166 sec
  while (isSgTypedefType(a)) {
    a = isSgTypedefType(a)->get_base_type();
  }
  while (isSgTypedefType(b)) {
    b = isSgTypedefType(b)->get_base_type();
  }
  if (a == b)
      return false;

// GB (2008-03-26): This was really horribly slow with all the string
// operations. Looking at the variants first should help...
  VariantT va = a->variantT();
  VariantT vb = b->variantT();
  if (va < vb)
      return true;
  if (va > vb)
      return false;

// SgPointerType appears to be the most frequent type of type in our ICFGs.
  if (SgPointerType *pa = isSgPointerType(a))
  {
      if (SgPointerType *pb = isSgPointerType(b))
      {
          SgType *ba = pa->stripType(SgType::STRIP_POINTER_TYPE);
          SgType *bb = pb->stripType(SgType::STRIP_POINTER_TYPE);
          return operator()(ba, bb);
      }
  }

// SgClassType is also very frequent.
  if (const SgNamedType *na = isSgNamedType(a))
  {
      if (const SgNamedType *nb = isSgNamedType(b))
      {
          std::string qa = na->get_qualified_name().str();
          std::string qb = nb->get_qualified_name().str();
          return qa < qb;
      }
  }

  bool result = Ir::fragmentToString(a) < Ir::fragmentToString(b);
  return result;
#endif
}

SgFunctionRefExp*
find_called_func(SgExpression *call_expr)
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

SgMemberFunctionRefExp*
find_called_memberfunc(SgExpression *call_expr)
{
  if (isSgMemberFunctionRefExp(call_expr))
    return isSgMemberFunctionRefExp(call_expr);
  else if (isSgDotExp(call_expr))
    return find_called_memberfunc(isSgDotExp(call_expr)->get_rhs_operand());
  else if (isSgArrowExp(call_expr))
    return find_called_memberfunc(isSgArrowExp(call_expr)->get_rhs_operand());
  else
    return NULL;
}

SgExpression*
calling_object_address(SgExpression *call_expr)
{
  SgDotExp *dot_exp = isSgDotExp(call_expr);
  SgArrowExp *arrow_exp = isSgArrowExp(call_expr);
  SgMemberFunctionRefExp *mfun_ref = isSgMemberFunctionRefExp(call_expr);
  
  if (dot_exp)
  {
    if (isSgMemberFunctionRefExp(dot_exp->get_rhs_operand()))
      return Ir::createAddressOfOp(dot_exp->get_lhs_operand(), dot_exp->get_type());
    else
      return calling_object_address(dot_exp->get_rhs_operand());
  }
  else if (arrow_exp)
  {
    if (isSgMemberFunctionRefExp(arrow_exp->get_rhs_operand()))
      return arrow_exp->get_lhs_operand();
    else
      return calling_object_address(arrow_exp->get_rhs_operand());
  }
  else if (mfun_ref)
  {
    return Ir::createNullPointerExp(
            new SgPointerType(mfun_ref->get_symbol()
                ->get_declaration()->get_associatedClassDeclaration()
                ->get_type()));
  }
  else
    return NULL;
}

std::string *
find_func_name(SgFunctionCallExp *call)
{
  SgFunctionRefExp *fr = find_called_func(call->get_function());
  SgMemberFunctionRefExp *mfr = find_called_memberfunc(call->get_function());
  
  if (fr)
    return new std::string(fr->get_symbol_i()->get_name().str());
  else if (mfr)
    return new std::string(mfr->get_symbol_i()->get_name().str());
  else
    return NULL;
}

std::string
CallStmt::unparseToString() const
{
  return infolabel;
}

std::string
CallStmt::get_funcname() const
{
  return name;
}

std::string IcfgStmt::unparseToString() const
{
  std::cout << "IcfgStmt::UnparseToString: use inherited class." << std::endl;
  assert(false);
}

ArgumentAssignment::ArgumentAssignment(SgVariableSymbol *l, SgExpression *r)
{
  init(Ir::createVarRefExp(l), r);
}

ArgumentAssignment::ArgumentAssignment(SgExpression *l, SgExpression *r)
{
  init(l, r);
}

void ArgumentAssignment::init(SgExpression *l, SgExpression *r)
{
  lhs = l;
  if (isSgFunctionCallExp(r)) {
    RetvalAttribute *varnameattr
      = (RetvalAttribute *) r->getAttribute("return variable");
 // rhs = Ir::createVarRefExp(varnameattr->get_str(),r->get_type());
    rhs = Ir::createVarRefExp(varnameattr->get_variable_symbol());
  } else if (isSgConstructorInitializer(r)
             && r->attributeExists("anonymous variable")) {
    RetvalAttribute *varnameattr
      = (RetvalAttribute *) r->getAttribute("anonymous variable");
 // rhs = Ir::createVarRefExp(varnameattr->get_str(),r->get_type());
    rhs = Ir::createVarRefExp(varnameattr->get_variable_symbol());
  } else {
    rhs = r;
  }
  if (rhs != NULL) {
 // rhs->set_parent(NULL);
  }
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
  buf += Ir::fragmentToString(lhs);
  buf += ", ";
  buf += Ir::fragmentToString(rhs);
  buf += ")";
  return buf;
}

std::string ReturnAssignment::unparseToString() const
{   
  std::string buf = "ReturnAssignment("
    + Ir::getStrippedName(lhs->get_declaration()) 
    + ", "
    + Ir::getStrippedName(rhs->get_declaration())
    + ")";
  return buf;
} 

std::string ParamAssignment::unparseToString() const
{   
  std::string buf = "ParamAssignment("
    + Ir::getStrippedName(lhs->get_declaration()) 
    + ", "
    + Ir::getStrippedName(rhs->get_declaration())
    + ")";
  return buf;
} 

std::string LogicalIf::unparseToString() const
{
  return std::string("LogicalIf(") + Ir::fragmentToString(expr) + ')';
}

std::string IfJoin::unparseToString() const
{
  return std::string("IfJoin");
}

std::string WhileJoin::unparseToString() const
{
  return std::string("WhileJoin");
}

std::string ConstructorCall::unparseToString() const
{
  return std::string("ConstructorCall(") + get_name() + ")";
}

std::string DestructorCall::unparseToString() const
{
  return std::string("DestructorCall(") + get_name() + ")";
}

SgVariableSymbol * MyAssignment::get_lhs() const {
  return lhs;
}

SgVariableSymbol * MyAssignment::get_rhs() const {
  return rhs;
}

MyAssignment::MyAssignment(SgVariableSymbol *l, SgVariableSymbol *r)
  : lhs(l), rhs(r),
    lhsVarRefExp(Ir::createVarRefExp(lhs)),
    rhsVarRefExp(Ir::createVarRefExp(rhs))
{
    lhsVarRefExp->set_parent(this);
    if (l->attributeExists("SATIrE: call target"))
    {
        AstAttribute *a = l->getAttribute("SATIrE: call target");
        lhsVarRefExp->addNewAttribute("SATIrE: call target", a);
    }

    rhsVarRefExp->set_parent(this);
    if (r->attributeExists("SATIrE: call target"))
    {
        AstAttribute *a = r->getAttribute("SATIrE: call target");
        rhsVarRefExp->addNewAttribute("SATIrE: call target", a);
    }
}

SgVarRefExp *
MyAssignment::get_lhsVarRefExp() const {
    return lhsVarRefExp;
}

SgVarRefExp *
MyAssignment::get_rhsVarRefExp() const {
    return rhsVarRefExp;
}

BasicBlock *call_destructor(SgInitializedName *in, CFG *cfg,
                            int procnum, BasicBlock *after, int *node_id)
{
  SgClassType *ct = isSgClassType(in->get_type());
  std::string class_name(ct->get_name().str());
  std::string destructor_name = class_name + "::~" + class_name;
//std::string this_var_name = std::string("$~") + class_name + "$this";
  std::deque<Procedure *>::const_iterator i;
  for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i) {
    /* we want member functions */
    if ((*i)->memberf_name == "")
      break;
    if (destructor_name == (*i)->memberf_name) {
   // SgVariableSymbol* this_var_sym = Ir::createVariableSymbol(this_var_name, in->get_type());
      SgVariableSymbol* this_var_sym = cfg->global_this_variable_symbol;

      CallBlock *entry = (*i)->entry;
      CallBlock *call_block = new CallBlock((*node_id)++, CALL, procnum,
              new std::vector<SgVariableSymbol *>(), destructor_name);
      CallBlock *return_block = new CallBlock((*node_id)++, RETURN, procnum,
              new std::vector<SgVariableSymbol *>(), destructor_name);
      cfg->nodes.push_back(call_block);
      cfg->calls.push_back(call_block);
      cfg->nodes.push_back(return_block);
      cfg->returns.push_back(return_block);
      call_block->partner = return_block;
      return_block->partner = call_block;
      BasicBlock *this_block
        = new BasicBlock((*node_id)++, INNER, procnum);
      cfg->nodes.push_back(this_block);

      SgVariableSymbol *varsym = Ir::createVariableSymbol(in);
      SgAddressOfOp* addressOfOp
          = Ir::createAddressOfOp(Ir::createVarRefExp(varsym),
                                  Ir::createPointerType(in->get_type()));
      ArgumentAssignment* argumentAssignment
          = new ArgumentAssignment(Ir::createVarRefExp(this_var_sym),addressOfOp);
      this_block->statements.push_back(argumentAssignment);

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
  if (i == cfg->procedures->end()) {
    BasicBlock *b = new BasicBlock((*node_id)++, INNER, procnum);
    cfg->nodes.push_back(b);
    b->statements.push_back(Ir::createDestructorCall(class_name, ct));
    add_link(b, after, NORMAL_EDGE);
    after = b;
  }
  return after;
}

bool subtype_of(SgClassDefinition *a, SgClassDefinition *b)
{
#if 0
  std::cout << "subtype_of(" << (void *) a;
  if (a != NULL)
      std::cout << " " << a->get_qualified_name().str();
  std::cout << ", " << (void *) b;
  if (b != NULL)
      std::cout << " " << b->get_qualified_name().str();
  std::cout << ")" << std::endl;
#endif

  if (a == NULL || b == NULL)
      return false;
  if (a == b)
      return true;
  if (a->get_declaration()->get_firstNondefiningDeclaration()
          == b->get_declaration()->get_firstNondefiningDeclaration())
      return true;

  std::string aname = a->get_declaration()->get_qualified_name().str();
  std::string bname = b->get_declaration()->get_qualified_name().str();
  if (aname == bname)
  {
   // std::cout << "names match: " << aname << " == " << bname << std::endl;
      return true;
  }

  const SgBaseClassPtrList &base_classes = a->get_inheritances();
  SgBaseClassPtrList::const_iterator i;
  for (i = base_classes.begin(); i != base_classes.end(); ++i) {
    SgClassDefinition *base = (*i)->get_base_class()->get_definition();
 // GB (2008-03-18): For some reason, get_definition on the base class
 // sometimes returns NULL even if there is a definition of the class in the
 // program. Thus we also try this alternative way of looking for the base
 // class.
    if (base == NULL) {
        SgDeclarationStatement *dd = (*i)->get_base_class()->get_definingDeclaration();
        if (isSgClassDeclaration(dd)) {
            SgClassDefinition *cd = isSgClassDeclaration(dd)->get_definition();
            if (cd != NULL)
                base = cd;
        }
    }
    if (base == b || subtype_of(base, b)) {
      return true;
    }
  }

  return false;
}

void dumpTreeFragment(SgNode *node, std::ostream &stream)
{
    TreeFragmentDumper tfd(stream);
    tfd.run(node);
}

std::string dumpTreeFragmentToString(SgNode *node)
{
    std::stringstream ss;
    dumpTreeFragment(node, ss);
    return ss.str();
}

Procedure::Procedure()
  : class_type(NULL), entry(NULL), exit(NULL),
    arg_block(NULL), first_arg_block(NULL), last_arg_block(NULL),
    this_assignment(NULL), returnvar(NULL),
    params(NULL), decl(NULL), static_file(NULL)
{
}

Procedure::~Procedure()
{
    if (arg_block != NULL)
        delete arg_block;
}

BasicBlock::~BasicBlock()
{
#if 0
    std::cout << "*** destructing basic block " << id
        << " " << (void *) this << " "
        << Ir::fragmentToString(statements.front()) << " ";
    if (statements.front() != NULL && !dynamic_cast<IcfgStmt *>(statements.front()))
        std::cout << statements.front()->class_name();
    std::cout << std::endl;
#endif
    std::deque<SgStatement *>::iterator s;
    for (s = statements.begin(); s != statements.end(); ++s)
    {
     // Delete SATIrE-specific statements. ROSE nodes are (hopefully)
     // collected by ROSE.
        if (dynamic_cast<IcfgStmt *>(*s))
            delete *s;
    }
}

#include "satire/analysis_info.h"

bool
CFG::statementHasLabels(SgStatement *stmt)
{
    StatementAttribute *start, *end;
    start = (stmt->attributeExists("PAG statement start")
            ? (StatementAttribute *) stmt->getAttribute("PAG statement start")
            : NULL);
    end = (stmt->attributeExists("PAG statement end")
          ? (StatementAttribute *) stmt->getAttribute("PAG statement end")
          : NULL);
    return (start != NULL && end != NULL);
}

std::pair<int, int>
CFG::statementEntryExitLabels(SgStatement *stmt)
{
    StatementAttribute *start, *end;
    start = (stmt->attributeExists("PAG statement start")
            ? (StatementAttribute *) stmt->getAttribute("PAG statement start")
            : NULL);
    end = (stmt->attributeExists("PAG statement end")
          ? (StatementAttribute *) stmt->getAttribute("PAG statement end")
          : NULL);
    if (isSgScopeStatement(stmt->get_parent())
        && !isSgGlobal(stmt->get_parent())
        && (start == NULL || end == NULL))
    {
        std::cerr
            << "*** internal error: " << __FILE__ << ":" << __LINE__
            << ": in function CFG::statementEntryExitLabels: statement "
            << stmt->class_name() << " of type " << Ir::fragmentToString(stmt)
            << " has no valid statement start/end attributes"
            << std::endl;
        std::abort();
    }
    int startLabel = start->get_bb()->id;
    int endLabel = end->get_bb()->id;
    return std::make_pair(startLabel, endLabel);
}

std::set<int>
CFG::statementAllLabels(SgStatement *stmt)
{
    return stmt_blocks_map[stmt];
}

void
CFG::registerStatementLabel(int label, SgStatement *stmt)
{
    block_stmt_map[label] = stmt;
    stmt_blocks_map[stmt].insert(label);
}

void 
CFG::print_map() const
{
    std::map<int, SgStatement *>::const_iterator i;
    for (i = block_stmt_map.begin(); i != block_stmt_map.end(); ++i)
    {
        std::cout
            << "block " << std::setw(4) << i->first
            << " stmt " << i->second << ": "
            << Ir::fragmentToString((i->second)) << std::endl;
    }
}

#if 0
void
CFG::add_iteratorToDelete(BlockListIterator *i)
{
    iteratorsToDelete.push_back(i);
}
#endif

char *
CFG::dupstr(const char *str)
{
    char *s = strdup(str);
    cStringsToDelete.push_back(s);
    return s;
}

#if 0
BlockListIterator::BlockListIterator(CFG *cfg, BlockList *blocks,
                                     DeletionFlag deletionFlag)
  : cfg(cfg), blocks(blocks), pos(blocks->begin()), deletionFlag(deletionFlag)
{
}

BlockListIterator::BlockListIterator(CFG *cfg, BlockList *blocks,
                                     BlockList::iterator pos,
                                     DeletionFlag deletionFlag)
  : cfg(cfg), blocks(blocks), pos(pos), deletionFlag(deletionFlag)
{
}

BlockListIterator::~BlockListIterator()
{
    if (deletionFlag == DELETE_LIST)
        delete blocks;
}

BasicBlock *
BlockListIterator::head() const
{
    return *pos;
}

BlockListIterator *
BlockListIterator::tail() const
{
    BlockListIterator *t = new BlockListIterator(cfg, blocks, pos+1);
    cfg->add_iteratorToDelete(t);
    return t;
}

bool
BlockListIterator::empty() const
{
    return (pos == blocks->end());
}

int
BlockListIterator::size() const
{
    BlockList::iterator i;
    int size = 0;
    for (i = pos; i != blocks->end(); ++i)
        size++;
    return size;
}
#endif

// GB (2008-04-07): The code below has great similarities for all of our
// statement classes; it is a subset of the functions that are generated by
// ROSETTA for ROSE. We also generate skeletons that are hand-hacked where
// necessary. Below you will find the code template and the simple shell
// script that creates code from it.

/* This is the script that instantiates the templates.

#!/bin/sh

# This is default_funcs.sh, a very simple "code generator" that instantiates
# a code template passed as the first command line argument. Every
# occurrence of "$CLASSNAME" in the template is replaced by a class name.

# The classes to instantiate the code with.
CLASSNAMES="IcfgStmt CallStmt FunctionCall FunctionReturn FunctionEntry DeclareStmt UndeclareStmt ExternalCall ExternalReturn ConstructorCall DestructorCall ArgumentAssignment MyAssignment ReturnAssignment ParamAssignment LogicalIf IfJoin WhileJoin FunctionExit"

for c in $CLASSNAMES
do
    sed -s "s/\$CLASSNAME/$c/g" < $1
done

# Wow, that was easy.

 */

/* This is the code template.

// BEGIN code for $CLASSNAME
std::vector<SgNode *>
$CLASSNAME::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
$CLASSNAME::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
$CLASSNAME::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in $CLASSNAME::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
$CLASSNAME::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

$CLASSNAME *
is$CLASSNAME(SgNode *node)
{
    return dynamic_cast<$CLASSNAME *>(node);
}

const $CLASSNAME *
is$CLASSNAME(const SgNode *node)
{
    return dynamic_cast<const $CLASSNAME *>(node);
}

std::string
$CLASSNAME::class_name() const
{
    return "$CLASSNAME";
}

SatireVariant
$CLASSNAME::satireVariant() const
{
    return V_$CLASSNAME;
}
// END code for $CLASSNAME

 */

// Semi-generated code follows. The traversal successors were inserted by
// hand because they are not completely trivial to generate.
// The traversal functions should not be called on IcfgStmt, CallStmt, and
// MyAssignment, only on their concrete derived classes. This is not
// checked, however.

// BEGIN code for IcfgStmt
std::vector<SgNode *>
IcfgStmt::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
IcfgStmt::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
IcfgStmt::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in IcfgStmt::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
IcfgStmt::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

IcfgStmt *
isIcfgStmt(SgNode *node)
{
    return dynamic_cast<IcfgStmt *>(node);
}

const IcfgStmt *
isIcfgStmt(const SgNode *node)
{
    return dynamic_cast<const IcfgStmt *>(node);
}

std::string
IcfgStmt::class_name() const
{
    return "IcfgStmt";
}

SatireVariant
IcfgStmt::satireVariant() const
{
    return V_IcfgStmt;
}
// END code for IcfgStmt

// BEGIN code for CallStmt
std::vector<SgNode *>
CallStmt::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
CallStmt::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
CallStmt::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in CallStmt::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
CallStmt::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

CallStmt *
isCallStmt(SgNode *node)
{
    return dynamic_cast<CallStmt *>(node);
}

const CallStmt *
isCallStmt(const SgNode *node)
{
    return dynamic_cast<const CallStmt *>(node);
}

std::string
CallStmt::class_name() const
{
    return "CallStmt";
}

SatireVariant
CallStmt::satireVariant() const
{
    return V_CallStmt;
}
// END code for CallStmt

// BEGIN code for FunctionCall
std::vector<SgNode *>
FunctionCall::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
FunctionCall::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
FunctionCall::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in FunctionCall::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
FunctionCall::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

FunctionCall *
isFunctionCall(SgNode *node)
{
    return dynamic_cast<FunctionCall *>(node);
}

const FunctionCall *
isFunctionCall(const SgNode *node)
{
    return dynamic_cast<const FunctionCall *>(node);
}

std::string
FunctionCall::class_name() const
{
    return "FunctionCall";
}

SatireVariant
FunctionCall::satireVariant() const
{
    return V_FunctionCall;
}
// END code for FunctionCall

// BEGIN code for FunctionReturn
std::vector<SgNode *>
FunctionReturn::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
FunctionReturn::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
FunctionReturn::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in FunctionReturn::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
FunctionReturn::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

FunctionReturn *
isFunctionReturn(SgNode *node)
{
    return dynamic_cast<FunctionReturn *>(node);
}

const FunctionReturn *
isFunctionReturn(const SgNode *node)
{
    return dynamic_cast<const FunctionReturn *>(node);
}

std::string
FunctionReturn::class_name() const
{
    return "FunctionReturn";
}

SatireVariant
FunctionReturn::satireVariant() const
{
    return V_FunctionReturn;
}
// END code for FunctionReturn

// BEGIN code for FunctionEntry
std::vector<SgNode *>
FunctionEntry::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
FunctionEntry::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
FunctionEntry::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in FunctionEntry::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
FunctionEntry::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

FunctionEntry *
isFunctionEntry(SgNode *node)
{
    return dynamic_cast<FunctionEntry *>(node);
}

const FunctionEntry *
isFunctionEntry(const SgNode *node)
{
    return dynamic_cast<const FunctionEntry *>(node);
}

std::string
FunctionEntry::class_name() const
{
    return "FunctionEntry";
}

SatireVariant
FunctionEntry::satireVariant() const
{
    return V_FunctionEntry;
}
// END code for FunctionEntry

// BEGIN code for DeclareStmt
std::vector<SgNode *>
DeclareStmt::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
DeclareStmt::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
DeclareStmt::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in DeclareStmt::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
DeclareStmt::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

DeclareStmt *
isDeclareStmt(SgNode *node)
{
    return dynamic_cast<DeclareStmt *>(node);
}

const DeclareStmt *
isDeclareStmt(const SgNode *node)
{
    return dynamic_cast<const DeclareStmt *>(node);
}

std::string
DeclareStmt::class_name() const
{
    return "DeclareStmt";
}

SatireVariant
DeclareStmt::satireVariant() const
{
    return V_DeclareStmt;
}
// END code for DeclareStmt

// BEGIN code for UndeclareStmt
std::vector<SgNode *>
UndeclareStmt::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
UndeclareStmt::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
UndeclareStmt::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in UndeclareStmt::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
UndeclareStmt::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

UndeclareStmt *
isUndeclareStmt(SgNode *node)
{
    return dynamic_cast<UndeclareStmt *>(node);
}

const UndeclareStmt *
isUndeclareStmt(const SgNode *node)
{
    return dynamic_cast<const UndeclareStmt *>(node);
}

std::string
UndeclareStmt::class_name() const
{
    return "UndeclareStmt";
}

SatireVariant
UndeclareStmt::satireVariant() const
{
    return V_UndeclareStmt;
}
// END code for UndeclareStmt

// BEGIN code for ExternalCall
std::vector<SgNode *>
ExternalCall::get_traversalSuccessorContainer()
{
 // GB: Added traversal successors.
    std::vector<SgNode *> successors;
    successors.push_back(function);
    return successors;
}

size_t
ExternalCall::get_numberOfTraversalSuccessors()
{
 // GB: Added traversal successors.
    return 1;
}

SgNode *
ExternalCall::get_traversalSuccessorByIndex(size_t idx)
{
 // GB: Added traversal successors.
    switch (idx)
    {
    case 0:
        return function;
    default:
        std::cerr
            << "error in ExternalCall::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
ExternalCall::get_traversalSuccessorNamesContainer()
{
 // GB: Added traversal successors.
    std::vector<std::string> successorNames;
    successorNames.push_back("function");
    return successorNames;
}

ExternalCall *
isExternalCall(SgNode *node)
{
    return dynamic_cast<ExternalCall *>(node);
}

const ExternalCall *
isExternalCall(const SgNode *node)
{
    return dynamic_cast<const ExternalCall *>(node);
}

std::string
ExternalCall::class_name() const
{
    return "ExternalCall";
}

SatireVariant
ExternalCall::satireVariant() const
{
    return V_ExternalCall;
}
// END code for ExternalCall

// BEGIN code for ExternalReturn
std::vector<SgNode *>
ExternalReturn::get_traversalSuccessorContainer()
{
 // GB: Added traversal successors.
    std::vector<SgNode *> successors;
    successors.push_back(function);
    return successors;
}

size_t
ExternalReturn::get_numberOfTraversalSuccessors()
{
 // GB: Added traversal successors.
    return 1;
}

SgNode *
ExternalReturn::get_traversalSuccessorByIndex(size_t idx)
{
 // GB: Added traversal successors.
    switch (idx)
    {
    case 0:
        return function;
    default:
        std::cerr
            << "error in ExternalReturn::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
ExternalReturn::get_traversalSuccessorNamesContainer()
{
 // GB: Added traversal successors.
    std::vector<std::string> successorNames;
    successorNames.push_back("function");
    return successorNames;
}

ExternalReturn *
isExternalReturn(SgNode *node)
{
    return dynamic_cast<ExternalReturn *>(node);
}

const ExternalReturn *
isExternalReturn(const SgNode *node)
{
    return dynamic_cast<const ExternalReturn *>(node);
}

std::string
ExternalReturn::class_name() const
{
    return "ExternalReturn";
}

SatireVariant
ExternalReturn::satireVariant() const
{
    return V_ExternalReturn;
}
// END code for ExternalReturn

// BEGIN code for ConstructorCall
std::vector<SgNode *>
ConstructorCall::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
ConstructorCall::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
ConstructorCall::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in ConstructorCall::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
ConstructorCall::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

ConstructorCall *
isConstructorCall(SgNode *node)
{
    return dynamic_cast<ConstructorCall *>(node);
}

const ConstructorCall *
isConstructorCall(const SgNode *node)
{
    return dynamic_cast<const ConstructorCall *>(node);
}

std::string
ConstructorCall::class_name() const
{
    return "ConstructorCall";
}

SatireVariant
ConstructorCall::satireVariant() const
{
    return V_ConstructorCall;
}
// END code for ConstructorCall

// BEGIN code for DestructorCall
std::vector<SgNode *>
DestructorCall::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
DestructorCall::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
DestructorCall::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in DestructorCall::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
DestructorCall::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

DestructorCall *
isDestructorCall(SgNode *node)
{
    return dynamic_cast<DestructorCall *>(node);
}

const DestructorCall *
isDestructorCall(const SgNode *node)
{
    return dynamic_cast<const DestructorCall *>(node);
}

std::string
DestructorCall::class_name() const
{
    return "DestructorCall";
}

SatireVariant
DestructorCall::satireVariant() const
{
    return V_DestructorCall;
}
// END code for DestructorCall

// BEGIN code for ArgumentAssignment
std::vector<SgNode *>
ArgumentAssignment::get_traversalSuccessorContainer()
{
 // GB: Added traversal successors.
    std::vector<SgNode *> successors;
    successors.push_back(lhs);
    successors.push_back(rhs);
    return successors;
}

size_t
ArgumentAssignment::get_numberOfTraversalSuccessors()
{
 // GB: Added traversal successors.
    return 2;
}

SgNode *
ArgumentAssignment::get_traversalSuccessorByIndex(size_t idx)
{
 // GB: Added traversal successors.
    switch (idx)
    {
    case 0:
        return lhs;
    case 1:
        return rhs;
    default:
        std::cerr
            << "error in ArgumentAssignment::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
ArgumentAssignment::get_traversalSuccessorNamesContainer()
{
 // GB: Added traversal successors.
    std::vector<std::string> successorNames;
    successorNames.push_back("lhs");
    successorNames.push_back("rhs");
    return successorNames;
}

ArgumentAssignment *
isArgumentAssignment(SgNode *node)
{
    return dynamic_cast<ArgumentAssignment *>(node);
}

const ArgumentAssignment *
isArgumentAssignment(const SgNode *node)
{
    return dynamic_cast<const ArgumentAssignment *>(node);
}

std::string
ArgumentAssignment::class_name() const
{
    return "ArgumentAssignment";
}

SatireVariant
ArgumentAssignment::satireVariant() const
{
    return V_ArgumentAssignment;
}
// END code for ArgumentAssignment

// BEGIN code for MyAssignment
std::vector<SgNode *>
MyAssignment::get_traversalSuccessorContainer()
{
 // GB: Added traversal successors.
    std::vector<SgNode *> successors;
    successors.push_back(lhsVarRefExp);
    successors.push_back(rhsVarRefExp);
    return successors;
}

size_t
MyAssignment::get_numberOfTraversalSuccessors()
{
 // GB: Added traversal successors.
    return 2;
}

SgNode *
MyAssignment::get_traversalSuccessorByIndex(size_t idx)
{
 // GB: Added traversal successors.
    switch (idx)
    {
    case 0:
        return lhsVarRefExp;
    case 1:
        return rhsVarRefExp;
    default:
        std::cerr
            << "error in MyAssignment::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
MyAssignment::get_traversalSuccessorNamesContainer()
{
 // GB: Added traversal successors.
    std::vector<std::string> successorNames;
    successorNames.push_back("lhsVarRefExp");
    successorNames.push_back("rhsVarRefExp");
    return successorNames;
}

MyAssignment *
isMyAssignment(SgNode *node)
{
    return dynamic_cast<MyAssignment *>(node);
}

const MyAssignment *
isMyAssignment(const SgNode *node)
{
    return dynamic_cast<const MyAssignment *>(node);
}

std::string
MyAssignment::class_name() const
{
    return "MyAssignment";
}

SatireVariant
MyAssignment::satireVariant() const
{
    return V_MyAssignment;
}
// END code for MyAssignment

// BEGIN code for ReturnAssignment
std::vector<SgNode *>
ReturnAssignment::get_traversalSuccessorContainer()
{
 // GB: Added traversal successors.
    std::vector<SgNode *> successors;
    successors.push_back(lhsVarRefExp);
    successors.push_back(rhsVarRefExp);
    return successors;
}

size_t
ReturnAssignment::get_numberOfTraversalSuccessors()
{
 // GB: Added traversal successors.
    return 2;
}

SgNode *
ReturnAssignment::get_traversalSuccessorByIndex(size_t idx)
{
 // GB: Added traversal successors.
    switch (idx)
    {
    case 0:
        return lhsVarRefExp;
    case 1:
        return rhsVarRefExp;
    default:
        std::cerr
            << "error in ReturnAssignment::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
ReturnAssignment::get_traversalSuccessorNamesContainer()
{
 // GB: Added traversal successors.
    std::vector<std::string> successorNames;
    successorNames.push_back("lhsVarRefExp");
    successorNames.push_back("rhsVarRefExp");
    return successorNames;
}

ReturnAssignment *
isReturnAssignment(SgNode *node)
{
    return dynamic_cast<ReturnAssignment *>(node);
}

const ReturnAssignment *
isReturnAssignment(const SgNode *node)
{
    return dynamic_cast<const ReturnAssignment *>(node);
}

std::string
ReturnAssignment::class_name() const
{
    return "ReturnAssignment";
}

SatireVariant
ReturnAssignment::satireVariant() const
{
    return V_ReturnAssignment;
}
// END code for ReturnAssignment

// BEGIN code for ParamAssignment
std::vector<SgNode *>
ParamAssignment::get_traversalSuccessorContainer()
{
 // GB: Added traversal successors.
    std::vector<SgNode *> successors;
    successors.push_back(lhsVarRefExp);
    successors.push_back(rhsVarRefExp);
    return successors;
}

size_t
ParamAssignment::get_numberOfTraversalSuccessors()
{
 // GB: Added traversal successors.
    return 2;
}

SgNode *
ParamAssignment::get_traversalSuccessorByIndex(size_t idx)
{
 // GB: Added traversal successors.
    switch (idx)
    {
    case 0:
        return lhsVarRefExp;
    case 1:
        return rhsVarRefExp;
    default:
        std::cerr
            << "error in ParamAssignment::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
ParamAssignment::get_traversalSuccessorNamesContainer()
{
 // GB: Added traversal successors.
    std::vector<std::string> successorNames;
    successorNames.push_back("lhsVarRefExp");
    successorNames.push_back("rhsVarRefExp");
    return successorNames;
}

ParamAssignment *
isParamAssignment(SgNode *node)
{
    return dynamic_cast<ParamAssignment *>(node);
}

const ParamAssignment *
isParamAssignment(const SgNode *node)
{
    return dynamic_cast<const ParamAssignment *>(node);
}

std::string
ParamAssignment::class_name() const
{
    return "ParamAssignment";
}

SatireVariant
ParamAssignment::satireVariant() const
{
    return V_ParamAssignment;
}
// END code for ParamAssignment

// BEGIN code for LogicalIf
std::vector<SgNode *>
LogicalIf::get_traversalSuccessorContainer()
{
 // GB: Added traversal successors.
    std::vector<SgNode *> successors;
    successors.push_back(expr);
    return successors;
}

size_t
LogicalIf::get_numberOfTraversalSuccessors()
{
 // GB: Added traversal successors.
    return 1;
}

SgNode *
LogicalIf::get_traversalSuccessorByIndex(size_t idx)
{
 // GB: Added traversal successors.
    switch (idx)
    {
    case 0:
        return expr;
    default:
        std::cerr
            << "error in LogicalIf::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
LogicalIf::get_traversalSuccessorNamesContainer()
{
 // GB: Added traversal successors.
    std::vector<std::string> successorNames;
    successorNames.push_back("expr");
    return successorNames;
}

LogicalIf *
isLogicalIf(SgNode *node)
{
    return dynamic_cast<LogicalIf *>(node);
}

const LogicalIf *
isLogicalIf(const SgNode *node)
{
    return dynamic_cast<const LogicalIf *>(node);
}

std::string
LogicalIf::class_name() const
{
    return "LogicalIf";
}

SatireVariant
LogicalIf::satireVariant() const
{
    return V_LogicalIf;
}
// END code for LogicalIf

// BEGIN code for IfJoin
std::vector<SgNode *>
IfJoin::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
IfJoin::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
IfJoin::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in IfJoin::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
IfJoin::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

IfJoin *
isIfJoin(SgNode *node)
{
    return dynamic_cast<IfJoin *>(node);
}

const IfJoin *
isIfJoin(const SgNode *node)
{
    return dynamic_cast<const IfJoin *>(node);
}

std::string
IfJoin::class_name() const
{
    return "IfJoin";
}

SatireVariant
IfJoin::satireVariant() const
{
    return V_IfJoin;
}
// END code for IfJoin

// BEGIN code for WhileJoin
std::vector<SgNode *>
WhileJoin::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
WhileJoin::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
WhileJoin::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in WhileJoin::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
WhileJoin::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

WhileJoin *
isWhileJoin(SgNode *node)
{
    return dynamic_cast<WhileJoin *>(node);
}

const WhileJoin *
isWhileJoin(const SgNode *node)
{
    return dynamic_cast<const WhileJoin *>(node);
}

std::string
WhileJoin::class_name() const
{
    return "WhileJoin";
}

SatireVariant
WhileJoin::satireVariant() const
{
    return V_WhileJoin;
}
// END code for WhileJoin

// BEGIN code for FunctionExit
std::vector<SgNode *>
FunctionExit::get_traversalSuccessorContainer()
{
    std::vector<SgNode *> successors;
    return successors;
}

size_t
FunctionExit::get_numberOfTraversalSuccessors()
{
    return 0;
}

SgNode *
FunctionExit::get_traversalSuccessorByIndex(size_t idx)
{
    switch (idx)
    {
    default:
        std::cerr
            << "error in FunctionExit::get_traversalSuccessorByIndex: "
            << " invalid index " << idx << ", outside of expected range "
            << "[0, " << get_numberOfTraversalSuccessors() << ")"
            << std::endl;
        abort();
    }
}

std::vector<std::string>
FunctionExit::get_traversalSuccessorNamesContainer()
{
    std::vector<std::string> successorNames;
    return successorNames;
}

FunctionExit *
isFunctionExit(SgNode *node)
{
    return dynamic_cast<FunctionExit *>(node);
}

const FunctionExit *
isFunctionExit(const SgNode *node)
{
    return dynamic_cast<const FunctionExit *>(node);
}

std::string
FunctionExit::class_name() const
{
    return "FunctionExit";
}

SatireVariant
FunctionExit::satireVariant() const
{
    return V_FunctionExit;
}
// END code for FunctionExit
