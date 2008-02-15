// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.

// #include "nameQuery.h"
#include "rose.h"
#include "roseQueryLib.h"

#if 0

// Contains class declarations for abstractions attributes 
// to be added to the AST nodes.
#include "AbstractionAttribute.h"

// Indlude the library specific code for this translator
#include "AplusplusAbstractionGeneratedSource.h"

// Define a traversal over the AST to look at the library specific 
// abstractions identified at each node.
class MyTraversal : public AstSimpleProcessing
   {
     public:
          void visit (SgNode* astNode);
   };

void MyTraversal::visit (SgNode* astNode)
   {
     ROSE_ASSERT (astNode != NULL);

  // switch (astNode->variantT(DEFINED_LIBRARY_NAME_Enum) )
     arrayVariantEnum arrayVariant = getArrayAbstractionVariant(astNode);
     switch (arrayVariant)
        {
       // Common C++ language construct enum
          case V_SgExpressionStatement:
               SgExpressionStatement* expressionStatement = isSgExpressionStatement(astNode);
               break;

       // Specialized enum value that is specific to the binary operator+()
       // for a doubleArray type within an array library.
          case V_doubleArrayBinaryOperatorPlus:
            // BinaryOperatorAttribute* binaryOperator = astNode->attribute.get("DEFINED_LIBRARY_NAME_Abstraction");
               BinaryOperatorAttribute* binaryOperator = isDEFINED_LIBRARY_NAME_Abstraction(astNode);
               break;

       // Everything else
          default:
               printf ("Error, default reached in DEFINED_LIBRARY_NAME_RecognitionTraversal::visit() \n");
               ROSE_ABORT();
        }
   }
#endif

int main( int argc, char * argv[] )
   {
     SgProject* sageProject = frontend(argc,argv);

  // printf ("Generate the pdf output of the SAGE III AST \n");
  // generatePDF ( project );

#if 0
     MyTraversal treeTraversal;

  // Ignore the return value since we don't need it
  // treeTraversal.traverseInputFiles(project,inheritedAttribute);
     treeTraversal.traverse(project,MyTraversal::preorder);
#endif

     return backend(sageProject); // only backend error code is reported
   }






