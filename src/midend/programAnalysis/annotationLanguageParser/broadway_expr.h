
#ifndef BDWY_EXPR_H
#define BDWY_EXPR_H

typedef std::list< idNodeAnn * > idnodeann_list;
typedef idnodeann_list::iterator idnodeann_list_p;

typedef std::list< exprAnn * > exprann_list;
typedef exprann_list::iterator exprann_list_p;
typedef exprann_list::const_iterator exprann_list_cp;


/** @brief Expression base class
 * */

class exprAnn : public Ann
{
private:

  /** @brief Expression operator
   *
   * One of the Broadway::Operator enum above. */

  Broadway::Operator _op;

public:

  /** @brief Create a new expression
   *
   */

  exprAnn(Broadway::Operator op, int line)
    : Ann(line),
      _op(op)
  {}

  /** @brief Operator */

  inline Broadway::Operator op() const { return _op; }

  /** @brief Test method
   *
   *  Use this to test a conditional expression. The calling context
   *  determines the "now" values of the various properties.  */

#ifdef __PROCLOCATION
  virtual bool test(procLocation * where,
		    propertyAnalyzer * property_analyzer) =0;
#endif
  /** @brief Compute the next state
   *
   * When a test returns true, analysis rules invoke this method for each
   * action. The method determines what the new flowvalue will be once the
   * action is invoked. */

#ifdef __PROCLOCATION
  virtual void compute_next(procLocation * where, ruleAnn * rule);
#endif

  /** @brief Apply the next state
   *
   * After computing the next state, actually apply it to the analysis
   * information. Record any changes. */

#ifdef __PROCLOCATION
  virtual void apply_next(procLocation * where,
			  propertyAnalyzer * property_analyzer,
			  ruleAnn * rule,
			  memoryblock_set & changes);
#endif

  /** @brief Lookup method
   *
   *  Once a procedure annotation is complete and all the variables have
   *  been created, we call this routine to lookup the variables referenced
   *  in the expressions. */

  virtual void lookup(propertyAnn * default_property,
		      procedureAnn * procedure,
		      Annotations * annotations) =0;

  /** @brief Report method
   *
   * Interpret the expression as an informational query (for the report
   * annotations). Write the result out to the given stream. */

#ifdef __PROCLOCATION
  virtual void report(std::ostream & out, bool is_error, procLocation * where) =0;
#endif

  /** @brief Print method
   *
   * Display the expression using the annotation syntax. */

  virtual void print(std::ostream & out) =0;

// begin TB new

  /** @brief Diagnostic trace.
   *
   * Display the diagnostic trace of a reported error. */

#ifdef __PROCLOCATION
  virtual void diagnostic(std::ostream & out,
                          procLocation *where,
                          propertyAnalyzer * property_analyzer) const =0;
#endif
// end TB new
};

/** @brief Boolean connective expression
 *
 */

class connectiveExprAnn : public exprAnn
{
private:

  /** @brief Left-hand sub-expression*/

  exprAnn * _lhs;

  /** @brief Right-hand sub-expression */

  exprAnn * _rhs;

public:

  /** @brief
   *
   */

  connectiveExprAnn(Broadway::Operator op,
		    exprAnn * lhs,
		    exprAnn * rhs,
		    int line);

  inline exprAnn * lhs() const { return _lhs; }
  inline exprAnn * rhs() const { return _rhs; }

#ifdef __PROCLOCATION
  virtual bool test(procLocation * where,
		    propertyAnalyzer * property_analyzer);
#endif
  virtual void lookup(propertyAnn * default_property,
		      procedureAnn * procedure,
		      Annotations * annotations);

#ifdef __PROCLOCATION
  virtual void report(std::ostream & out, bool is_error, procLocation * where);
#endif

  virtual void print(std::ostream & out);

// begin TB new
#ifdef __PROCLOCATION
  virtual void diagnostic(std::ostream & out,
                          procLocation *where,
                          propertyAnalyzer * property_analyzer) const;
#endif
// end TB new
};

