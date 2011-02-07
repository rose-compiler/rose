
#ifndef BDWY_POINTERRULE_H
#define BDWY_POINTERRULE_H

typedef std::list< pointerRuleAnn * > pointerrule_list;
typedef pointerrule_list::iterator pointerrule_list_p;
typedef pointerrule_list::const_iterator pointerrule_list_cp;


/** @brief Pointer analysis rule
 *
 */

class pointerRuleAnn : public Ann
{
private:

  /** @brief Pointerrule condition
   *
   * The pointerrule condition will be null, for an unconditional pointerrule. */

  exprAnn * _condition;

  /** @brief Condition state
   *
   *  True if the condition is true */

  bool _is_condition_true;

  /** @brief Pointer effects tree
   *
   * This is the precompiled version that is produced by the parser. It is
   * then "compiled" by the procedure annotation into the form below, in
   * _effects. */

  structuretree_list * _effects_tree;

  /** @brief Pointer effects
   *
   * This is a pointer annotation list. */

  TREE structure_list _effects;

 public:

  /** @brief Create a new pointerrule
   *
   */

  pointerRuleAnn(exprAnn * condition, structuretree_list * effects_tree, int line);

  /** @brief Destructor */

  ~pointerRuleAnn();

  /** @brief Return true if has condition */

  inline bool has_condition() const { return _condition != 0; }

  /** @brief Return the condition */

  inline exprAnn * condition() const { return _condition; }

  /** @brief Get the effects tree */

  inline structuretree_list * effects_tree() const { return _effects_tree; }

  /** @brief Get the effects list
   *
   * This list is populated by the procedureAnn object. */

  inline structure_list & effects() { return _effects; }

  /** @brief Test the condition
   *
   * Test each condition. Special case: a pointerRule without a condition is
   * automatically true. */

#ifdef __PROCLOCATION
  void test(procLocation * where,
            propertyAnalyzer * property_analyzer);
#endif
  /** @brief Return the result of the test */

  bool is_condition_true() const { return _is_condition_true; }

  /** @brief Look up */

  void lookup(procedureAnn * procedure,
              Annotations * annotations);

};

#endif /* BDWY_POINTERRULE_H */
