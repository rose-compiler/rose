
#ifndef BDWY_ACTION_H
#define BDWY_ACTION_H

typedef list< actionAnn * > action_list;
typedef action_list::iterator action_list_p;
typedef action_list::const_iterator action_list_cp;

class callsiteEntry;

typedef map< callsiteEntry *, bool > candidate_map;
typedef candidate_map::iterator candidate_map_p;


/** @brief Action annotation
 *
 * */

class actionAnn : public Ann
{
public:

  typedef enum { Inline, Replace } ActionKind;

private:

  /** @brief Kind of action
   *
   * Currently, there are only two kinds: inline (inline the body of the
   * library routine) and replace (replace the call to the library routine
   * with some other code). */

  ActionKind _kind;

  /** @brief Condition
   *
   *  The condition to test for activating this substitution. */

  exprAnn * _condition;

  /** @brief Replacement statement
   *
   * This is a C code statement that provides the template for the
   * substitution. We replace elements of the statement with information
   * and variable names from the actual call site. */

  stmtNode * _replacement;

  /** @brief Candidates
   *
   * A substitution candidate is a particular callsite in the text of the
   * program (a stmtNode). For each callsite, we record the possible
   * contexts that reach it, and the truth value of the condition in that
   * context. We need this information for two reasons. First, we can only
   * modify the code when the transformation applies in all the contexts
   * (or we need to do something else, like clone the code). Second, we
   * need to record the different contexts so that we can get the variable
   * bindings during substitution. TBD: What if the condition is true is
   * all contexts, but the bindings are different? */

  candidate_map _candidates;

public:

  /** @brief Create a new "inline" action annotation
   */

  actionAnn(exprAnn * condition, int line);

  /** @brief Create a new "replace" action annotation
   *
   * Used by the parser, this constructor creates a new subsitution with
   * the given condition and substitute code. */

  actionAnn(exprAnn * condition, stmtNode * replacement, int line);

  /** @brief Return the condition */

  inline exprAnn * condition() const { return _condition; }

  /** @brief Lookup variables
   *
   * After parsing, this method is called by procedureAnn::postprocess()
   * method to lookup the variables in both the condition and the
   * substitution. */

  virtual void lookup(procedureAnn * procedure,
                      Annotations * annotations);
  
  /** @brief Test the condition
   *
   * For the given calling context, it evaluates the condition and stores
   * the results in the _candidates map. */

  void test(callsiteEntry * callsite,
            procLocation * where,
            propertyAnalyzer * property_analyzer);

  /** @brief Is applicable
   *
   * Return true if the action is applicable at the given call site */

  bool is_applicable(callsiteEntry * callsite);


  /** @brief Generate replacement code
   *
   * For inlining and replacement actions, generate the code that will
   * replace the given procedure call. */

  stmtNode * generate_replacement(callsiteEntry * callsite,
                                  propertyAnalyzer * analyzer,
                                  Linker &linker,
                                  Annotations * annotations);
};

#endif /* BDWY_ACTION_H */
