
#include "broadway.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

using namespace std;

// ------------------------------------------------------------
//  Expr base class
// ------------------------------------------------------------

#ifdef __PROCLOCATION

void exprAnn::compute_next(procLocation * where, ruleAnn * rule)
{
  cout << "INTERNAL ERROR at " << * where << ": Invalid expression for update";
}

void exprAnn::apply_next(procLocation * where,
                         propertyAnalyzer * property_analyzer,
                         ruleAnn * rule,
                         memoryblock_set & changes)
{
  cout << "INTERNAL ERROR at " << * where << ": Invalid expression for update";
}

#endif

// ------------------------------------------------------------
//  Boolean connective
// ------------------------------------------------------------

connectiveExprAnn::connectiveExprAnn(Broadway::Operator op,
                                     exprAnn * lhs,
                                     exprAnn * rhs,
                                     int line)
  : exprAnn(op, line),
    _lhs(lhs),
    _rhs(rhs)
{}

#ifdef __PROCLOCATION

bool connectiveExprAnn::test(procLocation * where,
                             propertyAnalyzer * property_analyzer)
{
  bool result = false;

  switch (op()) {
  case Broadway::And:
    result = (lhs()->test(where, property_analyzer) && rhs()->test(where, property_analyzer));
    break;
  case Broadway::Or:
    result = (lhs()->test(where, property_analyzer) || rhs()->test(where, property_analyzer));
    break;
  case Broadway::Not:
    result = ! lhs()->test(where, property_analyzer);
    break;
  default:
    cout << "Invalid operator for boolean connective";
  }

  return result;
}


// begin TB new
void connectiveExprAnn::diagnostic(ostream & out,
                                   procLocation *where,
                                   propertyAnalyzer * property_analyzer) const {
  switch (op()) {
  case Broadway::And: // both lhs/rhs are true; perform diagnostic on both side.
    lhs()->diagnostic(out, where, property_analyzer);
    rhs()->diagnostic(out, where, property_analyzer);
    break;
  case Broadway::Or: // only perform diagnostic on the true branch(es)
    if(lhs()->test(where, property_analyzer))
      lhs()->diagnostic(out, where, property_analyzer);
    if(rhs()->test(where, property_analyzer))
      rhs()->diagnostic(out, where, property_analyzer);
    break;
  case Broadway::Not:
    lhs()->diagnostic(out, where, property_analyzer);
  }
}
// end TB new

#endif


void connectiveExprAnn::lookup(propertyAnn * default_property,
                               procedureAnn * procedure,
                               Annotations * annotations)
{
  if (_lhs)
    _lhs->lookup(default_property,
                 procedure,
                 annotations);

  if (_rhs)
    _rhs->lookup(default_property,
                 procedure,
                 annotations);
}

#ifdef __PROCLOCATION

void connectiveExprAnn::report(ostream & out, bool is_error, procLocation * where)
{
  out << "(No report for logic expressions)";
}

#endif

void connectiveExprAnn::print(ostream & out)
{
  if (op() == Broadway::Not)
    out << " ! ";

  if (_lhs) {
    out << '(';
    _lhs->print(out);
    out << ')';
  }

  if (op() == Broadway::And)
    out << " && ";

  if (op() == Broadway::Or)
    out << " || ";

  if (_rhs) {
    out << '(';
    _rhs->print(out);
    out << ')';
  }
}

// ------------------------------------------------------------
//  Property test
// ------------------------------------------------------------

enumPropertyExprAnn::enumPropertyExprAnn(Broadway::FlowSensitivity flow_sensitivity,
                                         const parserID * property,
                                         const parserID * lhs,
                                         Broadway::Operator op,
                                         const parserID * rhs,
                                         int line)
  : exprAnn(op, line),
    _flow_sensitivity(flow_sensitivity),
    _property_name(),
    _property(0),
    _lhs_name(lhs->name()),
    _lhs(0),
    _lhs_value(),
    _rhs_name(),
    _rhs_is_property_value(false),
    _rhs(0),
    _rhs_value(),
    _rhs_lost_information(false),
#ifdef __COMPLICIT
    _complicit_property_blocks(),
#endif
    _rhs_ever_values(),
    _is_weak(false)
{
  if (property)
    _property_name = property->name();

  if (rhs)
    _rhs_name = rhs->name();
}

#ifdef __PROCLOCATION

