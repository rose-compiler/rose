#include "sage3basic.h"

#include "transformationSupport.h"
#include "nameQuery.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;

// DQ (3/6/2017): Added support for message logging to control output from ROSE tools.
#undef mprintf
#define mprintf Rose::Diagnostics::mfprintf(Rose::ir_node_mlog[Rose::Diagnostics::DEBUG])

string
TransformationSupport::stringifyOperator (string name)
   {
  // This function figures out what sort of operator is used from the name of the overloaded
  // operator (a function name in C++) and returns the associated operator string for the
  // overloaded operators element wise operators.

     string operatorString;
     int stringLength = name.length();
     ROSE_ASSERT (stringLength >= 3);

     char lastCharacter = name[stringLength-1];
     char secondLastCharacter = name[stringLength-2];
     char thirdLastCharacter = name[stringLength-3];

  // Now turn the last character code into a string (we want to return a string instead of a character!)
     if (secondLastCharacter == 'r')
        {
       // Case of an operator ending with a ONE character specifier
          switch (lastCharacter)
             {
               case '=': operatorString = "="; break;
               case '!': operatorString = "!"; break;
               case '<': operatorString = "<"; break;
               case '>': operatorString = ">"; break;
               case '+': operatorString = "+"; break;
               case '-': operatorString = "-"; break;
               case '*':
                 // Both the multiplication operator and the reference operator return "*"
                    operatorString = "*";
                    break;
               case '/': operatorString = "/"; break;
               case '%': operatorString = "%"; break;
               case '&': 
                 // Both the bitwise AND operator and the address operator return "&"
                    operatorString = "&";
                    break;
               case '|': operatorString = "|"; break;
               case '^': operatorString = "^"; break;
               default:
                    printf ("default reached in case secondLastCharacter == 'r' of identifyOperator() \n");
                    ROSE_ABORT();
             }
        }
       else
        {
       // Case of an operator ending with a TWO character specifier
          ROSE_ASSERT (secondLastCharacter != 'r');
          switch (lastCharacter)
             {
               case '=':
                  {
                    switch (secondLastCharacter)
                       {
                         case 'r': operatorString = "=";  break;
                         case '=': operatorString = "=="; break;
                         case '!': operatorString = "!="; break;
                         case '<':  // distinguish between <= and <<=
                              if (thirdLastCharacter == '<')
                                   operatorString = "<<=";
                                else
                                 {
                                   ROSE_ASSERT (thirdLastCharacter == 'r');
                                   operatorString = "<=";
                                 }
                              break;
                       case '>':  // distinguish between >= and >>=
                              if (thirdLastCharacter == '>')
                                   operatorString = ">>=";
                                else
                                 {
                                   ROSE_ASSERT (thirdLastCharacter == 'r');
                                   operatorString = ">=";
                                 }
                              break;
                         case '+': operatorString = "+="; break;
                         case '-': operatorString = "-="; break;
                         case '*': operatorString = "*="; break;
                         case '/': operatorString = "/="; break;
                         case '%': operatorString = "%="; break;
                         case '&': operatorString = "&="; break;
                         case '|': operatorString = "|="; break;
                         case '^': operatorString = "^="; break;
                         default:
                              printf ("default reached in case '=' of identifyOperator() \n");
                              ROSE_ABORT();
                       }
                    break;
                  }
               case '&':
                  {
                    ROSE_ASSERT (secondLastCharacter == '&');
                    operatorString = "&&";
                    break;
                  }
               case '|':
                  {
                    ROSE_ASSERT (secondLastCharacter == '|');
                    operatorString = "||";
                    break;
                  }
               case '<':
                  {
                    ROSE_ASSERT (secondLastCharacter == '<');
                    operatorString = "<<";
                    break;
                  }
               case '>':
                  {
                    ROSE_ASSERT (secondLastCharacter == '>');
                    operatorString = ">>";
                    break;
                  }
               case ')':
                  {
                    ROSE_ASSERT (secondLastCharacter == '(');
                    operatorString = "()";
                    break;
                  }
               case ']':
                  {
                    ROSE_ASSERT (secondLastCharacter == '[');
                    operatorString = "[]";
                    break;
                  }
               case '+':
                  {
                    ROSE_ASSERT (secondLastCharacter == '+');
                    operatorString = "++";
                    break;
                  }
               case '-':
                  {
                    ROSE_ASSERT (secondLastCharacter == '-');
                    operatorString = "--";
                    break;
                  }
               default:
                    printf ("default reached in case secondLastCharacter != 'r' of identifyOperator() \n");
                    ROSE_ABORT();
             }
        }

     ROSE_ASSERT (operatorString.length() > 0);

  // Copy the string to avoid returning a reference to local (stack) memory
     string returnString = operatorString;

     printf ("TransformationSupport::stringifyOperator() returnString = %s \n",returnString.c_str());

     return returnString;
   }

