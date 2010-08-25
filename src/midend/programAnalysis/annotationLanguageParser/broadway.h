
#ifndef BDWY_H
#define BDWY_H

#define TREE
#define REF

// DQ (12/30/2005): This is a Bad Bad thing to do (I can explain)
// it hides names in the global namespace and causes errors in 
// otherwise valid and useful code. Where it is needed it should
// appear only in *.C files (and only ones not included for template 
// instantiation reasons) else they effect user who use ROSE unexpectedly.
// using namespace std;

#include <iostream>

#ifdef __C_BREEZE_H
#include "c_breeze.h"
#endif 
#ifdef __POINTERS_H
#include "pointers.h"
#endif
#ifdef __ID_LOOKUP_WALKER_H
#include "id_lookup_walker.h"
#endif
#ifdef __META_H
#include "meta.h"
#endif

class exprAnn;
class enumPropertyExprAnn;
class procedureAnn;
class analyzeAnn;
class structureAnn;
class Annotations;
class Analyzer;
class propertyAnalyzer;
class idNodeAnn;
class annVariable;
class reportAnn;
class ruleAnn;
class pointerRuleAnn;
class actionAnn;

class propertyAnn;
class enumValueAnn;
class enumPropertyAnn;
class setPropertyAnn;

class enumvalue_set;

class Adaptor_Statistics;  // TB

class Broadway
{
public:

  /** @brief Kinds of operators */

  typedef enum { /* Logical connectives: */    And, Or, Not,
                 /* Property tests: */         Is_Exactly, Is_AtLeast, Could_Be, Is_AtMost, Is_Bottom, Report,
                 /* Numeric tests: */          Evaluate, Is_Constant,
                 /* Pointer alias tests: */    Is_AliasOf, Is_SameAs, Is_Empty,
                 /* Flow-value update: */      Assign,
                 /* Sets: */                   Is_Element_Of, Is_EmptySet, Add_Elements,
                 /* Equivalences: */           Is_Equivalent, Add_Equivalences

  } Operator;

  /** @brief Flow sensitivity modes */

  typedef enum { None, Before, After, Always, Ever,
                 Trace, Confidence
  } FlowSensitivity;
};

#include "ann.h"

#include "annvariable.h"

#ifdef INCLUDE_EXTRA
#include "callingcontext.h"

#endif
#include "property.h"
#include "enum_property.h"
#include "set_property.h"

// DQ (8/12/2004): Changed name to avoid conflit with EDG "expr.h" file
// #include "expr.h"
#include "broadway_expr.h"

#include "structure.h"
#include "rule.h"

#include "pointerrule.h"
#include "report.h"
#ifdef INCLUDE_EXTRA
#include "action.h"
#endif
#include "analyze.h"
#include "procedure.h"

#include "annotations.h"
#ifdef INCLUDE_EXTRA
#ifdef __PROPERTYANALYZER_H
#include "propertyanalyzer.h"
#endif
#include "bdwy_liveness.h"
#ifdef __ANALYZER_H
#include "analyzer.h"
#endif

#include "cpattern.h"

#include "actionchanger.h"

#include "adaptor_statistics.h"
#endif

#endif /* BDWY_H */
