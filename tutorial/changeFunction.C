// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.
// Specifically it shows the design of a transformation to instrument source code, placing source code
// at the top and bottome of each basic block.

#include "rose.h"

using namespace std;

/*
   Design of this code.
      Inputs: source code (file.C)
      Outputs: instrumented source code (rose_file.C and file.o)

   Properties of instrumented source code:
       1) added declaration for coverage support function
          (either forward function declaration or a #include to include a header file).
       2) Each function in the source program is instrumented to include a call to the
          coverage support function/
*/


// Simple ROSE traversal class: This allows us to visit all the functions and add
// new code to instrument/record their use.
class SimpleInstrumentation : public SgSimpleProcessing
   {
     public:
          SgFunctionSymbol* functionSymbol;
          SgFunctionSymbol* newFunctionSymbol;

          SimpleInstrumentation();
       // SgFunctionSymbol* buildNewFunctionDeclaration ( SgProject* project, SgFunctionType* previousFunctionType );
          SgFunctionSymbol* buildNewFunctionDeclaration ( SgStatement* statementLocation, SgFunctionType* previousFunctionType );

       // required visit function to define what is to be done
          void visit ( SgNode* astNode );
   };

SimpleInstrumentation::SimpleInstrumentation() 
   : functionSymbol(NULL), newFunctionSymbol(NULL)
   {
   }

// SgFunctionType *origFunctionType = NULL;
// SgFunctionDeclaration *targetFunctionDeclaration = NULL;

SgFunctionSymbol*
SimpleInstrumentation::buildNewFunctionDeclaration ( SgStatement* statementLocation, SgFunctionType* previousFunctionType )
   {
  // *****************************************************
  // Create the functionDeclaration
  // *****************************************************

  // Must mark the newly built node to be a part of a transformation so that it will be unparsed!
     Sg_File_Info * file_info = new Sg_File_Info();
     ROSE_ASSERT(file_info != NULL);
     file_info->set_isPartOfTransformation(true);

     SgName function_name = "contest_call";
     ROSE_ASSERT(previousFunctionType != NULL);
     SgFunctionDeclaration* functionDeclaration = new SgFunctionDeclaration(file_info, function_name, previousFunctionType);
     ROSE_ASSERT(functionDeclaration != NULL);
     ROSE_ASSERT(functionDeclaration->get_parameterList() != NULL);

  // ********************************************************************
  // Create the InitializedName for a parameter within the parameter list
  // ********************************************************************
     SgTypePtrList & argList = previousFunctionType->get_arguments();
     SgTypePtrList::iterator i = argList.begin();
     while ( i != argList.end() )
        {
          SgName var_name = "";
          SgInitializer* var_initializer = NULL;
          SgInitializedName *var_init_name = new SgInitializedName(var_name, *i, var_initializer, NULL);
          functionDeclaration->get_parameterList()->append_arg(var_init_name);
          i++;
        }

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
          case V_SgFunctionCallExp:
             {
               SgFunctionCallExp *functionCallExp = isSgFunctionCallExp(astNode);
               SgExpression *function = functionCallExp->get_function();
               ROSE_ASSERT(function);
               switch (function->variantT())
                  {
                    case V_SgFunctionRefExp:
                       {
                         SgFunctionRefExp *functionRefExp = isSgFunctionRefExp(function);
                         SgFunctionSymbol *symbol = functionRefExp->get_symbol();
                         ROSE_ASSERT(symbol != NULL);
                         SgFunctionDeclaration *functionDeclaration = symbol->get_declaration();
                         ROSE_ASSERT(functionDeclaration != NULL);
                         if (symbol == functionSymbol)
                            {
                           // Now we know that we have found the correct function call 
                           // (even in the presence of overloading or other forms of hidding)
                           // Now fixup the symbol and type of the SgFunctionRefExp object to 
                           // reflect the new function to be called (after this we still have to 
                           // fixup the argument list in the SgFunctionCallExp.

                           // We only want to build the decalration once (and insert it into the global scope)
                           // after that we save the symbol and reuse it.
                              if (newFunctionSymbol == NULL)
                                 {
                                   SgFunctionType* originalFunctionType = isSgFunctionType(functionSymbol->get_type());
                                   ROSE_ASSERT(originalFunctionType != NULL);
                                   newFunctionSymbol = buildNewFunctionDeclaration (TransformationSupport::getStatement(astNode),originalFunctionType);
                                 }

                              ROSE_ASSERT(newFunctionSymbol != NULL);
                              ROSE_ASSERT(newFunctionSymbol->get_type() != NULL);

                              functionRefExp->set_symbol(newFunctionSymbol);
                            }

                         break;
                       }
                    default:
                         cerr<<"warning: unrecognized variant: "<<function->class_name();
                  }
               break;
             }

          case V_SgFunctionDeclaration:
             {
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
               string functionName = functionDeclaration->get_name().str();
               if (functionName == "send")
                  {
                    SgFunctionType *functionType = functionDeclaration->get_type();
                    ROSE_ASSERT(functionType != NULL);
                    bool foundFunction = false;
                    if (functionType->get_return_type()->unparseToString() == "ssize_t")
                       {
                         SgTypePtrList & argumentList = functionType->get_arguments();
                         SgTypePtrList::iterator i = argumentList.begin();
                         if ( (*i++)->unparseToString() == "int" )
                              if ( (*i++)->unparseToString() == "const void *" )
                                   if ( (*i++)->unparseToString() == "size_t" )
                                        if ( (*i++)->unparseToString() == "int" )
                                             foundFunction = true;
                       }

                    if (foundFunction == true)
                       {
                      // Now get the sysmbol using functionType
                         SgScopeStatement *scope = functionDeclaration->get_scope();
                         ROSE_ASSERT(scope != NULL);
                         functionSymbol = scope->lookup_function_symbol (functionName,functionType);
                       }
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