TransformationSupport::operatorCodeType
TransformationSupport::classifyOverloadedOperator ( string name, int numberOfParameters, bool prefixOperator )
   {
  // This function figures out what sort of operator is used from the name of the overloaded
  // operator (a function name in C++) and returns the associated operator string for the
  // overloaded operators element wise operators.

  // Note: the value of numberOfParameters is important in distinguishing between
  // multiplication-operator/reference-operator and the bitwise-AND-operator/address-operator.  The
  // prefixOperator is important in distinguishing between ++int/int++ and --int/int-- operators.

  // Use the variants within the C++ grammar to classify associated elementwise operators for
  // overloaded functions.  The default is a FUNC_CALL value indicating that the input name is
  // associated with a function call (e.g. cos(A) in A++/P++ is associated with the math function
  // call cos(A_ptr[i])).  We could not use the Cxx_GrammarVariants enum since it does not contain a
  // classification for operator() and operator[].
     operatorCodeType operatorCode = FUNCTION_CALL_OPERATOR_CODE;

     int stringLength = name.length();
     ROSE_ASSERT (stringLength >= 3);

     char lastCharacter = name[stringLength-1];
     char secondLastCharacter = name[stringLength-2];
     char thirdLastCharacter = name[stringLength-3];

  // Now turn the last character code into a string (we want to return a string instead of a character!)
     if (secondLastCharacter == 'r')
        {
       // Case of an operator ending with a ONE character specifier
          switch (lastCharacter)
             {
               case '=': operatorCode = ASSIGN_OPERATOR_CODE; break;
               case '!': operatorCode = NOT_OPERATOR_CODE; break;
               case '<': operatorCode = LT_OPERATOR_CODE; break;
               case '>': operatorCode = GT_OPERATOR_CODE; break;
               case '+': operatorCode = ADD_OPERATOR_CODE; break;
               case '-': operatorCode = SUBT_OPERATOR_CODE; break;
               case '*':
                 // Need to distinguish between the multiplication operator and the reference operator
                    if (numberOfParameters > 0)
                         operatorCode = MULT_OPERATOR_CODE;
                      else
                         operatorCode = DEREFERENCE_OPERATOR_CODE;
                    break;
               case '/': operatorCode = DIV_OPERATOR_CODE; /* or INTEGER_DIV_OPERATOR_CODE */  break;
               case '%': operatorCode = MOD_OPERATOR_CODE; break;
               case '&':
                 // Need to distinguish between the bitwise AND operator and the address operator
                    if (numberOfParameters > 0)
                         operatorCode = BITAND_OPERATOR_CODE;
                      else
                         operatorCode = ADDRESS_OPERATOR_CODE;
                    break;
               case '|': operatorCode = BITOR_OPERATOR_CODE; break;
               case '^': operatorCode = BITXOR_OPERATOR_CODE; break;
               default:
                    printf ("default reached in case secondLastCharacter == 'r' of identifyOperator() \n");
                    ROSE_ABORT();
             }
        }
       else
        {
       // Case of an operator ending with a TWO character specifier
          ROSE_ASSERT (secondLastCharacter != 'r');
          switch (lastCharacter)
             {
               case '=':
                  {
                    switch (secondLastCharacter)
                       {
                         case '=': operatorCode = EQ_OPERATOR_CODE; break;
                         case '!': operatorCode = NE_OPERATOR_CODE; break;
                         case '<':  // distinguish between <= and <<=
                              if (thirdLastCharacter == '<')
                                   operatorCode = LSHIFT_ASSIGN_OPERATOR_CODE;
                                else
                                 {
                                   ROSE_ASSERT (thirdLastCharacter == 'r');
                                   operatorCode = LE_OPERATOR_CODE;
                                 }
                              break;
                         case '>':  // distinguish between >= and >>=
                              if (thirdLastCharacter == '>')
                                   operatorCode = RSHIFT_ASSIGN_OPERATOR_CODE;
                                else
                                 {
                                   ROSE_ASSERT (thirdLastCharacter == 'r');
                                   operatorCode = GE_OPERATOR_CODE;
                                 }
                              break;
                         case '+': operatorCode = PLUS_ASSIGN_OPERATOR_CODE;  break;
                         case '-': operatorCode = MINUS_ASSIGN_OPERATOR_CODE; break;
                         case '*': operatorCode = MULT_ASSIGN_OPERATOR_CODE;  break;
                         case '/': operatorCode = DIV_ASSIGN_OPERATOR_CODE;   break;
                         case '%': operatorCode = MOD_ASSIGN_OPERATOR_CODE;   break;
                         case '&': operatorCode = AND_ASSIGN_OPERATOR_CODE;   break;
                         case '|': operatorCode = IOR_ASSIGN_OPERATOR_CODE;   break;
                         case '^': operatorCode = XOR_ASSIGN_OPERATOR_CODE;   break;
                         default:
                              printf ("default reached in case '=' of identifyOperator() secondLastCharacter = %c \n",secondLastCharacter);
                              ROSE_ABORT();
                       }
                    break;
                  }
               case '&':
                  {
                    ROSE_ASSERT (secondLastCharacter == '&');
                    operatorCode = AND_OPERATOR_CODE;
                    break;
                  }
               case '|':
                  {
                    ROSE_ASSERT (secondLastCharacter == '|');
                    operatorCode = OR_OPERATOR_CODE;
                    break;
                  }
               case '<':
                  {
                    ROSE_ASSERT (secondLastCharacter == '<');
                    operatorCode = LSHIFT_OPERATOR_CODE;
                    break;
                  }
               case '>':
                  {
                    ROSE_ASSERT (secondLastCharacter == '>');
                    operatorCode = RSHIFT_OPERATOR_CODE;
                    break;
                  }
               case ')':
                  {
                    ROSE_ASSERT (secondLastCharacter == '(');
                    operatorCode = PARENTHESIS_OPERATOR_CODE;
                    break;
                  }
               case ']':
                  {
                    ROSE_ASSERT (secondLastCharacter == '[');
                    operatorCode = BRACKET_OPERATOR_CODE;
                    break;
                  }
               case '+':
                  {
                    ROSE_ASSERT (secondLastCharacter == '+');
                    if (prefixOperator)
                         operatorCode = PREFIX_PLUSPLUS_OPERATOR_CODE;
                      else
                         operatorCode = POSTFIX_PLUSPLUS_OPERATOR_CODE;
                    break;
                  }
               case '-':
                  {
                    ROSE_ASSERT (secondLastCharacter == '-');
                    if (prefixOperator)
                         operatorCode = PREFIX_MINUSMINUS_OPERATOR_CODE;
                      else
                         operatorCode = POSTFIX_MINUSMINUS_OPERATOR_CODE;
                    break;
                  }
               default:
                    printf ("default reached in case secondLastCharacter != 'r' of identifyOperator() lastCharacter = %c \n",lastCharacter);
             }
        }

     return operatorCode;
   }


