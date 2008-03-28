// Copyright 2005,2006,2007 Markus Schordan, Gergo Barany
// $Id: cfg_support.C,v 1.13 2008-03-28 10:36:25 gergo Exp $

#include "CFGTraversal.h"
#include "cfg_support.h"
#include "IrCreation.h"

CallBlock::CallBlock(KFG_NODE_ID id_, KFG_NODE_TYPE type_, int procnum_,
		     std::vector<SgVariableSymbol *> *paramlist_, const char *name_)
  : BasicBlock(id_, type_, procnum_), paramlist(paramlist_), name(name_)
{
  switch (node_type) {
  case X_FunctionEntry:
    statements.push_back(stmt = Ir::createFunctionEntry(node_type, name, this));
    break;
  case X_FunctionExit:
    statements.push_back(stmt = Ir::createFunctionExit(node_type, name, this));
    break;
  default:
    statements.push_back(stmt = Ir::createCallStmt(node_type, name, this));
    }
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

// GB (2007-10-23): Added this function to unparse the new members of the
// ExternalCall node (the function expression and the list of parameter
// variables).
std::string ExternalCall::unparseToString() const
{
    std::stringstream label;
    label << "ExternalCall(" << expr_to_string(function) << ", [";
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

CallStmt::CallStmt(KFG_NODE_TYPE type_, const char *name_, CallBlock *parent_)
  : type(type_), name(name_), parent(parent_) 
{
  update_infolabel();
}

void 
CallStmt::update_infolabel() 
{
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

  const char* retval = strdup(Ir::fragmentToString(expr).c_str());
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

const char*
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
    rhs = Ir::createVarRefExp(varnameattr->get_str(),r->get_type());
  } else if (isSgConstructorInitializer(r)
	     && r->attributeExists("anonymous variable")) {
    RetvalAttribute *varnameattr
      = (RetvalAttribute *) r->getAttribute("anonymous variable");
    rhs = Ir::createVarRefExp(varnameattr->get_str(),r->get_type());
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
  buf += expr_to_string(lhs);
  buf += ", ";
  buf += expr_to_string(rhs);
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
    rhsVarRefExp(Ir::createVarRefExp(rhs)) {
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
  std::string this_var_name = std::string("$~") + class_name + "$this";
  std::deque<Procedure *>::const_iterator i;
  for (i = cfg->procedures->begin(); i != cfg->procedures->end(); ++i) {
    /* we want member functions */
    if ((*i)->memberf_name == NULL)
      break;
    if (destructor_name == (*i)->memberf_name) {
      SgVariableSymbol* this_var_sym = Ir::createVariableSymbol(this_var_name, in->get_type());

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

      SgAddressOfOp* addressOfOp
	= Ir::createAddressOfOp(Ir::createVarRefExp(Ir::createVariableSymbol(in)),
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
    b->statements.push_back(Ir::createDestructorCall(strdup(class_name.c_str()), ct));
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
