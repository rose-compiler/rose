
#include "rose.h"

#include "rose_config.h"

// This code will make calls to the finite state machine representing the stencil 
// so that we can execute events and accumulate state (and then read the state as
// and intermediate form for the stencil (maybe saved as an attribute).  This data
// is then the jumping off point for different groups to experiment with the generation
// of architecture specific code.
#include "operatorFiniteStateMachine.h"

#include "operatorEvaluation.h"

#include "dslSupport.h"

#include "mapleOperatorAPI.h"

// Added because we call this from the SgExprStatement in the synthisized attribute evaluate.
#include "dslCodeGeneration.h"

#include "callMaple.h"

using namespace std;

using namespace DSL_Support;

OperatorEvaluation_InheritedAttribute::OperatorEvaluation_InheritedAttribute()
   {
     isShiftExpression = false;
     discretizationFSM = NULL;
   }

OperatorEvaluation_InheritedAttribute::OperatorEvaluation_InheritedAttribute( const OperatorEvaluation_InheritedAttribute & X )
   {
     isShiftExpression = X.isShiftExpression;
     discretizationFSM = X.discretizationFSM;
   }

void
OperatorEvaluation_InheritedAttribute::set_ShiftExpression(bool value)
   {
     isShiftExpression = value;
   }

bool
OperatorEvaluation_InheritedAttribute::get_ShiftExpression()
   {
     return isShiftExpression;
   }


OperatorEvaluation_SynthesizedAttribute::OperatorEvaluation_SynthesizedAttribute()
   {
     operatorTransformed = false;
     discretizationFSM   = NULL;

  // OperatorFSM operatorApplication;
     operatorApplicationOperand = NULL;
     operatorApplicationLhs     = NULL;
   }

OperatorEvaluation_SynthesizedAttribute::OperatorEvaluation_SynthesizedAttribute( const OperatorEvaluation_SynthesizedAttribute & X )
   {
     operatorTransformed = X.operatorTransformed;
     discretizationFSM   = X.discretizationFSM;
     operatorList        = X.operatorList;

  // This is used to hold the information required to apply the operator to a RHS and assignment to a LHS.
     operatorApplication        = X.operatorApplication;
     operatorApplicationOperand = X.operatorApplicationOperand;
     operatorApplicationLhs     = X.operatorApplicationLhs;
   }

void
OperatorEvaluation_SynthesizedAttribute::set_operatorTransformed(bool value)
   {
     operatorTransformed = value;
   }

bool
OperatorEvaluation_SynthesizedAttribute::get_operatorTransformed()
   {
     return operatorTransformed;
   }


OperatorEvaluationTraversal::OperatorEvaluationTraversal()
   {
   }


std::vector<SgFunctionCallExp*> & 
OperatorEvaluationTraversal::get_operatorFunctionCallList()
   {
     return operatorFunctionCallList;
   }


std::map<std::string,OperatorFSM*> & 
OperatorEvaluationTraversal::get_operatorMap()
   {
     return operatorMap;
   }


