/**
 * \file    default_argument.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    August 15, 2012
 */

#include "rose.h"
#include "string_functions.h"

#include <boost/foreach.hpp>

#include "compass2/compass.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const defaultArgumentChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_DEFAULT_ARGUMENT_H
#define COMPASS_DEFAULT_ARGUMENT_H

namespace CompassAnalyses
{
/**
 * \brief Detect default arguments in function parameter lists.
 */
namespace DefaultArgument
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
} // ::DefaultArgument
#endif // COMPASS_DEFAULT_ARGUMENT_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace DefaultArgument
 {
  const string checker_name      = "DefaultArgument";
  const string short_description = "Default function argument detected.";
  const string long_description  = "This analysis looks for functions \
      with default arguments in their parameter list.";
 }
}

CompassAnalyses::DefaultArgument::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::defaultArgumentChecker->checkerName,
                          ::defaultArgumentChecker->shortDescription) {}

CompassAnalyses::DefaultArgument::Traversal::
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
 *  Search for function parameters that are assigned
 *  default values.  For example:
 *
 *      // Here, `x` is given a default value of `7`
 *      void foobar(int y, int x=7);
 *
 *  Target AST
 *  ==========
 *                (1)                           (2)
 *      SgFunctionParameterList ------> `(int y, int x=7)`
 *                                               '___'
 *                                                 |
 *                                      ___________|___________
 *                                     |           |           |
 * (3) Variable is being assigned to:  |  SgInitializedName    |
 *                                     |           |           |
 *                                     |  SgAssignInitializer  |
 *                                     |_______________________|
 */
void
CompassAnalyses::DefaultArgument::Traversal::
visit(SgNode *node)
{
    SgLocatedNode* located_node = isSgLocatedNode(node);
    if (located_node != NULL &&
        Compass::IsNodeInUserLocation(located_node, source_directory_))
    {
        // (1)
        SgFunctionParameterList* parameters = isSgFunctionParameterList(node);
        if (parameters != NULL)
        {
            // (2)
            SgInitializedNamePtrList args = parameters->get_args();
            BOOST_FOREACH(SgInitializedName* name, args)
            {
                // (3)
                Rose_STL_Container<SgNode*> default_assignment =
                    NodeQuery::querySubTree(name, V_SgAssignInitializer);
                if (default_assignment.size() > 0)
                {
                    output_->addOutput(
                      new CheckerOutput(node));
                }
            }
        }
    }
}// end of the visit function.

// Checker main run function and metadata

static void
run(Compass::Parameters params, Compass::OutputObject* output)
  {
    CompassAnalyses::DefaultArgument::Traversal(params, output).run(
      Compass::projectPrerequisite.getProject());
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return new CompassAnalyses::DefaultArgument::Traversal(params, output);
  }

extern const Compass::Checker* const defaultArgumentChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::DefaultArgument::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::DefaultArgument::short_description,
      CompassAnalyses::DefaultArgument::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

