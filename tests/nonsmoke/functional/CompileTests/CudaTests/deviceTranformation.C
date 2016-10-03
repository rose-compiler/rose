// Example ROSE Translator used for testing ROSE infrastructure
#include "rose.h"


// Build an inherited attribute for the tree traversal to test the rewrite mechanism
class InheritedAttribute
   {
     public:
         SgFunctionCallExp* functionCallExp;

       // Specific constructors are required
          InheritedAttribute () {};
          InheritedAttribute ( const InheritedAttribute & X ) : functionCallExp(X.functionCallExp) {};
   };

class visitorTraversal : public AstTopDownProcessing<InheritedAttribute>
   {
     public:
       // virtual function must be defined
          virtual InheritedAttribute evaluateInheritedAttribute(SgNode* n, InheritedAttribute inheritedAttribute);
   };

InheritedAttribute
visitorTraversal::evaluateInheritedAttribute(SgNode* n, InheritedAttribute inheritedAttribute)
   {
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(n);
     if (functionCallExp != NULL)
        {
          inheritedAttribute.functionCallExp = functionCallExp;
        }

     SgLambdaExp* lambdaExp = isSgLambdaExp(n);
     if (lambdaExp != NULL)
        {
#if 0
          printf ("Found lambda expression. \n");
#endif
          SgExprListExp* exprListExp = isSgExprListExp(lambdaExp->get_parent());
          ROSE_ASSERT(exprListExp != NULL);

          if (inheritedAttribute.functionCallExp != NULL)
             {
               SgExpression* expression = inheritedAttribute.functionCallExp->get_function();
               SgFunctionRefExp*         functionRefExp         = isSgFunctionRefExp(expression);
               SgTemplateFunctionRefExp* templateFunctionRefExp = isSgTemplateFunctionRefExp(expression);

               SgFunctionDeclaration* functionDeclaration = NULL;

               if (functionRefExp != NULL)
                  {
                    SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(functionRefExp->get_symbol());
                    ROSE_ASSERT(functionSymbol != NULL);
                    functionDeclaration = functionSymbol->get_declaration();
                    ROSE_ASSERT(functionDeclaration != NULL);
                  }
                 else
                  {
                    if (templateFunctionRefExp != NULL)
                       {
#if 1
                         printf ("Error: We should not have this case: Not required to be handled! (focus only on template instantiations and normal functions) \n");
                         ROSE_ASSERT(false);
#endif
                       }
                      else
                       {
                      // I don't think we have to worry about this case.
#if 0
                         printf ("Not handled: expression = %p = %s \n",expression,expression->class_name().c_str());
#endif
                       }
                  }

               ROSE_ASSERT(functionDeclaration != NULL);

            // Find the function argument position of the SgLambdaExp in the exprListExp.
               size_t index_of_lambda_argument = 0;
               SgExpressionPtrList::iterator i = exprListExp->get_expressions().begin();
               while (i != exprListExp->get_expressions().end() && lambdaExp != *i)
                  {
#if 0
                    printf ("Looking for the next function call argument: *i = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
                    index_of_lambda_argument++;
                    i++;
                  }

            // SgFunctionParameterList* get_parameterList() 
#if 0
               printf ("index_of_lambda_argument = %zu \n",index_of_lambda_argument);
#endif
            // Find the function parameter position of the device marked SgInitializedName in the function parameter list.
               SgInitializedName* associatedFunctionParameter = functionDeclaration->get_parameterList()->get_args()[index_of_lambda_argument];
               if (associatedFunctionParameter->get_using_device_keyword() == true)
                  {
                    lambdaExp->set_is_device(true);
                  }
             }
            else
             {
               printf ("Error: We should have seen a function call expression at this point \n");
               ROSE_ASSERT(false);
             }

        }

     return inheritedAttribute;
   }


int main( int argc, char * argv[] )
   {
  // Generate the ROSE AST.
     SgProject* project = frontend(argc,argv);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute;

  // Build the traversal object
     visitorTraversal exampleTraversal;

  // Call the traversal starting at the project node of the AST
     exampleTraversal.traverseInputFiles(project,inheritedAttribute);

  // Or the traversal over all AST IR nodes can be called!
     exampleTraversal.traverse(project,inheritedAttribute);

#if 0
  // Output an optional graph of the AST (just the tree, when active)
     printf ("Generating a dot file... (ROSE Release Note: turn off output of dot files before committing code) \n");
     generateDOT ( *project );
  // generateAstGraph(project, 2000);
#endif

#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 8000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH);
#endif

  // regenerate the source code and call the vendor 
  // compiler, only backend error code is reported.
     return backend(project);
   }
