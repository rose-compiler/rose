// This translator acts as a filter to restrict the language used to
// express events in the discrete event simulation.
// We only restrict the language constructs used to express events,
// and impose no restriction of the langauge used to express other 
// parts of the the descrete event simulation.
// Step 1: Identify the event functions (save as a list)
// Step 2: Enforce language restrictions on the event functions.

#include "rose.h"

#include <algorithm>
#include <functional>
#include <numeric>
#include "LanguageRestrictions.h"
#include "normalizations/CPPDefinesAndNamespaces.h"


using namespace std;
using namespace SageBuilder;
using namespace SageInterface;



class RestrictionTraversal : public AstBottomUpProcessing<bool>
{
public:
	virtual bool evaluateSynthesizedAttribute(SgNode* n, SynthesizedAttributesList childAttributes);
};


bool RestrictionTraversal::evaluateSynthesizedAttribute(SgNode* node, SynthesizedAttributesList childAttributes)
{
	// Note: because the default value for a bool is false, or "0", we have to phrase the question in terms of a violation being true!

	// Fold up the list of child attributes using logical or, i.e. the local
	// result will be true if any of the child attributes are true.
	// SynthesizedAttribute localResult = std::accumulate(childAttributes.begin(), childAttributes.end(), true, std::logical_and<bool>());
	// bool localResult = std::accumulate(childAttributes.begin(), childAttributes.end(), true, std::logical_and<bool>());
	bool localResult = std::accumulate(childAttributes.begin(), childAttributes.end(), false, std::logical_or<bool>());

#if 0
	printf("In evaluateSynthesizedAttribute (node = %p = %s) localResult = %s childAttributes.size() = %zu \n", node, node->class_name().c_str(), localResult ? "true" : "false", childAttributes.size());
#endif

#if 0
	for (size_t i = 0; i < childAttributes.size(); i++)
	{
		printf("childAttributes[%zu] = %s \n", i, childAttributes[i] ? "true" : "false");
	}
#endif

	// Test if this is an acceptable type
	switch (node->variantT())
	{
			// This sort of statement is NOT OK (we do need to specify everything that is unaccpetable).
		case V_SgWhileStmt: localResult = true;
			break;
		case V_SgForStatement: localResult = true;
			break;
		case V_SgIfStmt: localResult = true;
			break;
		case V_SgClassDeclaration: localResult = true;
			break;

		case V_SgInitializedName:
		{
			// If this is a variable declaration then check the type (we only allow integer types)
			SgInitializedName* initializedName = isSgInitializedName(node);
			SgType* type = initializedName->get_type();

			SgTypeInt* typeInt = isSgTypeInt(type);
			if (typeInt == NULL)
			{
				printf("Failing type test for intialized name = %p = %s type = %p = %s \n", initializedName, initializedName->get_name().str(), type, type->class_name().c_str());
				localResult = true;
			}
			break;
		}

			// And don't allow any of this complexity!
		case V_SgPragma: localResult = true;
			break;
		case V_SgTemplateArgument: localResult = true;
			break;
		case V_SgBaseClass: localResult = true;
			break;
		case V_SgLabelRefExp: localResult = true;
			break;
		case V_SgMemberFunctionRefExp: localResult = true;
			break;

		case V_SgTemplateInstantiationDirectiveStatement: localResult = true;
			break;
		case V_SgUsingDirectiveStatement: localResult = true;
			break;
		case V_SgUsingDeclarationStatement: localResult = true;
			break;
		case V_SgNamespaceDefinitionStatement: localResult = true;
			break;
		case V_SgNamespaceAliasDeclarationStatement: localResult = true;
			break;
		case V_SgNamespaceDeclarationStatement: localResult = true;
			break;
		case V_SgTemplateDeclaration: localResult = true;
			break;
		case V_SgContinueStmt: localResult = true;
			break;
		case V_SgBreakStmt: localResult = true;
			break;
		case V_SgDefaultOptionStmt: localResult = true;
			break;
		case V_SgTryStmt: localResult = true;
			break;
		case V_SgCaseOptionStmt: localResult = true;
			break;
		case V_SgLabelStatement: localResult = true;
			break;
		case V_SgTypedefDeclaration: localResult = true;
			break;
		case V_SgAsmStmt: localResult = true;
			break;
		case V_SgEnumDeclaration: localResult = true;
			break;
		case V_SgCatchOptionStmt: localResult = true;
			break;
		case V_SgSwitchStatement: localResult = true;
			break;
		case V_SgDoWhileStmt: localResult = true;
			break;
		case V_SgCtorInitializerList: localResult = true;
			break;
		case V_SgCatchStatementSeq: localResult = true;
			break;
		case V_SgForInitStatement: localResult = true;
			break;
		case V_SgLshiftAssignOp: localResult = true;
			break;
		case V_SgRshiftAssignOp: localResult = true;
			break;
		case V_SgConditionalExp: localResult = true;
			break;
		case V_SgSizeOfOp: localResult = true;
			break;
		case V_SgAggregateInitializer: localResult = true;
			break;
		case V_SgConstructorInitializer: localResult = true;
			break;
		case V_SgNewExp: localResult = true;
			break;
		case V_SgDeleteExp: localResult = true;
			break;
		case V_SgThisExp: localResult = true;
			break;
		case V_SgPointerDerefExp: localResult = true;
			break;
		case V_SgThrowOp: localResult = true;
			break;
		case V_SgArrowExp: localResult = true;
			break;
		case V_SgDotExp: localResult = true;
			break;
		case V_SgDotStarOp: localResult = true;
			break;
		case V_SgArrowStarOp: localResult = true;
			break;
		case V_SgCommaOpExp: localResult = true;
			break;
		case V_SgLshiftOp: localResult = true;
			break;
		case V_SgRshiftOp: localResult = true;
			break;
		case V_SgPntrArrRefExp: localResult = true;
			break;
		case V_SgScopeOp: localResult = true;
			break;

			// Unclear if we should allow casts (maybe implicit casts will be OK).
			// case V_SgCastExp: localResult = true; break;

			// What we hand in (root of event) is a SgFunctionDefinition, so we can force this to fail.
			// case V_SgFunctionDefinition: localResult = true; break;

			// This sort of statement IS OK (but we don't need to specify everything that is acceptable).
		case V_SgExprStatement: localResult = false;
			break;

		default:
		{
			// Nothing to do here!
		}


	}

	// Test for acceptable sorts of value expressions...
	SgValueExp* valueExp = isSgValueExp(node);
	if (valueExp != NULL)
	{
		SgIntVal* integerValueExp = isSgIntVal(valueExp);

		// If this is NOT and integer value expression then we don't allow it
		if (integerValueExp == NULL)
		{
			printf("Failing type test for value expression valueExp = %p = %s \n", valueExp, valueExp->class_name().c_str());
			localResult = true;
		}
	}

	if (localResult == true)
	{
		SgLocatedNode* locatedNode = isSgLocatedNode(node);
		if (locatedNode != NULL)
		{
			// Note that this message will be repeated for each IR node in the AST from the filing node to the root of the event.
			// But we will make the failing event function more clearly.
			Sg_File_Info* fileInfo = locatedNode->get_file_info();
			SgFunctionDefinition* failingEventFunctionDefinition = isSgFunctionDefinition(node);
			if (failingEventFunctionDefinition != NULL)
			{
				SgFunctionDeclaration* functionDeclaration = failingEventFunctionDefinition->get_declaration();
				ROSE_ASSERT(functionDeclaration != NULL);
				printf("Failing event function = %s: failure on line %d \n", functionDeclaration->get_name().str(), fileInfo->get_line());
			}
			else
			{
				printf("AST node = %p = %s: failure on line %d column = %d \n", node, node->class_name().c_str(), fileInfo->get_line(), fileInfo->get_col());
			}
		}
	}

	return localResult;
}


