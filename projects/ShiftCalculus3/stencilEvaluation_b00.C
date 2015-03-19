
// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

// #include "shiftCalculusCompiler.h"

#include "stencilAndStencilOperatorDetection.h"
#include "stencilEvaluation.h"

// This code will make calls to the finite state machine representing the stencil 
// so that we can execute events and accumulate state (and then read the state as
// and intermediate form for the stencil (maybe saved as an attribute).  This data
// is then the jumping off point for different groups to experiment with the generation
// of architecture specific code.
#include "stencilFiniteStateMachine.h"

#include "dslSupport.h"

#include "VariableIdMapping.h"

using namespace SPRAY; 

extern VariableIdMapping variableIdMapping;

// Controls output of debugging information as compile-time evaluation is done using 
// the Shift Calculus operations.
#define DEBUG_SHIFT_CALCULUS 1

// Define stencil DSL evaluation using more complete use of the variable ID mapping
// initial use was for the Stencil variables only.
#define USING_VARIABLE_ID_MAPPING 1


using namespace std;

using namespace DSL_Support;

StencilEvaluation_InheritedAttribute::StencilEvaluation_InheritedAttribute()
   {
     isShiftExpression      = false;
     stencilOffsetFSM       = NULL;
     stencilCoeficientValue = 0.0;
   }

StencilEvaluation_InheritedAttribute::StencilEvaluation_InheritedAttribute( const StencilEvaluation_InheritedAttribute & X )
   {
     isShiftExpression      = X.isShiftExpression;
     stencilOffsetFSM       = X.stencilOffsetFSM;
     stencilCoeficientValue = X.stencilCoeficientValue;
   }

void
StencilEvaluation_InheritedAttribute::set_ShiftExpression(bool value)
   {
     isShiftExpression = value;
   }

bool
StencilEvaluation_InheritedAttribute::get_ShiftExpression()
   {
     return isShiftExpression;
   }


StencilEvaluation_SynthesizedAttribute::StencilEvaluation_SynthesizedAttribute()
   {
     stencilOperatorTransformed = false;
     stencilOffsetFSM           = NULL;
     stencilCoeficientValue     = 0.0;
   }

StencilEvaluation_SynthesizedAttribute::StencilEvaluation_SynthesizedAttribute( const StencilEvaluation_SynthesizedAttribute & X )
   {
     stencilOperatorTransformed = X.stencilOperatorTransformed;
     stencilOffsetFSM           = X.stencilOffsetFSM;
     stencilCoeficientValue     = X.stencilCoeficientValue;
   }

void
StencilEvaluation_SynthesizedAttribute::set_stencilOperatorTransformed(bool value)
   {
     stencilOperatorTransformed = value;
   }

bool
StencilEvaluation_SynthesizedAttribute::get_stencilOperatorTransformed()
   {
     return stencilOperatorTransformed;
   }



StencilEvaluationTraversal::StencilEvaluationTraversal(DetectionTraversal & previousTraversal)
   {
#if 0
     printf ("In StencilEvaluationTraversal constructor: previousTraversal.get_stencilInputInitializedNameList().size() = %zu \n",previousTraversal.get_stencilInputInitializedNameList().size());
#endif
  // stencilInputInitializedNameList = previousTraversal.get_stencilInputInitializedNameList();

#if 0
     printf ("In StencilEvaluationTraversal constructor: previousTraversal.get_stencilInputInitializedNameList().size() = %zu \n",previousTraversal.get_stencilInputInitializedNameList().size());
     for (size_t i = 0; i < stencilInputInitializedNameList.size(); i++)
        {
          printf ("   --- stencilInputInitializedNameList[%zu]->get_name() = %s \n",i,stencilInputInitializedNameList[i]->get_name().str());
        }
#endif

  // Copy the names of the SgInitializedNames associated with Stencil Operators.
  // stencilOperatorInitializedNameList = previousTraversal.get_stencilOperatorInitializedNameList();

  // Copy the locations where stencil operators are called.
  // stencilOperatorFunctionCallList = previousTraversal.get_stencilOperatorFunctionCallList();

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif
   }


std::vector<SgFunctionCallExp*> & 
StencilEvaluationTraversal::get_stencilOperatorFunctionCallList()
   {
     return stencilOperatorFunctionCallList;
   }


std::map<std::string,StencilFSM*> & 
StencilEvaluationTraversal::get_stencilMap()
   {
     return stencilMap;
   }


// Names used to attach attributes to the AST.
const string dsl_value_string = "dsl_value";
const string StencilEvaluationTraversal::PointValue   = dsl_value_string;
const string StencilEvaluationTraversal::IntegerValue = dsl_value_string;
const string StencilEvaluationTraversal::DoubleValue = dsl_value_string;
const string StencilEvaluationTraversal::ShiftValue   = dsl_value_string;
const string StencilEvaluationTraversal::ArrayValue   = dsl_value_string;
const string StencilEvaluationTraversal::StencilValue = dsl_value_string;



StencilEvaluation_InheritedAttribute
StencilEvaluationTraversal::evaluateInheritedAttribute (SgNode* astNode, StencilEvaluation_InheritedAttribute inheritedAttribute )
   {
#if 1
     printf ("In StencilEvaluationTraversal::evaluateInheritedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

     AstAttributeMechanism* astAttributeContainer = astNode->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
       // I think there should only be one DSL attribute, in the future we can support more on a single IR node.
          if (astAttributeContainer->size() != 1)
             {
               printf ("WARNING: astAttributeContainer->size() != 1: astAttributeContainer->size() = %d \n",astAttributeContainer->size());
             }
          ROSE_ASSERT(astAttributeContainer->size() == 1);

       // Loop over all the attributes at this IR node
          for (AstAttributeMechanism::iterator i = astAttributeContainer->begin(); i != astAttributeContainer->end(); i++)
             {
               AstAttribute* attribute = i->second;
               ROSE_ASSERT(attribute != NULL);
 
               DSL_Attribute* dslAstAttribute = dynamic_cast<DSL_Attribute*>(attribute);
               ROSE_ASSERT(dslAstAttribute != NULL);

            // The attributes for Point, Shift, Stencil, etc are only put at variables.
               SgInitializedName* initializedName = isSgInitializedName(astNode);

               Point_Attribute* pointAstAttribute = dynamic_cast<Point_Attribute*>(dslAstAttribute);
               if (pointAstAttribute != NULL)
                  {
                 // Set the value of the point in the attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- evaluateInheritedAttribute(): Detected a Point_Attribute \n");
#endif
#if 1
                    PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
                    ROSE_ASSERT(pointValue_Attribute != NULL);
#if 0
                    printf ("Adding (PointValue) pointValue_Attribute = %p \n",pointValue_Attribute);
#endif
                    ROSE_ASSERT(initializedName != NULL);
                    VariableId varId = variableIdMapping.variableId(initializedName);
#if 1
                    int varIdCode = varId.getIdCode();
                    printf ("Insert the PointValue_Attribute = %p into the pointValueState map: varIdCode = %d = %s (but using the VariableId) \n",pointValue_Attribute,varIdCode,variableIdMapping.variableName(varId).c_str());
#endif
                 // stencilValueState[varIdCode] = stencilValue_Attribute;
                    pointValueState[varId] = pointValue_Attribute;
#else
                    pointValueState[variableIdMapping.variableId(initializedName)] = new PointValue_Attribute();
#endif
                  }

#if 0
               Stencil_Attribute* stencilAstAttribute = dynamic_cast<Stencil_Attribute*>(dslAstAttribute);
               if (stencilAstAttribute != NULL)
                  {
                    stencilValueState[variableIdMapping.variableId(initializedName)] = new StencilValue_Attribute();
                  }
#endif
               Array_Attribute* arrayAstAttribute = dynamic_cast<Array_Attribute*>(dslAstAttribute);
               if (arrayAstAttribute != NULL)
                  {
                    arrayValueState[variableIdMapping.variableId(initializedName)] = new ArrayValue_Attribute();
                  }






             }
        }

  // Construct the return attribute from the modified input attribute.
     return StencilEvaluation_InheritedAttribute(inheritedAttribute);
   }