string
TransformationSupport::buildOperatorString ( SgNode* astNode )
   {
  // This function defines the operator string used between 
  // the lhs and rhs for all binary operators. It supports
  // the assembly function below.
     ASSERT_not_null(astNode);
     string operatorString;

     switch (astNode->variantT())
        {
          case V_SgAddOp:           operatorString = "+";  break;
          case V_SgSubtractOp:          operatorString = "-";  break;
          case V_SgMultiplyOp:          operatorString = "*";  break;
          case V_SgDivideOp:           operatorString = "/";  break;
          case V_SgIntegerDivideOp:   operatorString = "/";  break;
          case V_SgModOp:           operatorString = "%";  break;
          case V_SgAndOp:           operatorString = "&&"; break;
          case V_SgOrOp:            operatorString = "||"; break;
          case V_SgBitXorOp:        operatorString = "^";  break;
          case V_SgBitAndOp:        operatorString = "&";  break;
          case V_SgBitOrOp:         operatorString = "|";  break;
          case V_SgEqualityOp:            operatorString = "=="; break;
          case V_SgLessThanOp:            operatorString = "<";  break;
          case V_SgGreaterThanOp:            operatorString = ">";  break;
          case V_SgNotEqualOp:            operatorString = "!="; break;
          case V_SgLessOrEqualOp:            operatorString = "<="; break;
          case V_SgGreaterOrEqualOp:            operatorString = ">="; break;
          case V_SgAssignOp:        operatorString = "=";  break;
          case V_SgPlusAssignOp:   operatorString = "+="; break;
          case V_SgMinusAssignOp:  operatorString = "-="; break;
          case V_SgAndAssignOp:    operatorString = "&="; break;
          case V_SgIorAssignOp:    operatorString = "|="; break;
          case V_SgMultAssignOp:   operatorString = "*="; break;
          case V_SgDivAssignOp:    operatorString = "/="; break;
          case V_SgModAssignOp:    operatorString = "%="; break;
          case V_SgXorAssignOp:    operatorString = "^="; break;
          case V_SgLshiftAssignOp: operatorString = "<<="; break;
          case V_SgRshiftAssignOp: operatorString = ">>="; break;

       // Since a SgMemberFunctionRefExp is derived from a SgFunctionCallExp we need only handle one case
       // case V_SgMemberFunctionRefExp:
          case V_SgFunctionCallExp:   // SgFunctionCallExp
             {
               SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
               ASSERT_not_null(functionCallExp);

               SgExpression* functionExpression = functionCallExp->get_function();
               ASSERT_not_null(functionExpression);

               operatorString = buildOperatorString (functionExpression);
               break;
             }
          case V_SgMemberFunctionRefExp:
             {
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(astNode);
               ASSERT_not_null(memberFunctionRefExp);

               SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();
               ASSERT_not_null(memberFunctionSymbol);

               operatorString = buildOperatorString (memberFunctionSymbol);
               break;
             }
          case V_SgMemberFunctionSymbol:
            {
               SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(astNode);
               ASSERT_not_null(memberFunctionSymbol);

               SgName name = memberFunctionSymbol->get_name();
               string tempName = name.str();

               int stringLength = tempName.length();
               ASSERT_require(stringLength >= 2);

               bool isAnOperator = false;
               if ( (stringLength >= 8) && (!strncmp (tempName.c_str(),"operator",8)) )
                    isAnOperator = true;

               if ( isAnOperator == true )
                  {
                    operatorString = stringifyOperator(tempName);
                  }
                 else
                  {
                    printf ("isAnOperator == false in buildOperatorString() \n");
                    ROSE_ABORT();
                  }
               break;

            }
          case V_SgFunctionSymbol:
             {
               SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(astNode);
               ASSERT_not_null(functionSymbol);

               SgName name = functionSymbol->get_name();
               string tempName = name.str();

               int stringLength = tempName.length();
               ASSERT_require(stringLength >= 2);

               bool isAnOperator = false;
               if ( (stringLength >= 8) && (!strncmp (tempName.c_str(),"operator",8)) )
                    isAnOperator = true;

               if ( isAnOperator == true )
                  {
                    operatorString = stringifyOperator(tempName);
                  }
                 else
                  {
                    printf ("isAnOperator == false in buildOperatorString() \n");
                    ROSE_ABORT();
                  }
               break;
             }

          case V_SgDotExp:  // SgDotExp
             {
               SgDotExp* dotExp = isSgDotExp(astNode);
               ASSERT_not_null(dotExp);
               SgExpression* rhs = dotExp->get_rhs_operand();
               ASSERT_not_null(rhs);

               operatorString = buildOperatorString (rhs);
               break;
             }
          case V_SgFunctionRefExp: // SgFunctionRefExp
             {
               SgFunctionRefExp* functionReferenceExp = isSgFunctionRefExp(astNode);
               ASSERT_not_null(functionReferenceExp);
               SgFunctionSymbol* functionSymbol = functionReferenceExp->get_symbol();
               ASSERT_not_null(functionSymbol);

               operatorString = buildOperatorString (functionSymbol);
               break;
             }

          default:
               printf ("default reached in buildOperatorString() \n");
               ROSE_ABORT();
        }

     return operatorString;
   }


