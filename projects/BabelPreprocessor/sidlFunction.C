
// We need this header file so that we can use the mechanisms within ROSE to build a preprocessor
#include "rose.h"
#include <set>


// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

// We need this header file so that we can use the global tree traversal mechanism
// #include "Cxx_GrammarTreeTraversalClass.h"

// Main transfromation mechanism
#include "babelTransformation.h"


	string
SIDL_TreeTraversal::stringifyOperatorWithoutSymbols (string name)
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

	// function names that are not C++ operators are returned unmodified
	bool isAnOperator = true;

	// Now turn the last character code into a string (we want to return a string instead of a character!)
	if (secondLastCharacter == 'r')
	{
		// Case of an operator ending with a ONE character specifier
		switch (lastCharacter)
		{
			case '=': operatorString = "Equals"; break;
			case '!': operatorString = "Not"; break;
			case '<': operatorString = "LessThan"; break;
			case '>': operatorString = "GreaterThan"; break;
			case '+': operatorString = "Plus"; break;
			case '-': operatorString = "Minus"; break;
			case '*':
					// Both the multiplication operator and the reference operator return "*"
					operatorString = "Multiply";
					break;
			case '/': operatorString = "Divide"; break;
			case '%': operatorString = "Mod"; break;
			case '&': 
					// Both the bitwise AND operator and the address operator return "&"
					operatorString = "BitwiseAnd";
					break;
			case '|': operatorString = "BitwiseOr"; break;
			case '^': operatorString = "BitwiseXor"; break;
			default:
					isAnOperator = false;
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
						case 'r': operatorString = "Equals";  break;
						case '=': operatorString = "BooleanEquality"; break;
						case '!': operatorString = "BooleanInequality"; break;
						case '<':  // distinguish between <= and <<=
								if (thirdLastCharacter == '<')
									operatorString = "LShiftEquals";
								else
								{
									ROSE_ASSERT (thirdLastCharacter == 'r');
									operatorString = "LessThanOrEquals";
								}
								break;
						case '>':  // distinguish between >= and >>=
								if (thirdLastCharacter == '>')
									operatorString = "RshiftEquals";
								else
								{
									ROSE_ASSERT (thirdLastCharacter == 'r');
									operatorString = "GreaterThanOrEquals";
								}
								break;
						case '+': operatorString = "PlusEquals"; break;
						case '-': operatorString = "MinusEquals"; break;
						case '*': operatorString = "MultiplyEquals"; break;
						case '/': operatorString = "DivideEquals"; break;
						case '%': operatorString = "ModuloEquals"; break;
						case '&': operatorString = "BitwiseAndEquals"; break;
						case '|': operatorString = "BitwiseOrEquals"; break;
						case '^': operatorString = "BitwiseXorEquals"; break;
						default:
								isAnOperator = false;
					}
					break;
				}
			case '&':
				{
					ROSE_ASSERT (secondLastCharacter == '&');
					operatorString = "BooleanAnd";
					break;
				}
			case '|':
				{
					ROSE_ASSERT (secondLastCharacter == '|');
					operatorString = "BooleanOr";
					break;
				}
			case '<':
				{
					ROSE_ASSERT (secondLastCharacter == '<');
					operatorString = "LShift";
					break;
				}
			case '>':
				{
					ROSE_ASSERT (secondLastCharacter == '>');
					operatorString = "Rshift";
					break;
				}
			case ')':
				{
					ROSE_ASSERT (secondLastCharacter == '(');
					operatorString = "Parenthesis";
					break;
				}
			case ']':
				{
					ROSE_ASSERT (secondLastCharacter == '[');
					operatorString = "Bracket";
					break;
				}
			case '+':
				{
					ROSE_ASSERT (secondLastCharacter == '+');
					operatorString = "Increment";
					break;
				}
			case '-':
				{
					ROSE_ASSERT (secondLastCharacter == '-');
					operatorString = "Decrement";
					break;
				}
			default:
				isAnOperator = false;
		}
	}

	ROSE_ASSERT (operatorString.c_str() != NULL);

	// Copy the string to avoid returning a reference to local (stack) memory
	string returnString;

	if (isAnOperator == true)
		returnString = "operator" + operatorString;
	else
		returnString = name;

	// printf ("TransformationSupport::stringifyOperatorWithoutSymbols() returnString = %s \n",returnString.c_str());

	return returnString;
}


//FIXME: remove this once we're sure that the order doesn't matter
/*class overloadInformation{
	int order;
	int count;

	vector<SgType*> importantTypes;
	
	public:
	overloadInformation(int a, int b)
	{
		order = a;
		count = b;
	}
 	
	void setTypes(vector<SgType*> a)
	{
		importantTypes.insert(importantTypes.end(),a.begin(),a.end());
	}

	int get_order(void)
	{
		return order;
	}

};
*/


