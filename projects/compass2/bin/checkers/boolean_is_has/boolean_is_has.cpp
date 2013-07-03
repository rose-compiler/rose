/**
 * \file
 * \author
 * \date
 */

#include "rose.h"
#include "compass2/compass.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const booleanIsHasChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_BOOLEAN_IS_HAS_H
#define COMPASS_BOOLEAN_IS_HAS_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace BooleanIsHas
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
                      BooleanIsHas::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::BooleanIsHas
#endif // COMPASS_BOOLEAN_IS_HAS_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace BooleanIsHas
 {
  const string checker_name      = "BooleanIsHas";
  const string short_description = "";
  const string long_description  = "";
  string source_directory = "/";
 }
}

CompassAnalyses::BooleanIsHas::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::booleanIsHasChecker->checkerName,
                          ::booleanIsHasChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::BooleanIsHas::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      SgNode* root_node = (SgNode*) sageProject;
      
      // perform AST matching here
      
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const booleanIsHasChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::BooleanIsHas::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::BooleanIsHas::short_description,
      CompassAnalyses::BooleanIsHas::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

