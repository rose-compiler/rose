/**
 * \file   data_member_acces.cpp
 * \author Mike Roup <roup1@llnl.gov>
 * \date   July 15, 2013
 */

#include <boost/foreach.hpp>

#include "rose.h"
#include "compass2/compass.h"
#include "AstMatching.h"

using std::string;
using namespace StringUtility;

extern const Compass::Checker* const dataMemberAccessChecker;

/*-----------------------------------------------------------------------------
 * Interface
 *---------------------------------------------------------------------------*/

#ifndef COMPASS_DATA_MEMBER_ACCESS_H
#define COMPASS_DATA_MEMBER_ACCESS_H

namespace CompassAnalyses
{
/**
 * \brief Description of checker
 */
namespace DataMemberAccess
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
                      DataMemberAccess::source_directory);
      }
      else
      {
          return true;
      }
  };

} // ::CompassAnalyses
} // ::DataMemberAccess
#endif // COMPASS_DATA_MEMBER_ACCESS_H

/*-----------------------------------------------------------------------------
 * Implementation
 *---------------------------------------------------------------------------*/

namespace CompassAnalyses
{
 namespace DataMemberAccess
 {
  const string checker_name      = "DataMemberAccess";
  const string short_description = "Class has public and private/protected data members";
  const string long_description  = "Checks for classes that have both public as well as private or protected data members";
  string source_directory = "/";
 }
}

CompassAnalyses::DataMemberAccess::
CheckerOutput::CheckerOutput(SgNode *const node)
    : OutputViolationBase(node,
                          ::dataMemberAccessChecker->checkerName,
                          ::dataMemberAccessChecker->shortDescription) {}

static void
run(Compass::Parameters parameters, Compass::OutputObject* output)
  {
      // We only care about source code in the user's space, not,
      // for example, Boost or system files.
      string target_directory =
          parameters["general::target_directory"].front();
      CompassAnalyses::DataMemberAccess::source_directory.assign(target_directory);
      
      // Use the pre-built ROSE AST
      SgProject* sageProject = Compass::projectPrerequisite.getProject();
      
      // perform AST matching here
      AstMatching match_classes;
      MatchResult result_classes = match_classes.performMatching("$r = SgClassDefinition", sageProject);
      BOOST_FOREACH(SingleMatchVarBindings match, result_classes)
	{
	  SgClassDefinition* classdef = (SgClassDefinition*)match["$r"];
	  int pub, prot, priv;
	  pub = prot = priv = 0;
	  SgDeclarationStatementPtrList members = classdef->get_members();
	  SgDeclarationStatementPtrList::iterator member;
	  for (member = members.begin(); member != members.end(); ++member)
	    {
	      SgVariableDeclaration* vardecl = isSgVariableDeclaration(*member);
	      if (vardecl != NULL)
		{
		  SgAccessModifier &mod = vardecl->get_declarationModifier().get_accessModifier();
		  if (mod.isPublic())
		    ++pub;
		  else if (mod.isProtected())
		    ++prot;
		  else if (mod.isPrivate())
		    ++priv;
		}
	    }
	  if (pub != 0 && prot + priv != 0)
	    {
	      output->addOutput(new CompassAnalyses::DataMemberAccess::CheckerOutput(classdef));
	    }
	}
  }

// Remove this function if your checker is not an AST traversal
static Compass::AstSimpleProcessingWithRunFunction*
createTraversal(Compass::Parameters params, Compass::OutputObject* output)
  {
    return NULL;
  }

extern const Compass::Checker* const dataMemberAccessChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
      CompassAnalyses::DataMemberAccess::checker_name,
    // Descriptions should not include the newline character "\n".
      CompassAnalyses::DataMemberAccess::short_description,
      CompassAnalyses::DataMemberAccess::long_description,
      Compass::C | Compass::Cpp,
      Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
      run,
      createTraversal);

