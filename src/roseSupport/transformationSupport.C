
// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

#include "transformationSupport.h"
#include "nameQuery.h"

// DQ (12/31/2005): This is OK if not declared in a header file
using namespace std;



#if 0
void
TransformationSupport::insertDeclarations (
     SgProject & globalProject,
     SgNode* astNode, 
     const SynthesizedAttributeBaseClassType & gatheredData,
     string prefixString  )
   {
#if 1
     printf ("Exiting before call to gatheredData.insertGatheredStringData() \n");
     ROSE_ABORT();

  // gatheredData.insertGatheredStringData(astNode);
#else
     list<string> variableDeclarationStrings =
          gatheredData.variableDeclarationStrings.getSourceCodeStringList();

  // list<string> variableDeclarationStrings =
  //      gatheredData.transformationStrings.buildStringList(
  //           SourceCodeStringType::LocalScope,
  //           SourceCodeStringType::TopOfScope );

     string tempString = StringUtility::listToString(variableDeclarationStrings);

  // printf ("Case BASIC_BLOCK (before): tempString = \n%s\n",tempString.c_str());
     string variableDeclarations = StringUtility::removePseudoRedundentSubstrings(tempString);

  // printf ("Case BASIC_BLOCK: variableDeclarations = \n%s\n",variableDeclarations.c_str());

     string globalDeclarationCodeString =
          TransformationSupport::internalSupportingGlobalDeclarations( astNode, prefixString );
     string localDeclarationCodeString = "";

  // Need to mark declarations so that they can be extracted properly (but this should be done elsewhere)
     string sourceCodeString      = "int VARIABLE_DECLARATIONS_MARKER_START; \n" + 
                                    variableDeclarations +
                                    "\nint VARIABLE_DECLARATIONS_MARKER_END;\n";

#if 1
     printf ("Case BASIC_BLOCK: globalDeclarationCodeString = \n%s\n",globalDeclarationCodeString.c_str());
     printf ("Case BASIC_BLOCK: localDeclarationCodeString  = \n%s\n",localDeclarationCodeString.c_str());
     printf ("Case BASIC_BLOCK: sourceCodeString            = \n%s\n",sourceCodeString.c_str());
#endif

#if 0
     printf ("Exiting before call to basicBlock->insertSourceCode() \n");
     ROSE_ABORT();
#endif

  // Push the variable declarations for all the transformations onto the top of the local scope
     bool isADeclaration     = true;
     bool insertAtTopOfBlock = true;
     astNode->insertSourceCode (globalProject,
                                sourceCodeString,
                                localDeclarationCodeString,
                                globalDeclarationCodeString,
                                insertAtTopOfBlock,
                                isADeclaration);
#endif

   }
#endif

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
  // printf ("lastCharacter = %c \n",lastCharacter);

     char secondLastCharacter = name[stringLength-2];
  // printf ("secondLastCharacter = %c \n",secondLastCharacter);

     char thirdLastCharacter = name[stringLength-3];
  // printf ("thirdLastCharacter = %c \n",thirdLastCharacter);

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
  // printf ("lastCharacter = %c \n",lastCharacter);

     char secondLastCharacter = name[stringLength-2];
  // printf ("secondLastCharacter = %c \n",secondLastCharacter);

     char thirdLastCharacter = name[stringLength-3];
  // printf ("thirdLastCharacter = %c \n",thirdLastCharacter);

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
                 // ROSE_ABORT();
             }
        }

#if 0
  // Temporary test (e.g. this would not be correct for cos(A) function)
     ROSE_ASSERT (operatorCode != FUNCTION_CALL_OPERATOR_CODE);
#endif

     return operatorCode;
   }


string
TransformationSupport::buildOperatorString ( SgNode* astNode )
   {
  // This function defines the operator string used between 
  // the lhs and rhs for all binary operators. It supports
  // the assembly function below.

     string operatorString;
     ROSE_ASSERT (astNode != NULL);

  // printf ("In TransformationSupport::buildOperatorString(): astNode->sage_class_name() = %s \n",astNode->sage_class_name());

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

       // ??? Since a SgMemberFunctionRefExp is derived from a SgFunctionCallExp we need only handle one case
       // case V_SgMemberFunctionRefExp:
          case V_SgFunctionCallExp:   // SgFunctionCallExp
             {
               SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
               ROSE_ASSERT (functionCallExp != NULL);

               SgExpression* functionExpression = functionCallExp->get_function();
               ROSE_ASSERT (functionExpression != NULL);

               operatorString = buildOperatorString (functionExpression);
               break;
             }
#if 1
          case V_SgMemberFunctionRefExp:
             {
               SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(astNode);
               ROSE_ASSERT (memberFunctionRefExp != NULL);

               SgMemberFunctionSymbol* memberFunctionSymbol = memberFunctionRefExp->get_symbol();
               ROSE_ASSERT (memberFunctionSymbol != NULL);

               operatorString = buildOperatorString (memberFunctionSymbol);
               break;
             }
#endif

          case V_SgMemberFunctionSymbol:
	    {
	      // AS (9/24/03) Was not implemented before.

	       SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(astNode);
               ROSE_ASSERT (memberFunctionSymbol != NULL);

               SgName name = memberFunctionSymbol->get_name();
            // printf ("name = %s \n",name.str());
            // string tempName = ROSE::stringDuplicate(name.str());
               string tempName = name.str();

            // int stringLength = strlen(tempName);
               int stringLength = tempName.length();
               ROSE_ASSERT (stringLength >= 2);

               bool isAnOperator = false;
            // if ( (stringLength >= 8) && (!strncmp (tempName,"operator",8)) )
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
               ROSE_ASSERT (functionSymbol != NULL);

               SgName name = functionSymbol->get_name();
            // printf ("name = %s \n",name.str());
            // string tempName = ROSE::stringDuplicate(name.str());
               string tempName = name.str();

            // int stringLength = strlen(tempName);
               int stringLength = tempName.length();
               ROSE_ASSERT (stringLength >= 2);

               bool isAnOperator = false;
            // if ( (stringLength >= 8) && (!strncmp (tempName,"operator",8)) )
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
               ROSE_ASSERT (dotExp != NULL);
               SgExpression* rhs = dotExp->get_rhs_operand();
               ROSE_ASSERT (rhs != NULL);

               operatorString = buildOperatorString (rhs);
               break;
             }
          case V_SgFunctionRefExp: // SgFunctionRefExp
             {
               SgFunctionRefExp* functionReferenceExp = isSgFunctionRefExp(astNode);
               ROSE_ASSERT (functionReferenceExp != NULL);
               SgFunctionSymbol* functionSymbol = functionReferenceExp->get_symbol();
               ROSE_ASSERT (functionSymbol != NULL);

               operatorString = buildOperatorString (functionSymbol);
               break;
             }

          default:
               printf ("default reached in buildOperatorString() \n");
               ROSE_ABORT();
        }

  // printf ("operatorString = %s \n",operatorString);
  // ROSE_ABORT();

     return operatorString;
   }


