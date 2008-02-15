// For Loop Construction Control Stmt
// Author: Gary M. Yuan
// Date: 23-July-2007

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
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.

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

