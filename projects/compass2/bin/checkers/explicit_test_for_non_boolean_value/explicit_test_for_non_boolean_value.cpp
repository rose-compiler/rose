/**
 * \file   explicit_test_for_non_boolean_value.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 16, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const explicitTestForNonBooleanValueChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_EXPLICIT_TEST_FOR_NON_BOOLEAN_VALUE_H
#define COMPASS_EXPLICIT_TEST_FOR_NON_BOOLEAN_VALUE_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace ExplicitTestForNonBooleanValue
{
  extern const string checker_name;
  extern const string short_description;
  extern const string long_description;
  extern       string source_directory;

  /**
   * \brief Specificaiton of checker results.
   */
  class CheckerOutput: public Compass::OutputViolationBase {
   public:
    explicit CheckerOutput(SgNode *const node);

   private:
    DISALLOW_COPY_AND_ASSIGN(CheckerOutput);
  };

  bool IsNodeNotInUserLocation(const SgNode* node)
  {
      const SgLocatedNode* located_node = isSgLocatedNode(node);
      if (located_node != NULL)
      {
          return ! Compass::IsNodeInUserLocation(
                      located_node,
                      ExplicitTestForNonBooleanValue::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::ExplicitTestForNonBooleanValue
#endif // COMPASS_EXPLICIT_TEST_FOR_NON_BOOLEAN_VALUE_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace ExplicitTestForNonBooleanValue
 {
  const string checker_name      = "ExplicitTestForNonBooleanValue";
  const string short_description = "found implicit test for non-bool value";
  const string long_description  = "Tests in conditional statements should always be explicit if the variable is not boolean.";
  string source_directory = "/";
 }
}

CompassAnalyses::ExplicitTestForNonBooleanValue::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::explicitTestForNonBooleanValueChecker->checkerName,
                          ::explicitTestForNonBooleanValueChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::ExplicitTestForNonBooleanValue::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_statements;
      MatchResult result = match_statements.performMatching
	("$r = SgIfStmt(SgExprStatement(SgCastExp),_,_)\
        | $r = SgIfStmt(SgExprStatement(SgNotOp(SgCastExp)),_,_)\
        | $r = SgWhileStmt(SgExprStatement(SgCastExp),_,_)\
	| $r = SgWhileStmt(SgExprStatement(SgNotOp(SgCastExp)),_,_)\
        | $r = SgDoWhileStmt(_,SgExprStatement(SgCastExp))\
        | $r = SgDoWhileStmt(_,SgExprStatement(SgNotOp(SgCastExp)))\
        | $r = SgForStatement(_,SgExprStatement(SgCastExp),_,_,_)\
        | $r = SgForStatement(_,SgExprStatement(SgNotOp(SgCastExp)),_,_,_)"\
	,sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, result)
	{
	  output->addOutput(new CompassAnalyses::ExplicitTestForNonBooleanValue::CheckerOutput(match["$r"]));
	}
      
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const explicitTestForNonBooleanValueChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::ExplicitTestForNonBooleanValue::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::ExplicitTestForNonBooleanValue::short_description,
      CompassAnalyses::ExplicitTestForNonBooleanValue::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

