/**
 * \file   comma_operator.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 10, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const commaOperatorChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_COMMA_OPERATOR_H
#define COMPASS_COMMA_OPERATOR_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace CommaOperator
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
                      CommaOperator::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::CommaOperator
#endif // COMPASS_COMMA_OPERATOR_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace CommaOperator
 {
  const string checker_name      = "CommaOperator";
  const string short_description = "The comma operator should not be used";
  const string long_description  = "The comma operator (not the seperator) should not be used.";
  string source_directory = "/";
 }
}

CompassAnalyses::CommaOperator::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::commaOperatorChecker->checkerName,
                          ::commaOperatorChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::CommaOperator::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_comma;
      MatchResult results_comma = match_comma.performMatching("$r = SgCommaOpExp", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, results_comma)
	{
	  SgCommaOpExp* op = (SgCommaOpExp*)match["$r"];
	  output->addOutput(new CompassAnalyses::CommaOperator::CheckerOutput(op));
	}
      
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const commaOperatorChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::CommaOperator::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::CommaOperator::short_description,
      CompassAnalyses::CommaOperator::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

