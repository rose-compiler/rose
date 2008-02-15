// Prefer Setvbuf To Setbuf
// Author: Mark Lewandowski, -422-3849
// Date: 20-November-2007

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
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.

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

