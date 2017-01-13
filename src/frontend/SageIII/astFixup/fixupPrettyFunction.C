#include "sage3basic.h"
#include "fixupPrettyFunction.h"

void fixupPrettyFunctionVariables ( SgNode* node )
   {
     TimingPerformance timer ("Fixup Pretty Print variables:");

  // This simplifies how the traversal is called!
     FixupPrettyFunctionVariables astFixupTraversal;

     FixupPrettyFunctionVariablesInheritedAttribute inheritedAttribute;

#if 1
     astFixupTraversal.traverse(node,inheritedAttribute);
#else
     printf ("fixupPrettyFunctionVariables traversal was SKIPPED \n");
#endif
   }

FixupPrettyFunctionVariablesInheritedAttribute::
FixupPrettyFunctionVariablesInheritedAttribute() : functionDeclaration(NULL)
   {
  // Default Constructor
   }

FixupPrettyFunctionVariablesInheritedAttribute::
FixupPrettyFunctionVariablesInheritedAttribute( const FixupPrettyFunctionVariablesInheritedAttribute & X )
   {
  // NOTE: This copy constructor is required to propegate the value of "functionDeclaration" member data across copies.
     functionDeclaration = X.functionDeclaration;
   }

FixupPrettyFunctionVariablesInheritedAttribute
FixupPrettyFunctionVariables::evaluateInheritedAttribute ( SgNode* node, FixupPrettyFunctionVariablesInheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("In FixupPrettyFunctionVariables::evaluateInheritedAttribute(): node = %p = %s \n",node,node->class_name().c_str());
#endif

  // We are looking for the variable "__assert_fail" which EDG has used instead of "__PRETTY_FUNCTION__".
  // An example from CPP is:
  // ((!"Inside of struct Y::foo1 (using assert)") ? static_cast<void> (0) : (__assert_fail ("!\"Inside of struct Y::foo1 (using assert)\"", "/home/dquinlan/ROSE/git-dq-main-rc/tests/nonsmoke/functional/CompileTests/Cxx_tests/test2010_07.C", 21, __PRETTY_FUNCTION__), static_cast<void> (0)));
  // But EDG will substitute "__PRETTY_FUNCTION__" with "__assert_fail", as is clearly documented in the EDG_3.3/Changes file (EDG ChangeLog).
  // So we have to back this out so that we can allow ROSE to generate the same code that GNU would (and other compilers).

  // Se we are looking for every function call.
     SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(node);
     if (functionCallExpression != NULL)
        {
       // Now we know that we are in a function call and we find the function declaration.
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(functionCallExpression->getAssociatedFunctionDeclaration());
       // ROSE_ASSERT(functionDeclaration != NULL);

          if (functionDeclaration != NULL)
             {
            // Set the function declaration in the inherited attribute.
               inheritedAttribute.functionDeclaration = functionDeclaration;
             }
        }

  // Then if we are in a function call (e.g. evaluating it's function arguments) the inheritect attribute will be set.
     if (inheritedAttribute.functionDeclaration != NULL)
        {
#if 0
          printf ("In FixupPrettyFunctionVariables::evaluateInheritedAttribute(): inside of function = %s \n",inheritedAttribute.functionDeclaration->get_name().str());
#endif
       // Now we are interested in finding variable names (in variable reference expressions).
          SgVarRefExp* variableReferenceExpression = isSgVarRefExp(node);
          if (variableReferenceExpression != NULL)
             {
               SgVariableSymbol*  variableSymbol  = isSgVariableSymbol(variableReferenceExpression->get_symbol());
               SgInitializedName* initializedName = variableSymbol->get_declaration();

#if 0
               printf ("In FixupPrettyFunctionVariables::evaluateInheritedAttribute(): variable = %s \n",initializedName->get_name().str());
#endif

               SgName functionName = inheritedAttribute.functionDeclaration->get_name();
            // if (initializedName != NULL && initializedName->get_file_info()->isCompilerGenerated() == true && initializedName->get_name() == "__PRETTY_FUNCTION__")
               if (initializedName != NULL && initializedName->get_file_info()->isCompilerGenerated() == true && initializedName->get_name() == functionName)
            // if (initializedName != NULL && initializedName->get_name() == functionName)
                  {
#if 0
                    printf ("Found EDG normalized name that is really supposed to be __PRETTY_FUNCTION__ \n");
#endif
                 // Now change the name to what is is supposed to be (before EDG normalized it).
                    initializedName->set_name("__PRETTY_FUNCTION__");
                  }
             }
        }

     return inheritedAttribute;
   }

