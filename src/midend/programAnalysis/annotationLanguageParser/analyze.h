
#ifndef BDWY_ANALYZE_H
#define BDWY_ANALYZE_H

typedef std::map< propertyAnn *, analyzeAnn *> analyze_map;
typedef analyze_map::iterator analyze_map_p;
typedef analyze_map::const_iterator analyze_map_cp;


/** @brief Analyze annotation
 *
 */

class analyzeAnn : public Ann
{
private:

  /** @brief the property begin analyzed */

  REF propertyAnn * _property;

  /** @brief the list of rules */

  TREE rule_list _rules;

  /** @brief a pointer to the default rule */

  REF ruleAnn * _default;

  /** @brief Applicable rule
   *
   * After testing the conditions, compute_next() will determine a rule to
   * fire. Store this rule here so that apply_next() can execute it. */

  REF ruleAnn * _applicable_rule;

public:

  /** @brief Create a new analyze annotation
   * */

  analyzeAnn(Annotations * anns,
             const parserID * property_name,
	     rule_list * rules,
	     int line);

  /** @brief Return the property */

  inline propertyAnn * property() const { return _property; }

  /** @brief Test all the conditions
   *
   * */

#ifdef __PROCLOCATION
  void test(procLocation * where,
	    propertyAnalyzer * property_analyzer);
#endif

  /** @brief Compute the next state
   *
   *  For each true condition, figure out the state changes implied by the
   *  effects. Store the proposed changes in the updates map. */

#ifdef __PROCLOCATION
  void compute_next(procLocation * where,
		    propertyAnalyzer * property_analyzer);
#endif

  /** @brief Apply next state
   *
   * Use the states computed by compute_next() to update the
   * variables. Store the changed blocks in the changes set. */

#ifdef __PROCLOCATION
  void apply_next(procLocation * where,
		  propertyAnalyzer * property_analyzer,
		  memoryblock_set & changes);
#endif
  /** @brief Look up */

  void lookup(procedureAnn * procedure,
	      Annotations * annotations);

};

#endif /* BDWY_ANALYZE_H */
