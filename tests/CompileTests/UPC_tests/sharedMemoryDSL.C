// Example ROSE Translator reads input program and implements a DSL embedded within UPC
// to support the "shared" keyword and a specific transformation to use a shared memory
// runtime support (developed seperately).
#include "rose.h"
#include "sharedMemoryDSL.h"

#include "sharedMemoryTypeTraversal.h"

InheritedAttribute::InheritedAttribute()
   {
  // It appears that I might not need this (at least for the simple cases).
   }

SynthesizedAttribute::SynthesizedAttribute()
   {
  // It appears that I might not need this (at least for the simple cases).
   }


std::vector<SgNode*> &
Traversal::get_nodeListWithTypesToModify()
   {
  // This is a simple access function.

     return nodeListWithTypesToModify;
   }


bool
Traversal::isSharedType(SgType* type)
   {
#if 0
     printf ("In Traversal::isSharedType(): type = %p = %s \n",type,type->class_name().c_str());
#endif

  // How complex can be expect the type system to be (do we required a nested type traversal).
     bool returnValue = false;
     SgPointerType* pointerType = isSgPointerType(type);

  // DQ (4/24/2014): This is the better (original) version of the code to use.
     if (pointerType != NULL)
        {
       // Check if the base type is marked as shared.
          SgModifierType* mod_type = isSgModifierType(pointerType->get_base_type());
          if (mod_type != NULL && mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
             {
               long block_size = mod_type->get_typeModifier().get_upcModifier().get_layout();

            // printf ("In Traversal::isSharedType(): Detected a shared type: block_size = %ld \n",block_size);
               returnValue = true;
             }
        }
       else
        {
       // DQ (4/26/2014): Added additional case as a result of fixing generated cases.
          SgModifierType* mod_type = isSgModifierType(type);
          if (mod_type != NULL && mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
             {
               long block_size = mod_type->get_typeModifier().get_upcModifier().get_layout();

            // printf ("In Traversal::isSharedType(): Detected a shared type: block_size = %ld \n",block_size);
               returnValue = true;
             }
        }

#if 0
     printf ("Leaving Traversal::isSharedType(): type = %p = %s returnValue = %s \n",type,type->class_name().c_str(),returnValue ? "true" : "false");
#endif

     return returnValue;
   }


void
Traversal::transformType(SgType* type)
   {
  // This function uses a traversal over the types, this is really rare to have to use this technique.
  // Its use here has caused a number of internal fixes in ROSE to allow this feature to work properly.

  // Build the inherited attribute
     TypeTraversalInheritedAttribute inheritedAttribute;

  // Define the traversal
     TypeTraversal sharedMemoryDSL_Traversal;

  // Call the traversal starting at the project (root) node of the AST
  // SynthesizedAttribute result = sharedMemoryDSL_Traversal.traverseWithinFile(project,inheritedAttribute);
     TypeTraversalSynthesizedAttribute result = sharedMemoryDSL_Traversal.traverse(type,inheritedAttribute);
   }


// SgExpression* Traversal::transformExpression(SgVarRefExp* varRefExp)
SgExpression*
Traversal::transformExpression(SgExpression* exp)
   {
     SgExpression* returnExp = NULL;

     SgGlobal*         globalScope          = SageInterface::getGlobalScope(exp);
     SgName            offset_variable_name = "MPISMOFFSET";
     SgVariableSymbol* variableSymbol       = globalScope->lookup_variable_symbol(offset_variable_name);
     if (variableSymbol == NULL)
        {
       // Build the variable in global scope.
          SgInitializer *varInit = NULL;
          SgVariableDeclaration* variableDeclaration = SageBuilder::buildVariableDeclaration(offset_variable_name,SageBuilder::buildUnsignedLongType(),varInit,globalScope);

       // Mark this as an extern variable.
          variableDeclaration->get_declarationModifier().get_storageModifier().setExtern();
#if 0
          printf ("In transformExpression(): Put the variableDeclaration = %p into the global scope \n",variableDeclaration);
#endif
          SageInterface::prependStatement(variableDeclaration,globalScope);

       // Now the symbol should exist.
          variableSymbol = globalScope->lookup_variable_symbol(offset_variable_name);
          ROSE_ASSERT(variableSymbol != NULL);
        }
     ROSE_ASSERT(variableSymbol != NULL);

#if 0
     printf ("In transformExpression(): exp                 = %p = %s = %s \n",exp,exp->class_name().c_str(),exp->unparseToString().c_str());
#endif

     SgCastExp*   char_cast_exp = SageBuilder::buildCastExp(exp,SageBuilder::buildPointerType(SageBuilder::buildCharType()));
     ROSE_ASSERT(char_cast_exp != NULL);

#if 0
     printf ("In transformExpression(): char_cast_exp->get_type() = %p = %s = %s \n",char_cast_exp->get_type(),char_cast_exp->get_type()->class_name().c_str(),char_cast_exp->get_type()->unparseToString().c_str());
#endif

     SgVarRefExp* offsetExp     = SageBuilder::buildVarRefExp(variableSymbol);
     ROSE_ASSERT(offsetExp != NULL);
     SgAddOp*     addExp        = SageBuilder::buildAddOp(char_cast_exp,offsetExp);
     ROSE_ASSERT(addExp != NULL);

#if 0
     printf ("In transformExpression(): addExp->get_type()  = %p = %s = %s \n",addExp->get_type(),addExp->get_type()->class_name().c_str(),addExp->get_type()->unparseToString().c_str());
     printf ("In transformExpression(): exp->get_type()     = %p = %s = %s \n",exp->get_type(),exp->get_type()->class_name().c_str(),exp->get_type()->unparseToString().c_str());
#endif

     SgPointerType* pointerType = isSgPointerType(exp->get_type());
     SgType* pointerBaseType = exp->get_type();
#if 0
     if (pointerType == NULL)
        {
          printf ("Error: pointerType == NULL: Need to handle that exp->get_type() is NOT a SgPointerType \n");
        }
  // ROSE_ASSERT(pointerType != NULL);

  // printf ("In transformExpression():    --- pointerType  = %p = %s = %s \n",pointerType,pointerType->class_name().c_str(),pointerType->unparseToString().c_str());
     printf ("In transformExpression():    --- pointerType  = %p = %s = %s \n",pointerBaseType,pointerBaseType->class_name().c_str(),pointerBaseType->unparseToString().c_str());
#endif

  // SgCastExp*   int_cast_exp  = SageBuilder::buildCastExp(addExp,SageBuilder::buildPointerType(pointerType->get_base_type()));
  // SgCastExp*   int_cast_exp  = SageBuilder::buildCastExp(addExp,SageBuilder::buildPointerType(pointerBaseType));
     SgCastExp*   int_cast_exp  = SageBuilder::buildCastExp(addExp,pointerBaseType);
     ROSE_ASSERT(int_cast_exp != NULL);

#if 0
     printf ("In transformExpression():    --- int_cast_exp = %p = %s = %s \n",int_cast_exp,int_cast_exp->class_name().c_str(),int_cast_exp->unparseToString().c_str());
#endif

     returnExp = int_cast_exp;

#if 0
     printf ("Leaving transformExpression(): - returnExp    = %p = %s = %s \n",returnExp,returnExp->class_name().c_str(),returnExp->unparseToString().c_str());
#endif

     return returnExp;
   }


InheritedAttribute
Traversal::evaluateInheritedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("In evaluateInheritedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

     SgInitializedName* initializedName = isSgInitializedName(astNode);
     if (initializedName != NULL)
        {
          SgType* type = initializedName->get_type();
          if (isSharedType(type) == true)
             {
            // Save the reference to the initializedName so that it can have it's type fixed up later.
               nodeListWithTypesToModify.push_back(astNode);
             }
        }

  // DQ (4/28/2014): Adding support for function return types (function parameters are handled via the SgInitializedName support).
     SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
     if (functionDeclaration != NULL)
        {
          ROSE_ASSERT(functionDeclaration->get_type() != NULL);
          SgType* type = functionDeclaration->get_type()->get_return_type();
          if (isSharedType(type) == true)
             {
            // Save the reference to the initializedName so that it can have it's type fixed up later.
               nodeListWithTypesToModify.push_back(astNode);
             }
        }

  // DQ (4/28/2014): Adding support for typedef base types.
     SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(astNode);
     if (typedefDeclaration != NULL)
        {
          ROSE_ASSERT(typedefDeclaration->get_type() != NULL);
          SgType* type = typedefDeclaration->get_base_type();
          if (isSharedType(type) == true)
             {
            // Save the reference to the initializedName so that it can have it's type fixed up later.
               nodeListWithTypesToModify.push_back(astNode);
             }
        }

  // Also need to investigate anything hiding types but which would NOT be traversed as part of the AST.
  // typedefs (base types), function parameters (handled in SgInitializedName case), function return types, 
  // template details (for C++), etc.

     return inheritedAttribute;
   }


SynthesizedAttribute
Traversal::evaluateSynthesizedAttribute (
     SgNode* astNode,
     InheritedAttribute inheritedAttribute,
     SynthesizedAttributesList childAttributes )
   {
#if 0
     printf ("In evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

     SynthesizedAttribute localResult;

  // We want to find and test all SgPointerDerefExp IR nodes.
     SgPointerDerefExp* pointerDerefExp = isSgPointerDerefExp(astNode);
     if (pointerDerefExp != NULL)
        {
          SgType* type = pointerDerefExp->get_type();

#if 0
          printf ("SgPointerDerefExp: type                           = %p = %s \n",type,type->class_name().c_str());
          printf ("SgPointerDerefExp: pointerDerefExp->get_operand() = %p = %s \n",pointerDerefExp->get_operand(),pointerDerefExp->get_operand()->class_name().c_str());
#endif

          SgVarRefExp*       varRefExp = isSgVarRefExp(pointerDerefExp->get_operand());
          SgDotExp*          dotExp    = isSgDotExp(pointerDerefExp->get_operand());
          SgArrowExp*        arrowExp  = isSgArrowExp(pointerDerefExp->get_operand());
          SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(pointerDerefExp->get_operand());

       // if (varRefExp != NULL)
       // if (varRefExp != NULL || dotExp != NULL || arrowExp != NULL)
          if (varRefExp != NULL || dotExp != NULL || arrowExp != NULL || functionCallExp != NULL)
             {
            // SgType* type = varRefExp->get_type();
               SgType* type = pointerDerefExp->get_operand()->get_type();
#if 0
               printf ("SgPointerDerefExp: (SgVarRefExp|SgDotExp|SgArrowExp|SgFunctionCallExp): pointerDerefExp->get_operand()->get_type(): type = %p = %s \n",type,type->class_name().c_str());
#endif
               if (isSharedType(type) == true)
                  {
                 // Found a reference to a variable that will we want to transform (post order traversal).

                 // The transformation we want is to introduce a new subtree using the varRefExp.
                 // SgExpression* newSubtree = transformExpression(varRefExp);
                    SgExpression* newSubtree = transformExpression(pointerDerefExp->get_operand());
                    ROSE_ASSERT(newSubtree != NULL);
#if 0
                    printf ("***** SgVarRefExp: pointerDerefExp = %p = %s newSubtree = %p = %s = %s \n",pointerDerefExp,pointerDerefExp->class_name().c_str(),newSubtree,newSubtree->class_name().c_str(),newSubtree->unparseToString().c_str());
#endif
                 // Set the new subtree into the SgPointerDerefExp's operand.
                    pointerDerefExp->set_operand(newSubtree);
                    newSubtree->set_parent(pointerDerefExp);
#if 0
                    printf ("***** SgVarRefExp: pointerDerefExp = %p = %s = %s \n",pointerDerefExp,pointerDerefExp->class_name().c_str(),pointerDerefExp->unparseToString().c_str());
#endif
                  }
             }
            else
             {
#if 0
               printf ("case of NOT a SgVarRefExp or SgDotExp or SgArrowExp \n");
#endif
            // SgPointerDerefExp* nested_pointerDerefExp = isSgPointerDerefExp(pointerDerefExp);
            // SgPointerDerefExp* nested_pointerDerefExp = isSgPointerDerefExp(type);
            // SgPointerDerefExp* nested_pointerDerefExp = NULL;
               SgPointerDerefExp* nested_pointerDerefExp = isSgPointerDerefExp(pointerDerefExp->get_operand());
               if (nested_pointerDerefExp != NULL)
                  {
                 // SgType* nested_type = nested_pointerDerefExp->get_type();
                 // SgType* nested_type = nested_pointerDerefExp->get_operand()->get_type();
                    SgType* nested_type = nested_pointerDerefExp->get_type();
#if 0
                    printf ("Nested: SgPointerDerefExp: nested_type                           = %p = %s \n",nested_type,nested_type->class_name().c_str());
#endif
                    SgExpression* dereferencedExpression = nested_pointerDerefExp->get_operand();
#if 0
                    printf ("Nested: SgPointerDerefExp: nested dereferencedExpression = %p = %s \n",dereferencedExpression,dereferencedExpression->class_name().c_str());
#endif
                    if (isSharedType(nested_type) == true)
                       {
                      // Found a reference to a variable that will we want to transform (post order traversal).
#if 0
                         printf ("Nested: Found a SgPointerDerefExp in SgPointerDerefExp with shared type: dereferencedExpression = %p (perform transformation) (not yet implemented) \n",dereferencedExpression);
#endif
                      // The transformation we want is to introduce a new subtree using the varRefExp.
                      // SgExpression* newSubtree = transformExpression(varRefExp);
                      // SgExpression* newSubtree = transformExpression(dereferencedExpression);
                         SgExpression* newSubtree = transformExpression(nested_pointerDerefExp);
                         ROSE_ASSERT(newSubtree != NULL);
#if 0
                         printf ("***** SgPointerDerefExp: nested_pointerDerefExp = %p = %s newSubtree = %p = %s \n",nested_pointerDerefExp,nested_pointerDerefExp->class_name().c_str(),newSubtree,newSubtree->class_name().c_str());
#endif
                      // Set the new subtree into the SgPointerDerefExp's operand.
                         pointerDerefExp->set_operand(newSubtree);
                         newSubtree->set_parent(pointerDerefExp);
#if 0
                         printf ("***** SgVarRefExp: nested_pointerDerefExp = %p = %s = %s \n",nested_pointerDerefExp,nested_pointerDerefExp->class_name().c_str(),nested_pointerDerefExp->unparseToString().c_str());
#endif
#if 0
                         printf ("Nested case of shared type expression that is not a SgVarRefExp is not implemented \n");
                         ROSE_ASSERT(false);
#endif
                       }
                      else
                       {
#if 1
                         printf ("case of NOT a shared type is not implemented \n");
                         ROSE_ASSERT(false);
#endif
                       }
                  }
                 else
                  {
#if 0
                    printf ("case of NOT a SgPointerDerefExp is not implemented \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
        }

#if 0
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
     if (functionCallExp != NULL)
        {
          SgType* type = functionCallExp->get_type();
          printf ("SgFunctionCallExp: type = %p = %s \n",type,type->class_name().c_str());
        }
#endif

  // DQ (4/29/2014): Kinds of IR nodes that can have references to typedefs to shared pointers.
     SgVarRefExp*       varRefExp = isSgVarRefExp(astNode);
     SgDotExp*          dotExp    = isSgDotExp(astNode);
     SgArrowExp*        arrowExp  = isSgArrowExp(astNode);
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);

     if (varRefExp != NULL)
        {
          SgType* type = varRefExp->get_type();
       // printf ("SgVarRefExp: type = %p = %s \n",type,type->class_name().c_str());
          SgTypedefType* typedefType = isSgTypedefType(type);
          if (typedefType != NULL)
             {
            // Found a typedef type, we have to check if this can be unwrapped to identify a shared pointer type.
               printf ("SgVarRefExp: Identified a SgTypedefType: need to implement investigation of internal shared type within typedef type \n");
             }
        }

  // Need to look at other kinds of expression that can hold shared types: e.g. SgArrayRefExp

     return localResult;
   }


void
fixupNodesWithTypes(SgProject* project,Traversal & traversal)
   {
  // This step does the transformations on types as a final step.

#if 0
     printf ("Transform the types in IR nodes where we detected shared types \n");
#endif

     std::vector<SgNode*> & nodeListWithTypesToModify = traversal.get_nodeListWithTypesToModify();

     std::vector<SgNode*>::iterator i = nodeListWithTypesToModify.begin();

  // Iterate over the IR nodes and transform the associated types.
     while (i != nodeListWithTypesToModify.end())
        {
#if 0
          printf ("In fixupNodesWithTypes(): in loop over IR nodes using types: i = %p = %s \n",*i,(*i)->class_name().c_str());
#endif
          SgInitializedName* initializedName = isSgInitializedName(*i);
          if (initializedName != NULL)
             {
            // This operation is working on shared types so the first modification will cause future uses to be hidden.
            // The problem with this solution is that we need to first mark all of the IR nodes that are using this type, 
            // before changing the type.  Alternatively we could record the type that was transformed so that it is 
            // transformed in the synthesized attribute evaluation.
               SgType* type = initializedName->get_type();

            // Note that this type might have been previously transformed, this is fine since it was shared.
               traversal.transformType(type);
             }

       // DQ (4/28/2014): Adding support for function return types (function parameters are handled via the SgInitializedName support).
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
          if (functionDeclaration != NULL)
             {
               ROSE_ASSERT(functionDeclaration->get_type() != NULL);
               SgType* type = functionDeclaration->get_type()->get_return_type();

            // Note that this type might have been previously transformed, this is fine since it was shared.
               traversal.transformType(type);
             }

       // DQ (4/28/2014): Adding support for typedef base types.
          SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(*i);
          if (typedefDeclaration != NULL)
             {
               ROSE_ASSERT(typedefDeclaration->get_type() != NULL);
               SgType* type = typedefDeclaration->get_base_type();

            // Note that this type might have been previously transformed, this is fine since it was shared.
               traversal.transformType(type);
             }

          i++;
        }
   }


int main( int argc, char * argv[] )
   {
  // Form the command line so that we can add some ROSE specific options to turn on UPC mode and skip the final compilation.
     Rose_STL_Container<std::string> argList = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);

  // Add UPC option so that ROSE will process the file as a UPC file.
  // We can add the option anywhere on the command line.
     argList.push_back("-rose:UPC");

  // This tool will only unparse the file and not compile it.
     argList.push_back("-rose:skipfinalCompileStep");

  // Generate the ROSE AST.
     SgProject* project = frontend(argList);
     ROSE_ASSERT(project != NULL);

#if 0
  // generateDOTforMultipleFile(*project);
     generateDOT(*project,"_before_transformation");
#endif
#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute;

  // Define the traversal
     Traversal sharedMemoryDSL_Traversal;

  // Call the traversal starting at the project (root) node of the AST
  // SynthesizedAttribute result = sharedMemoryDSL_Traversal.traverseWithinFile(project,inheritedAttribute);
     SynthesizedAttribute result = sharedMemoryDSL_Traversal.traverse(project,inheritedAttribute);

  // This is the compiler pass that will do the transformations on declarations of shared 
  // pointers and there associated expressions.
     fixupNodesWithTypes(project,sharedMemoryDSL_Traversal);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

#if 0
     printf ("Write out the DOT file after the transformation \n");
     generateDOTforMultipleFile(*project,"after_transformation");
     printf ("DONE: Write out the DOT file after the transformation \n");
#endif
#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

  // Regenerate the source code but skip the call the to the vendor compiler.
     return backend(project);
   }

