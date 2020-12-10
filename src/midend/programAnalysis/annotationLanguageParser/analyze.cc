
#include "broadway.h"
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

using namespace std;

analyzeAnn::analyzeAnn(Annotations * anns,
                       const parserID * property,
                       rule_list * rules,
                       int line)
  : Ann(line),
    _property(0),
    _rules(),
    _default(0),
    _applicable_rule(0)
{
  if (rules) {
    _rules.swap(*rules);
    delete rules;
  }

  // -- Look up the property. It can be either an enum property or a set
  // property

  _property = anns->lookup_property(property->name());

  if ( ! _property)
    anns->Error(line, "Unkown property \"" + property->name() + "\"");

  // -- Find the default, and make sure there is only one

  for (rule_list_p p = _rules.begin();
       p != _rules.end();
       ++p)
    {
      ruleAnn * rule = *p;

      // -- If this is a default rule, see if we already have one. If not,
      // set the _default field.

      if ( ! rule->has_condition() ) {

        if (_default)
          anns->Error(line, "Each analyze annotation may only have one default rule.");
        else
          _default = rule;
      }
    }
}

#ifdef __PROCLOCATION

void analyzeAnn::test(procLocation * where,
                      propertyAnalyzer * property_analyzer)
{
  // -- Make sure we only analyze properties that have the same direction
  // as the current analysis.

  if (property()->is_enabled() &&
      (property()->direction() == property_analyzer->direction())) {

    if (Annotations::Verbose_properties)
      cout << "- Test rules for \"" << property()->name() << "\"" << endl;

    for (rule_list_p p = _rules.begin();
         p != _rules.end();
         ++p)
      (*p)->test(where, property_analyzer);
  }
}

void analyzeAnn::compute_next(procLocation * where,
                              propertyAnalyzer * property_analyzer)
{
  // -- Make sure we only analyze properties that have the same direction
  // as the current analysis.

  if (property()->is_enabled() &&
      (property()->direction() == property_analyzer->direction())) {

    // -- First compute next on the first true non-default rule

    bool found_non_default = false;
    for (rule_list_p p = _rules.begin();
         p != _rules.end();
         ++p)
      {
        ruleAnn * rule = *p;

        if ((rule != _default) &&
            rule->is_condition_true()) {

          if (Annotations::Verbose_properties)
            cout << "- Proposed updates to \"" << property()->name() << "\"" << endl;

          rule->compute_next(where);
          found_non_default = true;

          _applicable_rule = rule;

          break;
        }
      }

    // -- If no non-default rule is true, then compute next on the default
    // rule.

    if (_default && ( ! found_non_default )) {
      if (Annotations::Verbose_properties)
        cout << "- Proposed updates to \"" << property()->name() << "\"" << endl;

      _default->compute_next(where);

      _applicable_rule = _default;
    }
  }
}

/** @brief Apply next state
 *
 * Use the states computed by compute_next() to update the
 * variables. Store the changed blocks in the changes set. */

void analyzeAnn::apply_next(procLocation * where,
                            propertyAnalyzer * property_analyzer,
                            memoryblock_set & changes)
{
  // -- If a rule was applicable, call apply_next() on the actions

  if (_applicable_rule)
    _applicable_rule->apply_next(where, property_analyzer, changes);

  _applicable_rule = 0;
}

#endif /* __PROCLOCATION */

void analyzeAnn::lookup(procedureAnn * procedure,
                        Annotations * annotations)
{
  // -- Call look up on each of the rules

  for (rule_list_p p = _rules.begin();
       p != _rules.end();
       ++p)
    (*p)->lookup(_property, procedure, annotations);
}