bool enumPropertyExprAnn::test(procLocation * where,
                               propertyAnalyzer * property_analyzer)
{
  bool result = false;

  if (Annotations::Verbose_properties) {
    cout << "    - ";
    print(cout);
    cout << endl;
  }

  // -- Get the current bindings for the lhs

  pointerValue & left = _lhs->get_binding(where);

  if (_rhs_is_property_value) {

    // -- CASE 1: Compare the left-hand variable to a specific flow value
    // (given in _rhs_value).

    result = _property->test(op(), _flow_sensitivity, where, left, _lhs->name(), _rhs_value);
  }
  else {

    // -- CASE 2: Compare the flow values for two variables

    pointerValue & right = _rhs->get_binding(where);

    result = _property->test(op(), _flow_sensitivity, where, left, _lhs->name(),
                             right, _rhs->name());
  }

  return result;
}


// begin TB new
void enumPropertyExprAnn::diagnostic(ostream & out,
                                     procLocation * where,
                                     propertyAnalyzer * property_analyzer) const {
  if (_rhs_is_property_value)
    _property->diagnostic(out, where, _lhs->get_binding(where), _rhs_value);
}
// end TB new


void enumPropertyExprAnn::compute_next(procLocation * where, ruleAnn * rule)
{
  // -- Clear the lost-information flag

  _rhs_lost_information = false;
  _complicit_property_blocks.clear();

  // -- We only need to figure what state is next if the right-hand side is
  // a variable. Otherwise, we already know what the next state is.

  if ( ! _rhs_is_property_value) {

    // -- Get the right-hand-side binding

    pointerValue & right = _rhs->get_binding(where);

    // -- Design decision: when the rhs of an update is based on the value
    // of some other variable, we use the flow-sensitive value:

    _rhs_value = _property->compute_next(where, rule, this,
                                         right, _rhs->name(),
                                         _rhs_lost_information,
                                         _complicit_property_blocks,
                                         _rhs_ever_values);
  }

  if (Annotations::Verbose_properties)
    cout << "  + Update " << _lhs_name << " <- " << _property->to_string(_rhs_value) << endl;
}

void enumPropertyExprAnn::apply_next(procLocation * where,
                                     propertyAnalyzer * property_analyzer,
                                     ruleAnn * rule,
                                     memoryblock_set & changes)
{
  // -- Get the left-hand-side binding

  pointerValue & left = _lhs->get_binding(where);

  bool rhs_changed = Annotations::Adapt_uses_uba &&
                    (_previous_rhs_value[where] != _rhs_value);

  // -- Call the apply_next method: this depends on whether there was
  // actually a right-hand-side or not.

  if ( ! _rhs_is_property_value) {

    // -- Get the right-hand-side binding

    pointerValue & right = _rhs->get_binding(where);

    // -- Apply the right-hand side value

    _property->apply_next(where, (stmtLocation *)0,
                          rule, this,
                          left, _lhs->name(),
                          right, _rhs_value,
                          _rhs_lost_information, rhs_changed,
                          _complicit_property_blocks,
                          _rhs_ever_values, changes);
  }
  else {

    // -- Right-hand side is not a variable

    pointerValue right;

    // -- Apply the given property value

    _property->apply_next(where, (stmtLocation *)0,
                          rule, this,
                          left, _lhs->name(),
                          right, _rhs_value,
                          false, rhs_changed, _complicit_property_blocks,
                          _rhs_ever_values, changes);
  }

  _previous_rhs_value[where] = _rhs_value;
}

#endif

