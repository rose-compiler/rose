// Cyclomatic Complexity
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"

#ifndef COMPASS_CYCLOMATIC_COMPLEXITY_H
#define COMPASS_CYCLOMATIC_COMPLEXITY_H

namespace CompassAnalyses
   { 
     namespace CyclomaticComplexity
        { 
        /*! \brief Cyclomatic Complexity: Add your description here 
         */

          extern const std::string checkerName;
          extern std::string shortDescription;
          extern std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
	       CheckerOutput(SgNode* node, const std::string & reason );
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;
	       static int cc;
	       static int complexity;
               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };
		    static std::string getCC() { 
		      std::ostringstream myStream; //creates an ostringstream object
		      myStream << cc << std::flush;
		      return (myStream.str()); //returns the string form of the stringstream object
		    }
		    static std::string getComplexity() { 
		      std::ostringstream myStream; //creates an ostringstream object
		      myStream << complexity << std::flush;
		      return (myStream.str()); //returns the string form of the stringstream object
		    }
		    void checkNode(SgNode* node);
		    int checkDepth(std::vector<SgNode*> children);
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_CYCLOMATIC_COMPLEXITY_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Cyclomatic Complexity Analysis
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"
// #include "cyclomaticComplexity.h"

namespace CompassAnalyses
{ 
  namespace CyclomaticComplexity
  { 
    /*namespace Traversal {
      int cc=0;
      int complexity=0;
    }*/
    //GMY 12/26/2007 changed namespace Traversal to explicit scope because of
    //error in EDG front-end processing.
    //These are static data member initializations of class Traversal
    int Traversal::cc = 0;
    int Traversal::complexity = 0;

    //12/26/2007 END

    const std::string checkerName      = "CyclomaticComplexity";

    // Descriptions should not include the newline character "\n".
    std::string shortDescription = 
    "This function is too complex : CC = ";
    std::string longDescription  = 
    "This analysis detects functions with lots of branch conditions. The analysis is implemented according to Mc Cabe's Cyclomatic Complexity. The CC value for this analysis  = ";
  } //End of namespace CyclomaticComplexity.
} //End of namespace CompassAnalyses.

CompassAnalyses::CyclomaticComplexity::
CheckerOutput::CheckerOutput ( SgNode* node,const std::string & reason  )
  : OutputViolationBase(node,checkerName,reason+shortDescription+
                        Traversal::getComplexity()+" > "+Traversal::getCC())
{}

CompassAnalyses::CyclomaticComplexity::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["CyclomaticComplexity.YourParameter"]);
  cc = Compass::parseInteger(inputParameters["CyclomaticComplexity.maxComplexity"]);
#if 0 // JJW (8/12/2008): We cannot change these now
  setShortDescription(shortDescription + Traversal::getComplexity()+" > "+Traversal::getCC());
  setLongDescription (longDescription + Traversal::getComplexity()+" > "+Traversal::getCC());
#endif
}

void CompassAnalyses::CyclomaticComplexity::Traversal::
checkNode(SgNode* node) {
  if (isSgIfStmt(node) || isSgCaseOptionStmt(node) || isSgForStatement(node) || isSgDoWhileStmt(node) || isSgWhileStmt(node)) {
    complexity++;
  }
}


int CompassAnalyses::CyclomaticComplexity::Traversal::
checkDepth(std::vector<SgNode*> children) {
  int nrchildren=0;
  for (unsigned int i=0; i< children.size(); i++) {
    SgNode* node = children[i];
    if (node==NULL) {
      continue;
    }
    std::vector<SgNode*> childrenRec = node->get_traversalSuccessorContainer();
    if (childrenRec.size()>0) {
      nrchildren+=checkDepth(childrenRec);
    }
    nrchildren += 1;
    checkNode(node);
  }
  return nrchildren;
}


void
CompassAnalyses::CyclomaticComplexity::Traversal::
visit(SgNode* node)
{ 
  if (isSgFunctionDeclaration(node)) {
    complexity=0;
    // Implement your traversal here.  
    SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(node);
    // iterate the children and check for problems.
    std::vector<SgNode*> children = funcDecl->get_traversalSuccessorContainer();
    checkDepth(children);
    if (complexity>cc)
      output->addOutput(new CheckerOutput(funcDecl, "\t"+funcDecl->get_name()+"\t"));
  }
} //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::CyclomaticComplexity::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::CyclomaticComplexity::Traversal(params, output);
}

extern const Compass::Checker* const cyclomaticComplexityChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::CyclomaticComplexity::checkerName,
        CompassAnalyses::CyclomaticComplexity::shortDescription,
        CompassAnalyses::CyclomaticComplexity::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
