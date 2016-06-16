/**
 * \file no_vfork.cpp
 * \author Sam Kelly <kelly64@llnl.gov, kellys@dickinson.edu>
 * \date Friday, July 19, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const noVforkChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *
 * Performs basic AST match for function calls, then checks that none of the
 * names of functions are vfork.
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_NO_VFORK_H
#define COMPASS_NO_VFORK_H

namespace CompassAnalyses
{
/**
 * \brief Checks that vfork() hasn't been used
 */
namespace NoVfork
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
                      NoVfork::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::NoVfork
#endif // COMPASS_NO_VFORK_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace NoVfork
 {
  const string checker_name      = "NoVfork";
  const string short_description = "vfork() call detected!";
  const string long_description  = "Checks that vfork() hasn't been used. The vfork() function has the same effect as fork(), except that the behavior is undefined if the process created by vfork() either modifies any data other than a variable of type pid_t used to store the return value from vfork(), or returns from the function in which vfork() was called, or calls any other function before successfully calling _exit() or one of the exec family of functions.";
  string source_directory = "/";
 }
}

CompassAnalyses::NoVfork::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::noVforkChecker->checkerName,
                          ::noVforkChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::NoVfork::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      SgNode *root_node = (SgNode*)sageProject;

      AstMatching func_ref_matcher;
      MatchResult func_ref_matches = func_ref_matcher
          .performMatching("$f=SgFunctionRefExp", root_node);
      BOOST_FOREACH(SingleMatchVarBindings match, func_ref_matches)
      {
        SgFunctionRefExp *func_ref = (SgFunctionRefExp *)match["$f"];
        std::string func_str = func_ref->get_symbol()->get_name().getString();
        if(func_str.compare("vfork") == 0)
        {
          output->addOutput(
              new CompassAnalyses::NoVfork::
              CheckerOutput(func_ref));
        }
      }
      
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const noVforkChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::NoVfork::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::NoVfork::short_description,
      CompassAnalyses::NoVfork::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

