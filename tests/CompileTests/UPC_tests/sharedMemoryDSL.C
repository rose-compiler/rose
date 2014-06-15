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

InheritedAttribute::InheritedAttribute( const InheritedAttribute & X )
   {
     isSharedTypeExpression = X.isSharedTypeExpression;
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
                    long block_size = mod_type->get_typeModifier().get_upcModifier().get_layout();

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

  // DQ (5/24/2014): Added more general support for detecting expressions that need to be transformed.
     SgExpression* expression = isSgExpression(astNode);
     if (expression != NULL)
        {
#if 0
          printf ("In evaluateInheritedAttribute(): found SgExpression: expression = %p = %s \n",expression,expression->class_name().c_str());
          printf ("In evaluateInheritedAttribute(): found SgExpression: type = %p = %s = %s \n",expression->get_type(),expression->get_type()->class_name().c_str(),expression->get_type()->unparseToString().c_str());
#endif
          bool skipTransformation = false;
          SgBinaryOp* parentBinaryOp = isSgBinaryOp(expression->get_parent());
          SgArrowExp* parentArrowExp = isSgArrowExp(expression->get_parent());
          SgDotExp*   parentDotExp = isSgDotExp(expression->get_parent());
       // if (parentArrowExp != NULL)
          if (parentArrowExp != NULL || parentDotExp != NULL)
             {
            // Check if the current expression is the lhs or rhs of the parentArrowExp.
               bool expressionIsLhs = (expression == parentBinaryOp->get_lhs_operand());
               bool expressionIsRhs = (expression == parentBinaryOp->get_rhs_operand());
               ROSE_ASSERT(expressionIsLhs == true || expressionIsRhs == true);
#if 0
               printf ("In evaluateInheritedAttribute(): SgArrowExp: expressionIsLhs = %s expressionIsRhs = %s \n",expressionIsLhs ? "true" : "false",expressionIsRhs ? "true" : "false");
#endif
               skipTransformation = (expressionIsRhs == true);
             }
#if 0
          printf ("In evaluateInheritedAttribute(): SgArrowExp: skipTransformation = %s \n",skipTransformation ? "true" : "false");
#endif
       // Reset the isSharedTypeExpression variable.
          inheritedAttribute.set_SharedTypeExpression(false);

          SgType* type = expression->get_type();
          if (isSharedType(type) == true && skipTransformation == false)
             {
            // This might be a better way to identify expressions that should be transformed.
#if 0
               printf ("In evaluateInheritedAttribute(): found expression with shared type = %p = %s = %s \n",type,type->class_name().c_str(),type->unparseToString().c_str());
#endif
               SgPointerType* pointerType = isSgPointerType(type);
               SgModifierType* modifierType = isSgModifierType(type);

               if (modifierType != NULL)
                  {
                    SgType* base_type = modifierType->get_base_type();
                    ROSE_ASSERT(base_type != NULL);
#if 0
                    printf ("found SgModifierType: base_type = %p = %s \n",base_type,base_type->class_name().c_str(),base_type->unparseToString().c_str());
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
#if 1
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
#else
#error "DEAD CODE!"
                         modifierType = NULL;
#endif
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

     SynthesizedAttribute localResult(astNode);

#if 0
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
     if (functionCallExp != NULL)
        {
          SgType* type = functionCallExp->get_type();
          printf ("SgFunctionCallExp: type = %p = %s \n",type,type->class_name().c_str());
        }
#endif

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
          if (skip_transformation_of_children == true)
             {
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
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
          printf ("Found SgCastExp: Supressed transformation on children of SgCastExp \n");
#endif
          skip_transformation_of_children = true;
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // DQ (6/14/2014): We need to suppress the transformations on the operands of a SgCastExp, 
  // so that it will be done on the SgCastExp directly instead (see test2014_51.c).
     SgExprListExp* exprListExp = isSgExprListExp(astNode);
     if (exprListExp != NULL)
        {
#if 0
          printf ("Found SgExprListExp: Supressed transformation on children of SgExprListExp \n");
#endif
          skip_transformation_of_children = true;
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
          printf ("skip_transformation_of_children = %s \n",skip_transformation_of_children ? "true" : "false");
#endif


#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

  // DQ (5/25/2014): Iterate over the child attributes and determin if any transformations are required to the AST.
     for (SynthesizedAttributesList::iterator i = childAttributes.begin(); i != childAttributes.end(); i++)
        {
#if 0
          printf ("--- synthesized attribute: (*i).get_MarkedForTransformation() = %s \n",(*i).get_MarkedForTransformation() ? "true" : "false");
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
                    printf ("skip_transformation_of_children = %s \n",skip_transformation_of_children ? "true" : "false");
#endif
                    if (skip_transformation_of_children == false)
                       {
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
                         printf ("Skipping the possible transformation: skip_transformation_of_children == true \n");
#endif
                       }
                  }
                 else
                  {
#if 0
                    printf ("Skipping the possible transformation: parentExpression == NULL \n");
#endif
                  }
#if 0
                printf ("Exiting as a test! \n");
                ROSE_ASSERT(false);
#endif
             }
        }

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
#if DEBUG_USING_DOT_GRAPHS
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
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

#if DEBUG_USING_DOT_GRAPHS
     printf ("Write out the DOT file after the transformation \n");
     generateDOTforMultipleFile(*project,"after_transformation");
     printf ("DONE: Write out the DOT file after the transformation \n");
#endif
#if 0
  // Output an optional graph of the AST (the whole graph, of bounded complexity, when active)
  // const int MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH = 10000;
     generateAstGraph(project,MAX_NUMBER_OF_IR_NODES_TO_GRAPH_FOR_WHOLE_GRAPH,"");
#endif

  // Regenerate the source code but skip the call the to the vendor compiler.
     return backend(project);
   }