string
TransformationSupport::getFunctionName ( SgFunctionCallExp* functionCallExp )
   {
  // SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
     ROSE_ASSERT (functionCallExp != NULL);

     SgExpression* expression = functionCallExp->get_function();
     ROSE_ASSERT (expression != NULL);

     string returnNameString;

     SgDotExp* dotExp = isSgDotExp(expression);
     if (dotExp != NULL)
        {
          ROSE_ASSERT (dotExp != NULL);

          SgExpression* rhsOperand = dotExp->get_rhs_operand();
          ROSE_ASSERT (rhsOperand != NULL);

          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsOperand);
          ROSE_ASSERT (memberFunctionRefExp != NULL);

          SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(memberFunctionRefExp->get_symbol());
          ROSE_ASSERT (memberFunctionSymbol != NULL);

       // There will be a lot of different possible overloaded operators called
       // and "operator()" is just one of them (that we are debugging presently)

       // returnNameString = ROSE::stringDuplicate(memberFunctionSymbol->get_name().str());
          returnNameString = memberFunctionSymbol->get_name().str();
	  ROSE_ASSERT (returnNameString.length() > 0);
        }

     SgFunctionRefExp* functionReferenceExp = isSgFunctionRefExp(expression);
     if (functionReferenceExp != NULL)
       {
	 SgFunctionSymbol* functionSymbol = isSgFunctionSymbol(functionReferenceExp->get_symbol());
	 ROSE_ASSERT (functionSymbol != NULL);
	 
	 // returnNameString = ROSE::stringDuplicate(functionSymbol->get_name().str());
	 returnNameString = functionSymbol->get_name().str();
	 
	 // ROSE_ASSERT (returnNameString != NULL);
	 // printf ("SgFunctionRefExp case: returnNameString = %s \n",returnNameString);
	 
	 // printf ("Not implemented case in getFunctionName \n");
	 // ROSE_ABORT();
	 ROSE_ASSERT (returnNameString.length() > 0);
       }

     SgArrowExp* arrowExp = isSgArrowExp(expression);
     if ( arrowExp != NULL)
       {
	 ROSE_ASSERT (arrowExp != NULL);
	 
	 SgExpression* rhsOperand = arrowExp->get_rhs_operand();
	 ROSE_ASSERT (rhsOperand != NULL);
	 
	 SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsOperand);
	 ROSE_ASSERT (memberFunctionRefExp != NULL);
	 
	 SgMemberFunctionSymbol* memberFunctionSymbol = isSgMemberFunctionSymbol(memberFunctionRefExp->get_symbol());
	 ROSE_ASSERT (memberFunctionSymbol != NULL);
	 
	 // There will be a lot of different possible overloaded operators called
	 // and "operator()" is just one of them (that we are debugging presently)
	 
	 // returnNameString = ROSE::stringDuplicate(memberFunctionSymbol->get_name().str());
	 returnNameString = memberFunctionSymbol->get_name().str();
	 ROSE_ASSERT (returnNameString.length() > 0);
       }
     
     ROSE_ASSERT (returnNameString.length() > 0);
     // printf ("returnNameString = %s \n",returnNameString.c_str());
     
     return returnNameString;
   }


// DQ (3/18/2005): Improved version from Brian White (merged bugfix from Tom Epperly as well
string
TransformationSupport::getTypeName ( SgType* type )
   {
     string typeName;

  // printf ("In TransformationSupport::getTypeName(): type->sage_class_name() = %s \n",type->sage_class_name());

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
#if 0
          case V_SgEnumType:
            // DQ (3/2/2005): This needs to be fixed, but Tom is going to send me the fix since I'm working on Kull presently.
               typeName = "enum";
               break;
#endif
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
               ROSE_ASSERT ( isSgReferenceType(type)->get_base_type() != NULL );
               typeName = getTypeName(isSgReferenceType(type)->get_base_type()) + "&";
               break;
             }
          case V_SgPointerType:
             {
               ROSE_ASSERT ( isSgPointerType(type)->get_base_type() != NULL );
               typeName = getTypeName(isSgPointerType(type)->get_base_type()) + "*";
               break;
             }
          case V_SgModifierType:
             {
               ROSE_ASSERT ( isSgModifierType(type)->get_base_type() != NULL );
               SgModifierType *modifier = isSgModifierType(type);
               typeName = modifier->unparseToString();
            // typeName = getTypeName(modifier->get_base_type());
               break;
             }
          case V_SgEnumType:
          case V_SgNamedType:
             {
               SgNamedType* sageNamedType = isSgNamedType(type);
               ROSE_ASSERT( sageNamedType != NULL );
               typeName = sageNamedType->get_name().str();
               break;
             }
          case V_SgClassType:
             {
               SgClassType* sageClassType = isSgClassType(type);
               ROSE_ASSERT( sageClassType != NULL );
               typeName = sageClassType->get_name().str();
               break;
             }
          case V_SgTypedefType:
             {
               SgTypedefType* sageTypedefType = isSgTypedefType(type);
               ROSE_ASSERT( sageTypedefType != NULL );
               typeName = sageTypedefType->get_name().str();
               break;
             }
          case V_SgPointerMemberType:
             {
               SgPointerMemberType* pointerMemberType = isSgPointerMemberType(type);
               ROSE_ASSERT (pointerMemberType != NULL);
               SgClassType* classType = isSgClassType(pointerMemberType->get_class_type()->stripTypedefsAndModifiers());
               ROSE_ASSERT (classType != NULL);
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
               ROSE_ASSERT (classDeclaration != NULL);
               typeName = classDeclaration->get_name().str();
               break;
             }
          case V_SgArrayType: 
             {
               ROSE_ASSERT ( isSgArrayType(type)->get_base_type() != NULL );
               typeName = getTypeName(isSgArrayType(type)->get_base_type());
               break;
             }
          case V_SgFunctionType:
             {
               SgFunctionType* functionType = isSgFunctionType(type);
               ROSE_ASSERT(functionType != NULL);
               typeName = functionType->get_mangled_type().str();
               break;
             }
          case V_SgMemberFunctionType:
             {
               SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(type);
               ROSE_ASSERT (memberFunctionType != NULL);
               SgClassType* classType = isSgClassType(memberFunctionType->get_class_type()->stripTypedefsAndModifiers());
               ROSE_ASSERT (classType != NULL);
               SgClassDeclaration* classDeclaration = isSgClassDeclaration(classType->get_declaration());
               ROSE_ASSERT (classDeclaration != NULL);
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

  // Fix for purify problem report
  // typeName = ROSE::stringDuplicate(typeName);

     return typeName;
   }

