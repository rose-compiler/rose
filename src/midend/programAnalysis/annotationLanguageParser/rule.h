
#ifndef BDWY_RULE_H
#define BDWY_RULE_H

typedef std::list< ruleAnn * > rule_list;
typedef rule_list::iterator rule_list_p;
typedef rule_list::const_iterator rule_list_cp;


/** @brief Analysis rule annotation
 *
 */

class ruleAnn : public Ann
{
private:

  /** @brief Rule condition
   *
   * The rule condition will be null, for an unconditional rule. */

  exprAnn * _condition;

  /** @brief Condition state
   *
   *  True if the condition is true */

  bool _is_condition_true;

  /** @brief Rule effects
   *
   * These are all "Assign" expressions that define the consequences of the
   * rule firing. */

  exprann_list _effects;

 public:

  /** @brief Create a new rule
   *
   */

  ruleAnn(exprAnn * condition, exprann_list * effects, int line);

  /** @brief Return true if has condition */

  inline bool has_condition() const { return _condition != 0; }

  /** @brief Return the condition */

  inline exprAnn * condition() const { return _condition; }

  /** @brief Test the condition
   *
   * Test each condition. Special case: a rule without a condition is
   * automatically true. */

#ifdef __PROCLOCATION 
  void test(procLocation * where,
            propertyAnalyzer * property_analyzer);
#endif

  /** @brief Return the result of the test */

  bool is_condition_true() const { return _is_condition_true; }

  /** @brief Compute the next state
   *
   * When the condition is true, go through the effects and compute the
   * next state for each. */

#ifdef __PROCLOCATION 
  void compute_next(procLocation * where);
#endif

  /** @brief Apply next state
   *
   * Use the states computed by compute_next() to update the
   * variables. Return true if any of them result in changes, meaning that
   * the analysis problem has not converged. */

#ifdef __PROCLOCATION 
  void apply_next(procLocation * where,
                  propertyAnalyzer * property_analyzer,
                  memoryblock_set & changes);
#endif

  /** @brief Look up */

  void lookup(propertyAnn * default_property,
              procedureAnn * procedure,
              Annotations * annotations);

};

#endif /* BDWY_RULE_H */