void enumPropertyExprAnn::lookup(propertyAnn * default_property,
                                 procedureAnn * procedure,
                                 Annotations * annotations)
{
  // --- Look up the property definition. Two cases: when no specific
  // property is provided, we assume the property from the enclosing
  // analyze annotation, otherwise, we look up the property definition.

  if (_property_name.empty())
    _property = (enumPropertyAnn *) default_property;
  else
    _property = annotations->lookup_enum_property(_property_name);

  if (_property == 0) {
    annotations->Error(line(), string("Unknown property \"") + _property_name + "\"");
    return;
  }

  // --- Make sure we understand that forward analysis is run first. Issue
  // an error for forward analyses that test backward values.

  if (default_property &&
      (default_property->direction() == Forward) &&
      (_property->direction() == Backward))
    annotations->Error(line(), "NOTE: Results for backward analysis \"" + _property_name +
                       "\" will not be available yet.");

  // --- Lookup the left-hand variable

  _lhs = procedure->lookup(_lhs_name, false);
  if ( ! _lhs )
    annotations->Error(line(), string("Undeclared identifier \"") + _lhs_name + "\"");

  // --- Check the right-hand side, if there is one

  if ( ! _rhs_name.empty()) {

    // --- The right-hand side can be either another variable, or it can be a
    // specific property value. First try it as a variable, then try is as a
    // property value.

    _rhs = procedure->lookup(_rhs_name, false);
    if (_rhs )
      _rhs_is_property_value = false;
    else {

      // -- No variable, try a property value

      enumValueAnn * val = 0;

      if (_property)
        val = _property->lookup(_rhs_name);

      if (! val )
        annotations->Error(line(), string("Unknown value or variable\"") + _rhs_name + "\"");
      else
        _rhs_value = enumvalue_set(val);

      _rhs_is_property_value = true;
    }
  }

  // --- Generate uses and defs

  //     Left hand side can be either a use or def, depending on the
  //     operator

  // -- Forward or backward

  if (_property->direction() == Forward) {

    // -- Forward analysis: the intuitive mode: when we read the value of a
    // property, that generates a use; when we modify the value of a
    // property, that generates a def.

    if (_lhs) {
      if (op() == Broadway::Assign) {

        // -- Record that this property is modified on this variable

        procedure->add_property_def(_lhs, _property);

        // -- Check for weak def

        if (is_weak())
          procedure->add_property_weak_def(_lhs, _property);
      }
      else
        procedure->add_property_use(_lhs, _property);
    }

    //     Right hand side is always a use

    if (_rhs)
      procedure->add_property_use(_rhs, _property);
  }
  else {

    // -- Backwards analysis: non-intuitive mode: going backwards, we test
    // the property value associated with a def, and update the value
    // associate with a use:

    if (_lhs) {
      if (op() == Broadway::Assign)
        procedure->add_property_use(_lhs, _property);
      else
        procedure->add_property_def(_lhs, _property);
    }

    //     Right hand side is always a def

    if (_rhs)
      procedure->add_property_def(_rhs, _property);
  }

  // --- Semantic check: flow-sensitivity of "After" is not allowed inside
  // the analyze annotations. We will assume that we an inside an analyze
  // annotation if there is a default property.

  if ((_flow_sensitivity == Broadway::After) && (default_property != 0))
    annotations->Error(line(), string("Flow sensitivity operator \"@after\" only allowed in report annotations"));

  // --- Semantic check: We must have a right-hand side for binary
  // operators: this is done by the parser

  /*
  if (((op() == Broadway::Is_Exactly) ||
       (op() == Broadway::Is_AtLeast) ||
       (op() == Broadway::Is_AtMost)) &&
      ( ! _rhs && ! _rhs_value))
    annotations->Error(line(), string("Binary test operator missing right-hand side."));
  */

  // --- Semantic check: we will require that for "Assign" operations, if a
  // property is specified, it must match the default_property, which is
  // passed down from the enclosing analyze annoation.

  if ((op() == Broadway::Assign) && (_property) &&
      (_property != default_property))
    annotations->Error(line(), string("Actions in this context may only modify property \"")
                       + _property->name() + "\"");

  // --- Semantic check: we are not allowed to change properties inside the
  // constants analysis:

  if ((op() == Broadway::Assign) && (_property) &&
      (_property == annotations->constants_property()))
    annotations->Error(line(), string("Actions in the constant analysis may not change property \"")
                       + _property->name() + "\"");
}

#ifdef __PROCLOCATION

void enumPropertyExprAnn::report(ostream & out, bool is_error, procLocation * where)
{
  // -- Get the current bindings for the variable to query

  pointerValue & var_binding = _lhs->get_binding(where);

  // -- Invoke the query on the propery itself

  _property->report(out, is_error, where, _flow_sensitivity, var_binding, _lhs->name());
}

#endif