string
TransformationSupport::getFunctionName ( SgFunctionCallExp* functionCallExp )
   {
     ASSERT_not_null(functionCallExp);

     SgExpression* expression = functionCallExp->get_function();
     ASSERT_not_null(expression);

     string returnNameString;

     SgDotExp* dotExp = isSgDotExp(expression);
     if (dotExp != nullptr)
        {
          ASSERT_not_null(dotExp);

          SgExpression* rhsOperand = dotExp->get_rhs_operand();
          ASSERT_not_null(rhsOperand);

          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsOperand);
          ASSERT_not_null(memberFunctionRefExp);

          SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(memberFunctionRefExp->get_symbol());
          ASSERT_not_null(memberFunctionSymbol);

       // There will be a lot of different possible overloaded operators called
       // and "operator()" is just one of them (that we are debugging presently)

          returnNameString = memberFunctionSymbol->get_name().str();
        }

     SgFunctionRefExp* functionReferenceExp = isSgFunctionRefExp(expression);
     if (functionReferenceExp != nullptr)
       {
         SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(functionReferenceExp->get_symbol());
         ASSERT_not_null(functionSymbol);
         
         returnNameString = functionSymbol->get_name().str();
       }

     SgArrowExp* arrowExp = isSgArrowExp(expression);
     if (arrowExp != nullptr)
       {
         ASSERT_not_null(arrowExp);
         
         SgExpression* rhsOperand = arrowExp->get_rhs_operand();
         ASSERT_not_null(rhsOperand);
         
         SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsOperand);
         ASSERT_not_null(memberFunctionRefExp);
         
         SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(memberFunctionRefExp->get_symbol());
         ASSERT_not_null(memberFunctionSymbol);
         
         // There will be a lot of different possible overloaded operators called
         // and "operator()" is just one of them (that we are debugging presently)
         
         returnNameString = memberFunctionSymbol->get_name().str();
       }
     
     ASSERT_require(returnNameString.length() > 0);
     return returnNameString;
   }

string
TransformationSupport::getTypeName ( SgType* type )
   {
     string typeName;

     switch (type->variantT())
        {
          case V_SgTypeComplex:
               typeName = "complex";
               break;
          case V_SgTypeImaginary:
               typeName = "imaginary";
               break;
          case V_SgTypeBool:
               typeName = "bool";
               break;
          case V_SgTypeChar:
               typeName = "char";
               break;
          case V_SgTypeVoid:
               typeName = "void";
               break;
          case V_SgTypeInt:
               typeName = "int";
               break;
          case V_SgTypeDouble:
               typeName = "double";
               break;
          case V_SgTypeFloat:
               typeName = "float";
               break;
          case V_SgTypeLong:
               typeName = "long";
               break;
          case V_SgTypeLongDouble:
               typeName = "long double";
               break;
          case V_SgTypeEllipse:
               typeName = "ellipse";
               break;
          case V_SgTypeGlobalVoid:
               typeName = "void";
               break;
          case V_SgTypeLongLong:
               typeName = "long long";
               break;
          case V_SgTypeShort:
               typeName = "short";
               break;
          case V_SgTypeSignedChar:
               typeName = "signed char";
               break;
          case V_SgTypeSignedInt:
               typeName = "signed int";
               break;
          case V_SgTypeSignedLong:
               typeName = "signed long";
               break;
          case V_SgTypeSignedShort:
               typeName = "signed short";
               break;
          case V_SgTypeString:
               typeName = "string";
               break;
          case V_SgTypeUnknown:
               typeName = "unknown";
               break;
          case V_SgTypeUnsignedChar:
               typeName = "unsigned char";
               break;
          case V_SgTypeUnsignedInt:
               typeName = "unsigned int";
               break;
          case V_SgTypeUnsignedLong:
               typeName = "unsigned long";
               break;
          case V_SgTypeUnsignedShort:
               typeName = "unsigned short";
               break;
          case V_SgTypeUnsignedLongLong:
               typeName = "unsigned long long";
               break;
          case V_SgReferenceType: 
             {
               ASSERT_not_null( isSgReferenceType(type)->get_base_type() );
               typeName = getTypeName(isSgReferenceType(type)->get_base_type()) + "&";
               break;
             }
          case V_SgPointerType:
             {
               ASSERT_not_null( isSgPointerType(type)->get_base_type() );
               typeName = getTypeName(isSgPointerType(type)->get_base_type()) + "*";
               break;
             }
          case V_SgModifierType:
             {
               ASSERT_not_null( isSgModifierType(type)->get_base_type() );
               SgModifierType *modifier = isSgModifierType(type);
               typeName = modifier->unparseToString();
               break;
             }
          case V_SgEnumType:
          case V_SgNamedType:
             {
               SgNamedType* sageNamedType = isSgNamedType(type);
               ASSERT_not_null(sageNamedType);
               typeName = sageNamedType->get_name().str();
               break;
             }
          case V_SgClassType:
             {
               SgClassType* sageClassType = isSgClassType(type);
               ASSERT_not_null(sageClassType);
               typeName = sageClassType->get_name().str();
               break;
             }
          case V_SgTypedefType:
             {
               SgTypedefType* sageTypedefType = isSgTypedefType(type);
               ASSERT_not_null(sageTypedefType);
               typeName = sageTypedefType->get_name().str();
               break;
             }
          case V_SgPointerMemberType:
             {
               SgPointerMemberType* pointerMemberType = isSgPointerMemberType(type);
               ASSERT_not_null(pointerMemberType);
               SgClassType* classType = isSgClassType(pointerMemberType->get_class_type()->stripTypedefsAndModifiers());
               ASSERT_not_null(classType);
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
               ASSERT_not_null(classDeclaration);
               typeName = classDeclaration->get_name().str();
               break;
             }
          case V_SgArrayType: 
             {
               ASSERT_not_null( isSgArrayType(type)->get_base_type() );
               typeName = getTypeName(isSgArrayType(type)->get_base_type());
               break;
             }
          case V_SgFunctionType:
             {
               SgFunctionType* functionType = isSgFunctionType(type);
               ASSERT_not_null(functionType);
               typeName = functionType->get_mangled_type().str();
               break;
             }
          case V_SgMemberFunctionType:
             {
               SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(type);
               ASSERT_not_null(memberFunctionType);
               SgClassType* classType = isSgClassType(memberFunctionType->get_class_type()->stripTypedefsAndModifiers());
               ASSERT_not_null(classType);
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
               ASSERT_not_null(classDeclaration);
               typeName = classDeclaration->get_name().str();
               break;
             }
          case V_SgTypeWchar:
               typeName = "wchar";
               break;
          case V_SgTypeDefault:
               typeName = "default";
               break;
          default:
             {
               printf ("default reached in switch within TransformationSupport::getTypeName type->sage_class_name() = %s variant = %d \n",
               type->sage_class_name(),type->variant());
               ROSE_ABORT();
               break;
             }
        }

     return typeName;
   }


