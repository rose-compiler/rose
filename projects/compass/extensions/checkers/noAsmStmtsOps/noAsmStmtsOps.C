// No Asm Stmts Ops
// Author: Gary Yuan
// Date: 19-December-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_NO_ASM_STMTS_OPS_H
#define COMPASS_NO_ASM_STMTS_OPS_H

namespace CompassAnalyses
   { 
     namespace NoAsmStmtsOps
        { 
        /*! \brief No Asm Stmts Ops: Add your description here 
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

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n){ this->traverse(n, preorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_NO_ASM_STMTS_OPS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// No Asm Stmts Ops Analysis
// Author: Gary Yuan
// Date: 19-December-2007

#include "compass.h"
// #include "noAsmStmtsOps.h"

namespace CompassAnalyses
   { 
     namespace NoAsmStmtsOps
        { 
          const std::string checkerName      = "NoAsmStmtsOps";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that no C assembly statements or ops occur.";
          const std::string longDescription  = "This checker checks that no C assembly statements or ops occur.";
        } //End of namespace NoAsmStmtsOps.
   } //End of namespace CompassAnalyses.

CompassAnalyses::NoAsmStmtsOps::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::NoAsmStmtsOps::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["NoAsmStmtsOps.YourParameter"]);


   }

void
CompassAnalyses::NoAsmStmtsOps::Traversal::
visit(SgNode* node)
   {
     SgAsmStmt *stmt = isSgAsmStmt(node);
     SgAsmOp *op = isSgAsmOp(node);

     if( stmt || op )
       output->addOutput( new CheckerOutput(node) );

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::NoAsmStmtsOps::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::NoAsmStmtsOps::Traversal(params, output);
}

extern const Compass::Checker* const noAsmStmtsOpsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::NoAsmStmtsOps::checkerName,
        CompassAnalyses::NoAsmStmtsOps::shortDescription,
        CompassAnalyses::NoAsmStmtsOps::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
