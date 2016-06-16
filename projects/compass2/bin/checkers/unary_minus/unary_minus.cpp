/**
 * \file   unary_minus.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 10, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const unaryMinusChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_UNARY_MINUS_H
#define COMPASS_UNARY_MINUS_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace UnaryMinus
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
                      UnaryMinus::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::UnaryMinus
#endif // COMPASS_UNARY_MINUS_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace UnaryMinus
 {
  const string checker_name      = "UnaryMinus";
  const string short_description = "Unary minus used with unsigned type";
  const string long_description  = "The unary minus operation should only be used with signed types.";
  string source_directory = "/";
 }
}

CompassAnalyses::UnaryMinus::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::unaryMinusChecker->checkerName,
                          ::unaryMinusChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::UnaryMinus::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_var;
      MatchResult result_var = match_var.performMatching
	("SgMinusOp($r=SgVarRefExp)\
        | SgMinusOp(SgCastExp($r=SgVarRefExp))"\
        , sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, result_var)
	{
	  SgVarRefExp* var = (SgVarRefExp*)match["$r"];
	  if (var->get_type()->isUnsignedType())
	    {
	      output->addOutput(new CompassAnalyses::UnaryMinus::CheckerOutput(var));
	    }
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const unaryMinusChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::UnaryMinus::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::UnaryMinus::short_description,
      CompassAnalyses::UnaryMinus::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

