// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

// #include "shiftCalculusCompiler.h"

// #include "stencilAndStencilOperatorDetection.h"
// #include "stencilEvaluation.h"

#include "dslSupport.h"

#if 1
// Reference classe names that are specific to the DSL.
extern std::vector<std::string> dsl_type_names;

// Reference functions names that are specific to the DSL.
extern std::vector<std::string> dsl_function_names;

// Reference member functions (class and member function names) that are specific to the DSL.
extern std::vector< std::pair<std::string,std::string> > dsl_member_function_names;

// Example attributes that we need to have generated to support array DSL example.
class dsl_attribute : public AstAttribute
   {
     public:
          dsl_attribute();
          virtual ~dsl_attribute();

          std::string toString();
          std::string additionalNodeOptions();

       // Most attributes will have children that define there embedding into the AST.
          std::vector<SgNode*> dslChildren;

          SgNode* currentNode;
          std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();

          std::string get_name();
          std::string get_color();
   };

// References to dsl attributes in a map inexed by the name of the dsl abstraction.
extern std::map<std::string,dsl_attribute> dsl_attribute_map;
#endif


// This code will make calles to the finite state machine representing the stencil 
// so that we can execute events and accumulate state (and then read the state as
// and intermediate form for the stencil (maybe saved as an attribute).  This data
// is then the jumping off point for different groups to experiment with the generation
// of architecture specific code.
// #include "stencilFiniteStateMachine.h"

// DSL specific code being included.
#include "array.h"

#if 0
   #include "generated_dsl_attributes.h"
#else
   #include "nongenerated_dsl_attributes.h"
#endif

using namespace std;


// This is a function refactoring code used within the stencil evaluation.
// Likely it is generally useful in writing DSLs so it should be refactored 
// to be elsewhere directly in ROSE (e.g. SageInterface namespace).
bool
DSL_Support::isMatchingClassType(SgType* type, const string & name, bool isTemplateClass)
   {
  // This function returns true if the type is a class type or template class instantiation matching the input name.

     bool returnValue = false;

     ROSE_ASSERT(type != NULL);

  // We could maybe just use the type from varRefExp->get_type() (which would be shorter/simpler).
  // SgClassType* classType = isSgClassType(initializedName->get_type());
     SgClassType* classType = isSgClassType(type);
     if (classType != NULL)
        {
       // Check if this is associated with a template instantiation.
          if (isTemplateClass == true)
             {
               SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(classType->get_declaration());
               if (templateInstantiationDecl != NULL)
                  {
#if 0
                    printf ("case SgTemplateInstaiationDecl: class name = %s \n",classType->get_name().str());
                    printf ("case SgTemplateInstaiationDecl: templateInstantiationDecl->get_templateName() = %s \n",templateInstantiationDecl->get_templateName().str());
#endif
                    if (templateInstantiationDecl->get_templateName() == name)
                       {
#if 0
                         printf ("This is verified to be associated with the template class type name = %s \n",name.c_str());
#endif
                         returnValue = true;
                       }
                  }
             }
            else
             {
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
               if (classDeclaration != NULL)
                  {
                    if (classDeclaration->get_name() == name)
                       {
#if 0
                         printf ("This is verified to be associated with the class type name = %s \n",name.c_str());
#endif
                         returnValue = true;
                       }
                  }
             }
        }

     return returnValue;
   }



bool
DSL_Support::isMatchingMemberFunction(SgMemberFunctionRefExp* memberFunctionRefExp, const string & name, bool isTemplateInstantiation)
   {
      bool returnValue = false;
      ROSE_ASSERT(memberFunctionRefExp != NULL);

      SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();
      ROSE_ASSERT(memberFunctionSymbol != NULL);

#if 0
      printf ("memberFunctionSymbol = %p = %s \n",memberFunctionSymbol,memberFunctionSymbol->class_name().c_str());
#endif

      SgMemberFunctionDeclaration* memberFunctionDeclaration = memberFunctionSymbol->get_declaration();
      ROSE_ASSERT(memberFunctionDeclaration != NULL);

#if 0
      printf ("memberFunctionDeclaration = %p = %s \n",memberFunctionDeclaration,memberFunctionDeclaration->class_name().c_str());
#endif

      if (isTemplateInstantiation == true)
         {
           SgTemplateInstantiationMemberFunctionDecl* templateInstantiationMemberFunctionDecl = isSgTemplateInstantiationMemberFunctionDecl(memberFunctionDeclaration);

        // It is interesting that for operator+, the names of all three ways of computing then are the same.
#if 0
           printf ("memberFunctionDeclaration->get_name()                       = %s \n",memberFunctionDeclaration->get_name().str());
           if (templateInstantiationMemberFunctionDecl != NULL)
              {
                printf ("templateInstantiationMemberFunctionDecl->get_name()         = %s \n",templateInstantiationMemberFunctionDecl->get_name().str());
                printf ("templateInstantiationMemberFunctionDecl->get_templateName() = %s \n",templateInstantiationMemberFunctionDecl->get_templateName().str());
              }
#endif
           if (templateInstantiationMemberFunctionDecl != NULL && templateInstantiationMemberFunctionDecl->get_templateName() == name)
              {
                returnValue = true;
              }
         }
        else
         {
#if 0
           printf ("memberFunctionDeclaration->get_name()                       = %s \n",memberFunctionDeclaration->get_name().str());
#endif
           if (memberFunctionDeclaration->get_name() == name)
              {
                returnValue = true;
              }
         }

     return returnValue;
   }

