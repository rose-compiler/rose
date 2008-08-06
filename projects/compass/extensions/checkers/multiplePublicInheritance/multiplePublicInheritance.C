// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Multiple Public Inheritance Analysis
// Author: Gergo Barany
// Date: 24-July-2007

#include "compass.h"
#include "multiplePublicInheritance.h"

namespace CompassAnalyses
   { 
     namespace MultiplePublicInheritance
        { 
          const std::string checkerName      = "MultiplePublicInheritance";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "A class is defined with more than one public superclass.";
          const std::string longDescription  = "A class is defined with more than one public superclass.";
        } //End of namespace MultiplePublicInheritance.
   } //End of namespace CompassAnalyses.

CompassAnalyses::MultiplePublicInheritance::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::MultiplePublicInheritance::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
   }

void
CompassAnalyses::MultiplePublicInheritance::Traversal::
visit(SgNode* node)
   { 
     if (SgClassDefinition *cd = isSgClassDefinition(node))
     {
       SgBaseClassPtrList bases = cd->get_inheritances();
       SgBaseClassPtrList::iterator itr;
       size_t publics = 0;
       for (itr = bases.begin(); itr != bases.end(); ++itr)
       {
         SgAccessModifier &am = (*itr)->get_baseClassModifier().get_accessModifier();
         if (am.isPublic())
           publics++;
       }
       if (publics > 1)
         output->addOutput(new CheckerOutput(cd));
     }
   } //End of the visit function.
