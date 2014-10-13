/**
 * \file    non_global_cpp_directive.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    June 25, 2012
 */

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>

#include "rose.h"
#include "compass2/compass.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const nonGlobalCppDirectiveChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_NON_GLOBAL_CPP_DIRECTIVE_H
#define COMPASS_NON_GLOBAL_CPP_DIRECTIVE_H

namespace CompassAnalyses
{
/**
 * \brief Detect non-global CPP directives.
 */
namespace NonGlobalCppDirective
{
  extern const string checker_name;
  extern const string short_description;
  extern const string long_description;

  /**
   * \brief Specification of checker results.
   */
  class CheckerOutput: public Compass::OutputViolationBase {
   public:
    CheckerOutput(SgNode *const owning_node,
                  const PreprocessingInfo::RelativePositionType& position,
                  const string& directive);

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
} // ::NonGlobalCppDirective
#endif // COMPASS_NON_GLOBAL_CPP_DIRECTIVE_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/


std::string toString(const PreprocessingInfo::RelativePositionType& position)
{
    std::string position_string("undefined");
    switch (position)
    {
      case PreprocessingInfo::before:
      {
          position_string.assign("before");
          break;
      }
      case PreprocessingInfo::after:
      {
          position_string.assign("after");
          break;
      }
      case PreprocessingInfo::inside:
      {
          position_string.assign("inside");
          break;
      }
    }
    return position_string;
}

namespace CompassAnalyses
{
namespace NonGlobalCppDirective
{
  const string checker_name      = "NonGlobalCppDirective";
  const string short_description = "Non-global CPP directive detected ";
  const string long_description  = "This analysis looks for non-global CPP directives";
} // ::CompassAnalyses
} // ::NonGlobalCppDirective

CompassAnalyses::NonGlobalCppDirective::
CheckerOutput::CheckerOutput(SgNode *owning_node,
                             const PreprocessingInfo::RelativePositionType& position,
                             const string& directive)
    : OutputViolationBase(
        owning_node,
        ::nonGlobalCppDirectiveChecker->checkerName,
        ::nonGlobalCppDirectiveChecker->shortDescription +
          toString(position) + " " +
          owning_node->class_name() + ": " + directive + ".")
{}


CompassAnalyses::NonGlobalCppDirective::Traversal::
Traversal(Compass::Parameters a_parameters, Compass::OutputObject *output)
    : output_(output)
{
    try
    {
        string target_directory =
            a_parameters["general::target_directory"].front();
        source_directory_.assign(target_directory);
    }
    catch (Compass::ParameterNotFoundException e)
    {
        std::cout << "ParameterNotFoundException: " << e.what() << std::endl;
        homeDir(source_directory_);
    }
}

//////////////////////////////////////////////////////////////////////////////

void
CompassAnalyses::NonGlobalCppDirective::Traversal::
visit(SgNode *node)
{
    SgLocatedNode* located_node = isSgLocatedNode(node);
    if (located_node != NULL &&
        Compass::IsNodeInUserLocation(located_node, source_directory_))
    {
        // CPP information is attached to a neighboring `SgLocatedNode`.
        // (It's not actually part of the AST.)
        AttachedPreprocessingInfoType* cpp = located_node->getAttachedPreprocessingInfo();
        if (cpp != NULL)
        {
            AttachedPreprocessingInfoType::iterator i;
            for ( i = cpp->begin(); i != cpp->end(); i++ )
            {
                // What type of CPP directive is this (#include, #define, etc.)?
                switch ((*i)->getTypeOfDirective())
                {
                    case PreprocessingInfo::CpreprocessorIncludeDeclaration:
                    case PreprocessingInfo::CpreprocessorDefineDeclaration:
                    {
                       // Where is this CPP information, relative to its
                       // neighboring (owning) node?
                        switch ((*i)->getRelativePosition())
                        {
                            // (1) Inside of something => not in the global scope!
                            case PreprocessingInfo::inside:
                            {
                                output_->addOutput(
                                  new CheckerOutput(
                                    node,
                                    (*i)->getRelativePosition(),
                                    (*i)->getString().c_str()));
                                break;
                            }// case PreprocessingInfo::inside

                            // (2) Before or After...
                            default:
                            {
                                // a `SgBasicBlock`...
                                if (isSgBasicBlock(located_node) != NULL)
                                {
                                    // that is attached to some parent node...
                                    SgNode* parent_node = located_node->get_parent();
                                    if (parent_node != NULL)
                                    {
                                        // ==Corner case==
                                        //
                                        //    void foo()
                                        //      #define non "global" // in-between
                                        //    {
                                        //       ...
                                        //    }
                                        if (! isSgFunctionDefinition(parent_node))
                                        {
                                            output_->addOutput(
                                              new CheckerOutput(
                                                node,
                                                (*i)->getRelativePosition(),
                                                (*i)->getString().c_str()));
                                        }
                                    }
                                }
                                break;
                            }
                        }// switch getRelativePosition
                        break;
                    }// case "#define"
                    default:
                    {
                        break;
                    }// case default
                }// switch type of cpp
            }// for all attached cpp
        }// if has cpp
    }// if target node
}// end of the visit function.

// Checker main run function and metadata

static void
run(Compass::Parameters params, Compass::OutputObject *output)
{
    CompassAnalyses::NonGlobalCppDirective::Traversal(params, output).run(
      Compass::projectPrerequisite.getProject());
}

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject *output)
{
    return new CompassAnalyses::NonGlobalCppDirective::Traversal(params, output);
}

extern const Compass::Checker* const nonGlobalCppDirectiveChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::NonGlobalCppDirective::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::NonGlobalCppDirective::short_description,
      CompassAnalyses::NonGlobalCppDirective::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

