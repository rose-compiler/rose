/**
 * \file    floating_point_exact_comparison.cpp
 * \author  Sam Kelly <kelly64@llnl.gov> <kellys@dickinson.edu>
 * \date    June 26, 2013
 */

#include <iostream>
#include <fstream>
#include "rose.h"
#include "string_functions.h"
#include "AstMatching.h"

#include <boost/foreach.hpp>

#include "compass2/compass.h"

using namespace rose::StringUtility;
using std::string;

extern const Compass::Checker* const floatingPointExactComparisonChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_FLOATING_POINT_EXACT_COMPARISON_H
#define COMPASS_FLOATING_POINT_EXACT_COMPARISON_H

namespace CompassAnalyses
{
  /**
   * \brief Detect float on float comparisons that are prone
   *        to floating point error
   */
  namespace FloatingPointExactComparison
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
            FloatingPointExactComparison::source_directory);
      }
      else
      {
        return true;
      }
    };

  } // ::CompassAnalyses
} // ::FloatingPointExactComparison
#endif // COMPASS_FLOATING_POINT_EXACT_COMPARISON_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
  namespace FloatingPointExactComparison
  {
    const string checker_name      = "FloatingPointExactComparison";
    const string short_description = "Exact (==) comparison between two floats detected";
    const string long_description  = "This analysis looks for exact comparisons (e.g. a == b) between two float or double types (this should be avoided due to floating point error)";
    string source_directory = "/";
  }
}

CompassAnalyses::FloatingPointExactComparison::
CheckerOutput::CheckerOutput(SgNode *const node)
: OutputViolationBase(node,
                      ::floatingPointExactComparisonChecker->checkerName,
                       ::floatingPointExactComparisonChecker->shortDescription) {}

//////////////////////////////////////////////////////////////////////////////

// Checker main run function and metadata

/** run(Compass::Parameters parameters, Compass::OutputObject* output)
 *
 *  Purpose
 *  ========
 *
 *  Search for float comparisons that are prone to floating point error
 *    e.g.:
 *    float a = 0.233;
 *    if(a == 0.233) return 0; // this is prone to floating point error
 *
 *
 *  Algorithm
 *  ==========
 *
 *  1. Collect all SgEqualityOp's (comparisons)
 *
 *  2. For each SgEqualityOp, check if the LHS and RHS are both float types.
 *     If so, activate the checker on the corresponding SgEqualtiyOp
 */
static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
{
  // We only care about source code in the user's space, not,
  // for example, Boost or system files.
  string target_directory = parameters["general::target_directory"].front();
  CompassAnalyses::FloatingPointExactComparison::source_directory.assign(
      target_directory);

  // Use the pre-built ROSE AST
  SgProject* sageProject = Compass::projectPrerequisite.getProject();
  SgNode* root_node = (SgNode*) sageProject;

  // Search for all SgEqualityOps
  AstMatching equality_matcher;
  MatchResult equality_matches = equality_matcher
      .performMatching("$r = SgEqualityOp", root_node);

  // for each SgEqualityOp check if the LHS and RHS are both float types
  BOOST_FOREACH(SingleMatchVarBindings matched_equality, equality_matches)
  {
    SgEqualityOp *comparison = (SgEqualityOp *)matched_equality["$r"];
    if(comparison->get_lhs_operand_i() == NULL ||
        comparison->get_lhs_operand_i()->get_type() == NULL ||
        comparison->get_rhs_operand_i() == NULL ||
        comparison->get_rhs_operand_i()->get_type() == NULL)
    {
      continue;  // skip if we can't do our comparison
    }
    //                                       (could change to && for less strict)
    if(comparison->get_lhs_operand_i()->get_type()->isFloatType() || // ^
        comparison->get_rhs_operand_i()->get_type()->isFloatType())
    {
      output->addOutput(
          new CompassAnalyses::FloatingPointExactComparison::
          CheckerOutput(comparison));
    }
  }
}

extern const Compass::Checker* const floatingPointExactComparisonChecker =
    new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::FloatingPointExactComparison::checker_name,
        // Descriptions should not include the newline character "\n".
        CompassAnalyses::FloatingPointExactComparison::short_description,
        CompassAnalyses::FloatingPointExactComparison::long_description,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        NULL);

