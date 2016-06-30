/**
 * \file    function_with_multiple_returns.cpp
 * \author  Sam Kelly <kelly64@llnl.gov> <kellys@dickinson.edu>
 * \date    June 25, 2013
 */

#include <iostream>
#include <fstream>

#include <algorithm>
#include <map>

#include <boost/foreach.hpp>

#include "rose.h"
#include "AstMatching.h"
#include "CodeThorn/src/AstTerm.h"
#include "compass2/compass.h"

using namespace rose::StringUtility;

extern const Compass::Checker* const functionWithMultipleReturnsChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_FUNCTION_WITH_MULTIPLE_RETURNS_H
#define COMPASS_FUNCTION_WITH_MULTIPLE_RETURNS_H

namespace CompassAnalyses
{
  /**
   * \brief Detect functions that are never called,
   *        sometimes called "unreachable" or "dead".
   */
  namespace FunctionWithMultipleReturns
  {
    extern const std::string checker_name;
    extern const std::string short_description;
    extern const std::string long_description;
    extern std::string source_directory;

    /**
     * \brief Specificaiton of checker results.
     */
    class CheckerOutput : public Compass::OutputViolationBase
    {
     public:
      explicit CheckerOutput(SgNode * const node);

     private:
      DISALLOW_COPY_AND_ASSIGN(CheckerOutput);
    };

    bool IsNodeNotInUserLocation(const SgNode* node)
    {
      const SgLocatedNode* located_node = isSgLocatedNode(node);
      if (located_node != NULL)
      {
        return !Compass::IsNodeInUserLocation(
            located_node, FunctionWithMultipleReturns::source_directory);
      } else {
        return true;
      }
    };

  }  // ::CompassAnalyses
}  // ::FunctionWithMultipleReturns
#endif // COMPASS_FUNCTION_WITH_MULTIPLE_RETURNS_H
/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
  namespace FunctionWithMultipleReturns
  {
    const std::string checker_name = "FunctionWithMultipleReturns";
    const std::string short_description = "Function with multiple returns detected";
    const std::string long_description =
        "This analysis looks for functions with multiple returns";
    std::string source_directory = "/";
  }
}

CompassAnalyses::FunctionWithMultipleReturns::CheckerOutput::CheckerOutput(
    SgNode * const node) : OutputViolationBase(
    node, ::functionWithMultipleReturnsChecker->checkerName,
    ::functionWithMultipleReturnsChecker->shortDescription){}

//////////////////////////////////////////////////////////////////////////////

// Checker main run function and metadata

/** run(Compass::Parameters parameters, Compass::OutputObject* output)
 *
 *  Purpose
 *  ========
 *
 *  Search for functions with multiple returns
 *  int func1(int a)
 *  {
 *      if(a > 0) return 1;
 *      else return -1;
 *  }
 *
 *  ^ would be flagged because there are 2 returns
 *
 *
 *  Algorithm
 *  ==========
 *
 *  1. Search for all SgFunctionDeclarations
 *
 *  2. For each SgFunctionDeclaration, search for child nodes that
 *     are SgReturnStmt's
 */
static void run(Compass::Parameters parameters, Compass::OutputObject* output)
{
  // We only care about source code in the user's space, not,
  // for example, Boost or system files.
  std::string target_directory = parameters["general::target_directory"].front();
  CompassAnalyses::FunctionWithMultipleReturns::source_directory.assign(
      target_directory);

  // Use the pre-built ROSE AST
  SgProject* sageProject = Compass::projectPrerequisite.getProject();
  SgNode* root_node = (SgNode*) sageProject;

  // 1. Search for all SgFunctionDeclarations
  AstMatching function_matcher;
  MatchResult function_matches = function_matcher
      .performMatching("$r = SgFunctionDefinition", root_node);

  // 2. Check if there are multiple SgReturnStmt's for this SgFunctionDefinition
  BOOST_FOREACH(SingleMatchVarBindings matched_function, function_matches)
  {
    SgNode* function = matched_function["$r"];

    AstMatching return_matcher;
    MatchResult return_matches = return_matcher
        .performMatching("$r = SgReturnStmt", function);

    int num_matches = return_matches.size();

    if(num_matches > 1)
    {
      output->addOutput(
          new CompassAnalyses::FunctionWithMultipleReturns::
          CheckerOutput(function));

      // verbose output
      /*
      if (SgProject::get_verbose > 0)
      {
        std::cout << "num_returns: " << num_matches << std::endl;
      }*/
    }

  }
}


extern const Compass::Checker* const functionWithMultipleReturnsChecker =
    new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::FunctionWithMultipleReturns::checker_name,
        // Descriptions should not include the newline character "\n".
        CompassAnalyses::FunctionWithMultipleReturns::short_description,
        CompassAnalyses::FunctionWithMultipleReturns::long_description,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite), run,
        NULL);