string
TransformationSupport::getFunctionTypeName ( SgFunctionCallExp* functionCallExpression )
   {
     ASSERT_not_null(functionCallExpression);
     string associatedClassName;

     SgExpression* expression = functionCallExpression->get_function();
     ASSERT_not_null(expression);

     string functionTypeName;
     SgDotExp* dotExpression = isSgDotExp(expression);
     SgArrowExp* arrowExpression = isSgArrowExp(expression);
     if (dotExpression != nullptr)
        {
       // Get the class name associated with the member function
          SgExpression* rhsExpression = dotExpression->get_rhs_operand();
          ASSERT_not_null(rhsExpression);
          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsExpression);
          ASSERT_not_null(memberFunctionRefExp);
          ASSERT_not_null(memberFunctionRefExp->get_type());

          SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(memberFunctionRefExp->get_type());
          ASSERT_not_null(memberFunctionType);
          functionTypeName = TransformationSupport::getTypeName ( memberFunctionType );
        }
     else if (arrowExpression != nullptr)
        {
       // Get the class name associated with the member function
          SgExpression* rhsExpression = arrowExpression->get_rhs_operand();
          ASSERT_not_null(rhsExpression);
          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsExpression);
          ASSERT_not_null(memberFunctionRefExp);
          ASSERT_not_null(memberFunctionRefExp->get_type());
          SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(memberFunctionRefExp->get_type());

          ASSERT_not_null(memberFunctionType);
          functionTypeName = TransformationSupport::getTypeName ( memberFunctionType );
       }
       else
        {
       // Get the class name associated with the friend function
          SgType* functionType    = functionCallExpression->get_type();
          ASSERT_not_null(functionType);
          functionTypeName = TransformationSupport::getTypeName (functionType);
        }

     associatedClassName = functionTypeName;
     ROSE_ASSERT (associatedClassName.length() > 0);

     return associatedClassName;
   }


string
TransformationSupport::buildMacro ( string s )
   {
  // This function wraps the string representing a macro that we want to appear in a transformation
  // so that it can be precessed by the unparser.

     string returnString = s;
     string roseMacroMarkerString = "ROSE-TRANSFORMATION-MACRO:";
     returnString = roseMacroMarkerString + returnString;

  // Wrap the string into quotes for use as a C++ string expression
     returnString = "\"" + returnString;
     returnString = returnString + "\";";

     return returnString;
   }

void
TransformationSupport::getTransformationOptions (
   SgNode* astNode,
   list<int> & generatedList,
   string identifingTypeName )
   {
  // This function searches for variables of type ScopeBasedTransformationOptimization.  Variables
  // of type ScopeBasedTransformationOptimization are used to communicate optimizations from the
  // application to the preprocessor. If called from a project or file object it traverses down to
  // the global scope of the file and searches only the global scope, if called from and other
  // location within the AST it searches the current scope and then traverses the parent nodes to
  // find all enclosing scopes until in reaches the global scope.  At each scope it searches for
  // variables of type ScopeBasedTransformationOptimization.

     ASSERT_not_null(astNode);
     ASSERT_not_null(identifingTypeName.c_str());

     switch (astNode->variantT())
        {
          case V_SgProject:
             {
               SgProject* project = isSgProject(astNode);
               ASSERT_not_null(project);

           //! Loop through all the files in the project and call the mainTransform function for each file
               int i = 0;
               for (i=0; i < project->numberOfFiles(); i++)
                  {
                    SgFile* file = &(project->get_file(i));
                    getTransformationOptions ( file, generatedList, identifingTypeName );
                  }
               break;
             }

       // case V_SgFile:
          case V_SgSourceFile:
             {
               SgSourceFile* file = isSgSourceFile(astNode);
               ASSERT_not_null(file);
               SgGlobal* globalScope = file->get_globalScope();
               ASSERT_not_null(globalScope);

               getTransformationOptions ( globalScope, generatedList, identifingTypeName );
               break;
             }

       // Global Scope
          case V_SgGlobal:
             {
               SgGlobal* globalScope = isSgGlobal(astNode);
               ASSERT_not_null(globalScope);
            // DQ (5/21/2013): We now make this an error, but I think this code is not used any more.
               printf ("ERROR: access to symbol table is restricted from SgGlobal \n");
               ROSE_ABORT();
               break;
             }

          case V_SgSymbolTable:
             {
            // List the variable in each scope
               SgSymbolTable* symbolTable = isSgSymbolTable(astNode);
               ASSERT_not_null(symbolTable);

               if (symbolTable->get_table() != nullptr)
                  {
                    SgSymbolTable::hash_iterator i = symbolTable->get_table()->begin();

                    while (i != symbolTable->get_table()->end())
                       {
                         ASSERT_not_null(isSgSymbol( (*i).second ));
                         SgSymbol* symbol = isSgSymbol((*i).second);
                         ASSERT_not_null(symbol);
                         SgType* type = symbol->get_type();
                         ASSERT_not_null(type);

                         SgNamedType* namedType = isSgNamedType(type);
                         string typeName;
                         if (namedType != nullptr)
                            {
                              SgName n = namedType->get_name();
                              typeName = namedType->get_name().str();
                              ASSERT_not_null(identifingTypeName.c_str());
                              if ( typeName == identifingTypeName )
                                 {
                                // Now look at the parameter list to the constructor and save the
                                // values into the list.
                                   SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);

                                   if (variableSymbol != nullptr)
                                      {
                                        SgInitializedName* initializedNameDeclaration = variableSymbol->get_declaration();
                                        ASSERT_not_null(initializedNameDeclaration);

                                        SgDeclarationStatement* declarationStatement = initializedNameDeclaration->get_declaration();
                                        ASSERT_not_null(declarationStatement);

                                        SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declarationStatement);
                                        ASSERT_not_null(variableDeclaration);

                                        getTransformationOptionsFromVariableDeclarationConstructorArguments(variableDeclaration,generatedList);
                                      }
                                 }
                            }
                           else
                            {
                              typeName = (char *)type->sage_class_name();
                            }
                         i++;
                       }
                  }
               break;
             }

          case V_SgBasicBlock:
             {
            // List the variable in each scope
               SgBasicBlock* basicBlock = isSgBasicBlock(astNode);
               ASSERT_not_null(basicBlock);
            // DQ (5/21/2013): We now make this an error, but I think this code is not used any more.
               printf ("ERROR: access to symbol table is restricted from SgBasicBlock \n");
               ROSE_ABORT();
            // Next go (fall through this case) to the default case so that we traverse the parent
            // of the SgBasicBlock.
            // break;
             }

          default:
            // Most cases will be the default (this is by design)
            // Need to recursively backtrack through the parents until we reach the SgGlobal (global scope)
               SgStatement* statement = isSgStatement(astNode);
               if (statement != nullptr)
                  {
                    SgStatement* parentStatement = isSgStatement(statement->get_parent());
                    ASSERT_not_null(parentStatement);

                 // Call this function recursively (directly rather than through the query mechanism)
                    getTransformationOptions ( parentStatement, generatedList, identifingTypeName );
                  }
               break;
        }
   }