void enumPropertyExprAnn::print(ostream & out)
{
  out << _property->name() << " : " << _lhs_name;

  switch (_flow_sensitivity) {
  case Broadway::None:
    break;
  case Broadway::Before:
    out << " @before";
    break;
  case Broadway::After:
    out << " @after";
    break;
  case Broadway::Always:
    out << " @always";
    break;
  case Broadway::Ever:
    out << " @ever";
    break;
  default:
    out << " (unknown)";
    break;
  }

  switch (op()) {
  case Broadway::Is_Exactly:
    out << " is-exactly";
    break;
  case Broadway::Is_AtLeast:
    out << " is-atleast";
    break;
  case Broadway::Is_AtMost:
    out << " is-atmost";
    break;
  case Broadway::Is_Bottom:
    out << " is-??";
    break;
  case Broadway::Report:
    out << " (report)";
    break;
  case Broadway::Assign:
    out << " <-";
    break;
  default:
    out << " (unknown)";
    break;
  }

  if ((op() != Broadway::Is_Bottom) &&
      (op() != Broadway::Report))
    out << " " << _rhs_name;
}

// ------------------------------------------------------------
//  Set property expression
// ------------------------------------------------------------

setPropertyExprAnn::setPropertyExprAnn(Broadway::FlowSensitivity flow_sensitivity,
                                       const parserID * setProperty,
                                       const parserID * lhs,
                                       Broadway::Operator op,
                                       const parserID * rhs,
                                       int line)
  : exprAnn(op, line),
    _flow_sensitivity(flow_sensitivity),
    _property_name(),
    _property(0),
    _lhs_name(),
    _lhs(0),
    _rhs_name(),
    _rhs(0)
{
  if (setProperty)
    _property_name = setProperty->name();

  if (lhs)
    _lhs_name = lhs->name();

  if (rhs)
    _rhs_name = rhs->name();
}

#ifdef __PROCLOCATION

bool setPropertyExprAnn::test(procLocation * where,
                              propertyAnalyzer * property_analyzer)
{
  bool result = false;

  if (Annotations::Verbose_properties) {
    cout << "    - ";
    print(cout);
    cout << endl;
  }

  // -- Different kinds of tests:

  switch (op()) {
  case Broadway::Is_Element_Of:
    {
      // -- Element-of is a unary operator

      // -- Get the current bindings for the lhs

      pointerValue & left = _lhs->get_binding(where);

      // -- Test the element-of operator:

      result = _property->is_element_of(left);
    }
    break;

  case Broadway::Is_EmptySet:
    {
      // -- Is-empty is a zero-ary operato

      result = _property->is_emptyset();
    }
    break;

  case Broadway::Is_Equivalent:
    {
      // -- Is-equivalent is a binary operator

      // -- Get the current bindings for the lhs

      pointerValue & left = _lhs->get_binding(where);

      // -- Get the current bindings for the rhs

      pointerValue & right = _rhs->get_binding(where);

      // -- Test for equivalence

      result = _property->is_equivalent(left, right);
    }
    break;

  default:

    cout << "Bad operator for set property \"" + _property_name + "\"" << endl;
    return false;
  }

  return result;
}


// begin TB new
void setPropertyExprAnn::diagnostic(ostream & out,
                                    procLocation *where,
                                    propertyAnalyzer * property_analyzer) const {
  assert(false); // TBD
}
// end TB new


void setPropertyExprAnn::compute_next(procLocation * where, ruleAnn * rule)
{
  // -- Perform the updates immediately. The property will record whether
  // anything changes and then report it later.

  if (op() == Broadway::Add_Elements) {

    // -- Get only the left-hand side binding

    pointerValue & left = _lhs->get_binding(where);

    // -- Add all the objects to the set

    _property->add_elements(left);
  }

  // -- Two-arg update is assumed to be an addition to the equivalence relation

  if (op() == Broadway::Add_Equivalences) {

    // -- Get both bindings

    pointerValue & right = _rhs->get_binding(where);
    pointerValue & left = _lhs->get_binding(where);

    // -- Record them as being equivalent

    _property->add_equivalence(left, right);
  }
}

void setPropertyExprAnn::apply_next(procLocation * where,
                                    propertyAnalyzer * property_analyzer,
                                    ruleAnn * rule,
                                    memoryblock_set & changes)
{
  // -- Pick up the set property object if changes occured

  _property->apply_next(changes);
}

#endif

