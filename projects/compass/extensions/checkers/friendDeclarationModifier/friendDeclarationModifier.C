// Friend Declaration Modifier
// Author: Gary M. Yuan
// Date: 30-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_FRIEND_DECLARATION_MODIFIER_H
#define COMPASS_FRIEND_DECLARATION_MODIFIER_H

namespace CompassAnalyses
   { 
     namespace FriendDeclarationModifier
        { 
        /*! \brief Friend Declaration Modifier: Add your description here 
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

// COMPASS_FRIEND_DECLARATION_MODIFIER_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// Friend Declaration Modifier Analysis
// Author: Gary M. Yuan
// Date: 30-July-2007

#include "compass.h"
// #include "friendDeclarationModifier.h"

namespace CompassAnalyses
   { 
     namespace FriendDeclarationModifier
        { 
          const std::string checkerName      = "FriendDeclarationModifier";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks if a declarations statement contains the \"friend\" keyword modifier ";
          const std::string longDescription  = "Avoid using the \"friend\" keyword modifier because they bypass access restrictions and hide dependencies between classes and functions. Also \"friends\" are often indicative of poor design.";
        } //End of namespace FriendDeclarationModifier.
   } //End of namespace CompassAnalyses.

CompassAnalyses::FriendDeclarationModifier::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::FriendDeclarationModifier::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["FriendDeclarationModifier.YourParameter"]);


   }

void
CompassAnalyses::FriendDeclarationModifier::Traversal::
visit(SgNode* node)
   { 
     SgDeclarationStatement *ds = isSgDeclarationStatement(node);

     if( ds != NULL )
     {
       SgDeclarationModifier dsm = ds->get_declarationModifier();

       if( dsm.isFriend() == true )
       {
         output->addOutput( new CheckerOutput(node) );
       } //if( dsm.isFriend() == true )
     } // if( ds != NULL )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::FriendDeclarationModifier::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::FriendDeclarationModifier::Traversal(params, output);
}

extern const Compass::Checker* const friendDeclarationModifierChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::FriendDeclarationModifier::checkerName,
        CompassAnalyses::FriendDeclarationModifier::shortDescription,
        CompassAnalyses::FriendDeclarationModifier::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
