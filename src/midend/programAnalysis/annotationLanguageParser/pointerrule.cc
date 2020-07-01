
#include "broadway.h"
#include <assert.h>
#include "RoseAsserts.h" /* JFR: Added 17Jun2020 */

using namespace std;

pointerRuleAnn::pointerRuleAnn(exprAnn * condition, structuretree_list * effects_tree, int line)
  : Ann(line),
    _condition(condition),
    _is_condition_true(false),
    _effects_tree(effects_tree),
    _effects()
{}

pointerRuleAnn::~pointerRuleAnn()
{
  structure_list_p p;

  delete _condition;

  delete _effects_tree;

  for (p = _effects.begin();
       p != _effects.end();
       ++p)
    delete (*p);

}

pointerRuleAnn::pointerRuleAnn(const pointerRuleAnn & X)
   : Ann(X)
   {
  // DQ (9/13/2011): This copy constructor was built because static analysis tools
  // suggested it would avoid a possible double free error.  I agree.
     printf ("Error: it is an error to call this copy constructor. \n");
     assert(false);
   }


#ifdef __PROCLOCATION
void pointerRuleAnn::test(procLocation * where,
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
#endif

void pointerRuleAnn::lookup(procedureAnn * procedure,
                            Annotations * annotations)
{
  propertyAnn * no_property = 0;

  // -- Call lookup on the condition expression

  if (_condition)
    _condition->lookup(no_property, procedure, annotations);

  // -- Looking up the effects is handled by procedureAnn
}