#if 0
string
TransformationSupport::getTypeName ( SgType* type )
   {
     string typeName;

  // printf ("In TransformationSupport::getTypeName(): type->sage_class_name() = %s \n",type->sage_class_name());

     switch (type->variantT())
        {
          case V_SgComplex:
               typeName = "complex";
               break;
          case V_SgTypeBool:
               typeName = "bool";
               break;
#if 0
          case V_SgEnumType:
            // DQ (3/2/2005): This needs to be fixed, but Tom is going to send me the fix since I'm working on Kull presently.
               printf ("getTypeName() of an enum should return the name of the enum, not the string \"enum\" \n");
               typeName = "enum";
               break;
#endif
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
               ROSE_ASSERT ( isSgReferenceType(type)->get_base_type() != NULL );
               typeName = getTypeName(isSgReferenceType(type)->get_base_type());
               break;
             }
          case V_SgPointerType:
             {
               ROSE_ASSERT ( isSgPointerType(type)->get_base_type() != NULL );
               typeName = getTypeName(isSgPointerType(type)->get_base_type());
               break;
             }
          case V_SgModifierType:
             {
               ROSE_ASSERT ( isSgModifierType(type)->get_base_type() != NULL );
               typeName = getTypeName(isSgModifierType(type)->get_base_type());
               break;
             }
       // DQ (3/2/2005): Merged cases of SgEnumType and SgNamedType (Tom Epperly's fix)
          case V_SgEnumType:
          case V_SgNamedType:
             {
               SgNamedType* sageNamedType = isSgNamedType(type);
               ROSE_ASSERT( sageNamedType != NULL );
               typeName = sageNamedType->get_name().str();
               break;
             }
          case V_SgClassType:
             {
               SgClassType* sageClassType = isSgClassType(type);
               ROSE_ASSERT( sageClassType != NULL );
               if( sageClassType->get_name().str() == NULL)
               typeName = "";
               else
               typeName = sageClassType->get_name().str();
               break;
             }
          case V_SgTypedefType:
             {
               SgTypedefType* sageTypedefType = isSgTypedefType(type);
               ROSE_ASSERT( sageTypedefType != NULL );
               typeName = sageTypedefType->get_name().str();
               break;
             }
          case V_SgPointerMemberType:
             {
               SgPointerMemberType* pointerMemberType = isSgPointerMemberType(type);
               ROSE_ASSERT (pointerMemberType != NULL);
               SgClassDefinition* classDefinition = pointerMemberType->get_class_of();
               ROSE_ASSERT (classDefinition != NULL);
               SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
               ROSE_ASSERT (classDeclaration != NULL);
               typeName = classDeclaration->get_name().str();
               break;
             }
          case V_SgArrayType: 
             {
               ROSE_ASSERT ( isSgArrayType(type)->get_base_type() != NULL );
               typeName = getTypeName(isSgArrayType(type)->get_base_type());
               break;
             }
          case V_SgFunctionType:
             {
               SgFunctionType* functionType = isSgFunctionType(type);
               ROSE_ASSERT(functionType != NULL);
               typeName = functionType->get_mangled_type().str();
               break;
             }
          case V_SgMemberFunctionType:
             {
               SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(type);
               ROSE_ASSERT (memberFunctionType != NULL);
               SgClassDefinition* classDefinition = memberFunctionType->get_struct_name();
               ROSE_ASSERT (classDefinition != NULL);
               SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
               ROSE_ASSERT (classDeclaration != NULL);
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

  // Fix for purify problem report
  // typeName = ROSE::stringDuplicate(typeName);

     ROSE_ASSERT(typeName.c_str() != NULL);
  // return typeName;
     return ROSE::stringDuplicate(typeName.c_str());
   }
#endif

string
TransformationSupport::getFunctionTypeName ( SgFunctionCallExp* functionCallExpression )
   {
     string associatedClassName;

  // SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(astNode);
     ROSE_ASSERT (functionCallExpression != NULL);

  // string operatorName = TransformationSupport::getFunctionName ( functionCallExpression );
  // ROSE_ASSERT (operatorName.c_str() != NULL);

     SgExpression* expression = functionCallExpression->get_function();
     ROSE_ASSERT (expression != NULL);

     string functionTypeName;
     SgDotExp* dotExpression = isSgDotExp(expression);
     SgArrowExp* arrowExpression = isSgArrowExp(expression);
     if (dotExpression != NULL)
        {
       // Get the class name associated with the member function
          SgExpression* rhsExpression = dotExpression->get_rhs_operand();
          ROSE_ASSERT (rhsExpression != NULL);
          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsExpression);
          ROSE_ASSERT (memberFunctionRefExp != NULL);

       // DQ (1/14/2006): Call get_type() instead of get_function_type() (part of work
       // to remove the explicit storage of the type within expressions were possible).
       // ROSE_ASSERT (memberFunctionRefExp->get_function_type() != NULL);
          ROSE_ASSERT (memberFunctionRefExp->get_type() != NULL);

       // DQ (1/14/2006): Call get_type() instead of get_function_type() (part of work
       // to remove the explicit storage of the type within expressions were possible).
       // SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(memberFunctionRefExp->get_function_type());
          SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(memberFunctionRefExp->get_type());

          ROSE_ASSERT (memberFunctionType != NULL);
       // functionTypeName = ROSE::stringDuplicate(TransformationSupport::getTypeName ( memberFunctionType ));
          functionTypeName = TransformationSupport::getTypeName ( memberFunctionType );
        }
     else if ( arrowExpression != NULL )
        {
       // Get the class name associated with the member function
          SgExpression* rhsExpression = arrowExpression->get_rhs_operand();
          ROSE_ASSERT (rhsExpression != NULL);
          SgMemberFunctionRefExp* memberFunctionRefExp = isSgMemberFunctionRefExp(rhsExpression);
          ROSE_ASSERT (memberFunctionRefExp != NULL);

       // DQ (1/14/2006): Call get_type() instead of get_function_type() (part of work
       // to remove the explicit storage of the type within expressions were possible).
       // ROSE_ASSERT (memberFunctionRefExp->get_function_type() != NULL);
       // SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(memberFunctionRefExp->get_function_type());
          ROSE_ASSERT (memberFunctionRefExp->get_type() != NULL);
          SgMemberFunctionType* memberFunctionType = isSgMemberFunctionType(memberFunctionRefExp->get_type());

          ROSE_ASSERT (memberFunctionType != NULL);
       // functionTypeName = ROSE::stringDuplicate(TransformationSupport::getTypeName ( memberFunctionType ));
          functionTypeName = TransformationSupport::getTypeName ( memberFunctionType );
       }
       else
        {
       // Get the class name associated with the friend function
          SgType* functionType    = functionCallExpression->get_type();
          ROSE_ASSERT (functionType != NULL);
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

#if 0
     char* returnString = ROSE::stringDuplicate(s);
     char* roseMacroMarkerString = "ROSE-TRANSFORMATION-MACRO:";
     returnString = ROSE::stringConcatinate (roseMacroMarkerString,returnString);

  // Wrap the string into quotes for use as a C++ string expression
     returnString = ROSE::stringConcatinate ("\"",returnString);
     returnString = ROSE::stringConcatinate (returnString,"\";");
     ROSE_ASSERT (returnString != NULL);
#else
     string returnString = s;
     string roseMacroMarkerString = "ROSE-TRANSFORMATION-MACRO:";
     returnString = roseMacroMarkerString + returnString;

  // Wrap the string into quotes for use as a C++ string expression
     returnString = "\"" + returnString;
     returnString = returnString + "\";";
#endif

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

  // printf ("######################### START OF TRANSFORMATION OPTION QUERY ######################## \n");

     ROSE_ASSERT (astNode != NULL);
     ROSE_ASSERT (identifingTypeName.c_str() != NULL);

#if 0
     printf ("In getTransformationOptions(): astNode->sage_class_name() = %s generatedList.size() = %d \n",
          astNode->sage_class_name(),generatedList.size());
#endif

     switch (astNode->variantT())
        {
          case V_SgProject:
             {
               SgProject* project = isSgProject(astNode);
               ROSE_ASSERT (project != NULL);

           //! Loop through all the files in the project and call the mainTransform function for each file
               int i = 0;
               for (i=0; i < project->numberOfFiles(); i++)
                  {
                    SgFile* file = &(project->get_file(i));

                 // printf ("Calling Query::traverse(SgFile,QueryFunctionType,QueryAssemblyFunctionType) \n");
                    getTransformationOptions ( file, generatedList, identifingTypeName );
                  }
               break;
             }

       // case V_SgFile:
          case V_SgSourceFile:
             {
               SgSourceFile* file = isSgSourceFile(astNode);
               ROSE_ASSERT (file != NULL);
            // SgGlobal* globalScope = &(file->root());
               SgGlobal* globalScope = file->get_globalScope();
               ROSE_ASSERT (globalScope != NULL);

            // DQ (9/5/2008): This test is pointless!
            // ROSE_ASSERT (isSgGlobal(globalScope) != NULL);

               getTransformationOptions ( globalScope, generatedList, identifingTypeName );
               break;
             }

       // Global Scope
          case V_SgGlobal:
             {
               SgGlobal* globalScope = isSgGlobal(astNode);
               ROSE_ASSERT (globalScope != NULL);

               SgSymbolTable* symbolTable = globalScope->get_symbol_table();
               ROSE_ASSERT (symbolTable != NULL);
               getTransformationOptions ( symbolTable, generatedList, identifingTypeName );

            // printf ("Processed global scope, exiting .. \n");
            // ROSE_ABORT();
               break;
             }

          case V_SgSymbolTable:
             {
            // List the variable in each scope
            // printf ("List all the variables in this symbol table! \n");
               SgSymbolTable* symbolTable = isSgSymbolTable(astNode);
               ROSE_ASSERT (symbolTable != NULL);

               bool foundTransformationOptimizationSpecifier = false;

            // printf ("Now print out the information in the symbol table for this scope: \n");
            // symbolTable->print();

#if 0
            // I don't know when a SymbolTable is given a name!
               printf ("SymbolTable has a name = %s \n",
                    (symbolTable->get_no_name()) ? "NO: it has no name" : "YES: it does have a name");
               if (!symbolTable->get_no_name())
                    printf ("SymbolTable name = %s \n",symbolTable->get_name().str());
                 else
                    ROSE_ASSERT (symbolTable->get_name().str() == NULL);
#endif

               if (symbolTable->get_table() != NULL)
                  {
                 // AJ (10/21/2004): Adjusted implementation to use new STL hash map interface
                 // SgSymbolHashBase::iterator i = symbolTable->get_table()->begin();
                    SgSymbolTable::hash_iterator i = symbolTable->get_table()->begin();

                    int counter = 0;
                    while (i != symbolTable->get_table()->end())
                       {
                         ROSE_ASSERT ( isSgSymbol( (*i).second ) != NULL );

                      // printf ("Initial info: number: %d pair.first (SgName) = %s pair.second (SgSymbol) sage_class_name() = %s \n",
                      //      counter,(*i).first.str(),(*i).second->sage_class_name());

                         SgSymbol* symbol = isSgSymbol((*i).second);
                         ROSE_ASSERT ( symbol != NULL );
                         SgType* type = symbol->get_type();
                         ROSE_ASSERT ( type != NULL );

                         SgNamedType* namedType = isSgNamedType(type);
                         string typeName;
                         if (namedType != NULL)
                            {
                              SgName n = namedType->get_name();
                              typeName = namedType->get_name().str();
                           // char* nameString = namedType->get_name().str();
                           // printf ("Type is: (named type) = %s \n",nameString);
                              ROSE_ASSERT (identifingTypeName.c_str() != NULL);
                           // ROSE_ASSERT (typeName != NULL);
                           // if ( (typeName != NULL) && (ROSE::isSameName(typeName,identifingTypeName) == true) )
                              if ( typeName == identifingTypeName )
                                 {
                                // Now look at the parameter list to the constructor and save the
                                // values into the list.

                                // printf ("Now save the constructor arguments! \n");

                                   SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);

                                   if ( variableSymbol != NULL )
                                      {
                                        SgInitializedName* initializedNameDeclaration = variableSymbol->get_declaration();
                                        ROSE_ASSERT (initializedNameDeclaration != NULL);

                                        SgDeclarationStatement* declarationStatement = initializedNameDeclaration->get_declaration();
                                        ROSE_ASSERT (declarationStatement != NULL);

                                        SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declarationStatement);
                                        ROSE_ASSERT (variableDeclaration != NULL);

                                        getTransformationOptionsFromVariableDeclarationConstructorArguments(variableDeclaration,generatedList);

                                        foundTransformationOptimizationSpecifier = true;

                                     // printf ("Exiting after saving the constructor arguments! \n");
                                     // ROSE_ABORT();
                                      }
                                     else
                                      {
#if 0
                                        printf ("Not a SgVariableSymbol: symbol->sage_class_name() = %s \n",
                                             symbol->sage_class_name());
#endif
                                      }
                                 }
                                else
                                 {
#if 0
                                // I don't think this should ever be NULL (but it is sometimes)
                                   if (typeName != NULL)
                                        printf ("typeName == NULL \n");
#endif
                                 }
                            }
                           else
                            {
                              typeName = (char *)type->sage_class_name();
                            }
#if 0
                         printf ("Symbol number: %d pair.first (SgName) = %s pair.second (SgSymbol) sage_class_name() = %s type = %s \n",
                              counter,(*i).first.str(),(*i).second->sage_class_name(),
                              (typeName != NULL) ? typeName : "NULL TYPE NAME");
#endif
                         i++;
                         counter++;
                       }
                  }
                 else
                  {
                 // printf ("Pointer to symbol table is NULL \n");
                  }

            // printf ("foundTransformationOptimizationSpecifier = %s \n",foundTransformationOptimizationSpecifier ? "true" : "false");

            // SgSymbolTable objects don't have a parent node (specifically they lack a get_parent
            // member function in the interface)!
               break;
             }

          case V_SgBasicBlock:
             {
            // List the variable in each scope
            // printf ("List all the variables in this scope! \n");
               SgBasicBlock* basicBlock = isSgBasicBlock(astNode);
               ROSE_ASSERT (basicBlock != NULL);

               SgSymbolTable* symbolTable = basicBlock->get_symbol_table();
               ROSE_ASSERT (symbolTable != NULL);
               getTransformationOptions ( symbolTable, generatedList, identifingTypeName );

            // Next go (fall through this case) to the default case so that we traverse the parent
            // of the SgBasicBlock.
            // break;
             }

          default:
            // Most cases will be the default (this is by design)
            // printf ("default in switch found in globalQueryGetListOperandStringFunction() (sage_class_name = %s) \n",astNode->sage_class_name());
#if 1
            // Need to recursively backtrack through the parents until we reach the SgGlobal (global scope)
               SgStatement* statement = isSgStatement(astNode);
               if (statement != NULL)
                  {
                 // SgStatement* parentStatement = statement->get_parent();
                    SgStatement* parentStatement = isSgStatement(statement->get_parent());
                    ROSE_ASSERT (parentStatement != NULL);
                 // printf ("Parent is a %s \n",parentStatement->sage_class_name());

                 // Call this function recursively (directly rather than through the query mechanism)
                    getTransformationOptions ( parentStatement, generatedList, identifingTypeName );
                  }
                 else
                  {
                 // printf ("astNode is not a SgStatement! \n");
                  }
#else
               printf ("Skipping the traversal of the parent node in the AST! \n");
#endif

               break;
        }

