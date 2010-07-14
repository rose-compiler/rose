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
#include "utilities/CPPDefinesAndNamespaces.h"
#include "utilities/Utilities.h"


using namespace std;
using namespace SageBuilder;
using namespace SageInterface;


bool LanguageRestrictions::violatesRestrictionsOnEventFunctions(SgFunctionDefinition* functionDefinition)
{
	// Call the traversal starting at the project node of the AST
	bool failure = false; 

	failure = failure || containsDeclarationsOfPointerVariables(functionDefinition);
	failure = failure || containsJumpStructures(functionDefinition);
	failure = failure || containsExceptionHandling(functionDefinition);
	failure = failure || usesFunctionPointersOrVirtualFunctions(functionDefinition);
	failure = failure || usesArrays(functionDefinition);
	failure = failure || variablesOfPointerTypesAreAssigned(functionDefinition);
	failure = failure || dynamicMemoryAllocationUsed(functionDefinition);
	failure = failure || hasVariableArguments(functionDefinition);
	failure = failure || usesBannedTypes(functionDefinition);
	failure = failure || usesGnuExtensions(functionDefinition);
	failure = failure || returnsBeforeFunctionEnd(functionDefinition);

	return failure;
}

bool LanguageRestrictions::violatesRestrictionsOnEventFunctions(SgFunctionDeclaration* functionDeclaration)
{
	functionDeclaration = isSgFunctionDeclaration(functionDeclaration->get_definingDeclaration());

	if (functionDeclaration == NULL || functionDeclaration->get_definition() == NULL)
	{
		//We can't verify that a function passes if we don't have its body
		return true;
	}

	return violatesRestrictionsOnEventFunctions(functionDeclaration->get_definition());
}


