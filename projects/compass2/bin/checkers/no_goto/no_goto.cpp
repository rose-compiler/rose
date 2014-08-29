/**
 * \file    no_goto.cpp
 * \author  Sam Kelly <kelly64@llnl.gov> <kellys@dickinson.edu>
 * \date    July 2, 2013
 */

#include "rose.h"
#include "compass2/compass.h"
#include <boost/foreach.hpp>
#include "AstMatching.h"


using std::string;
using namespace StringUtility;

extern const Compass::Checker* const noGotoChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_NO_GOTO_H
#define COMPASS_NO_GOTO_H

namespace CompassAnalyses
{
/**
 * \brief Finds Goto Statements
 */
namespace NoGoto
{
extern const string checker_name;
extern const string short_description;
extern const string long_description;
extern       string source_directory;

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
        NoGoto::source_directory);
  }
  else
  {
    return true;
  }
};

} // ::CompassAnalyses
} // ::NoGoto
#endif // COMPASS_NO_GOTO_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
namespace NoGoto
{
const string checker_name      = "NoGoto";
const string short_description = "GOTO Statement Found";
const string long_description  = "Finds Goto Statements";
string source_directory = "/";
}
}

CompassAnalyses::NoGoto::
CheckerOutput::CheckerOutput(SgNode *const node)
: OutputViolationBase(node,
                      ::noGotoChecker->checkerName,
                       ::noGotoChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
{
  // We only care about source code in the user's space, not,
  // for example, Boost or system files.
  string target_directory =
      parameters["general::target_directory"].front();
  CompassAnalyses::NoGoto::source_directory.assign(target_directory);

  // Use the pre-built ROSE AST
  SgProject* sageProject = Compass::projectPrerequisite.getProject();
  SgNode* root_node = (SgNode*) sageProject;



  // search for SgGotoStatements
  AstMatching matcher;
  MatchResult matches = matcher
      .performMatching("$r = SgGotoStatement", root_node);

  BOOST_FOREACH(SingleMatchVarBindings match, matches)
  {
    SgGotoStatement *goto_statement = (SgGotoStatement *)match["$r"];
    output->addOutput(
        new CompassAnalyses::NoGoto::
        CheckerOutput(goto_statement));
  }

}

extern const Compass::Checker* const noGotoChecker =
    new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoGoto::checker_name,
        // Descriptions should not include the newline character "\n".
        CompassAnalyses::NoGoto::short_description,
        CompassAnalyses::NoGoto::long_description,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        NULL);