void setPropertyExprAnn::lookup(propertyAnn * default_property,
                                procedureAnn * procedure,
                                Annotations * annotations)
{
  // --- Look up the property definition. Two cases: when no specific
  // property is provided, we assume the property from the enclosing
  // analyze annotation, otherwise, we look up the property definition.

  if (_property_name.empty())
    _property = (setPropertyAnn *) default_property;
  else
    _property = annotations->lookup_set_property(_property_name);

  if (_property == 0) {
    annotations->Error(line(), string("Unknown property \"") + _property_name + "\"");
    return;
  }
  // -- Lookup the setProperty


  if (_property == 0) {
    annotations->Error(line(), string("Unknown set property \"") + _property_name + "\"");
    return;
  }

  // --- Make sure we understand that forward analysis is run first. Issue
  // an error for forward analyses that test backward values.

  if (default_property &&
      (default_property->direction() == Forward) &&
      (_property->direction() == Backward))
    annotations->Error(line(), "NOTE: Results for backward analysis \"" + _property_name +
                       "\" will not be available yet.");

  // --- Lookup the left-hand variable, if there is one

  if ( ! _lhs_name.empty()) {

    _lhs = procedure->lookup(_lhs_name, false);
    if ( ! _lhs )
      annotations->Error(line(), string("Undeclared identifier \"") + _lhs_name + "\"");
  }

  // --- Check the right-hand side, if there is one

  if ( ! _rhs_name.empty()) {

    _rhs = procedure->lookup(_rhs_name, false);
    if ( ! _rhs )
      annotations->Error(line(), string("Undeclared identifier \"") + _rhs_name + "\"");
  }

  // --- Generate uses and defs

  // This is a lot simpler here because the only thing used or defd is the
  // special property variable.

  annVariable * property_var = _property->property_variable();

  // -- Forward or backward

  if (_property->direction() == Forward) {

    // -- Forward analysis: the intuitive mode: when we read the value of a
    // property, that generates a use; when we modify the value of a
    // property, that generates a def.

    if ((op() == Broadway::Add_Elements) ||
        (op() == Broadway::Add_Equivalences))
      procedure->add_one_def(property_var);

    // -- For the set operations (such as adding an element), we need the
    // reaching value in order to the make the change. Therefore all
    // "Assigns" also generate uses.

    procedure->add_one_use(property_var);
  }
  else {

    // -- Backwards analysis: non-intuitive mode: going backwards, we test
    // the property value associated with a def, and update the value
    // associate with a use:

    if ((op() == Broadway::Add_Elements) ||
        (op() == Broadway::Add_Equivalences))
      procedure->add_one_use(property_var);

    procedure->add_one_def(property_var);
  }

  // Semantic checks: we need some semantic checks here...

}

#ifdef __PROCLOCATION

void setPropertyExprAnn::report(ostream & out, bool is_error, procLocation * where)
{
  _property->report(out);
}

#endif

void setPropertyExprAnn::print(ostream & out)
{
  out << _property->name() << " : " << _lhs_name;

  switch (_flow_sensitivity) {
  case Broadway::None:
    break;
  case Broadway::Before:
    out << " @before";
    break;
  case Broadway::After:
    out << " @after";
    break;
  case Broadway::Always:
    out << " @always";
    break;
  case Broadway::Ever:
    out << " @ever";
    break;
  default:
    out << " (unknown)";
    break;
  }

  switch (op()) {
  default:
    out << " (unknown)";
    break;
  }
}



// ------------------------------------------------------------
//  Numeric test
// ------------------------------------------------------------

#ifdef __EXPRNODE

numericExprAnn::numericExprAnn(exprNode * expr,
                               int line)
  : exprAnn(Broadway::Evaluate, line),
    _lhs_name(),
    _lhs(0),
    _expr(expr)
{}

#endif

numericExprAnn::numericExprAnn(const parserID * lhs,
                               int line)
  : exprAnn(Broadway::Is_Constant, line),
    _lhs_name(),
    _lhs(0)
#ifdef __EXPR
  ,
    _expr(0)
#endif
{
  if (lhs)
    _lhs_name = lhs->name();
}


#ifdef __EXPRNODE
numericExprAnn::numericExprAnn(const parserID * lhs,
                               exprNode * expr,
                               int line)
  : exprAnn(Broadway::Assign, line),
    _lhs_name(),
    _lhs(0),
    _expr(expr)
{
  if (lhs)
    _lhs_name = lhs->name();
}

#endif

#ifdef __PROCLOCATION

bool numericExprAnn::test(procLocation * where,
                          propertyAnalyzer * property_analyzer)
{
  if (op() == Broadway::Evaluate) {

    // Regular evaluation: compute the value of the expression and
    // interpret it as a boolean

    _expr->eval();
    bool result = _expr->value().Boolean();

    return result;
  }
  else
    if (op() == Broadway::Is_Constant) {

      // Special constant test: return true if the left-hand side is some
      // constant (not bottom or top).

      const constant * val = property_analyzer->lookup_constant(_lhs, where);

      if (val && ( ! val->no_val()))
        return true;

    }
    else
      cout << "Invalid operator in numeric test" << endl;

  return false;
}