#if 0
     printf ("In globalQueryGetOperandFunction(): astNode->sage_class_name() = %s size of generatedList = %d \n",
          astNode->sage_class_name(),generatedList.size());
#endif

  // printf ("######################### END OF TRANSFORMATION OPTION QUERY ######################## \n");
   }


#if 0

// Code moved to Babel Directory to simplify development temporarily

void
TransformationSupport::getTransformationOptions (
   SgNode* astNode,
   list<OptionDeclaration> & generatedList,
   string identifingTypeName )
   {
  // This function searches for variables of type ScopeBasedTransformationOptimization.  Variables
  // of type ScopeBasedTransformationOptimization are used to communicate optimizations from the
  // application to the preprocessor. If called from a project or file object it traverses down to
  // the global scope of the file and searches only the global scope, if called from and other
  // location within the AST it searches the current scope and then traverses the parent nodes to
  // find all enclosing scopes until in reaches the global scope.  At each scope it searches for
  // variables of type ScopeBasedTransformationOptimization.

  // printf ("######################### START OF TRANSFORMATION OPTION QUERY ######################## \n");

     ROSE_ASSERT (astNode != NULL);
     ROSE_ASSERT (identifingTypeName.c_str() != NULL);

#if 0
     printf ("In getTransformationOptions(): astNode->sage_class_name() = %s generatedList.size() = %d \n",
          astNode->sage_class_name(),generatedList.size());
#endif

     switch (astNode->variantT())
        {
          case V_SgProject:
             {
               SgProject* project = isSgProject(astNode);
               ROSE_ASSERT (project != NULL);

           //! Loop through all the files in the project and call the mainTransform function for each file
               int i = 0;
               for (i=0; i < project->numberOfFiles(); i++)
                  {
                    SgFile* file = &(project->get_file(i));

                 // printf ("Calling Query::traverse(SgFile,QueryFunctionType,QueryAssemblyFunctionType) \n");
                    getTransformationOptions ( file, generatedList, identifingTypeName );
                  }
               break;
             }

       // case V_SgFile:
          case V_SgSourceFile:
             {
               SgFile* file = isSgFile(astNode);
               ROSE_ASSERT (file != NULL);
               SgGlobal* globalScope = &(file->root());
               ROSE_ASSERT (globalScope != NULL);
               ROSE_ASSERT (isSgGlobal(globalScope) != NULL);
               getTransformationOptions ( globalScope, generatedList, identifingTypeName );
               break;
             }

       // Global Scope
          case V_SgGlobal:
             {
               SgGlobal* globalScope = isSgGlobal(astNode);
               ROSE_ASSERT (globalScope != NULL);

               SgSymbolTable* symbolTable = globalScope->get_symbol_table();
               ROSE_ASSERT (symbolTable != NULL);
               getTransformationOptions ( symbolTable, generatedList, identifingTypeName );

            // printf ("Processed global scope, exiting .. \n");
            // ROSE_ABORT();
               break;
             }

          case V_SgSymbolTable:
             {
            // List the variable in each scope
            // printf ("List all the variables in this symbol table! \n");
               SgSymbolTable* symbolTable = isSgSymbolTable(astNode);
               ROSE_ASSERT (symbolTable != NULL);

               bool foundTransformationOptimizationSpecifier = false;

            // printf ("Now print out the information in the symbol table for this scope: \n");
            // symbolTable->print();

#if 0
            // I don't know when a SymbolTable is given a name!
               printf ("SymbolTable has a name = %s \n",
                    (symbolTable->get_no_name()) ? "NO: it has no name" : "YES: it does have a name");
               if (!symbolTable->get_no_name())
                    printf ("SymbolTable name = %s \n",symbolTable->get_name().str());
                 else
                    ROSE_ASSERT (symbolTable->get_name().str() == NULL);
#endif

               if (symbolTable->get_table() != NULL)
                  {
                 // AJ (10/21/2004): Adjusted implementation to use new STL hash map interface
                 // SgSymbolHashBase::iterator i = symbolTable->get_table()->begin();
                    SgSymbolTable::hash_iterator i = symbolTable->get_table()->begin();

                    int counter = 0;
                    while (i != symbolTable->get_table()->end())
                       {
                         ROSE_ASSERT ( (*i).first.str() != NULL );
                         ROSE_ASSERT ( isSgSymbol( (*i).second ) != NULL );

                      // printf ("Initial info: number: %d pair.first (SgName) = %s pair.second (SgSymbol) sage_class_name() = %s \n",
                      //      counter,(*i).first.str(),(*i).second->sage_class_name());

                         SgSymbol* symbol = isSgSymbol((*i).second);
                         ROSE_ASSERT ( symbol != NULL );
                         SgType* type = symbol->get_type();
                         ROSE_ASSERT ( type != NULL );

                         SgNamedType* namedType = isSgNamedType(type);
                         string typeName;
                         if (namedType != NULL)
                            {
                              SgName n = namedType->get_name();
                              typeName = namedType->get_name().str();
                           // char* nameString = namedType->get_name().str();
                           // printf ("Type is: (named type) = %s \n",nameString);
                              ROSE_ASSERT (identifingTypeName.c_str() != NULL);
                           // ROSE_ASSERT (typeName != NULL);
                              printf ("In getTransformationOptions(): typeName = %s identifingTypeName = %s \n",typeName.c_str(),identifingTypeName.c_str());
                           // if ( (typeName != NULL) && ( typeName == identifingTypeName) )
                              if ( typeName == identifingTypeName )
                                 {
                                // Now look at the parameter list to the constructor and save the
                                // values into the list.

                                   printf ("Now save the constructor arguments! \n");

                                   SgVariableSymbol* variableSymbol = isSgVariableSymbol(symbol);

                                   if ( variableSymbol != NULL )
                                      {
                                        SgInitializedName* initializedNameDeclaration = variableSymbol->get_declaration();
                                        ROSE_ASSERT (initializedNameDeclaration != NULL);

                                        SgDeclarationStatement* declarationStatement = initializedNameDeclaration->get_declaration();
                                        ROSE_ASSERT (declarationStatement != NULL);

                                        SgVariableDeclaration* variableDeclaration = isSgVariableDeclaration(declarationStatement);
                                        ROSE_ASSERT (variableDeclaration != NULL);

                                        getTransformationOptionsFromVariableDeclarationConstructorArguments(variableDeclaration,generatedList);

                                        foundTransformationOptimizationSpecifier = true;

                                     // printf ("Exiting after saving the constructor arguments! \n");
                                     // ROSE_ABORT();
                                      }
                                     else
                                      {
#if 0
                                        printf ("Not a SgVariableSymbol: symbol->sage_class_name() = %s \n",
                                             symbol->sage_class_name());
#endif
                                      }
                                 }
                                else
                                 {
#if 0
                                // I don't think this should ever be NULL (but it is sometimes)
                                   if (typeName != NULL)
                                        printf ("typeName == NULL \n");
#endif
                                 }
                            }
                           else
                            {
                              typeName = (char *)type->sage_class_name();
                            }
#if 0
                         printf ("Symbol number: %d pair.first (SgName) = %s pair.second (SgSymbol) sage_class_name() = %s type = %s \n",
                              counter,(*i).first.str(),(*i).second->sage_class_name(),
                              (typeName != NULL) ? typeName : "NULL TYPE NAME");
#endif
                         i++;
                         counter++;
                       }
                  }
                 else
                  {
                 // printf ("Pointer to symbol table is NULL \n");
                  }

            // printf ("foundTransformationOptimizationSpecifier = %s \n",foundTransformationOptimizationSpecifier ? "true" : "false");

            // SgSymbolTable objects don't have a parent node (specifically they lack a get_parent
            // member function in the interface)!
               break;
             }

          case V_SgBasicBlock:
             {
            // List the variable in each scope
            // printf ("List all the variables in this scope! \n");
               SgBasicBlock* basicBlock = isSgBasicBlock(astNode);
               ROSE_ASSERT (basicBlock != NULL);

               SgSymbolTable* symbolTable = basicBlock->get_symbol_table();
               ROSE_ASSERT (symbolTable != NULL);
               getTransformationOptions ( symbolTable, generatedList, identifingTypeName );

            // Next go (fall through this case) to the default case so that we traverse the parent
            // of the SgBasicBlock.
            // break;
             }

          default:
            // Most cases will be the default (this is by design)
            // printf ("default in switch found in globalQueryGetListOperandStringFunction() (sage_class_name = %s) \n",astNode->sage_class_name());
#if 1
            // Need to recursively backtrack through the parents until we reach the SgGlobal (global scope)
               SgStatement* statement = isSgStatement(astNode);
               if (statement != NULL)
                  {
                    SgStatement* parentStatement = statement->get_parent();
                    ROSE_ASSERT (parentStatement != NULL);
                 // printf ("Parent is a %s \n",parentStatement->sage_class_name());

                 // Call this function recursively (directly rather than through the query mechanism)
                    getTransformationOptions ( parentStatement, generatedList, identifingTypeName );
                  }
                 else
                  {
                 // printf ("astNode is not a SgStatement! \n");
                  }
#else
               printf ("Skipping the traversal of the parent node in the AST! \n");
#endif

               break;
        }

#if 0
     printf ("In globalQueryGetOperandFunction(): astNode->sage_class_name() = %s size of generatedList = %d \n",
          astNode->sage_class_name(),generatedList.size());
#endif

  // printf ("######################### END OF TRANSFORMATION OPTION QUERY ######################## \n");
   }
