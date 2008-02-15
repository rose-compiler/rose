// Forbidden Functions
// Author: Gary Yuan
// Date: 28-December-2007

#include <map>
#include <string>
#include "compass.h"

#ifndef COMPASS_FORBIDDEN_FUNCTIONS_H
#define COMPASS_FORBIDDEN_FUNCTIONS_H

namespace CompassAnalyses
   { 
     namespace ForbiddenFunctions
        { 
        /*! \brief Forbidden Functions: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(
                      SgNode* node, 
                      const std::string &, 
                      const std::string & );
             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
               private:
                 bool isCheckerFile;
                 std::string previousFileName;
                 std::map<std::string,std::string> forbiddenFunctionMap;
                 void parseParameter( const std::string & param );
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

// COMPASS_FORBIDDEN_FUNCTIONS_H
#endif 