overloadInformation
SIDL_TreeTraversal::isOverloaded ( SgClassDefinition* classDefinition, string functionName, string mangledFunctionName )
{
	int orderOfOverloadedFunction = 0;

	SgDeclarationStatementPtrList & memberDeclarationList = classDefinition->get_members();
	SgDeclarationStatementPtrList::iterator i;

	bool foundMangledFunctionName = false;
	int numberOfOverloadedFunctionNames = 0;

	list<SgFunctionDeclaration*> matches;
	
	for (i = memberDeclarationList.begin(); i != memberDeclarationList.end(); i++)
	{	// Loop through the member declarations
		
		
		SgFunctionDeclaration* const functionDeclaration = isSgFunctionDeclaration(*i);
		if ( functionDeclaration != NULL )
		{
			
			string thisFunctionName = functionDeclaration->get_name().str();
			//count each function with the same name
			if ( thisFunctionName == functionName )
			{
				numberOfOverloadedFunctionNames++;
				matches.push_back(functionDeclaration);
			}
			
			// This causes the redundent call to generate the mangled function name
			string thisFunctionMangledName = functionDeclaration->get_mangled_name().str();
			//the "order" is set when we reach the exact instance of the function with the desired name
			if ( thisFunctionMangledName == mangledFunctionName )
			{
				foundMangledFunctionName = true;
				orderOfOverloadedFunction = numberOfOverloadedFunctionNames;
			}

			
		}
	}

	// Make sure that we found the input function and not just an overloaded version of it
	ROSE_ASSERT (foundMangledFunctionName == true);

	orderOfOverloadedFunction--; //make it zero indexed
	overloadInformation information(orderOfOverloadedFunction, numberOfOverloadedFunctionNames);
	
	//determine which arguments differ
	if(numberOfOverloadedFunctionNames > 1)
	{
// #ifndef __INTEL_COMPILER
#if !defined(__INTEL_COMPILER) && !defined(USE_ROSE)
		// DQ (6/26/2006): The declaration of arguments uses a GNU extension when we can not support on other compilers.
		vector<SgType*> arguments[numberOfOverloadedFunctionNames];
		
	
		//build a matrix of the arguments
		list<SgFunctionDeclaration*>::iterator funcIter; int j = 0;
		for(funcIter = matches.begin(); funcIter != matches.end(); funcIter++, j++)
		{
			SgInitializedNamePtrList &args = (*funcIter)->get_args();
			for(SgInitializedNamePtrList::iterator nameIter = args.begin(); nameIter != args.end(); nameIter++)
			{
				SgType *T = (*nameIter)->get_type();
				arguments[j].push_back(T);
			}
		}

		//cull out the arguments that are the same	
		unsigned int k = 0;
		int numLeft;
		do{
			SgType *T = NULL;
			SgType *type = NULL;
			int numEqual = 0;
			numLeft = numberOfOverloadedFunctionNames; 
			for(int j = 0; j < numberOfOverloadedFunctionNames; j++)
				if(k < arguments[j].size()) 	
				{
					//the type for the current argument
					type = arguments[j][k]; 
					
					//T is the type carried by the loop
					if(T == NULL) T = type;
					if(type == T) numEqual++;
				}
				else numLeft--;

			//if every declaration has the same type for the Jth argument, that argument should be removed from consideration
			//if the declaration doesn't have a Jth argument, we can ignore that declaration
			if(numEqual == numberOfOverloadedFunctionNames) 
			//FIXME need to find a way to handle a case like foo(int) foo(int, int, int) foo(int, int ,float) where the latter are distinguishable
			//even if all three use int, we should be able to strip off the first one. We also need to be able to handle say foo(a,a,a) and foo(b,b,b)
			{
				for(int j = 0; j < numberOfOverloadedFunctionNames; j++)
					if(k < arguments[j].size())	arguments[j].erase(arguments[j].begin()+k);
			}			
			else k++; 	//if they aren't equal, we move forward
			
		}while(numLeft);	 //we loop until we have passed the end of all declarations
		
		information.setTypes(arguments[orderOfOverloadedFunction]);
#else
   // DQ (6/26/2006): The declaration of arguments uses a GNU extension when we can not support on other compilers.
      printf ("Error: Code constructs not supported using Intel C++ Compiler \n");
      ROSE_ASSERT(false);
#endif
	}

//	return orderOfOverloadedFunction;
	return information;
}

