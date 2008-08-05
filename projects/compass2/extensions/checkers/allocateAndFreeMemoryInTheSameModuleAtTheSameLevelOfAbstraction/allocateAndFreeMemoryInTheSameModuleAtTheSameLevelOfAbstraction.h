// Allocate And Free Memory In The Same Module At The Same Level Of Abstraction
// Author: Mark Lewandowski, -422-3849
// Date: 16-November-2007

#include "compass.h"

#ifndef COMPASS_ALLOCATE_AND_FREE_MEMORY_IN_THE_SAME_MODULE_AT_THE_SAME_LEVEL_OF_ABSTRACTION_H
#define COMPASS_ALLOCATE_AND_FREE_MEMORY_IN_THE_SAME_MODULE_AT_THE_SAME_LEVEL_OF_ABSTRACTION_H

namespace CompassAnalyses
   { 
     namespace AllocateAndFreeMemoryInTheSameModuleAtTheSameLevelOfAbstraction
        { 
        /*! \brief Allocate And Free Memory In The Same Module At The Same Level Of Abstraction: Add your description here 
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
                    void run(SgNode* n){ this->traverse(n, postorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_ALLOCATE_AND_FREE_MEMORY_IN_THE_SAME_MODULE_AT_THE_SAME_LEVEL_OF_ABSTRACTION_H
#endif 