bool LanguageRestrictions::violatesRestrictionsOnEventFunctions(SgFunctionDefinition* functionDefinition)
{
	// Build the traversal object
	RestrictionTraversal restrictionTraversal;

	// Call the traversal starting at the project node of the AST
	bool failure = false; //restrictionTraversal.traverse(functionDefinition);

	failure = failure || containsDeclarationsOfPointerVariables(functionDefinition);
	failure = failure || takesArgumentsByReference(functionDefinition);
	failure = failure || containsJumpStructures(functionDefinition);
	failure = failure || containsExceptionHandling(functionDefinition);
	failure = failure || usesFunctionPointersOrVirtualFunctions(functionDefinition);

	return failure;
}


bool LanguageRestrictions::violatesRestrictionsOnEventFunctions(vector<SgFunctionDefinition*> eventList)
{
	bool returnValue = false;
	for (size_t i = 0; i < eventList.size(); i++)
	{
		// Output the name of the function being evaluated...
		string functionName = eventList[i]->get_declaration()->get_name();
		printf("Evaluate function %s using language restriction tests \n", functionName.c_str());

		// Check restrictions
		bool failed = violatesRestrictionsOnEventFunctions(eventList[i]);

		// Now test if the event function passed or failed...
		if (failed == false)
		{
			printf("Function %s PASSED language restriction tests \n", eventList[i]->get_declaration()->get_name().str());
		}
		else
		{
			printf("Function %s FAILED language restriction tests \n", eventList[i]->get_declaration()->get_name().str());

#if 0
			printf("Exiting due to failure to pass language restriction tests \n");
			ROSE_ASSERT(true);
#endif
		}

		returnValue = returnValue || failed;
	}

	return returnValue;
}


