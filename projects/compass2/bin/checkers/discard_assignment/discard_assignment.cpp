/**
 * \file   discard_assignment.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 19, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const discardAssignmentChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_DISCARD_ASSIGNMENT_H
#define COMPASS_DISCARD_ASSIGNMENT_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace DiscardAssignment
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
                      DiscardAssignment::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::DiscardAssignment
#endif // COMPASS_DISCARD_ASSIGNMENT_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace DiscardAssignment
 {
  const string checker_name      = "DiscardAssignment";
  const string short_description = "Result of assignment should be discarded";
  const string long_description  = "Always discard the result of the assignment operator to avoid confusion with the comparison operator.";
  string source_directory = "/";
 }
}

CompassAnalyses::DiscardAssignment::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::discardAssignmentChecker->checkerName,
                          ::discardAssignmentChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::DiscardAssignment::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_assign;
      MatchResult result_assign = match_assign.performMatching("$r = SgAssignOp", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, result_assign)
	{
	  SgAssignOp* op = (SgAssignOp*)match["$r"];
	  if (!isSgExprStatement(op->get_parent()))
	    {
	      output->addOutput(new CompassAnalyses::DiscardAssignment::CheckerOutput(op));
	    }
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const discardAssignmentChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::DiscardAssignment::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::DiscardAssignment::short_description,
      CompassAnalyses::DiscardAssignment::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