void numericExprAnn::compute_next(procLocation * where, ruleAnn * rule)
{
  if (op() == Broadway::Assign) {

    _expr->eval();

    if (Annotations::Verbose_properties)
      cout << "    + Update " << _lhs_name << " = " << _expr->value().to_string() << endl;
  }
  else
    cout << "Invalid operator in numeric state update" << endl;
}

void numericExprAnn::apply_next(procLocation * where,
                                propertyAnalyzer * property_analyzer,
                                ruleAnn * rule,
                                memoryblock_set & changes)
{
  // -- Get the left-hand side bindings

  pointerValue & left = _lhs->get_binding(where);

  // -- Tell the property analyzer about the new value

  property_analyzer->update_constant(left, _expr->value(), changes);
}

#endif

// Special idNode walker to look up idNodeAnn variables. We assume, from
// the parser that all identifiers in these expressions are actually
// of type idNodeAnn.

#ifdef __WALKER

class idNodeAnnWalker : public Walker
{
public:

  procedureAnn * _procedure;
  Annotations * _annotations;

  idNodeAnnWalker(procedureAnn * procedure,
                  Annotations * annotations)
    : Walker(Preorder, Subtree),
      _procedure(procedure),
      _annotations(annotations)
  {}

  virtual void at_id(idNode * the_id, Order ord) {

    // --- Look up a right-hand side variable

    idNodeAnn * ida = (idNodeAnn *) the_id;
    ida->lookup(_procedure, _annotations);

    if ( ! ida->variable())
      _annotations->Error(ida->line(),
                          string("Undeclared identifier \"") + ida->name() + "\"");

    // --- Record the variable as a use

    if (ida->variable())
      _procedure->add_one_use(ida->variable());
  }
};


#endif

void numericExprAnn::lookup(propertyAnn * default_property,
                            procedureAnn * procedure,
                            Annotations * annotations)
{
  if ( ! _lhs_name.empty()) {

    // --- Lookup the left-hand variable

    _lhs = procedure->lookup(_lhs_name, false);
    if ( ! _lhs )
      annotations->Error(line(),
                         string("Undeclared identifier \"") + _lhs_name + "\"");
  }

  // --- Left hand side is a def

  if (_lhs)
    procedure->add_one_def(_lhs);

  // --- Walk the right-hand side expression looking for identifiers

#ifdef __EXPR
  if (_expr) {
    idNodeAnnWalker idaw(procedure, annotations);
    _expr->walk(idaw);
  }
#endif

  // --- Semantic check: Assign operators can only update constant values
  // inside the special constant analysis annotation.

  if ((op() == Broadway::Assign) && (default_property != annotations->constants_property()))
    annotations->Error(line(), string("Actions may only update constant values in the \"constants\" analyze annotation."));
}

#ifdef __PROCLOCATION

void numericExprAnn::report(ostream & out, bool is_error, procLocation * where)
{
  // -- Evaluate the expression

  _expr->eval();

  // -- Print the result

  if (_expr->value().no_val())
    out << "??";
  else
    out << _expr->value().to_string();
}


#endif

void numericExprAnn::print(ostream & out)
{

#ifdef __FOO
  output_context ct(out);

  if (op() == Broadway::Evaluate) {
    _expr->output(ct, (Node *)0);
  }

  if (op() == Broadway::Is_Constant) {
    out << _lhs_name << " is-#";
  }

  if (op() == Broadway::Assign) {
    out << _lhs_name << " <- ";
    _expr->output(ct, (Node *)0);
  }
#endif
}

// ------------------------------------------------------------
//  Pointer test
// ------------------------------------------------------------

pointerExprAnn::pointerExprAnn(Broadway::Operator op,
                               const parserID * lhs,
                               const parserID * rhs)
  : exprAnn(op, lhs->line()),
    _lhs_name(),
    _lhs(0),
    _rhs_name(),
    _rhs(0)
{
  if (lhs)
    _lhs_name = lhs->name();

  if (rhs)
    _rhs_name = rhs->name();
}

#ifdef __PROCLOCATION