void
StencilEvaluationTraversal::add_dslValueAttribute ( SgNode* node, DSL_ValueAttribute* dslValueAttribute )
   {
     ROSE_ASSERT(node != NULL);
     ROSE_ASSERT(dslValueAttribute != NULL);

     SgVarRefExp* varRefExp = isSgVarRefExp(node);
     SgInitializedName* initializedName = isSgInitializedName(node);
     if (varRefExp != NULL || initializedName != NULL)
        {
          VariableId varId;
          if (varRefExp != NULL)
             {
               varId = variableIdMapping.variableId(varRefExp);
             }
            else
             {
               if (initializedName != NULL)
                  {
                    varId = variableIdMapping.variableId(initializedName);
                  }
                 else
                  {
                    printf ("Error: input what neither a SgVarRefExp nor SgInitializedName: node = %p = %s \n",node,node->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }
#if 1
          int varIdCode = varId.getIdCode();
          printf ("Add dsl attribute into dslValueState map: varIdCode = %d = %s (but using the VariableId) \n",varIdCode,variableIdMapping.variableName(varId).c_str());
#endif
          ROSE_ASSERT(dslValueState.find(varId) == dslValueState.end());
          dslValueState[varId] = dslValueAttribute;
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
          printf ("Need to handle the case where node is not a SgVarRefExp: node = %p = %s \n",node,node->class_name().c_str());

       // All value strings are the same.

          node->addNewAttribute(dsl_value_string,dslValueAttribute);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

   }


DSL_ValueAttribute*
StencilEvaluationTraversal::get_dslValueAttribute ( SgNode* node  )
   {
     DSL_ValueAttribute* dslValueAttribute = NULL;

     ROSE_ASSERT(node != NULL);

     SgVarRefExp* varRefExp = isSgVarRefExp(node);
     SgInitializedName* initializedName = isSgInitializedName(node);
     if (varRefExp != NULL || initializedName != NULL)
        {
          VariableId varId;
          if (varRefExp != NULL)
             {
               varId = variableIdMapping.variableId(varRefExp);
             }
            else
             {
               if (initializedName != NULL)
                  {
                    varId = variableIdMapping.variableId(initializedName);
                  }
                 else
                  {
                    printf ("Error: input what neither a SgVarRefExp nor SgInitializedName: node = %p = %s \n",node,node->class_name().c_str());
                    ROSE_ASSERT(false);
                  }
             }
#if 1
          int varIdCode = varId.getIdCode();
          printf ("Get dsl attribute from dslValueState map: varIdCode = %d = %s (but using the VariableId) \n",varIdCode,variableIdMapping.variableName(varId).c_str());
#endif
          ROSE_ASSERT(dslValueState.find(varId) != dslValueState.end());
          dslValueAttribute = dslValueState[varId];
          ROSE_ASSERT(dslValueAttribute != NULL);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
          printf ("Need to handle the case where node is not a SgVarRefExp: node = %p = %s \n",node,node->class_name().c_str());

       // All value strings are the same.
       // It might be that this case should look for the DSL_ValueAttribute directly on the input node as an attached attribute.
       // dslValueAttribute = dynamic_cast<DSL_ValueAttribute*>(node->getAttribute(PointValue));
          dslValueAttribute = dynamic_cast<DSL_ValueAttribute*>(node->getAttribute(dsl_value_string));

          ROSE_ASSERT(dslValueAttribute != NULL);
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     return dslValueAttribute;
   }


// Note that these could now be just a single template function.
PointValue_Attribute*
StencilEvaluationTraversal::get_PointValueAttribute ( SgNode* node )
   {
  // This function is type specific and calls the lower level support function: get_dslValueAttribute().

     PointValue_Attribute* valueAttribute = dynamic_cast<PointValue_Attribute*>(get_dslValueAttribute(node));
     ROSE_ASSERT(valueAttribute != NULL);

     return valueAttribute;
   }

StencilValue_Attribute*
StencilEvaluationTraversal::get_StencilValueAttribute ( SgNode* node )
   {
  // This function is type specific and calls the lower level support function: get_dslValueAttribute().

     StencilValue_Attribute* valueAttribute = dynamic_cast<StencilValue_Attribute*>(get_dslValueAttribute(node));
     ROSE_ASSERT(valueAttribute != NULL);

     return valueAttribute;
   }

ArrayValue_Attribute*
StencilEvaluationTraversal::get_ArrayValueAttribute ( SgNode* node )
   {
  // This function is type specific and calls the lower level support function: get_dslValueAttribute().

     ArrayValue_Attribute* valueAttribute = dynamic_cast<ArrayValue_Attribute*>(get_dslValueAttribute(node));
     ROSE_ASSERT(valueAttribute != NULL);

     return valueAttribute;
   }

ShiftValue_Attribute*
StencilEvaluationTraversal::get_ShiftValueAttribute ( SgNode* node )
   {
  // This function is type specific and calls the lower level support function: get_dslValueAttribute().

     ShiftValue_Attribute* valueAttribute = dynamic_cast<ShiftValue_Attribute*>(get_dslValueAttribute(node));
     ROSE_ASSERT(valueAttribute != NULL);

     return valueAttribute;
   }

IntegerValue_Attribute*
StencilEvaluationTraversal::get_IntegerValueAttribute ( SgNode* node )
   {
  // This function is type specific and calls the lower level support function: get_dslValueAttribute().

     IntegerValue_Attribute* valueAttribute = dynamic_cast<IntegerValue_Attribute*>(get_dslValueAttribute(node));
     ROSE_ASSERT(valueAttribute != NULL);

     return valueAttribute;
   }

DoubleValue_Attribute*
StencilEvaluationTraversal::get_DoubleValueAttribute ( SgNode* node )
   {
  // This function is type specific and calls the lower level support function: get_dslValueAttribute().

     DoubleValue_Attribute* valueAttribute = dynamic_cast<DoubleValue_Attribute*>(get_dslValueAttribute(node));
     ROSE_ASSERT(valueAttribute != NULL);

     return valueAttribute;
   }




StencilEvaluation_SynthesizedAttribute
StencilEvaluationTraversal::evaluateSynthesizedAttribute (SgNode* astNode, StencilEvaluation_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     StencilEvaluation_SynthesizedAttribute return_synthesizedAttribute;

#if 1
     printf ("In StencilEvaluationTraversal::evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

// #if USING_VARIABLE_ID_MAPPING
#if 0
     SgAssignInitializer* assignInitializer = isSgAssignInitializer(astNode);
     if (assignInitializer != NULL)
        {
       // Take the computed value in the attribute of the operand and assign it to the associated variable in the parent.
          SgInitializedName* initializedName = isSgInitializedName(assignInitializer->get_parent());
          SgExpression* operandExp = isSgExpression(assignInitializer->get_operand());

          ROSE_ASSERT(initializedName != NULL);
          ROSE_ASSERT(operandExp      != NULL);

       // Get the value attribute on the operandExp and assign it to the value attribute on the initializedName.

          if (initializedName != NULL && operandExp != NULL)
             {
               printf ("Get the value attribute on the operandExp and assign it to the value attribute on the initializedName \n");

               DSL_Attribute* dslAstAttribute = dynamic_cast<DSL_Attribute*>(operandExp->getAttribute(PointValue));
            // ROSE_ASSERT(dslAstAttribute != NULL);
               if (dslAstAttribute != NULL)
                  {

                    printf ("Found attribute at operandExp = %p = %s \n",operandExp,operandExp->class_name().c_str());

#if 1
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

  // Look for DSL attributes so that we can support compile-time evaluation of DSL abstractions.
     AstAttributeMechanism* astAttributeContainer = astNode->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
       // I think there should only be one DSL attribute, in the future we can support more on a single IR node.
          if (astAttributeContainer->size() != 1)
             {
               printf ("WARNING: astAttributeContainer->size() != 1: astAttributeContainer->size() = %d \n",astAttributeContainer->size());
             }
          ROSE_ASSERT(astAttributeContainer->size() == 1);

       // Loop over all the attributes at this IR node
          for (AstAttributeMechanism::iterator i = astAttributeContainer->begin(); i != astAttributeContainer->end(); i++)
             {
               AstAttribute* attribute = i->second;
               ROSE_ASSERT(attribute != NULL);
 
               DSL_Attribute* dslAstAttribute = dynamic_cast<DSL_Attribute*>(attribute);
               ROSE_ASSERT(dslAstAttribute != NULL);

#define DEBUG_DSL_CHILDREN 0

#if DEBUG_DSL_CHILDREN
               printf ("Identified dslAstAttribute in evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
            // printf ("   --- return_synthesizedAttribute.dslChildren.size() = %zu \n",return_synthesizedAttribute.dslChildren.size());
               printf ("   --- dslAstAttribute->toString()         = %s \n",dslAstAttribute->toString().c_str());
               printf ("   --- dslAstAttribute->dslChildren.size() = %zu \n",dslAstAttribute->dslChildren.size());
#endif
               for (vector<SgNode*>::iterator j = dslAstAttribute->dslChildren.begin(); j != dslAstAttribute->dslChildren.end(); j++)
                  {
                    SgNode* child = *j;
                    ROSE_ASSERT(child != NULL);
#if DEBUG_DSL_CHILDREN
                    printf ("   --- --- child = %p = %s \n",child,child->class_name().c_str());
#endif
                  }

#define DEBUG_DSL_ATTRIBUTES 1

#if (USING_VARIABLE_ID_MAPPING == 0)
            // If we put a discrimiator value into the DSL_Attribute, then we could use a switch statement (later).
               Point_Attribute* pointAstAttribute = dynamic_cast<Point_Attribute*>(dslAstAttribute);
               if (pointAstAttribute != NULL)
                  {
                 // Set the value of the point in the attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a Point_Attribute \n");
#endif
                 // Add a value attribute to the current AST node.
                    PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
                    ROSE_ASSERT(pointValue_Attribute != NULL);
#if 0
                    printf ("Adding (PointValue) pointValue_Attribute = %p \n",pointValue_Attribute);
#endif

#if 0
                    VariableId varId = variableIdMapping.variableId(initializedName);
                    int varIdCode = varId.getIdCode();
#if 1
                    printf ("Insert the PointValue_Attribute = %p into the pointValueState map: varIdCode = %d = %s (but using the VariableId) \n",pointValue_Attribute,varIdCode,variableIdMapping.variableName(varId).c_str());
#endif
                 // stencilValueState[varIdCode] = stencilValue_Attribute;
                    pointValueState[varId] = pointValue_Attribute;
#else
                    astNode->addNewAttribute(PointValue,pointValue_Attribute);

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);

                 // Find the point value attribute on the child.
                    PointValue_Attribute* child_pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(childNode->getAttribute(PointValue));
                 // ROSE_ASSERT(child_pointValue_Attribute != NULL);
                    if (child_pointValue_Attribute != NULL)
                       {
                      // Assign the value from the initialization function to this variable.
                         pointValue_Attribute->value = child_pointValue_Attribute->value;
                       }
                      else
                       {
                         printf ("child_pointValue_Attribute == NULL: childNode = %p = %s \n",childNode,childNode->class_name().c_str());
                         ROSE_ASSERT(false);
                       }

#ifdef DEBUG_SHIFT_CALCULUS
                    SgName name = "unknown_name";
                    SgInitializedName* initializedName = isSgInitializedName(astNode);
                    if (initializedName != NULL)
                       {
                         name = initializedName->get_name();
                       }
                    printf ("After call to evaluate Point on node = %p = %s = %s using childNode = %p = %s \n",astNode,astNode->class_name().c_str(),name.str(),childNode,childNode->class_name().c_str());
                    pointValue_Attribute->value.print();
#endif
#endif
                  }
#else
               Point_Attribute* pointAstAttribute = dynamic_cast<Point_Attribute*>(dslAstAttribute);
               if (pointAstAttribute != NULL)
                  {
                 // Set the value of the point in the attribute?
#if 1
                    PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
                    ROSE_ASSERT(pointValue_Attribute != NULL);
                    add_dslValueAttribute(astNode,pointValue_Attribute);

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);
                    PointValue_Attribute* child_pointValue_Attribute = get_PointValueAttribute(childNode);

                 // Assign the child attribute value to the attribute value on the current astNode.
                    pointValue_Attribute->value = child_pointValue_Attribute->value;
#else
                    SgInitializedName* initializedName = isSgInitializedName(astNode);
                    ROSE_ASSERT(initializedName != NULL);

                    VariableId varId = variableIdMapping.variableId(initializedName);
#if 1
                    int varIdCode = varId.getIdCode();
                    printf ("Insert into the map: varIdCode = %d = %s (but using the VariableId) \n",varIdCode,variableIdMapping.variableName(varId).c_str());
#endif
                 // pointValueState[varId] = pointValue_Attribute;

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);

                    printf ("Processing Point_Attribute: childNode = %p = %s \n",childNode,childNode->class_name().c_str());

                 // Find the point value attribute on the child.
                 // PointValue_Attribute* child_pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(childNode->getAttribute(PointValue));
                    PointValue_Attribute* child_pointValue_Attribute = NULL;
                    SgVarRefExp* varRefExp = isSgVarRefExp(childNode);
                    if (varRefExp != NULL)
                       {
                         VariableId varRefExp_varId = variableIdMapping.variableId(initializedName);
                         ROSE_ASSERT(pointValueState.find(varRefExp_varId) != pointValueState.end());
                         child_pointValue_Attribute = pointValueState[varRefExp_varId];
                         ROSE_ASSERT(child_pointValue_Attribute != NULL);

                      // This should not be Point X = X;
                         ROSE_ASSERT(variableIdMapping.variableId(varRefExp).getIdCode() != variableIdMapping.variableId(initializedName).getIdCode());
                       }
                      else
                       {
                         child_pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(childNode->getAttribute(PointValue));
                         ROSE_ASSERT(child_pointValue_Attribute != NULL);
                       }

                    ROSE_ASSERT(pointValueState.find(varId) != pointValueState.end());
                    pointValueState[varId]->value = child_pointValue_Attribute->value;
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
#endif

#if (USING_VARIABLE_ID_MAPPING == 0)
               Array_Attribute* arrayAstAttribute = dynamic_cast<Array_Attribute*>(dslAstAttribute);
               if (arrayAstAttribute != NULL)
                  {
                 // Set the value of the point in the attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a Array_Attribute \n");
#endif
                 // Add a value attribute to the current AST node.
                    ArrayValue_Attribute* arrayValue_Attribute = new ArrayValue_Attribute();
                    ROSE_ASSERT(arrayValue_Attribute != NULL);
#if 0
                    printf ("Adding (ArrayValue) arrayValue_Attribute = %p \n",arrayValue_Attribute);
#endif
                    astNode->addNewAttribute(ArrayValue,arrayValue_Attribute);

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);

                 // Find the point value attribute on the child.
                    ArrayValue_Attribute* child_arrayValue_Attribute = dynamic_cast<ArrayValue_Attribute*>(childNode->getAttribute(ArrayValue));
                 // ROSE_ASSERT(child_pointValue_Attribute != NULL);
                    if (child_arrayValue_Attribute != NULL)
                       {
                      // Assign the value from the initialization function to this variable.
                         arrayValue_Attribute->value = child_arrayValue_Attribute->value;
                       }
                      else
                       {
                         printf ("child_arrayValue_Attribute == NULL: childNode = %p = %s \n",childNode,childNode->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }
#else
               Array_Attribute* arrayAstAttribute = dynamic_cast<Array_Attribute*>(dslAstAttribute);
               if (arrayAstAttribute != NULL)
                  {
                 // Set the value of the point in the attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a Array_Attribute \n");
#endif
#if 1
                    ArrayValue_Attribute* arrayValue_Attribute = new ArrayValue_Attribute();
                    ROSE_ASSERT(arrayValue_Attribute != NULL);
                    add_dslValueAttribute(astNode,arrayValue_Attribute);

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);
                    ArrayValue_Attribute* child_arrayValue_Attribute = get_ArrayValueAttribute(childNode);

                 // Assign the child attribute value to the attribute value on the current astNode.
                    arrayValue_Attribute->value = child_arrayValue_Attribute->value;
#else
                    SgInitializedName* initializedName = isSgInitializedName(astNode);
                    ROSE_ASSERT(initializedName != NULL);

                    VariableId varId = variableIdMapping.variableId(initializedName);
#if 1
                    int varIdCode = varId.getIdCode();
                    printf ("Insert into the map: varIdCode = %d = %s (but using the VariableId) \n",varIdCode,variableIdMapping.variableName(varId).c_str());
#endif
                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);
#if 1
                    printf ("Processing Array_Attribute: childNode = %p = %s \n",childNode,childNode->class_name().c_str());
#endif
                    SgVarRefExp* varRefExp = isSgVarRefExp(childNode);
                    ROSE_ASSERT(varRefExp == NULL);

                 // Find the point value attribute on the child.
                    ArrayValue_Attribute* child_arrayValue_Attribute = dynamic_cast<ArrayValue_Attribute*>(childNode->getAttribute(ArrayValue));
                    ROSE_ASSERT(child_arrayValue_Attribute != NULL);

                    ROSE_ASSERT(arrayValueState.find(varId) != arrayValueState.end());
                    arrayValueState[varId]->value = child_arrayValue_Attribute->value;
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
#endif

#if (USING_VARIABLE_ID_MAPPING == 0)
               Stencil_Attribute* stencilAstAttribute = dynamic_cast<Stencil_Attribute*>(dslAstAttribute);
               if (stencilAstAttribute != NULL)
                  {
                 // Set the value of the point in the attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a Stencil_Attribute \n");
#endif
                 // Add a value attribute to the current AST node.
#if 1
                    StencilValue_Attribute* stencilValue_Attribute = new StencilValue_Attribute();
                    ROSE_ASSERT(stencilValue_Attribute != NULL);
                    add_dslValueAttribute(astNode,stencilValue_Attribute);

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);
                    StencilValue_Attribute* child_stencilValue_Attribute = get_StencilValueAttribute(childNode);

                 // Assign the child attribute value to the attribute value on the current astNode.
                    stencilValue_Attribute->value = child_stencilValue_Attribute->value;
#else
                    StencilValue_Attribute* stencilValue_Attribute = new StencilValue_Attribute();
                    ROSE_ASSERT(stencilValue_Attribute != NULL);
#if 0
                    printf ("Adding (StencilValue) stencilValue_Attribute = %p \n",stencilValue_Attribute);
#endif
                    astNode->addNewAttribute(StencilValue,stencilValue_Attribute);

                 // Initialize the stencil value by getting the value from the variable initializer.

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);

                 // Find the point value attribute on the child.
                    StencilValue_Attribute* child_stencilValue_Attribute = dynamic_cast<StencilValue_Attribute*>(childNode->getAttribute(StencilValue));
                 // ROSE_ASSERT(child_pointValue_Attribute != NULL);
                    if (child_stencilValue_Attribute != NULL)
                       {
                      // Assign the value from the initialization function to this variable.
                         stencilValue_Attribute->value = child_stencilValue_Attribute->value;
                       }
                      else
                       {
                         printf ("child_stencilValue_Attribute == NULL: childNode = %p = %s \n",childNode,childNode->class_name().c_str());
                         ROSE_ASSERT(false);
                       }

                 // Save the StencilValue attribute into the map for the interpreted state.
                    SgInitializedName* initializedName = isSgInitializedName(astNode);
                    ROSE_ASSERT(initializedName != NULL);
#if 1
                    printf ("variableIdMapping.getVariableIdSet().size() = %zu \n",variableIdMapping.getVariableIdSet().size());
#endif
                    VariableId varId = variableIdMapping.variableId(initializedName);
                    int varIdCode = varId.getIdCode();
#if 1
                    printf ("Insert the StencilValue_Attribute = %p into the stencilValueState map: varIdCode = %d = %s (but using the VariableId) \n",stencilValue_Attribute,varIdCode,variableIdMapping.variableName(varId).c_str());
#endif
                 // stencilValueState[varIdCode] = stencilValue_Attribute;
                    stencilValueState[varId] = stencilValue_Attribute;

                 // variableIdMapping.toStream(cout);
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
#else
               Stencil_Attribute* stencilAstAttribute = dynamic_cast<Stencil_Attribute*>(dslAstAttribute);
               if (stencilAstAttribute != NULL)
                  {
                 // Set the value of the stencil in the attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a Stencil_Attribute \n");
#endif
#if 1
                    StencilValue_Attribute* stencilValue_Attribute = new StencilValue_Attribute();
                    ROSE_ASSERT(stencilValue_Attribute != NULL);
                    add_dslValueAttribute(astNode,stencilValue_Attribute);

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);
                    StencilValue_Attribute* child_stencilValue_Attribute = get_StencilValueAttribute(childNode);

                 // Assign the child attribute value to the attribute value on the current astNode.
                    stencilValue_Attribute->value = child_stencilValue_Attribute->value;
#else
                    SgInitializedName* initializedName = isSgInitializedName(astNode);
                    ROSE_ASSERT(initializedName != NULL);

                    VariableId varId = variableIdMapping.variableId(initializedName);
#if 1
                    int varIdCode = varId.getIdCode();
                    printf ("Insert into the map: varIdCode = %d = %s (but using the VariableId) \n",varIdCode,variableIdMapping.variableName(varId).c_str());
#endif
                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);

                 // Find the point value attribute on the child.
                    StencilValue_Attribute* child_stencilValue_Attribute = dynamic_cast<StencilValue_Attribute*>(childNode->getAttribute(StencilValue));
                    ROSE_ASSERT(child_stencilValue_Attribute != NULL);

                    ROSE_ASSERT(stencilValueState.find(varId) != stencilValueState.end());
                    stencilValueState[varId]->value = child_stencilValue_Attribute->value;
#endif
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
#endif

               OperatorZero_Attribute* operatorZeroAstAttribute = dynamic_cast<OperatorZero_Attribute*>(dslAstAttribute);
               if (operatorZeroAstAttribute != NULL)
                  {
                 // return the Zero valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a OperatorZero_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
#if 0
                    printf ("Adding (PointValue) pointValue_Attribute = %p \n",pointValue_Attribute);
#endif
                    ROSE_ASSERT(pointValue_Attribute != NULL);

                    astNode->addNewAttribute(PointValue,pointValue_Attribute);

                 // Compute the constant value returned by the associated operator abstraction.
                    pointValue_Attribute->value = getZeros();

#ifdef DEBUG_SHIFT_CALCULUS
                    printf ("After call to getZeros() operator \n");
                    pointValue_Attribute->value.print();
#endif
                  }

               OperatorOnes_Attribute* operatorOnesAstAttribute = dynamic_cast<OperatorOnes_Attribute*>(dslAstAttribute);
               if (operatorOnesAstAttribute != NULL)
                  {
                 // return the Ones valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a OperatorOnes_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
#if 0
                    printf ("Adding (PointValue) pointValue_Attribute = %p \n",pointValue_Attribute);
#endif
                    ROSE_ASSERT(pointValue_Attribute != NULL);

                    astNode->addNewAttribute(PointValue,pointValue_Attribute);

                 // Compute the constant value returned by the associated operator abstraction.
                    pointValue_Attribute->value = getOnes();

#ifdef DEBUG_SHIFT_CALCULUS
                    printf ("After call to getOnes() operator \n");
                    pointValue_Attribute->value.print();
#endif
                  }

               OperatorUnit_Attribute* operatorUnitAstAttribute = dynamic_cast<OperatorUnit_Attribute*>(dslAstAttribute);
               if (operatorUnitAstAttribute != NULL)
                  {
                 // return the Ones valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a OperatorUnit_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
#if 0
                    printf ("Adding (PointValue) pointValue_Attribute = %p \n",pointValue_Attribute);
#endif
                    ROSE_ASSERT(pointValue_Attribute != NULL);

                    astNode->addNewAttribute(PointValue,pointValue_Attribute);

                 // This value need to be interpreted (later).
                    int dimension = 0;

                 // Compute the constant value returned by the associated operator abstraction.
                    pointValue_Attribute->value = getUnitv(dimension);

#ifdef DEBUG_SHIFT_CALCULUS
                    printf ("After call to getUnitv(dimension = %d) operator \n",dimension);
                    pointValue_Attribute->value.print();
#endif
                  }

               OperatorCarot_Attribute* operatorCarotAstAttribute = dynamic_cast<OperatorCarot_Attribute*>(dslAstAttribute);
               if (operatorCarotAstAttribute != NULL)
                  {
                 // return the Ones valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a OperatorCarot_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    ShiftValue_Attribute* shiftValue_Attribute = new ShiftValue_Attribute();
#if 0
                    printf ("Adding (OperatorCarotValue) shiftValue_Attribute = %p \n",shiftValue_Attribute);
#endif
#if 0
                    ROSE_ASSERT(shiftValue_Attribute != NULL);
                    astNode->addNewAttribute(ShiftValue,shiftValue_Attribute);
#else
                 // Newer API.
                    add_dslValueAttribute(astNode,shiftValue_Attribute);
#endif
                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 2);
                    SgNode* childNode_lhs = dslAstAttribute->dslChildren[0];
                    SgNode* childNode_rhs = dslAstAttribute->dslChildren[1];
                    ROSE_ASSERT(childNode_lhs != NULL);
                    ROSE_ASSERT(childNode_rhs != NULL);
#if 1
                    ArrayValue_Attribute* child_lhs_arrayValue_Attribute = get_ArrayValueAttribute(childNode_lhs);
                    PointValue_Attribute* child_rhs_pointValue_Attribute = get_PointValueAttribute(childNode_rhs);

                    shiftValue_Attribute->value = operator^(child_lhs_arrayValue_Attribute->value,child_rhs_pointValue_Attribute->value);
#else
                 // IntegerValue_Attribute* child_lhs_scalarValue_Attribute = NULL;
                 // ArrayValue_Attribute*   child_rhs_arrayValue_Attribute  = NULL;
#if 1
                    printf ("childNode_lhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str());
                    printf ("childNode_rhs = %p = %s \n",childNode_rhs,childNode_rhs->class_name().c_str());
#endif
                    ArrayValue_Attribute* child_lhs_arrayValue_Attribute = dynamic_cast<ArrayValue_Attribute*>(childNode_lhs->getAttribute(ArrayValue));
                    PointValue_Attribute* child_rhs_pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(childNode_rhs->getAttribute(PointValue));
#if 1
                    printf ("child_lhs_arrayValue_Attribute = %p \n",child_lhs_arrayValue_Attribute);
                    printf ("child_rhs_pointValue_Attribute = %p \n",child_rhs_pointValue_Attribute);
#endif
                    if (child_lhs_arrayValue_Attribute != NULL && child_rhs_pointValue_Attribute != NULL)
                       {
                      // Call the associated operator and assign result to his attribute.
                      // shiftValue_Attribute->value = child_lhs_pointValue_Attribute->value ^ child_rhs_arrayValue_Attribute->value;

                         printf ("Calling operator^(): with lhs array value and rhs Point value \n");

#ifdef DEBUG_SHIFT_CALCULUS
                         printf ("Before call to operator^(): child_rhs_pointValue_Attribute->value: \n");
                         child_rhs_pointValue_Attribute->value.print();
#endif

                         shiftValue_Attribute->value = operator^(child_lhs_arrayValue_Attribute->value,child_rhs_pointValue_Attribute->value);

#ifdef DEBUG_SHIFT_CALCULUS
                         printf ("After call to operator^(): shiftValue_Attribute->value: \n");
                         shiftValue_Attribute->value.print();
#endif
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                      else
                       {
                         printf ("child_lhs_arrayValue_Attribute == NULL: childNode_lhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str());
                         printf ("child_rhs_pointValue_Attribute == NULL: childNode_rhs = %p = %s \n",childNode_rhs,childNode_rhs->class_name().c_str());

                         ROSE_ASSERT(child_lhs_arrayValue_Attribute  != NULL);
                         ROSE_ASSERT(child_rhs_pointValue_Attribute != NULL);

                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
                       }
#endif
                 // Compute the constant value returned by the associated operator abstraction.
                 // shiftValue_Attribute->value = operator^(lhs,rhs);
                  }
               
               PointOperatorMultiply_Attribute* pointOperatorMultiplyAstAttribute = dynamic_cast<PointOperatorMultiply_Attribute*>(dslAstAttribute);
               if (pointOperatorMultiplyAstAttribute != NULL)
                  {
                 // return the Ones valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a PointOperatorMultiply_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
#if 0
                    printf ("Adding (PointValue) pointValue_Attribute = %p \n",pointValue_Attribute);
#endif
#if 0
                 // Older API.
                    ROSE_ASSERT(pointValue_Attribute != NULL);
                    astNode->addNewAttribute(PointValue,pointValue_Attribute);
#else
                 // Newer API.
                    add_dslValueAttribute(astNode,pointValue_Attribute);
#endif
                 // Compute the constant value returned by the associated operator abstraction.
                 // pointValue_Attribute->value = getOnes();

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 2);
                    SgNode* childNode_lhs = dslAstAttribute->dslChildren[0];
                    SgNode* childNode_rhs = dslAstAttribute->dslChildren[1];
                    ROSE_ASSERT(childNode_lhs != NULL);
                    ROSE_ASSERT(childNode_rhs != NULL);

                    printf ("Processing PointOperatorMultiply: childNode_lhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str());

                 // Find the point value attribute on the child (one of these is a scalar).
                 // PointValue_Attribute* child_lhs_pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(childNode_lhs->getAttribute(PointValue));
                 // PointValue_Attribute* child_rhs_pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(childNode_rhs->getAttribute(PointValue));

                    PointValue_Attribute* child_lhs_pointValue_Attribute = NULL;
#if 1
                    child_lhs_pointValue_Attribute = get_PointValueAttribute(childNode_lhs);
#else
                    SgVarRefExp* varRefExp = isSgVarRefExp(childNode_lhs);
                    if (varRefExp != NULL)
                       {
                         VariableId varId = variableIdMapping.variableId(varRefExp);
#if 1
                         int varIdCode = varId.getIdCode();
                         printf ("Get lhs from the pointValueState map: varIdCode = %d = %s (but using the VariableId) \n",varIdCode,variableIdMapping.variableName(varId).c_str());
#endif
                         ROSE_ASSERT(pointValueState.find(varId) != pointValueState.end());
                         child_lhs_pointValue_Attribute = pointValueState[varId];
                       }
                      else
                       {
                         printf ("Need to handle the case where childNode_lhs is not a SgVarRefExp: childNode_lhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str());

                         child_lhs_pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(childNode_lhs->getAttribute(PointValue));
                         ROSE_ASSERT(child_lhs_pointValue_Attribute != NULL);
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
#endif
#if 1
                    printf ("childNode_lhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str());
                    printf ("child_lhs_pointValue_Attribute = %p \n",child_lhs_pointValue_Attribute);
#endif
                    int value = 42;
                    SgValueExp* valueExp = isSgValueExp(childNode_rhs);
                    if (valueExp != NULL)
                       {
                         SgIntVal* integerValue = isSgIntVal(valueExp);
                         if (integerValue != NULL)
                            {
                              value = integerValue->get_value();
                            }
                           else
                            {
                              printf ("Error: childNode_rhs not yet evaluated to an integer constant (using value 42): childNode_rhs = %p = %s \n",childNode_rhs,childNode_rhs->class_name().c_str());
                              value = 42;
                            }
                       }
                      else
                       {
                         printf ("Error: childNode_rhs not yet constant evaluated using value 42: childNode_rhs = %p = %s \n",childNode_rhs,childNode_rhs->class_name().c_str());
                         value = 42;
                       }

                 // ROSE_ASSERT(child_pointValue_Attribute != NULL);

                    if (child_lhs_pointValue_Attribute != NULL)
                       {
                      // Call the associated operator and assign result to this attribute.
                         pointValue_Attribute->value = child_lhs_pointValue_Attribute->value * value;
                       }
                      else
                       {
                         printf ("child_pointValue_Attribute == NULL: childNode_lhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str());
                         printf ("child_pointValue_Attribute == NULL: childNode_rhs = %p = %s \n",childNode_rhs,childNode_rhs->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }

               ScalarShiftOperatorMultiply_Attribute* scalarShiftOperatorMultiplyAstAttribute = dynamic_cast<ScalarShiftOperatorMultiply_Attribute*>(dslAstAttribute);
               if (scalarShiftOperatorMultiplyAstAttribute != NULL)
                  {
                 // return the Ones valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a ScalarShiftOperatorMultiply_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    StencilValue_Attribute* stencilValue_Attribute = new StencilValue_Attribute();
#if 0
                    printf ("Adding (StencilValue) stencilValue_Attribute = %p \n",stencilValue_Attribute);
#endif
                    ROSE_ASSERT(stencilValue_Attribute != NULL);

                    astNode->addNewAttribute(StencilValue,stencilValue_Attribute);

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 2);
                    SgNode* childNode_lhs = dslAstAttribute->dslChildren[0];
                    SgNode* childNode_rhs = dslAstAttribute->dslChildren[1];
                    ROSE_ASSERT(childNode_lhs != NULL);
                    ROSE_ASSERT(childNode_rhs != NULL);

                    DoubleValue_Attribute* child_lhs_doubleValue_Attribute = dynamic_cast<DoubleValue_Attribute*>(childNode_lhs->getAttribute(DoubleValue));
                    ShiftValue_Attribute*  child_rhs_shiftValue_Attribute  = dynamic_cast<ShiftValue_Attribute*>(childNode_rhs->getAttribute(ShiftValue));
#if 1
                    printf ("child_lhs_doubleValue_Attribute = %p \n",child_lhs_doubleValue_Attribute);
                    printf ("child_rhs_shiftValue_Attribute  = %p \n",child_rhs_shiftValue_Attribute);
#endif
                    if (child_lhs_doubleValue_Attribute != NULL && child_rhs_shiftValue_Attribute != NULL)
                       {
                      // Call the associated operator and assign result to this attribute.
#ifdef DEBUG_SHIFT_CALCULUS
                         printf ("Calling Stencil operator*(double,Shift & X): with lhs double value and rhs Shift value \n");
                         printf ("child_lhs_doubleValue_Attribute->value = %f \n",child_lhs_doubleValue_Attribute->value);
                         child_rhs_shiftValue_Attribute->value.print();
#endif
#if 0
                      // The operator+=() is syntax that is not implemented, so we can't actually call it to evaluate the stencil value.
#if 0
                      // Runtime error: error - trying to add two stencils with different LHS shifts / ratios
                         stencilValue_Attribute->value = child_lhs_stencilValue_Attribute->value + child_rhs_stencilValue_Attribute->value;
#else
                      // Get this working using this (incorrect) code first.
                         stencilValue_Attribute->value = child_lhs_stencilValue_Attribute->value;
#endif
#else
                         stencilValue_Attribute->value = child_lhs_doubleValue_Attribute->value * child_rhs_shiftValue_Attribute->value;
#endif

#ifdef DEBUG_SHIFT_CALCULUS
                         printf ("resulting stencilValue_Attribute->value: calling stencilDump() \n");
                         stencilValue_Attribute->value.stencilDump();
#endif
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                      else
                       {
                         printf ("child_lhs_doubleValue_Attribute == NULL: childNode_lhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str());
                         printf ("child_rhs_shiftValue_Attribute  == NULL: childNode_rhs = %p = %s \n",childNode_rhs,childNode_rhs->class_name().c_str());

                         ROSE_ASSERT(child_lhs_doubleValue_Attribute  != NULL);
                         ROSE_ASSERT(child_rhs_shiftValue_Attribute != NULL);

                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
                       }
#if 0
                    printf ("Need to compute the stencil value for astNode = %p = %s \n",astNode,astNode->class_name().c_str());
                    ROSE_ASSERT(false);
#endif
                  }

               StencilUpdate_Attribute* stencilUpdateAstAttribute = dynamic_cast<StencilUpdate_Attribute*>(dslAstAttribute);
               if (stencilUpdateAstAttribute != NULL)
                  {
                 // return the Ones valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a stencilUpdate_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    StencilValue_Attribute* stencilValue_Attribute = new StencilValue_Attribute();
#if 0
                    printf ("Adding (StencilValue) stencilValue_Attribute = %p \n",stencilValue_Attribute);
#endif
#if 0
                    ROSE_ASSERT(stencilValue_Attribute != NULL);
                    astNode->addNewAttribute(StencilValue,stencilValue_Attribute);
#else
                 // Newer API.
                    add_dslValueAttribute(astNode,stencilValue_Attribute);
#endif
                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 2);
                    SgNode* childNode_lhs = dslAstAttribute->dslChildren[0];
                    SgNode* childNode_rhs = dslAstAttribute->dslChildren[1];
                    ROSE_ASSERT(childNode_lhs != NULL);
                    ROSE_ASSERT(childNode_rhs != NULL);

#if 1
                    StencilValue_Attribute* child_lhs_stencilValue_Attribute = get_StencilValueAttribute(childNode_lhs);
                    StencilValue_Attribute* child_rhs_stencilValue_Attribute = get_StencilValueAttribute(childNode_rhs);

                    stencilValue_Attribute->value = child_lhs_stencilValue_Attribute->value + child_rhs_stencilValue_Attribute->value;
#else
                    StencilValue_Attribute* child_lhs_stencilValue_Attribute = NULL;
                 // Now copy the value back from where it is present in the StencilUpdate_Attribute to the Stencil_Attribute.
                 // Not to the StencilValue_Attribute on the SgVarRefExp, but the StencilValue_Attribute on the SgInitializedName.
                    printf ("childNode_lhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str());
                    SgVarRefExp* varRefExp = isSgVarRefExp(childNode_lhs);
                    if (varRefExp != NULL)
                       {
                         SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
                         ROSE_ASSERT(variableSymbol != NULL);

                         SgInitializedName* initializedName = variableSymbol->get_declaration();
                         ROSE_ASSERT(initializedName != NULL);

                         VariableId varId = variableIdMapping.variableId(initializedName);
                         int varIdCode = varId.getIdCode();

                         VariableId varId_varRef = variableIdMapping.variableId(varRefExp);
                         int varIdCode_varRef = varId_varRef.getIdCode();

                         ROSE_ASSERT(varIdCode == varIdCode_varRef);

                         printf ("In lookup of StencilValue_Attribute: varIdCode = %d varIdCode_varRef = %d \n",varIdCode,varIdCode_varRef);

                         ROSE_ASSERT(stencilValueState.find(varId) != stencilValueState.end());
                         StencilValue_Attribute* variable_stencilValue_Attribute = stencilValueState[varId];
                      // StencilValue_Attribute* variable_stencilValue_Attribute = dynamic_cast<StencilValue_Attribute*>(initializedName->getAttribute(StencilValue));
                         ROSE_ASSERT(variable_stencilValue_Attribute != NULL);

                         child_lhs_stencilValue_Attribute = variable_stencilValue_Attribute;
#if 0
                         printf ("Need to find the StencilValue_Attribute associated with the SgInitializedName (from this varRefExp = %p = %s) \n",varRefExp,varRefExp->class_name().c_str());
                         ROSE_ASSERT(false);
#endif
                       }
                      else
                       {
                      // Not clear what case this is.
                         printf ("Finding the StencilValue_Attribute on childNode_lhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str());

                         child_lhs_stencilValue_Attribute = dynamic_cast<StencilValue_Attribute*>(childNode_lhs->getAttribute(StencilValue));

                         printf ("Need to copy data back to the StencilValue_Attribute on the SgInitializedName IR node (not implemented) \n");
                         ROSE_ASSERT(false);
                       }

                 // StencilValue_Attribute* child_lhs_stencilValue_Attribute = dynamic_cast<StencilValue_Attribute*>(childNode_lhs->getAttribute(StencilValue));
                    StencilValue_Attribute* child_rhs_stencilValue_Attribute = dynamic_cast<StencilValue_Attribute*>(childNode_rhs->getAttribute(StencilValue));
#if 1
                    printf ("child_lhs_stencilValue_Attribute = %p \n",child_lhs_stencilValue_Attribute);
                    printf ("child_rhs_stencilValue_Attribute = %p \n",child_rhs_stencilValue_Attribute);
#endif
                    if (child_lhs_stencilValue_Attribute != NULL && child_rhs_stencilValue_Attribute != NULL)
                       {
                      // Call the associated operator and assign result to this attribute.
                         printf ("Calling Stencil::operator+=(Stencil & X): with lhs array value and rhs Point value \n");
#ifdef DEBUG_SHIFT_CALCULUS
                         printf ("resulting child_lhs_stencilValue_Attribute->value: calling stencilDump() \n");
                         child_lhs_stencilValue_Attribute->value.stencilDump();
                         printf ("resulting child_rhs_stencilValue_Attribute->value: calling stencilDump() \n");
                         child_rhs_stencilValue_Attribute->value.stencilDump();
#endif
                      // The operator+=() is syntax that is not implemented, so we can't actually call it to evaluate the stencil value.
#if 1
                      // Runtime error: error - trying to add two stencils with different LHS shifts / ratios
                         stencilValue_Attribute->value = child_lhs_stencilValue_Attribute->value + child_rhs_stencilValue_Attribute->value;
#else
                      // Get this working using this (incorrect) code first.
                         stencilValue_Attribute->value = child_lhs_stencilValue_Attribute->value;
#endif
#ifdef DEBUG_SHIFT_CALCULUS
                         printf ("resulting stencilValue_Attribute->value: calling stencilDump() \n");
                         stencilValue_Attribute->value.stencilDump();
#endif
                      // Copy the side effects back the the StencilValue_Attribute on the SgInitializedName.
                         child_lhs_stencilValue_Attribute->value = stencilValue_Attribute->value;

#ifdef DEBUG_SHIFT_CALCULUS
                         printf ("\n##### After copy back to update the stencil variable: child_lhs_stencilValue_Attribute->value: calling stencilDump() \n");
                         child_lhs_stencilValue_Attribute->value.stencilDump();
#endif
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                      else
                       {
                         printf ("child_lhs_stencilValue_Attribute == NULL: childNode_lhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str());
                         printf ("child_rhs_stencilValue_Attribute == NULL: childNode_rhs = %p = %s \n",childNode_rhs,childNode_rhs->class_name().c_str());

                         ROSE_ASSERT(child_lhs_stencilValue_Attribute  != NULL);
                         ROSE_ASSERT(child_rhs_stencilValue_Attribute != NULL);

                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
                       }
                 // stencilValue_Attribute->value += rhs_stencil_value;
#endif

#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               OperatorShiftVec_Attribute* operatorShiftVecAstAttribute = dynamic_cast<OperatorShiftVec_Attribute*>(dslAstAttribute);
               if (operatorShiftVecAstAttribute != NULL)
                  {
                 // return the ShiftVec valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a OperatorShiftVec_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    ArrayValue_Attribute* shiftVecValue_Attribute = new ArrayValue_Attribute();
#if 0
                    printf ("Adding (ShiftVecValue) shiftVecValue_Attribute = %p \n",shiftVecValue_Attribute);
#endif
                    ROSE_ASSERT(shiftVecValue_Attribute != NULL);

                    astNode->addNewAttribute(ArrayValue,shiftVecValue_Attribute);

                 // Compute the constant value returned by the associated operator abstraction.
                    shiftVecValue_Attribute->value = getShiftVec();
                  }

               ConstantExpression_Attribute* constantExpressionAstAttribute = dynamic_cast<ConstantExpression_Attribute*>(dslAstAttribute);
               if (constantExpressionAstAttribute != NULL)
                  {
                 // return the Ones valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a ConstantExpression_Attribute \n");
#endif
                 // Since this is a const expression we want to get the constant. I will get the 
                 // new version of the constant propogation analysis on March 4th 2015. Until 
                 // then I will take the short cut of grabing the constant value directly for
                 // the few cases where I need this to support the DSL.

                    SgValueExp* valueExp = NULL;
                    SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
                 // ROSE_ASSERT(varRefExp != NULL);
                    if (varRefExp != NULL)
                       {
                         SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
                         ROSE_ASSERT(variableSymbol != NULL);

                         SgInitializedName* initializedName = variableSymbol->get_declaration();
                         ROSE_ASSERT(initializedName != NULL);
#if 1
                         printf ("The constant value needs to be retrieved from the constant initializedName's initializer = %p \n",initializedName->get_initptr());
#endif
                         SgAssignInitializer* assignInitializer = isSgAssignInitializer(initializedName->get_initptr());
                         ROSE_ASSERT(assignInitializer != NULL);

                         SgExpression* initializationExpression = assignInitializer->get_operand();
                         ROSE_ASSERT(initializationExpression != NULL);
#if 1
                         printf ("The constant value needs to be retrieved from the initializationExpression = %p = %s \n",initializationExpression,initializationExpression->class_name().c_str());
#endif
                         valueExp = isSgValueExp(initializationExpression);
                         ROSE_ASSERT(valueExp != NULL);
                       }
                      else
                       {
#if 1
                         printf ("case Detected a ConstantExpression_Attribute: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
                         valueExp = isSgValueExp(astNode);
                         ROSE_ASSERT(valueExp != NULL);
                       }

                    ROSE_ASSERT(valueExp != NULL);

                    SgDoubleVal* doubleVal = isSgDoubleVal(valueExp);
                    if (valueExp != NULL && doubleVal == NULL)
                       {
                         int value = SageInterface::getIntegerConstantValue(valueExp);

                         IntegerValue_Attribute* value_Attribute = new IntegerValue_Attribute();
                         ROSE_ASSERT(value_Attribute != NULL);
                         astNode->addNewAttribute(IntegerValue,value_Attribute);
                         
                      // Compute the constant value returned by the associated operator abstraction.
                         value_Attribute->value = value;
#if 1
                         printf ("Adding (IntegerValue) value_Attribute = %p value = %d \n",value_Attribute,value);
#endif
                       }
                      else
                       {
                      // Handle floating point valued constants.
#if 1
                         printf ("floating point valued identified: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
                         ROSE_ASSERT(doubleVal != NULL);
                         double value = doubleVal->get_value();

                         DoubleValue_Attribute* value_Attribute = new DoubleValue_Attribute();
                         ROSE_ASSERT(value_Attribute != NULL);
                         astNode->addNewAttribute(DoubleValue,value_Attribute);

                      // Compute the constant value returned by the associated operator abstraction.
                         value_Attribute->value = value;
#if 1
                         printf ("Adding (DoubleValue) value_Attribute = %p value = %f \n",value_Attribute,value);
#endif
                       }
#if 0
                    printf ("Exiting as a test at the end of the processing of the ConstantExpression_Attribute \n");
                    ROSE_ASSERT(false);
#endif
                  }

#if (USING_VARIABLE_ID_MAPPING == 0)
               PointVariableRefExp_Attribute* pointVariableRefExpAstAttribute = dynamic_cast<PointVariableRefExp_Attribute*>(dslAstAttribute);
               if (pointVariableRefExpAstAttribute != NULL)
                  {
                 // return the Zero valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a PointVariableRefExp_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
#if 0
                    printf ("Adding (PointValue) pointValue_Attribute = %p \n",pointValue_Attribute);
#endif
                    ROSE_ASSERT(pointValue_Attribute != NULL);

                    astNode->addNewAttribute(PointValue,pointValue_Attribute);

                 // Compute the constant value returned by the associated operator abstraction.
                 // pointValue_Attribute->value = getZeros();
                    SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
                    ROSE_ASSERT(varRefExp != NULL);

                    SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
                    ROSE_ASSERT(variableSymbol != NULL);

                    SgInitializedName* initializedName = variableSymbol->get_declaration();
                    ROSE_ASSERT(initializedName != NULL);

                    PointValue_Attribute* variable_pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(initializedName->getAttribute(PointValue));
                    ROSE_ASSERT(variable_pointValue_Attribute != NULL);

                 // Assign the constant Point value.
                    pointValue_Attribute->value = variable_pointValue_Attribute->value;
                  }
#endif

#if (USING_VARIABLE_ID_MAPPING == 0)
               ArrayVariableRefExp_Attribute* arrayVariableRefExpAstAttribute = dynamic_cast<ArrayVariableRefExp_Attribute*>(dslAstAttribute);
               if (arrayVariableRefExpAstAttribute != NULL)
                  {
                 // return the array valued Stencil value.
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a ArrayVariableRefExp_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    ArrayValue_Attribute* arrayValue_Attribute = new ArrayValue_Attribute();
#if 0
                    printf ("Adding (ArrayValue) arrayValue_Attribute = %p \n",arrayValue_Attribute);
#endif
                    ROSE_ASSERT(arrayValue_Attribute != NULL);

                    astNode->addNewAttribute(ArrayValue,arrayValue_Attribute);

                 // Compute the constant value returned by the associated operator abstraction.
                 // pointValue_Attribute->value = getZeros();
                    SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
                    ROSE_ASSERT(varRefExp != NULL);

                    SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
                    ROSE_ASSERT(variableSymbol != NULL);

                    SgInitializedName* initializedName = variableSymbol->get_declaration();
                    ROSE_ASSERT(initializedName != NULL);

                    ArrayValue_Attribute* variable_arrayValue_Attribute = dynamic_cast<ArrayValue_Attribute*>(initializedName->getAttribute(ArrayValue));
                    ROSE_ASSERT(variable_arrayValue_Attribute != NULL);

                 // Assign the constant array value.
                    arrayValue_Attribute->value = variable_arrayValue_Attribute->value;
                  }
#endif

#if (USING_VARIABLE_ID_MAPPING == 0)
               StencilVariableRefExp_Attribute* stencilVariableRefExpAstAttribute = dynamic_cast<StencilVariableRefExp_Attribute*>(dslAstAttribute);
               if (stencilVariableRefExpAstAttribute != NULL)
                  {
                 // return the array valued Stencil value.
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a ArrayVariableRefExp_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    StencilValue_Attribute* stencilValue_Attribute = new StencilValue_Attribute();
#if 0
                    printf ("Adding (StencilValue) stencilValue_Attribute = %p \n",stencilValue_Attribute);
#endif
                    ROSE_ASSERT(stencilValue_Attribute != NULL);

                    astNode->addNewAttribute(StencilValue,stencilValue_Attribute);

                 // Compute the constant value returned by the associated operator abstraction.
                 // pointValue_Attribute->value = getZeros();
                    SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
                    ROSE_ASSERT(varRefExp != NULL);

                    SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
                    ROSE_ASSERT(variableSymbol != NULL);

                    SgInitializedName* initializedName = variableSymbol->get_declaration();
                    ROSE_ASSERT(initializedName != NULL);

                    StencilValue_Attribute* variable_stencilValue_Attribute = dynamic_cast<StencilValue_Attribute*>(initializedName->getAttribute(StencilValue));
                    ROSE_ASSERT(variable_stencilValue_Attribute != NULL);

                 // Assign the constant array value.
                    stencilValue_Attribute->value = variable_stencilValue_Attribute->value;
                  }
#endif

               ForLoopOperator_Attribute* forLoopOperatorAstAttribute = dynamic_cast<ForLoopOperator_Attribute*>(dslAstAttribute);
               if (forLoopOperatorAstAttribute != NULL)
                  {
                 // This is the loop over all dimensions so that we can support stencils of 1D, 2D and 3D.  This step requires interpretation of
                 // the loop semantics, else we will only support 1D loop semantics.  I want to debug the 1D stencil support before moving
                 // to support the interprestaion of the loop body required for 2D and 3D stencil semantics.  The interpreation is just 
                 // a reevluation of the loop body using a different value for the index variable (so it uses the same infrastructure, namely 
                 // a nested traversal on the loop body with the loop index value updated).

#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a ForLoopOperator_Attribute \n");
#endif
                    printf ("Ignoring the for loop operator to support initial work on 1D stencils \n");
                  }

               StencilApplyOperator_Attribute* stencilApplyOperatorAstAttribute = dynamic_cast<StencilApplyOperator_Attribute*>(dslAstAttribute);
               if (stencilApplyOperatorAstAttribute != NULL)
                  {
                 // At this point we have a well-defined stencil operator, array abstractions, and a box domain on which to apply the stencil.
                 // Now we just have to translate the our ROSE DSL specific data structures so that we can support the compile-time code generation.

#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a StencilApplyOperator_Attribute \n");
#endif

                    printf ("Do the translation to the ROSE DSL data structures so that we can support code generation phase. \n");
                    ROSE_ASSERT(false);
                  }

#if 0
               if (dslAstAttribute->dslChildren.empty() == false)
                  {
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
                  }
#endif
             }

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

#if 0
     printf ("Leaving StencilEvaluationTraversal::evaluateSynthesizedAttribute(): return_synthesizedAttribute \n");
#endif

     return return_synthesizedAttribute;
   }


void StencilEvaluationTraversal::displayStencil(const string & label)
   {
  // This function outputs the data associated with each of the stencil in the evaluation.

     printf ("In StencilEvaluationTraversal::displayStencil(): label = %s \n",label.c_str());

     const string innerLabel = "stencil element";
     std::map<std::string,StencilFSM*>::iterator i = stencilMap.begin();
     while(i != stencilMap.end())
        {
          (*i).second->display(innerLabel);
          i++;
        }
   }

