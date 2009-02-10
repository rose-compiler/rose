// Size Of Pointer
// Author: pants,,,
// Date: 24-August-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_SIZE_OF_POINTER_H
#define COMPASS_SIZE_OF_POINTER_H

namespace CompassAnalyses
   { 
     namespace SizeOfPointer
        { 
        /*! \brief Size Of Pointer: Add your description here 
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

// COMPASS_SIZE_OF_POINTER_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Size Of Pointer Analysis
// Author: pants,,,
// Date: 24-August-2007

#include "compass.h"
// #include "sizeOfPointer.h"

namespace CompassAnalyses
   { 
     namespace SizeOfPointer
        { 
          const std::string checkerName      = "SizeOfPointer";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "Finding sizeof of a pointer";
          const std::string longDescription  = "Checks args to sizeof to make sure they are dereferenced to a proper type.";
        } //End of namespace SizeOfPointer.
   } //End of namespace CompassAnalyses.

CompassAnalyses::SizeOfPointer::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::SizeOfPointer::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["SizeOfPointer.YourParameter"]);


   }


int countStars(std::string varName)
{
  int total = 0;
  for (int i = 0; i < varName.length(); i++)
    {
      if (varName[i] == '*')
        {
          total++;
        }
    }
  return total;
}

void
CompassAnalyses::SizeOfPointer::Traversal::
visit(SgNode* node)
   { 
     int starCount;
     SgSizeOfOp *szOf = isSgSizeOfOp(node);
     if(!szOf) return;
     Rose_STL_Container<SgNode*> pointers = NodeQuery::querySubTree(node,V_SgPointerType);
     Rose_STL_Container<SgNode*> deRefs = NodeQuery::querySubTree(node,V_SgPointerDerefExp);
     Rose_STL_Container<SgNode*> varRefs = NodeQuery::querySubTree(node,V_SgVarRefExp);
     for (Rose_STL_Container<SgNode *>::iterator i = varRefs.begin(); i != varRefs.end(); i++)
       {
         SgVarRefExp *vRef = isSgVarRefExp((*i));
         if (!vRef) return;
         SgType *t = vRef->get_type();
         std::string typeName = t->unparseToString();
         //std::cout << countStars(typeName) <<   std::endl;
         starCount = countStars(typeName);
         if (!starCount or
             (starCount == pointers.size() and 
              deRefs.size() == (starCount - 1)))
           {
             //std::cout << "IT'S OK!" << std::endl;
             return;
           }
         
         
       }
     output->addOutput(new CheckerOutput(node));
} //End of the visit function.


static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::SizeOfPointer::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::SizeOfPointer::Traversal(params, output);
}

extern const Compass::Checker* const sizeOfPointerChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::SizeOfPointer::checkerName,
        CompassAnalyses::SizeOfPointer::shortDescription,
        CompassAnalyses::SizeOfPointer::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
