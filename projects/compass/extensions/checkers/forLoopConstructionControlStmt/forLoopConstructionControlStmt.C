// For Loop Construction Control Stmt
// Author: Gary M. Yuan
// Date: 23-July-2007

#include "rose.h"
#include "compass.h"

#ifndef COMPASS_FOR_LOOP_CONSTRUCTION_CONTROL_STMT_H
#define COMPASS_FOR_LOOP_CONSTRUCTION_CONTROL_STMT_H

namespace CompassAnalyses
   { 
     namespace ForLoopConstructionControlStmt
        { 
          /// \brief checkerName is a std::string containing the name of this
          /// checker.
          extern const std::string checkerName;
          /// \brief shortDescription is a std::string with a short description
          /// of this checker's pattern
          extern const std::string shortDescription;
          /// \brief longDescription is a std::string with a detailed
          /// description of this checker's pattern and purpose.
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          //////////////////////////////////////////////////////////////////////
          /// The CheckerOutput class implements the violation output for this
          /// checker
          //////////////////////////////////////////////////////////////////////
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    /// The constructor
                    /// \param node is a SgNode*
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation
          //////////////////////////////////////////////////////////////////////
          /// The Traversal class performs a simple AST traversal seeking out
          /// for loop statements. The SgNode at that point is stripped of its
          /// containing block and searched for instances of non-construct
          /// expressions. If any are found then they are flagged as violations.
          //////////////////////////////////////////////////////////////////////
          class Traversal
             : public Compass::AstSimpleProcessingWithRunFunction
             {
            // Checker specific parameters should be allocated here.
               Compass::OutputObject* output;

               public:
                    /// The constructor
                    /// \param inputParameters is a Compass::Parameters
                    /// \param output is a Compass::OutputObject*
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    /// run function
                    /// \param n is a SgNode*
                    void run(SgNode* n){ this->traverse(n, preorder); };
                    /// visit function
                    /// \param n is a SgNode*
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_FOR_LOOP_CONSTRUCTION_CONTROL_STMT_H
#endif 

// -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*-
// vim: expandtab:shiftwidth=2:tabstop=2

// For Loop Construction Control Stmt Analysis
// Author: Gary M. Yuan
// Date: 23-July-2007

#include "compass.h"
// #include "forLoopConstructionControlStmt.h"

namespace CompassAnalyses
   { 
     namespace ForLoopConstructionControlStmt
        { 
          const std::string checkerName      = "ForLoopConstructionControlStmt";

       // Descriptions should not include the newline character "\n".
          const std::string shortDescription = "This checker checks that only loop control expressions appear in the for loop constructor block";

          const std::string longDescription  = "Unlike while loop constructs, non-control statements in for loop constructors tend to appear beyond the test expression. Often these statements may be moved in to the body of the for loop construct easily and results in greater clarity in the for loop construction code.";
        } //End of namespace ForLoopConstructionControlStmt.
   } //End of namespace CompassAnalyses.

CompassAnalyses::ForLoopConstructionControlStmt::
CheckerOutput::CheckerOutput ( SgNode* node )
   : OutputViolationBase(node,checkerName,shortDescription)
   {}

CompassAnalyses::ForLoopConstructionControlStmt::Traversal::
Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output)
   : output(output)
   {
  // Initalize checker specific parameters here, for example: 
  // YourParameter = Compass::parseInteger(inputParameters["ForLoopConstructionControlStmt.YourParameter"]);


   }

void
CompassAnalyses::ForLoopConstructionControlStmt::Traversal::
visit(SgNode* node)
   { 
     SgCommaOpExp *commaStmt = isSgCommaOpExp( node );

     if( commaStmt != NULL )
     {
       SgNode *parent = node->get_parent();

       if( parent != NULL && isSgForStatement(parent) != NULL )
         output->addOutput( new CheckerOutput( parent ) );
     } //if( commaStmt != NULL )

     return;
   } //End of the visit function.

static void run(Compass::Parameters params, Compass::OutputObject* output) {
  CompassAnalyses::ForLoopConstructionControlStmt::Traversal(params, output).run(Compass::projectPrerequisite.getProject());
}

static Compass::AstSimpleProcessingWithRunFunction* createTraversal(Compass::Parameters params, Compass::OutputObject* output) {
  return new CompassAnalyses::ForLoopConstructionControlStmt::Traversal(params, output);
}

extern const Compass::Checker* const forLoopConstructionControlStmtChecker =
  new Compass::CheckerUsingAstSimpleProcessing(
        CompassAnalyses::ForLoopConstructionControlStmt::checkerName,
        CompassAnalyses::ForLoopConstructionControlStmt::shortDescription,
        CompassAnalyses::ForLoopConstructionControlStmt::longDescription,
        Compass::C | Compass::Cpp,
        Compass::PrerequisiteList(1, &Compass::projectPrerequisite),
        run,
        createTraversal);
