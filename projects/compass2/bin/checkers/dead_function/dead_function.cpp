/**
 * \file    dead_function.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    August 16, 2012
 */

#include "rose.h"
#include "string_functions.h"

#include <algorithm>
#include <map>

#include <boost/foreach.hpp>

#include "compass2/compass.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const deadFunctionChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_FUNCTION_CALL_COVERAGE_H
#define COMPASS_FUNCTION_CALL_COVERAGE_H

namespace CompassAnalyses
{
/**
 * \brief Detect functions that are never called,
 *        sometimes called "unreachable" or "dead".
 */
namespace DeadFunction
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
                      DeadFunction::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::DeadFunction
#endif // COMPASS_FUNCTION_CALL_COVERAGE_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace DeadFunction
 {
  const string checker_name      = "DeadFunction";
  const string short_description = "Dead function detected";
  const string long_description  = "This analysis looks for functions \
      that are never invoked (dead functions).";
  string source_directory = "/";
 }
}

CompassAnalyses::DeadFunction::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::deadFunctionChecker->checkerName,
                          ::deadFunctionChecker->shortDescription) {}

//////////////////////////////////////////////////////////////////////////////

// Checker main run function and metadata

/** run(Compass::Parameters parameters, Compass::OutputObject* output)
 *
 *  Purpose
 *  ========
 *
 *  Search for functions that are "dead", or "unreachable":
 *    1. Never called: `foo();`
 *    2. Never defined `void foo() { ... } // definition`
 *
 *
 *  Algorithm
 *  ==========
 *
 *  1. Collect all function declarations (SgFunctionDeclaration) within
 *     the user-configurable location.
 *
 *  2. Collect all function calls (SgFunctionCallExp) within the
 *     user-configurable location.
 *
 *  3. Cross-check the function declarations against the function calls
 */
static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::DeadFunction::source_directory.assign(target_directory);

      // (1)
      // Collect all SgFunctionDeclarations in the
      // user-configurable location:

      // Keep a collection of the target SgFunctionDeclarations.
      // Use a map to eliminate duplicates and for O(1) access:
      std::map<std::string, SgFunctionDeclaration*> function_decl_map;

      // Get the SgFunctionDeclarations
      Rose_STL_Container<SgNode*> function_decls =
          NodeQuery::querySubTree(Compass::projectPrerequisite.getProject(),
                                  V_SgFunctionDeclaration);

     // Create some iterators for the SgFunctionDeclaration collection:
      Rose_STL_Container<SgNode*>::iterator ibegin_function_decls =
          function_decls.begin();
      Rose_STL_Container<SgNode*>::iterator iend_function_decls =
          function_decls.end();

      // Remove the SgFunctionDeclarations that the user doesn't care about:
      iend_function_decls =
          remove_if(ibegin_function_decls,
                    iend_function_decls,
                    CompassAnalyses::DeadFunction::IsNodeNotInUserLocation);

      // Add the SgFunctionDeclarations to the map collection.
      // Map `unparsed SgFunctionDeclaration string`
      //  to `SgFunctionDeclaration`
      for ( ;
            ibegin_function_decls != iend_function_decls;
            ++ibegin_function_decls)
      {
          SgFunctionDeclaration* function_decl =
              isSgFunctionDeclaration(*ibegin_function_decls);
          ROSE_ASSERT(function_decl != NULL);

          // Remove duplicates by using the function prototype
          // (first non-defining declaration) if it is available.
          SgDeclarationStatement* decl_statement =
              function_decl->get_firstNondefiningDeclaration();
          if (decl_statement != NULL)
          {
              function_decl = isSgFunctionDeclaration(decl_statement);
              ROSE_ASSERT(function_decl != NULL);
          }

          std::string function_decl_unparsed =
              function_decl->unparseToString();
          ROSE_ASSERT(function_decl_unparsed.size() > 0);

          // Add to map
          function_decl_map[function_decl_unparsed] =
              function_decl;
      }

      // (2)
      // Collect all SgFunctionCallExp in the
      // user-configurable location:

      // Keep a map of all SgFunctionCallExp's associated
      // SgFunctionDeclarations (unparsed as strings) for O(1) access:
      // [SgFunctionCallExp -> SgFunctionDeclaration unparsed] =>
      //      SgFunctionDeclaration object
      std::map<std::string, SgFunctionDeclaration*> function_call_map;

      // Get the SgFunctionDeclarations
      Rose_STL_Container<SgNode*> function_call_decls =
          NodeQuery::querySubTree(Compass::projectPrerequisite.getProject(),
                                  V_SgFunctionCallExp);

     // Create some iterators for the SgFunctionDeclaration collection:
      Rose_STL_Container<SgNode*>::iterator ibegin_function_call_decls =
          function_call_decls.begin();
      Rose_STL_Container<SgNode*>::iterator iend_function_call_decls =
          function_call_decls.end();

      // Remove the SgFunctionDeclarations that the user doesn't care about:
      iend_function_call_decls =
          remove_if(ibegin_function_call_decls,
                    iend_function_call_decls,
                    CompassAnalyses::DeadFunction::IsNodeNotInUserLocation);

      // Add the SgFunctionDeclarations to the map collection.
      // Map `unparsed SgFunctionDeclaration string`
      //  to `SgFunctionDeclaration`
      for ( ;
            ibegin_function_call_decls != iend_function_call_decls;
            ++ibegin_function_call_decls)
      {
          SgFunctionCallExp* function_call_exp =
              isSgFunctionCallExp(*ibegin_function_call_decls);
          ROSE_ASSERT(function_call_exp != NULL);

          SgFunctionDeclaration* function_decl =
              function_call_exp->getAssociatedFunctionDeclaration();
// TODO: SQlite3 has xCallBack callback function with no associated decl
          ROSE_ASSERT(function_decl != NULL);
//          if (function_decl != NULL)
//          {
              std::string function_decl_unparsed =
                  function_decl->unparseToString();
              ROSE_ASSERT(function_decl_unparsed.size() > 0);

              // Add to map
              function_call_map[function_decl_unparsed] =
                  function_decl;
          //}
// ^^^^^^
      }

      // (3)
      // Cross-check the function declarations against the function calls
      // 1. Iterate over our collection of SgFunctionDeclarations
      // 2. Check if there is a corresponding SgFunctionCallExp

      std::map<std::string, SgFunctionDeclaration*>::iterator map_it;
      typedef std::map<std::string, SgFunctionDeclaration*> map_type;

      // 1.
      BOOST_FOREACH(
          map_type::value_type& function_decl_map_pair,
          function_decl_map)
      {
          std::string function_decl_unparsed = function_decl_map_pair.first;
          SgFunctionDeclaration* function_decl = function_decl_map_pair.second;

          // 2.
          map_it = function_call_map.find(function_decl_unparsed);
          if (map_it == function_call_map.end())
          {
              output->addOutput(
                new CompassAnalyses::DeadFunction::CheckerOutput(
                    function_decl));
          }
      }
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const deadFunctionChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::DeadFunction::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::DeadFunction::short_description,
      CompassAnalyses::DeadFunction::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

