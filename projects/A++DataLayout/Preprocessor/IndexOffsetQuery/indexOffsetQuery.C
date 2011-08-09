#include "arrayPreprocessor.h"

IndexOffsetExpressionQuerySynthesizedAttributeType IndexOffsetQuery::transformation(
		SgNode* astNode) {
	// This function returns a string representing the index offset used in A(I+1) (e.g. the "+1" part).

	// printf ("################# START OF INDEX OFFSET EXPRESSION STRING QUERY ################ \n");

	IndexOffsetQuery treeTraversal;
	IndexOffsetExpressionQuerySynthesizedAttributeType returnString;

	returnString = treeTraversal.traverse(astNode);

	// printf ("######################### END OF INDEX OFFSET EXPRESSION STRING QUERY ######################## \n");

	// printf ("In IndexOffsetQuery::transformation(): returnString = %s (exiting ...) \n",returnString.c_str());

	return returnString;
}

IndexOffsetExpressionQuerySynthesizedAttributeType IndexOffsetQuery::evaluateSynthesizedAttribute(
		SgNode* astNode, SubTreeSynthesizedAttributes synthesizedAttributeList) {
	// This function assembles the elements of the input list (a list of char*) to form the output (a single char*)

#if 0
	printf ("&&&&& TOP of indexOffsetExpressionQueryAssemblyFunction (astNode->sage_class_name() = %s,synthesizedAttributeList.size() = %d) \n",
			astNode->sage_class_name(),synthesizedAttributeList.size());
#endif

	// Build up a return string
	string returnString;

	vector<IndexOffsetExpressionQuerySynthesizedAttributeType>::iterator i;

#if 0
	// Print out the elements of the list
	int counter = 0;
	for (i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end(); i++)
	{
		ROSE_ASSERT ((*i).c_str() != NULL);
		printf ("Element %d of SynthesizedAttributeListElements: string = %s \n",counter,(*i).c_str());
		counter++;
	}
#endif

	string operatorString;

	// Need to handle all unary and binary operators and variables (but not much else)
	switch (astNode->variant()) {
	// Need to handle prefix and postfix unary operators
	case UNARY_MINUS_OP:
	case UNARY_ADD_OP:
	case NOT_OP:
	case DEREF_OP:
	case ADDRESS_OP:
	case MINUSMINUS_OP:
	case PLUSPLUS_OP:
	case UNARY_EXPRESSION:

		// Handle binary operators
	case ADD_OP:
	case SUBT_OP:
	case MULT_OP:
	case DIV_OP:
	case INTEGER_DIV_OP:
	case MOD_OP:
	case AND_OP:
	case OR_OP:
	case BITXOR_OP:
	case BITAND_OP:
	case BITOR_OP:
	case EQ_OP:
	case LT_OP:
	case GT_OP:
	case NE_OP:
	case LE_OP:
	case GE_OP:
	case ASSIGN_OP:
	case BINARY_EXPRESSION:

		// Values
	case ValueExpTag:
	case BOOL_VAL:
	case STRING_VAL:
	case SHORT_VAL:
	case CHAR_VAL:
	case UNSIGNED_CHAR_VAL:
	case WCHAR_VAL:
	case UNSIGNED_SHORT_VAL:
	case ENUM_VAL:
	case UNSIGNED_INT_VAL:
	case LONG_INT_VAL:
	case LONG_LONG_INT_VAL:
	case UNSIGNED_LONG_LONG_INT_VAL:
	case UNSIGNED_LONG_INT_VAL:
	case INT_VAL:
	case FLOAT_VAL:
	case DOUBLE_VAL:
	case LONG_DOUBLE_VAL: {
		// Call the unparser to generate a string for any scalar expression (shortest possible
		// way to generate string). This causes the unparse function to be called redundently
		// until we finaly use unparsed string associated with the maximal expression (longest
		// scalar expression).
		returnString = astNode->unparseToString();

		break;
	}

		// We do need this since the expression list in the overloaded operators is the only access
		// to the integer expression that we need to build.
	case EXPR_LIST: {
		// Most overloaded functions accept their lhs and rhs within the parameter list
		// (e.g. friend X operator+(X lhs,X rhs) or X operator=(X rhs) for a class X)
		SgExprListExp* exprListExp = isSgExprListExp(astNode);
		ROSE_ASSERT (exprListExp != NULL);

		// NOTE: Similar code appears in the assembly function in 
		//       arrayAssignmentStatementTransformation.C
		// Get the parent and check if it is an overloaded operator
		// Otherwise this could be ANY expression list in a program's AST!
		SgExpression* parentExpression = isSgExpression(exprListExp->get_parent());
		ROSE_ASSERT (parentExpression != NULL);

		switch (parentExpression->variant()) {
		case FUNC_CALL: {
			SgFunctionCallExp* functionCallExpression = isSgFunctionCallExp(parentExpression);
			ROSE_ASSERT (functionCallExpression != NULL);

			// Get the name of the parent function
			string operatorName = TransformationSupport::getFunctionName(functionCallExpression);
			ROSE_ASSERT (operatorName.c_str() != NULL);

			// Classify the function name (it could be an overloaded function)
			TransformationSupport::operatorCodeType operatorCodeVariant =
					TransformationSupport::classifyOverloadedOperator(operatorName.c_str());
			// printf ("operatorCodeVariant = %d \n",operatorCodeVariant);

			switch (operatorCodeVariant) {
			// normal function call (not an overloaded function) (used as the default value)
			case TransformationSupport::FUNCTION_CALL_OPERATOR_CODE:
				break;

			case TransformationSupport::PARENTHESIS_OPERATOR_CODE:
				// returnString = " --operator()() not implemented-- ";
				// printf ("Found an operator()() (operator code = %d)  \n",operatorCodeVariant);
				returnString = "";
				for (i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end();
						i++) {
					// concatenate the synthesized attributes
					returnString = returnString + *i;
				}

				// printf ("ERROR: PARENTHESIS_OPERATOR_CODE not implemented \n");
				// ROSE_ABORT();
				break;
#if 1
			case TransformationSupport::ASSIGN_OPERATOR_CODE:
				ROSE_ASSERT (synthesizedAttributeList.size() == 1);

				returnString = "$OPERATOR" + synthesizedAttributeList[0];

				// printf ("ERROR: ASSIGN_OPERATOR_CODE not implemented \n");
				// ROSE_ABORT();
				break;
#else
				case TransformationSupport::ASSIGN_OPERATOR_CODE:
#endif
				// Overloaded binary operators: These are implemented a friend functions taking 2
				// parameters instead of member functions taking only 1 parameter, as a result they
				// have to be handled differently.
			case TransformationSupport::ADD_OPERATOR_CODE:
			case TransformationSupport::SUBT_OPERATOR_CODE:
			case TransformationSupport::MULT_OPERATOR_CODE:
			case TransformationSupport::DIV_OPERATOR_CODE:
			case TransformationSupport::INTEGER_DIV_OPERATOR_CODE:
			case TransformationSupport::MOD_OPERATOR_CODE:
			case TransformationSupport::AND_OPERATOR_CODE:
			case TransformationSupport::OR_OPERATOR_CODE:
			case TransformationSupport::BITXOR_OPERATOR_CODE:
			case TransformationSupport::BITAND_OPERATOR_CODE:
			case TransformationSupport::BITOR_OPERATOR_CODE:
			case TransformationSupport::EQ_OPERATOR_CODE:
			case TransformationSupport::LT_OPERATOR_CODE:
			case TransformationSupport::GT_OPERATOR_CODE:
			case TransformationSupport::NE_OPERATOR_CODE:
			case TransformationSupport::LE_OPERATOR_CODE:
			case TransformationSupport::GE_OPERATOR_CODE:
			case TransformationSupport::PLUS_ASSIGN_OPERATOR_CODE:
			case TransformationSupport::MINUS_ASSIGN_OPERATOR_CODE:
			case TransformationSupport::AND_ASSIGN_OPERATOR_CODE:
			case TransformationSupport::IOR_ASSIGN_OPERATOR_CODE:
			case TransformationSupport::MULT_ASSIGN_OPERATOR_CODE:
			case TransformationSupport::DIV_ASSIGN_OPERATOR_CODE:
			case TransformationSupport::MOD_ASSIGN_OPERATOR_CODE:
			case TransformationSupport::XOR_ASSIGN_OPERATOR_CODE: {
				// printf ("Found an OVERLOADED BINARY OPERATOR expression \n");

				// Using a higher-level grammar would allow us to separate the different
				// overloaded functions but since we don't have that in place we need to
				// do so manually (what a pain!).

				// Check the type of the operand, if it is an InternalIndex then the
				// expression statement is in the other operand.  There should oly be two
				// operands.

				ROSE_ASSERT (synthesizedAttributeList.size() == 2);

				// useless test ???
				SgExpression* expression = isSgExpression(astNode);
				ROSE_ASSERT (expression != NULL);

				returnString = synthesizedAttributeList[0] + "$OPERATOR"
						+ synthesizedAttributeList[1];
				break;
			}

				// unary operators:
			case TransformationSupport::NOT_OPERATOR_CODE:
			case TransformationSupport::DEREFERENCE_OPERATOR_CODE:
			case TransformationSupport::ADDRESS_OPERATOR_CODE:
			case TransformationSupport::LSHIFT_OPERATOR_CODE:
			case TransformationSupport::RSHIFT_OPERATOR_CODE:
			case TransformationSupport::LSHIFT_ASSIGN_OPERATOR_CODE:
			case TransformationSupport::RSHIFT_ASSIGN_OPERATOR_CODE:
			case TransformationSupport::PREFIX_PLUSPLUS_OPERATOR_CODE:
			case TransformationSupport::POSTFIX_PLUSPLUS_OPERATOR_CODE:
			case TransformationSupport::PREFIX_MINUSMINUS_OPERATOR_CODE:
			case TransformationSupport::POSTFIX_MINUSMINUS_OPERATOR_CODE: {
				// Assemble the string for "!A" instead of "A!" which is what is currently generated!
				// assemble the name of the function name + variable refernece to be the string (instead of default assembly!) 

				// printf ("Found an OVERLOADED UNARY OPERATOR expression \n");

				// Need to recognize prefix vs. postfix operators and build the return
				// string accordingly.

				printf("ERROR: Still need to associated operator with unary expression \n");
				ROSE_ABORT();

				returnString = "$OPERATOR" + synthesizedAttributeList[0];
				break;
			}

			default:
				printf("default in switch found in variant = %d (NAME NOT IMPLEMENTED) \n",
						operatorCodeVariant);
				ROSE_ABORT();
			}

			break;
		}

		case CONSTRUCTOR_INIT: {
			// This case is important for the identification of scalars used within
			// indexing operators (not the same thing as scalar indexing)
			printf("Parent of SgExprListExp is SgContructorInitializer \n");

			ROSE_ASSERT (synthesizedAttributeList.size() > 0);
			string integerExpressionString = synthesizedAttributeList[0];

			// This handles the case of "A(I,2)" where the "2" is promoted to a
			// InternalIndex though a call to the InternalIndex constructor which takes a
			// single integer.
			returnString = integerExpressionString;

#if 0
			printf ("Exiting as a test of this case (SgContructorInitializer) \n");
			ROSE_ABORT();
#endif
			break;
		}

		default: {
			// Not really sure how to handle it since it depends upon what the parent is!
			printf("parentExpression = %s \n", parentExpression->sage_class_name());

			printf(
					"In IndexOffsetQuery: parent of SgExprListExp is not a SgFunctionCallExp (need to allow for these cases explicitly) exiting ... \n");
			ROSE_ABORT();
		}
		}

#if 0
		printf ("Found case of EXPR_LIST (SgExprListExp) in assembly (returnString = %s) \n",returnString.c_str());
		ROSE_ABORT();
#endif
		break;
	}

	case FUNC_CALL: // tag for SgFunctionCallExp class
	{
		// The first overloaded function we want to handle is the "operator()(InternalIndex)"
		// all other function generate an error presently
		SgFunctionCallExp* functionCallExp = isSgFunctionCallExp(astNode);
		ROSE_ASSERT (functionCallExp != NULL);

		string operatorName = TransformationSupport::getFunctionName(functionCallExp);
		ROSE_ASSERT (operatorName.c_str() != NULL);

		TransformationSupport::operatorCodeType operatorCodeVariant =
				TransformationSupport::classifyOverloadedOperator(operatorName.c_str());
		// printf ("operatorCodeVariant = %d \n",operatorCodeVariant);

		string operatorString = TransformationSupport::buildOperatorString(astNode);

		// printf ("operatorString = %s \n",operatorString.c_str());

		switch (operatorCodeVariant) {
		// Overloaded binary operators: These are implemented a friend functions taking 2
		// parameters instead of member functions taking only 1 parameter, as a result they
		// have to be handled differently.
		case TransformationSupport::ADD_OPERATOR_CODE:
		case TransformationSupport::SUBT_OPERATOR_CODE: {
			// printf ("Found an OVERLOADED operator+ or operator- OPERATOR expression \n");

#if 0
			// Now get the operands out and search for the offsets in the index objects
			// get the value of the offsets (start the search from the functionCallExp)
			SgExprListExp* exprListExp = functionCallExp->get_args();
			ROSE_ASSERT (exprListExp != NULL);
			SgExpressionPtrList & expressionPtrList = exprListExp->get_expressions();
			SgExpressionPtrList::iterator i = expressionPtrList.begin();
			// The InternalIndex::operator+ only takes a single operand (an integer expression)
			ROSE_ASSERT (expressionPtrList.size() == 2);
			string indexOffsetString[2];
			printf ("Looking for the offset: \n");
#endif

			returnString = synthesizedAttributeList[SgFunctionCallExp_args];
			//returnString = SgNode::copyEdit(returnString,"$OPERATOR",operatorString);

			// Add parenthesis in case this is part of a subexpression ( e.g. (4+I)-7 )
			// returnString = "(" + returnString + ")";

			// Logic for requirement of parenthesises
			SgExpression* parentExpression = isSgExpression(functionCallExp->get_parent());
			int parent_precedence = (parentExpression) ? parentExpression->precedence() : 0;
			int our_precedence = functionCallExp->precedence();
			int skip_parens = ((parent_precedence > 0) && (our_precedence > parent_precedence));

			if (!skip_parens) {
				returnString = "(" + returnString + ")";
			}

			// printf ("returnString = %s \n",returnString.c_str());

#if 0
			printf ("Exiting after finding an offset to InternalIndex::operator+(int) or operator-(int)  (how are other dimensions handled?) ... \n");
			ROSE_ABORT();
#endif
			break;
		}

		case TransformationSupport::MULT_OPERATOR_CODE:
		case TransformationSupport::DIV_OPERATOR_CODE:
		case TransformationSupport::INTEGER_DIV_OPERATOR_CODE:
		case TransformationSupport::MOD_OPERATOR_CODE:
		case TransformationSupport::AND_OPERATOR_CODE:
		case TransformationSupport::OR_OPERATOR_CODE:
		case TransformationSupport::BITXOR_OPERATOR_CODE:
		case TransformationSupport::BITAND_OPERATOR_CODE:
		case TransformationSupport::BITOR_OPERATOR_CODE:
		case TransformationSupport::EQ_OPERATOR_CODE:
		case TransformationSupport::LT_OPERATOR_CODE:
		case TransformationSupport::GT_OPERATOR_CODE:
		case TransformationSupport::NE_OPERATOR_CODE:
		case TransformationSupport::LE_OPERATOR_CODE:
		case TransformationSupport::GE_OPERATOR_CODE:
		case TransformationSupport::PLUS_ASSIGN_OPERATOR_CODE:
		case TransformationSupport::MINUS_ASSIGN_OPERATOR_CODE:
		case TransformationSupport::AND_ASSIGN_OPERATOR_CODE:
		case TransformationSupport::IOR_ASSIGN_OPERATOR_CODE:
		case TransformationSupport::MULT_ASSIGN_OPERATOR_CODE:
		case TransformationSupport::DIV_ASSIGN_OPERATOR_CODE:
		case TransformationSupport::MOD_ASSIGN_OPERATOR_CODE:
		case TransformationSupport::XOR_ASSIGN_OPERATOR_CODE: {
			printf("Overloaded operator %s not implemented \n", operatorName.c_str());
			break;
		}

		case TransformationSupport::PARENTHESIS_OPERATOR_CODE: {
			ROSE_ASSERT ( operatorName == "operator()" );
			printf(
					"Indexing of InternalIndex objects using InternalIndex::operator() is not implemented yet! \n");
			ROSE_ABORT();
			break;
		}

		default:
			printf("default in switch found in variant = %d (NAME NOT IMPLEMENTED) \n",
					operatorCodeVariant);
			ROSE_ABORT();
		}

		// printf ("In indexOffsetExpressionQueryAssemblyFunction(): FUNC_CALL case: returnString = %s \n",returnString.c_str());
		// ROSE_ABORT();

		// Error checking
		ROSE_ASSERT (returnString.c_str() != NULL);
		break;
	}

	case VAR_REF: {
		SgVarRefExp* varRefExp = isSgVarRefExp(astNode);
		ROSE_ASSERT (varRefExp != NULL);

		SgType* type = varRefExp->get_type();
		ROSE_ASSERT (type != NULL);

		string typeName = TransformationSupport::getTypeName(type);
		ROSE_ASSERT (typeName.c_str() != NULL);

		// Check for any array objects (such as for indirect addressing)
		if (typeName == "intArray" || typeName == "floatArray" || typeName == "doubleArray") {
			printf(
					"Sorry, Indirect addressing not implemented yet (there should be no A++/P++ array objects defined within index expressions) \n");
			ROSE_ABORT();
		}

		// Recognize only these types at present
		if ((typeName == "Range") || (typeName == "Index") || (typeName == "InternalIndex")) {
			// Only define the variable name if we are using an object of InternalIndex type
			returnString = "$INDEX_OBJECT";
		} else {
			// Everything else just output the variable name (permits "I+n")
			SgVariableSymbol* variableSymbol = varRefExp->get_symbol();
			ROSE_ASSERT (variableSymbol != NULL);

			SgInitializedName* initializedName = variableSymbol->get_declaration();
			ROSE_ASSERT (initializedName != NULL);
			SgName variableName = initializedName->get_name();
			ROSE_ASSERT (variableName.str() != NULL);

			returnString = variableName.str();
		}
		break;
	}

	case RECORD_REF:  // SgDotExp
		// case ValueExpTag:
	default:
		// printf ("default in switch found in indexOffsetExpressionQueryAssemblyFunction() (sage_class_name = %s \n",
		//      astNode->sage_class_name());

		// The default is to concatinate all the strings together (in the order in which they appear)
		for (i = synthesizedAttributeList.begin(); i != synthesizedAttributeList.end(); i++) {
#if 0
			printf ("Looping through the list of SynthesizedAttributeListElements: size of attributeList = %s \n",
					(*i).c_str());
#endif
			returnString = returnString + *i;
		}

		// ROSE_ABORT();
		break;
	}

#if 0
	printf ("&&&&& BOTTOM of indexOffsetExpressionQueryAssemblyFunction at astNode = %s (returnString = %s) \n",astNode->sage_class_name(),returnString.c_str());
#endif

	return returnString;
}