// SgFunctionCallExp* DSL_Support::buildMemberFunctionCall(SgExpression* expressionRoot, SgType* type, const string & memberFunctionName, SgExpression* expression, bool isOperator)
SgFunctionCallExp*
DSL_Support::buildMemberFunctionCall(SgExpression* expressionRoot, const string & memberFunctionName, SgExpression* expression, bool isOperator)
   {
     SgFunctionCallExp* memberFunctionCall = NULL;

#if 0
     printf ("In DSL_Support::buildMemberFunctionCall(): expressionRoot = %p = %s memberFunctionName = %s expression = %p isOperator = %s \n",
          expressionRoot,expressionRoot->class_name().c_str(),memberFunctionName.c_str(),expression,isOperator ? "true" : "false");
#endif

  // Need to get the symbol for the member function "operator[]" in the RectMDArray<TDest> template class instantiation.

  // We can't get the type from the expressionRoot since it might be a function call and 
  // return a type that is different from the SgClassType of the declaration where we want 
  // to find the member function.
  // SgClassType* classType = isSgClassType(expressionRoot->get_type());
  // SgClassType* classType = isSgClassType(type);

  // Note that we need to get past any possible tyep references, etc.
  // SgClassType* classType = isSgClassType(expressionRoot->get_type());
  // Available values to strip (we only select a subset): STRIP_MODIFIER_TYPE|STRIP_REFERENCE_TYPE|STRIP_POINTER_TYPE|STRIP_ARRAY_TYPE|STRIP_TYPEDEF_TYPE
     SgClassType* classType = isSgClassType(expressionRoot->get_type()->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_TYPEDEF_TYPE));

     if (classType == NULL)
        {
          printf ("Error: classType == NULL: expressionRoot->get_type() = %p \n",expressionRoot->get_type());
          if (expressionRoot->get_type() != NULL)
             {
               printf ("   --- expressionRoot->get_type() = %s \n",expressionRoot->get_type()->class_name().c_str());
             }

       // classType = SgType::stripType(expressionRoot->get_type());
       // Available values: STRIP_MODIFIER_TYPE|STRIP_REFERENCE_TYPE|STRIP_POINTER_TYPE|STRIP_ARRAY_TYPE|STRIP_TYPEDEF_TYPE
       // classType = isSgClassType(expressionRoot->get_type()->stripType(SgType::STRIP_MODIFIER_TYPE|SgType::STRIP_REFERENCE_TYPE|SgType::STRIP_TYPEDEF_TYPE));
       // ROSE_ASSERT(classType != NULL);
        }
     ROSE_ASSERT(classType != NULL);

     SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
     ROSE_ASSERT(classDeclaration != NULL);
     SgClassDeclaration* definingClassDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
     ROSE_ASSERT(definingClassDeclaration != NULL);

  // We need the class definition scope so that we can look up the member function "operator[]".
     SgClassDefinition* classDefinition = definingClassDeclaration->get_definition();
     ROSE_ASSERT(classDefinition != NULL);
#if 0
     printf ("classDeclaration->get_name() = %s classDefinition = %p = %s \n",classDeclaration->get_name().str(),classDefinition,classDefinition->class_name().c_str());
#endif
  // For the moment we will assume this is not a overloaded operator.
  // SgMemberFunctionSymbol* memberFunctionSymbol = classDefinition->lookup_nontemplate_member_function_symbol("opearator[]");
  // SgFunctionSymbol* functionSymbol = classDefinition->lookup_function_symbol("operator[]");
     SgFunctionSymbol* functionSymbol = classDefinition->lookup_function_symbol(memberFunctionName);
     if (functionSymbol == NULL)
        {
          printf ("Error: function not found in classDeclaration = %s symbol table: memberFunctionName = %s \n",classDeclaration->get_name().str(),memberFunctionName.c_str());

       // Debugging the missing symbol which we expected in the symbol table.
       // classDefinition->get_symbol_table()->print();
        }
     ROSE_ASSERT(functionSymbol != NULL);
  // SgMemberFunctionSymbol* memberFunctionSymbol = classDefinition->lookup_function_symbol("opearator[]");
     SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(functionSymbol);
     ROSE_ASSERT(memberFunctionSymbol != NULL);

  // Build the member function reference expression.
     bool virtual_call   = false;
     bool need_qualifier = false;
     SgMemberFunctionRefExp* memberFunctionRefExp = SageBuilder::buildMemberFunctionRefExp(memberFunctionSymbol,virtual_call,need_qualifier);

  // Build the variable reference so that we can apply the member function.
  // SgVarRefExp* varRefExp = SageBuilder::buildVarRefExp(variableSymbol);

  // Build the dot expression (array variable reference on lhs and member function reference on rhs).
     SgDotExp* dotExp                   = SageBuilder::buildDotExp(expressionRoot,memberFunctionRefExp);

  // Build the function argument list.
  // SgExprListExp* exprListExp         = SageBuilder::buildExprListExp(expression);
     SgExprListExp* exprListExp = NULL;
     if (expression != NULL)
        {
          exprListExp = SageBuilder::buildExprListExp(expression);
        }
       else
        {
          exprListExp = SageBuilder::buildExprListExp();
        }

  // Build the function call expression.
     SgFunctionCallExp* functionCallExp = SageBuilder::buildFunctionCallExp(dotExp,exprListExp);

     if (isOperator == true)
        {
       // Cause the unparsed code to use the operator syntax (instead of function syntax).
          functionCallExp->set_uses_operator_syntax(true);
        }

     memberFunctionCall = functionCallExp;

     ROSE_ASSERT(memberFunctionCall != NULL);

     return memberFunctionCall;
   }


SgFunctionCallExp*
DSL_Support::buildMemberFunctionCall(SgVariableSymbol* variableSymbol, const string & memberFunctionName, SgExpression* expression, bool isOperator)
   {
  // Build the variable reference so that we can apply the member function.
     SgVarRefExp* varRefExp = SageBuilder::buildVarRefExp(variableSymbol);
  // SgType* type           = variableSymbol->get_type();
  // SgFunctionCallExp* memberFunctionCall = buildMemberFunctionCall(varRefExp, type, memberFunctionName, expression, isOperator);
     SgFunctionCallExp* memberFunctionCall = buildMemberFunctionCall(varRefExp, memberFunctionName, expression, isOperator);
     ROSE_ASSERT(memberFunctionCall != NULL);

     return memberFunctionCall;
   }

SgVariableDeclaration*
DSL_Support::buildDataPointer(const string & pointerVariableName, SgVariableSymbol* variableSymbol, SgScopeStatement* outerScope)
   {
  // Optionally build a pointer variable so that we can optionally support a C style indexing for the DTEC DSL blocks.
     SgExpression* pointerExp = buildMemberFunctionCall(variableSymbol,"getPointer",NULL,false);
     ROSE_ASSERT(pointerExp != NULL);
     SgAssignInitializer* assignInitializer = SageBuilder::buildAssignInitializer_nfi(pointerExp);
     ROSE_ASSERT(assignInitializer != NULL);

  // Build the variable declaration for the pointer to the data.
     SgVariableDeclaration* variableDeclaration  = SageBuilder::buildVariableDeclaration_nfi(pointerVariableName,SageBuilder::buildPointerType(SageBuilder::buildDoubleType()),assignInitializer,outerScope);
     ROSE_ASSERT(variableDeclaration != NULL);

     return variableDeclaration;
   }

