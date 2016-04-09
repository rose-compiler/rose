// Example program to report global variables in lambda functions or anywhere in normal functions.

#include "rose.h"

// Inherited attribute (see ROSE Tutorial (Chapter 9)).
class InheritedAttribute
   {
     public:
          bool inLambdaFunction;
          bool parentConstraintSatisfied;
          InheritedAttribute();
   };

// Constructor (not really needed)
InheritedAttribute::InheritedAttribute()
   {
     inLambdaFunction          = false;
     parentConstraintSatisfied = false;
   }

// Synthesized attribute (see ROSE Tutorial (Chapter 9)).
class SynthesizedAttribute
   {
     public:
          SynthesizedAttribute();
   };

// Constructor
SynthesizedAttribute::SynthesizedAttribute()
   {
   }

class Traversal : public SgTopDownBottomUpProcessing<InheritedAttribute,SynthesizedAttribute>
   {
     public:
          typedef std::set<SgInitializedName*> globalVariableContainerType;
          globalVariableContainerType globalVariables;

          bool onlyInLambdaFunctions;

       // We need to keep track of all functions that we enter to count variables so that we 
       // don't report variable usage on the grainularity of the lambda fuinctions themselves.
       // We are calling these "outerFunctions" for now.
          typedef std::set<SgFunctionDeclaration*> outerFunctionsContainerType;
          outerFunctionsContainerType outerFunctions;

          Traversal();
          Traversal(bool input_onlyInLambdaFunctions);

       // Functions required
          InheritedAttribute   evaluateInheritedAttribute   ( SgNode* astNode, InheritedAttribute inheritedAttribute );
          SynthesizedAttribute evaluateSynthesizedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList );
   };


InheritedAttribute
Traversal::evaluateInheritedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute )
   {
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
     if (functionDeclaration != NULL && functionDeclaration->isCompilerGenerated() == true)
       {
      // We we only consider lambda functions, then we need the functionDeclaration.
         if (onlyInLambdaFunctions == false)
            {
              functionDeclaration = NULL;
            }
       }

     if (onlyInLambdaFunctions == false)
        {
       // We want to catch all global variable references in the outer function.
          if (inheritedAttribute.parentConstraintSatisfied == false && functionDeclaration != NULL)
             {
               inheritedAttribute.parentConstraintSatisfied = true;

            // Save the asociated function declaration
               SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
               outerFunctions.insert(functionDeclaration);
             }
        }
       else
        {
       // We want to catch only global variable references in the lambda functions for every outer function.
          if (inheritedAttribute.inLambdaFunction == false && inheritedAttribute.parentConstraintSatisfied == false && functionDeclaration != NULL)
             {
            // Save the asociated function declaration
               outerFunctions.insert(functionDeclaration);
             }

          if (inheritedAttribute.inLambdaFunction == false && inheritedAttribute.parentConstraintSatisfied == false && astNode->variantT() == V_SgLambdaExp)
             {
               inheritedAttribute.inLambdaFunction = true;
             }

          if (inheritedAttribute.inLambdaFunction == true && inheritedAttribute.parentConstraintSatisfied == false && functionDeclaration != NULL)
             {
               inheritedAttribute.parentConstraintSatisfied = true;
             }
        }

#if 0
     printf ("evaluateInheritedAttribute(): astNode = %p = %s inheritedAttribute.parentConstraintSatisfied = %s \n",
          astNode,astNode->class_name().c_str(),inheritedAttribute.parentConstraintSatisfied ? "true" : "false");
#endif

     if (inheritedAttribute.parentConstraintSatisfied == true)
        {
          SgVarRefExp* varRefExp = isSgVarRefExp(astNode);

          if (varRefExp != NULL)
             {
               SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
               ROSE_ASSERT(variableSymbol != NULL);
               SgInitializedName* initializedName = variableSymbol->get_declaration();
               ROSE_ASSERT(initializedName != NULL);

            // Save in the global variable list.
               globalVariables.insert(initializedName);
             }

       // Look for declarations of arrays and check the array bounds for global variables.
          SgInitializedName* initializedName = isSgInitializedName(astNode);
          if (initializedName != NULL)
             {
            // Need to look for cases of typedefed types.
               SgType* type = initializedName->get_type();
               ROSE_ASSERT(type != NULL);

               SgArrayType* arrayType = isSgArrayType(initializedName->get_type());
               if (arrayType != NULL)
                  {
                    SgExpression* indexExpression = arrayType->get_index();

                 // Use the same constraint
                    Traversal nestedTraversal(onlyInLambdaFunctions);

                 // Call the traversal starting at the project (root) node of the AST
                    nestedTraversal.traverse(indexExpression,inheritedAttribute);

                    if (nestedTraversal.globalVariables.empty() == false)
                       {
                      // Copy the globalVariables assocoated with the nestedTraversal of the
                      // array size declaration into the globalVariables of the current traversal.
                         globalVariables.insert(nestedTraversal.globalVariables.begin(),nestedTraversal.globalVariables.end());
                       }

                  }
             }
        }

  // This will call the default copy constructor for InheritedAttribute.
     return inheritedAttribute;
   }


SynthesizedAttribute
Traversal::evaluateSynthesizedAttribute ( SgNode* astNode, InheritedAttribute inheritedAttribute, SynthesizedAttributesList childAttributes )
   {
     SynthesizedAttribute localResult;

  // printf ("evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());

     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
     if (functionDeclaration != NULL && outerFunctions.find(functionDeclaration) != outerFunctions.end())
        {
       // Report the global variables referenced for the function here.
          if (globalVariables.empty() == false)
             {
               printf ("file: %s function: %s global variables: ",functionDeclaration->get_file_info()->get_filenameString().c_str(),functionDeclaration->get_name().str());
               for (globalVariableContainerType::iterator i = globalVariables.begin(); i != globalVariables.end(); i++)
                  {
                    SgInitializedName* initializedName = *i;
                    printf ("%s ",initializedName->get_name().str());
                  }
               printf ("\n");

            // Empty the set.
               globalVariables.clear();
             }
        }

     return localResult;
   }


Traversal::Traversal()
   {
     onlyInLambdaFunctions = false;
   }

Traversal::Traversal(bool input_onlyInLambdaFunctions)
   {
     onlyInLambdaFunctions = input_onlyInLambdaFunctions;
   }

int
main ( int argc, char* argv[] )
   {
  // Build the abstract syntax tree
     SgProject* project = frontend(argc,argv);
     ROSE_ASSERT (project != NULL);

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute;

  // Case we want to detect:
  //   1) Use of global variables in all lmbda functions in each function.
  //   2) Use of global variables in all functions (independent of use of lambda functions).
#if 0
     bool onlyInLambdaFunctions = false;
#else
     bool onlyInLambdaFunctions = true;
#endif

  // Define the traversal
     Traversal astTraversal(onlyInLambdaFunctions);

  // Call the traversal starting at the project (root) node of the AST
     astTraversal.traverseInputFiles(project,inheritedAttribute);

     return 0;
   }


