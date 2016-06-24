#if 0
// IBM using gcc 3.2.2 (so this is what we will require for ROSE)

// The point of this program is to transform code to call functions
// before and after all variable references (SgVarRefExp).
// Examples include:
//    x = 0;  ->  (b(),x) = 0, a(), x;
//    x = y + z;  -> ((b(),x) = ((b(),y) + ((b(),z), az(), z), ay(), y), ax(), x);

// It appears the instrumentation "after" a read or write can not be done well 
// (not done immediately after the read or write and before any other operations 
// and without forcing multiple reads of the variable).  So we restrict the 
// instrumentation to "before" variable reads and writes.  In this case
// we transform "x;" to "br("x"),x; and "x = 0;" to "(bw("x"),x) = 0;", because
// of the precedence of the operator, is the lowest possilbe, we have to introduce
// "()" everywhere we use the comma operator. 

//  A problem is that the user could have defined a comma operator for a class
// (it can not be defined for primative types directly), for example:
// "someType operator, (void,someType);".  In this case we can use ROSE to
// detect the existence of such a function (in global scope or a namespace).
// An AST Query could be use to check this (or a visit function over the whole AST).
#endif

// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it shows the design of a transformation to instrument source code, placing source code
// at the top and bottome of each basic block.

#include "rose.h"

/*
   Design of this code.
      Inputs: source code (file.C)
      Outputs: instrumented source code (rose_file.C and file.o)

   Properties of instrumented source code:
       1) Builds functions to be called before and after each variable access.
       2) adds function call before an after the reading and writing of all variables.
*/

// Simple ROSE traversal class: This allows us to visit all the functions and add
// new code to instrument/record their use.
class SimpleInstrumentation : public SgSimpleProcessing
   {
     public:
          SgFunctionSymbol* startingFunctionSymbol;
          SgFunctionSymbol* endingFunctionSymbol;

          SimpleInstrumentation();
       // SgFunctionSymbol* buildNewFunctionDeclaration ( SgProject* project, SgFunctionType* previousFunctionType );
          SgFunctionSymbol* buildNewFunctionDeclaration ( SgStatement* statementLocation, SgFunctionType* previousFunctionType );

       // required visit function to define what is to be done
          void visit ( SgNode* astNode );
   };

SimpleInstrumentation::SimpleInstrumentation() 
   : startingFunctionSymbol(NULL), endingFunctionSymbol(NULL)
   {
   }

SgFunctionSymbol*
SimpleInstrumentation::buildNewFunctionDeclaration ( SgStatement* statementLocation, string functionName )
   {
  // *****************************************************
  // Create the functionDeclaration
  // *****************************************************

  // Must mark the newly built node to be a part of a transformation so that it will be unparsed!
     Sg_File_Info * file_info = new Sg_File_Info();
     ROSE_ASSERT(file_info != NULL);
     file_info->set_isPartOfTransformation(true);

     SgType* returnType = new SgTypeVoid();
     SgFunctionType *functionType = new SgFunctionType(returnType,false);
     ROSE_ASSERT(previousFunctionType != NULL);
     SgFunctionDeclaration* functionDeclaration = new SgFunctionDeclaration(file_info, functionName, functionType);
     ROSE_ASSERT(functionDeclaration != NULL);
     ROSE_ASSERT(functionDeclaration->get_parameterList() != NULL);

  // Add any additional function arguments here! Make sure that the function type is consistant.

  // Get the scope
     SgScopeStatement* scope = statementLocation->get_scope();

  // Set the parent node in the AST (this could be done by the AstPostProcessing
     functionDeclaration->set_parent(scope);

  // Set the scope explicitly (since it could be different from the parent?)
     functionDeclaration->set_scope(scope);

  // If it is not a forward declaration then the unparser will skip the ";" at the end (need to fix this better)
     functionDeclaration->setForward();
     ROSE_ASSERT(functionDeclaration->isForward() == true);

  // Mark function as extern "C"
     functionDeclaration->get_declarationModifier().get_storageModifier().setExtern();
     functionDeclaration->set_linkage("C");  // This mechanism could be improved!

  // Put the new function declaration before the function containing the input statementLocation
     bool inFront = true;
     SgGlobal* globalScope = TransformationSupport::getGlobalScope(statementLocation);
     SgFunctionDeclaration* functionDeclarationInGlobalScope = 
          TransformationSupport::getFunctionDeclaration(statementLocation);
     ROSE_ASSERT(globalScope != NULL);
     ROSE_ASSERT(functionDeclarationInGlobalScope != NULL);
     globalScope->insert_statement(functionDeclarationInGlobalScope,functionDeclaration,inFront);

     SgFunctionSymbol* functionSymbol = new SgFunctionSymbol(functionDeclaration);
     ROSE_ASSERT(functionSymbol != NULL);
     ROSE_ASSERT(functionSymbol->get_type() != NULL);

     return functionSymbol;
   }

void SimpleInstrumentation::visit ( SgNode* astNode )
   {
     switch(astNode->variantT())
        {
          case V_SgVarRefExp:
             {
               SgVarRefExp *varRefExp = isSgVarRefExp(astNode);
               SgType *variableType = varRefExp->get_type();
               ROSE_ASSERT(variableType);

            // We are only interested in primative types or variables that are 
            // pointers or references to primative types. So check the stripped type.
               SgType* baseType = variableType->stripType();

            // If the base type is a class then ignore it, its primative types 
            // will be handled individually.
               if (isSgClassType(baseType) == NULL)
                  {
                    
                  }
               break;
             }

          default:
             {
            // No other special cases
             }
        }
   }

int
main ( int argc, char * argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

  // Build ROSE AST
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT(project != NULL);

     generateDOT ( *project );

  // Call traversal to do instrumentation (put instumentation into the AST).
     SimpleInstrumentation treeTraversal;
     treeTraversal.traverseInputFiles ( project, preorder );

  // Generate Code and compile it with backend (vendor) compiler to generate object code
  // or executable (as specified on commandline using vendor compiler's command line).
  // Returns error code form compilation using vendor's compiler.
     return backend(project);
   }

