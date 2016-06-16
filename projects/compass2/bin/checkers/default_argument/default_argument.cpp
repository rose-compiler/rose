/**
 * \file   default_argument.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 17, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const defaultArgumentChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_DEFAULT_ARGUMENT_H
#define COMPASS_DEFAULT_ARGUMENT_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace DefaultArgument
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
                      DefaultArgument::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::DefaultArgument
#endif // COMPASS_DEFAULT_ARGUMENT_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace DefaultArgument
 {
  const string checker_name      = "DefaultArgument";
  const string short_description = "Default function argument detected";
  const string long_description  = "This analysis looks for functions with default arguments in their parameter list.";
  string source_directory = "/";
 }
}

CompassAnalyses::DefaultArgument::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::defaultArgumentChecker->checkerName,
                          ::defaultArgumentChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::DefaultArgument::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_defarg;
      MatchResult result_defarg = match_defarg.performMatching
	("$r = SgFunctionParameterList", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, result_defarg)
	{
	  SgFunctionParameterList* fpl = (SgFunctionParameterList*)match["$r"];
	  AstMatching match_assign;
	  MatchResult result_assign = match_assign.performMatching
	    ("$s = SgAssignInitializer", fpl);
	  BOOST_FOREACH(SingleMatchVarBindings new_match, result_assign)
	    {
	      output->addOutput(new CompassAnalyses::DefaultArgument::CheckerOutput(new_match["$s"]));
	    }
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const defaultArgumentChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::DefaultArgument::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::DefaultArgument::short_description,
      CompassAnalyses::DefaultArgument::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

