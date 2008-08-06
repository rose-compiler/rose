// Possibly Replicated Variables
// Author: Jeremiah J. Willcock
// Date: 07-January-2008

#include "compass.h"

#ifndef COMPASS_POSSIBLY_REPLICATED_VARIABLES_H
#define COMPASS_POSSIBLY_REPLICATED_VARIABLES_H

namespace CompassAnalyses
   { 
     namespace PossiblyReplicatedVariables
        { 
        /*! \brief Possibly Replicated Variables: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(SgNode* node, bool isSharable);
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n);
             };
        }
   }

// COMPASS_POSSIBLY_REPLICATED_VARIABLES_H
#endif 