#endif

void
TransformationSupport::getTransformationOptionsFromVariableDeclarationConstructorArguments (
   SgVariableDeclaration* variableDeclaration,
   list<int> & returnEnumValueList )
   {
     ROSE_ASSERT (variableDeclaration != NULL);
     SgInitializedNamePtrList & variableList = variableDeclaration->get_variables();

     for (SgInitializedNamePtrList::iterator i = variableList.begin(); i != variableList.end(); i++)
        {
       // We don't care about the name
       // SgName & name = (*i).get_name();

          ROSE_ASSERT((*i) != NULL);
          //if ((*i)->get_named_item() != NULL)
          //   {
          //     ROSE_ASSERT ((*i)->get_named_item() != NULL);
          //     SgInitializer *initializerOther = (*i)->get_named_item()->get_initializer();
               SgInitializer *initializerOther = (*i)->get_initializer();

            // This is not always a valid pointer
            // ROSE_ASSERT (initializerOther != NULL);

               if (initializerOther != NULL)
                  {
                 // There are other things we could ask of the initializer
                    ROSE_ASSERT (initializerOther != NULL);

                 // printf ("Now print out the initializerOther (a SgExpression*) \n");
                 // ROSE_ABORT();

                    SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(initializerOther);
                    ROSE_ASSERT (constructorInitializer != NULL);

                    SgExprListExp* argumentList = constructorInitializer->get_args();
                    ROSE_ASSERT (argumentList != NULL);

                    SgExpressionPtrList & expressionPtrList = argumentList->get_expressions();
                    SgExpressionPtrList::iterator i = expressionPtrList.begin();

                    int counter = 0;
	            while (i != expressionPtrList.end()) 
                       {
                      // printf ("Expression List Element #%d of %zu (total) \n",counter,expressionPtrList.size());

                         SgEnumVal* enumVal = isSgEnumVal(*i);
                         ROSE_ASSERT (enumVal != NULL);

                         int enumValue = enumVal->get_value();
                         SgName enumName = enumVal->get_name();
                      // printf ("Name = %s value = %d \n",enumName.str(),enumValue);
                      // printf ("Name = %s \n",enumName.str());

                      // string name ( ROSE::stringDuplicate( enumName.str() ) );
                      // char* name = ROSE::stringDuplicate( enumName.str() );

                      // Put the value at the start of the list so that the list can be processed in
                      // consecutive order to establish options for consecutive scopes (root to
                      // child scope). Order is not important if we are only ORing the operands!
                      // returnEnumValueList.push_front (name);
                         returnEnumValueList.push_front (enumValue);

                         i++;
                         counter++;
                       }
                  }
                 else
                  {
                 // printf ("initializerOther = NULL \n");
                  }
/*
             }
            else
             {
               printf ("Warning: In showSgDeclarationStatement case V_SgVariableDeclaration: (*i).get_named_item() = NULL \n");
             }
*/
        }
   }


