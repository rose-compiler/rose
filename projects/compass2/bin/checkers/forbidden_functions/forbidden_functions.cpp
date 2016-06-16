/**
 * \file   forbidden_functions.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 10, 2013
 */

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const forbiddenFunctionsChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_FORBIDDEN_FUNCTIONS_H
#define COMPASS_FORBIDDEN_FUNCTIONS_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace ForbiddenFunctions
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
                      ForbiddenFunctions::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::ForbiddenFunctions
#endif // COMPASS_FORBIDDEN_FUNCTIONS_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace ForbiddenFunctions
 {
  const string checker_name      = "ForbiddenFunctions";
  const string short_description = "forbidden function found";
  const string long_description  = "Checks for usage of blacklisted functions.";
  string source_directory = "/";
 }
}

CompassAnalyses::ForbiddenFunctions::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::forbiddenFunctionsChecker->checkerName,
                          ::forbiddenFunctionsChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
    std::map<string, string> blacklist;
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::ForbiddenFunctions::source_directory.assign(target_directory);
      
      Compass::ParametersMap forbidden = parameters[boost::regex("forbiddenFunctions::.*$")];
      BOOST_FOREACH(const Compass::ParametersMap::value_type& pair, forbidden)
	{
	  Compass::ParameterValues values = pair.second;
	  BOOST_FOREACH(string func, values)
	    {
	      blacklist[func] = func;
	    }
	}

      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_func_ref;
      MatchResult res_function_refs = match_func_ref.performMatching("$r = SgFunctionRefExp", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, res_function_refs)
	{
	  SgFunctionRefExp* reference = (SgFunctionRefExp*)match["$r"];
	  string function_name = reference->get_symbol()->get_name().getString();
	  if(blacklist[function_name] == function_name)
	    {
	      output->addOutput(new CompassAnalyses::ForbiddenFunctions::CheckerOutput(reference));
	    }
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const forbiddenFunctionsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::ForbiddenFunctions::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::ForbiddenFunctions::short_description,
      CompassAnalyses::ForbiddenFunctions::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

