// Computational Functions
// Author: Thomas Panas
// Date: 23-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_COMPUTATIONAL_FUNCTIONS_H
#define COMPASS_COMPUTATIONAL_FUNCTIONS_H

namespace CompassAnalyses
{ 
  namespace ComputationalFunctions
    { 
      /*! \brief Computational Functions: Add your description here 
       */

      extern const std::string checkerName;
      extern std::string shortDescription;
      extern std::string longDescription;

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
	  static int intOps;
	  static int floatOps;
	  static int intOps_actual;
	  static int floatOps_actual;

	public:
	  Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);
	  static std::string intToString(int n) {
	    std::ostringstream myStream; //creates an ostringstream object
	    myStream << n << std::flush;
	    return (myStream.str()); //returns the string form of the stringstream object
	  }
	  // The implementation of the run function has to match the traversal being called.
	  void run(SgNode* n){ this->traverse(n, preorder); };
	  static std::string getIntOps() { return intToString(intOps);} 
	  static std::string getIntOps_actual() { return intToString(intOps_actual);} 
	  static std::string getFloatOps() { return intToString(floatOps);} 
	  static std::string getFloatOps_actual() { return intToString(floatOps_actual);} 

	  void checkNode(SgNode* node);
	  int checkDepth(std::vector<SgNode*> children);
	  void visit(SgNode* n);
	};
    }
}

// COMPASS_COMPUTATIONAL_FUNCTIONS_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Computational Functions Analysis
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"
// #include "computationalFunctions.h"

namespace CompassAnalyses
{ 
  namespace ComputationalFunctions
  { 
    /*class Traversal {
      int intOps=0;
      int intOps_actual=0;
      int floatOps=0;
      int floatOps_actual=0;
    }*/
    //GMY 12/26/2007 changed namespace Traversal to explicit scope because of 
    //error in EDG front-end processing. 
    //These are static data member initializations of class Traversal
    int Traversal::intOps=0;
    int Traversal::intOps_actual=0;
    int Traversal::floatOps=0;
    int Traversal::floatOps_actual=0;

    //12/26/2007 END

    const std::string checkerName      = "ComputationalFunctions";

    // Descriptions should not include the newline character "\n".
    std::string shortDescription = 
    "This function has either too many integer or floating point operations : ";
    std::string longDescription  = 
    "This analysis checks for functions that use lots of integer or floating point operations.";
  } //End of namespace ComputationalFunctions.
} //End of namespace CompassAnalyses.

CompassAnalyses::ComputationalFunctions::
CheckerOutput::CheckerOutput ( SgNode* node )
  : OutputViolationBase(node,checkerName,shortDescription+
                        "int = " + Traversal::getIntOps_actual()+" > "+Traversal::getIntOps() +
                        "  float = " + Traversal::getFloatOps_actual()+" > "+Traversal::getFloatOps()
                        )
{}

CompassAnalyses::ComputationalFunctions::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ComputationalFunctions.YourParameter"]);

  intOps = Compass::parseInteger(inputParameters["computationalFunctions.maxIntOps"]);
  floatOps = Compass::parseInteger(inputParameters["computationalFunctions.maxFloatOps"]);
#if 0 // JJW (8/12/2008): We are not allowed to change these anymore
  setShortDescription(shortDescription +  
                      "int = " + Traversal::getIntOps_actual()+" > "+Traversal::getIntOps() +
                      "  float = " + Traversal::getFloatOps_actual()+" > "+Traversal::getFloatOps());
#endif

}