SgVariableDeclaration*
DSL_Support::buildBoxRef(const string & pointerVariableName, SgVariableSymbol* variableSymbol, SgScopeStatement* outerScope, SgType* type)
   {
  // Optionally build a pointer variable so that we can optionally support a C style indexing for the DTEC DSL blocks.
     SgExpression* pointerExp = buildMemberFunctionCall(variableSymbol,"getBox",NULL,false);
     ROSE_ASSERT(pointerExp != NULL);
     SgAssignInitializer* assignInitializer = SageBuilder::buildAssignInitializer_nfi(pointerExp);
     ROSE_ASSERT(assignInitializer != NULL);

  // Build the variable declaration for the pointer to the data.
     SgVariableDeclaration* variableDeclaration  = SageBuilder::buildVariableDeclaration_nfi(pointerVariableName,type,assignInitializer,outerScope);
     ROSE_ASSERT(variableDeclaration != NULL);

     return variableDeclaration;
   }



SgVariableDeclaration*
DSL_Support::buildMultiDimPointer(const string & pointerVariableName, SgVariableSymbol* variableSymbol, SgScopeStatement* outerScope, std::vector<SgVariableSymbol*> SymbolArray, int dimSize)
   {
     SgArrayType* baseType = SageBuilder::buildArrayType(SageBuilder::buildDoubleType(), SageBuilder::buildVarRefExp(SymbolArray[0]));

     for(int i=1; i < dimSize-1; ++i)
     {
        SgArrayType* oldType = baseType; 
        SgArrayType* newType = SageBuilder::buildArrayType(oldType, SageBuilder::buildVarRefExp(SymbolArray[i]));
        baseType = newType;
     } 

  // Optionally build a pointer variable so that we can optionally support a C style indexing for the DTEC DSL blocks.
     SgExpression* pointerExp = buildMemberFunctionCall(variableSymbol,"getPointer",NULL,false);
     ROSE_ASSERT(pointerExp != NULL);
     SgPointerType* castType = SageBuilder::buildPointerType(baseType);
     SgCastExp* castExp = SageBuilder::buildCastExp(pointerExp, castType,SgCastExp::e_C_style_cast);
     SgAssignInitializer* assignInitializer = SageBuilder::buildAssignInitializer_nfi(castExp);
     ROSE_ASSERT(assignInitializer != NULL);

  // Build the variable declaration for the pointer to the data.
     SgVariableDeclaration* variableDeclaration  = SageBuilder::buildVariableDeclaration_nfi(pointerVariableName,castType,assignInitializer,outerScope);
     ROSE_ASSERT(variableDeclaration != NULL);

     return variableDeclaration;
   }