static string
numberToOverloadString(int num)
{
	string result;
	char remainder;
	while (num > 0) {
		remainder = ((char)(num % 26) + 'A'); // 
		num /= 26;
		result += remainder;
	}
	return result;
}

static string
sidlOverloadExtension(const string &str)
{
  static set<string> keywords;
  static const char * const s_keywords[] = {
    "abstract",
    "array",
    "bool",
    "char",
    "copy",
    "dcomplex",
    "double",
    "extends",
    "fcomplex",
    "final"
    "float",
    "implements",
    "implements-all"
    "import",
    "in",
    "inout",
    "int",
    "interface",
    "local",
    "long",
    "oneway",
    "opaque",
    "out",
    "package",
    "rarray",
    "require",
    "static",
    "string",
    "throws",
    "version",
    "void",
    NULL
  };
  if (0 == keywords.size()) {
    const char * const *key = s_keywords;
    while (*key) {
      keywords.insert(string(*key));
      ++key;
    }
  }
  if ((str.length() == 0) || (keywords.find(str) == keywords.end()))  return str;
  string result = str;
  result[0] = toupper(result[0]);
  return result;
}

string
SIDL_TreeTraversal::generateSIDLFunctionDeclaration(SgFunctionDeclaration* functionDeclarationStatement )
{
	ROSE_ASSERT (functionDeclarationStatement != NULL);
	ROSE_ASSERT (functionDeclarationStatement->get_file_info() != NULL);
	const SgSpecialFunctionModifier &functionModifier = functionDeclarationStatement->get_specialFunctionModifier();

	string functionName = functionDeclarationStatement->get_name().str();
	string sidlFunctionName ;
	if (functionModifier.isConstructor()) {
		if (functionDeclarationStatement->get_args().size() == 0) return ""; // skip empty constructor
                sidlFunctionName = constructorName;
	}
	else{
                sidlFunctionName = functionName;
	}

	// We have to force the mangled name to be generated before we access it (else we just get "defaultName")
	string mangledFunctionName = functionDeclarationStatement->get_mangled_name().str();

	sidlFunctionName = stringifyOperatorWithoutSymbols(sidlFunctionName);

	// Get the class name
	SgClassDefinition* classDefinition   = isSgClassDefinition(functionDeclarationStatement->get_scope());

	// DQ (1/7/2004): Modified for make EDG version 3.3 work (member function declarations's normalized by EDG)
	if (classDefinition != NULL)
	{
		SgClassDeclaration* classDeclaration = classDefinition->get_declaration();

		string className = classDeclaration->get_name().str();

		overloadInformation info = isOverloaded(classDefinition,functionName,mangledFunctionName);
		int orderofOverloadedFunction = info.get_order();
			
		// If function is overloaded then append the number indicating the order of appearance in the
		// class declaration
		if (info.get_count() > 1)
		{
			vector<SgType*> types = info.get_types();

		// SgInitializedNamePtrList &args = functionDeclarationStatement->get_args ();
			int size = types.size();
			if(size > 0)
			{
				if(size < 3)
				{
					sidlFunctionName += "[";
					for(vector<SgType*>::iterator i = types.begin(); i!= types.end(); i++)
					{
						if(i != types.begin()) sidlFunctionName += "_";
						if(isSgPointerType(*i) != NULL)	sidlFunctionName += "P";
						sidlFunctionName += sidlOverloadExtension(TransformationSupport::getTypeName(*i));
					}
					sidlFunctionName += "]";
				}
				else
					sidlFunctionName += "["+numberToOverloadString(orderofOverloadedFunction)+"]";
			}
		}
	}
	else
	{
		printf ("EDG version 3.3 can return a null pointer to the member function definition \n");
	}

     SgFunctionType* functionType = functionDeclarationStatement->get_type();
     ROSE_ASSERT(functionType != NULL);

  // SgType* returnType = functionType->get_return_type();
  // ROSE_ASSERT (returnType != NULL);
  // string returnTypeName = TransformationSupport::getTypeName(returnType);

  // printf ("function has_ellipses %s \n",(functionType->get_has_ellipses() != false) ? "true" : "false");
  // showSgFunctionType(cout, functionType, "Called from generateSIDLFunctionDeclaration", 0 );
  // printf ("Function return type = %s \n",returnTypeName.c_str());

#if 0
	SgTypePtrList & argumentTypeList = functionType->get_arguments();
	ROSE_ASSERT (argumentTypeList.size() >= 0);
	SgTypePtrList::iterator argumentIterator = argumentTypeList.begin();

	for (argumentIterator = argumentTypeList.begin(); argumentIterator != argumentTypeList.end(); argumentIterator++)
	{
		// showSgType(os,(*argumentIterator), label, depth+1);

		string argumentTypeName = TransformationSupport::getTypeName(*argumentIterator);
		printf ("-----> argument #%d  argumentTypeName = %s \n",argumentCounter++,argumentTypeName.c_str());
	}
#endif
	
	//Determine the SIDL parameter passing mechanism (in,out,inout)
	SgInitializedNamePtrList & argumentList = functionDeclarationStatement->get_args();
	string parameterTypesAndNames;
	SgInitializedNamePtrList::iterator i;

	unsigned int argumentCounter = 0;
	for (i = argumentList.begin(); i != argumentList.end(); i++)
	{
		SgType* type = (*i)->get_type();
		ROSE_ASSERT (type != NULL);

		string typeName = TransformationSupport::getTypeName(type);
		ROSE_ASSERT (typeName.c_str() != NULL);
		
		string sidlParameterPassingMechanim = "in";
		
		//it seems like the has_ellipses value is wrong, so we'll set it
		functionType->set_has_ellipses(false);
		if(type->variantT() == V_SgTypeEllipse)
		{
			sidlParameterPassingMechanim = "inout";
			functionType->set_has_ellipses(true);
		}
		//else if (type->variantT() == V_SgTypeVoid)
		/*else if (ROSE::stringDuplicate(type->sage_class_name()) == "SgTypeVoid")
		{
			printf("found a void\n");
			//void type is only viable for a pointer.  foo(void) will just become foo()
			if(isSgPointerType(type) != NULL)
			{
				printf("found a void pointer\n");
				sidlParameterPassingMechanim ="inout opaque";
			}	
		}*/
		else if (isSgReferenceType(type) != NULL)
		{
			sidlParameterPassingMechanim = "inout";
		}
		else if (isSgPointerType(type) != NULL)
		{
			sidlParameterPassingMechanim = "inout";
		}
		else if (isSgArrayType(type) != NULL)
		{
			SgArrayType array = isSgArrayType(type);
			sidlParameterPassingMechanim = "inout Array<";
			SgType* baseType = array.get_base_type();
			sidlParameterPassingMechanim += TransformationSupport::getTypeName(baseType);
			sidlParameterPassingMechanim += ",1>";
			//FIXME: I don't see a way to determine the dimention of the array
		}
	
		

		// Build the substring for each parameter
		
		parameterTypesAndNames += sidlParameterPassingMechanim;
		parameterTypesAndNames += " ";
		
		//if(type->variantT() != V_SgTypeGlobalVoid)
		//{
			if(type->variantT() == V_SgTypeEllipse)
			{
				parameterTypesAndNames += "Array<BabelBaseType,1> "; //FIXME: need to include a declaration for BaseType
				parameterTypesAndNames += "elips" +  argumentCounter; //this fails to actually append the counter, but I don't think it will matter: kmk
			}
			else
			{
				SgName name = (*i)->get_name();
				string nameString = name.str();
				string typeName = TransformationSupport::getTypeName(type);
				
				if(typeName == "void")
				{
					if(nameString!="")
					{
						parameterTypesAndNames += "opaque ";
						parameterTypesAndNames += nameString;
					}
				}
				else
				{
					parameterTypesAndNames += typeName;
					parameterTypesAndNames += " ";
					if(nameString != "") //will be empty if the function declaration doesn't provide a name
						parameterTypesAndNames += nameString;
				}
			}

			// Add a "," to the string if there are more parameters in the list
			if ( argumentCounter < argumentList.size()-1 )
				parameterTypesAndNames += ",";
		//}else printf("avoiding the void\n");
		argumentCounter++;
	}

	SgType* returnType = functionType->get_return_type();
	ROSE_ASSERT (returnType != NULL);
	string returnTypeName = "void";
	if(returnType->variantT() != V_SgTypeVoid)
		returnTypeName = TransformationSupport::getTypeName(returnType);

	
	string sidlMemberFunctionDeclaration = "          $RETURN_TYPE $FUNCTION_NAME($PARAMETERS);\n";

	sidlMemberFunctionDeclaration = StringUtility::copyEdit ( sidlMemberFunctionDeclaration, "$RETURN_TYPE" , returnTypeName );
	sidlMemberFunctionDeclaration = StringUtility::copyEdit ( sidlMemberFunctionDeclaration, "$FUNCTION_NAME" , sidlFunctionName );
	sidlMemberFunctionDeclaration = StringUtility::copyEdit ( sidlMemberFunctionDeclaration, "$PARAMETERS" , parameterTypesAndNames );

	
	return sidlMemberFunctionDeclaration;
}

