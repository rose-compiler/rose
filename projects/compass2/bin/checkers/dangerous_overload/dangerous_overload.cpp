/**
 * \file   dangerous_overload.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 18, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const dangerousOverloadChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_DANGEROUS_OVERLOAD_H
#define COMPASS_DANGEROUS_OVERLOAD_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace DangerousOverload
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
                      DangerousOverload::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::DangerousOverload
#endif // COMPASS_DANGEROUS_OVERLOAD_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace DangerousOverload
 {
  const string checker_name      = "DangerousOverload";
  const string short_description = "avoid overloading & && || ,";
  const string long_description  = "Some operators can lead to confusing and hard to find bugs when they are overloaded.";
  string source_directory = "/";
 }
}

CompassAnalyses::DangerousOverload::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::dangerousOverloadChecker->checkerName,
                          ::dangerousOverloadChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::DangerousOverload::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_member;
      MatchResult func_matches = match_member.performMatching("$r = SgMemberFunctionDeclaration", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, func_matches)
        {
          SgMemberFunctionDeclaration* decl = (SgMemberFunctionDeclaration*)match["$r"];
          string name = decl->get_name().getString();
          if (name == "operator&" || name == "operator&&" || name == "operator||" || name == "operator,")
            {
              output->addOutput(new CompassAnalyses::DangerousOverload::CheckerOutput(decl));
            }
        }
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const dangerousOverloadChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::DangerousOverload::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::DangerousOverload::short_description,
      CompassAnalyses::DangerousOverload::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

