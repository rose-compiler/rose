/**
 * \file   float_for_loop_counter.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 15, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace rose::StringUtility;

extern const Compass::Checker* const floatForLoopCounterChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_FLOAT_FOR_LOOP_COUNTER_H
#define COMPASS_FLOAT_FOR_LOOP_COUNTER_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace FloatForLoopCounter
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
                      FloatForLoopCounter::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::FloatForLoopCounter
#endif // COMPASS_FLOAT_FOR_LOOP_COUNTER_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace FloatForLoopCounter
 {
  const string checker_name      = "FloatForLoopCounter";
  const string short_description = "float or double used as loop counter";
  const string long_description  = "Floating point numbers should not be used as loop counters";
  string source_directory = "/";
 }
}

CompassAnalyses::FloatForLoopCounter::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::floatForLoopCounterChecker->checkerName,
                          ::floatForLoopCounterChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::FloatForLoopCounter::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_for_init;
      MatchResult result_for_init = match_for_init.performMatching("$r = SgForInitStatement", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, result_for_init)
	{
	  SgForInitStatement* for_root = (SgForInitStatement*)match["$r"];
	  AstMatching var_ref_y_init;
	  MatchResult results = var_ref_y_init.performMatching("$s = SgVarRefExp | $s = SgInitializedName", for_root);
	  BOOST_FOREACH(SingleMatchVarBindings new_match, results)
	    {
	      SgType* t;
	      SgVarRefExp* ref = isSgVarRefExp(new_match["$s"]);
	      if (ref != NULL)
		{
		  t = ref->get_type();
		}
	      else
		{
		  SgInitializedName* init = (SgInitializedName*)new_match["$s"];
		  t = init->get_type();
		}
	      if (isSgTypeDouble(t) || isSgTypeFloat(t))
		{
		  output->addOutput(new CompassAnalyses::FloatForLoopCounter::CheckerOutput(new_match["$s"]));
		}
	    }
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const floatForLoopCounterChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::FloatForLoopCounter::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::FloatForLoopCounter::short_description,
      CompassAnalyses::FloatForLoopCounter::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

