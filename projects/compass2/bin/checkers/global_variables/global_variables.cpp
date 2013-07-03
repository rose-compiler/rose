/**
 * \file
 * \author
 * \date
 */

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
 * \brief Description of checker
 */
namespace GlobalVariables
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
                      GlobalVariables::source_directory);
      }
      else
      {
          return true;
      }
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

CompassAnalyses::GlobalVariables::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::globalVariablesChecker->checkerName,
                          ::globalVariablesChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::GlobalVariables::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      SgNode* root_node = (SgNode*) sageProject;
      
      // perform AST matching here
      
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
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

