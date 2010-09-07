// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"
#include "patternRewrite.h"
#include "expressionTreeEqual.h"
#include <vector>
#include <iostream>

// DQ (8/1/2005): test use of new static function to create 
// Sg_File_Info object that are marked as transformations
#undef SgNULL_FILE
#define SgNULL_FILE Sg_File_Info::generateDefaultFileInfoForTransformationNode()

using namespace std;

bool RewriteRuleCombiner::doRewrite(SgNode*& n) const {
  for (vector<RewriteRule*>::const_iterator i = rules.begin();
      i != rules.end(); ++i) {
    if ((*i)->doRewrite(n)) {
      // cout << "Rewriting using rule " << i - rules.begin() << " on node (new) " << n << " of type " << n->class_name() << endl;
      return true;
    }
  }
  return false;
}

class DoRewriteRuleDeepVisitor: public AstSimpleProcessing {
  RewriteRule* rule;
  bool done;
  SgNode* top;

  public:
  DoRewriteRuleDeepVisitor(RewriteRule* rule, SgNode* top)
    : rule(rule), done(false), top(top) {}

  virtual void visit(SgNode* n) {
    SgNode* oldN = n;
    if (done) return;
    // cout << "DoRewriteRuleDeepVisitor " << n->unparseToString() << endl;
    if (rule->doRewrite(n)) {
      // cout << "Before set done" << endl;
      done = true; // Only do one rewrite rule at a time
      if (oldN == top) top = n;
    }
  }

  bool isDone() const {
    return done;
  }

  SgNode* getNewNode() const {return top;}
};

bool doRewriteRuleDeep(RewriteRule* rule, SgNode*& n) {
  DoRewriteRuleDeepVisitor vis(rule, n);
  vis.traverse(n, preorder);
  n = vis.getNewNode();
  // cout << "After visitor " << vis.isDone() << endl;
  return vis.isDone();
}

void rewrite(RewriteRule* rule, SgNode*& top) {
  // Iterate until no changes are made
  while (doRewriteRuleDeep(rule, top)) {
    // cout << top->unparseToString() << endl;
  }
}

void replaceChild(SgNode* parent, SgNode* from, SgNode* to) {
  ROSE_ASSERT (parent);
  ROSE_ASSERT (from);
  ROSE_ASSERT (to);
  ROSE_ASSERT (from != to);
  ROSE_ASSERT (from != parent);
  ROSE_ASSERT (to != parent);
  if (parent->get_childIndex(from) == (size_t)(-1)) {
    cerr << "From not found: from is a " << from->class_name() << " and parent is a " << parent->class_name() << endl;
    ROSE_ASSERT (false);
  }
  ROSE_ASSERT (parent->get_childIndex(to) == (size_t)(-1));
  if (isSgExpression(parent) && isSgExpression(from) && isSgExpression(to)) {
    to->set_parent(parent);
    isSgExpression(parent)->replace_expression(
      isSgExpression(from), isSgExpression(to));
    ROSE_ASSERT(parent->get_childIndex(from) == (size_t)(-1));
    ROSE_ASSERT(parent->get_childIndex(to) != (size_t)(-1));
    return;
  }
  if (isSgExprStatement(parent) && isSgExpression(to)) {
    to->set_parent(parent);
    isSgExprStatement(parent)->set_expression(isSgExpression(to));
    ROSE_ASSERT(parent->get_childIndex(from) == (size_t)(-1));
    ROSE_ASSERT(parent->get_childIndex(to) != (size_t)(-1));
    return;
  }
  cout << parent->sage_class_name() << " " << from->sage_class_name() << " " << to->sage_class_name() << endl;
  ROSE_ASSERT (!"replaceChild FIXME");
}

bool PatternActionRule::doRewrite(SgNode*& n) const {
  PatternVariables vars;
  if (pattern->match(n, vars)) {
#if 0
    cout << "Beginning of vars" << endl;
    for (PatternVariables::iterator i = vars.begin(); i != vars.end(); ++i)
      cout << i->first << " -> " << i->second->unparseToString() << endl;
    cout << "End of vars" << endl;
    cout << "Before" << endl;
    cout << "After " << n2->unparseToString() << endl;
#endif
    SgNode* n2 = action->subst(vars);
    replaceChild(n->get_parent(), n, n2);
    n = n2;
#if 0
    cout << "After replaceChild" << endl;
#endif
    return true;
  } else
    return false;
}

PatternActionRule* patact(Pattern* pattern, Pattern* action) {
  return new PatternActionRule(pattern, action);
}

template <class Operator>
class UnaryPattern: public Pattern {
  Pattern* rhs;

