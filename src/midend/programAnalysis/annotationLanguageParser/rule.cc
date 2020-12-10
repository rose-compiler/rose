
#include "broadway.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

using namespace std;

ruleAnn::ruleAnn(exprAnn * condition, exprann_list * effects, int line)
  : Ann(line),
    _condition(condition),
    _is_condition_true(false),
    _effects()
{
  if (effects) {
    _effects.swap(*effects);
    delete effects;
  }
}

#ifdef __PROCLOCATION

void ruleAnn::test(procLocation * where,
                   propertyAnalyzer * property_analyzer)
{
  if (Annotations::Verbose_properties) {
    cout << "  + Test: ";
    if (_condition)
      _condition->print(cout);
    else
      cout << "(no condition)";
    cout << endl;
  }

  if (_condition)
    _is_condition_true = _condition->test(where, property_analyzer);
  else
    _is_condition_true = true;

  if (Annotations::Verbose_properties) {
    cout << "   -> evaluated to ";
    if (_is_condition_true)
      cout << "true" << endl;
    else
      cout << "false" << endl;
  }
}

void ruleAnn::compute_next(procLocation * where)
{
  for (exprann_list_p p = _effects.begin();
       p != _effects.end();
       ++p)
    (*p)->compute_next(where, this);
}

/** @brief Apply next state
 *
 * Use the states computed by compute_next() to update the
 * variables. Return true if any of them result in changes, meaning that
 * the analysis problem has not converged. */

void ruleAnn::apply_next(procLocation * where,
                         propertyAnalyzer * property_analyzer,
                         memoryblock_set & changes)
{
  for (exprann_list_p p = _effects.begin();
       p != _effects.end();
       ++p)
    (*p)->apply_next(where, property_analyzer, this, changes);
}

#endif /* __PROCLOCATION */

void ruleAnn::lookup(propertyAnn * default_property,
                     procedureAnn * procedure,
                     Annotations * annotations)
{
  // -- Call lookup on the condition expression

  if (_condition)
    _condition->lookup(default_property, procedure, annotations);

  // -- Call lookup on each of the effects

  for (exprann_list_p p = _effects.begin();
       p != _effects.end();
       ++p)
    (*p)->lookup(default_property, procedure, annotations);
}

