#ifndef PATTERNREWRITE_H
#define PATTERNREWRITE_H

// #include "config.h"

//#include "rose.h"
#include <vector>

//! Definition of a rewrite rule on a given SgNode.  The rule modifies the node
//! and/or its children in-place, and then returns true for success or false for
//! failure.
class RewriteRule {
  public:
  virtual bool doRewrite(SgNode*& n) const = 0;
// Liao (2/7/2008): Added destructor to eliminate g++ compiler warning.
  virtual ~RewriteRule(){}
};

//! Combine several rules into one, doing the constituents in order
class RewriteRuleCombiner: public RewriteRule {
  std::vector<RewriteRule*> rules;

  public:
  std::vector<RewriteRule*>& getRules() {return rules;}
  const std::vector<RewriteRule*>& getRules() const {return rules;}

  void add(RewriteRule* r) {rules.push_back(r);}

  virtual bool doRewrite(SgNode*& n) const;

// DQ (12/16/2006): Added destructor to eliminate g++ compiler warning.
  virtual ~RewriteRuleCombiner() {}      
};

//! Rewrite a node and its children recursively using a rule.  Iterate until no
//! more changes can be made.
void rewrite(RewriteRule* rule, SgNode*& top);

//! Replace a child of a node.
void replaceChild(SgNode* parent, SgNode* from, SgNode* to);

typedef std::map<std::string, SgNode*> PatternVariables;

//! Abstract interface for a pattern which can match ASTs
class Pattern {
  public:
  virtual bool match(SgNode* top, PatternVariables& vars) const = 0;
  virtual SgNode* subst(PatternVariables& vars) const = 0;
// Liao (2/7/2008): Added destructor to eliminate g++ compiler warning.
  virtual ~Pattern(){}
};

//! A rule which changes one pattern of code to another
class PatternActionRule: public RewriteRule {
  Pattern* pattern;
  Pattern* action;

  public:
  PatternActionRule(Pattern* pattern, Pattern* action):
    pattern(pattern), action(action) {}

  virtual bool doRewrite(SgNode*& n) const;
// Liao (2/7/2008): Added destructor to eliminate g++ compiler warning.
  virtual ~PatternActionRule(){}
};

//! Create a PatternActionRule
PatternActionRule* patact(Pattern* pattern, Pattern* action);

//! Match an addition operation
Pattern* p_AddOp(Pattern* lhs, Pattern* rhs);

//! Match a multiplication operation
Pattern* p_MultiplyOp(Pattern* lhs, Pattern* rhs);

//! Match an add-assign operation
Pattern* p_PlusAssignOp(Pattern* lhs, Pattern* rhs);

//! Match a comma operation
Pattern* p_CommaOp(Pattern* lhs, Pattern* rhs);

//! Match anything into a variable
Pattern* p_var(std::string name);

//! Match any constant into a variable
Pattern* p_value(std::string name);

//! Match a particular integer
Pattern* p_int(int x);

//! Match anything
extern Pattern* p_wildcard;

//! A standard set of algebraic rules for simple optimizations
RewriteRule* getAlgebraicRules();

//! A standard set of algebraic and finite-differencing rules
RewriteRule* getFiniteDifferencingRules();

#endif // PATTERNREWRITE_H
