/**
 * \file    keywordMacro.C
 * \author  Justin Too <too1@llnl.gov>
 * \date    September 9, 2011
 */

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>

#include "rose.h"
#include "compass2/compass.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const keywordMacroChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_KEYWORD_MACRO_H
#define COMPASS_KEYWORD_MACRO_H

namespace CompassAnalyses
{
/**
 * \brief Detect function pointers and function references.
 */
namespace KeywordMacro
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
                  const string& keyword);

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

    typedef std::map<std::string, std::string> ExpectedValuesMap;
    ExpectedValuesMap keywords_;

    /*-----------------------------------------------------------------------
     * Utilities
     *---------------------------------------------------------------------*/
    DISALLOW_COPY_AND_ASSIGN(Traversal);
  };
} // ::CompassAnalyses
} // ::KeywordMacro
#endif // COMPASS_KEYWORD_MACRO_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
namespace KeywordMacro
{
  const string checker_name      = "KeywordMacro";
  const string short_description = "Keyword macro detected ";
  const string long_description  = "This analysis looks for keyword macros";
} // ::CompassAnalyses
} // ::KeywordMacro

CompassAnalyses::KeywordMacro::
CheckerOutput::CheckerOutput(SgNode *owning_node,
                             const PreprocessingInfo::RelativePositionType& position,
                             const string& keyword)
   : OutputViolationBase(owning_node, ::keywordMacroChecker->checkerName,
                         ::keywordMacroChecker->shortDescription +
                         string(((position == PreprocessingInfo::before) ? "before " : "after ")) +
                         owning_node->class_name() + " that overrides " + keyword + ".")
   {}


CompassAnalyses::KeywordMacro::Traversal::
Traversal(Compass::Parameters a_parameters, Compass::OutputObject *output)
    : output_(output)
  {
    try
    {
        string target_directory =
            a_parameters["general::target_directory"].front();
        source_directory_.assign(target_directory);

        Compass::ParametersMap parameters =
            a_parameters[boost::regex("keywordMacros::.*")];
        // Save keywords in map for faster lookups.
        BOOST_FOREACH(const Compass::ParametersMap::value_type& pair, parameters)
        {
            Compass::ParameterValues values = pair.second;
            BOOST_FOREACH(std::string keyword, values)
            {
                keywords_[keyword] = keyword;
            }
        }
    }
    catch (Compass::ParameterNotFoundException e)
    {
        std::cout << "ParameterNotFoundException: " << e.what() << std::endl;
        homeDir(source_directory_);
    }
  }

//////////////////////////////////////////////////////////////////////////////

void
CompassAnalyses::KeywordMacro::Traversal::
visit(SgNode *node)
  {
    SgLocatedNode* located_node = isSgLocatedNode(node);
    if (located_node != NULL && Compass::IsNodeInUserLocation(located_node, source_directory_))
    {
        AttachedPreprocessingInfoType* cpp = located_node->getAttachedPreprocessingInfo();
        if ( cpp != NULL )
        {
            AttachedPreprocessingInfoType::iterator i;
            for ( i = cpp->begin(); i != cpp->end(); i++ )
            {
                if ((*i)->getTypeOfDirective() ==
                     PreprocessingInfo::CpreprocessorDefineDeclaration)
                {
                    std::string macro_name = boost::trim_copy((*i)->getString());
                    //std::cout << macro_name << std::endl;

                    size_t count = 0;
                    std::vector<std::string> tokens;
                    boost::split(tokens, macro_name, boost::is_any_of(" "));
                    BOOST_FOREACH(std::string macro_name, tokens)
                    {
                        // (0) #define (1) name (2) value
                        if (count == 1)
                        {
                            //std::cout << "Found: " << macro_name << std::endl;
                            ExpectedValuesMap::const_iterator f =
                                keywords_.find(macro_name);
                            if (f != keywords_.end() )
                            {
                                output_->addOutput(
                                    new CheckerOutput(located_node,
                                                      (*i)->getRelativePosition(),
                                                      macro_name));
                            }
                        }
                        ++count;
                    }
                    if (count != 3)
                    {
                        std::cout << "Found " << count << " parts to the macro!" << std::endl;
                        std::cout << "macro_name = " << macro_name << std::endl;
                        ROSE_ASSERT(count == 3);
                    }
                    //PreprocessingInfo::rose_macro_definition* macro_def = (*i)->get_macro_def();
                    //macro_def->macro_name;
                    //segfault <= boost::wave
                }
            }
        }
    }
  } //End of the visit function.

// Checker main run function and metadata

static void
run(Compass::Parameters params, Compass::OutputObject *output)
  {
    CompassAnalyses::KeywordMacro::Traversal(params, output).run(
      Compass::projectPrerequisite.getProject());
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject *output)
  {
    return new CompassAnalyses::KeywordMacro::Traversal(params, output);
  }

extern const Compass::Checker* const keywordMacroChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::KeywordMacro::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::KeywordMacro::short_description,
      CompassAnalyses::KeywordMacro::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

