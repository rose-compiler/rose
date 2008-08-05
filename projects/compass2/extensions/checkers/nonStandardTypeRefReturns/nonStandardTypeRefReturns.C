// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Non Standard Type Ref Returns Analysis
// Author: pants,,,
// Date: 30-July-2007

#include "compass.h"
#include "nonStandardTypeRefReturns.h"

namespace CompassAnalyses
   { 
     namespace NonStandardTypeRefReturns
        { 
          const std::string checkerName      = "NonStandardTypeRefReturns";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding non-standard return types that are not references.";
          const std::string longDescription  = "Finds types that are structs or classes returned by functions that are not returned as a reference.";
        } //End of namespace NonStandardTypeRefReturns.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NonStandardTypeRefReturns::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NonStandardTypeRefReturns::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NonStandardTypeRefReturns.YourParameter"]);


   }

void
CompassAnalyses::NonStandardTypeRefReturns::Traversal::
visit(SgNode* node)
   {        
     std::string typeName, thingName;
     SgFunctionDeclaration *fn = isSgFunctionDeclaration(node);
     if (!fn) return;
     if ( fn->get_file_info()->isCompilerGenerated() == false)
       {
         SgInitializedNamePtrList args = fn->get_args();
         for (SgInitializedNamePtrList::iterator i = args.begin(); i != args.end(); i ++)
           {
             SgInitializedName* initName = isSgInitializedName (*i);
             if(!initName) {return;}
             SgType* th = initName->get_type();
             SgType* t = th->stripType();
             typeName = t->unparseToString();
             if (typeName.rfind("struct",0) != std::string::npos or 
                 typeName.rfind("class",0) != std::string::npos)
               {
                 SgReferenceType* sgRef = isSgReferenceType(th);
                 if (!sgRef)
                   {
                     output->addOutput(new CheckerOutput(node));
                   }
               }
           }
       }
   } //End of the visit function.