bool pointerExprAnn::test(procLocation * where,
                          propertyAnalyzer * property_analyzer)
{
  bool result = false;

  pointerValue & left = _lhs->get_binding(where);

  if (op() == Broadway::Is_AliasOf) {

    // -- Check aliasing: if there is any overlap at all between the two
    // bindings.

    pointerValue & right = _rhs->get_binding(where);

    // -- Look for a non-empty set intersection. Since the sets are
    // ordered, we can do this quickly.

    memoryblock_set_p cur_left = left.blocks.begin();
    memoryblock_set_p cur_right = right.blocks.begin();

    result = true;

    while ((cur_left != left.blocks.end()) &&
           (cur_right != right.blocks.end())) {
      if (*cur_left < *cur_right)
        cur_left++;
      else
        if (*cur_right < *cur_left)
          cur_right++;
        else {
          result = false;
          break;
        }
    }

    if (Annotations::Verbose_properties)
      if (result)
        cout << "      pointerExprAnn : " << _lhs->name() << " aliases " << _rhs->name() << endl;
      else
        cout << "      pointerExprAnn : " << _lhs->name() << " does not alias " << _rhs->name() << endl;
  }

  if (op() == Broadway::Is_SameAs) {

    // -- Check to see if the two bindings are exactly the same

    pointerValue & right = _rhs->get_binding(where);

    if (left.blocks.size() == right.blocks.size()) {

      // -- Visit the elements one by one. Since they are sorted, the same
      // elements must occur in the same positions.

      memoryblock_set_p cur_left = left.blocks.begin();
      memoryblock_set_p cur_right = right.blocks.begin();

      result = true;

      while ((cur_left != left.blocks.end()) &&
             (cur_right != right.blocks.end())) {
        if (*cur_left != *cur_right) {
          result = false;
          break;
        }
        cur_left++;
        cur_right++;
      }
    }

    if (Annotations::Verbose_properties)
      if (result)
        cout << "      pointerExprAnn : " << _lhs->name() << " is the same as " << _rhs->name() << endl;
      else
        cout << "      pointerExprAnn : " << _lhs->name() << " is different from " << _rhs->name() << endl;
  }

  if (op() == Broadway::Is_Empty) {

    // -- Check for no binding

    if (left.blocks.empty())
      result = true;

    if (Annotations::Verbose_properties)
      if (result)
        cout << "      pointerExprAnn : " << _lhs->name() << " is empty." << endl;
      else
        cout << "      pointerExprAnn : " << _lhs->name() << " is not empty." << endl;
  }

  return result;
}

#endif

void pointerExprAnn::lookup(propertyAnn * default_property,
                            procedureAnn * procedure,
                            Annotations * annotations)
{
  // --- Lookup the left-hand variable

  _lhs = procedure->lookup(_lhs_name, false);
  if ( ! _lhs )
    annotations->Error(line(), string("Undeclared identifier \"") + _lhs_name + "\"");

  // -- For the binary operators...

  if ((op() == Broadway::Is_AliasOf) || (op() == Broadway::Is_SameAs)) {

    // --- Lookup the right-hand variable

    _rhs = procedure->lookup(_rhs_name, false);
    if ( ! _rhs )
      annotations->Error(line(), string("Undeclared identifier \"") + _rhs_name + "\"");
  }
}

#ifdef __PROCLOCATION

void pointerExprAnn::report(ostream & out, bool is_error, procLocation * where)
{
  out << "(No report for pointer expressions)";
}

#endif

void pointerExprAnn::print(ostream & out)
{
  out << _lhs_name;

  if (op() == Broadway::Is_AliasOf)
    out << " is-aliasof " << _rhs_name;

  if (op() == Broadway::Is_SameAs)
    out << " is-sameas " << _rhs_name;

  if (op() == Broadway::Is_Empty)
    out << " is-empty";
}

// ------------------------------------------------------------
//  Special idNode
// ------------------------------------------------------------

#ifdef __IDNODE_ANN

idNodeAnn::idNodeAnn(const parserID * id)
  : idNode(id->name().c_str()),
    _variable(0),
    _line(id->line())
{}

void idNodeAnn::lookup(procedureAnn * procedure,
                       Annotations * annotations)
{
  _variable = procedure->lookup(name(), false);

  if (_variable)
    procedure->numeric_identifiers().push_back(this);
}

void idNodeAnn::output_expr(output_context & ct, Node * parent, int prec, Assoc assoc)
{
  ct << '(' << name() << " = " << value().to_string() << ')';
}

#endif /* __IDNODE_ANN */
