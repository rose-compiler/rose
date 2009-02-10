// Prefer Setvbuf To Setbuf
// Author: Mark Lewandowski, -422-3849
// Date: 20-November-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_PREFER_SETVBUF_TO_SETBUF_H
#define COMPASS_PREFER_SETVBUF_TO_SETBUF_H

namespace CompassAnalyses
   { 
     namespace PreferSetvbufToSetbuf
        { 
        /*! \brief Prefer Setvbuf To Setbuf: Add your description here 
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

// COMPASS_PREFER_SETVBUF_TO_SETBUF_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Prefer Setvbuf To Setbuf Analysis
// Author: Mark Lewandowski, -422-3849
// Date: 20-November-2007

#include "compass.h"
// #include "preferSetvbufToSetbuf.h"

namespace CompassAnalyses
   { 
     namespace PreferSetvbufToSetbuf
        { 
          const std::string checkerName      = "PreferSetvbufToSetbuf";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "For added error checking, prefer using setvbuf() over setbuf()";
          const std::string longDescription  = "setvbuf() is equivalent to setbuf() with _IOFBF for mode and BUFSIZE for size (if buf is not NULL) or _IONBF for mode (if buf is NULL), except that it returns a nonzero value if the request could not be honored. For added error checking, prefer using setvbuf() over setbuf()";
        } //End of namespace PreferSetvbufToSetbuf.
   } //End of namespace CompassAnalyses.

CompassAnalyses::PreferSetvbufToSetbuf::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::PreferSetvbufToSetbuf::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["PreferSetvbufToSetbuf.YourParameter"]);


   }

void
CompassAnalyses::PreferSetvbufToSetbuf::Traversal::
visit(SgNode* node)
   { 
     SgFunctionRefExp* func= isSgFunctionRefExp(node);

     if (func) {
       if (func->get_symbol()->get_name().getString().compare("setbuf") == 0) {
         output->addOutput(new CheckerOutput(node));
       }
     }

   } //End of the visit function.
   

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::PreferSetvbufToSetbuf::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::PreferSetvbufToSetbuf::Traversal(params, output);
}

extern const Compass::Checker* const preferSetvbufToSetbufChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::PreferSetvbufToSetbuf::checkerName,
        CompassAnalyses::PreferSetvbufToSetbuf::shortDescription,
        CompassAnalyses::PreferSetvbufToSetbuf::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
