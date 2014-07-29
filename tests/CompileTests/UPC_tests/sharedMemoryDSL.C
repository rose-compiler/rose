// Example ROSE Translator reads input program and implements a DSL embedded within UPC
// to support the "shared" keyword and a specific transformation to use a shared memory
// runtime support (developed seperately).
#include "rose.h"
#include "sharedMemoryDSL.h"

// #include "cmdline.h"

#include "sharedMemoryTypeTraversal.h"

InheritedAttribute::InheritedAttribute()
   {
     isSharedTypeExpression = false;
     suppressTransformation = false;
     inFunctionCallArgumentList = false;
   }

void
InheritedAttribute::set_SharedTypeExpression( bool value )
   {
     isSharedTypeExpression = value;
   }

bool
InheritedAttribute::get_SharedTypeExpression()
   {
     return isSharedTypeExpression;
   }

void
InheritedAttribute::set_suppressTransformation( bool value )
   {
     suppressTransformation = value;
   }

bool
InheritedAttribute::get_suppressTransformation()
   {
     return suppressTransformation;
   }

void
InheritedAttribute::set_inFunctionCallArgumentList( bool value )
   {
     inFunctionCallArgumentList = value;
   }

bool
InheritedAttribute::get_inFunctionCallArgumentList()
   {
     return inFunctionCallArgumentList;
   }

InheritedAttribute::InheritedAttribute( const InheritedAttribute & X )
   {
     isSharedTypeExpression      = X.isSharedTypeExpression;
     suppressTransformation      = X.suppressTransformation;
     inFunctionCallArgumentList = X.inFunctionCallArgumentList;
   }



SynthesizedAttribute::SynthesizedAttribute()
   {
     isSharedTypeExpression    = false;
     isMarkedForTransformation = false;
     node                      = NULL;
   }

SynthesizedAttribute::SynthesizedAttribute( SgNode* n )
   {
     isSharedTypeExpression    = false;
     isMarkedForTransformation = false;
     node                      = n;
   }

SynthesizedAttribute::SynthesizedAttribute( const SynthesizedAttribute & X )
   {
     isSharedTypeExpression    = X.isSharedTypeExpression;
     isMarkedForTransformation = X.isMarkedForTransformation;
     node                      = X.node;
   }

void
SynthesizedAttribute::set_SharedTypeExpression( bool value )
   {
     isSharedTypeExpression = value;
   }

bool
SynthesizedAttribute::get_SharedTypeExpression()
   {
     return isSharedTypeExpression;
   }

void
SynthesizedAttribute::set_MarkedForTransformation(bool value)
   {
     isMarkedForTransformation = value;
   }

bool
SynthesizedAttribute::get_MarkedForTransformation()
   {
     return isMarkedForTransformation;
   }



std::vector<SgNode*> &
Traversal::get_nodeListWithTypesToModify()
   {
  // This is a simple access function.

     return nodeListWithTypesToModify;
   }


Traversal::Traversal()
   {
  // If we transformat any expression then we will save the required offset variable declaration 
  // and add it to the global scope last (to avoid changing the SgGlobal IR node during a traversal).
  // DQ (6/18/2014): This fixes a bug which was causing the first global scope statement to be transformed 
  // to be traversed a second time where that statement was not the last statement.  It was a strange bug.
     supportingOffset = NULL;
   }


#define DEBUG_IS_SHARED_TYPE 0

