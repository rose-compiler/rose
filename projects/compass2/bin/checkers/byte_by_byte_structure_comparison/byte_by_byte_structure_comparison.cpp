/**
 * \file   byte_by_byte_structure_comparison.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 16, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const byteByByteStructureComparisonChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_BYTE_BY_BYTE_STRUCTURE_COMPARISON_H
#define COMPASS_BYTE_BY_BYTE_STRUCTURE_COMPARISON_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace ByteByByteStructureComparison
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
                      ByteByByteStructureComparison::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::ByteByByteStructureComparison
#endif // COMPASS_BYTE_BY_BYTE_STRUCTURE_COMPARISON_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace ByteByByteStructureComparison
 {
  const string checker_name      = "ByteByByteStructureComparison";
  const string short_description = "structures compared byte by byte";
  const string long_description  = "Byte by byte structure comparison is unreliable and should not be done.";
  string source_directory = "/";
 }
}

CompassAnalyses::ByteByByteStructureComparison::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::byteByByteStructureComparisonChecker->checkerName,
                          ::byteByByteStructureComparisonChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::ByteByByteStructureComparison::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_functions;
      MatchResult result_functions =
	match_functions.performMatching("$r = SgFunctionCallExp(SgFunctionRefExp,SgExprListExp(_,_,_))", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, result_functions)
	{
	  SgFunctionCallExp* function_call = (SgFunctionCallExp*)match["$r"];
	  AstMatching match_vars;
	  MatchResult result_vars;
	  if ("memcmp" == function_call->getAssociatedFunctionDeclaration()->get_name().getString())
	    result_vars = match_vars.performMatching("$s = SgVarRefExp", function_call);
	  else
	    continue;
	  bool struct_used = false;
	  BOOST_FOREACH(SingleMatchVarBindings var_match, result_vars)
	    {
	      SgVarRefExp* var = (SgVarRefExp*)var_match["$s"];
	      if (isSgClassType(var->get_type()->findBaseType()) != NULL)
		struct_used = true;
	    }
	  if (struct_used)
	    output->addOutput(new CompassAnalyses::ByteByByteStructureComparison::CheckerOutput(function_call));
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const byteByByteStructureComparisonChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::ByteByByteStructureComparison::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::ByteByByteStructureComparison::short_description,
      CompassAnalyses::ByteByByteStructureComparison::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

