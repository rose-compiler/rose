// Non Associative Relational Operators
// Author: Gary Yuan
// Date: 02-January-2008

#include "compass.h"

#ifndef COMPASS_NON_ASSOCIATIVE_RELATIONAL_OPERATORS_H
#define COMPASS_NON_ASSOCIATIVE_RELATIONAL_OPERATORS_H

namespace CompassAnalyses
   { 
     namespace NonAssociativeRelationalOperators
        { 
        /*! \brief Non Associative Relational Operators: Add your description here 
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

          bool isRelationalOperator( SgNode *n );

          class ExpressionTraversal : public AstSimpleProcessing
          {
            private:
              int count;
            public:
              ExpressionTraversal();
              int run(SgNode *n);
              void visit(SgNode *n);
          }; //class ExpressionTraversal

          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
               private:
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

// COMPASS_NON_ASSOCIATIVE_RELATIONAL_OPERATORS_H
#endif 

