/**
 * \file    non_static_array_size.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    August 5, 2012
 */

#include "rose.h"
#include "string_functions.h"

#include <boost/foreach.hpp>

#include "compass2/compass.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const nonStaticArraySizeChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_NON_STATIC_ARRAY_SIZE_H
#define COMPASS_NON_STATIC_ARRAY_SIZE_H

namespace CompassAnalyses
{
/**
 * \brief Detect non-static array sizes.
 */
namespace NonStaticArraySize
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
} // ::NonStaticArraySize
#endif // COMPASS_NON_STATIC_ARRAY_SIZE_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace NonStaticArraySize
 {
  const string checker_name      = "NonStaticArraySize";
  const string short_description = "Non-static array size detected.";
  const string long_description  = "This analysis looks for array \
      declarations with non-static array sizes";
 }
}

CompassAnalyses::NonStaticArraySize::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::nonStaticArraySizeChecker->checkerName,
                          ::nonStaticArraySizeChecker->shortDescription) {}

CompassAnalyses::NonStaticArraySize::Traversal::
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

// TODO: check dynamic arrays?

// TODO: not possible to detect. According to Tristan Vanderbruggen:
// * EDG performs constant propagation; no original expression tree saved
// * Clang may be able to handle this

void
CompassAnalyses::NonStaticArraySize::Traversal::
visit(SgNode *node)
{
    SgLocatedNode* located_node = isSgLocatedNode(node);
    if (located_node != NULL &&
        Compass::IsNodeInUserLocation(located_node, source_directory_))
    {
        // Variable declarations
        SgInitializedName* initialized_name = isSgInitializedName(located_node);
        if (initialized_name != NULL)
        {
            // Arrays
            SgArrayType* array_type = isSgArrayType(initialized_name->get_type());
            if (array_type != NULL)
            {
                // Dimension expressions
                std::vector<SgExpression*> expressions =
                    SageInterface::get_C_array_dimensions(*array_type);
std::cout << "Expressions = " << expressions.size() << std::endl;
                BOOST_FOREACH(SgExpression* expr, expressions)
                {
                    if (! (expr->variantT() == V_SgNullExpression))
                    {
std::cout << "!!!!" << expr->get_originalExpressionTree() << std::endl;
                        // Variable reference expression
                        // TODO: check if constant; well, it should be...
                        Rose_STL_Container<SgNode*> x = NodeQuery::querySubTree(expr, V_SgVarRefExp);
                        std::cout << "Nodes=" << x.size() << std::endl;
                        for (Rose_STL_Container<SgNode*>::iterator iter2 = x.begin(); iter2 != x.end(); iter2++)
                        {
                            std::cout << "== SgVarRef!" << std::endl;
                        }
                        std::cout << expr->sage_class_name() << std::endl;
                    }
                }
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
    CompassAnalyses::NonStaticArraySize::Traversal(params, output).run(
      Compass::projectPrerequisite.getProject());
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return new CompassAnalyses::NonStaticArraySize::Traversal(params, output);
  }

extern const Compass::Checker* const nonStaticArraySizeChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::NonStaticArraySize::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::NonStaticArraySize::short_description,
      CompassAnalyses::NonStaticArraySize::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