/** @brief Property test expression
 *
 */

class enumPropertyExprAnn : public exprAnn
{
private:

  /** @brief Flow sensitivity */

  Broadway::FlowSensitivity _flow_sensitivity;

  /** @brief Property name
   *
   * The name of the property to test */

  std::string _property_name;

  /** @brief Property pointer 
   *
   * A pointer to the property definition */

  enumPropertyAnn * _property;

  /** @brief Left-hand identifier
   *
   * The identifier on the left-hand side */

  std::string _lhs_name;

  /** @brief Left-hand variable
   *
   * A pointer to the actual left-hand side variable */

  REF annVariable * _lhs;

  /** @brief Left-hand value
   *
   * A place-holder for the associated left-hand property value */

  enumvalue_set _lhs_value;

  /** @brief Right-hand identifier
   *
   * The identifier on the right-hand side */

  std::string _rhs_name;

  /** @brief Right-hand is property value
   *
   *  True if the right-hand side of the expression is just the name of a
   *  particular property value, as opposed to the name of a variable. */

  bool _rhs_is_property_value;

  /** @brief Right-hand variable
   *
   *  A pointer to the actual right-hand side variable This pointer should
   *  be null if the right hand side is just a property value */

  REF annVariable * _rhs;

  /** @brief Right-hand property
   *
   * A place-holder for the associated right-hand property value. if the
   * right hand side is just a property value, then this will be
   * permanently set to point to that value. */

  enumvalue_set _rhs_value;

  /* @brief Previous _rhs_value at a location. */
#ifdef __PROCLOCATION
  map<procLocation*,enumvalue_set> _previous_rhs_value; // TB_unify
#endif
  /** @brief Right-hand lost information
   *
   * When constructing the right-hand-side value from a variable, record if
   * constructing the now value caused information loss. */

  bool _rhs_lost_information;

  /** @brief Complicit property blocks
   *
   * When running in adaptive mode, this records any objects on the
   * right-hand-side that already have multiple possible values -- these
   * are "complicit" in any loss of accuracy. */

#ifdef __MEMORYBLOCK
  memoryblock_set _complicit_property_blocks;
#endif

  /** @brief Right-hand ever values
   *
   * We pass on the ever values without meeting them together. This field
   * holds those values from the right-hand side. */

  enumvalue_set _rhs_ever_values;

  /** @brief Weak assignment
   *
   * Some annotations need to force weak updates. For example, strcat()
   * just adds the properties of the new string to the old string. */

  bool _is_weak;

public:

  /** @brief
   *
   */

// DQ (8/12/2004): Added virtual destructor to avoid g++ compiler warnings
  virtual ~enumPropertyExprAnn() {};

  enumPropertyExprAnn(Broadway::FlowSensitivity flow_sensitivity,
		      const parserID * property,
		      const parserID * lhs,
		      Broadway::Operator op,
		      const parserID * rhs,
		      int line);

  /** @brief Is RHS a property value?
   *
   * Return true if the right-hand side of the operation is a specific
   * property value. */

  inline bool rhs_is_property_value() const { return _rhs_is_property_value; }

#ifdef __PROCLOCATION
  virtual bool test(procLocation * where,
		    propertyAnalyzer * property_analyzer);
#endif

#ifdef __PROCLOCATION
  virtual void compute_next(procLocation * where, ruleAnn * rule);
#endif

#ifdef __PROCLOCATION
  virtual void apply_next(procLocation * where,
			  propertyAnalyzer * property_analyzer,
			  ruleAnn * rule,
			  memoryblock_set & changes);
#endif
  virtual void lookup(propertyAnn * default_property,
		      procedureAnn * procedure,
		      Annotations * annotations);

#ifdef __PROCLOCATION
  virtual void report(std::ostream & out, bool is_error, procLocation * where);
#endif

  virtual void print(std::ostream & out);