OperatorEvaluation_InheritedAttribute
OperatorEvaluationTraversal::evaluateInheritedAttribute (SgNode* astNode, OperatorEvaluation_InheritedAttribute inheritedAttribute )
   {
#if 0
     printf ("In evaluateInheritedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif

  // The D_plus or D_minus constructors are easy to find and don't have complex inputs (in our DSL specification)
  // so we can just detect them directly in the inherited attribute as we traverse the AST.  It is a slightly 
  // more obscure point that the DSL specification is not formally expressed and that it likely should be at some 
  // point).  At present it is represented only be a few examples that we are working thorough.

     DiscretizationFSM::DiscreticationKind_enum discreticationKind = DiscretizationFSM::e_error;
     SgInitializedName* initializedName = detectVariableDeclarationOfSpecificType (astNode,"D_plus");
     if (initializedName != NULL)
        {
       // Found a variable declaration associated with type name D_plus.
          discreticationKind = DiscretizationFSM::e_plus;
        }
       else
        {
       // If the current IR node is not the declaration for a variable of type D_plus, then check for D_minus.
          initializedName = detectVariableDeclarationOfSpecificType (astNode,"D_minus");
          if (initializedName != NULL)
             {
            // Found a variable declaration associated with type name D_minus.
               discreticationKind = DiscretizationFSM::e_minus;
             }
        }

     if (initializedName != NULL)
        {
       // This is the code that is specific to the DSL.

          string name = initializedName->get_name();

          SgInitializer* initializer = initializedName->get_initptr();
#if 0
          printf ("initializer = %p = %s \n",initializer,initializer->class_name().c_str());
#endif
          SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(initializer);
          ROSE_ASSERT(constructorInitializer != NULL);

          SgExprListExp* exprListExp = constructorInitializer->get_args();

       // Enforce that the specification of the DSL only permits one argument.
          ROSE_ASSERT(exprListExp->get_expressions().size() == 1);

          SgExpression* argumentExpression = exprListExp->get_expressions()[0];
          ROSE_ASSERT(argumentExpression != NULL);

          SgIntVal* integerValue = isSgIntVal(argumentExpression);
          ROSE_ASSERT(integerValue != NULL);
          
          int value = integerValue->get_value();
#if 0
          printf ("dimension = %d \n",value);
#endif
       // Build the discretizationFSM for D_plus and D_minus abstractions.
          DiscretizationFSM* discretizationFSM = new DiscretizationFSM(value,discreticationKind);

       // Save the DiscretizationFSM into the map using the variable name as the key.
       // Put the FSM into the map.
#if 0
          printf ("Put the discretizationFSM = %p into the discretizationMap using key = %s \n",discretizationFSM,name.c_str());
#endif
          ROSE_ASSERT(discretizationMap.find(name) == discretizationMap.end());

       // We have a choice of syntax to add the element to the map.
          discretizationMap[name] = discretizationFSM;

       // Not clear if I need to worry about setting this in the inherited attribute (might not be required).
       // inheritedAttribute.discretizationFSM = discretizationFSM;

       // We might need the name as part of the Maple code generation.
          discretizationFSM->discretizationName = name;

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // Recognize member function calls on "Point" objects so that we can trigger events on those associated finite state machines.
     bool isTemplateClass = false;
     bool isTemplateFunctionInstantiation = false;
     SgInitializedName* initializedNameUsedToCallMemberFunction = NULL;
     SgFunctionCallExp* functionCallExp = detectMemberFunctionOfSpecificClassType(astNode,initializedNameUsedToCallMemberFunction,"D_plus",isTemplateClass,"operator()",isTemplateFunctionInstantiation);
     if (functionCallExp != NULL)
        {
       // This is the DSL specific part (capturing the semantics of D_plus::operator() and D_minus::operator() with specific integer value arguments).

       // The name of the variable off of which the member function is called (variable has type "D_plus").
          ROSE_ASSERT(initializedNameUsedToCallMemberFunction != NULL);
          string name = initializedNameUsedToCallMemberFunction->get_name();

       // Need to get the dimention argument.
          SgExprListExp* argumentList = functionCallExp->get_args();
          ROSE_ASSERT(argumentList != NULL);
       // This function has a single argument.
          ROSE_ASSERT(argumentList->get_expressions().size() == 1);
          SgExpression* functionArg = argumentList->get_expressions()[0];
          ROSE_ASSERT(functionArg != NULL);
#if 0
          printf ("functionArg = %p = %s \n",functionArg,functionArg->class_name().c_str());
#endif
          SgCastExp* castExp = isSgCastExp(functionArg);
          ROSE_ASSERT(castExp != NULL);

          SgVarRefExp* varRefExp = isSgVarRefExp(castExp->get_operand());
          ROSE_ASSERT(varRefExp != NULL);

       // Look up the discretization object's finite state machine
          ROSE_ASSERT(discretizationMap.find(name) != discretizationMap.end());
          DiscretizationFSM* discretizationFSM = discretizationMap[name];
          ROSE_ASSERT(discretizationFSM != NULL);
#if 0
          printf ("We have found the DiscretizationFSM associated with the discretization named %s \n",name.c_str());
#endif
          string arg_name = varRefExp->get_symbol()->get_name();
#if 0
          printf ("arg_name = %s \n",arg_name.c_str());
#endif
       // Look up the discretization object's finite state machine
          ROSE_ASSERT(discretizationMap.find(name) != discretizationMap.end());
          DiscretizationFSM* discretizationFSM_arg = discretizationMap[arg_name];
          ROSE_ASSERT(discretizationFSM_arg != NULL);
#if 0
          discretizationFSM->display("before operator() event");
#endif
       // Trigger the event to capture the state change.
          discretizationFSM->operator()(discretizationFSM_arg);
#if 0
          discretizationFSM->display("after operator() event");
#endif
       // I think that since the synthesized attribute evaluation is constructing the Operator FSM, it
       // is more clear that we have to initialize the inheritedAttribute here (than in the case above).
          inheritedAttribute.discretizationFSM = discretizationFSM;
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     return inheritedAttribute;
   }


OperatorEvaluation_SynthesizedAttribute
OperatorEvaluationTraversal::evaluateSynthesizedAttribute (SgNode* astNode, OperatorEvaluation_InheritedAttribute inheritedAttribute, SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     OperatorEvaluation_SynthesizedAttribute return_synthesizedAttribute;

#if 0
     printf ("In OperatorEvaluationTraversal::evaluateSynthesizedAttribute(): astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
#if 0
     printf ("   --- inheritedAttribute.discretizationFSM = %p \n",inheritedAttribute.discretizationFSM);
#endif

     bool foundDiscretizationFSM = false;

  // SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(astNode);
  // if (constructorInitializer != NULL && inheritedAttribute.discretizationFSM != NULL)
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
     if (functionCallExp != NULL && inheritedAttribute.discretizationFSM != NULL)
        {
       // This case addresses that the D_plus or D_minus operator() has been called and we need to 
       // communicate the D_plus or D_minus object that was built (a DiscretizationFSM object).
#if 0
          printf ("Found D_plus or D_minus constructor: set them in the synthesizedAttribute: astNode = %p = %s \n",astNode,astNode->class_name().c_str());
#endif
          return_synthesizedAttribute.discretizationFSM = inheritedAttribute.discretizationFSM;
          foundDiscretizationFSM = true;
        }

  // There should only be a single child set with a DiscretizationFSM object.
     for (size_t i = 0; i < synthesizedAttributeList.size(); i++)
        {
          if (synthesizedAttributeList[i].discretizationFSM != NULL)
             {
            // Check that the return_synthesizedAttribute.discretizationFSM has not already been set.
#if 0
               printf ("synthesizedAttributeList[i].discretizationFSM != NULL \n");
#endif
            // ROSE_ASSERT(foundStencilOffsetFSM == false);
               if (foundDiscretizationFSM == false)
                  {
#if 0
                    printf ("foundDiscretizationFSM == false \n");
#endif
                 // ROSE_ASSERT(return_synthesizedAttribute.stencilOffsetFSM == NULL);
                    if (return_synthesizedAttribute.discretizationFSM == NULL)
                       {
#if 0
                         printf ("return_synthesizedAttribute.discretizationFSM != NULL \n");
#endif
                         return_synthesizedAttribute.discretizationFSM = synthesizedAttributeList[i].discretizationFSM;
                       }

                    foundDiscretizationFSM = true;
                  }
             }

          if (synthesizedAttributeList[i].operatorList.empty() == false)
             {
            // return_synthesizedAttribute.operatorList.insert(synthesizedAttributeList[i].operatorList.begin(),synthesizedAttributeList[i].operatorList.end());
               for (size_t j = 0; j < synthesizedAttributeList[i].operatorList.size(); j++)
                  {
#if 0
                    printf ("Calling return_synthesizedAttribute.operatorList.push_back(synthesizedAttributeList[%zu].operatorList[%zu]) astNode = %p = %s \n",i,j,astNode,astNode->class_name().c_str());
#endif
                    return_synthesizedAttribute.operatorList.push_back(synthesizedAttributeList[i].operatorList[j]);
                  }
             }

          if (synthesizedAttributeList[i].operatorApplicationOperand != NULL)
             {
#if 0
               printf ("Setting the return_synthesizedAttribute.operatorApplicationOperand (operator and operand) \n");
#endif
               return_synthesizedAttribute.operatorApplicationOperand = synthesizedAttributeList[i].operatorApplicationOperand;
               return_synthesizedAttribute.operatorApplication = synthesizedAttributeList[i].operatorApplication;
             }

          if (synthesizedAttributeList[i].operatorApplicationLhs != NULL)
             {
#if 0
               printf ("Setting the return_synthesizedAttribute.operatorApplicationLhs \n");
#endif
               return_synthesizedAttribute.operatorApplicationLhs = synthesizedAttributeList[i].operatorApplicationLhs;
             }

       // Pass on the marking that a transformation has been done.
          if (synthesizedAttributeList[i].get_operatorTransformed() == true)
             {
               return_synthesizedAttribute.set_operatorTransformed(true);
             }
        }

#if 0
     if (foundDiscretizationFSM == true)
        {
          printf ("foundDiscretizationFSM -- true: we need to build OperatorFSM (I think) \n");

          ROSE_ASSERT(return_synthesizedAttribute.discretizationFSM != NULL);
#if 0
          OperatorFSM operatorFSM(*(return_synthesizedAttribute.discretizationFSM));
          return_synthesizedAttribute.operatorList.push_back(operatorFSM);
#endif
          printf ("return_synthesizedAttribute.operatorList.empty() = %s \n",return_synthesizedAttribute.operatorList.empty() ? "true" : "false");

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }
#endif

  // We likely want to recognize the Operators in the synthesized attribute traversal, because the AST parts 
  // that make them up are more complex (e.g. multiple D_plus and D_minus first-order discretizations or 
  // multiple Operator objects.

     SgInitializedName* initializedName = detectVariableDeclarationOfSpecificType (astNode,"Operator");

     if (initializedName != NULL)
        {
       // This is the DSL specific behavior.

          string name = initializedName->get_name();
          ROSE_ASSERT(operatorMap.find(name) == operatorMap.end());
       // OperatorFSM* operatorFSM = new OperatorFSM();
          OperatorFSM* operatorFSM = NULL;
          if (return_synthesizedAttribute.discretizationFSM != NULL)
             {
            // Build the operator from the DiscretizationFSM
               operatorFSM = new OperatorFSM(*(return_synthesizedAttribute.discretizationFSM));

            // We have now used this, so clear it from the synthesizedAttribute.
               return_synthesizedAttribute.discretizationFSM = NULL;
             }
            else
             {
            // This is part of an Operator constructor via a rhs expression (else is is not defined in the DSL).
               SgInitializer* initializer = initializedName->get_initptr();
               if (initializer != NULL)
                  {
                    SgAssignInitializer* assignInitializer = isSgAssignInitializer(initializer);
                    if (assignInitializer != NULL)
                       {
#if 0
                         printf ("Found a SgAssignInitializer \n");
#endif
                         ROSE_ASSERT(return_synthesizedAttribute.operatorList.size() == 1);
                      // operatorFSM = return_synthesizedAttribute.operatorList[0];
                         operatorFSM = new OperatorFSM(return_synthesizedAttribute.operatorList[0]);

                         return_synthesizedAttribute.operatorList.erase(return_synthesizedAttribute.operatorList.begin(),return_synthesizedAttribute.operatorList.end());
                         ROSE_ASSERT(return_synthesizedAttribute.operatorList.empty() == true);
#if 0
                         printf ("Exiting as a test! \n");
                         ROSE_ASSERT(false);
#endif
                       }
                      else
                       {
                         printf ("Error: assignInitializer == NULL: initializer = %p = %s \n",initializer,initializer->class_name().c_str());
                         ROSE_ASSERT(false);
                       }
                  }
                 else
                  {
                    printf ("Error: improper declaration of Operator in DSL: initializedName = %p = %s = %s \n",initializedName,initializedName->class_name().c_str(),initializedName->get_name().str());
                    ROSE_ASSERT(false);
                  }
             }

          ROSE_ASSERT(operatorFSM != NULL);
          operatorMap[name] = operatorFSM;
          ROSE_ASSERT(operatorMap.find(name) != operatorMap.end());

       // Set the name of the operator to support Maple code generation.
          operatorFSM->operatorName = name;

#if 1
          printf ("Added OperatorFSM to operatorMap using name = %s \n",name.c_str());
#endif
#if 0
          printf ("Trigger an event on the operatorFSM ========================== %p \n",operatorFSM);
          printf ("   --- Use the return_synthesizedAttribute.discretizationFSM = %p \n",return_synthesizedAttribute.discretizationFSM);
#endif
#if 0
          printf ("return_synthesizedAttribute.operatorList.empty() = %s \n",return_synthesizedAttribute.operatorList.empty() ? "true" : "false");
#endif
#if 0
          operatorFSM->display("after FSM operator default construction plus constructor argument specification event: OperatorEvaluationTraversal::evaluateSynthesizedAttribute()");
#endif
        }

     SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
     if (varRefExp != NULL)
        {
          SgType* type = varRefExp->get_type();
          bool isTemplateClass = false;
          bool isOperatorType = isMatchingClassType(type,"Operator",isTemplateClass);
          if (isOperatorType == true)
             {
               ROSE_ASSERT(varRefExp->get_symbol() != NULL);
               SgInitializedName* initializedName = varRefExp->get_symbol()->get_declaration();
               ROSE_ASSERT(initializedName != NULL);
               string name = initializedName->get_name();
               ROSE_ASSERT(operatorMap.find(name) != operatorMap.end());
               OperatorFSM* FSM_ref = operatorMap[name];
               ROSE_ASSERT(FSM_ref != NULL);

               return_synthesizedAttribute.operatorList.push_back(*FSM_ref);
#if 0
               printf ("Detected a SgVarRefExp of type Operator: name = %s \n",name.c_str());
#endif
#if 0
               string label = "Display from detection of SgVarRefExp name = " + name;
               FSM_ref->display(label);
#endif
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
        }

     bool isTemplateClass                 = false;
     bool isTemplateFunctionInstantiation = false;
     SgInitializedName* initializedNameUsedToCallMemberFunctionPlus = NULL;
     SgFunctionCallExp* functionCallExpOperatorPlus = detectMemberFunctionOfSpecificClassType(astNode,initializedNameUsedToCallMemberFunctionPlus,"Operator",isTemplateClass,"operator+",isTemplateFunctionInstantiation);
     if (functionCallExpOperatorPlus != NULL)
        {
#if 0
          printf ("Detected Operator::operator+() \n");
          printf ("return_synthesizedAttribute.operatorList.size() = %zu \n",return_synthesizedAttribute.operatorList.size());
#endif
          ROSE_ASSERT(return_synthesizedAttribute.operatorList.size() == 2);
          OperatorFSM lhs = return_synthesizedAttribute.operatorList[0];
          OperatorFSM rhs = return_synthesizedAttribute.operatorList[1];
#if 0
          lhs.display("Operator lhs");
          rhs.display("Operator rhs");
#endif
       // Trigger the event to record the plus operation in the generated OperatorFSM.
       // tempOperatorFSM->operator+(lhs,rhs);
          lhs.operator+(rhs);

          return_synthesizedAttribute.operatorList.erase(return_synthesizedAttribute.operatorList.begin(),return_synthesizedAttribute.operatorList.end());
#if 0
          printf ("after erase: return_synthesizedAttribute.operatorList.size() = %zu \n",return_synthesizedAttribute.operatorList.size());
#endif
       // Save the result in the synthesizedAttribute.
          return_synthesizedAttribute.operatorList.push_back(lhs);

          ROSE_ASSERT(return_synthesizedAttribute.operatorList.size() == 1);
#if 0
          printf ("after push_back: return_synthesizedAttribute.operatorList.size() = %zu \n",return_synthesizedAttribute.operatorList.size());
#endif
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // Look for the location of the application of the function: "GridFunction Operator::operator()(GridFunction&)"
     SgInitializedName* initializedNameUsedToCallMemberFunctionApply = NULL;
     SgFunctionCallExp* functionCallExpOperatorApply = detectMemberFunctionOfSpecificClassType(astNode,initializedNameUsedToCallMemberFunctionApply,"Operator",isTemplateClass,"operator()",isTemplateFunctionInstantiation);
     if (functionCallExpOperatorApply != NULL)
        {
#if 0
          printf ("Detected Operator::operator() \n");
          printf ("return_synthesizedAttribute.operatorList.size() = %zu \n",return_synthesizedAttribute.operatorList.size());
          printf ("initializedNameUsedToCallMemberFunctionApply = %p \n",initializedNameUsedToCallMemberFunctionApply);
#endif

          ROSE_ASSERT(return_synthesizedAttribute.operatorList.size() == 1);
          OperatorFSM lhs = return_synthesizedAttribute.operatorList[0];
#if 0
       // The rhs should be a GridFunction.
          lhs.display("In Operator::operator(): rhs should be a GridFunction");
#endif
          SgExprListExp* exprListExp = functionCallExpOperatorApply->get_args();
          ROSE_ASSERT(exprListExp != NULL);

          ROSE_ASSERT(exprListExp->get_expressions().size() == 1);
          SgExpression* rhs_exp = exprListExp->get_expressions()[0];
          ROSE_ASSERT(rhs_exp != NULL);

          SgVarRefExp* gridFunctionVarRefExp = isSgVarRefExp(rhs_exp);
          ROSE_ASSERT(gridFunctionVarRefExp != NULL);

       // Record the Operator application to GridFunction rhs into the synthesizedAttribute.
          return_synthesizedAttribute.operatorApplication        = lhs;
          return_synthesizedAttribute.operatorApplicationOperand = gridFunctionVarRefExp;
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

  // Look for the location of the application of the function: "GridFunction Operator::operator()(GridFunction&)"
     SgInitializedName* initializedNameUsedToCallMemberFunctionGridFunctionAssign = NULL;
     SgFunctionCallExp* functionCallExpOperatorGridFunctionAssign = detectMemberFunctionOfSpecificClassType(astNode,initializedNameUsedToCallMemberFunctionGridFunctionAssign,"GridFunction",isTemplateClass,"operator=",isTemplateFunctionInstantiation);
     if (functionCallExpOperatorGridFunctionAssign != NULL)
        {
#if 0
          printf ("Detected GridFunction::operator=() \n");
#endif
          SgDotExp* dotExp = isSgDotExp(functionCallExp->get_function());
          if (dotExp != NULL)
             {
               SgVarRefExp* gridFunctionVarRefExp = isSgVarRefExp(dotExp->get_lhs_operand());

               return_synthesizedAttribute.operatorApplicationLhs = gridFunctionVarRefExp;
             }
#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     SgExprStatement* expressionStatement = isSgExprStatement(astNode);
     if (expressionStatement != NULL)
        {
       // This is the top most part of the AST where we should apply the operator to the RHS and assign the result to the LHS.
       // For now generate the code as a string and assign it to the SgExpressionStatement as a comment.
#if 0
          printf ("Found an SgExprStatement: expressionStatement = %p \n",expressionStatement);
          printf ("   --- return_synthesizedAttribute.operatorApplicationLhs     = %p \n",return_synthesizedAttribute.operatorApplicationLhs);
          printf ("   --- return_synthesizedAttribute.operatorApplicationOperand = %p \n",return_synthesizedAttribute.operatorApplicationOperand);
#endif
       // If the LHS and the Operator's operand have been set then we assume the Operator is valid.
          if ((return_synthesizedAttribute.operatorApplicationLhs != NULL) && (return_synthesizedAttribute.operatorApplicationOperand != NULL))
             {
#if 1
               printf ("Apply the discretization operator to the RHS and assign it to the LHS \n");
#endif
#if 1
               expressionStatement->get_file_info()->display("Apply the discretization operator to the RHS and assign it to the LHS");
#endif
#if 1
               printf ("Call generateMapleCode to generate example code \n");
#endif
            // Generate code from operator data structure (stored in return_synthesizedAttribute.operatorApplication).
               bool generateLowlevelCode = true;
               SgExpression* lhs = return_synthesizedAttribute.operatorApplicationLhs;
               SgExpression* rhs = return_synthesizedAttribute.operatorApplicationOperand;

               string generatedCode = generateMapleCode(lhs,rhs,return_synthesizedAttribute.operatorApplication,generateLowlevelCode);
#if 0
               printf ("DONE: Call generateMapleCode to generate example code \n");
#endif
            // Clear the return_synthesizedAttribute (since we have at this point used all of the data.
               return_synthesizedAttribute.operatorApplicationLhs     = NULL;
               return_synthesizedAttribute.operatorApplicationOperand = NULL;
               return_synthesizedAttribute.operatorApplication        = OperatorFSM();

#ifdef USE_ROSE_MAPLE_SUPPORT
            // Call Maple using the generateed code.
               StencilOperator stencil = callMaple(generatedCode);

            // Here we assume that we will get a data structure used to communicate the stencil coefficients.
            // I need help from jeff to get this generated via Maple (the idea is that we extract the coefficients 
            // from the Maple generated expression (so we need not generate the code using Maple).  Then from the
            // Matrix of stencil coefficients we support the code generation by building up the AST.

               printf ("DONE: Building the Maple Operator API \n");

               ROSE_ASSERT(lhs != NULL);
               ROSE_ASSERT(rhs != NULL);

               generateStencilCode(stencil,lhs,rhs);
#else
               printf ("\n\n******************************************************************************************************************** \n");
               printf ("NOTE: Maple DSL not configured in ROSE to use Maple Library (requires installation via ROSE configure command line) \n");
               printf ("******************************************************************************************************************** \n\n\n");
#endif

            // Mark that a transformation was done.
               return_synthesizedAttribute.set_operatorTransformed(true);
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }
        }

#if 0
     printf ("Leaving OperatorEvaluationTraversal::evaluateSynthesizedAttribute(): return_synthesizedAttribute.discretizationFSM = %p \n",return_synthesizedAttribute.discretizationFSM);
#endif

     return return_synthesizedAttribute;
   }



void OperatorEvaluationTraversal::displayOperator(const string & label)
   {
  // This function outputs the data associated with each of the stencil in the evaluation.

     printf ("In OperatorEvaluationTraversal::displayOperator(): label = %s \n",label.c_str());

     const string innerLabel = "operator element";
     std::map<std::string,OperatorFSM*>::iterator i = operatorMap.begin();
     while(i != operatorMap.end())
        {
          (*i).second->display(innerLabel);
          i++;
        }
   }

