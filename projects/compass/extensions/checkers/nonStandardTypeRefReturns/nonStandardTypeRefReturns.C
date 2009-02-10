// Non Standard Type Ref Returns
// Author: pants,,,
// Date: 30-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NON_STANDARD_TYPE_REF_RETURNS_H
#define COMPASS_NON_STANDARD_TYPE_REF_RETURNS_H

namespace CompassAnalyses
   { 
     namespace NonStandardTypeRefReturns
        { 
        /*! \brief Non Standard Type Ref Returns: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_NON_STANDARD_TYPE_REF_RETURNS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Non Standard Type Ref Returns Analysis
// Author: pants,,,
// Date: 30-July-2007

#include "compass.h"
// #include "nonStandardTypeRefReturns.h"

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
   : output(output)
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


static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NonStandardTypeRefReturns::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NonStandardTypeRefReturns::Traversal(params, output);
}

extern const Compass::Checker* const nonStandardTypeRefReturnsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NonStandardTypeRefReturns::checkerName,
        CompassAnalyses::NonStandardTypeRefReturns::shortDescription,
        CompassAnalyses::NonStandardTypeRefReturns::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
