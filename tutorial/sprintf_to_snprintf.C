#include "rose.h"

// using namespace std;

int
main( int argc, char* argv[] )
   {
  // Initialize and check compatibility. See rose::initialize
     ROSE_INITIALIZE;

     SgProject* project = frontend(argc,argv);
     AstTests::runAllTests(project);

#if 0
  // Output the graph so that we can see the whole AST graph, for debugging.
     generateAstGraph(project, 4000);
#endif
#if 1
     printf ("Generate the dot output of the SAGE III AST \n");
     generateDOT ( *project );
     printf ("DONE: Generate the dot output of the SAGE III AST \n");
#endif

  // There are lots of way to write this, this is one simple approach; get all the function calls.
     std::vector<SgNode*> functionCalls = NodeQuery::querySubTree (project,V_SgFunctionCallExp);

  // Find the SgFunctionSymbol for snprintf so that we can reset references to "sprintf" to "snprintf" instead.
  // SgGlobal* globalScope = (*project)[0]->get_globalScope();
     SgSourceFile* sourceFile = isSgSourceFile(project->get_fileList()[0]);
     ROSE_ASSERT(sourceFile != NULL);
     SgGlobal* globalScope = sourceFile->get_globalScope();
     SgFunctionSymbol* snprintf_functionSymbol = globalScope->lookup_function_symbol("snprintf");
     ROSE_ASSERT(snprintf_functionSymbol != NULL);

  // Iterate over the function calls to find the calls to "sprintf"
     for (unsigned long i = 0; i < functionCalls.size(); i++)
        {
          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(functionCalls[i]);
          ROSE_ASSERT(functionCallExp != NULL);

          SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionCallExp->get_function());
          if (functionRefExp != NULL)
             {
               SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();
               if (functionSymbol != NULL)
                  {
                    SgName functionName = functionSymbol->get_name();
                 // printf ("Function being called: %s \n",functionName.str());
                    if (functionName == "sprintf")
                       {
                      // Now we have something to do!
                         functionRefExp->set_symbol(snprintf_functionSymbol);

                      // Now add the "n" argument
                         SgExprListExp* functionArguments = functionCallExp->get_args();
                         SgExpressionPtrList & functionArgumentList = functionArguments->get_expressions();

                      // "sprintf" shuld have exactly 2 arguments (I guess the "..." don't count)
                         printf ("functionArgumentList.size() = %zu \n",functionArgumentList.size());
                      // ROSE_ASSERT(functionArgumentList.size() == 2);
                         SgExpressionPtrList::iterator i = functionArgumentList.begin();

                      // printf ("(*i) = %p = %s = %s \n",*i,(*i)->class_name().c_str(),SageInterface::get_name(*i).c_str());
                         SgVarRefExp* variableRefExp = isSgVarRefExp(*i);
                         ROSE_ASSERT(variableRefExp != NULL);

                      // printf ("variableRefExp->get_type() = %p = %s = %s \n",variableRefExp->get_type(),variableRefExp->get_type()->class_name().c_str(),SageInterface::get_name(variableRefExp->get_type()).c_str());

                         SgType* bufferType = variableRefExp->get_type();
                         SgExpression* bufferLengthExpression = NULL;
                         switch(bufferType->variantT())
                            {
                              case V_SgArrayType:
                                 {
                                   SgArrayType* arrayType = isSgArrayType(bufferType);
                                   bufferLengthExpression = arrayType->get_index();
                                   break;
                                 }

                              case V_SgPointerType:
                                 {
                                // SgPointerType* pointerType = isSgPointerType(bufferType);
                                   SgInitializedName* variableDeclaration = variableRefExp->get_symbol()->get_declaration();
                                   ROSE_ASSERT(variableDeclaration != NULL);
                                   SgExpression* initializer = variableDeclaration->get_initializer();
                                   if (initializer != NULL)
                                      {
                                        SgAssignInitializer* assignmentInitializer = isSgAssignInitializer(initializer);
                                        ROSE_ASSERT(assignmentInitializer != NULL);

                                     // This is the rhs of the initialization of the pointer (likely a malloc through a cast).
                                     // This assumes: buffer = (char*) malloc(bufferLengthExpression);
                                        SgExpression* initializationExpression = assignmentInitializer->get_operand();
                                        ROSE_ASSERT(initializationExpression != NULL);
                                        SgCastExp* castExp = isSgCastExp(initializationExpression);
                                        ROSE_ASSERT(castExp != NULL);
                                        SgFunctionCallExp* functionCall = isSgFunctionCallExp(castExp->get_operand());
                                        ROSE_ASSERT(functionCall != NULL);
                                        SgExprListExp* functionArguments = isSgExprListExp(functionCall->get_args());
                                        bufferLengthExpression = functionArguments->get_expressions()[0];
                                        ROSE_ASSERT(bufferLengthExpression != NULL);
                                      }
                                     else
                                      {
                                        printf ("Initializer not found, so no value for n in snprintf can be computed currently \n");
                                      }
                                   break;
                                 }

                              default:
                                 {
                                   printf ("Error: default reached in evaluation of buffer type = %p = %s \n",bufferType,bufferType->class_name().c_str());
                                   ROSE_ASSERT(false);
                                 }
                            }

                         ROSE_ASSERT(bufferLengthExpression != NULL);

                      // printf ("bufferLengthExpression = %p = %s = %s \n",bufferLengthExpression,bufferLengthExpression->class_name().c_str(),SageInterface::get_name(bufferLengthExpression).c_str());

                      // Jump over the first argument, the "n" is defined to be the 2nd argument (the rest are shifted one position).
                         i++;

                      // Build a deep copy of the expression used to define the static buffer (could be any complex expression).
                         SgTreeCopy copy_help;
                         SgExpression* bufferLengthExpression_copy = isSgExpression(bufferLengthExpression->copy(copy_help));

                      // Insert the "n" for the parameter list to work with "snprintf" instead of "sprintf"
                         functionArgumentList.insert(i,bufferLengthExpression_copy);
                       }
                  }
             }
        }

     return backend(project);
   }