bool
Traversal::isSharedType(SgType* type)
   {
#if DEBUG_IS_SHARED_TYPE
     printf ("In Traversal::isSharedType(): type = %p = %s \n",type,type->class_name().c_str());
#endif

  // How complex can be expect the type system to be (do we required a nested type traversal).
     bool returnValue = false;
     SgPointerType* pointerType = isSgPointerType(type);

  // DQ (4/24/2014): This is the better (original) version of the code to use.
     if (pointerType != NULL)
        {
#if 0

#error "DEAD CODE"

       // Check if the base type is marked as shared.
          SgModifierType* mod_type = isSgModifierType(pointerType->get_base_type());
          if (mod_type != NULL)
             {
#if DEBUG_IS_SHARED_TYPE
               printf ("(pointerType != NULL): mod_type->get_typeModifier().get_upcModifier().get_isShared() = %s \n",mod_type->get_typeModifier().get_upcModifier().get_isShared() ? "true" : "false");
#endif
               if (mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
                  {
                    long block_size = mod_type->get_typeModifier().get_upcModifier().get_layout();

                 // printf ("In Traversal::isSharedType(): Detected a shared type: block_size = %ld \n",block_size);
                    returnValue = true;
                  }
                 else
                  {
                 // It appears that there can sometimes be a nested list of SgModifierType IR nodes (see test2014_24.c).
                    SgModifierType* nested_mod_type = isSgModifierType(mod_type->get_base_type());
                    if (nested_mod_type != NULL)
                       {
#if DEBUG_IS_SHARED_TYPE
                         printf ("(pointerType != NULL): nested_mod_type->get_typeModifier().get_upcModifier().get_isShared() = %s \n",nested_mod_type->get_typeModifier().get_upcModifier().get_isShared() ? "true" : "false");
#endif
                         if (nested_mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
                            {
                              returnValue = true;
                            }
                       }
                  }
             }
#else
       // DQ (6/8/2014): I think this makes for a more useful recursively defined implementation of this function.
          return isSharedType(pointerType->get_base_type());
#endif
        }
       else
        {
       // DQ (4/26/2014): Added additional case as a result of fixing generated cases.
          SgModifierType* mod_type = isSgModifierType(type);
          if (mod_type != NULL)
             {
#if DEBUG_IS_SHARED_TYPE
               printf ("(pointerType == NULL): mod_type->get_typeModifier().get_upcModifier().get_isShared() = %s \n",mod_type->get_typeModifier().get_upcModifier().get_isShared() ? "true" : "false");
#endif
               if (mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
                  {
                 // This variable is unused.
                 // long block_size = mod_type->get_typeModifier().get_upcModifier().get_layout();

                 // printf ("In Traversal::isSharedType(): Detected a shared type: block_size = %ld \n",block_size);
                    returnValue = true;
                  }
                 else
                  {
#if 0

#error "DEAD CODE"

                 // It appears that there can sometimes be a nested list of SgModifierType IR nodes (see test2014_24.c).
                    SgModifierType* nested_mod_type = isSgModifierType(mod_type->get_base_type());
                    if (nested_mod_type != NULL)
                       {
#if DEBUG_IS_SHARED_TYPE
                         printf ("(pointerType == NULL): nested_mod_type->get_typeModifier().get_upcModifier().get_isShared() = %s \n",nested_mod_type->get_typeModifier().get_upcModifier().get_isShared() ? "true" : "false");
#endif
                         if (nested_mod_type->get_typeModifier().get_upcModifier().get_isShared() == true)
                            {
                              returnValue = true;
                            }
                       }
#else
                 // DQ (6/8/2014): I think this makes for a more useful recursively defined implementation of this function.
                    return isSharedType(mod_type->get_base_type());
#endif
                  }
             }
            else
             {
               SgArrayType* arrayType = isSgArrayType(type);
               if (arrayType != NULL)
                  {
#if DEBUG_IS_SHARED_TYPE
                    printf ("Found a SgArrayType: arrayType = %p \n",arrayType);
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                    return isSharedType(arrayType->get_base_type());
                  }
             }
        }

#if DEBUG_IS_SHARED_TYPE
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
 
#if 0
     printf ("***** In transformExpression(): exp = %p = %s \n",exp,exp->class_name().c_str());
#endif

     SgGlobal*         globalScope          = SageInterface::getGlobalScope(exp);
     SgName            offset_variable_name = "MPISMOFFSET";
     SgVariableSymbol* variableSymbol       = globalScope->lookup_variable_symbol(offset_variable_name);
     if (variableSymbol == NULL)
        {
       // Build the variable in global scope.
          SgInitializer *varInit = NULL;

       // DQ (5/20/2014): The offset variable should be signed.
       // SgVariableDeclaration* variableDeclaration = SageBuilder::buildVariableDeclaration(offset_variable_name,SageBuilder::buildUnsignedLongType(),varInit,globalScope);
          SgVariableDeclaration* variableDeclaration = SageBuilder::buildVariableDeclaration(offset_variable_name,SageBuilder::buildLongType(),varInit,globalScope);

       // Mark this as an extern variable.
          variableDeclaration->get_declarationModifier().get_storageModifier().setExtern();
#if 0
          printf ("In transformExpression(): Put the variableDeclaration = %p into the global scope \n",variableDeclaration);
#endif

       // DQ (6/18/2014): This fixes a bug which was causing the first global scope statement to be transformed 
       // to be traversed a second time where that statement was not the last statement.  It was a strange bug.
       // DQ (6/18/2014): We have to add this offset variable as a last step 
       // since we are within a traversal of the global scope at this point.
       // SageInterface::prependStatement(variableDeclaration,globalScope);
          supportingOffset = variableDeclaration;

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

     SgType* pointerBaseType = exp->get_type();

#if 0
     SgPointerType* pointerType = isSgPointerType(exp->get_type());
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

  // DQ (7/22/2014): Save the reference to the initializedName so that it can have it's type fixed up later.
     nodeListWithTypesToModify.push_back(int_cast_exp);

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

            // DQ (7/22/2014): Mark the inheritedAttribute to supress transformations in this subtree 
            // (see test2014_59.c).  It might be that this only applies to initializations in variable 
            // declarations.
#if 0
               printf ("Mark inheritedAttribute to suppressTransformation (initializedName->get_name() = %s) \n",initializedName->get_name().str());
#endif
               inheritedAttribute.set_suppressTransformation(true);
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

  // DQ (5/24/2014): Added more general support for detecting expressions that need to be transformed.
     SgExpression* expression = isSgExpression(astNode);
     if (expression != NULL)
        {
          SgType* type = expression->get_type();
#if 0
          printf ("In evaluateInheritedAttribute(): found SgExpression: expression = %p = %s \n",expression,expression->class_name().c_str());
          printf ("In evaluateInheritedAttribute(): found SgExpression: type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
          bool skipTransformation = false;

       // This code has to preceed the code that will call the set_inFunctionCallArgumentList() 
       // (else it will refer to the same node in the AST, instead of a shild node in the AST traversal).
          if (inheritedAttribute.get_inFunctionCallArgumentList() == true)
             {
            // The rest of the subtree is not the expression in the function call argument list, so mark this as false.
               inheritedAttribute.set_inFunctionCallArgumentList(false);

            // Check the type of the associated argument against the functions types parameter type list.
               SgType* functionParameterType = SageInterface::getAssociatedTypeFromFunctionTypeList(expression);

               ROSE_ASSERT(functionParameterType != NULL);
#if 0
               printf ("functionParameterType = %p = %s = %s \n",functionParameterType,functionParameterType->class_name().c_str(),functionParameterType->unparseToString().c_str());
#endif
            // If this is a shared type in the function type then supress transformaions on shared type expression in the subtree.
               bool isShared = isSharedType(functionParameterType);

               if (isShared == true)
                  {
                 // DQ (7/22/2014): Mark the inheritedAttribute to supress transformations in this subtree 
                 // (see test2014_56.c).  This will supress transformations in the function argument subtrees.
#if 0
                    printf ("Detected shared type as formal parameter of associated actual argument expression \n");
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                    inheritedAttribute.set_suppressTransformation(true);
                  }
             }

       // DQ (7/22/2014): Check if this is a part of the arument list of a function call.
          SgExprListExp* exprListExp = isSgExprListExp(expression);
          if (exprListExp != NULL)
             {
               SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(exprListExp->get_parent());
               if (functionCallExp)
                  {
                    inheritedAttribute.set_inFunctionCallArgumentList(true);
                  }
             }

          SgBinaryOp* parentBinaryOp = isSgBinaryOp(expression->get_parent());
          SgArrowExp* parentArrowExp = isSgArrowExp(expression->get_parent());
          SgDotExp*   parentDotExp   = isSgDotExp(expression->get_parent());
       // if (parentArrowExp != NULL)
          if (parentArrowExp != NULL || parentDotExp != NULL)
             {
            // Check if the current expression is the lhs or rhs of the parentArrowExp.
               bool expressionIsLhs = (expression == parentBinaryOp->get_lhs_operand());
               bool expressionIsRhs = (expression == parentBinaryOp->get_rhs_operand());
               ROSE_ASSERT(expressionIsLhs == true || expressionIsRhs == true);
#if 0
               printf ("In evaluateInheritedAttribute(): SgArrowExp or SgDotExp: expressionIsLhs = %s expressionIsRhs = %s \n",expressionIsLhs ? "true" : "false",expressionIsRhs ? "true" : "false");
#endif
               skipTransformation = (expressionIsRhs == true);
             }
#if 0
          printf ("In evaluateInheritedAttribute(): SgArrowExp or SgDotExp: skipTransformation = %s \n",skipTransformation ? "true" : "false");
#endif
       // Reset the isSharedTypeExpression variable.
          inheritedAttribute.set_SharedTypeExpression(false);

#if 0
          printf ("In evaluateInheritedAttribute(): expression->get_type(): type = %p = %s \n",type,type->class_name().c_str());
#endif
          SgTypeDefault* defaultType = isSgTypeDefault(type);
          if (defaultType != NULL)
             {
#if 0
               printf ("Found a SgTypeDefault (likely a SgExprListExp) \n");
#endif
               if (isSgExprListExp(expression) == NULL)
                  {
#if 0
                    printf ("Warning: Expression returning SgTypeDefault, but not a SgExprListExp: expression = %p = %s \n",expression,expression->class_name().c_str());
#endif
                 // ROSE_ASSERT(false);
                  }
#if 0
               printf ("In evaluateInheritedAttribute(): Skip possible transformation of SgExprListExp: expression = %p = %s \n",expression,expression->class_name().c_str());
#endif
               skipTransformation = true;
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

          if (isSharedType(type) == true && skipTransformation == false)
             {
            // This might be a better way to identify expressions that should be transformed.
#if 0
               printf ("In evaluateInheritedAttribute(): found expression WITH shared type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
               SgPointerType* pointerType   = isSgPointerType(type);
               SgModifierType* modifierType = isSgModifierType(type);

               if (modifierType != NULL)
                  {
                    SgType* base_type = modifierType->get_base_type();
                    ROSE_ASSERT(base_type != NULL);
#if 0
                    printf ("found SgModifierType: base_type = %p = %s = %s \n",base_type,base_type->class_name().c_str(),base_type->unparseToString().c_str());
#endif
                    SgPointerType* pointerType = isSgPointerType(base_type);
                    if (pointerType != NULL)
                       {
#if 0
                         printf ("Found SgModifierType: base_type is a SgPointerType \n");
#endif
                       }
                      else
                       {
                      // DQ (6/8/2014): We have to reach one level deeper to search for the pointer type below a chain of length two (of SgModifierType IR nodes).
                         SgModifierType* nested_modifierType = isSgModifierType(base_type);
                         if (nested_modifierType != NULL)
                            {
#if 0
                              printf ("Found SgModifierType: nested_modifierType is a SgModifierType \n");
#endif
                              SgType* nested_base_type = nested_modifierType->get_base_type();
                              ROSE_ASSERT(nested_base_type != NULL);

                              SgPointerType* nested_pointerType = isSgPointerType(nested_base_type);
                              if (nested_pointerType != NULL)
                                 {
#if 0
                                   printf ("Found SgModifierType: nested_base_type is a SgPointerType \n");
#endif
                                 }
                                else
                                 {
                                   modifierType = NULL;
                                 }
                            }
                           else
                            {
                           // DQ (7/22/2014): Since this is not a pointer (or hiding a pointer), there is no associated transformation to be done.
#if 0
                              printf ("The base_type is not a SgPointerType or a SgModifierType: set modifierType = NULL \n");
#endif
                              modifierType = NULL;
                            }
                       }
                  }

            // if (pointerType != NULL)
               if (pointerType != NULL || modifierType != NULL)
                  {
#if 0
                    printf ("@@@@@ In evaluateInheritedAttribute(): Found a shared pointer type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
                    inheritedAttribute.set_SharedTypeExpression(true);
                  }
             }
            else
             {
#if 0
               printf ("In evaluateInheritedAttribute(): found expression WITHOUT shared type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
             }
        }

#if 0
     printf ("Leaving evaluateInheritedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
     printf ("   --- inheritedAttribute.get_SharedTypeExpression()       = %s \n",inheritedAttribute.get_SharedTypeExpression()       ? "true" : "false");
     printf ("   --- inheritedAttribute.get_suppressTransformation()     = %s \n",inheritedAttribute.get_suppressTransformation()     ? "true" : "false");
     printf ("   --- inheritedAttribute.get_inFunctionCallArgumentList() = %s \n",inheritedAttribute.get_inFunctionCallArgumentList() ? "true" : "false");
#endif

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
     printf ("\nIn evaluateSynthesizedAttribute(): astNode = %p = %s inheritedAttribute.get_suppressTransformation() = %s \n",astNode,astNode->class_name().c_str(),inheritedAttribute.get_suppressTransformation() ? "true" : "false");
#endif

     SynthesizedAttribute localResult(astNode);

#if 0
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
     if (functionCallExp != NULL)
        {
          SgType* type = functionCallExp->get_type();
          printf ("SgFunctionCallExp: type = %p = %s \n",type,type->class_name().c_str());
        }
#endif

     if (inheritedAttribute.get_suppressTransformation() == true)
        {
#if 0
          printf ("!!!!!!!!!! inheritedAttribute.get_suppressTransformation() == true: short circute the evaluation of the SynthesizedAttribute \n");
#endif
          return localResult;
        }

  // DQ (6/18/2014): This fixes a bug which was causing the first global scope statement to be transformed 
  // to be traversed a second time where that statement was not the last statement.  It was a strange bug.
     SgGlobal* globalScope = isSgGlobal(astNode);
     if (globalScope != NULL)
        {
          if (supportingOffset != NULL)
             {
#if 0
               printf ("Add required offset variable declaration to the global scope \n");
#endif
               SageInterface::prependStatement(supportingOffset,globalScope);
             }
        }

  // DQ (5/24/2014): This is the newer version of the logic to control where expresion are transformed.
  // This version makes better use of the inherited attribute and uses it to set the synthesized 
  // attribute.  Then we seperately look at the child attributes and use the marking in the child 
  // synthesized attribute to trigger the transformations.
     bool isSharedTypeExpression = inheritedAttribute.get_SharedTypeExpression();
     SgExpression* expression = isSgExpression(astNode);
     if (expression != NULL)
        {
#if 0
          printf ("In evaluateSynthesizedAttribute(): expression = %p = %s isSharedTypeExpression = %s \n",expression,expression->class_name().c_str(),isSharedTypeExpression ? "true" : "false");
#endif
          if (isSharedTypeExpression == true)
             {
#if 0
               printf ("In evaluateSynthesizedAttribute(): mark as shared type expression = %p = %s lvalue = %s \n",expression,expression->class_name().c_str(),expression->get_lvalue() ? "true" : "false");
#endif
               localResult.set_SharedTypeExpression(true);
               if (expression->get_lvalue() == false)
                  {
#if 0
                    printf ("&&&&& In evaluateSynthesizedAttribute(): transform expression = %p = %s lvalue = %s \n",expression,expression->class_name().c_str(),expression->get_lvalue() ? "true" : "false");
#endif
                    localResult.set_MarkedForTransformation(true);
                  }
             }
        }

  // Need to look at other kinds of expression that can hold shared types: e.g. SgArrayRefExp

     bool skip_transformation_of_children = false;

     SgAssignOp* assignOp = isSgAssignOp(astNode);
     if (assignOp != NULL)
        {
#if 0
          printf ("Found SgAssignOp: check for matching transformation of lhs and rhs expressions \n");
#endif
          for (SynthesizedAttributesList::iterator i = childAttributes.begin(); i != childAttributes.end(); i++)
             {
               ROSE_ASSERT((*i).node != NULL);
               SgExpression* child_expression = isSgExpression((*i).node);
               if (child_expression != NULL)
                  {
#if 0
                    printf ("Evaluate matching of types on the child nodes of the AST (child = %p = %s) shared type = %s lvalue = %s marked for transformation = %s \n",
                       child_expression,child_expression->class_name().c_str(),(*i).get_SharedTypeExpression() ? "true" : "false",
                       child_expression->get_lvalue() ? "true" : "false",(*i).get_MarkedForTransformation() ? "true" : "false");
#endif
                    if ( (*i).get_SharedTypeExpression() == true )
                       {
                         if (child_expression->get_lvalue() == true)
                            {
                              if ( (*i).get_MarkedForTransformation() == false )
                                 {
#if 0
                                   printf ("This SgAssignOp has shared type lhs expression not marked for transformation (skip transformation of rhs to match types) \n");
#endif
                                   skip_transformation_of_children = true;
#if 0
                                   printf ("Exiting as a test! \n");
                                   ROSE_ASSERT(false);
#endif
                                 }
                            }
                       }
                  }
             }
#if 0
          printf ("skip_transformation_of_children = %s \n",skip_transformation_of_children ? "true" : "false");
#endif

#if 0
          if (skip_transformation_of_children == true)
             {
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
             }
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // DQ (6/14/2014): We need to suppress the transformations on the operands of a SgCastExp, 
  // so that it will be done on the SgCastExp directly instead (see test2014_50.c).
     SgCastExp* castExp = isSgCastExp(astNode);
     if (castExp != NULL)
        {
#if 0
          printf ("Found SgCastExp: Suppressed transformation on children of SgCastExp: castExp = %p castExp->get_type() = %p = %s = %s \n",
               castExp,castExp->get_type(),castExp->get_type()->class_name().c_str(),castExp->get_type()->unparseToString().c_str());
#endif
#if 0
#error "DEAD CODE!"
          skip_transformation_of_children = true;
#else
          SgType* cast_to_type   = castExp->get_type();
          ROSE_ASSERT(cast_to_type != NULL);
          SgType* cast_from_type = castExp->get_operand()->get_type();
          ROSE_ASSERT(cast_from_type != NULL);

          bool cast_to_type_is_shared   = isSharedType(cast_to_type);
          bool cast_from_type_is_shared = isSharedType(cast_from_type);
#if 0
          printf ("cast_to_type_is_shared   = %s \n",cast_to_type_is_shared   ? "true" : "false");
          printf ("cast_from_type_is_shared = %s \n",cast_from_type_is_shared ? "true" : "false");
#endif
          if ( (cast_from_type_is_shared == true) && (cast_to_type_is_shared == false) )
             {
#if 0
               printf ("types do NOT match in sharing \n");
#endif
             }
            else
             {
#if 0
               printf ("types DO match in sharing \n");
#endif
               skip_transformation_of_children = true;
             }
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
  // DQ (7/22/2014): Start of commented out section for initializers and binary expressions (now better handled with inherited attribute).

#error "DEAD CODE!"

  // DQ (7/16/2014): We need to suppress the transformations on the NULL pointer initializers (see test2014_55.c).
  // DQ (7/22/2014): The problem is that we need to supress the transformation deepenr into the sub-tree of the initializer, 
  // so this is not an effective approach.  Test code test2014_59.c demonstrates this where the expression to be 
  // transformed (or supressed) is hidden behind a SgAddressOf Operator and so we can't support the supression 
  // in the evaluateSynthesizedAttribute() function.  This must be supported using the InheritedAttribute instead.
  // As a result this code might not be required.
     SgInitializer* initializer = isSgInitializer(astNode);
     if (initializer != NULL)
        {
#if 0
          printf ("Found SgInitializer: Suppressed transformation on children of SgInitializer \n");
#endif
       // Detect cases of initialization using NULL literal.
          SgAssignInitializer* assignInitializer = isSgAssignInitializer(initializer);
          if (assignInitializer != NULL)
             {
               SgExpression* expression = assignInitializer->get_operand();
               ROSE_ASSERT(expression != NULL);

            // Iterate over any possible cast expressions (common in pointer initializations).
               while (isSgCastExp(expression) != NULL)
                  {
#if 0
                    printf ("Found cast in SgAssignInitializer: expression = %p \n",expression);
#endif
                    SgCastExp* castExp = isSgCastExp(expression);
                    expression = castExp->get_operand();
                  }
               ROSE_ASSERT(expression != NULL);
#if 0
               printf ("Found expression (past possible cast): in SgAssignInitializer: expression = %p = %s \n",expression,expression->class_name().c_str());
#endif
               SgIntVal* intValue = isSgIntVal(expression);
               if (intValue != NULL)
                  {
                    skip_transformation_of_children = (intValue->get_value() == 0);
#if 0
                    printf ("Found SgIntVal initializer: skip_transformation_of_children = %s intValue->get_value() = %d \n",skip_transformation_of_children ? "true" : "false",intValue->get_value());
#endif
                  }
                 else
                  {
                    SgLongIntVal* longValue = isSgLongIntVal(expression);
                    if (longValue != NULL)
                       {
                         skip_transformation_of_children = (longValue->get_value() == 0);
#if 0
                         printf ("Found SgLongIntVal initializer: skip_transformation_of_children = %s intValue->get_value() = %zu \n",skip_transformation_of_children ? "true" : "false",longValue->get_value());
#endif
                       }
                      else
                       {
                      // DQ (7/18/2014): Using this a a temporary test (test2014_57.c).
                         skip_transformation_of_children = true;
#if 0
                         printf ("Found a non SgIntVal or SgLongIntVal initializer: skip_transformation_of_children = %s \n",skip_transformation_of_children ? "true" : "false");
#endif
                       }
                  }
             }
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

#error "DEAD CODE!"

#if 0
     printf ("skip_transformation_of_children = %s \n",skip_transformation_of_children ? "true" : "false");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#error "DEAD CODE!"

  // DQ (7/22/2014): End of commented out section for initializers expressions (now better handled with inherited attribute).
#endif

#if 0
  // DQ (7/22/2014): Start of commented out section for binary expressions (now better handled with inherited attribute).

  // DQ (7/16/2014): We need to suppress the transformations on the NULL pointer initializers (see test2014_55.c).
     SgBinaryOp* binaryOp = isSgBinaryOp(astNode);
     if (binaryOp != NULL)
        {
       // Check the lhs and rhs and if they are both shared then suppress the transformation on children.
#if 0
          printf ("Found SgBinaryOp: check to suppresse transformation on children of SgBinaryOp \n");
#endif
          bool lhs_expression_is_shared_type = (isSharedType(binaryOp->get_lhs_operand()->get_type()) == true);
          bool rhs_expression_is_shared_type = (isSharedType(binaryOp->get_rhs_operand()->get_type()) == true);

// #error "DEAD CODE!"

          printf ("lhs_expression_is_shared_type = %s rhs_expression_is_shared_type = %s \n",lhs_expression_is_shared_type ? "true" : "false",rhs_expression_is_shared_type ? "true" : "false");

          if (lhs_expression_is_shared_type && rhs_expression_is_shared_type)
             {
#if 0
               printf ("Found SgBinaryOp: Suppress transformation on children of SgBinaryOp = %p = %s \n",binaryOp,binaryOp->class_name().c_str());
#endif
               skip_transformation_of_children = true;
             }
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

// #error "DEAD CODE!"

#if 0
     printf ("skip_transformation_of_children = %s \n",skip_transformation_of_children ? "true" : "false");
#endif

// #error "DEAD CODE!"

  // DQ (7/22/2014): End of commented out section for initializers and binary expressions (now better handled with inherited attribute).
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

#if 0
  // DQ (6/14/2014): We need to suppress the transformations on the operands of a SgCastExp, 
  // so that it will be done on the SgCastExp directly instead (see test2014_51.c).
     SgExprListExp* exprListExp = isSgExprListExp(astNode);
     if (exprListExp != NULL)
        {
#if 0
          printf ("Found SgExprListExp: Suppressed transformation on children of SgExprListExp \n");
#endif
#if 0
          skip_transformation_of_children = true;
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

#if 0
     printf ("skip_transformation_of_children = %s \n",skip_transformation_of_children ? "true" : "false");
#endif

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     size_t child_index = 0;

  // DQ (5/25/2014): Iterate over the child attributes and determin if any transformations are required to the AST.
     for (SynthesizedAttributesList::iterator i = childAttributes.begin(); i != childAttributes.end(); i++)
        {
#if 0
           if ((*i).node != NULL)
              {
                printf ("--- TOP OF LOOP: synthesized attribute: (child = %p = %s) (*i).get_MarkedForTransformation() = %s \n",(*i).node,(*i).node->class_name().c_str(),(*i).get_MarkedForTransformation() ? "true" : "false");
              }
             else
              {
                printf ("--- TOP OF LOOP: synthesized attribute: (child = NULL) (*i).get_MarkedForTransformation() = %s \n",(*i).get_MarkedForTransformation() ? "true" : "false");
              }
#endif
       // if ( (*i).get_SharedTypeExpression() == true )
          if ( (*i).get_MarkedForTransformation() == true )
             {
            // This is where we have to call for the transformation on the child node of the AST.

               ROSE_ASSERT((*i).node != NULL);
#if 0
               printf ("--- synthesized attribute: This is where we have to call for the transformation on the child nodes of the AST (child = %p = %s) \n",(*i).node,(*i).node->class_name().c_str());
#endif
               SgExpression* expression_to_transform = isSgExpression((*i).node);
               SgExpression* parentExpression = isSgExpression(expression_to_transform->get_parent());
#if 0
               printf ("--- synthesized attribute: parentExpression = %p \n",parentExpression);
#endif
            // DQ (5/25/2014): We don't have to 
            // ROSE_ASSERT(parentExpression != NULL);
               if (parentExpression != NULL)
                  {
#if 0
                    printf ("--- parentExpression = %p = %s skip_transformation_of_children = %s \n",parentExpression,parentExpression->class_name().c_str(),skip_transformation_of_children ? "true" : "false");
#endif
                 // DQ (6/18/2014): We have to match the expression with the function parameter type.
                    SgExprListExp* exprListExp = isSgExprListExp(astNode);
                    if (exprListExp != NULL)
                       {
                         SgExpressionPtrList & exprList   = exprListExp->get_expressions();
                         SgExpression* indexed_expression = exprList[child_index];
                         ROSE_ASSERT(indexed_expression != NULL);
                         ROSE_ASSERT(exprListExp->get_parent() != NULL);
#if 0
                         printf ("--- SgExprListExp = %p child_index = %zu indexed_expression = %p = %s \n",exprListExp,child_index,indexed_expression,indexed_expression->class_name().c_str());
                         printf ("--- exprListExp->get_parent() = %p = %s \n",exprListExp->get_parent(),exprListExp->get_parent()->class_name().c_str());
#endif
                      // Check if the expression and the type in the SgExprListExp match.
                         SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(exprListExp->get_parent());
                         if (functionCallExp != NULL)
                            {
#if 0
                              printf ("--- SgExprListExp IS associated with a SgFunctionCallExp = %p \n",functionCallExp);
#endif
                              SgExpression* tmp_exp = functionCallExp->get_function();
                              ROSE_ASSERT(tmp_exp != NULL);
                              SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(tmp_exp);
                              ROSE_ASSERT(functionRefExp != NULL);
                              SgType* tmp_type = functionRefExp->get_type();
                              SgFunctionType* functionType = isSgFunctionType(tmp_type);
                              ROSE_ASSERT(functionType != NULL);

                              SgTypePtrList & typeList = functionType->get_arguments();

                              SgType* indexed_expression_type = indexed_expression->get_type();
                              ROSE_ASSERT(indexed_expression_type != NULL);

                              SgType* indexed_function_parameter_type = typeList[child_index];
                              ROSE_ASSERT(indexed_function_parameter_type != NULL);

                              bool indexed_function_parameter_type_is_shared = isSharedType(indexed_function_parameter_type);
                              bool indexed_expression_type_is_shared         = isSharedType(indexed_expression_type);
#if 0
                              printf ("--- indexed_function_parameter_type_is_shared = %s \n",indexed_function_parameter_type_is_shared ? "true" : "false");
                              printf ("--- indexed_expression_type_is_shared         = %s \n",indexed_expression_type_is_shared ? "true" : "false");
#endif
                              if ( (indexed_expression_type_is_shared == true) && (indexed_function_parameter_type_is_shared == false) )
                                 {
#if 0
                                   printf ("--- types do NOT match in sharing (set skip_transformation_of_children = false) \n");
#endif
                                   skip_transformation_of_children = false;
                                 }
                                else
                                 {
#if 0
                                   printf ("--- types DO match in sharing (set skip_transformation_of_children = true) \n");
#endif
                                   skip_transformation_of_children = true;
                                 }
#if 0
                              printf ("Exiting as a test! \n");
                              ROSE_ASSERT(false);
#endif
                            }
                           else
                            {
                           // There are other uses of a SgExprListExp in the AST, this is one of them.
                              printf ("--- SgExprListExp is NOT associated with a SgFunctionCallExp \n");
                            }
                       }

                    if (skip_transformation_of_children == false)
                       {
                      // DQ (6/17/2014): This function is causing the traversal to be called twice.
                         SgExpression* newSubtree = transformExpression(expression_to_transform);
                         ROSE_ASSERT(newSubtree != NULL);
#if 0
                         printf ("--- synthesized attribute: ***** expression_to_transform = %p = %s BEFORE: parentExpression = %p = %s newSubtree = %p = %s = %s \n",
                              expression_to_transform,expression_to_transform->class_name().c_str(),
                              parentExpression,parentExpression->class_name().c_str(),newSubtree,newSubtree->class_name().c_str(),newSubtree->unparseToString().c_str());
#endif
                      // Set the new subtree into the SgPointerDerefExp's operand.
                         parentExpression->replace_expression(expression_to_transform,newSubtree);

                      // DQ (5/20/2014): We can sometimes have to set the lvalue flag.
                         newSubtree->set_lvalue(expression_to_transform->get_lvalue());
                         expression_to_transform->set_lvalue(false);
#if 0
                         printf ("--- synthesized attribute: BEFORE: Reset the parent \n");
#endif
                      // pointerDerefExp->set_operand(newSubtree);
                      // newSubtree->set_parent(pointerDerefExp);
                         newSubtree->set_parent(parentExpression);
#if 0
                         printf ("--- synthesized attribute: AFTER: Reset the parent parentExpression = %p \n",parentExpression);
#endif
#if 0
                         printf ("--- synthesized attribute: ***** expression_to_transform = %p = %s AFTER: parentExpression = %p = %s = %s \n",
                              expression_to_transform,expression_to_transform->class_name().c_str(),
                              parentExpression,parentExpression->class_name().c_str(),parentExpression->unparseToString().c_str());
#endif
                       }
                      else
                       {
#if 0
                         printf ("--- Skipping the possible transformation: skip_transformation_of_children == true \n");
#endif
                       }
                  }
                 else
                  {
#if 0
                    printf ("--- Skipping the possible transformation: parentExpression == NULL \n");
#endif
                  }
#if 0
                printf ("Exiting as a test! \n");
                ROSE_ASSERT(false);
#endif
             }

#if 0
          if ((*i).node != NULL)
             {
               printf ("--- END OF LOOP: synthesized attributes: This is where we have to call for the transformation on the child nodes of the AST (child = %p = %s) \n",(*i).node,(*i).node->class_name().c_str());
             }
            else
             {
               printf ("--- END OF LOOP: synthesized attribute: (child = NULL) (*i).get_MarkedForTransformation() = %s \n",(*i).get_MarkedForTransformation() ? "true" : "false");
             }
#endif
          child_index++;
        }

#if 0
     printf ("Leaving evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
     printf ("   --- localResult.get_SharedTypeExpression()    = %s \n",localResult.get_SharedTypeExpression() ? "true" : "false");
     printf ("   --- localResult.get_MarkedForTransformation() = %s \n\n",localResult.get_MarkedForTransformation() ? "true" : "false");
#endif

     return localResult;
   }


void
fixupNodesWithTypes(SgProject* project,Traversal & traversal)
   {
  // This step does the transformations on types as a final step.
  // It must be done last because we use the marking of types as shared
  // to first do all of the transformations of relevant expressions.

     std::vector<SgNode*> & nodeListWithTypesToModify = traversal.get_nodeListWithTypesToModify();

#if 0
     printf ("Transform the types in IR nodes where we detected shared types: nodeListWithTypesToModify size = %zu \n",nodeListWithTypesToModify.size());
#endif

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
#if 0
               printf ("SgInitializedName: Calling traversal.transformType(type = %p = %s = %s) \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
            // Note that this type might have been previously transformed, this is fine since it was shared.
               traversal.transformType(type);
             }

       // DQ (4/28/2014): Adding support for function return types (function parameters are handled via the SgInitializedName support).
          SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(*i);
          if (functionDeclaration != NULL)
             {
               ROSE_ASSERT(functionDeclaration->get_type() != NULL);
               SgType* type = functionDeclaration->get_type()->get_return_type();
#if 0
               printf ("SgFunctionDeclaration: Calling traversal.transformType(type = %p = %s = %s) \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
            // Note that this type might have been previously transformed, this is fine since it was shared.
               traversal.transformType(type);
             }

       // DQ (4/28/2014): Adding support for typedef base types.
          SgTypedefDeclaration* typedefDeclaration = isSgTypedefDeclaration(*i);
          if (typedefDeclaration != NULL)
             {
               ROSE_ASSERT(typedefDeclaration->get_type() != NULL);
               SgType* type = typedefDeclaration->get_base_type();
#if 0
               printf ("SgTypedefDeclaration: Calling traversal.transformType(type = %p = %s = %s) \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
            // Note that this type might have been previously transformed, this is fine since it was shared.
               traversal.transformType(type);
             }

       // DQ (7/22/2014): Added support for types hidden in the SgCastExp.
          SgCastExp* castExp = isSgCastExp(*i);
          if (castExp)
             {
            // DQ (7/22/2014): Save the reference to the initializedName so that it can have it's type fixed up later.
            // nodeListWithTypesToModify.push_back(int_cast_exp);
               SgType* type = castExp->get_type();
#if 0
               printf ("SgCastExp: Calling traversal.transformType(type = %p = %s = %s) \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
            // Note that this type might have been previously transformed, this is fine since it was shared.
               traversal.transformType(type);
             }

          i++;
        }
   }


#define DEBUG_USING_DOT_GRAPHS 0

int main( int argc, char * argv[] )
   {
  // Form the command line so that we can add some ROSE specific options to turn on UPC mode and skip the final compilation.
     Rose_STL_Container<std::string> argList = CommandlineProcessing::generateArgListFromArgcArgv (argc,argv);

  // DQ (5/16/2014): Added option to permit optional compilation of generated code using C compiler.
     bool testGeneratedSourceCode = false;
     if ( CommandlineProcessing::isOption(argList,"-","(testCompile)",true) == true )
        {
#if 0
          printf ("Note: Testing generated code by compiling it with C compiler \n");
#endif
          testGeneratedSourceCode = true;
        }

#if 0
     printf ("testGeneratedSourceCode = %s \n",testGeneratedSourceCode ? "true" : "false");
#endif

  // Add UPC option so that ROSE will process the file as a UPC file.
  // We can add the option anywhere on the command line.
     argList.push_back("-rose:UPC");

  // This tool will only unparse the file and not compile it.
     if (testGeneratedSourceCode == false)
        {
          argList.push_back("-rose:skipfinalCompileStep");
        }

  // Generate the ROSE AST.
     SgProject* project = frontend(argList);
     ROSE_ASSERT(project != NULL);

#if DEBUG_USING_DOT_GRAPHS
  // generateDOTforMultipleFile(*project);
     generateDOT(*project,"_before_transformation");
#endif
#if DEBUG_USING_DOT_GRAPHS
     const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 12000;
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_before");
#endif

  // Build the inherited attribute
     InheritedAttribute inheritedAttribute;

  // Define the traversal
     Traversal sharedMemoryDSL_Traversal;

#if 0
     printf ("Call the traversal starting at the project (root) node of the AST \n");
#endif

  // Call the traversal starting at the project (root) node of the AST
  // SynthesizedAttribute result = sharedMemoryDSL_Traversal.traverseWithinFile(project,inheritedAttribute);
     SynthesizedAttribute result = sharedMemoryDSL_Traversal.traverse(project,inheritedAttribute);

#if 0
     printf ("DONE: Call the traversal starting at the project (root) node of the AST \n");
#endif

  // This is the compiler pass that will do the transformations on declarations of shared 
  // pointers and there associated expressions.
     fixupNodesWithTypes(project,sharedMemoryDSL_Traversal);

  // AST consistency tests (optional for users, but this enforces more of our tests)
     AstTests::runAllTests(project);

#if DEBUG_USING_DOT_GRAPHS
     printf ("Write out the DOT file after the transformation \n");
     generateDOTforMultipleFile(*project,"after_transformation");
     printf ("DONE: Write out the DOT file after the transformation \n");
#endif
#if DEBUG_USING_DOT_GRAPHS && 1
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
  // const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"_after");
#endif

  // Regenerate the source code but skip the call the to the vendor compiler.
     return backend(project);
   }

