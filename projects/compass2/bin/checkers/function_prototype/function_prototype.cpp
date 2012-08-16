/**
 * \file    function_has_prototype.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    August 15, 2012
 */

#include "rose.h"
#include "string_functions.h"

#include <boost/foreach.hpp>

#include "compass2/compass.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const functionPrototypeChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_FUNCTION_PROTOTYPE_H
#define COMPASS_FUNCTION_PROTOTYPE_H

namespace CompassAnalyses
{
/**
 * \brief Detect functions that have or don't have a
 *        prototype sizes (configurable).
 */
namespace FunctionPrototype
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
  const string long_description  = "This analysis looks for functions \
      that don't have a prototype (forward + defining declarations)";
 }
}

CompassAnalyses::FunctionPrototype::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::functionPrototypeChecker->checkerName,
                          ::functionPrototypeChecker->shortDescription) {}

CompassAnalyses::FunctionPrototype::Traversal::
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
 *  Search for functions that don't have a prototype,
 *  i.e. forward function definition.  For example:
 *
 *      void function_def_and_decl()
 *      {
 *      }
 *
 *  Target AST
 *  ==========
 *              (1)
 *      SgFunctionDeclaration
 *               |
 *           search for
 *  first non-defining declaration
 *               |
 *         ------------
 *        |            |
 *       YES           NO
 *        |            |
 *    prototype  (2) <target: does not have prototype>
 */
void
CompassAnalyses::FunctionPrototype::Traversal::
visit(SgNode *node)
{
    SgLocatedNode* located_node = isSgLocatedNode(node);
    if (located_node != NULL &&
        Compass::IsNodeInUserLocation(located_node, source_directory_))
    {
        // (1)
        SgFunctionDeclaration* function_decl =
            isSgFunctionDeclaration(node);
        if (function_decl != NULL)
        {
            // (2)
            SgDeclarationStatement* first_defining_decl =
                function_decl->get_firstNondefiningDeclaration();
            // No forward declaration (i.e. function prototype)
            if (first_defining_decl == NULL)
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
    CompassAnalyses::FunctionPrototype::Traversal(params, output).run(
      Compass::projectPrerequisite.getProject());
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return new CompassAnalyses::FunctionPrototype::Traversal(params, output);
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

