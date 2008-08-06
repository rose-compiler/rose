// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Size Of Pointer Analysis
// Author: pants,,,
// Date: 24-August-2007

#include "compass.h"
#include "sizeOfPointer.h"

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
   : Compass::TraversalBase(output, checkerName, shortDescription, longDescription)
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