  public:
  UnaryPattern(Pattern* rhs): rhs(rhs) {}

  virtual bool match(SgNode* top, PatternVariables& vars) const {
    if (dynamic_cast<Operator*>(top)) {
      Operator* t = dynamic_cast<Operator*>(top);
      return rhs->match(t->get_rhs_operand(), vars);
    } else
      return false;
  }

  virtual SgNode* subst(PatternVariables& vars) const {
    SgNode* new_rhs = rhs->subst(vars);
    ROSE_ASSERT (isSgExpression(new_rhs));
    SgExpression* op = new Operator(SgNULL_FILE, isSgExpression(new_rhs));
    op->set_endOfConstruct(SgNULL_FILE);
    new_rhs->set_parent(op);
    return op;
  }
};

template <class Operator>
class BinaryPattern: public Pattern {
  Pattern* lhs;
  Pattern* rhs;

  public:
  BinaryPattern(Pattern* lhs, Pattern* rhs): lhs(lhs), rhs(rhs) {}

  virtual bool match(SgNode* top, PatternVariables& vars) const {
    if (dynamic_cast<Operator*>(top)) {
      Operator* t = dynamic_cast<Operator*>(top);
      return lhs->match(t->get_lhs_operand(), vars) &&
             rhs->match(t->get_rhs_operand(), vars);
    } else
      return false;
  }

  virtual SgNode* subst(PatternVariables& vars) const {
    SgNode* new_lhs = lhs->subst(vars);
    SgNode* new_rhs = rhs->subst(vars);
    ROSE_ASSERT (isSgExpression(new_lhs));
    ROSE_ASSERT (isSgExpression(new_rhs));
    SgExpression* op = new Operator(SgNULL_FILE, isSgExpression(new_lhs), 
                                                 isSgExpression(new_rhs));
    op->set_endOfConstruct(SgNULL_FILE);
    new_lhs->set_parent(op);
    new_rhs->set_parent(op);
    return op;
  }
};

template <class NodeClass>
class VariablePattern: public Pattern {
  std::string name;

  public:
  VariablePattern(std::string name): name(name) {}

  virtual bool match(SgNode* n, PatternVariables& vars) const {
    NodeClass* n2 = dynamic_cast<NodeClass*>(n);
    if (n2 && 
	(!vars[name] || 
         (isSgExpression(n2) && isSgExpression(vars[name]) &&
	   expressionTreeEqual(isSgExpression(n2), 
                               isSgExpression(vars[name]))))) {
      vars[name] = n2;
      return true;
    } else
      return false;
  }

  virtual SgNode* subst(PatternVariables& vars) const {
    SgNode* binding = vars[name];
    ROSE_ASSERT (binding);
    return binding;
  }
};

class NullPattern: public Pattern {
  virtual bool match(SgNode* n, PatternVariables& vars) const {
    return true;
  }

  virtual SgNode* subst(PatternVariables& vars) const {
    ROSE_ASSERT (!"Should not use NullPattern in substitutions");

#ifdef _MSC_VER
	#pragma message ("patternrewrite: Returning arbitrary value")
    return false; // Windows requires return value
#endif
  }
};

template <class NodeClass, class Data>
class ConstantPattern: public Pattern {
  Data value;

  public:
  ConstantPattern(Data value): value(value) {}

  virtual bool match(SgNode* n, PatternVariables& vars) const {
    NodeClass* n2 = dynamic_cast<NodeClass*>(n);
    if (n2 && n2->get_value() == value) {
      return true;
    } else
      return false;
  }

  virtual SgNode* subst(PatternVariables& vars) const {
    NodeClass* n = new NodeClass(SgNULL_FILE, value);
    n->set_endOfConstruct(SgNULL_FILE);
    return n;
  }
};

Pattern* p_AddOp(Pattern* lhs, Pattern* rhs) {
  return new BinaryPattern<SgAddOp>(lhs, rhs);
}

Pattern* p_MultiplyOp(Pattern* lhs, Pattern* rhs) {
  return new BinaryPattern<SgMultiplyOp>(lhs, rhs);
}

Pattern* p_PlusAssignOp(Pattern* lhs, Pattern* rhs) {
  return new BinaryPattern<SgPlusAssignOp>(lhs, rhs);
}

Pattern* p_CommaOp(Pattern* lhs, Pattern* rhs) {
  return new BinaryPattern<SgCommaOpExp>(lhs, rhs);
}

Pattern* p_var(std::string name) {
  return new VariablePattern<SgNode>(name);
}

Pattern* p_value(std::string name) {
  return new VariablePattern<SgValueExp>(name);
}

Pattern* p_int(int x) {
  return new ConstantPattern<SgIntVal, int>(x);
}

