/**
 * \file   function_prototype.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 17, 2013
 */

#include <map>

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const functionPrototypeChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_FUNCTION_PROTOTYPE_H
#define COMPASS_FUNCTION_PROTOTYPE_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace FunctionPrototype
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
                      FunctionPrototype::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::FunctionPrototype
#endif // COMPASS_FUNCTION_PROTOTYPE_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace FunctionPrototype
 {
  const string checker_name      = "FunctionPrototype";
  const string short_description = "No function prototype detected";
  const string long_description  = "Use function prototypes.";
  string source_directory = "/";
 }
}

CompassAnalyses::FunctionPrototype::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::functionPrototypeChecker->checkerName,
                          ::functionPrototypeChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::FunctionPrototype::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_func_decl;
      MatchResult result_func_decl = match_func_decl.performMatching
	("$r = SgFunctionDeclaration", sageProject);
      std::map<string, bool> has_prototype;

      BOOST_FOREACH(SingleMatchVarBindings match, result_func_decl)
	{
	  SgFunctionDeclaration* function_decl = (SgFunctionDeclaration*)match["$r"];
	  string function_name = function_decl->get_name().getString();
	  has_prototype[function_name] = false;
	}
      BOOST_FOREACH(SingleMatchVarBindings match, result_func_decl)
	{
	  SgFunctionDeclaration* function_decl = (SgFunctionDeclaration*)match["$r"];
	  string function_name = function_decl->get_name().getString();
	  if (function_decl->isForward())
	    {
	      has_prototype[function_name] = true;
	    }
	}
      BOOST_FOREACH(SingleMatchVarBindings match, result_func_decl)
	{
	  SgFunctionDeclaration* function_decl = (SgFunctionDeclaration*)match["$r"];
	  string function_name = function_decl->get_name().getString();
	  if (has_prototype[function_name] == false)
	    {
	      output->addOutput(new CompassAnalyses::FunctionPrototype::CheckerOutput(function_decl));
	    }
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const functionPrototypeChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::FunctionPrototype::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::FunctionPrototype::short_description,
      CompassAnalyses::FunctionPrototype::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

