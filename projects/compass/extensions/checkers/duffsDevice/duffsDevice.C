// Duff's Device
// Author: Gergo  Barany
// Date: 24-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_DUFFS_DEVICE_H
#define COMPASS_DUFFS_DEVICE_H

namespace CompassAnalyses
   { 
     namespace DuffsDevice
        { 
        /*! \brief Duff's Device: Add your description here 
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
                 Compass::OutputObject* output;
                 bool isLoopStatement(SgNode *n) const;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_DUFFS_DEVICE_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Duff's Device Analysis
// Author: Gergo Barany
// Date: 24-July-2007

#include "compass.h"
// #include "duffsDevice.h"

namespace CompassAnalyses
   { 
     namespace DuffsDevice
        { 
          const std::string checkerName      = "DuffsDevice";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Case label inside loop inside switch.";
          const std::string longDescription  = "The infamous Duff's Device, a"
            " construct consisting of a case label within a loop within the"
            " enclosing switch statement, is encountered.";
        } //End of namespace DuffsDevice.
   } //End of namespace CompassAnalyses.

CompassAnalyses::DuffsDevice::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::DuffsDevice::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
   }

void
CompassAnalyses::DuffsDevice::Traversal::
visit(SgNode* node)
   { 
     if (isSgCaseOptionStmt(node) || isSgDefaultOptionStmt(node))
     {
       SgNode *p = node->get_parent();
       while (p != NULL && !isSgSwitchStatement(p) && !isLoopStatement(p))
         p = p->get_parent();
       if (isLoopStatement(p))
         output->addOutput(new CheckerOutput(p)); // report the loop as Duff's Device
     }
   } //End of the visit function.
   
bool
CompassAnalyses::DuffsDevice::Traversal::
isLoopStatement(SgNode* n) const
{
  return isSgDoWhileStmt(n) || isSgForStatement(n) || isSgWhileStmt(n);
}

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::DuffsDevice::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::DuffsDevice::Traversal(params, output);
}

extern const Compass::Checker* const duffsDeviceChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::DuffsDevice::checkerName,
        CompassAnalyses::DuffsDevice::shortDescription,
        CompassAnalyses::DuffsDevice::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