#if 0
void
TransformationSupport::getTransformationOptionsFromVariableDeclarationConstructorArguments (
   SgVariableDeclaration* variableDeclaration,
   list<string> & returnEnumValueList )
   {
     ROSE_ASSERT (variableDeclaration != NULL);
     SgInitializedNameList & variableList = variableDeclaration->get_variables();

     for (SgInitializedNameList::iterator i = variableList.begin(); i != variableList.end(); i++)
        {
       // We don't care about the name
       // SgName & name = (*i).get_name();

          if ((*i).get_named_item() != NULL)
             {
               ROSE_ASSERT ((*i).get_named_item() != NULL);
               SgInitializer *initializerOther = (*i).get_named_item()->get_initializer();

            // This is not always a valid pointer
            // ROSE_ASSERT (initializerOther != NULL);

               if (initializerOther != NULL)
                  {
                 // There are other things we could ask of the initializer
                    ROSE_ASSERT (initializerOther != NULL);

                 // printf ("Now print out the initializerOther (a SgExpression*) \n");
                 // ROSE_ABORT();

                    SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(initializerOther);
                    ROSE_ASSERT (constructorInitializer != NULL);

                    SgExprListExp* argumentList = constructorInitializer->get_args();
                    ROSE_ASSERT (argumentList != NULL);

                    SgExpressionPtrList & expressionPtrList = argumentList->get_expressions();
                    SgExpressionPtrList::iterator i = expressionPtrList.begin();

                    int counter = 0;
	            while (i != expressionPtrList.end()) 
                       {
                      // printf ("Expression List Element #%d of %zu (total) \n",counter,expressionPtrList.size());

                         SgEnumVal* enumVal = isSgEnumVal(*i);
                         ROSE_ASSERT (enumVal != NULL);

                      // int enumValue = enumVal->get_value();
                         SgName enumName = enumVal->get_name();
                      // printf ("Name = %s value = %d \n",enumName.str(),enumValue);
                      // printf ("Name = %s \n",enumName.str());

                         string name ( ROSE::stringDuplicate( enumName.str() ) );
                      // char* name = ROSE::stringDuplicate( enumName.str() );

                      // Put the value at the start of the list so that the list can be processed in
                      // consecutive order to establish options for consecutive scopes (root to
                      // child scope). Order is not important if we are only ORing the operands!
                         returnEnumValueList.push_front (name);
                      // returnEnumValueList.push_front (enumValue);

                         i++;
                         counter++;
                       }
                  }
                 else
                  {
                 // printf ("initializerOther = NULL \n");
                  }
             }
            else
             {
               printf ("Warning: In showSgDeclarationStatement case V_SgVariableDeclaration: (*i).get_named_item() = NULL \n");
             }
        }
   }
#endif

#if 0
void
TransformationSupport::getTransformationOptionsFromVariableDeclarationConstructorArguments (
   SgVariableDeclaration* variableDeclaration,
   list<OptionDeclaration> & returnEnumValueList )
   {
     ROSE_ASSERT (variableDeclaration != NULL);
     SgInitializedNameList & variableList = variableDeclaration->get_variables();

     for (SgInitializedNameList::iterator i = variableList.begin(); i != variableList.end(); i++)
        {
       // We don't care about the name
       // SgName & name = (*i).get_name();

          if ((*i).get_named_item() != NULL)
             {
               ROSE_ASSERT ((*i).get_named_item() != NULL);
               SgInitializer *initializerOther = (*i).get_named_item()->get_initializer();

            // This is not always a valid pointer
            // ROSE_ASSERT (initializerOther != NULL);

               if (initializerOther != NULL)
                  {
                 // There are other things we could ask of the initializer
                    ROSE_ASSERT (initializerOther != NULL);

                 // printf ("Now print out the initializerOther (a SgExpression*) \n");
                 // ROSE_ABORT();

                    SgConstructorInitializer* constructorInitializer = isSgConstructorInitializer(initializerOther);
                    ROSE_ASSERT (constructorInitializer != NULL);

                    SgExprListExp* argumentList = constructorInitializer->get_args();
                    ROSE_ASSERT (argumentList != NULL);

                    SgExpressionPtrList & expressionPtrList = argumentList->get_expressions();
                    SgExpressionPtrList::iterator i = expressionPtrList.begin();

                 // First value is a char* identifying the option
                    SgStringVal* charString = isSgStringVal(*i);
                    ROSE_ASSERT (charString != NULL);


#if 0
                    int counter = 0;
	            while (i != expressionPtrList.end()) 
                       {
                         printf ("Expression List Element #%d of %zu (total) \n",counter,expressionPtrList.size());

                         SgEnumVal* enumVal = isSgEnumVal(*i);
                         ROSE_ASSERT (enumVal != NULL);

                      // int enumValue = enumVal->get_value();
                         SgName enumName = enumVal->get_name();
                      // printf ("Name = %s value = %d \n",enumName.str(),enumValue);
                      // printf ("Name = %s \n",enumName.str());

                         string name ( ROSE::stringDuplicate( enumName.str() ) );
                      // char* name = ROSE::stringDuplicate( enumName.str() );

                      // Put the value at the start of the list so that the list can be processed in
                      // consecutive order to establish options for consecutive scopes (root to
                      // child scope). Order is not important if we are only ORing the operands!
                         returnEnumValueList.push_front (name);
                      // returnEnumValueList.push_front (enumValue);

                         i++;
                         counter++;
                       }
#endif
                  }
                 else
                  {
                 // printf ("initializerOther = NULL \n");
                  }
             }
            else
             {
               printf ("Warning: In showSgDeclarationStatement case V_SgVariableDeclaration: (*i).get_named_item() = NULL \n");
             }
        }
   }
#endif

