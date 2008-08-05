// Type Typedef
// Author: Andreas Saebjoernsen
// Date: 24-July-2007

#include "compass.h"

#ifndef COMPASS_TYPE_TYPEDEF_H
#define COMPASS_TYPE_TYPEDEF_H

namespace CompassAnalyses
   { 
     namespace TypeTypedef
        { 
        /*! \brief Type Typedef: Add your description here 
         */

          extern const std::string checkerName;
           extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             {
                    SgType* toType;
                    SgInitializedName* IRnode;

                    //What the type is 
                    std::string is;
                    //What typedef the type shoud be
                    std::string shouldBe;
               public:
                    CheckerOutput(SgType* to, std::string isN, std::string shouldBeN, SgInitializedName* node);
                    virtual std::string getString() const;

             };

       // Specification of Checker Traversal Implementation

          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.
                    std::string ruleFile;
                    //map of a typedef to a pair. The pair is boolean and a string.
                    //The string is the type of the typedef. The boolean is true if
                    //const modifiers are allowed in front of the type of the typedef.
                    std::map<std::string, std::pair<bool,std::string> > typedefsToUse;
                    //A map from the type of a typedef to the typedef
                    std::map<std::string, std::string > typeToTypedefs;

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_TYPE_TYPEDEF_H
#endif 