bool LanguageRestrictions::violatesRestrictionsOnEventFunctions(vector<SgFunctionDefinition*> eventList)
{
	bool returnValue = false;
	for (size_t i = 0; i < eventList.size(); i++)
	{
		// Output the name of the function being evaluated...
		string functionName = eventList[i]->get_declaration()->get_name();

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


/** Returns true if the function accesses an array or a pointer through the bracket operator []. */
bool LanguageRestrictions::usesArrays(SgFunctionDefinition* functionDefinition)
{
	Rose_STL_Container<SgNode*> arrayBracketReferences = NodeQuery::querySubTree(functionDefinition->get_body(), V_SgPntrArrRefExp);

	//TODO: use some sort of analysis to decide wether the array is written to or read to. Only ban writing to arrays;
	//constant lookup tables are Ok.
	if (arrayBracketReferences.size() > 0)
	{
		return true;
	}

	return false;
}


/** Returns true if any variable of a pointer type (possibly passed as an argument) is written
  * in the function. Note that this refers to the pointer value itself, not to the object it's pointing to. */
bool LanguageRestrictions::variablesOfPointerTypesAreAssigned(SgFunctionDefinition* functionDefinition)
{
	vector<SgNode*> readRefs;
	vector<SgNode*> writeRefs;
	SageInterface::collectReadWriteRefs(functionDefinition->get_body(), readRefs, writeRefs);

	foreach (SgNode* writeRefNode, writeRefs)
	{
		SgExpression* writeExpression = isSgExpression(writeRefNode);
		if (writeExpression == NULL)
		{
			continue;
		}

		SgType* writeVarType = writeExpression->get_type();
		if (isSgPointerType(writeVarType) || isSgArrayType(writeVarType))
		{
			backstroke_util::printCompilerError(writeRefNode, "An expression of pointer type is written to.");
			return true;
		}
	}

	return false;
}



/** Returns true if the function uses C++-style dynamic memory allocation. C-style allocation
  * is more difficult to detect because it is a library function and not a language feature. */
bool LanguageRestrictions::dynamicMemoryAllocationUsed(SgFunctionDefinition* functionDefinition)
{
	Rose_STL_Container<SgNode*> newExps = NodeQuery::querySubTree(functionDefinition->get_body(), V_SgNewExp);

	foreach (SgNode* newExpr, newExps)
	{
		backstroke_util::printCompilerError(newExpr, "Dynamic memory allocation not allowed");
	}

	return newExps.size() > 0;
}


/** Returns true if the function takes a variable number of arguments. */
bool LanguageRestrictions::hasVariableArguments(SgFunctionDefinition* functionDefinition)
{
	SgFunctionDeclaration* declaration = functionDefinition->get_declaration();

	foreach (SgInitializedName* arg, declaration->get_args())
	{
		SgType* argType = arg->get_type();
		if (isSgTypeEllipse(argType))
		{
			backstroke_util::printCompilerError(declaration, "Ellipses arguments not allowed.");
			return true;
		}
	}

	return false;
}


/** Returns true if the function uses any type that is not a scalar or a struct strictly containing scalars.*/
bool LanguageRestrictions::usesBannedTypes(SgFunctionDefinition* functionDefinition)
{
	//First, let's collect all the types used in this function
	map<SgType*, SgNode*> types;

	//Types of variables declared in the function
	Rose_STL_Container<SgNode*> initializedNames = NodeQuery::querySubTree(functionDefinition->get_body(), V_SgInitializedName);
	foreach (SgNode* initializedNameNode, initializedNames)
	{
		types[isSgInitializedName(initializedNameNode)->get_type()] = initializedNameNode;
	}

	//Return types of functions called
	Rose_STL_Container<SgNode*> functionCalls = NodeQuery::querySubTree(functionDefinition->get_body(), V_SgFunctionCallExp);
	foreach (SgNode* functionCallExp, functionCalls)
	{
		types[isSgFunctionCallExp(functionCallExp)->get_type()] = functionCallExp;
	}

	//Types of the arguments.
	foreach (SgInitializedName* arg, functionDefinition->get_declaration()->get_args())
	{
		SgType* argType = arg->get_type();

		//Pointer types are allowed as arguments.
		if (isSgPointerType(argType))
		{
			types[isSgPointerType(argType)->get_base_type()] = arg;
		}
		else
		{
			types[argType] = arg;
		}
	}

	//Return type of the function
	types[functionDefinition->get_declaration()->get_type()->get_return_type()] = functionDefinition->get_declaration();

	//Ok, now we have a list of types used in the function. We have to check them
	pair<SgType*,SgNode*> typeUsePair;
	foreach (typeUsePair, types)
	{
		SgType* type = typeUsePair.first;
		SgNode* nodeWhereTypeIsUsed = typeUsePair.second;

		if (SageInterface::isScalarType(type))
		{
			continue;
		}
		else if (isSgClassType(type))
		{
			SgClassDeclaration* classDeclaration = isSgClassDeclaration(isSgClassType(type)->get_declaration());
			ROSE_ASSERT(classDeclaration != NULL);
			classDeclaration = isSgClassDeclaration(classDeclaration->get_definingDeclaration());
			if (classDeclaration == NULL)
			{
				backstroke_util::printCompilerError(nodeWhereTypeIsUsed, "Could not find definition for the given type. A full definition is needed"
						" in order to check constraints.");
				return true;
			}
			SgClassDefinition* classDefinition = classDeclaration->get_definition();
			ROSE_ASSERT(classDefinition != NULL);

			//Check each member variable of the class to make sure it is a primitive type
			foreach (SgDeclarationStatement* classMember, classDefinition->get_members())
			{
				if (!isSgVariableDeclaration(classMember))
				{
					continue;
				}

				SgVariableDeclaration* memberVariableDeclaration = isSgVariableDeclaration(classMember);
				foreach (SgInitializedName* memberVariable, memberVariableDeclaration->get_variables())
				{
					if (!SageInterface::isScalarType(memberVariable->get_type()))
					{
						backstroke_util::printCompilerError(memberVariableDeclaration, "Classes/Structs used inside the event function can"
								" only contain scalar types");
						return true;
					}
				}
			}
		}
		else
		{
			backstroke_util::printCompilerError(nodeWhereTypeIsUsed, "Only primitive types and classes/structs containing primitive types are allowed");
			return true;
		}
	}

	return false;
}


/** Returns true if the function uses any syntax which is not part of the C++ standard, but the GNU extension.*/
bool LanguageRestrictions::usesGnuExtensions(SgFunctionDefinition* functionDefinition)
{
	// Forbid use of statement expression.
	vector<SgExpression*> all_exps = backstroke_util::querySubTree<SgExpression> (functionDefinition->get_body());

	foreach(SgExpression* exp, all_exps)
	{
		if (isSgStatementExpression(exp))
		{
			backstroke_util::printCompilerError(exp, "gcc-style statement expressions are not allowed.");
			return true;
		}
	}

	return false;
}


/** True if the function has return statements before the bottom of the body. */
bool LanguageRestrictions::returnsBeforeFunctionEnd(SgFunctionDefinition* functionDefinition)
{
	vector<SgReturnStmt*> returnStatements = SageInterface::querySubTree<SgReturnStmt>(functionDefinition, V_SgReturnStmt);

	SgBasicBlock* body = functionDefinition->get_body();
	SgStatement* lastStatementInBody = body->get_statements().empty() ? NULL : body->get_statements().back();

	foreach (SgReturnStmt* returnStatement, returnStatements)
	{
		if (returnStatement != lastStatementInBody)
		{
			backstroke_util::printCompilerError(returnStatement, "return statements are only allowed at the end of a function");
			return true;
		}
	}

	return false;
}
