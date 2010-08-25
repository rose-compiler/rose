// Loc Per Function
// Author: Thomas Panas
// Date: 23-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_LOC_PER_FUNCTION_H
#define COMPASS_LOC_PER_FUNCTION_H

namespace CompassAnalyses
   { 
     namespace LocPerFunction
        { 
        /*! \brief Loc Per Function: Add your description here 
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
	       static int loc;
	       static int loc_actual;
               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);
                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };
		    static std::string getLOC() { 
		      std::ostringstream myStream; //creates an ostringstream object
		      myStream << loc << std::flush;
		      return (myStream.str()); //returns the string form of the stringstream object
		    }
		    static std::string getLOC_actual() { 
		      std::ostringstream myStream; //creates an ostringstream object
		      myStream << loc_actual << std::flush;
		      return (myStream.str()); //returns the string form of the stringstream object
		    }
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_LOC_PER_FUNCTION_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Loc Per Function Analysis
// Author: Thomas Panas
// Date: 23-July-2007

#include "compass.h"
// #include "locPerFunction.h"


namespace CompassAnalyses
{ 
  namespace LocPerFunction
  { 
    /*namespace Traversal {
      int loc=0;
      int loc_actual=0;
    }*/
    //GMY 12/26/2007 changed namespace Traversal to explicit scope because of
    //error in EDG front-end processing.
    //These are static data member initializations of class Traversal

    int Traversal::loc = 0;
    int Traversal::loc_actual = 0;
    //12/26/2007 END

    const std::string checkerName      = "LocPerFunction";

    // Descriptions should not include the newline character "\n".
    std::string shortDescription = 
        "This function has too many lines of code :: LOC = " ;
    std::string longDescription  = 
    "This analysis detects functions with lines of code larger than a predefined value. The current value is ";
  } //End of namespace LocPerFunction.
} //End of namespace CompassAnalyses.

CompassAnalyses::LocPerFunction::
CheckerOutput::CheckerOutput ( SgNode* node )
  : OutputViolationBase(node,checkerName,shortDescription+
                        Traversal::getLOC_actual()+" > "+Traversal::getLOC())
{}

CompassAnalyses::LocPerFunction::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
  : output(output)
{
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["LocPerFunction.YourParameter"]);

  loc = Compass::parseInteger(inputParameters["LocPerFunction.Size"]);
#if 0 // JJW 8/12/2008: We cannot change these anymore
  setShortDescription(shortDescription +  Traversal::getLOC_actual()+" > "+Traversal::getLOC());
  setLongDescription (longDescription +  Traversal::getLOC_actual()+" > "+Traversal::getLOC());
#endif
}


void
CompassAnalyses::LocPerFunction::Traversal::
visit(SgNode* sgNode)
{ 
  // Implement your traversal here.  
  if (isSgFunctionDeclaration(sgNode)) {
    SgFunctionDeclaration* funcDecl = isSgFunctionDeclaration(sgNode);
    SgFunctionDefinition* funcDef = funcDecl->get_definition();
    if (funcDef) {
      Sg_File_Info* start = funcDef->get_body()->get_startOfConstruct();
      Sg_File_Info* end = funcDef->get_body()->get_endOfConstruct();
      ROSE_ASSERT(start);
      ROSE_ASSERT(end);
      int lineS = start->get_line();
      int lineE = end->get_line();
      loc_actual = lineE-lineS;
      if (loc_actual>loc) {
        output->addOutput(new CheckerOutput(funcDef));
      }
    }
  }

} //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::LocPerFunction::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::LocPerFunction::Traversal(params, output);
}

extern const Compass::Checker* const locPerFunctionChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::LocPerFunction::checkerName,
        CompassAnalyses::LocPerFunction::shortDescription,
        CompassAnalyses::LocPerFunction::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