void CompassAnalyses::ComputationalFunctions::Traversal::
checkNode(SgNode* node) {
  std::string nameNode = node->class_name();
  /*
    if (isSgAddOp(node))
    std::cout << "**** isSgAddOp: " << " . " <<  std::endl;
    if(isSgSubtractOp(node))
    std::cout << "**** isSgSubtractOp: " << " . " <<  std::endl;
    if (isSgDivideOp(node))
    std::cout << "**** isSgDivideOp: " << " . " <<  std::endl;
    if(isSgMultiplyOp(node))
    std::cout << "**** isSgMultiplyOp: " << " . " <<  std::endl;
  */
  if (isSgAddOp(node) || isSgSubtractOp(node) || isSgDivideOp(node) || isSgMultiplyOp(node)) {
    std::vector<SgNode*> childrenRec = node->get_traversalSuccessorContainer();
    if (childrenRec.size()>0) {
      bool isIntPointOP=false;
      bool isFloatPointOP=false;
      bool isFloatPointPointerOP=false;
      bool isFloatPointArrayOP=false;
      bool isIntPointPointerOP=false;
      bool isIntPointArrayOP=false;
      for (unsigned int i=0; i< childrenRec.size(); i++) {
        SgNode* node = childrenRec[i];
        if (isSgAddOp(node) || isSgSubtractOp(node) || isSgDivideOp(node) || isSgMultiplyOp(node)) {
          checkNode(node);
        }
        //               std::cout << "     ***** returning: " << " . " <<  std::endl;
        // found a double or float
        if (isSgDoubleVal(node) || isSgFloatVal(node)) {
          isFloatPointOP=true;
          //                   std::cout << "found isSgDouble: " << nameNode << " . " <<  std::endl;
        }
        // found an Integer

        if (isSgIntVal(node)) {
          isIntPointOP=true;
          //                   std::cout << "found isSgInt: " << nameNode << " . " <<  std::endl;
        }
        // found a pointer to a double or float
        if (isSgCastExp(node)) {
          //                   std::cout << "found isSGCastExp: " << nameNode << " . " <<  std::endl;
          std::vector<SgNode*> childrenCast = node->get_traversalSuccessorContainer();
          node = childrenCast[0];
          if (isSgAddOp(node) || isSgSubtractOp(node) || isSgDivideOp(node) || isSgMultiplyOp(node)) {
            checkNode(node);
          }
        }
        if (isSgVarRefExp(node)) {
          SgVarRefExp* refexp = isSgVarRefExp(node);
          SgVariableSymbol* symbol = refexp->get_symbol();
          SgType* type = symbol->get_type();
          //                        std::cout << "----- isVarRefExp: " << type->findBaseType()->class_name() << " . " <<  std::endl;
          if (isSgTypeDouble(type->findBaseType()) || isSgTypeFloat(type->findBaseType())) {
            isFloatPointOP=true;
            //                       std::cout << "found isSgTypeDouble: " << nameNode << " . " << type->findBaseType()->class_name()<<  std::endl;
          }
          if (isSgTypeInt(type->findBaseType()) ) {
            isIntPointOP=true;
            //                       std::cout << "found isSgTypeInt: " << nameNode << " . " << type->findBaseType()->class_name()<<  std::endl;
          }
        }
        if (isSgPointerDerefExp(node)) {
          SgPointerDerefExp* deref = isSgPointerDerefExp(node);
          std::vector<SgNode*> children = deref->get_traversalSuccessorContainer();
          for (unsigned int i=0; i< children.size(); i++) {
            SgNode* nodeChild = children[i];
            if (isSgVarRefExp(nodeChild)) {
              SgVarRefExp* refexp = isSgVarRefExp(nodeChild);
              SgVariableSymbol* symbol = refexp->get_symbol();
              SgType* type = symbol->get_type();
              //                        std::cout << "----- isSgPointer: " << type->findBaseType()->class_name() << " . " <<  std::endl;
              if (isSgTypeDouble(type->findBaseType()) || isSgTypeFloat(type->findBaseType())) {
                isFloatPointPointerOP=true;
                //                        std::cout << "found isSgTypeDouble*: " << nameNode << " . " << type->findBaseType()->class_name()<<  std::endl;
              }
              if (isSgTypeInt(type->findBaseType()) ) {
                isIntPointPointerOP=true;
                //                        std::cout << "found isSgTypeInt*: " << nameNode << " . " << type->findBaseType()->class_name()<<  std::endl;
              }
            }
          }
        }
        // found a pointer to a double or float in an array
        if (isSgPntrArrRefExp(node)) {
          SgPntrArrRefExp* deref = isSgPntrArrRefExp(node);
          //                   std::cout << "----- isSgPntrArrRefExp: " <<  " . " <<  std::endl;

          std::vector<SgNode*> childrenExp = deref->get_traversalSuccessorContainer();
          for (unsigned int i=0; i< childrenExp.size(); i++) {
            SgNode* nodeChild2 = childrenExp[i];
            if (isSgVarRefExp(nodeChild2)) {
              SgVarRefExp* refexp = isSgVarRefExp(nodeChild2);
              SgVariableSymbol* symbol = refexp->get_symbol();
              SgType* type = symbol->get_type();
              //                           std::cout << "----- isSgArray: " << type->findBaseType()->class_name() << " . " <<  std::endl;
              if (isSgTypeDouble(type->findBaseType()) || isSgTypeFloat(type->findBaseType())) {
                isFloatPointArrayOP=true;
                //                             std::cout << "found isSgTypeDouble*Array: " << nameNode << " . " << type->findBaseType()->class_name()<< std::endl;
              }
              if (isSgTypeInt(type->findBaseType())) {
                //                             std::cout << "found isSgTypeInt*Array: " << nameNode << " . " << type->findBaseType()->class_name()<<  std::endl;
                isIntPointArrayOP=true;
              }
            }
          }
        }
      }
      if (isFloatPointOP) {
        //        fpops++;
        floatOps_actual++;
      }
      if (isFloatPointPointerOP || isFloatPointArrayOP) {
        //        fpopsp++;
        floatOps_actual++;
      }
      if (isIntPointOP) {
        //        intops++;
        intOps_actual++;
      }
      if (isIntPointPointerOP || isIntPointArrayOP) {
        //        intopsp++;
        intOps_actual++;
      }
    }
  }
}



