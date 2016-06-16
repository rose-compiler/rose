/**
 * \file   do_not_delete_this.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 18, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const doNotDeleteThisChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_DO_NOT_DELETE_THIS_H
#define COMPASS_DO_NOT_DELETE_THIS_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace DoNotDeleteThis
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
                      DoNotDeleteThis::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::DoNotDeleteThis
#endif // COMPASS_DO_NOT_DELETE_THIS_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace DoNotDeleteThis
 {
  const string checker_name      = "DoNotDeleteThis";
  const string short_description = "do not delete this";
  const string long_description  = "It is bad practice to delete an instance of a class from within one of its own methods.";
  string source_directory = "/";
 }
}

CompassAnalyses::DoNotDeleteThis::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::doNotDeleteThisChecker->checkerName,
                          ::doNotDeleteThisChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::DoNotDeleteThis::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_del_this;
      MatchResult result_del_this = match_del_this.performMatching("$r = SgDeleteExp(SgThisExp)", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, result_del_this)
	{
	  SgDeleteExp* del = (SgDeleteExp*)match["$r"];
	  output->addOutput(new CompassAnalyses::DoNotDeleteThis::CheckerOutput(del));
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const doNotDeleteThisChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::DoNotDeleteThis::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::DoNotDeleteThis::short_description,
      CompassAnalyses::DoNotDeleteThis::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

