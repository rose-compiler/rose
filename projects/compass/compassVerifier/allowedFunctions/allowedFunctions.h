// Allowed Functions
// Author: Gary M. Yuan
// Date: 19-June-2008

#include "compass.h"

#ifndef COMPASS_ALLOWED_FUNCTIONS_H
#define COMPASS_ALLOWED_FUNCTIONS_H

namespace CompassAnalyses
   { 
     namespace AllowedFunctions
        { 
        /*! \brief Allowed Functions: Add your description here 
         */

          extern const std::string checkerName;
          extern const std::string shortDescription;
          extern const std::string longDescription;

       // Specification of Checker Output Implementation
          class CheckerOutput: public Compass::OutputViolationBase
             { 
               public:
                    CheckerOutput(
                      SgNode* node, const std::string &what);
             };

       // Specification of Checker Traversal Implementation

          enum OP_CODES{ CONTINUE=0, GENERATE_CURRENT_LIST=1 };

          class Traversal
             : public AstSimpleProcessing, public Compass::TraversalBase
             {
            // Checker specific parameters should be allocated here.
               private:
                 bool isGenerateCurrentListOfAllowedFunctions;
                 int allowedFunctionIndex;
                 std::ofstream *outf;
                 std::set<std::string> allowedFunctionSet;
                 std::vector<std::string> allowedNamespaces;
                 SgFile *currentFile;

//               int parseParameter( const std::string & param );

                 void functionDeclarationHandler( 
                   const SgFunctionDeclaration *fdecl, 
                   std::string frefFileName,
                   SgNode *node );

                 void uniqueNameGenerator(
                   std::stringstream &ss,
                   const SgFunctionDeclaration *fdecl,
                   std::string &qname );

//               std::string getQualifiedNamespace( const std::string &fname );

                 std::string typeVariantT( SgType *type, int vT ); 

               public:
                    Traversal(Compass::Parameters inputParameters, Compass::OutputObject* output);

                 // Change the implementation of this function if you are using inherited attributes.
                    void *initialInheritedAttribute() const { return NULL; }

                 // The implementation of the run function has to match the traversal being called.
                 // If you use inherited attributes, use the following definition:
                 // void run(SgNode* n){ this->traverse(n, initialInheritedAttribute()); }
                    void run(SgNode* n);//{ this->traverse(n, preorder); }

                 // Change this function if you are using a different type of traversal, e.g.
                 // void *evaluateInheritedAttribute(SgNode *, void *);
                 // for AstTopDownProcessing.
                    void visit(SgNode* n);
             };
        }
   }

// COMPASS_ALLOWED_FUNCTIONS_H
#endif 

