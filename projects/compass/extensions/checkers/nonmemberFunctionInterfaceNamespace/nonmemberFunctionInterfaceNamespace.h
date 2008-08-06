// Nonmember Function Interface Namespace
// Author: Gergo  Barany
// Date: 31-July-2007

#include "compass.h"

#ifndef COMPASS_NONMEMBER_FUNCTION_INTERFACE_NAMESPACE_H
#define COMPASS_NONMEMBER_FUNCTION_INTERFACE_NAMESPACE_H

namespace CompassAnalyses
   { 
     namespace NonmemberFunctionInterfaceNamespace
        { 
        /*! \brief Nonmember Function Interface Namespace: Add your description here 
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
                 SgNamespaceDefinitionStatement *enclosingNamespace(SgNode *p);
                 bool sameNamespace(SgNamespaceDefinitionStatement *a,
                                    SgNamespaceDefinitionStatement *b);
                 bool stdNamespace(SgNamespaceDefinitionStatement *n);
                 bool checkTypeNamespace(SgType *type,
                                         SgNamespaceDefinitionStatement *n);

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_NONMEMBER_FUNCTION_INTERFACE_NAMESPACE_H
#endif 