void
TransformationSupport::getTransformationOptionsFromVariableDeclarationConstructorArguments (
   SgVariableDeclaration* variableDeclaration,
   list<int> & returnEnumValueList )
   {
     ASSERT_not_null(variableDeclaration);
     SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();

     for (SgInitializedNamePtrList::iterator i = variableList.begin(); i != variableList.end(); i++)
        {
               ASSERT_not_null((*i));
               SgInitializer *initializerOther = (*i)->get_initializer();
               if (initializerOther != nullptr)
                  {
                    ASSERT_not_null(initializerOther);
                    SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(initializerOther);
                    ASSERT_not_null(constructorInitializer);

                    SgExprListExp* argumentList = constructorInitializer->get_args();
                    ASSERT_not_null(argumentList);

                    SgExpressionPtrList & expressionPtrList = argumentList->get_expressions();
                    SgExpressionPtrList::iterator i = expressionPtrList.begin();

                    while (i != expressionPtrList.end()) 
                       {
                         SgEnumVal* enumVal = isSgEnumVal(*i);
                         ASSERT_not_null(enumVal);

                         int enumValue = enumVal->get_value();
                         SgName enumName = enumVal->get_name();

                      // Put the value at the start of the list so that the list can be processed in
                      // consecutive order to establish options for consecutive scopes (root to
                      // child scope). Order is not important if we are only ORing the operands!
                         returnEnumValueList.push_front (enumValue);

                         i++;
                       }
                  }
        }
   }

