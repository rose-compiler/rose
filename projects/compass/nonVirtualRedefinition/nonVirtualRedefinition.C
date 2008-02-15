// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Non Virtual Redefinition Analysis
// Author: Gergo Barany
// Date: 31-July-2007

#include "compass.h"
#include "nonVirtualRedefinition.h"
#include <cstring>

namespace CompassAnalyses
   { 
     namespace NonVirtualRedefinition
        { 
          const std::string checkerName      = "NonVirtualRedefinition";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Redefinition of an inherited nonvirtual function";
          const std::string longDescription  = "A nonvirtual member function from a "
            "base class is redefined. Calls to this member function will be "
            "resolved statically, not dynamically.";
        } //End of namespace NonVirtualRedefinition.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NonVirtualRedefinition::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NonVirtualRedefinition::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NonVirtualRedefinition.YourParameter"]);


   }

void
CompassAnalyses::NonVirtualRedefinition::Traversal::
visit(SgNode* node)
   { 
     if (SgMemberFunctionDeclaration *mfd = isSgMemberFunctionDeclaration(node))
     {
       SgClassDefinition *classdef = mfd->get_class_scope();
       ROSE_ASSERT(classdef != NULL);
       checkBaseClasses(mfd, classdef);
     }
   } //End of the visit function.

bool
CompassAnalyses::NonVirtualRedefinition::Traversal::
isOverridingType(SgFunctionType *a, SgFunctionType *b)
{
  if (a->get_orig_return_type() != b->get_orig_return_type())
  {
    return false;
  }
  else
  {
    SgTypePtrList &aparams = a->get_arguments();
    SgTypePtrList &bparams = b->get_arguments();
    SgTypePtrList::iterator ap = aparams.begin(), bp = bparams.begin();
    while (ap != aparams.end() && bp != bparams.end())
    {
      // The arguments of a may have const or volatile modifiers, and we want
      // to compare underlying types behind typedefs. But for some reason
      // stripType() doesn't seem to work... So this will fail to report some
      // cases that should be reported.
      if ((*ap)->stripTypedefsAndModifiers() != (*bp)->stripType(SgType::STRIP_TYPEDEF_TYPE))
        return false;
      ++ap;
      ++bp;
    }
    if (ap != aparams.end() || bp != bparams.end())
      return false;
  }

  return true;
}

void
CompassAnalyses::NonVirtualRedefinition::Traversal::
checkBaseClasses(SgMemberFunctionDeclaration *mfd, SgClassDefinition *classdef)
{
  // do not look at defining declarations lexically outside of classes
  // (because we already warn about the declaration inside the class)
  if (mfd->get_definingDeclaration() == mfd && !isSgClassDefinition(mfd->get_parent()))
    return;

  SgBaseClassPtrList &bases = classdef->get_inheritances();
  SgBaseClassPtrList::iterator b_itr;
  for (b_itr = bases.begin(); b_itr != bases.end(); ++b_itr)
  {
    SgClassDefinition *base = (*b_itr)->get_base_class()->get_definition();
    if (base == NULL)
      continue;
    ROSE_ASSERT(base != NULL);
    SgDeclarationStatementPtrList &members = base->get_members();
    SgDeclarationStatementPtrList::iterator m_itr;
    for (m_itr = members.begin(); m_itr != members.end(); ++m_itr)
    {
      if (SgMemberFunctionDeclaration *base_mfd = isSgMemberFunctionDeclaration(*m_itr))
      {
        /* If the base member function is overridden by this one (which
         * we try to determine by names and types), and the base
         * member function is not virtual, report a violation. */
        if (std::strcmp(mfd->get_name().str(), base_mfd->get_name().str()) == 0
         && isOverridingType(mfd->get_type(), base_mfd->get_type())
         && !base_mfd->get_functionModifier().isVirtual()
         && !base_mfd->get_functionModifier().isPureVirtual())
        {
          output->addOutput(new CheckerOutput(mfd));
        }
      }
    }

    // check base classes recursively
    checkBaseClasses(mfd, base);
  }
}
