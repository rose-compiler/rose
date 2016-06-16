/**
 * \file   ternary_operator.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 15, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const ternaryOperatorChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_TERNARY_OPERATOR_H
#define COMPASS_TERNARY_OPERATOR_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace TernaryOperator
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
                      TernaryOperator::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::TernaryOperator
#endif // COMPASS_TERNARY_OPERATOR_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace TernaryOperator
 {
  const string checker_name      = "TernaryOperator";
  const string short_description = "Ternary operator was used";
  const string long_description  = "if/elseif/else is easier for people to look at than the ternary operator";
  string source_directory = "/";
 }
}

CompassAnalyses::TernaryOperator::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::ternaryOperatorChecker->checkerName,
                          ::ternaryOperatorChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::TernaryOperator::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_ternary;
      MatchResult result_ternary = match_ternary.performMatching("$r = SgConditionalExp", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, result_ternary)
	{
	  SgConditionalExp* tri = (SgConditionalExp*)match["$r"];
	  output->addOutput(new CompassAnalyses::TernaryOperator::CheckerOutput(tri));
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const ternaryOperatorChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::TernaryOperator::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::TernaryOperator::short_description,
      CompassAnalyses::TernaryOperator::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