// ***********************************************
// DQ (9/26/2003) Function that might be moved to SgNode (eventually)
// This is a sequence of functions which return the root nodes of different parts 
// of the AST,  we could imagine additional function like these which returned the 
// declaration appearing in global scope or more specifially the function scope
// (if the input node was in the subtree of a function) or the class declaration 
// (if the input node was in the subtree of a class declaration), etc.
// ***********************************************
SgProject*
TransformationSupport::getProject( const SgNode* astNode )
   {
     ASSERT_not_null(astNode);

     const SgNode* parentNode = astNode;
     while (parentNode->get_parent() != nullptr)
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgProject(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {
          if (astNode == nullptr)
             {
               printf ("Warning: could not trace back to SgProject node. \n");
             }
            else
             {
            // DQ (7/30/2010): This can be allowed for the expression in a SgArrayType!
               printf ("Warning: could not trace back to SgProject node from %s \n",astNode->class_name().c_str());
             }
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
               printf ("Error: can't locate an associated SgProject from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return nullptr;
             }
        }

  // Make sure we have a SgProject node
     const SgProject* project = isSgProject(parentNode);
     ASSERT_not_null(project);

     return const_cast<SgProject*>(project);
   }

SgDirectory*
TransformationSupport::getDirectory( const SgNode* astNode )
   {
     ASSERT_not_null(astNode);

     const SgNode* parentNode = astNode;
     while ( (isSgDirectory(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (8/2/2005): Modified this so that we can return NULL so that AST framents 
  // not associated with a primary AST can be used with this function!
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgDirectory(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {
          // printf ("Error: could not trace back to SgDirecoty node \n");
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
            // DQ (9/2/2014): Only output this message if this is not in a SgArrayType.
               if (isSgArrayType(parentNode) == nullptr)
                 {
                // DQ (3/6/2017): Converted to use message logging.
                   mprintf ("Warning: can't locate an associated SgFile from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
                 }
               return nullptr;
             }
        }

  // Make sure we have a SgFile node
     const SgDirectory* directory = isSgDirectory(parentNode);

     return const_cast<SgDirectory*>(directory);
   }

SgFile*
TransformationSupport::getFile( const SgNode* astNode )
   {
     ASSERT_not_null(astNode);

     const SgNode* parentNode = astNode;
     while ( (isSgFile(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (8/2/2005): Modified this so that we can return NULL so that AST framents 
  // not associated with a primary AST can be used with this function!
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgFile(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {
          // printf ("Error: could not trace back to SgFile node \n");
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
               if (isSgArrayType(parentNode) == nullptr)
                 {
                   mprintf ("Warning: can't locate an associated SgFile from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
                 }
               return nullptr;
             }
        }

  // Make sure we have a SgFile node
     const SgFile* file = isSgFile(parentNode);
     return const_cast<SgFile*>(file);
   }

SgSourceFile*
TransformationSupport::getSourceFile( const SgNode* astNode )
   {
     ASSERT_not_null(astNode);

     const SgNode* parentNode = astNode;
     while ( (isSgSourceFile(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (8/2/2005): Modified this so that we can return NULL so that AST framents 
  // not associated with a primary AST can be used with this function!
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgSourceFile(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {
          // printf ("Error: could not trace back to SgSourceFile node \n");
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
            // DQ (3/4/2009): This test will remove most of the instances of this warning (ignoring types).
               if (isSgType(parentNode) == nullptr)
                  {
                    printf ("Warning: can't locate an associated SgSourceFile from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
                  }
               return nullptr;
             }
        }

  // Make sure we have a SgSourceFile node
     const SgSourceFile* file = isSgSourceFile(parentNode);
     return const_cast<SgSourceFile*>(file);
   }

#ifdef ROSE_ENABLE_BINARY_ANALYSIS
SgBinaryComposite*
TransformationSupport::getBinaryFile( const SgNode* astNode )
   {
     ASSERT_not_null(astNode);

     const SgNode* parentNode = astNode;
     while ( (isSgBinaryComposite(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (8/2/2005): Modified this so that we can return NULL so that AST framents 
  // not associated with a primary AST can be used with this function!
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgBinaryComposite(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {
          // printf ("Error: could not trace back to SgBinaryComposite node \n");
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
               printf ("Error: can't locate an associated SgBinaryComposite from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return nullptr;
             }
        }


  // Make sure we have a SgBinaryComposite node
     const SgBinaryComposite* file = isSgBinaryComposite(parentNode);
     return const_cast<SgBinaryComposite*>(file);
   }
#endif

SgGlobal*
TransformationSupport::getGlobalScope( const SgNode* astNode )
   {
     ASSERT_not_null(astNode);
     const SgNode* parentNode = astNode;

     while ( (isSgGlobal(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }
     ASSERT_not_null(parentNode);

  // DQ (7/24/2010): Handle the case of an expression in an array type.
     if (isSgArrayType(parentNode) != nullptr)
        {
          return nullptr;
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgGlobal(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {  //It is possible to find no SgGlobal during transformation, changed to warning.
          if (astNode == nullptr)
             {
               printf ("Warning: could not trace back to SgGlobal node\n ");
             }
          return nullptr;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
               return nullptr;
             }
        }

  // Make sure we have a SgGlobal node
     const SgGlobal* globalScope = isSgGlobal(parentNode);
     ASSERT_not_null(globalScope);

     return const_cast<SgGlobal*>(globalScope);
   }

SgStatement*
TransformationSupport::getStatement( const SgNode* astNode )
   {
     ASSERT_not_null(astNode);
     const SgNode* parentNode = astNode;

  // DQ (6/27/2007): These IR nodes are not contained in any statement
     if (isSgProject(astNode) != nullptr || isSgFile(astNode) != nullptr)
          return nullptr;

  // DQ (7/24/2010): Handle the case of an expression in an array type.
     if (parentNode->get_parent() != nullptr && isSgArrayType(parentNode->get_parent()) != nullptr)
        {
          return nullptr;
        }

     while ( (isSgStatement(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }

     ASSERT_not_null(parentNode);

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgStatement(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {
          if (astNode == nullptr)
             {
               printf ("Error: could not trace back to SgStatement node \n");
             }
          return nullptr;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
            // Test for SgArrayType since a value if often hidden there and it is not possible to traverse 
            // through a SgType along parent IR nodes.
               if ( dynamic_cast<const SgArrayType*>(parentNode) != nullptr )
                  {
                    return nullptr;
                  }

            // DQ (11/10/2007): Note that for an AST fragment (e.g. expression) not connected to the AST, this function will return NULL.
#if PRINT_DEVELOPER_WARNINGS
               printf ("Warning: can't locate an associated SgStatement from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
#endif
               return nullptr;
             }
        }

  // Make sure we have a SgStatement node
     const SgStatement* statement = isSgStatement(parentNode);
     ASSERT_not_null(statement);

     return const_cast<SgStatement*>(statement);
   }


SgType*
TransformationSupport::getAssociatedType( const SgNode* astNode )
   {
  // DQ (8/19/2014): Iterate back through the parents and scopes to find the SgType that the current node is embedded into.
     ASSERT_not_null(astNode);

     const SgNode* parentNode = astNode;

  // DQ (6/27/2007): These IR nodes are not contained in any statement
     if (isSgProject(astNode) != nullptr || isSgFile(astNode) != nullptr)
          return nullptr;

  // DQ (7/24/2010): Handle the case of an expression in an array type.
     SgArrayType* arrayType = isSgArrayType(parentNode->get_parent());
     if (parentNode->get_parent() != nullptr && arrayType != nullptr)
        {
          printf ("TransformationSupport::getAssociatedType(): Case of expression in SgArrayType: arrayType = %p \n",arrayType);
          return arrayType;
        }

     while ( (isSgStatement(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }
     ASSERT_not_null(parentNode);

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgStatement(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {
          if (astNode == nullptr)
             {
               printf ("Error: could not trace back to SgStatement node \n");
             }
          return nullptr;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
            // Test for SgArrayType since a value if often hidden there and it is not possible to traverse 
            // through a SgType along parent IR nodes.
               if ( dynamic_cast<const SgArrayType*>(parentNode) != nullptr )
                  {
                    SgNode* tmp_node = const_cast<SgNode*>(parentNode);
                    SgArrayType* arrayType = isSgArrayType(tmp_node);
                    return arrayType;
                  }

            // DQ (11/10/2007): Note that for an AST fragment (e.g. expression) not connected to the AST, this function will return NULL.
#if PRINT_DEVELOPER_WARNINGS
               printf ("Warning: can't locate an associated SgStatement from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
#endif
               SgNode* other_node = const_cast<SgNode*>(parentNode);
               SgType* possibleOtherType = isSgType(other_node);
               return possibleOtherType;
             }
        }

  // Make sure we have a SgStatement node
     const SgStatement* statement = isSgStatement(parentNode);
     ASSERT_not_null(statement);

     return nullptr;
   }


SgFunctionDeclaration*
TransformationSupport::getFunctionDeclaration( const SgNode* astNode)
   {
     const SgNode* parentNode = astNode;
     while ( (isSgFunctionDeclaration(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgFunctionDeclaration(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {
          return nullptr;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
               printf ("Error: can't locate an associated SgFunctionDeclaration from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return nullptr;
             }
        }

  // Make sure we have a SgFunctionDeclaration node
     const SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(parentNode);
     ASSERT_not_null(functionDeclaration);

     return const_cast<SgFunctionDeclaration*>(functionDeclaration);
   }

SgFunctionDefinition*
TransformationSupport::getFunctionDefinition( const SgNode* astNode)
   {
     ASSERT_not_null(astNode);
     const SgNode* parentNode = astNode;

     while ( (isSgFunctionDefinition(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgFunctionDefinition(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {
          if (astNode == nullptr) {
            printf ("Error: could not trace back to SgFunctionDefinition node \n");
          }
          else {
            printf ("Warning: could not trace back to SgFunctionDefinition node from %s \n",astNode->class_name().c_str());
          }
          ROSE_ABORT();
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
               printf ("Error: can't locate an associated SgFunctionDefinition from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return nullptr;
             }
        }

  // Make sure we have a SgStatement node
     const SgFunctionDefinition* returnFunctionScope = isSgFunctionDefinition(parentNode);
     ASSERT_not_null(returnFunctionScope);

     return const_cast<SgFunctionDefinition*>(returnFunctionScope);
   }

SgClassDefinition*
TransformationSupport::getClassDefinition( const SgNode* astNode)
   {
     const SgNode* parentNode = astNode;
     while ( (isSgClassDefinition(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgClassDefinition(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {
       // DQ (12/11/2010): It is OK to return NULL from this function (used fortran_support.C
          return nullptr;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
               printf ("Error: can't locate an associated SgClassDefinition from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return nullptr;
             }
        }

  // Make sure we have a SgStatement node
     const SgClassDefinition* returnClassScope = isSgClassDefinition(parentNode);
     ASSERT_not_null(returnClassScope);

     return const_cast<SgClassDefinition*>(returnClassScope);
   }

SgModuleStatement*
TransformationSupport::getModuleStatement( const SgNode* astNode)
   {
  // DQ (11/24/2007): This function supports the unparsing of the PUBLIC, PRIVATE keywords (only permitted within modules)
  // The name of this function might change to getModuleDeclaration
     const SgNode* parentNode = astNode;
     while ( (isSgModuleStatement(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgModuleStatement(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr)
        {
                if (SgProject::get_verbose() > 0)
                {
                        printf("Warning: could not trace back to SgModuleStatement node \n");
                }
                return nullptr;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
               printf ("Error: can't locate an associated SgModuleStatement from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return nullptr;
             }
        }

  // Make sure we have a SgStatement node
     const SgModuleStatement* returnModuleDeclaration = isSgModuleStatement(parentNode);
     ASSERT_not_null(returnModuleDeclaration);

     return const_cast<SgModuleStatement*>(returnModuleDeclaration);
   }

SgDeclarationStatement*
TransformationSupport::getTemplateDeclaration( const SgNode* astNode)
   {
     const SgNode* parentNode = astNode;

     while ( (isSgTemplateDeclaration(parentNode) == nullptr)         && (isSgTemplateClassDeclaration(parentNode) == nullptr) &&
             (isSgTemplateFunctionDeclaration(parentNode) == nullptr) && (isSgTemplateMemberFunctionDeclaration(parentNode) == nullptr) &&
             (isSgTemplateVariableDeclaration(parentNode) == nullptr) && (parentNode->get_parent() != nullptr) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (7/25/2012): Updated to reflect new template design using different types or template IR nodes.
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgTemplateDeclaration(parentNode) == nullptr &&
          isSgTemplateClassDeclaration(parentNode) == nullptr &&
          isSgTemplateFunctionDeclaration(parentNode) == nullptr &&
          isSgTemplateMemberFunctionDeclaration(parentNode) == nullptr &&
          isSgTemplateVariableDeclaration(parentNode) == nullptr &&
          dynamic_cast<const SgType*>(parentNode) == nullptr &&
          dynamic_cast<const SgSymbol*>(parentNode) == nullptr )
        {
          if (astNode == nullptr)
             {
               printf ("Error: could not trace back to SgTemplateDeclaration node \n");
               ROSE_ABORT();
             }
            else
             {
               printf ("Warning: In TransformationSupport::getTemplateDeclaration(): could not trace back to template declaration node from %s \n",astNode->class_name().c_str());
             }

          return nullptr;
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != nullptr || dynamic_cast<const SgSymbol*>(parentNode) != nullptr )
             {
               printf ("Error: can't locate an associated SgTemplateDeclaration from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return nullptr;
             }
        }

  // Make sure we have a SgFunctionDeclaration node
     const SgDeclarationStatement* templateDeclaration = isSgDeclarationStatement(parentNode);
     ASSERT_not_null(templateDeclaration);

     return const_cast<SgDeclarationStatement*>(templateDeclaration);
   }