#if 0
// DQ (3/26/2004): I don't think this is used any more experiment with removing this
string
TransformationSupport::internalSupportingGlobalDeclarations (
     SgNode* astNode, string prefixString )
   {
  // This function generates a string that is used in the function which assembles the strings into
  // code that is called internally with the front-end to generate an AST fragements (which is
  // patched into the AST to introduce the transformation).

  // NOTE: This extra code is required to allow the string representing the transformation to be
  // compiled.  Once it is compiled, the AST is searched and only the AST fragment representing the
  // transformation is extracted (not the AST framents representing any of the code specified in
  // this function).

     printf ("In TransformationSupport::internalSupportingGlobalDeclarations(): astNode->unparseToString() = \n%s\n",astNode->unparseToString().c_str());

  // The use of a include file simplifies and shortens the 
  // declaration section of the intermediate (generated) file
     string staticStringSourceCodeTemplate = "\
\n\
$VARIABLE_DECLARATIONS\n\n\
";

     staticStringSourceCodeTemplate = prefixString + staticStringSourceCodeTemplate;

  // We need to have a string built from dynamically allocated memory using 
  // the C++ new operator since it will be deleted in the copyEdit() function
  // This avoids a purify error (so we have to call stringDuplicate())
     string sourceCodeTemplate = staticStringSourceCodeTemplate;

  // Variable declarations will have this form
     string staticVariableDeclarationString = "$TYPE_NAME $VARIABLE_NAME; \n";

  // The start of the variable declaration section of code begins with the following comment (this
  // string is appended with the actual variable declarations).
     string variableDeclarationString =
          "// Variables used in this transformation (automatically generated from simple dependence analysis of original code before transformation) \n";
     
     NameQuery::TypeOfQueryTypeOneParameter sageOneParameterEnumType = NameQuery::VariableTypeNames;
  // Nested Query:
  // Generate the list of types used within the target subtree of the AST
     list<string> typeNameStringList = NameQuery::querySubTree( astNode, sageOneParameterEnumType);

  // Loop over all the types and get list of variables of each type
  // (so they can be declared properly when the transformation is compiled)
     list<string>::iterator typeListStringElementIterator;
     for (typeListStringElementIterator = typeNameStringList.begin();
          typeListStringElementIterator != typeNameStringList.end();
          typeListStringElementIterator++)
        {
       // printf ("Type = %s \n",(*typeListStringElementIterator).c_str());

	  
       // Find a list of names of variable of type (*listStringElementIterator)
	  NameQuery::TypeOfQueryTypeTwoParameters sageTwoParametersEnumType = NameQuery::VariableNamesWithTypeName;
	  list<string> operandNameStringList =
               NameQuery::querySubTree(astNode, *typeListStringElementIterator, sageTwoParametersEnumType );

       // Loop over all the types and get list of variable of each type
          list<string>::iterator variableListStringElementIterator;
          for (variableListStringElementIterator = operandNameStringList.begin();
               variableListStringElementIterator != operandNameStringList.end();
               variableListStringElementIterator++)
             {
#if 0
               printf ("Type = %s Variable = %s \n",
                    (*typeListStringElementIterator).c_str(),
                    (*variableListStringElementIterator).c_str());
#endif

               string variableName = ROSE::stringDuplicate((*variableListStringElementIterator).c_str());
               string typeName = ROSE::stringDuplicate((*typeListStringElementIterator).c_str());
               string localOperandDataTemplate =
                    SgNode::copyEdit (staticVariableDeclarationString,"$VARIABLE_NAME",variableName);
               localOperandDataTemplate =
                    SgNode::copyEdit (localOperandDataTemplate,"$TYPE_NAME",typeName);

            // Append the new variable declaration to the variableDeclarationString
            // variableDeclarationString = ROSE::stringConcatinate (variableDeclarationString,localOperandDataTemplate);
               variableDeclarationString =  variableDeclarationString + localOperandDataTemplate;
             }
#if 0
          printf ("Exiting in loop internalSupportingGlobalDeclarations (type = %s) ... \n",(*typeListStringElementIterator).c_str());
          ROSE_ABORT();
#endif
        }

#if 0
     printf ("Exiting at base of internalSupportingGlobalDeclarations ... \n");
     ROSE_ABORT();
#endif

  // Substitute the code fragment representing variable declaration into the generated source code
  // representing the specification of the transformation.
     string finalSourceCodeString = SgNode::copyEdit ( sourceCodeTemplate, "$VARIABLE_DECLARATIONS" , variableDeclarationString );

     return finalSourceCodeString;
   }
#endif


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
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;
  // printf ("Starting at parentNode->sage_class_name() = %s \n",parentNode->sage_class_name());
     while (parentNode->get_parent() != NULL)
        {
          parentNode = parentNode->get_parent();
       // printf ("     parentNode->sage_class_name() = %s \n",parentNode->sage_class_name());
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
	if (isSgProject(parentNode) == NULL &&
			dynamic_cast<const SgType*> (parentNode) == NULL &&
			dynamic_cast<const SgSymbol*> (parentNode) == NULL)
	{
		if (SgProject::get_verbose() > 0)
		{
			if (astNode == NULL)
				printf("Warning: could not trace back to SgProject node. \n");
			else
				printf("Warning: could not trace back to SgProject node from %s \n",
					astNode->class_name().c_str());
		}
	}
	else
	{
		if (dynamic_cast<const SgType*> (parentNode) != NULL || dynamic_cast<const SgSymbol*> (parentNode) != NULL)
		{
			printf("Error: can't locate an associated SgProject from astNode = %p = %s parentNode = %p = %s \n", astNode, astNode->class_name().c_str(), parentNode, parentNode->class_name().c_str());
			return NULL;
		}
	}

  // Make sure we have a SgProject node
     const SgProject* project = isSgProject(parentNode);
     ROSE_ASSERT (project != NULL);

  // return project;
     return const_cast<SgProject*>(project);
   }