Pattern* p_wildcard = new NullPattern();

class AddIntsPattern: public Pattern {
  string a, b;

  public:
  AddIntsPattern(string a, string b): a(a), b(b) {}

  virtual bool match(SgNode* top, PatternVariables& vars) const {
    ROSE_ASSERT (false);

#ifdef _MSC_VER
	#pragma message ("patternRewrite: Returning arbitrary value")
    return false; // Windows requires return value
#endif
  }

  virtual SgNode* subst(PatternVariables& vars) const {
    SgIntVal* av = isSgIntVal(vars[a]);
    SgIntVal* bv = isSgIntVal(vars[b]);
    ROSE_ASSERT (av && bv);
    SgIntVal* iv = new SgIntVal(SgNULL_FILE, av->get_value() + bv->get_value());
    iv->set_endOfConstruct(SgNULL_FILE);
    return iv;
  }
};

class MoveConstantsToLeftInMultiply: public RewriteRule {
  public:
  bool doRewrite(SgNode*& n) const {
    SgMultiplyOp* m = isSgMultiplyOp(n);
    if (!m) return false;
    if (!isSgValueExp(m->get_rhs_operand())) return false;
    if (isSgValueExp(m->get_lhs_operand())) return false;
    SgExpression* lhs = m->get_lhs_operand();
    m->set_lhs_operand(m->get_rhs_operand());
    m->set_rhs_operand(lhs);
    return true;
  }
};

class FoldIntConstantsInMultiply: public RewriteRule {
  public:
  bool doRewrite(SgNode*& n) const {
    SgMultiplyOp* m = isSgMultiplyOp(n);
    if (!m) return false;
    SgIntVal* lhs = isSgIntVal(m->get_lhs_operand());
    SgIntVal* rhs = isSgIntVal(m->get_rhs_operand());
    if (!lhs || !rhs) return false;
    SgIntVal* iv = new SgIntVal(SgNULL_FILE, lhs->get_value() * rhs->get_value());
    iv->set_endOfConstruct(SgNULL_FILE);
    replaceChild(n->get_parent(), n, iv);
    n = iv;
    return true;
  }
};

RewriteRule* getAlgebraicRules() {
  RewriteRuleCombiner* rules = new RewriteRuleCombiner();
  rules->add(patact(p_AddOp(p_value("a"), p_value("b")),
		    new AddIntsPattern("a", "b")));
  rules->add(patact(p_AddOp(p_AddOp(p_var("a"), p_value("b")), p_value("c")),
		    p_AddOp(p_var("a"), new AddIntsPattern("b", "c"))));
  rules->add(new MoveConstantsToLeftInMultiply());
  rules->add(new FoldIntConstantsInMultiply());
  rules->add(patact(p_MultiplyOp(p_AddOp(p_var("a"), p_var("b")), p_var("c")),
		    p_AddOp(p_MultiplyOp(p_var("a"), p_var("c")),
			    p_MultiplyOp(p_var("b"), p_var("c")))));
  rules->add(patact(p_MultiplyOp(p_var("c"), p_AddOp(p_var("a"), p_var("b"))),
		    p_AddOp(p_MultiplyOp(p_var("c"), p_var("a")),
			    p_MultiplyOp(p_var("c"), p_var("b")))));
  rules->add(patact(p_AddOp(p_var("a"), p_AddOp(p_var("b"), p_var("c"))),
		    p_AddOp(p_AddOp(p_var("a"), p_var("b")), p_var("c"))));
  rules->add(patact(p_MultiplyOp(p_int(1), p_var("a")),
		    p_var("a")));
  rules->add(patact(p_MultiplyOp(p_var("a"), p_int(1)),
		    p_var("a")));
  rules->add(patact(p_MultiplyOp(p_int(0), p_var("a")),
		    p_int(0)));
  rules->add(patact(p_MultiplyOp(p_var("a"), p_int(0)),
		    p_int(0)));
  return rules;
}

RewriteRule* getFiniteDifferencingRules() {
  RewriteRuleCombiner* rules = (RewriteRuleCombiner*)getAlgebraicRules();
  rules->add(patact(
    p_CommaOp(p_var("var"), 
      p_CommaOp(p_var("lhs"), p_AddOp(p_var("lhs"), p_var("rhs")))),
    p_PlusAssignOp(p_var("var"), p_var("rhs"))));
  rules->add(patact(
    p_CommaOp(p_var("var"), 
      p_CommaOp(p_var("lhs"), p_AddOp(p_var("rhs"), p_var("lhs")))),
    p_PlusAssignOp(p_var("var"), p_var("rhs"))));
  return rules;
}
