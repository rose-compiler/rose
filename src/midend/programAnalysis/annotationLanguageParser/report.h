
#ifndef BDWY_REPORT_H
#define BDWY_REPORT_H

typedef std::list< reportAnn * > report_list;
typedef report_list::iterator report_list_p;
typedef report_list::const_iterator report_list_cp;

/** @brief Report element base class
 *
 * */

class reportElementAnn : public Ann
{
public:

  reportElementAnn(int line)
    : Ann(line)
  {}

  virtual void lookup(enumPropertyAnn * default_property,
                      procedureAnn * procedure,
                      Annotations * annotations) =0;

#ifdef NONEMPTY
  virtual void report(std::ostream & out,
                      bool is_error,
                      Analyzer * analyzer,
                      procLocation * where) =0;

  // static callNode * callnode(stmtNode * stmt);
#endif
};

/** @brief Literal string report element
 *
 * This type of element just prints out the string it's given. */

class literalReportElementAnn : public reportElementAnn
{
private:

  std::string _literal;

public:

// DQ (8/12/2004): Added virtual destructor to avoid g++ compiler warnings
  virtual ~literalReportElementAnn() {};

  literalReportElementAnn(const parserID * name);

  virtual void lookup(enumPropertyAnn * default_property,
                      procedureAnn * procedure,
                      Annotations * annotations);

#ifdef NONEMPTY
  virtual void report(std::ostream & out,
                      bool is_error,
                      Analyzer * analyzer,
                      procLocation * where);
#endif
};


/** @brief Expression report element
 *
 * This type of element queries the values of variable analysis properties.
 * */

class expressionReportElementAnn : public reportElementAnn
{
private:

  /** @brief Flow sensitivity */

  Broadway::FlowSensitivity _flow_sensitivity;

  /** @brief The expression to evaluate */

  exprAnn * _expr;

public:

  expressionReportElementAnn(Broadway::FlowSensitivity flow_sensitivity,
                             exprAnn * expr, int line);

  virtual void lookup(enumPropertyAnn * default_property,
                      procedureAnn * procedure,
                      Annotations * annotations);

#ifdef NONEMPTY
  virtual void report(std::ostream & out,
                      bool is_error,
                      Analyzer * analyzer,
                      procLocation * where);
#endif
};


/** @brief Location report element
 *
 * This type of element prints out locations in the program -- either
 * context sensitive, or not. */

class locationReportElementAnn : public reportElementAnn
{
public:

  typedef enum { Callsite, Context, Procedure, Num_Contexts } ContextKind;

private:

  std::string _kind_name;

  ContextKind _kind;

public:

// DQ (8/12/2004): Added virtual destructor to avoid g++ compiler warnings
  virtual ~locationReportElementAnn() {};

  locationReportElementAnn(const parserID * id);

  virtual void lookup(enumPropertyAnn * default_property,
                      procedureAnn * procedure,
                      Annotations * annotations);

#ifdef NONEMPTY
  virtual void report(std::ostream & out,
                      bool is_error,
                      Analyzer * analyzer,
                      procLocation * where);

#endif
};
  

/** @brief Binding report element
 *
 * This type of element prints out the bindings of a variable. It makes the
 * most sense for formal parameters, where the binding is one-to-one, but
 * will work for any variable. */

class bindingReportElementAnn : public reportElementAnn
{
private:

  /** @brief The name of the variable */

  std::string _variable_name;

  /** @brief Size only
   *
   * When true, just print the number of objects in the binding. */

  bool _size_only;

  /** @brief A reference to the annvariable object */

  annVariable * _variable;

public:

// DQ (8/12/2004): Added virtual destructor to avoid g++ compiler warnings
  virtual ~bindingReportElementAnn() {};

  bindingReportElementAnn(const parserID * varname, bool size_only);

  virtual void lookup(enumPropertyAnn * default_property,
                      procedureAnn * procedure,
                      Annotations * annotations);

#ifdef NONEMPTY
  virtual void report(std::ostream & out,
                      bool is_error,
                      Analyzer * analyzer,
                      procLocation * where);
#endif
};


typedef std::list< reportElementAnn * > report_element_list;
typedef report_element_list::iterator report_element_list_p;

/** @brief Report annotation
 *
 * */

class reportAnn : public Ann
{
private:

  /** @brief Condition
   *
   *  The condition to test for activating this report. Null for no
   *  condition. */

  exprAnn * _condition;

  /** @brief Report elements
   *
   * Each element of this list is an item to print. */

  report_element_list _elements;

  /** @brief Is error?
   *
   * Mark error reports so that we can count them up. */

  bool _is_error;

public:

// DQ (8/12/2004): Added virtual destructor to avoid g++ compiler warnings
  virtual ~reportAnn() {};

  reportAnn(exprAnn * condition,
            bool is_error,
            report_element_list * elements, int line);

  virtual void lookup(procedureAnn * procedure,
                      Annotations * annotations);

#ifdef NONEMPTY
  virtual void report(std::ostream & out,
                      Analyzer * analyzer,
                      procLocation * where,
                      propertyAnalyzer * property_analyzer);
#endif
};

#endif /* BDWY_REPORT_H */
