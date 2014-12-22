/**
 * \file   pointer_comparison.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 19, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const pointerComparisonChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_POINTER_COMPARISON_H
#define COMPASS_POINTER_COMPARISON_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace PointerComparison
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
                      PointerComparison::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::PointerComparison
#endif // COMPASS_POINTER_COMPARISON_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace PointerComparison
 {
  const string checker_name      = "PointerComparison";
  const string short_description = "don't use pointers in comparisons";
  const string long_description  = "Using pointers in binary comparison operators is a big no-no.";
  string source_directory = "/";
 }
}

CompassAnalyses::PointerComparison::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::pointerComparisonChecker->checkerName,
                          ::pointerComparisonChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::PointerComparison::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching matchop;
      MatchResult result = matchop.performMatching
	("$r = SgGreaterThanOp\
        | $r = SgGreaterOrEqualOp\
        | $r = SgLessThanOp\
        | $r = SgLessOrEqualOp"
	, sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, result)
	{
	  SgBinaryOp* op = (SgBinaryOp*)match["$r"];
	  SgType *lhs, *rhs;
	  lhs = op->get_lhs_operand()->get_type();
	  rhs = op->get_rhs_operand()->get_type();
	  if (isSgPointerType(lhs) || isSgPointerType(rhs))
	    {
	      output->addOutput(new CompassAnalyses::PointerComparison::CheckerOutput(op));
	    }
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const pointerComparisonChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::PointerComparison::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::PointerComparison::short_description,
      CompassAnalyses::PointerComparison::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

