/**
 * \file no_variadic_functions.cpp
 * \author Sam Kelly <kelly64@llnl.gov, kellys@dickinson.edu>
 * \date Friday, July 19, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const noVariadicFunctionsChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_NO_VARIADIC_FUNCTIONS_H
#define COMPASS_NO_VARIADIC_FUNCTIONS_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace NoVariadicFunctions
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
        NoVariadicFunctions::source_directory);
  }
  else
  {
    return true;
  }
};

} // ::CompassAnalyses
} // ::NoVariadicFunctions
#endif // COMPASS_NO_VARIADIC_FUNCTIONS_H

/*-----------------------------------------------------------------------------
 * Implementation
 *
 * 1. Find all SgFunctionDeclarations
 * 2. For each SgFunction Declaration, see if any of the parameters is an
 *    SgTypeEllipse. If so, then this function is variadic.
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
namespace NoVariadicFunctions
{
const string checker_name      = "NoVariadicFunctions";
const string short_description = "variadic function found!";
const string long_description  = "This checker forbids the definition of variadic functions. A variadic function – a function declared with a parameter list ending with ellipsis (...) – can accept a varying number of arguments of differing types. Variadic functions are flexible, but they are also hazardous. The compiler can't verify that a given call to a variadic function passes an appropriate number of arguments or that those arguments have appropriate types. Consequently, a runtime call to a variadic function that passes inappropriate arguments yields undefined behavior. Such undefined behavior could be exploited to run arbitrary code.";
string source_directory = "/";
}
}

CompassAnalyses::NoVariadicFunctions::
CheckerOutput::CheckerOutput(SgNode *const node)
: OutputViolationBase(node,
                      ::noVariadicFunctionsChecker->checkerName,
                       ::noVariadicFunctionsChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
{
  // We only care about source code in the user's space, not,
  // for example, Boost or system files.
  string target_directory =
      parameters["general::target_directory"].front();
  CompassAnalyses::NoVariadicFunctions::source_directory.assign(target_directory);

  // Use the pre-built ROSE AST
  SgProject* sageProject = Compass::projectPrerequisite.getProject();

  SgNode *root_node = (SgNode *)sageProject;

  AstMatching func_dec_matcher;
  MatchResult func_dec_matches = func_dec_matcher
      .performMatching("$f=SgFunctionDeclaration", root_node);
  BOOST_FOREACH(SingleMatchVarBindings func_dec_match, func_dec_matches)
  {
    SgFunctionDeclaration *func_dec = (SgFunctionDeclaration *)func_dec_match["$f"];
    SgFunctionParameterList *func_params = func_dec->get_parameterList();
    for(int i = 0; i < func_params->get_numberOfTraversalSuccessors(); i++)
    {
      SgInitializedName* func_param =
          isSgInitializedName(func_params->get_traversalSuccessorByIndex(i));
      if(isSgTypeEllipse(func_param->get_type()) != NULL)
      {
        // then this is a variadic function
        output->addOutput(
            new CompassAnalyses::NoVariadicFunctions::
            CheckerOutput(func_dec));
      }
    }
  }


}

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
{
  return NULL;
}

extern const Compass::Checker* const noVariadicFunctionsChecker =
    new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoVariadicFunctions::checker_name,
        // Descriptions should not include the newline character "\n".
        CompassAnalyses::NoVariadicFunctions::short_description,
        CompassAnalyses::NoVariadicFunctions::long_description,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);