  inline bool is_weak() const { return _is_weak; }
  inline void set_weak() { _is_weak = true; }

// begin TB new
#ifdef __PROCLOCATION
  virtual void diagnostic(std::ostream & out,
                          procLocation *where,
                          propertyAnalyzer * property_analyzer) const;
#endif
// end TB new
};

/** @brief Set property expression
 *
 */

class setPropertyExprAnn : public exprAnn
{
private:

  /** @brief Flow sensitivity */

  Broadway::FlowSensitivity _flow_sensitivity;

  /** @brief Set property name
   *
   * The name of the setProperty to test */

  std::string _property_name;

  /** @brief Set property pointer 
   *
   * A pointer to the setProperty definition */

  setPropertyAnn * _property;

  /** @brief Left-hand identifier
   *
   * The identifier on the left-hand side */

  std::string _lhs_name;

  /** @brief Left-hand variable
   *
   * A pointer to the actual left-hand side variable */

  REF annVariable * _lhs;

  /** @brief Right-hand identifier
   *
   * The identifier on the right-hand side */

  std::string _rhs_name;

  /** @brief Right-hand variable
   *
   *  A pointer to the actual right-hand side variable. */

  REF annVariable * _rhs;

public:

  /** @brief Create a new setProperty expression
   *
   */

// DQ (8/12/2004): Added virtual destructor to avoid g++ compiler warnings
  virtual ~setPropertyExprAnn() {};

  setPropertyExprAnn(Broadway::FlowSensitivity flow_sensitivity,
                     const parserID * setProperty,
		     const parserID * lhs,
		     Broadway::Operator op,
		     const parserID * rhs,
		     int line);

#ifdef __PROCLOCATION
  virtual bool test(procLocation * where,
		    propertyAnalyzer * property_analyzer);
#endif

#ifdef __PROCLOCATION
  virtual void compute_next(procLocation * where, ruleAnn * rule);
#endif

#ifdef __PROCLOCATION
  virtual void apply_next(procLocation * where,
			  propertyAnalyzer * property_analyzer,
			  ruleAnn * rule,
			  memoryblock_set & changes);
#endif

  virtual void lookup(propertyAnn * default_property,
		      procedureAnn * procedure,
		      Annotations * annotations);

#ifdef __PROCLOCATION
  virtual void report(std::ostream & out, bool is_error, procLocation * where);
#endif

  virtual void print(std::ostream & out);

// begin TB new
#ifdef __PROCLOCATION
  virtual void diagnostic(std::ostream & out,
                          procLocation *where,
                          propertyAnalyzer * property_analyzer) const;
#endif
// end TB new
};

/** @brief Numeric expression
 *
 *  There are basically three kinds of numeric expressions, distinguished
 *  by operator:
 *
 *    1. Evaluate. These expressions have no side-effects and are only used
 *    for querying the constant analysis. There is no left-hand side
 *    variable. In the test() method, we cast the result to boolean to
 *    decide whether or not the test is true. In the report() method, we
 *    print out the value.
 *
 *    2. Is_Constant. These expressions are used to test whether a variable
 *    is constant or not, when we don't care about any specific value. We
 *    use the left-hand side variable to hold the variable to test.
 *
 *    3. Assign. These expressions are only used in the constant analysis
 *    to update the constant propagation information. The left-hand side is
 *    the variable to update, and the expression provides the new value.
 */

class numericExprAnn : public exprAnn
{
private:

  /** @brief Left-hand side variable name*/

  std::string _lhs_name;

  /** @brief Left-hand side variable */

  REF annVariable * _lhs;

  /** @brief Right-hand side expression */

#ifdef __EXPRNODE
  exprNode * _expr;
#endif

public:

  /** @brief Create a numeric evaluation
   *
   *  These expressions are only used for test expressions and reports. */

// DQ (8/12/2004): Added virtual destructor to avoid g++ compiler warnings
  virtual ~numericExprAnn() {};

#ifdef __EXPRNODE
  numericExprAnn(exprNode * expr,
                 int line);
#endif

