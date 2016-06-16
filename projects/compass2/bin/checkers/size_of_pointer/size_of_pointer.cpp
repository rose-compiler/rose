/**
 * \file   size_of_pointer.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 16, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const sizeOfPointerChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_SIZE_OF_POINTER_H
#define COMPASS_SIZE_OF_POINTER_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace SizeOfPointer
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
                      SizeOfPointer::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::SizeOfPointer
#endif // COMPASS_SIZE_OF_POINTER_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace SizeOfPointer
 {
  const string checker_name      = "SizeOfPointer";
  const string short_description = "dereference pointer in sizeof";
  const string long_description  = "One should not assume that pointers will be the same size as the types pointed to and should not be used in sizeof() without dereferencing.";
  string source_directory = "/";
 }
}

CompassAnalyses::SizeOfPointer::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::sizeOfPointerChecker->checkerName,
                          ::sizeOfPointerChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::SizeOfPointer::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_sizeof;
      MatchResult result_sizeof = match_sizeof.performMatching("SgSizeOfOp($r=SgVarRefExp)", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, result_sizeof)
	{
	  SgVarRefExp* var_ref = (SgVarRefExp*)match["$r"];
	  if(isSgPointerType(var_ref->get_type()))
	    output->addOutput(new CompassAnalyses::SizeOfPointer::CheckerOutput(var_ref));
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const sizeOfPointerChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::SizeOfPointer::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::SizeOfPointer::short_description,
      CompassAnalyses::SizeOfPointer::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