/** Returns true if the given function declares any local variables that are of a pointer or array type. */
bool LanguageRestrictions::containsDeclarationsOfPointerVariables(SgFunctionDefinition* functionDefinition)
{
	Rose_STL_Container<SgNode*> localDeclarations = NodeQuery::querySubTree(functionDefinition, V_SgInitializedName);
	foreach (SgNode* initNameNode, localDeclarations)
	{
		SgInitializedName* initializedName = isSgInitializedName(initNameNode);
		SgType* varType = initializedName->get_type();

		if (isSgPointerType(varType) || isSgArrayType(varType))
		{
			return true;
		}
	}

	return false;
}


/** Returns true if any of the arguments of the given function are passed by reference. */
bool LanguageRestrictions::takesArgumentsByReference(SgFunctionDefinition* functionDefinition)
{
	SgFunctionDeclaration* declaration = functionDefinition->get_declaration();

	foreach (SgInitializedName* arg, declaration->get_args())
	{
		SgType* argType = arg->get_type();
		if (SageInterface::isReferenceType(argType))
		{
			return true;
		}
	}

	return false;
}


/** Returns true if the function contains continue, goto, or break. The break statement is allowed inside
  * switch statements. */
bool LanguageRestrictions::containsJumpStructures(SgFunctionDefinition* functionDefinition)
{
	class JumpStructuresTraversal : public AstTopDownProcessing<bool>
	{
	public:
		bool evaluateInheritedAttribute(SgNode* astNode, bool isInsideSwitchStatement)
		{
			if (isSgSwitchStatement(astNode))
			{
				return true;
			}

			//A nested loop inside a switch statement no longer counts as being inside a switch statement
			if (isSgForStatement(astNode) || isSgDoWhileStmt(astNode) || isSgWhileStmt(astNode))
			{
				return false;
			}

			if (isSgGotoStatement(astNode) || isSgContinueStmt(astNode))
			{
				containsJumpStructures = true;
			}
			else if (isSgBreakStmt(astNode) && !isInsideSwitchStatement)
			{
				containsJumpStructures = true;
			}

			return isInsideSwitchStatement;
		}

		bool containsJumpStructures;
	};

	JumpStructuresTraversal traversal;
	traversal.containsJumpStructures = false;
	traversal.traverse(functionDefinition->get_body(), false);

	return traversal.containsJumpStructures;
}

/** Returns true if the function has any throw or catch statements. */
bool LanguageRestrictions::containsExceptionHandling(SgFunctionDefinition* functionDefinition)
{
	Rose_STL_Container<SgNode*> badNodes = NodeQuery::querySubTree(functionDefinition->get_body(), V_SgThrowOp);
	if (badNodes.size() > 0)
	{
		return true;
	}

	badNodes = NodeQuery::querySubTree(functionDefinition->get_body(), V_SgCatchStatementSeq);
	if (badNodes.size() > 0)
	{
		return true;
	}

	return false;
}


/** Returns true if the given function calls any other functions through function pointers
  * or through virtual functions. */
bool LanguageRestrictions::usesFunctionPointersOrVirtualFunctions(SgFunctionDefinition* functionDefinition)
{
	Rose_STL_Container<SgNode*> functionCalls = NodeQuery::querySubTree(functionDefinition->get_body(), V_SgFunctionCallExp);

	foreach (SgNode* functionCallNode, functionCalls)
	{
		SgFunctionCallExp* functionCall = isSgFunctionCallExp(functionCallNode);

		SgFunctionDeclaration* functionDeclaration = functionCall->getAssociatedFunctionDeclaration();

		if (functionDeclaration == NULL)
		{
			return true;
		}

		//Check if the function is virtual
		SgFunctionModifier& functionModifier = functionDeclaration->get_functionModifier();
		if (functionModifier.isVirtual() || functionModifier.isPureVirtual())
		{
			return true;
		}
	}

	return false;
}
