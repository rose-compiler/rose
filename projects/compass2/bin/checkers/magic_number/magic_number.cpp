/**
 * \file   magic_number.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 8, 2013
 */

#include <iostream>

#include <boost/foreach.hpp>
#include <boost/regex.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const magicNumberChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_MAGIC_NUMBER_H
#define COMPASS_MAGIC_NUMBER_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace MagicNumber
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
                      MagicNumber::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::MagicNumber
#endif // COMPASS_MAGIC_NUMBER_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace MagicNumber
 {
  const string checker_name      = "MagicNumber";
  const string short_description = "Magic number found";
  const string long_description  = "Looks for floating point or integer constants that are not in init expressions.";
  string source_directory = "/";
 }
}

CompassAnalyses::MagicNumber::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::magicNumberChecker->checkerName,
                          ::magicNumberChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
    typedef std::map<string, string> ExpectedValuesMap;
    ExpectedValuesMap magic_;
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::MagicNumber::source_directory.assign(target_directory);

      Compass::ParametersMap things = parameters[boost::regex("^magicNumbers::.*$")];
      BOOST_FOREACH(const Compass::ParametersMap::value_type& pair, things)
	{
	  Compass::ParameterValues values = pair.second;
	  BOOST_FOREACH(string keyword, values)
	    {
	      magic_[keyword] = keyword;
	    }
	}

      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_val_exps;
      MatchResult val_exps_result = match_val_exps.performMatching("$r = SgIntVal | $r = SgDoubleVal", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, val_exps_result)
	{
	  SgValueExp* val = (SgValueExp*)match["$r"];
	  if (val->get_originalExpressionTree() == NULL)
	    {
	      SgNode* p = val->get_parent();
	      while (isSgExpression(p) && !isSgInitializer(p))
		{
		  p = p->get_parent();
		}
	      if (!isSgInitializer(p) || isSgConstructorInitializer(p))
		{
		  string number = val->get_constant_folded_value_as_string();
		  if (magic_[number] == "")
		    {
		      output->addOutput(new CompassAnalyses::MagicNumber::CheckerOutput(val));
		    }
		}
	    }
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const magicNumberChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::MagicNumber::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::MagicNumber::short_description,
      CompassAnalyses::MagicNumber::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

