// Example ROSE Translator reads input program and implements a DSL embedded within C++
// to support the stencil computations, and required runtime support is developed seperately.
#include "rose.h"

// #include "shiftCalculusCompiler.h"

// #include "stencilAndStencilOperatorDetection.h"
// #include "stencilEvaluation.h"

#include "dslSupport.h"

// This code will make calles to the finite state machine representing the stencil 
// so that we can execute events and accumulate state (and then read the state as
// and intermediate form for the stencil (maybe saved as an attribute).  This data
// is then the jumping off point for different groups to experiment with the generation
// of architecture specific code.
// #include "stencilFiniteStateMachine.h"

using namespace std;


// This is a function refactoring code used within the stencil evaluation.
// Likely it is generally useful in writing DSLs so it should be refactored 
// to be elsewhere directly in ROSE (e.g. SageInterface namespace).
bool DSL_Support::isMatchingClassType(SgType* type, const string & name, bool isTemplateClass)
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



bool DSL_Support::isMatchingMemberFunction(SgMemberFunctionRefExp* memberFunctionRefExp, const string & name, bool isTemplateInstantiation)
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
           printf ("templateInstantiationMemberFunctionDecl->get_name()         = %s \n",templateInstantiationMemberFunctionDecl->get_name().str());
           printf ("templateInstantiationMemberFunctionDecl->get_templateName() = %s \n",templateInstantiationMemberFunctionDecl->get_templateName().str());
#endif
           if (templateInstantiationMemberFunctionDecl->get_templateName() == name)
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
     printf ("In DSL_Support::buildMemberFunctionCall(): expressionRoot = %p = %s memberFunctionName = %s expression = %p \n",expressionRoot,expressionRoot->class_name().c_str(),memberFunctionName.c_str(),expression);
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

  // For the moment we will assume this is not a overloaded operator.
  // SgMemberFunctionSymbol* memberFunctionSymbol = classDefinition->lookup_nontemplate_member_function_symbol("opearator[]");
  // SgFunctionSymbol* functionSymbol = classDefinition->lookup_function_symbol("operator[]");
     SgFunctionSymbol* functionSymbol = classDefinition->lookup_function_symbol(memberFunctionName);
     if (functionSymbol == NULL)
        {
           printf ("Error: function not found in classDeclaration = %s symbol table: memberFunctionName = %s \n",classDeclaration->get_name().str(),memberFunctionName.c_str());
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

