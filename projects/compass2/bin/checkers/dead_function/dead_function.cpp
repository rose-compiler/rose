/**
 * \file dead_function.cpp
 * \author Sam Kelly <kelly64@llnl.gov, kellys@dickinson.edu>
 * \date Friday, July 19, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"
#include "SgNodeHelper.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const deadFunctionChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_DEAD_FUNCTION_H
#define COMPASS_DEAD_FUNCTION_H

namespace CompassAnalyses
{
/**
 * \brief Detects dead or "unreachable" functions or methods. These are functions
 * and methods that are never called
 */
namespace DeadFunction
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
                      DeadFunction::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::DeadFunction
#endif // COMPASS_DEAD_FUNCTION_H

/*-----------------------------------------------------------------------------
 * Implementation
 *
 * 1. Build a hash set of all called functions by doing an AST match
 *    for all SgFunctionCallExp nodes
 *
 * 2. Do an AST match for all SgFunctionDefinitions. If there are any
 *    that aren't contained in the called_functions hash set, then these
 *    are dead functions
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace DeadFunction
 {
  const string checker_name      = "DeadFunction";
  const string short_description = "Dead/unreachable function detected!";
  const string long_description  = "Detects dead/unreachable functions and methods";
  string source_directory = "/";
 }
}

CompassAnalyses::DeadFunction::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::deadFunctionChecker->checkerName,
                          ::deadFunctionChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::DeadFunction::source_directory.assign(target_directory);

      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();

      SgNode *root_node = (SgNode*)sageProject;

      // maintains a hash set of all functions that have been called
      boost::unordered_set<SgFunctionDefinition*> called_functions;

      AstMatching func_ref_matcher;
      MatchResult func_ref_matches = func_ref_matcher
          .performMatching("$f=SgFunctionCallExp", root_node);
      BOOST_FOREACH(SingleMatchVarBindings match, func_ref_matches)
      {
        SgFunctionCallExp *func_call = (SgFunctionCallExp *)match["$f"];
        SgFunctionDefinition *func_def =
            SgNodeHelper::determineFunctionDefinition(func_call);
        if(func_def == NULL) continue;
        called_functions.insert(func_def);
      }

      // if a function isn't in the called_functions hash set
      // then it is a dead function
      AstMatching func_dec_matcher;
      MatchResult func_dec_matches = func_dec_matcher
          .performMatching("$f=SgFunctionDeclaration", root_node);
      BOOST_FOREACH(SingleMatchVarBindings match, func_dec_matches)
      {
        SgFunctionDeclaration *func_dec = (SgFunctionDeclaration *)match["$f"];
        SgFunctionDefinition *func_def = isSgFunctionDefinition(func_dec->get_definition());
        bool report = false;
        if(func_def != NULL)
        {
          if(called_functions.find(func_def) == called_functions.end())
          {
            // function definition was found that is never called
            // ignore main()
            std::string func_name = func_def->get_declaration()->get_name().getString();
            if(func_name.compare("main") == 0) continue;
            report = true;
          }
        } else report = true; // functions without a definition are dead
        if(report)
          output->addOutput(
              new CompassAnalyses::DeadFunction::
              CheckerOutput(func_dec));
      }

  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const deadFunctionChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::DeadFunction::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::DeadFunction::short_description,
      CompassAnalyses::DeadFunction::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

