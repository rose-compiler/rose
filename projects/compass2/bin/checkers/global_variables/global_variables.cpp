/**
 * \file    global_variables.cpp
 * \author  Justin Too <too1@llnl.gov>
 * \date    August 3, 2012
 */

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>

#include "rose.h"
#include "compass2/compass.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const globalVariablesChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_GLOBAL_VARIABLES_H
#define COMPASS_GLOBAL_VARIABLES_H

namespace CompassAnalyses
{
/**
 * \brief Detect global variables.
 */
namespace GlobalVariables
{
  extern const string checker_name;
  extern const string short_description;
  extern const string long_description;

  /**
   * \brief Specification of checker results.
   */
  class CheckerOutput: public Compass::OutputViolationBase {
   public:
    CheckerOutput(SgNode *const node, std::string variable_name);

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

    typedef std::map<std::string, bool> WhitelistMap;
    WhitelistMap white_list_; ///< white list of permitted global variables

    /*-----------------------------------------------------------------------
     * Utilities
     *---------------------------------------------------------------------*/
    DISALLOW_COPY_AND_ASSIGN(Traversal);
  };
} // ::CompassAnalyses
} // ::GlobalVariables
#endif // COMPASS_GLOBAL_VARIABLES_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
namespace GlobalVariables
{
  const string checker_name      = "GlobalVariables";
  const string short_description = "Global variable detected ";
  const string long_description  = "This analysis looks for global variables";
} // ::CompassAnalyses
} // ::GlobalVariables

CompassAnalyses::GlobalVariables::
CheckerOutput::CheckerOutput(SgNode *const node, std::string variable_name)
    : OutputViolationBase(
        node,
        ::globalVariablesChecker->checkerName,
        ::globalVariablesChecker->shortDescription +
        ": " + variable_name)
{}


CompassAnalyses::GlobalVariables::Traversal::
Traversal(Compass::Parameters a_parameters, Compass::OutputObject *output)
    : output_(output)
{
    try
    {
        string target_directory =
            a_parameters["general::target_directory"].front();
        source_directory_.assign(target_directory);

        // Save list of permitted variables in map for faster lookups.
        Compass::ParametersMap parameters =
            a_parameters[boost::regex("globalVariables::.*")];
        BOOST_FOREACH(const Compass::ParametersMap::value_type& pair, parameters)
        {
            Compass::ParameterValues values = pair.second;
            BOOST_FOREACH(std::string variable_name, values)
            {
                white_list_[variable_name] = true; // true => permitted
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
CompassAnalyses::GlobalVariables::Traversal::
visit(SgNode *node)
{
    SgLocatedNode* located_node = isSgLocatedNode(node);
    if (located_node != NULL &&
        Compass::IsNodeInUserLocation(located_node, source_directory_))
    {
        SgInitializedName* initialized_name = isSgInitializedName(node);
        if (initialized_name != NULL)
        {
            SgScopeStatement* scope = initialized_name->get_scope();
            if (isSgGlobal(scope))
            {
                std::string variable_name =
                    initialized_name->get_name().getString();
                if (white_list_.find(variable_name) == white_list_.end())
                {
                    output_->addOutput(new CheckerOutput(node, variable_name));
                }
            }
        }
    }// if target node
}// end of the visit function.

// Checker main run function and metadata

static void
run(Compass::Parameters params, Compass::OutputObject *output)
{
    CompassAnalyses::GlobalVariables::Traversal(params, output).run(
      Compass::projectPrerequisite.getProject());
}

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject *output)
{
    return new CompassAnalyses::GlobalVariables::Traversal(params, output);
}

extern const Compass::Checker* const globalVariablesChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::GlobalVariables::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::GlobalVariables::short_description,
      CompassAnalyses::GlobalVariables::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

