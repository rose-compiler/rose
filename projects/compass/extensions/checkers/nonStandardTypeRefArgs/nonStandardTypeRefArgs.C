// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Non Standard Type Ref Args Analysis
// Author: pants,,,
// Date: 30-July-2007

#include "compass.h"
#include "nonStandardTypeRefArgs.h"

namespace CompassAnalyses
   { 
     namespace NonStandardTypeRefArgs
        { 
          const std::string checkerName      = "NonStandardTypeRefArgs";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding function with nonstandard type argument" 
                                               "that does not take a reference.";
          const std::string longDescription  = "Here we find functions that accept arguments of non-standard types that are not sent as references.  Any struct or class type will be flagged if passed.  If you require an unmutable copy please send const reference.";
        } //End of namespace NonStandardTypeRefArgs.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NonStandardTypeRefArgs::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NonStandardTypeRefArgs::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NonStandardTypeRefArgs.YourParameter"]);
   }

void
CompassAnalyses::NonStandardTypeRefArgs::Traversal::
visit(SgNode* node)
   {      
     std::string typeName, thingName;
     SgFunctionDeclaration *fn = isSgFunctionDeclaration(node);
     if (!fn) return;
     SgInitializedNamePtrList args = fn->get_args();
     if ( fn->get_file_info()->isCompilerGenerated() == false)
       {
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
                     return;
                   }
               }
           }
       }
   } //End of the visit function.
   
