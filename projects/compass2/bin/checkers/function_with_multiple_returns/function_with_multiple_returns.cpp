/**
 * \file    function_with_multiple_returns.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    August 16, 2012
 */

#include "rose.h"
#include "string_functions.h"

#include <boost/foreach.hpp>

#include "compass2/compass.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const functionWithMultipleReturnsChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_FUNCTION_WITH_MULTIPLE_RETURNS_H
#define COMPASS_FUNCTION_WITH_MULTIPLE_RETURNS_H

namespace CompassAnalyses
{
/**
 * \brief Detect functions that contain multiple return sites.
 */
namespace FunctionWithMultipleReturns
{
  extern const string checker_name;
  extern const string short_description;
  extern const string long_description;

  /**
   * \brief Specificaiton of checker results.
   */
  class CheckerOutput: public Compass::OutputViolationBase {
   public:
    explicit CheckerOutput(SgNode *const node);

   private:
    DISALLOW_COPY_AND_ASSIGN(CheckerOutput);
  };

  /**
   * \brief Specification of AST traversal.
   */
  class Traversal : public Compass::AstSimpleProcessingWithRunFunction {
   public:
    Traversal(Compass::Parameters inputParameters,
              Compass::OutputObject *output);

    /**
     * \brief Get inherited attribute for traversal.
     *
     * We are not using an inherited attribute for this checker.
     *
     * \returns NULL
     */
    void* initialInheritedAttribute() const
      {
        return NULL;
      }

    void run(SgNode *n)
      {
        this->traverse(n, preorder);
      }

    void visit(SgNode *n);

    /*-----------------------------------------------------------------------
     * Utilities
     *---------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------
     * Accessors / Mutators
     *---------------------------------------------------------------------*/
    string source_directory() const { return source_directory_; }
    void set_source_directory(const string &source_directory)
      {
        source_directory_ = source_directory;
      }

    Compass::OutputObject* output() const { return output_; }
    void set_output(Compass::OutputObject *const output)
      {
        output_ = output;
      }

   private:
    /*-----------------------------------------------------------------------
     * Attributes
     *---------------------------------------------------------------------*/
    string source_directory_; ///< Restrict analysis to user input files.
    Compass::OutputObject* output_;

    /*-----------------------------------------------------------------------
     * Utilities
     *---------------------------------------------------------------------*/
    DISALLOW_COPY_AND_ASSIGN(Traversal);
  };
} // ::CompassAnalyses
} // ::FunctionWithMultipleReturns
#endif // COMPASS_FUNCTION_WITH_MULTIPLE_RETURNS_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace FunctionWithMultipleReturns
 {
  const string checker_name      = "FunctionWithMultipleReturns";
  const string short_description = "Function with multiple returns detected.";
  const string long_description  = "This analysis looks for functions \
      that contain multiple return sites.";
 }
}

CompassAnalyses::FunctionWithMultipleReturns::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::functionWithMultipleReturnsChecker->checkerName,
                          ::functionWithMultipleReturnsChecker->shortDescription) {}

CompassAnalyses::FunctionWithMultipleReturns::Traversal::
Traversal(Compass::Parameters a_parameters, Compass::OutputObject* output)
    : output_(output)
  {
    try
    {
        string target_directory = a_parameters["general::target_directory"].front();
        source_directory_.assign(target_directory);
    }
    catch (Compass::ParameterNotFoundException e)
    {
        std::cout << "ParameterNotFoundException: " << e.what() << std::endl;
        homeDir(source_directory_);
    }
  }

//////////////////////////////////////////////////////////////////////////////

/** visit(SgNode *node)
 *
 *  Purpose
 *  ========
 *
 *  Search for functions that contain more than one
 *  return site.  For example:
 *
 *      int multiple_returns()
 *      {
 *          if (0)
 *            return 0;
 *          else
 *            return 1;
 *      }
 *
 *  Target AST
 *  ==========
 *              (1)
 *      SgFunctionDefinition
 *               |
 *             (body)
 *               |
 *          SgBasicBlock
 *      ```C++
 *        {
 *            if (0)
 *              return 0;
 *            else
 *              return 1;
 *        }
 *      ```
 *               |
 *              (2)
 *  (search for return statements)
 *           SgReturnStmt
 */
void
CompassAnalyses::FunctionWithMultipleReturns::Traversal::
visit(SgNode *node)
{
    SgLocatedNode* located_node = isSgLocatedNode(node);
    if (located_node != NULL &&
        Compass::IsNodeInUserLocation(located_node, source_directory_))
    {
        // (1)
        SgFunctionDefinition* function_definition =
            isSgFunctionDefinition(node);
        if (function_definition != NULL)
        {
            SgBasicBlock* function_body = function_definition->get_body();
            ROSE_ASSERT(function_body != NULL);

            // (2)
            Rose_STL_Container<SgNode*> return_statements =
                NodeQuery::querySubTree(function_body, V_SgReturnStmt);
            if (return_statements.size() > 1)
            {
                output_->addOutput(
                  new CheckerOutput(node));
            }
        }
    }
}// end of the visit function.

// Checker main run function and metadata

static void
run(Compass::Parameters params, Compass::OutputObject* output)
  {
    CompassAnalyses::FunctionWithMultipleReturns::Traversal(params, output).run(
      Compass::projectPrerequisite.getProject());
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return new CompassAnalyses::FunctionWithMultipleReturns::Traversal(params, output);
  }

extern const Compass::Checker* const functionWithMultipleReturnsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::FunctionWithMultipleReturns::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::FunctionWithMultipleReturns::short_description,
      CompassAnalyses::FunctionWithMultipleReturns::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