  /** @brief Create a constant test
   *
   *  In the test() method, this kind of expression evaluates to true if
   *  the left-hand side value has a constant value. */

  numericExprAnn(const parserID * lhs,
                 int line);

  /** @brief Create a numeric assignment
   *
   *  Here, the expression is evaluated and assigned to the left-hand side
   *  variable. This kind of object is only used in the built-in
   *  "constants" analysis, to allow updating of the constant propagation
   *  information. */

#ifdef __EXPRNODE
  numericExprAnn(const parserID * lhs,
                 exprNode * expr,
		 int line);
#endif

#ifdef __PROCLOCATION
  virtual bool test(procLocation * where,
		    propertyAnalyzer * property_analyzer);
#endif

#ifdef __PROCLOCATION
  virtual void compute_next(procLocation * where, ruleAnn * rule);

  virtual void apply_next(procLocation * where,
			  propertyAnalyzer * property_analyzer,
			  ruleAnn * rule,
			  memoryblock_set & changes);
#endif

  virtual void lookup(propertyAnn * default_property,
		      procedureAnn * procedure,
		      Annotations * annotations);

#ifdef __PROCLOCATION
  virtual void report(std::ostream & out, bool is_error, procLocation * where);
#endif

  virtual void print(std::ostream & out);

// begin TB new
#ifdef __PROCLOCATION
  virtual void diagnostic(std::ostream & out,
                          procLocation *where,
                          propertyAnalyzer * property_analyzer) const {}
#endif
// end TB new
};


/** @brief Pointer test expression
 *
 */

class pointerExprAnn : public exprAnn
{
private:

  /** @brief  */

  std::string _lhs_name;

  /** @brief  */

  REF annVariable * _lhs;

  /** @brief  */

  std::string _rhs_name;

  /** @brief  */

  REF annVariable * _rhs;

public:

  /** @brief
   *
   */

// DQ (8/12/2004): Added virtual destructor to avoid g++ compiler warnings
  virtual ~pointerExprAnn() {};

  pointerExprAnn(Broadway::Operator op,
                 const parserID * lhs,
		 const parserID * rhs);

#ifdef __PROCLOCATION
  virtual bool test(procLocation * where,
		    propertyAnalyzer * property_analyzer);
#endif
  virtual void lookup(propertyAnn * default_property,
		      procedureAnn * procedure,
		      Annotations * annotations);

#ifdef __PROCLOCATION
  virtual void report(std::ostream & out, bool is_error, procLocation * where);
#endif

  virtual void print(std::ostream & out);

// begin TB new
#ifdef __PROCLOCATION
  virtual void diagnostic(std::ostream & out,
                          procLocation *where,
                          propertyAnalyzer * property_analyzer) const {}
#endif
// end TB new
};

/** @brief Special annotation identifier
 *
 * This replaces the normal idNode in expressions generated by the
 * annotation parser. There are two differences: first, it holds a pointer
 * to the annVariable object that it represents (which behaves like the
 * declaration), and second, it overrides the eval() method so that it
 * looks up the results of constant propagation. */

#ifdef __IDNODEANN
class idNodeAnn : public idNode
{
private:

  REF annVariable * _variable;
  int _line;

public:

  /** @brief
   *
   */

  idNodeAnn(const parserID * id);

  inline annVariable * variable() const { return _variable; }
  inline int line() const { return _line; }

  void lookup(procedureAnn * procedure,
	      Annotations * annotations);

  /** @brief Eval function
   *
   * Override the normal idNode eval function. We compute the value of an
   * idNodeAnn when we start processing the library call. */

  virtual void eval() { }

  /** @brief Output functions
   *
   * Override the normal output function so that it also displays the
   * value. */

  virtual void output_expr(output_context & ct, Node * parent, int prec, Assoc assoc);

};
#endif /* __IDNODE_ANN */

#endif /* BDWY_EXPR_H */