int CompassAnalyses::ComputationalFunctions::Traversal::
checkDepth(std::vector<SgNode*> children) {
  int nrchildren=0;
  for (unsigned int i=0; i< children.size(); i++) {
    SgNode* node = children[i];
    if (node==NULL) {
      continue;
    }
    if (isSgAddOp(node) || isSgSubtractOp(node) || isSgDivideOp(node) || isSgMultiplyOp(node)) {
      checkNode(node);
    } else {
      std::vector<SgNode*> childrenRec = node->get_traversalSuccessorContainer();
      if (childrenRec.size()>0) {
        nrchildren+=checkDepth(childrenRec);
      }
    }
    nrchildren += 1;
  }
  return nrchildren;
}

void
CompassAnalyses::ComputationalFunctions::Traversal::
visit(SgNode* sgNode)
{ 
  // Implement your traversal here.  
  if (isSgFunctionDeclaration(sgNode)) {
    intOps_actual=0;
    floatOps_actual=0;

    SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(sgNode);
    // iterate the children and check for problems.
    std::vector<SgNode*> children = funcDecl->get_traversalSuccessorContainer();
    checkDepth(children);

    if (intOps_actual>intOps || floatOps_actual>floatOps)
      output->addOutput(new CheckerOutput(funcDecl));

    //    std::string name = funcDecl->get_qualified_name();
    //std::cout << "function: " << name << 
    //  "found intops: " << intOps_actual << "  found floatops: " << floatOps_actual << std::endl;
  }  

} //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::ComputationalFunctions::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::ComputationalFunctions::Traversal(params, output);
}

extern const Compass::Checker* const computationalFunctionsChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::ComputationalFunctions::checkerName,
        CompassAnalyses::ComputationalFunctions::shortDescription,
        CompassAnalyses::ComputationalFunctions::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