SgDirectory*
TransformationSupport::getDirectory( const SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;
     while ( (isSgDirectory(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (8/2/2005): Modified this so that we can return NULL so that AST framents 
  // not associated with a primary AST can be used with this function!
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgDirectory(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
       // printf ("Error: could not trace back to SgDirecoty node \n");
       // ROSE_ASSERT(false);
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgFile from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }

  // Make sure we have a SgFile node
     const SgDirectory* directory = isSgDirectory(parentNode);

     return const_cast<SgDirectory*>(directory);
   }

SgFile*
TransformationSupport::getFile( const SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;
     while ( (isSgFile(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (8/2/2005): Modified this so that we can return NULL so that AST framents 
  // not associated with a primary AST can be used with this function!
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgFile(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
       // printf ("Error: could not trace back to SgFile node \n");
       // ROSE_ASSERT(false);
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgFile from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }


  // Make sure we have a SgFile node
     const SgFile* file = isSgFile(parentNode);

  // DQ (8/2/2005): Allow to return NULL
  // ROSE_ASSERT (file != NULL);

  // return file;
     return const_cast<SgFile*>(file);
   }

// DQ (9/3/2008): This used to use SgFile and was switched to use SgSourceFile.
SgSourceFile*
TransformationSupport::getSourceFile( const SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;
     while ( (isSgSourceFile(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (8/2/2005): Modified this so that we can return NULL so that AST framents 
  // not associated with a primary AST can be used with this function!
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgSourceFile(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
       // printf ("Error: could not trace back to SgSourceFile node \n");
       // ROSE_ASSERT(false);
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
            // DQ (3/4/2009): This test will remove most of the instances of this warning (ignoring types).
               if (isSgType(parentNode) == NULL)
                  {
                    printf ("Warning: can't locate an associated SgSourceFile from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
                  }
               return NULL;
             }
        }


  // Make sure we have a SgSourceFile node
     const SgSourceFile* file = isSgSourceFile(parentNode);

  // DQ (8/2/2005): Allow to return NULL
  // ROSE_ASSERT (file != NULL);

  // return file;
     return const_cast<SgSourceFile*>(file);
   }

// DQ (9/3/2008): This used to use SgFile and was switched to use SgBinaryComposite.
SgBinaryComposite*
TransformationSupport::getBinaryFile( const SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;
     while ( (isSgBinaryComposite(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // DQ (8/2/2005): Modified this so that we can return NULL so that AST framents 
  // not associated with a primary AST can be used with this function!
  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgBinaryComposite(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
       // printf ("Error: could not trace back to SgBinaryComposite node \n");
       // ROSE_ASSERT(false);
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgBinaryComposite from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }


  // Make sure we have a SgBinaryComposite node
     const SgBinaryComposite* file = isSgBinaryComposite(parentNode);

  // DQ (8/2/2005): Allow to return NULL
  // ROSE_ASSERT (file != NULL);

  // return file;
     return const_cast<SgBinaryComposite*>(file);
   }

SgGlobal*
TransformationSupport::getGlobalScope( const SgNode* astNode )
   {
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;
  // printf ("TransformationSupport::getGlobalScope(): Starting node: parentNode = %p = %s \n",parentNode,parentNode->class_name().c_str());
     while ( (isSgGlobal(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
       // printf ("parentNode = %p = %s \n",parentNode,parentNode->class_name().c_str());
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
	if (isSgGlobal(parentNode) == NULL &&
			dynamic_cast<const SgType*> (parentNode) == NULL &&
			dynamic_cast<const SgSymbol*> (parentNode) == NULL)
	{
		//It is possible to find no SgGlobal during transformation, changed to warning.
		if (SgProject::get_verbose() > 0)
		{
			if (astNode != NULL)
				printf("Warning: could not trace back to SgGlobal node from %s \n",
					astNode->class_name().c_str());
			else
				printf("Warning: could not trace back to SgGlobal node\n ");
		}
		return NULL;
	}
	else
	{
		if (dynamic_cast<const SgType*> (parentNode) != NULL || dynamic_cast<const SgSymbol*> (parentNode) != NULL)
		{
			// printf ("Error: can't locate an associated SgGlobal from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
			return NULL;
		}
	}


  // Make sure we have a SgGlobal node
     const SgGlobal* globalScope = isSgGlobal(parentNode);
     ROSE_ASSERT (globalScope != NULL);

  // return globalScope;
     return const_cast<SgGlobal*>(globalScope);
   }

SgStatement*
TransformationSupport::getStatement( const SgNode* astNode )
   {
  // DQ (3/1/2009): Added assertion.
     ROSE_ASSERT(astNode != NULL);

     const SgNode* parentNode = astNode;

  // DQ (6/27/2007): These IR nodes are not contained in any statement
     if (isSgProject(astNode) != NULL || isSgFile(astNode) != NULL)
          return NULL;

     while ( (isSgStatement(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgStatement(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL)
	{
		if (SgProject::get_verbose() > 0)
		{
			if (astNode == NULL)
				printf("Error: could not trace back to SgStatement node \n");
			else
				printf("Warning: could not trace back to SgStatement node from %s \n", astNode->class_name().c_str());
		}

		return NULL;
	}
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
            // Test for SgArrayType since a value if often hidden there and it is not possible to traverse 
            // through a SgType along parent IR nodes.
               if ( dynamic_cast<const SgArrayType*>(parentNode) != NULL )
                  {
                    return NULL;
                  }

            // DQ (11/10/2007): Note that for an AST fragment (e.g. expression) not connected to the AST, this function will return NULL.
#if PRINT_DEVELOPER_WARNINGS
               printf ("Warning: can't locate an associated SgStatement from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
#endif
               return NULL;
             }
        }

  // Make sure we have a SgStatement node
     const SgStatement* statement = isSgStatement(parentNode);
     ROSE_ASSERT (statement != NULL);

  // return statement;
     return const_cast<SgStatement*>(statement);
   }

SgFunctionDeclaration*
TransformationSupport::getFunctionDeclaration( const SgNode* astNode)
   {
     const SgNode* parentNode = astNode;
     while ( (isSgFunctionDeclaration(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgFunctionDeclaration(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
          if (astNode==NULL)
          printf ("Error: could not trace back to SgFunctionDeclaration node \n");
          else
               printf ("Warning: could not trace back to SgFunctionDeclaration node from %s \n",
          astNode->class_name().c_str());
          ROSE_ABORT();
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgFunctionDeclaration from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }


  // Make sure we have a SgFunctionDeclaration node
     const SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(parentNode);
     ROSE_ASSERT (functionDeclaration != NULL);

  // return functionDeclaration;
     return const_cast<SgFunctionDeclaration*>(functionDeclaration);
   }

SgFunctionDefinition*
TransformationSupport::getFunctionDefinition( const SgNode* astNode)
   {
     const SgNode* parentNode = astNode;
     while ( (isSgFunctionDefinition(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgFunctionDefinition(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
          if(astNode==NULL)
          printf ("Error: could not trace back to SgFunctionDefinition node \n");
          else 
               printf ("Error: could not trace back to SgFunctionDefinition node from %s \n",
          astNode->class_name().c_str());
          ROSE_ABORT();
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgFunctionDefinition from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }

  // Make sure we have a SgStatement node
     const SgFunctionDefinition* returnFunctionScope = isSgFunctionDefinition(parentNode);
     ROSE_ASSERT (returnFunctionScope != NULL);

  // return class definition statement;
     return const_cast<SgFunctionDefinition*>(returnFunctionScope);
   }

SgClassDefinition*
TransformationSupport::getClassDefinition( const SgNode* astNode)
   {
     const SgNode* parentNode = astNode;
     while ( (isSgClassDefinition(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgClassDefinition(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
          printf ("Error: could not trace back to SgClassDefinition node \n");
          ROSE_ABORT();
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgClassDefinition from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }

  // Make sure we have a SgStatement node
     const SgClassDefinition* returnClassScope = isSgClassDefinition(parentNode);
     ROSE_ASSERT (returnClassScope != NULL);

  // return class definition statement;
     return const_cast<SgClassDefinition*>(returnClassScope);
   }

SgModuleStatement*
TransformationSupport::getModuleStatement( const SgNode* astNode)
   {
  // DQ (11/24/2007): This function supports the unparsing of the PUBLIC, PRIVATE keywords (only permitted within modules)
  // The name of this function might change to getModuleDeclaration
     const SgNode* parentNode = astNode;
     while ( (isSgModuleStatement(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgModuleStatement(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL)
	{
		if (SgProject::get_verbose() > 0)
		{
			printf("Warning: could not trace back to SgModuleStatement node \n");
		}
		return NULL;
	}
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgModuleStatement from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }

  // Make sure we have a SgStatement node
     const SgModuleStatement* returnModuleDeclaration = isSgModuleStatement(parentNode);
     ROSE_ASSERT (returnModuleDeclaration != NULL);

     return const_cast<SgModuleStatement*>(returnModuleDeclaration);
   }

#if 0
// Moved to SgTemplateArgument!!!
SgScopeStatement*
TransformationSupport::getScope( const SgNode* astNode )
   {
  // DQ (6/9/2007): This function traverses through the parents to the first scope (used for name qualification support of template arguments)

     const SgNode* parentNode = astNode;
     while ( (isSgScopeStatement(parentNode) == NULL) && (parentNode->get_parent() != NULL) )
        {
          parentNode = parentNode->get_parent();
        }

  // Check to see if we made it back to the root (current root is SgProject).
  // It is also OK to stop at a node for which get_parent() returns NULL (SgType and SgSymbol nodes).
     if ( isSgScopeStatement(parentNode) == NULL &&
          dynamic_cast<const SgType*>(parentNode) == NULL &&
          dynamic_cast<const SgSymbol*>(parentNode) == NULL )
        {
          printf ("Error: could not trace back to SgScopeStatement node \n");
          ROSE_ABORT();
        }
       else
        {
          if ( dynamic_cast<const SgType*>(parentNode) != NULL || dynamic_cast<const SgSymbol*>(parentNode) != NULL )
             {
               printf ("Error: can't locate an associated SgStatement from astNode = %p = %s parentNode = %p = %s \n",astNode,astNode->class_name().c_str(),parentNode,parentNode->class_name().c_str());
               return NULL;
             }
        }

  // Make sure we have a SgStatement node
     const SgScopeStatement* scopeStatement = isSgScopeStatement(parentNode);
     ROSE_ASSERT (scopeStatement != NULL);

  // return statement;
     return const_cast<SgScopeStatement*>(scopeStatement);
   }
#endif










