/**
 * \file   boolean_is_has.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 7, 2013
 */

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

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
  const string short_description = "Bool function or variable doesn't start with is_ or has_";
  const string long_description  = "Looks for any functions or variables that return boolean values whose names do not begin with is_ or has_";
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
      
      // perform AST matching here
      AstMatching function_decls;
      MatchResult func_matches = function_decls.performMatching("$r = SgFunctionDeclaration", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, func_matches)
	{
	  SgFunctionDeclaration* decl = (SgFunctionDeclaration*)match["$r"];
	  string func_name = decl->get_name().getString();
	  if (isSgTypeBool(decl->get_type()->get_return_type()) && func_name.find("is_") & func_name.find("has_"))
	    {
	      output->addOutput(new CompassAnalyses::BooleanIsHas::CheckerOutput(decl));
	    }
	}
      
      AstMatching var_init;
      MatchResult var_matches = var_init.performMatching("$r = SgInitializedName", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, var_matches)
	{
	  SgInitializedName* variable = (SgInitializedName*)match["$r"];
	  string var_name = variable->get_name().getString();
	  if (isSgTypeBool(variable->get_type()) && var_name.find("is_") & var_name.find("has_"))
	    {
	      output->addOutput(new CompassAnalyses::BooleanIsHas::CheckerOutput(variable));
	    }
	}
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

