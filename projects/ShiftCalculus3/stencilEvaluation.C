
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

using namespace SPRAY;

extern VariableIdMapping variableIdMapping;

// Controls output of debugging information as compile-time evaluation is done using 
// the Shift Calculus operations.
#define DEBUG_SHIFT_CALCULUS 0

// Define stencil DSL evaluation using more complete use of the variable ID mapping
// initial use was for the Stencil variables only.
// #define USING_VARIABLE_ID_MAPPING 1

#define DEBUG_DSL_ATTRIBUTES 0


using namespace std;
using namespace rose;

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


#if 0
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
#endif

StencilEvaluationTraversal::StencilEvaluationTraversal()
   {
#if 1
     printf ("In StencilEvaluationTraversal constructor: \n");
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
const string StencilEvaluationTraversal::DoubleValue  = dsl_value_string;
const string StencilEvaluationTraversal::ShiftValue   = dsl_value_string;
const string StencilEvaluationTraversal::ArrayValue   = dsl_value_string;
const string StencilEvaluationTraversal::StencilValue = dsl_value_string;

const string StencilEvaluationTraversal::BoxValue         = dsl_value_string;
const string StencilEvaluationTraversal::RectMDArrayValue = dsl_value_string;

// static data member
std::map<SPRAY::VariableId,DSL_ValueAttribute*> StencilEvaluationTraversal::dslValueState;


StencilEvaluation_InheritedAttribute
StencilEvaluationTraversal::evaluateInheritedAttribute (SgNode* astNode, StencilEvaluation_InheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("In StencilEvaluationTraversal::evaluateInheritedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

     SgForStatement* forStatement = isSgForStatement(astNode);
     if (forStatement != NULL)
        {
          ForLoopOperator_Attribute* forLoopAttribute = dynamic_cast<ForLoopOperator_Attribute*>(forStatement->getAttribute(DetectionTraversal::ForLoopOperator));
          if (forLoopAttribute != NULL)
             {
               SgInitializedName* indexVariable = SageInterface::getLoopIndexVariable(forStatement);
               ROSE_ASSERT(indexVariable != NULL);

               printf ("In StencilEvaluationTraversal::evaluateInheritedAttribute(): Identified the DSL ForLoopOperator: indexVariable = %p = %s \n",indexVariable,indexVariable->get_name().str());

               IntegerValue_Attribute* integerValue_Attribute = new IntegerValue_Attribute();
               add_dslValueAttribute(indexVariable,integerValue_Attribute);

               ROSE_ASSERT(get_dslValueAttribute(indexVariable) != NULL);

            // I am assuming that the DSL semantics are that the initializer is 0.
               integerValue_Attribute->value = 0;

               printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
               printf ("Compile-time evaluation of loop index is set to 0: integerValue_Attribute->value = %d \n",integerValue_Attribute->value);
               printf ("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$ \n");
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
        }



  // Construct the return attribute from the modified input attribute.
     return StencilEvaluation_InheritedAttribute(inheritedAttribute);
   }

bool
StencilEvaluationTraversal::isVariable ( SgNode* node )
   {
     return isSgVarRefExp(node) || isSgInitializedName(node);
   }

VariableId
StencilEvaluationTraversal::getVariableId ( SgNode* node )
   {
     VariableId varId;

     SgVarRefExp* varRefExp = isSgVarRefExp(node);
     SgInitializedName* initializedName = isSgInitializedName(node);
     if (varRefExp != NULL || initializedName != NULL)
        {
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
        }

     return varId;
   }


void
StencilEvaluationTraversal::add_dslValueAttribute ( SgNode* node, DSL_ValueAttribute* dslValueAttribute )
   {
     ROSE_ASSERT(node != NULL);
     ROSE_ASSERT(dslValueAttribute != NULL);

     if (isVariable(node) == true)
        {
          VariableId varId = getVariableId(node);
#if 0
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
#if 0
          printf ("Need to handle the case where node is not a SgVarRefExp: node = %p = %s \n",node,node->class_name().c_str());
#endif
       // All value strings are the same.
          if (node->getAttribute(dsl_value_string) != NULL)
             {
               printf ("Error: node = %p = %s already has an attribute (so it is a mistake to add another one) \n",node,node->class_name().c_str());
             }
          ROSE_ASSERT(node->getAttribute(dsl_value_string) == NULL);

          node->addNewAttribute(dsl_value_string,dslValueAttribute);

          ROSE_ASSERT(node->getAttribute(dsl_value_string) != NULL);
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

     if (isVariable(node) == true)
        {
          VariableId varId = getVariableId(node);
#if 0
          int varIdCode = varId.getIdCode();
          printf ("Get dsl attribute from dslValueState map: varIdCode = %d = %s (but using the VariableId) \n",varIdCode,variableIdMapping.variableName(varId).c_str());
#endif
       // DQ (2/21/2015): This appears to fail for the case of the DSL ForLoop index (because the
       // nested traversal used a different dslValueMap, this is not fixed to be a static map).
       // ROSE_ASSERT(dslValueState.find(varId) != dslValueState.end());
          if (dslValueState.find(varId) != dslValueState.end())
             {
               dslValueAttribute = dslValueState[varId];
               ROSE_ASSERT(dslValueAttribute != NULL);
             }
            else
             {
#if 0
               printf ("In get_dslValueAttribute(node = %p = %s): returing dslValueAttribute == NULL \n",node,node->class_name().c_str());
#endif
               dslValueAttribute = NULL;
             }
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
       else
        {
#if 0
          printf ("Need to handle the case where node is not a SgVarRefExp: node = %p = %s \n",node,node->class_name().c_str());
#endif
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

  // DQ (2/22/2015): To support nested traversals (evaluation), we have to allow this function to return NULL pointer.
  // ROSE_ASSERT(valueAttribute != NULL);

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

BoxValue_Attribute*
StencilEvaluationTraversal::get_BoxValueAttribute ( SgNode* node )
   {
  // This function is type specific and calls the lower level support function: get_dslValueAttribute().

     BoxValue_Attribute* valueAttribute = dynamic_cast<BoxValue_Attribute*>(get_dslValueAttribute(node));
  // ROSE_ASSERT(valueAttribute != NULL);

     return valueAttribute;
   }

RectMDArrayValue_Attribute*
StencilEvaluationTraversal::get_RectMDArrayValueAttribute ( SgNode* node )
   {
  // This function is type specific and calls the lower level support function: get_dslValueAttribute().

     RectMDArrayValue_Attribute* valueAttribute = dynamic_cast<RectMDArrayValue_Attribute*>(get_dslValueAttribute(node));
  // ROSE_ASSERT(valueAttribute != NULL);

     return valueAttribute;
   }




StencilEvaluation_SynthesizedAttribute
StencilEvaluationTraversal::evaluateSynthesizedAttribute (SgNode* astNode, StencilEvaluation_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     StencilEvaluation_SynthesizedAttribute return_synthesizedAttribute;

#if 0
     printf ("In StencilEvaluationTraversal::evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

  // Look for DSL attributes so that we can support compile-time evaluation of DSL abstractions.
     AstAttributeMechanism* astAttributeContainer = astNode->get_attributeMechanism();
     if (astAttributeContainer != NULL)
        {
#if 0
       // This assertion fails for a nested traversal because some IR nodes will have been given a "dsl_value" 
       // attribute and thus there will be IR nodes with 2 attributes. Note: the nested traversal happens in 
       // the evaluation of the DSL for loop construct.
       // I think there should only be one DSL attribute, in the future we can support more on a single IR node.
          if (astAttributeContainer->size() != 1)
             {
               printf ("WARNING: astAttributeContainer->size() != 1: astAttributeContainer->size() = %d \n",astAttributeContainer->size());
             }
       // ROSE_ASSERT(astAttributeContainer->size() == 1);
#endif

#if 1
       // Example iterating over attributes using the SgNode attribute API.
       // BOOST_FOREACH (const std::string &attributeName, obj_1->get_attributeMechanism()->getAttributeIdentifiers()) 
          BOOST_FOREACH (const std::string &attributeName, astAttributeContainer->getAttributeIdentifiers()) 
             {
               std::cout << "attribute \"" << StringUtility::cEscape(attributeName) << "\" pointer is " << astNode->getAttribute(attributeName) << "\n";
             }

          printf ("Implementation using new attribute API not implemented! \n");
          ROSE_ASSERT(false);
#else
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

               Point_Attribute* pointAstAttribute = dynamic_cast<Point_Attribute*>(dslAstAttribute);
               if (pointAstAttribute != NULL)
                  {
                 // Set the value of the point in the attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a Point_Attribute \n");
#endif
                 // PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
                 // ROSE_ASSERT(pointValue_Attribute != NULL);
                 // add_dslValueAttribute(astNode,pointValue_Attribute);
                 // PointValue_Attribute* pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(astNode->getAttribute(PointValue));
                    PointValue_Attribute* pointValue_Attribute = get_PointValueAttribute(astNode);
#if 0
                    printf ("pointValue_Attribute = %p \n",pointValue_Attribute);
#endif
                    if (pointValue_Attribute == NULL)
                       {
                         pointValue_Attribute = new PointValue_Attribute();
                         ROSE_ASSERT(pointValue_Attribute != NULL);
                      // astNode->addNewAttribute(PointValue,pointValue_Attribute);
                         add_dslValueAttribute(astNode,pointValue_Attribute);
                       }

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);
                    PointValue_Attribute* child_pointValue_Attribute = get_PointValueAttribute(childNode);

                 // Assign the child attribute value to the attribute value on the current astNode.
                    pointValue_Attribute->value = child_pointValue_Attribute->value;
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               Array_Attribute* arrayAstAttribute = dynamic_cast<Array_Attribute*>(dslAstAttribute);
               if (arrayAstAttribute != NULL)
                  {
                 // Set the value of the point in the attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a Array_Attribute \n");
#endif
                    ArrayValue_Attribute* arrayValue_Attribute = new ArrayValue_Attribute();
                    ROSE_ASSERT(arrayValue_Attribute != NULL);
                    add_dslValueAttribute(astNode,arrayValue_Attribute);

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);
                    ArrayValue_Attribute* child_arrayValue_Attribute = get_ArrayValueAttribute(childNode);

                 // Assign the child attribute value to the attribute value on the current astNode.
                    arrayValue_Attribute->value = child_arrayValue_Attribute->value;
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               Stencil_Attribute* stencilAstAttribute = dynamic_cast<Stencil_Attribute*>(dslAstAttribute);
               if (stencilAstAttribute != NULL)
                  {
                 // Set the value of the stencil in the attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a Stencil_Attribute \n");
#endif
                    StencilValue_Attribute* stencilValue_Attribute = new StencilValue_Attribute();
                    ROSE_ASSERT(stencilValue_Attribute != NULL);
                    add_dslValueAttribute(astNode,stencilValue_Attribute);

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 1);
                    SgNode* childNode = dslAstAttribute->dslChildren[0];
                    ROSE_ASSERT(childNode != NULL);
                    StencilValue_Attribute* child_stencilValue_Attribute = get_StencilValueAttribute(childNode);

                 // Assign the child attribute value to the attribute value on the current astNode.
                    stencilValue_Attribute->value = child_stencilValue_Attribute->value;
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }

               OperatorZero_Attribute* operatorZeroAstAttribute = dynamic_cast<OperatorZero_Attribute*>(dslAstAttribute);
               if (operatorZeroAstAttribute != NULL)
                  {
                 // return the Zero valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a OperatorZero_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                    PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
                    ROSE_ASSERT(pointValue_Attribute != NULL);

                    astNode->addNewAttribute(PointValue,pointValue_Attribute);

                 // Compute the constant value returned by the associated operator abstraction.
                    pointValue_Attribute->value = getZeros();

#if DEBUG_SHIFT_CALCULUS
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
                    ROSE_ASSERT(pointValue_Attribute != NULL);

                    astNode->addNewAttribute(PointValue,pointValue_Attribute);

                 // Compute the constant value returned by the associated operator abstraction.
                    pointValue_Attribute->value = getOnes();

#if DEBUG_SHIFT_CALCULUS
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
                 // PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
                 // ROSE_ASSERT(pointValue_Attribute != NULL);
                 // astNode->addNewAttribute(PointValue,pointValue_Attribute);

                 // DQ (2/21/2015): Because we call this traversal recursively, this needs to check for an existing attribute (and reuse it).
                    PointValue_Attribute* pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(astNode->getAttribute(PointValue));
                    if (pointValue_Attribute == NULL)
                       {
                         pointValue_Attribute = new PointValue_Attribute();
                         ROSE_ASSERT(pointValue_Attribute != NULL);
                      // astNode->addNewAttribute(PointValue,pointValue_Attribute);
                         add_dslValueAttribute(astNode,pointValue_Attribute);
                       }

                 // This value need to be interpreted (later).
                 // int dimension = 0;

                 // Find the associated value attribute for the input paramter.
                    printf ("Find the associated value attribute for the input paramter \n");

                    SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
                    ROSE_ASSERT(functionCallExp != NULL);

                    ROSE_ASSERT(functionCallExp->get_args() != NULL);
                    ROSE_ASSERT(functionCallExp->get_args()->get_expressions().size() == 1);
                    SgExpression* functionArgument = functionCallExp->get_args()->get_expressions()[0];
                    ROSE_ASSERT(functionArgument != NULL);

                    SgVarRefExp* varRefExp = isSgVarRefExp(functionArgument);
                    ROSE_ASSERT(varRefExp != NULL);

                    SgVariableSymbol* varRefExp_symbol = varRefExp->get_symbol();
                    ROSE_ASSERT(varRefExp_symbol != NULL);
#if 0
                    printf ("varRefExp = %p = %s \n",varRefExp,varRefExp_symbol->get_name().str());
#endif
                    IntegerValue_Attribute* integerValue_Attribute = get_IntegerValueAttribute(varRefExp);
                    ROSE_ASSERT(integerValue_Attribute != NULL);

                    int dimension = integerValue_Attribute->value;
#if 0
                    printf ("Calling getUnitv(dimension): dimension = %d \n",dimension);
#endif
                 // Compute the constant value returned by the associated operator abstraction.
                    pointValue_Attribute->value = getUnitv(dimension);

#if DEBUG_SHIFT_CALCULUS
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
                 // DQ (2/22/2015): Check for existing attribute to support nested evaluation.
                    ShiftValue_Attribute* shiftValue_Attribute = dynamic_cast<ShiftValue_Attribute*>(astNode->getAttribute(ShiftValue));
                    if (shiftValue_Attribute == NULL)
                       {
                      // Add a dsl_value attribute.
                         shiftValue_Attribute = new ShiftValue_Attribute();
                         add_dslValueAttribute(astNode,shiftValue_Attribute);
                       }

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 2);
                    SgNode* childNode_lhs = dslAstAttribute->dslChildren[0];
                    SgNode* childNode_rhs = dslAstAttribute->dslChildren[1];
                    ROSE_ASSERT(childNode_lhs != NULL);
                    ROSE_ASSERT(childNode_rhs != NULL);

                    ArrayValue_Attribute* child_lhs_arrayValue_Attribute = get_ArrayValueAttribute(childNode_lhs);
                    PointValue_Attribute* child_rhs_pointValue_Attribute = get_PointValueAttribute(childNode_rhs);

#if DEBUG_SHIFT_CALCULUS
                    printf ("Before call to operator^(): child_rhs_pointValue_Attribute->value: \n");
                    child_rhs_pointValue_Attribute->value.print();
#endif
                    shiftValue_Attribute->value = operator^(child_lhs_arrayValue_Attribute->value,child_rhs_pointValue_Attribute->value);

#if DEBUG_SHIFT_CALCULUS
                    printf ("After call to operator^(): shiftValue_Attribute->value: \n");
                    shiftValue_Attribute->value.print();
#endif
                  }
               
               PointOperatorMultiply_Attribute* pointOperatorMultiplyAstAttribute = dynamic_cast<PointOperatorMultiply_Attribute*>(dslAstAttribute);
               if (pointOperatorMultiplyAstAttribute != NULL)
                  {
                 // return the Ones valued vector in the synthesized attribute?
#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a PointOperatorMultiply_Attribute \n");
#endif
                 // Add a dsl_value attribute.
                 // PointValue_Attribute* pointValue_Attribute = new PointValue_Attribute();
                 // add_dslValueAttribute(astNode,pointValue_Attribute);

                 // DQ (2/21/2015): Because we call this traversal recursively, this needs to check for an existing attribute (and reuse it).
                    PointValue_Attribute* pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(astNode->getAttribute(PointValue));
                    if (pointValue_Attribute == NULL)
                       {
                         pointValue_Attribute = new PointValue_Attribute();
                         ROSE_ASSERT(pointValue_Attribute != NULL);
                      // astNode->addNewAttribute(PointValue,pointValue_Attribute);
                         add_dslValueAttribute(astNode,pointValue_Attribute);
                       }

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 2);
                    SgNode* childNode_lhs = dslAstAttribute->dslChildren[0];
                    SgNode* childNode_rhs = dslAstAttribute->dslChildren[1];
                    ROSE_ASSERT(childNode_lhs != NULL);
                    ROSE_ASSERT(childNode_rhs != NULL);
#if 0
                    printf ("Processing PointOperatorMultiply: childNode_lhs = %p = %s childNode_rhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str(),childNode_rhs,childNode_rhs->class_name().c_str());
#endif
                 // Find the point value attribute on the child (one of these is a scalar).
                 // PointValue_Attribute* child_lhs_pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(childNode_lhs->getAttribute(PointValue));
                 // PointValue_Attribute* child_rhs_pointValue_Attribute = dynamic_cast<PointValue_Attribute*>(childNode_rhs->getAttribute(PointValue));

                    PointValue_Attribute* child_lhs_pointValue_Attribute = get_PointValueAttribute(childNode_lhs);
#if 0
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
                 // StencilValue_Attribute* stencilValue_Attribute = new StencilValue_Attribute();
                 // ROSE_ASSERT(stencilValue_Attribute != NULL);
                 // astNode->addNewAttribute(StencilValue,stencilValue_Attribute);
                 // DQ (2/22/2015): Check for existing attribute to support nested evaluation.
                    StencilValue_Attribute* stencilValue_Attribute = dynamic_cast<StencilValue_Attribute*>(astNode->getAttribute(StencilValue));
                    if (stencilValue_Attribute == NULL)
                       {
                      // Add a dsl_value attribute.
                         stencilValue_Attribute = new StencilValue_Attribute();
                         add_dslValueAttribute(astNode,stencilValue_Attribute);
                       }
#if 0
                    printf ("Adding (StencilValue) stencilValue_Attribute = %p \n",stencilValue_Attribute);
#endif
                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 2);
                    SgNode* childNode_lhs = dslAstAttribute->dslChildren[0];
                    SgNode* childNode_rhs = dslAstAttribute->dslChildren[1];
                    ROSE_ASSERT(childNode_lhs != NULL);
                    ROSE_ASSERT(childNode_rhs != NULL);
#if 0
                    printf ("childNode_lhs = %p = %s \n",childNode_lhs,childNode_lhs->class_name().c_str());
                    printf ("childNode_rhs = %p = %s \n",childNode_rhs,childNode_rhs->class_name().c_str());
#endif

#if 0
                 // DQ (2/22/2015):
                 // This version of the code does not work yet because it will require the constant propagation (coming shortly).
                 // The reason is that the lhs DoubleValue_Attribute is associated with a C++ constant and not a DSL specific 
                 // constant.

                    DoubleValue_Attribute* child_lhs_doubleValue_Attribute = get_DoubleValueAttribute(childNode_lhs);
                    ShiftValue_Attribute*  child_rhs_shiftValue_Attribute  = get_ShiftValueAttribute(childNode_rhs);

                    ROSE_ASSERT(child_lhs_doubleValue_Attribute != NULL);
                    ROSE_ASSERT(child_rhs_shiftValue_Attribute  != NULL);

#if DEBUG_SHIFT_CALCULUS
                    printf ("Calling Stencil operator*(double,Shift & X): with lhs double value and rhs Shift value \n");
                    printf ("child_lhs_doubleValue_Attribute->value = %f \n",child_lhs_doubleValue_Attribute->value);
                    child_rhs_shiftValue_Attribute->value.print();
#endif

                    stencilValue_Attribute->value = child_lhs_doubleValue_Attribute->value * child_rhs_shiftValue_Attribute->value;

#if DEBUG_SHIFT_CALCULUS
                    printf ("resulting stencilValue_Attribute->value: calling stencilDump() \n");
                    stencilValue_Attribute->value.stencilDump();
#endif
#else
                 // DQ (2/22/2015):
                 // This is the only correct version of the code until we have better constant propagation in place (March 4th, 2015).
                 // printf ("############### This could/should call the refactored code! ############### \n");

                    DoubleValue_Attribute* child_lhs_doubleValue_Attribute = dynamic_cast<DoubleValue_Attribute*>(childNode_lhs->getAttribute(DoubleValue));
                    ShiftValue_Attribute*  child_rhs_shiftValue_Attribute  = dynamic_cast<ShiftValue_Attribute*>(childNode_rhs->getAttribute(ShiftValue));
#if 0
                    printf ("child_lhs_doubleValue_Attribute = %p \n",child_lhs_doubleValue_Attribute);
                    printf ("child_rhs_shiftValue_Attribute  = %p \n",child_rhs_shiftValue_Attribute);
#endif
                    if (child_lhs_doubleValue_Attribute != NULL && child_rhs_shiftValue_Attribute != NULL)
                       {
                      // Call the associated operator and assign result to this attribute.
#if DEBUG_SHIFT_CALCULUS
                         printf ("Calling Stencil operator*(double,Shift & X): with lhs double value and rhs Shift value \n");
                         printf ("child_lhs_doubleValue_Attribute->value = %f \n",child_lhs_doubleValue_Attribute->value);
                         child_rhs_shiftValue_Attribute->value.print();
#endif
                         stencilValue_Attribute->value = child_lhs_doubleValue_Attribute->value * child_rhs_shiftValue_Attribute->value;

#if DEBUG_SHIFT_CALCULUS
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
#endif
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
                 // StencilValue_Attribute* stencilValue_Attribute = new StencilValue_Attribute();
                 // add_dslValueAttribute(astNode,stencilValue_Attribute);
                 // DQ (2/22/2015): Check for existing attribute to support nested evaluation.
                    StencilValue_Attribute* stencilValue_Attribute = dynamic_cast<StencilValue_Attribute*>(astNode->getAttribute(StencilValue));
                    if (stencilValue_Attribute == NULL)
                       {
                      // Add a dsl_value attribute.
                         stencilValue_Attribute = new StencilValue_Attribute();
                         add_dslValueAttribute(astNode,stencilValue_Attribute);
                       }

                    ROSE_ASSERT(dslAstAttribute->dslChildren.size() == 2);
                    SgNode* childNode_lhs = dslAstAttribute->dslChildren[0];
                    SgNode* childNode_rhs = dslAstAttribute->dslChildren[1];
                    ROSE_ASSERT(childNode_lhs != NULL);
                    ROSE_ASSERT(childNode_rhs != NULL);

                    StencilValue_Attribute* child_lhs_stencilValue_Attribute = get_StencilValueAttribute(childNode_lhs);
                    StencilValue_Attribute* child_rhs_stencilValue_Attribute = get_StencilValueAttribute(childNode_rhs);

#if DEBUG_SHIFT_CALCULUS
                    printf ("resulting child_lhs_stencilValue_Attribute->value: calling stencilDump() \n");
                    child_lhs_stencilValue_Attribute->value.stencilDump();
                    printf ("resulting child_rhs_stencilValue_Attribute->value: calling stencilDump() \n");
                    child_rhs_stencilValue_Attribute->value.stencilDump();
#endif
                    stencilValue_Attribute->value = child_lhs_stencilValue_Attribute->value + child_rhs_stencilValue_Attribute->value;

#if DEBUG_SHIFT_CALCULUS
                    printf ("resulting stencilValue_Attribute->value: calling stencilDump() \n");
                    stencilValue_Attribute->value.stencilDump();
#endif
                 // Copy the side effects back the the StencilValue_Attribute on the SgInitializedName.
                    child_lhs_stencilValue_Attribute->value = stencilValue_Attribute->value;

#if DEBUG_SHIFT_CALCULUS
                    printf ("\n##### After copy back to update the stencil variable: child_lhs_stencilValue_Attribute->value: calling stencilDump() \n");
                    child_lhs_stencilValue_Attribute->value.stencilDump();
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
#if 1
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
#if 0
                         printf ("The constant value needs to be retrieved from the constant initializedName's initializer = %p \n",initializedName->get_initptr());
#endif
                         SgAssignInitializer* assignInitializer = isSgAssignInitializer(initializedName->get_initptr());
                         ROSE_ASSERT(assignInitializer != NULL);

                         SgExpression* initializationExpression = assignInitializer->get_operand();
                         ROSE_ASSERT(initializationExpression != NULL);
#if 0
                         printf ("The constant value needs to be retrieved from the initializationExpression = %p = %s \n",initializationExpression,initializationExpression->class_name().c_str());
#endif
                         valueExp = isSgValueExp(initializationExpression);
                         ROSE_ASSERT(valueExp != NULL);
                       }
                      else
                       {
#if 0
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

                      // DQ (2/21/2015): Because we call this traversal recursively, this needs to check for an existing attribute (and reuse it).
                      // IntegerValue_Attribute* value_Attribute = new IntegerValue_Attribute();
                      // ROSE_ASSERT(value_Attribute != NULL);
                      // astNode->addNewAttribute(IntegerValue,value_Attribute);
                         IntegerValue_Attribute* value_Attribute = dynamic_cast<IntegerValue_Attribute*>(astNode->getAttribute(IntegerValue));
                         if (value_Attribute == NULL)
                            {
                              value_Attribute = new IntegerValue_Attribute();
                              ROSE_ASSERT(value_Attribute != NULL);
                              astNode->addNewAttribute(IntegerValue,value_Attribute);
                            }
                         
                      // Compute the constant value returned by the associated operator abstraction.
                         value_Attribute->value = value;
#if 0
                         printf ("Adding (IntegerValue) value_Attribute = %p value = %d \n",value_Attribute,value);
#endif
                       }
                      else
                       {
                      // Handle floating point valued constants.
#if 0
                         printf ("floating point valued identified: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
                         ROSE_ASSERT(doubleVal != NULL);
                         double value = doubleVal->get_value();

                      // DQ (2/22/2015): We need to allow for nested traversals (evaluations) so there might already be an attribute available.
                      // DoubleValue_Attribute* value_Attribute = new DoubleValue_Attribute();
                      // ROSE_ASSERT(value_Attribute != NULL);
                      // astNode->addNewAttribute(DoubleValue,value_Attribute);
                         DoubleValue_Attribute* value_Attribute = dynamic_cast<DoubleValue_Attribute*>(astNode->getAttribute(DoubleValue));
                         if (value_Attribute == NULL)
                            {
                              value_Attribute = new DoubleValue_Attribute();
                              ROSE_ASSERT(value_Attribute != NULL);
                              astNode->addNewAttribute(DoubleValue,value_Attribute);
                            }

                      // Compute the constant value returned by the associated operator abstraction.
                         value_Attribute->value = value;
#if 0
                         printf ("Adding (DoubleValue) value_Attribute = %p value = %f \n",value_Attribute,value);
#endif
                       }
#if 0
                    printf ("Exiting as a test at the end of the processing of the ConstantExpression_Attribute \n");
                    ROSE_ASSERT(false);
#endif
                  }

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
                 // Evaluate the block again by incrementing the value of the index variable.
                    SgForStatement* forStatement = isSgForStatement(astNode);
                    ROSE_ASSERT(forStatement != NULL);

                    SgInitializedName* indexVariable = SageInterface::getLoopIndexVariable(forStatement);
                    ROSE_ASSERT(indexVariable != NULL);

                    printf ("Idendified the indexVariable = %p = %s \n",indexVariable,indexVariable->get_name().str());

                    IntegerValue_Attribute* loopIndexValueAttribute = get_IntegerValueAttribute(indexVariable);
                    ROSE_ASSERT(loopIndexValueAttribute != NULL);

                    SgStatement* loopBody = forStatement->get_loop_body();
                    ROSE_ASSERT(loopBody != NULL);

                 // Build the inherited attribute
                    StencilEvaluation_InheritedAttribute inheritedAttribute_stencilEval;

                 // Define the traversal
                 // StencilEvaluationTraversal shiftCalculus_StencilEvaluationTraversal();
                    StencilEvaluationTraversal shiftCalculus_StencilEvaluationTraversal;

                    SgExprStatement* testExprStatement = isSgExprStatement(forStatement->get_test());
                    ROSE_ASSERT(testExprStatement != NULL);

                 // SgExpression* testExpression = forStatement->get_test();
                    SgExpression* testExpression = testExprStatement->get_expression();
                    ROSE_ASSERT(testExpression != NULL);
                    SgBinaryOp* binaryOperator = isSgBinaryOp(testExpression);
                    ROSE_ASSERT(binaryOperator != NULL);

                    ROSE_ASSERT(isSgLessThanOp(binaryOperator) != NULL);

                 // This is the expression "dim < DIM", the rhs is a constant integer value.
                    SgIntVal* integerValue = isSgIntVal(binaryOperator->get_rhs_operand());
                    ROSE_ASSERT(integerValue != NULL);

                    int upperBound = integerValue->get_value();

                    printf ("get the upper bound value = %d \n",upperBound);

                    for (int i = 1; i < upperBound; i++)
                       {
#if 1
                         printf ("@@@@@@@@@@@@@@@@@@@@@@@ START OF DSL FOR LOOP @@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
                         loopIndexValueAttribute->value = i;
#if 1
                         printf ("Nested call the StencilEvaluation traversal starting at the SgForStatement node of the AST: loop index = %d \n",loopIndexValueAttribute->value);
#endif
                      // Call the traversal starting at the project (root) node of the AST
                      // StencilEvaluation_SynthesizedAttribute result_stencilEval = shiftCalculus_StencilEvaluationTraversal.traverse(project,inheritedAttribute_stencilEval);
                      // StencilEvaluation_SynthesizedAttribute result_stencilEval = shiftCalculus_StencilEvaluationTraversal.traverse(loopBody,inheritedAttribute_stencilEval);
                         shiftCalculus_StencilEvaluationTraversal.traverse(loopBody,inheritedAttribute_stencilEval);
#if 1
                         printf ("@@@@@@@@@@@@@@@@@@@@@@@ END OF DSL FOR LOOP @@@@@@@@@@@@@@@@@@@@@@@@ \n");
                      // printf ("@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ \n");
#endif
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

               StencilApplyOperator_Attribute* stencilApplyOperatorAstAttribute = dynamic_cast<StencilApplyOperator_Attribute*>(dslAstAttribute);
               if (stencilApplyOperatorAstAttribute != NULL)
                  {
                 // At this point we have a well-defined stencil operator, array abstractions, and a box domain on which to apply the stencil.
                 // Now we just have to translate the our ROSE DSL specific data structures so that we can support the compile-time code generation.

#if DEBUG_DSL_ATTRIBUTES
                    printf ("   --- Detected a StencilApplyOperator_Attribute \n");
#endif
#if 0
                    printf ("Evaluate the input arguments to the stencil apply operator \n");
#endif
                    SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
                    ROSE_ASSERT(functionCallExp != NULL);

                    ROSE_ASSERT(functionCallExp->get_args() != NULL);
                    ROSE_ASSERT(functionCallExp->get_args()->get_expressions().size() == 4);
                    SgExpression* stencilArgument = functionCallExp->get_args()->get_expressions()[0];
                    ROSE_ASSERT(stencilArgument != NULL);

                    SgVarRefExp* stencilVarRefExp = isSgVarRefExp(stencilArgument);
                    ROSE_ASSERT(stencilVarRefExp != NULL);

                    StencilValue_Attribute* child_stencilValue_Attribute = get_StencilValueAttribute(stencilVarRefExp);
                    ROSE_ASSERT(child_stencilValue_Attribute != NULL);

// #if DEBUG_SHIFT_CALCULUS
#if 1
                    printf ("In stencil apply operator child_stencilValue_Attribute->value: calling stencilDump() \n");
                    child_stencilValue_Attribute->value.stencilDump();
#endif

#if 1
                 // These DSL abstractions are not meant to be evaluated at compile-time.
                    SgExpression* rectMDArrayArgument_src = functionCallExp->get_args()->get_expressions()[1];
                    ROSE_ASSERT(rectMDArrayArgument_src != NULL);
                    SgVarRefExp* rectMDArrayVarRefExp_src = isSgVarRefExp(rectMDArrayArgument_src);
                    ROSE_ASSERT(rectMDArrayVarRefExp_src != NULL);
                    RectMDArrayValue_Attribute* child_rectMDArrayValue_src_Attribute = get_RectMDArrayValueAttribute(rectMDArrayVarRefExp_src);

                    SgExpression* rectMDArrayArgument_dest = functionCallExp->get_args()->get_expressions()[2];
                    ROSE_ASSERT(rectMDArrayArgument_dest != NULL);
                    SgVarRefExp* rectMDArrayVarRefExp_dest = isSgVarRefExp(rectMDArrayArgument_dest);
                    ROSE_ASSERT(rectMDArrayVarRefExp_dest != NULL);
                    RectMDArrayValue_Attribute* child_rectMDArrayValue_dest_Attribute = get_RectMDArrayValueAttribute(rectMDArrayVarRefExp_dest);

                 // Because these are not evaluated at compile-time, we don't have or need value attributes.
                    ROSE_ASSERT(child_rectMDArrayValue_src_Attribute == NULL);
                    ROSE_ASSERT(child_rectMDArrayValue_dest_Attribute == NULL);
#endif
#if 1
                 // These DSL abstractions are not meant to be evaluated at compile-time.
                    SgExpression* boxArgument = functionCallExp->get_args()->get_expressions()[3];
                    ROSE_ASSERT(boxArgument != NULL);
                    SgVarRefExp* boxVarRefExp = isSgVarRefExp(boxArgument);
                    ROSE_ASSERT(boxVarRefExp != NULL);
                    BoxValue_Attribute* child_boxValue_Attribute = get_BoxValueAttribute(boxVarRefExp);

                 // Because this is not evaluated at compile-time, we don't have or need a value attribute.
                    ROSE_ASSERT(child_boxValue_Attribute == NULL);
#endif

                 // Recode this as a stencil operator for the code generation phase.
                    stencilOperatorFunctionCallList.push_back(functionCallExp);


                    string stencilName = stencilVarRefExp->get_symbol()->get_name();
                    printf ("stencilName = %s \n",stencilName.c_str());


                 // stencilMap[stencilName] = new StencilFSM();
                    StencilFSM* stencilFSM = new StencilFSM();
                    ROSE_ASSERT(stencilFSM != NULL);

                    stencilMap[stencilName] = stencilFSM;

                 // Interpret the Shift Calculus stencil to build the StencilFSM
                    Stencil<double> & stencil = child_stencilValue_Attribute->value;
                    for (size_t k = 0; k < stencil.m_coef.size(); k++)
                       {
#if 0
                         cout << k << " , "<< stencil.m_coef[k] << " , ";
                         for (int dir = 0;dir < DIM; dir++)
                            {
                              cout << stencil.m_offsets[k][dir] << " ";
                            }
                         cout << " ; " ;
#endif
                         printf ("add stencil element: k = %d \n",k);

                         stencilFSM->stencilPointList.push_back(std::pair<StencilOffsetFSM,double>(StencilOffsetFSM(stencil.m_offsets[k][0],stencil.m_offsets[k][1],stencil.m_offsets[k][2]),stencil.m_coef[k]));

                       }
#if 0
                    printf ("Do the translation to the ROSE DSL data structures so that we can support code generation phase. \n");
                    ROSE_ASSERT(false);
#endif
                  }
#if 0
               if (dslAstAttribute->dslChildren.empty() == false)
                  {
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
                  }
#endif
             }
#endif

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