SgInitializedName*
DSL_Support::detectVariableDeclarationOfSpecificType (SgNode* astNode, const string & className)
   {
  // Within the stencil DSL we want to interogate the SgAssignInitializer, but we need to generality 
  // in the refactored function to use any SgInitializer (e.g. SgConstructorInitializer, etc.).
  // bool detectedPointVariableDeclaration = false;

     SgInitializedName* return_initializedName = NULL;

  // We need to find variables of type "Point" so that we can construct the associated finite state machines that will 
  // use the same name as a the variable.  This code identifies variable of type "Point" and their associated initializers.
  // The initializers are recognized and define specific semantics used to define events to the finite state machines
  // used to model each "Point" data member.  See below how we recognize functions "getZeros" and "getUnitv" to 
  // setup the constructed finite state machines which we then save in a map for later use.
     SgVariableDeclaration* variableDeclarationForPoint = isSgVariableDeclaration(astNode);
     if (variableDeclarationForPoint != NULL)
        {
       // Get the SgInitializedName from the SgVariableDeclaration.
          SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclarationForPoint);
#if 0
          printf ("initializedName->get_name() = %s initializedName->get_type() = %s \n",initializedName->get_name().str(),initializedName->get_type()->class_name().c_str());
#endif
          SgClassType* classType = isSgClassType(initializedName->get_type());
#if 0
          if (classType == NULL)
             {
            // Check for SgModifierType and strip away to get at possible SgClassType.
               printf ("initializedName type is not a SgClassType, check for SgModifierType and strip away to get at possible SgClassType (not implemented) \n");
             }
#endif
          if (classType != NULL)
             {
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
               ROSE_ASSERT(classDeclaration != NULL);
#if 0
               printf ("initializedName->get_name()  = %s \n",initializedName->get_name().str());
               printf ("classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
#endif
            // We might want to be more flexiable about the type of the 2nd parameter (allow SgTypeFloat, SgTypeComplex, etc.).
               if (classDeclaration->get_name() == className)
                  {
                 // Found a variable of type Point.
#if 0
                    printf ("initializedName->get_name()  = %s \n",initializedName->get_name().str());
                    printf ("classDeclaration->get_name() = %s \n",classDeclaration->get_name().str());
#endif
                    return_initializedName = initializedName;

                 // new StencilOffsetFSM();
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }
            else
             {
            // If "const" is used, then this will be a SgModifierType.
#if 0
               printf ("initializedName->get_type() = %s \n",initializedName->get_type()->class_name().c_str());
#endif
             }
        }

     return return_initializedName;
   }


SgFunctionCallExp* 
DSL_Support::detectMemberFunctionOfSpecificClassType(
     SgNode* astNode, SgInitializedName* & initializedNameUsedToCallMemberFunction, 
     const string & className, bool isTemplateClass, const string & memberFunctionName, 
     bool isTemplateFunctionInstantiation)
   {
  // Recognize member function calls on "Point" objects so that we can trigger events on those associated finite state machines.

     SgFunctionCallExp* return_functionCallExp = NULL;

     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
     if (functionCallExp != NULL)
        {
       // printf ("*** functionCallExp->get_function() = %s \n",functionCallExp->get_function()->class_name().c_str());

          SgDotExp* dotExp = isSgDotExp(functionCallExp->get_function());
          if (dotExp != NULL)
             {
               SgVarRefExp* varRefExp = isSgVarRefExp(dotExp->get_lhs_operand());
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(dotExp->get_rhs_operand());

            // Note that varRefExp might be NULL and the member function could still be from the target clas type.
            // We need to allow for this case. But this code handles the simpler case for now.
               if (varRefExp != NULL && memberFunctionRefExp != NULL)
                  {
                    ROSE_ASSERT(varRefExp->get_symbol() != NULL);
                    SgInitializedName* initializedName = varRefExp->get_symbol()->get_declaration();
                    ROSE_ASSERT(initializedName != NULL);

                    string name = initializedName->get_name();

                 // bool isTemplateClass = false;
                    bool isMatchingClassNameType = isMatchingClassType(initializedName->get_type(),className,isTemplateClass);
#if 0
                    printf ("isMatchingClassNameType = %s \n",isMatchingClassNameType ? "true" : "false");
                    printf ("Calling isMatchingMemberFunction() \n");
#endif
                 // bool isTemplateFunctionInstantiation = false;
                    bool isMatchingMemberFunctionName = isMatchingMemberFunction(memberFunctionRefExp,memberFunctionName,isTemplateFunctionInstantiation);
#if 0
                    printf ("isMatchingMemberFunctionName = %s \n",isMatchingMemberFunctionName ? "true" : "false");
#endif
                    if (isMatchingMemberFunctionName == true && isMatchingClassNameType == true)
                       {
#if 0
                         printf ("Found className::memberFunctionName = %s::%s \n",className.c_str(),memberFunctionName.c_str());
#endif
                         return_functionCallExp = functionCallExp;
                         initializedNameUsedToCallMemberFunction = initializedName;
                       }
                  }
             }
        }

     return return_functionCallExp;
   }
















  // ********************************************************
  // DQ (10/24/2014): Added feature for constant expresssion 
  // evaluation. This code below is an advanced version of 
  // that is going into the SageInterface.  It should be 
  // available there shortly and when in place it should be 
  // removed from here and referenced from the SageInterface 
  // namespace.
  // ********************************************************

struct DSL_Support::const_numeric_expr_t DSL_Support::SimpleExpressionEvaluator2::getValueExpressionValue(SgValueExp *valExp)
{   
    struct DSL_Support::const_numeric_expr_t subtreeVal;
    subtreeVal.isIntOnly_ = true;

   if (isSgIntVal(valExp)) {
     subtreeVal.value_ = isSgIntVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgLongIntVal(valExp)) {
     subtreeVal.value_ = isSgLongIntVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgLongLongIntVal(valExp)) {
     subtreeVal.value_ = isSgLongLongIntVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgShortVal(valExp)) {
     subtreeVal.value_ = isSgShortVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgUnsignedIntVal(valExp)) {
     subtreeVal.value_ = isSgUnsignedIntVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgUnsignedLongVal(valExp)) {
     subtreeVal.value_ = isSgUnsignedLongVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgUnsignedLongLongIntVal(valExp)) {
     subtreeVal.value_ = isSgUnsignedLongLongIntVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgUnsignedShortVal(valExp)) {
     subtreeVal.value_ = isSgUnsignedShortVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
   } else if (isSgDoubleVal(valExp)) {
     subtreeVal.value_ = isSgDoubleVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
     subtreeVal.isIntOnly_ = false;
   } else if (isSgFloatVal(valExp)){
     subtreeVal.value_ = isSgFloatVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
     subtreeVal.isIntOnly_ = false;
   } else if (isSgLongDoubleVal(valExp)){
     subtreeVal.value_ = isSgDoubleVal(valExp)->get_value();
     subtreeVal.hasValue_ = true;
     subtreeVal.isIntOnly_ = false;
   }
   return subtreeVal;
 }

 
  // ********************************************************
  // DQ (10/24/2014): Added feature for constant expresssion 
  // evaluation. This code below is an advanced version of 
  // that is going into the SageInterface.  It should be 
  // available there shortly and when in place it should be 
  // removed from here and referenced from the SageInterface 
  // namespace.
  // ********************************************************

 struct DSL_Support::const_numeric_expr_t DSL_Support::SimpleExpressionEvaluator2::evaluateVariableReference(SgVarRefExp *vRef) 
   {
   if (isSgModifierType(vRef->get_type()) == NULL) {
     struct DSL_Support::const_numeric_expr_t val;
     val.value_ = -1;
     val.hasValue_ = false;
     return val;
   }
   if (isSgModifierType(vRef->get_type())->get_typeModifier().get_constVolatileModifier().isConst()) {
     // We know that the var value is const, so get the initialized name and evaluate it
     SgVariableSymbol *sym = vRef->get_symbol();
     SgInitializedName *iName = sym->get_declaration();
     SgInitializer *ini = iName->get_initializer();
                                                                                 
     if (isSgAssignInitializer(ini)) {
       SgAssignInitializer *initializer = isSgAssignInitializer(ini);
       SgExpression *rhs = initializer->get_operand();
       SimpleExpressionEvaluator2 variableEval;
                                                                                                                
       return variableEval.traverse(rhs);
     }
   }
   struct DSL_Support::const_numeric_expr_t val;
   val.hasValue_ = false;
   val.value_ = -1;
   return val;
 }

 
  // ********************************************************
  // DQ (10/24/2014): Added feature for constant expresssion 
  // evaluation. This code below is an advanced version of 
  // that is going into the SageInterface.  It should be 
  // available there shortly and when in place it should be 
  // removed from here and referenced from the SageInterface 
  // namespace.
  // ********************************************************

struct DSL_Support::const_numeric_expr_t DSL_Support::SimpleExpressionEvaluator2::evaluateSynthesizedAttribute(SgNode *node, SynthesizedAttributesList synList) 
{
   if (isSgExpression(node)) {
     if (isSgValueExp(node)) {
       return this->getValueExpressionValue(isSgValueExp(node));
     }
                                                                                                                                                 
     if (isSgVarRefExp(node)) {
      //      std::cout << "Hit variable reference expression!" << std::endl;
       return evaluateVariableReference(isSgVarRefExp(node));
     }
     // Early break out for assign initializer // other possibility?
     if (isSgAssignInitializer(node)) {
       if(synList.at(0).hasValue_){
         return synList.at(0);
       } else { 
         struct DSL_Support::const_numeric_expr_t val;
         val.value_ = -1;
         val.hasValue_ = false;
         return val;
       }
     }
     struct DSL_Support::const_numeric_expr_t evaluatedValue;
     evaluatedValue.hasValue_ = false;
     evaluatedValue.value_ = -1;
     evaluatedValue.isIntOnly_ = false;
     if(isSgBinaryOp(node)){
       if((synList[0].isIntOnly_ == true) && (synList[1].isIntOnly_ == true)){
         evaluatedValue.isIntOnly_ = true;
       }
       // JP The += -= /= operator don't make sense, since we restrict ourselves to const marked variables.
       if(synList[0].hasValue_ && synList[1].hasValue_){
         if (isSgAddOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_ + synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgSubtractOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_  - synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgMultiplyOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_  * synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgDivideOp(node)) {
           assert(synList.size() == 2);
           evaluatedValue.value_ = synList[0].value_  / synList[1].value_ ;
           evaluatedValue.hasValue_ = true;
         } else if (isSgModOp(node)) {
           assert(synList.size() == 2);
           if (synList[0].isIntOnly_ && synList[1].isIntOnly_){
             long a = synList[0].value_;
             long b = synList[1].value_;
             evaluatedValue.value_ = a % b;
             evaluatedValue.isIntOnly_ = true;
           } else {
             std::cerr << "Wrong arguments to modulo operator." << std::endl;
           }
           evaluatedValue.hasValue_ = true;
         }
       } else {
         std::cerr << "Expression is not evaluatable" << std::endl;
         evaluatedValue.hasValue_ = false;
         evaluatedValue.value_ = -1;
         return evaluatedValue;
       }
     } else if(isSgUnaryOp(node)){
       evaluatedValue.isIntOnly_ = synList[0].isIntOnly_;
       if(synList[0].hasValue_ == true){
         if(isSgMinusOp(node)){
           evaluatedValue.value_ = (-1) * synList[0].value_;
           evaluatedValue.hasValue_ = true;
         } else if (isSgUnaryAddOp(node)){
           evaluatedValue.value_ = + synList[0].value_;
           evaluatedValue.hasValue_ = true;
         } else if (isSgCastExp(node)){

        // DQ: check if this is a cast which will not loose precission.
           SgCastExp* castExp = isSgCastExp(node);
           if (castExp->cast_looses_precision() == true)
              {
                evaluatedValue.hasValue_ = false;
                evaluatedValue.value_ = synList[0].value_;
              }
             else
              {
                evaluatedValue.hasValue_ = true;
                evaluatedValue.value_ = synList[0].value_;
              }
         }
       } else {
         std::cerr << "Expression is not evaluatable" << std::endl;
         evaluatedValue.hasValue_ = false;
         evaluatedValue.value_ = -1;
         return evaluatedValue;
       }
     }
//     evaluatedValue.hasValue_ = true;
     return evaluatedValue;
   }
   struct DSL_Support::const_numeric_expr_t evaluatedValue;
   evaluatedValue.hasValue_ = false;
   evaluatedValue.value_ = -1;
   return evaluatedValue;
 }


  // ********************************************************
  // DQ (10/24/2014): Added feature for constant expresssion 
  // evaluation. This code below is an advanced version of 
  // that is going into the SageInterface.  It should be 
  // available there shortly and when in place it should be 
  // removed from here and referenced from the SageInterface 
  // namespace.
  // ********************************************************

struct DSL_Support::const_numeric_expr_t 
DSL_Support::evaluateConstNumericExpression(SgExpression *expr)
{
  SimpleExpressionEvaluator2 eval;
  return eval.traverse(expr);
}


  // This function supports the DSL when constness is not rigidly followed.
SgExpression* 
DSL_Support::defining_expression ( SgNode* root, SgInitializedName* variable )
   {
  // This function is a work around to not knowing now to use some of the better program analysis 
  // in ROSE that should be a more efficient (but maybe less robust) way of doing the same thing.
  // This will for the moment support a concept of finding the only defining declaration for a variable 
  // globally across the program's AST.  As such this is a DSL specific concept and outside of
  // a inter-procedural use-def analysis (which would be better).

#if 0
     printf ("In DSL_Support::defining_expression(): Search the AST for the assignment to variable = %p = %s \n",variable,variable->get_name().str());
#endif

     class AssignmentSetTraversal : public AstSimpleProcessing
        {
          private:
               SgInitializedName* targetVariable;

          public:
               SgExpression* definingRhsExpression;

          public:
               AssignmentSetTraversal ( SgInitializedName* input_targetVariable ) 
                  {
                    targetVariable        = input_targetVariable;
                    definingRhsExpression = NULL;
                  }

               void visit (SgNode* node)
                  {
                    SgAssignOp* assignment = isSgAssignOp(node);
                    if (assignment != NULL)
                       {
                      // Get the lhs:
                         SgExpression* lhs = assignment->get_lhs_operand();
                         SgExpression* rhs = assignment->get_rhs_operand();

                         SgVarRefExp* varRefExp = isSgVarRefExp(lhs);
                         if (varRefExp != NULL)
                            {
                              SgInitializedName* var = varRefExp->get_symbol()->get_declaration();
                              if (var != NULL)
                                 {
                                   ROSE_ASSERT(targetVariable != NULL);
                                   if (var == targetVariable)
                                      {
                                        printf ("Sucess, we have identified the expression used to set the non-member variable = %p = %s \n",targetVariable,targetVariable->get_name().str());
                                        definingRhsExpression = rhs;
                                      }
                                     else
                                      {
#if 0
                                        printf ("variable on lhs does not match target var = %p = %s \n",var,var->get_name().str());
#endif
                                      }
                                 }
                                else
                                 {
                                 }
                            }
                           else
                            {
                              SgBinaryOp* binaryOp = isSgBinaryOp(lhs);

                           // This might be a member variable.
                              SgArrowExp* arrowExp = isSgArrowExp(binaryOp);
                              SgDotExp*   dotExp   = isSgDotExp(binaryOp);
                              if (arrowExp != NULL || dotExp != NULL)
                                 {
#if 0
                                   printf ("Found binaryOp = %p = %s \n",binaryOp,binaryOp->class_name().c_str());
#endif
                                   SgExpression* expression = binaryOp->get_rhs_operand();
                                   ROSE_ASSERT(expression != NULL);

                                   SgVarRefExp* varRefExp = isSgVarRefExp(expression);
                                   if (varRefExp != NULL)
                                      {
#if 0
                                        printf ("Found a SgVarRefExp: varRefExp = %p name = %s \n",varRefExp,SageInterface::get_name(varRefExp).c_str());
#endif
                                        ROSE_ASSERT(varRefExp->get_symbol() != NULL);
                                        SgInitializedName* var = varRefExp->get_symbol()->get_declaration();

                                        if (var == targetVariable)
                                           {
                                             printf ("Sucess, we have identified the expression used to set the member variable = %p = %s \n",targetVariable,targetVariable->get_name().str());
                                             definingRhsExpression = rhs;
                                           }
                                          else
                                           {
#if 0
                                             printf ("variable on lhs does not match target var = %p = %s \n",var,var->get_name().str());
#endif
                                           }
                                      }
                                 }
                                else
                                 {
                                   printf ("lhs not identified as part of DSL: lhs = %p = %s = %s \n",lhs,lhs->class_name().c_str(),lhs->unparseToString().c_str());
                                   ROSE_ASSERT(false);
                                 }
                            }
                       }

                    SgInitializedName* initializedName = isSgInitializedName(node); 
                    if (initializedName != NULL)
                       {
#if 0
                         printf ("In traversal: initializedName = %p = %s \n",initializedName,initializedName->get_name().str());
#endif
                       }
                  }
        };

     ROSE_ASSERT(variable != NULL);

  // Now buid the traveral object and call the traversal (preorder) on the AST subtree.
     AssignmentSetTraversal traversal(variable);
     traversal.traverse(root, preorder);

#if 0
     printf ("Exiting as a test! \n");
     ROSE_ASSERT(false);
#endif

     ROSE_ASSERT(traversal.definingRhsExpression != NULL);

     printf ("Leaving defining_expression(): traversal.definingRhsExpression = %s \n",traversal.definingRhsExpression->unparseToString().c_str());

     return traversal.definingRhsExpression;
   }


void
DSL_Support::outputGeneratedData()
   {
  // DQ (2/3/2016): Added output function for generated data about target DSL abstractions.

     printf ("\n\nOutput of DSL target abstractions (types, functions, and member functions): \n");

     printf ("   --- DSL target types (size = %zu): \n",dsl_type_names.size());
     for (size_t i=0; i < dsl_type_names.size(); i++)
        {
          printf ("      --- type name = %s \n",dsl_type_names[i].c_str());
        }
     printf ("   --- DSL target functions (size = %zu): \n",dsl_function_names.size());
     for (size_t i=0; i < dsl_function_names.size(); i++)
        {
          printf ("      --- function name = %s \n",dsl_function_names[i].c_str());
        }
     printf ("   --- DSL target member functions (size = %zu): \n",dsl_member_function_names.size());
     for (size_t i=0; i < dsl_member_function_names.size(); i++)
        {
          printf ("      --- class = %s member function name = %s \n",dsl_member_function_names[i].first.c_str(),dsl_member_function_names[i].second.c_str());
        }

   }

void
DSL_Support::checkAndResetToMakeConsistantCompilerGenerated ( SgInitializedName* initializedName )
   {
  // DQ (2/15/2015): This makes up for a bug in the ROSE AST (to be fixed later).
  // It seems that the compiler generated mode is not set uniformally between the 
  // starting and the ending source position construct.

     ROSE_ASSERT(initializedName->get_startOfConstruct() != NULL);
     bool isCompilerGenerated_result = initializedName->get_startOfConstruct()->isCompilerGenerated();
     ROSE_ASSERT(initializedName->get_endOfConstruct() != NULL);
     if (isCompilerGenerated_result != initializedName->get_endOfConstruct()->isCompilerGenerated())
        {
#if 0
          initializedName->get_endOfConstruct()->display("In DetectionTraversal::evaluateInheritedAttribute(): error: startOfConstruct()->isCompilerGenerated() != endOfConstruct()->isCompilerGenerated(): debug");
#endif
          initializedName->get_endOfConstruct()->setCompilerGenerated();
        }
     ROSE_ASSERT(isCompilerGenerated_result == initializedName->get_endOfConstruct()->isCompilerGenerated());
   }



bool DSL_Support::isDslVariable(SgNode* astNode)
   {
  // Recognition of DSL variables

     bool returnValue = false;

     SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(astNode);
     if (variableDeclaration != NULL)
        {
       // Get the SgInitializedName from the SgVariableDeclaration.
          SgInitializedName* initializedName = SageInterface::getFirstInitializedName(variableDeclaration);

          SgType* base_type = initializedName->get_type()->findBaseType();
          ROSE_ASSERT(base_type != NULL);

       // SgClassType* classType = isSgClassType(initializedName->get_type());
          SgClassType* classType = isSgClassType(base_type);

          if (classType != NULL)
             {
#if 1
               printf ("In isDslVariable(): case SgClassType: class name = %s \n",classType->get_name().str());
#endif
            // Check if this is associated with a template instantiation.
               SgTemplateInstantiationDecl* templateInstantiationDecl = isSgTemplateInstantiationDecl(classType->get_declaration());
               if (templateInstantiationDecl != NULL)
                  {
#if 1
                    printf ("case SgTemplateInstaiationDecl: class name = %s \n",classType->get_name().str());
                    printf ("case SgTemplateInstaiationDecl: templateInstantiationDecl->get_templateName() = %s \n",templateInstantiationDecl->get_templateName().str());
#endif
                 // inheritedAttribute.set_StencilDeclaration(templateInstantiationDecl->get_templateName() == "Stencil");
                 // inheritedAttribute.set_StencilOperatorDeclaration(templateInstantiationDecl->get_templateName() == "StencilOperator");

                 // if (templateInstantiationDecl->get_templateName() == "Stencil")
                    string templateName = templateInstantiationDecl->get_templateName();
                    std::vector<std::string>::iterator it = find(dsl_type_names.begin(),dsl_type_names.end(),templateName);
                    if (it != dsl_type_names.end())
                       {
                      // DQ (2/8/2015): Ignore compiler generated IR nodes (from template instantiations, etc.).
                      // Note that simpleCNS.cpp generates one of these from it's use of the tuple template and associated template instantations.

                      // DQ: Test the DSL support.
                      // ROSE_ASSERT(isMatchingClassType(classType,"Stencil",true) == true);
                         ROSE_ASSERT(isMatchingClassType(classType,templateName,true) == true);

                         checkAndResetToMakeConsistantCompilerGenerated(initializedName);

                         if (initializedName->isCompilerGenerated() == false)
                            {
                           // Save the SgInitializedName associated with the stencil.
                           // stencilInitializedNameList.push_back(initializedName);
                           // inheritedAttribute.set_StencilDeclaration(true);
                           // foundStencilVariable = true;
#if 1
                              printf ("In isDslVariable(): detected templateName = %s typed variable: initializedName = %p name = %s \n",templateName.c_str(),initializedName,initializedName->get_name().str());
                           // printf ("   --- stencilInitializedNameList.size() = %zu \n",stencilInitializedNameList.size());
#endif
#if 0
                              initializedName->get_file_info()->display("In isDslVariable(): initializedName : debug");
#endif
                              returnValue = true;
                            }
                       }
                  }
                 else
                  {
                    ROSE_ASSERT(templateInstantiationDecl == NULL);

                    SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                    if (classDeclaration != NULL)
                       {
                         string className = classDeclaration->get_name();
                         std::vector<std::string>::iterator it = find(dsl_type_names.begin(),dsl_type_names.end(),className);
                         if (it != dsl_type_names.end())
                            {
                           // Save the SgInitializedName associated with the Point type.
#if 1
                              printf ("Detected className = %s typed variable: initializedName = %p name = %s \n",className.c_str(),initializedName,initializedName->get_name().str());
#endif
                              checkAndResetToMakeConsistantCompilerGenerated(initializedName);

                              if (initializedName->isCompilerGenerated() == false)
                                 {
#if 1
                                   array_dsl_attribute* dslAttribute = new array_dsl_attribute();
#if 1
                                   printf ("Adding (array_dsl_attribute to dsl variable) dslAttribute = %p \n",dslAttribute);
#endif
                                   ROSE_ASSERT(dslAttribute != NULL);

                                // virtual void addNewAttribute (std::string s, AstAttribute *a);   
                                   initializedName->addNewAttribute(className,dslAttribute);
#endif
                                 }

                              returnValue = true;
                            }
                       }
                  }
             }

#if 1
          printf ("Found a DSL variable declaration \n");
#endif
#if 0
          variableDeclaration->get_file_info()->display("location of stencil declaration: debug");
#endif
        }

     return returnValue;
   }


bool DSL_Support::isDslFunction(SgNode* astNode)
   {
  // Recognition of DSL function abstractions (likely friend functions of classes defining types of DSL variables 
  // or functions taking DSL variablews are parameters or returning DSL variables).

     bool returnValue = false;

  // Find the invocation of the stencil operator (via a SgFunctionCallExp).
     SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
     if (functionCallExp != NULL)
        {
       // Here we make assumptions on how the stencil is specified in the DSL.
          SgDotExp* dotExp = isSgDotExp(functionCallExp->get_function());
          if (dotExp != NULL)
             {
               SgVarRefExp* varRefExp = isSgVarRefExp(dotExp->get_lhs_operand());
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(dotExp->get_rhs_operand());

            // Check if this is found a bit deeper.
               if (varRefExp == NULL)
                  {
                    SgArrowExp* arrowExp   = isSgArrowExp(dotExp->get_lhs_operand());
                    if (arrowExp != NULL)
                       {
                         varRefExp = isSgVarRefExp(arrowExp->get_rhs_operand());
                       }
                    ROSE_ASSERT(varRefExp != NULL);
                  }

               if (varRefExp != NULL && memberFunctionRefExp != NULL)
                  {
                 // if (initializedNameList.find(initializedName) != initializedNameList.end())
                    SgVariableSymbol* variableSymbol = isSgVariableSymbol(varRefExp->get_symbol());
                    SgInitializedName* initializedName = variableSymbol->get_declaration();
                    ROSE_ASSERT(initializedName != NULL);

                    SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();

                    printf ("In isDslFunction(): case SgDotExp: found function ref name = %s from variable reference for variable name = %s \n",memberFunctionSymbol->get_name().str(),initializedName->get_name().str());

                // Add attribute
#if 1
                   array_assignment_operator_dsl_attribute* dslAttribute = new array_assignment_operator_dsl_attribute();
#if 1
                   printf ("Adding (array_assignment_operator_dsl_attribute to dsl function call) dslAttribute = %p \n",dslAttribute);
#endif
                   ROSE_ASSERT(dslAttribute != NULL);

                // virtual void addNewAttribute (std::string s, AstAttribute *a);   
                   functionCallExp->addNewAttribute(memberFunctionSymbol->get_name(),dslAttribute);
#endif
#if 1
                   array_refexp_dsl_attribute* dslAttribute_refexp = new array_refexp_dsl_attribute();
#if 1
                   printf ("Adding (array_refexp_dsl_attribute to dsl var ref) dslAttribute = %p \n",dslAttribute_refexp);
#endif
                   ROSE_ASSERT(dslAttribute_refexp != NULL);

                // virtual void addNewAttribute (std::string s, AstAttribute *a);   
                   varRefExp->addNewAttribute(initializedName->get_name(),dslAttribute_refexp);
#endif
                  }

               string memberFunctionName = memberFunctionRefExp->get_symbol()->get_name();
               printf ("In isDslFunction(): case SgDotExp: found memberFunctionName = %s \n",memberFunctionName.c_str());

               std::vector<std::string>::iterator it = find(dsl_function_names.begin(),dsl_function_names.end(),memberFunctionName);
               if (it != dsl_function_names.end())
                  {
                    printf ("Detected function call from DSL variable:  memberFunctionName = %s \n",memberFunctionName.c_str());
                  }

            // DQ (2/16/2015): Check for operator*() in "const Point hi = getOnes() * scalar;"
            // This is actually the better and most general test for a specific member function 
            // call (since calling off of a variable is not general enough).
               SgFunctionCallExp* nestedFunctionCallExp = isSgFunctionCallExp(dotExp->get_lhs_operand());
               if (nestedFunctionCallExp != NULL)
                  {
                    SgType* returnType = nestedFunctionCallExp->get_type();
                    ROSE_ASSERT(returnType != NULL);
#if 0
                    printf ("returnType = %p = %s \n",returnType,returnType->class_name().c_str());
#endif
                    SgType* base_type = returnType->findBaseType();
                    ROSE_ASSERT(base_type != NULL);

                    SgClassType* classType = isSgClassType(base_type);
                    if (classType != NULL)
                       {
                         SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
                         if (classDeclaration != NULL)
                            {
                              string className = classDeclaration->get_name();
                              printf ("In isDslFunction(): case SgDotExp: case function returning DSL type: class name = %s \n",className.c_str());
                            }
                       }
#if 0
                    printf ("Exiting as a test! \n");
                    ROSE_ASSERT(false);
#endif
                  }
             }

       // This is the case for the apply function in the ShiftCalculus.
          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(functionCallExp->get_function());
          if (memberFunctionRefExp != NULL)
             {
            // We might have to narrow these to the specific cases were are interested in.
               SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();

               string memberFunctionName = memberFunctionSymbol->get_name();
               printf ("In isDslFunction(): case SgMemberFunctionRefExp: memberFunctionName = %s \n",memberFunctionName.c_str());
             }

          SgFunctionRefExp* functionRefExp = isSgFunctionRefExp(functionCallExp->get_function());
          if (functionRefExp != NULL)
             {
            // We might have to narrow these to the specific cases were are interested in.
               SgFunctionSymbol* functionSymbol = functionRefExp->get_symbol();

               printf ("***** functionSymbol->get_name() = %s \n",functionSymbol->get_name().str());

            // DQ: Added support for new attribute.
               SgFunctionDeclaration* functionDeclaration = functionSymbol->get_declaration();
               ROSE_ASSERT(functionDeclaration != NULL);

               printf ("functionDeclaration = %p = %s \n",functionDeclaration,functionDeclaration->get_name().str());

               SgTemplateInstantiationFunctionDecl* templateInstantiationFunctionDecl = isSgTemplateInstantiationFunctionDecl(functionDeclaration);
            // ROSE_ASSERT(templateInstantiationFunctionDecl != NULL);

               if (templateInstantiationFunctionDecl != NULL)
                  {
                    printf ("--- templateInstantiationFunctionDecl->get_name() = %s \n",templateInstantiationFunctionDecl->get_name().str());
                 // printf ("--- templateInstantiationFunctionDecl->get_template_name() = %s \n",templateInstantiationFunctionDecl->get_template_name().str());
                    printf ("--- templateInstantiationFunctionDecl->get_templateName() = %s \n",templateInstantiationFunctionDecl->get_templateName().str());
                  }

               if (templateInstantiationFunctionDecl != NULL)
                  {
                    string templateName = (templateInstantiationFunctionDecl != NULL) ? templateInstantiationFunctionDecl->get_templateName() : "";
                    printf ("In isDslFunction(): templateName = %s \n",templateName.c_str());
                  }
                 else
                  {
                    string functionName = functionSymbol->get_name();
                    printf ("In isDslFunction(): functionName = %s \n",functionName.c_str());

#if 1
                   plus_operator_dsl_attribute* dslAttribute = new plus_operator_dsl_attribute();
#if 1
                   printf ("Adding (plus_operator_dsl_attribute to dsl function call) dslAttribute = %p \n",dslAttribute);
#endif
                   ROSE_ASSERT(dslAttribute != NULL);

                // virtual void addNewAttribute (std::string s, AstAttribute *a);   
                   functionCallExp->addNewAttribute(functionSymbol->get_name(),dslAttribute);
#endif
                  }
             }

       // Now add attributes to the function arguments.
          SgExprListExp* argumentList = functionCallExp->get_args();
          ROSE_ASSERT(argumentList != NULL);

          for (size_t i = 0; i < argumentList->get_expressions().size(); i++)
             {
               SgExpression* arg = argumentList->get_expressions()[i];
               ROSE_ASSERT(arg != NULL);

               SgVarRefExp* varRefExp = isSgVarRefExp(arg);
               if (varRefExp != NULL)
                  {
#if 1
                   array_refexp_dsl_attribute* dslAttribute_refexp = new array_refexp_dsl_attribute();
#if 1
                   printf ("Adding (array_refexp_dsl_attribute to dsl var ref) dslAttribute = %p \n",dslAttribute_refexp);
#endif
                   ROSE_ASSERT(dslAttribute_refexp != NULL);

                // virtual void addNewAttribute (std::string s, AstAttribute *a);   
                   varRefExp->addNewAttribute("varRef",dslAttribute_refexp);
#endif
                  }

               SgValueExp* valueExp = isSgValueExp(arg);
               if (valueExp != NULL)
                  {
#if 1
                   value_refexp_dsl_attribute* dslAttribute_value = new value_refexp_dsl_attribute();
#if 1
                   printf ("Adding (value_refexp_dsl_attribute to dsl value) dslAttribute = %p \n",dslAttribute_value);
#endif
                   ROSE_ASSERT(dslAttribute_value != NULL);

                // virtual void addNewAttribute (std::string s, AstAttribute *a);   
                   valueExp->addNewAttribute("value",dslAttribute_value);
#endif
                  }
             }
        }

     return returnValue;
   }


bool DSL_Support::isDslMemberFunction(SgNode* astNode)
   {
  // Note clear if we need this function (the seperation into DSL member function
  // might have a lot of redundant code with the isDslFunction() function above).

  // Recognition of DSL member function abstractions (member functions of classes defining types of DSL variables).

     bool returnValue = false;

     return returnValue;
   }


bool DSL_Support::isDslAbstraction(SgNode* astNode)
   {
  // Detection of target abstractions.

  // The return value could be the associated attribute, or this function could add the attribute directly.
  // I think it is better for this function to add the attribute.

     bool returnValue = false;

     bool detectedDslVariable       = isDslVariable(astNode);
     bool detectedDslFunction       = isDslFunction(astNode);
  // bool detectedDslMemberFunction = isDslMemberFunction(astNode);

  // if (detectedDslVariable == true || detectedDslFunction == true || detectedDslMemberFunction == true)
     if (detectedDslVariable == true || detectedDslFunction == true)
        {
       // Identify the correct attribute and attach it to the IR node.

          if (detectedDslVariable == true)
             {
            // Select the attribute for this DSL variable.
               ROSE_ASSERT(detectedDslFunction == false);
             }

          if (detectedDslFunction == true)
             {
            // Select the attribute for this DSL function.
               ROSE_ASSERT(detectedDslVariable == false);
             }

        }

     return returnValue;
   }

