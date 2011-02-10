
#ifndef BDWY_CPATTERN_H
#define BDWY_CPATTERN_H

class exprPatternNode;
class stmtPatternNode;
class typePatternNode;

// ----------------------------------------------------------------------
//  Expression pattern node
// ----------------------------------------------------------------------

class exprPatternNode : public exprNode
{
private:

  /** @brief The name of the pattern variable */

  string _name;

  /** @brief Reference to the annotation variable
   *
   * This is only needed for pattern expansion. */

  REF annVariable * _variable;

public:

  /** @brief Create a new pattern node */

  exprPatternNode(const parserID * var);

  /** @name Accessors
   *
   *  Methods to get and set fields in the class. */
  //@{

  inline string & name() { return _name; }
  inline const string & name() const { return _name; }
  inline void name(string nm) { _name = nm; }

  inline annVariable * variable() const { return _variable; }
  inline void variable(annVariable *variable) { _variable = variable; }

  //@}

  /** @brief Lookup annotation variable */

  void lookup(procedureAnn * procedure,
              Annotations * annotations);

  /** @brief Constant folding (No-op) */

  virtual void eval() {}

  /** @name AST Traversal
   *
   *  Methods to uniformly traverse the AST. See the documentation in the
   *  Node class. */

  //@{

  virtual void visit(Visitor * the_visitor);
  virtual void walk(Walker & the_walker);
  virtual Node * change(Changer & the_changer, bool redispatch = false);

  //@}

  // -- Dataflow (No-op)

  virtual void dataflow(FlowVal * v, FlowProblem & fp) {}

  // -- Clone

  virtual Node * clone() const { return new exprPatternNode ( *this ); }

  // -- Output

  virtual void output_expr(output_context & ct, Node * par, int prec, Assoc assoc);
};

// ----------------------------------------------------------------------
//  Statement pattern node
// ----------------------------------------------------------------------

class stmtPatternNode : public stmtNode
{
private:

  /** @brief The name of the pattern variable */

  string _name;

  /** @brief Reference to the annotation variable
   *
   * This is only needed for pattern expansion. */

  REF annVariable * _variable;

public:

  /** @brief Create a new pattern node */

  stmtPatternNode(const parserID * var);

  /** @name Accessors
   *
   *  Methods to get and set fields in the class. */
  //@{

  inline string & name() { return _name; }
  inline const string & name() const { return _name; }
  inline void name(string nm) { _name = nm; }

  inline annVariable * variable() const { return _variable; }
  inline void variable(annVariable *variable) { _variable = variable; }

  //@}

  /** @brief Lookup annotation variable */

  void lookup(procedureAnn * procedure,
              Annotations * annotations);

  /** @name AST Traversal
   *
   *  Methods to uniformly traverse the AST. See the documentation in the Node class. */
  //@{

  virtual void visit(Visitor * the_visitor);
  virtual void walk(Walker & the_walker);
  virtual Node * change(Changer & the_changer, bool redispatch = false);

  //@}

  // -- Dataflow (No-op)

  virtual void dataflow(FlowVal * v, FlowProblem & fp) {}

  // -- Clone

  virtual Node * clone() const { return new stmtPatternNode ( *this ); }

  // -- Output 

  virtual void output_stmt(output_context & ct, Node * par);
};

// ----------------------------------------------------------------------
//  Type pattern node
// ----------------------------------------------------------------------

class typePatternNode : public typeNode
{
private:

  /** @brief The name of the pattern variable */

  string _name;

  /** @brief Reference to the annotation variable
   *
   * This is only needed for pattern expansion. */

  REF annVariable * _variable;

public:

  /** @brief Create a new pattern node */

  typePatternNode(const parserID * var);

  /** @name Accessors
   *
   *  Methods to get and set fields in the class. */
  //@{

  inline string & name() { return _name; }
  inline const string & name() const { return _name; }
  inline void name(string nm) { _name = nm; }

  inline annVariable * variable() const { return _variable; }
  inline void variable(annVariable *variable) { _variable = variable; }

  //@}

  /** @brief Lookup annotation variable */

  void lookup(procedureAnn * procedure,
              Annotations * annotations);

  /** @brief Type equality (No-op) */

  bool qualified_equal_to(typeNode * node2,
                          bool strict_toplevel, bool strict_recursive)
  { return false; }

  /** @name AST Traversal
   *
   *  Methods to uniformly traverse the AST. See the documentation in the Node class. */
  //@{

  virtual void visit(Visitor * the_visitor);
  virtual void walk(Walker & the_walker);
  virtual Node * change(Changer & the_changer, bool redispatch = false);

  //@}

  // -- Clone

  virtual Node * clone() const { return new typePatternNode ( *this ); }

  // -- Output

  virtual void output_type(output_context & ct, Node * par, Assoc context, Type_qualifiers q);

};

// ----------------------------------------------------------------------
//  Pattern expander
// ----------------------------------------------------------------------

class callsiteEntry;

class patternExpander : public Changer
{
private:

  /** @brief The callsite
   *
   * This provides the actual arguments to use in the expansion, as well as
   * information about the other variable bindings. */

  callsiteEntry * _callsite;

  /** @brief Property analyzer
   *
   * This is used to obtain constant value information. */

  propertyAnalyzer * _analyzer;

  /** @brief Top level annotations
   *
   * This is really only used for error reporting. */

  Annotations * _annotations;

public:

  patternExpander(callsiteEntry * callsite,
                  propertyAnalyzer * analyzer,
                  Annotations * annotations);

  virtual Node * at_expr(exprNode * the_expr, Order ord);
  virtual Node * at_stmt(stmtNode * the_stmt, Order ord);
  virtual Node * at_type(typeNode * the_type, Order ord);
};

#endif /* BDWY_CPATTERN_H */

