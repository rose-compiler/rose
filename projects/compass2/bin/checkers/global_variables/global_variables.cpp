/**
 * \file global_variables.cpp
 * \author Sam Kelly <kelly64@llnl.gov> <kellys@dickinson.edu>
 * \date July 3, 2013
 */

#include "rose.h"
#include "compass2/compass.h"
#include <boost/foreach.hpp>
#include "AstMatching.h"


using std::string;
using namespace StringUtility;

extern const Compass::Checker* const globalVariablesChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *
 * Algorithm: Does an AST match for all SgVariableDeclarations that do not
 * have any SgBasicBlock's as an ancestor
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_GLOBAL_VARIABLES_H
#define COMPASS_GLOBAL_VARIABLES_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace GlobalVariables
{
  extern const string checker_name;
  extern const string short_description;
  extern const string long_description;
  extern       string source_directory;

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
                      GlobalVariables::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::GlobalVariables
#endif // COMPASS_GLOBAL_VARIABLES_H

/*-----------------------------------------------------------------------------
 * Implementation
 *
 * Performs a simple AST matching operation that finds all variable
 * declarations that are not contained within a method body.
 *
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace GlobalVariables
 {
  const string checker_name      = "GlobalVariables";
  const string short_description = "Global Variable found";
  const string long_description  = "Finds all variable declarations that occur outside of a method body";
  string source_directory = "/";
 }
}

CompassAnalyses::GlobalVariables::
CheckerOutput::CheckerOutput(SgNode *const node)
:OutputViolationBase(node,
                          ::globalVariablesChecker->checkerName,
                          ::globalVariablesChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::GlobalVariables::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      SgNode* root_node = (SgNode*) sageProject;

      AstMatching matcher;
      MatchResult matches = matcher
          .performMatching("#SgClassDefinition|#SgBasicBlock|$r=SgVariableDeclaration", root_node);

      BOOST_FOREACH(SingleMatchVarBindings match, matches)
      {
        SgVariableDeclaration *variable_dec = (SgVariableDeclaration *)match["$r"];
        output->addOutput(
            new CompassAnalyses::GlobalVariables::
            CheckerOutput(variable_dec));
      }
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const globalVariablesChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::GlobalVariables::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::GlobalVariables::short_description,
      CompassAnalyses::GlobalVariables::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

