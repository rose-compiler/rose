// Default Constructor
// Author: Gary M. Yuan
// Date: 07-August-2007

#include "compass.h"

#ifndef COMPASS_DEFAULT_CONSTRUCTOR_H
#define COMPASS_DEFAULT_CONSTRUCTOR_H

namespace CompassAnalyses
   { 
     namespace DefaultConstructor
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
          //////////////////////////////////////////////////////////////////////          /// The CheckerOutput class implements the violation output for this
          /// checker
          /////////////////////////////////////////////////////////////////////
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                 /// The constructor
                 /// \param node is a SgNode
                    CheckerOutput(SgNode* node);
             };

       // Specification of Checker Traversal Implementation
          //////////////////////////////////////////////////////////////////////
          /// The Traversal class implements a simple AST traversal seeking out
          /// SgClassDefinition nodes. The children of any such nodes are
          /// stored in a successor container and are looped over to find
          /// a default constructor public member function. If no default
          /// constructor is found then a violation is flagged.
          //////////////////////////////////////////////////////////////////////
          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.

               public:
                    /// The constructor
                    /// \param out is a Compass::OutputObject*
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // The implementation of the run function has to match the traversal being called.
                    /// run function
                    /// \param n is a SgNode*
                    void run(SgNode* n){ this->traverse(n, preorder); };

                    /// visit function
                    /// \param n is a SgNode
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_DEFAULT_CONSTRUCTOR_H
#endif 

