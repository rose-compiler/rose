
// We need this header file so that we can use the mechanisms within ROSE to build a preprocessor
#include "rose.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;

// Main transfromation mechanism
#include "babelTransformation.h"
#include <map>
#include "caselessCompare.h"


// This might be handled better in the future (without using a global variable)
// This is required as a way to hand the SgProject to the prependSourceCode, 
// appendSourceCode, replaceSourceCode, and insertSourceCode functions.
SgProject* globalProject = NULL;
extern bool BTN;

// Functions required by the global tree traversal mechanism
string
SIDL_TreeTraversal::getPackageName( list<OptionDeclaration> OptionList )
{
	string returnString = "defaultPackageName";
	list<OptionDeclaration>::iterator i;

	for (i = OptionList.begin(); i != OptionList.end(); i++)
	{
		// printf ("In getPackageName(): (*i).optionName = %s (*i).optionName = %s \n",(*i).optionName,(*i).getValue());
		if ( string((*i).optionName) == string("PACKAGE") )
		{
			// printf ("Found a package option in the optionList: value = %s \n",(*i).getValue());
			returnString = (*i).getValue();
		}
	}

#if 0
	printf ("At base of getPackageName() (exiting ...) \n");
	ROSE_ABORT();
#endif

	return returnString;
}

string
SIDL_TreeTraversal::getVersionNumber( list<OptionDeclaration> OptionList )
{
	// This function returns the version number (as a string) specificed using the "hint" machanism
	// within the user's header files.

	// Declare return string with a defult initializer
	string returnString = "1.0";

	// Traverse the list of options looking for the "VERSION" option
	for (list<OptionDeclaration>::iterator i = OptionList.begin(); i != OptionList.end(); i++)
	{
		// Note: we don't use string objects in the declaration interface of the option interface
		// because we want to reduce the footprint on the user's code (using only primative types).
		if ( string((*i).optionName) == string("VERSION") )
		{
			// get the string representing the version number
			returnString = (*i).valueString;
		}
	}

	return returnString;
}

static string
genNewName(string prefix,
	   int count)
{
  return prefix + '_' + StringUtility::numberToString(count);
}

BabelTransformationSynthesizedAttributeType
SIDL_TreeTraversal::evaluateSynthesizedAttribute ( SgNode* astNode, SubTreeSynthesizedAttributes synthesizedAttributeList )
{
	// This function is called by the global tree traversal mechanism (called on each node of the
	// AST!).  At present we use a member function of the transformation object. 

	// Build the synthesizied attribute that this node wil return
	BabelTransformationSynthesizedAttributeType returnSynthesizedAttribute;

	string identifingTypeName = "OptionDeclaration";
	list<OptionDeclaration> OptionList;
	TransformationSupport::getTransformationOptions ( astNode, OptionList, identifingTypeName );
	// printf ("##### OptionList.size() = %zu \n",OptionList.size());

#if 0
	printf ("$$$$$ TOP of attributeAssemblyFunction (astNode = %s) (synthesizedAttributeList.size() = %d) \n",
			astNode->sage_class_name(),synthesizedAttributeList.size());
#endif

	SgLocatedNode* locatedNode = isSgLocatedNode(astNode);

#if 0
	printf ("Processing Source Line %d \n",
			isSgLocatedNode(locatedNode) ? rose::getLineNumber(locatedNode) : -1 );
#endif

	// Need to handle all unary and binary operators and variables (but not much else)
	switch (astNode->variant())
	{
		case ProjectTag:
			{
				printf ("Found the SgProject where we can close off the generation of the SIDL files \n");
				ROSE_ABORT();
				break;
			}

		case FileTag:
			{
				//find the values for the version line of the SIDL file
				string sidlPackageName   = getPackageName(OptionList);
				string sidlVersionNumber = getVersionNumber(OptionList);
				string sidlClassDeclarations = synthesizedAttributeList[SgSourceFile_globalScope].getSIDLDeclarationString();

				//build the version line of the SIDL file
				string sourceCodeString = "\npackage $PACKAGE version $VERSION_NUMBER\n   { $CLASS_DECLARATIONS \n";
				sourceCodeString = StringUtility::copyEdit ( sourceCodeString, "$PACKAGE" , sidlPackageName );
				sourceCodeString = StringUtility::copyEdit ( sourceCodeString, "$VERSION_NUMBER" , sidlVersionNumber );
				sourceCodeString = StringUtility::copyEdit ( sourceCodeString, "$CLASS_DECLARATIONS" , sidlClassDeclarations );
				
				if(BTN)sourceCodeString += "\tclass BabelBaseType\n\t{\n\t}\n"; //FIXME: nothing is being done to ensure a unique class name
				
				sourceCodeString += "   }\n\n";

				
				SgFile* file = isSgFile(astNode);
				ROSE_ASSERT (file != NULL);

         // string fileNameWithPath    = rose::getFileName(file);
            string fileNameWithPath    = file->getFileName();
				string fileNameWithoutPath = rose::utility_stripPathFromFileName ( (char*) fileNameWithPath.c_str() );
				string pathOfFile          = rose::getPathFromFileName ( (char*) fileNameWithPath.c_str() );
				

				// printf ("fileNameWithPath    = %s \n",fileNameWithPath.c_str());
				// printf ("fileNameWithoutPath = %s \n",fileNameWithoutPath.c_str());
				// printf ("pathOfFile          = %s \n",pathOfFile.c_str());

				ROSE_ASSERT (fileNameWithoutPath.length() > 0);

				// Substitute the filename suffix (assumes the extention is a
				// single character).
				string sildFileNameWithoutPath = fileNameWithoutPath.substr(0,fileNameWithoutPath.length()-1) + "sidl";

#if 1			//print the SIDL code
				printf ("\n################## START OF %s #####################\n",sildFileNameWithoutPath.c_str());
				printf ("Output in %s file: \n",sildFileNameWithoutPath.c_str());
				printf ("%s\n",sourceCodeString.c_str()); //dump the entire SIDL source string to output
				printf (  "##################  END OF %s  #####################\n",sildFileNameWithoutPath.c_str());
#endif

				// Let the user know where to file the SIDL output file
				printf ("\nFinal SIDL output in file: %s \n\n",sildFileNameWithoutPath.c_str());

				StringUtility::writeFile( 
						sourceCodeString.c_str(),         //SIDL code
						sildFileNameWithoutPath.c_str(),  //file name
						"./" );                           //path

#if 0
				printf ("At base of case FileTag (exiting ...) \n");
				ROSE_ABORT();
#endif
				break;
			}

		case ENUM_DECL_STMT:
			{
				SgEnumDeclaration* enumDecl = isSgEnumDeclaration(astNode);
				map<string, int, CaseLessCompare> enumerands;
				ROSE_ASSERT(enumDecl != NULL);
				string enumDeclString = string("\n    enum ") + enumDecl->get_name().str() + " {\n         ";
				const SgInitializedNamePtrList& enumerators = enumDecl->get_enumerators();
				for(SgInitializedNamePtrList::const_iterator i = enumerators.begin(); i != enumerators.end() ; ) {
					const SgInitializedName* initializedName = *i;
					ROSE_ASSERT(initializedName != NULL);
					string enumerand = initializedName->get_name().str();
					if (enumerands[enumerand]++) {
					  int count = enumerands[enumerand];
					  string newName = genNewName(enumerand, count);
					  while (enumerands[newName]) {
					    newName = genNewName(enumerand, ++count);
					  }
					  enumerand = newName;
					  enumerands[enumerand] = 1;
					}
					enumDeclString += enumerand;
					if (++i != enumerators.end()) {
						enumDeclString += ",\n         ";
					}
				}
				enumDeclString += "\n    }\n";
				returnSynthesizedAttribute.addSIDLDeclarationString(enumDeclString);
				break;
			}


		case CLASS_DECL_STMT:
			{
				// Found the Class Declaration Statement where we expect most class declarations to be
				// found (ignoring nested classes at the moment).

				ROSE_ASSERT (locatedNode != NULL);

#if 0
				printf ("Found a member function declaration (BabelTransformation.C) line %d \n",
						rose::getLineNumber(locatedNode));
#endif

				SgClassDeclaration* classDeclaration = isSgClassDeclaration(astNode);
				ROSE_ASSERT (classDeclaration != NULL);

				string sidlClassDeclaration =
					"\n     class $CLASS_NAME\n        { \n$MEMBER_FUNCTION_DECLARATIONS        }\n";

				// printf ("sidlClassDeclaration = %s \n",sidlClassDeclaration.c_str());
				// string sidlMemberFunctionDeclarations = "          int get_x();\n          set_x(int i);";


				// If the class definition is in another file then the tree traversal mechanism does not
				// currenty traverse it and so the synthesizedAttributeList.size() == 0.
				// So currently we ignore these cases (but this will be fixed)
				if ( synthesizedAttributeList.size() > 0 )
				{
					ROSE_ASSERT (synthesizedAttributeList.size() > 0);
					string sidlMemberFunctionDeclarations = synthesizedAttributeList[SgClassDeclaration_definition].getSIDLDeclarationString();

					// printf ("In case CLASS_DECL_STMT: sidlMemberFunctionDeclarations = %s \n",sidlMemberFunctionDeclarations.c_str());

					sidlClassDeclaration = StringUtility::copyEdit ( sidlClassDeclaration, "$MEMBER_FUNCTION_DECLARATIONS", sidlMemberFunctionDeclarations );

					string className = classDeclaration->get_name().str();

					sidlClassDeclaration = StringUtility::copyEdit ( sidlClassDeclaration, "$CLASS_NAME", className );

					// printf ("sidlClassDeclaration = %s \n",sidlClassDeclaration.c_str());

					// Forward declarations should not generate a SIDL class declaration
					if ( !classDeclaration->isForward() )
					{
						returnSynthesizedAttribute.addSIDLDeclarationString(sidlClassDeclaration);
					}
					else
					{
						// printf ("This is a forward declaration! \n");
					}
				}

#if 0
				printf ("At base of case CLASS_DECL_STMT (exiting ...) \n");
				ROSE_ABORT();
#endif

				break;
			}

		case FUNC_DECL_STMT:
		case MFUNC_DECL_STMT:
			{
				// Treating all function declarations the same (member and non-member functions)
				// printf ("Found a function declaration (BabelTransformation.C) \n");

				SgFunctionDeclaration* functionDeclaration = isSgFunctionDeclaration(astNode);
				ROSE_ASSERT (functionDeclaration != NULL);
				if (functionDeclaration->get_definition()) break;
				const SgAccessModifier &accessModifier = functionDeclaration->get_declarationModifier().get_accessModifier();
				if (accessModifier.isPrivate()) break; // private methods don't belong in SIDL
				const SgSpecialFunctionModifier &functionModifier = functionDeclaration->get_specialFunctionModifier();
				if (functionModifier.isDestructor()) break;

				string sidlFunctionDeclaration = generateSIDLFunctionDeclaration(functionDeclaration);

				BTN = BTN || functionDeclaration->get_type()->get_has_ellipses();

				// printf ("sidlFunctionDeclaration = %s \n",sidlFunctionDeclaration.c_str());

				returnSynthesizedAttribute.addSIDLDeclarationString(sidlFunctionDeclaration);

#if 0
				printf ("At base of case MFUNC_DECL_STMT (exiting ...) \n");
				ROSE_ABORT();
#endif

				break;
			}

			// Most cases will use the default case to assemble attributes
		default:
			{
#if 0
				printf ("DEFAULT ASSEMBLY: default case in attributeAssemblyFunction() (sage_class_name = %s \n",
						astNode->sage_class_name());
#endif

				// The default is to concatinate all the strings together (in the order in which they appear)

				unsigned int i = 0;
				for (i=0; i < synthesizedAttributeList.size(); i++)
				{
#if 0
					printf ("In the DEFAULT CASE: SynthesizedAttributeListElements: attributeList = %s \n",
							synthesizedAttributeList[i].getSIDLDeclarationString().c_str());
#endif
					// Call the ArrayAssignmentStatementQuerySynthesizedAttributeType::operator+= overloaded operator
					returnSynthesizedAttribute += synthesizedAttributeList[i];
				}

				// ROSE_ABORT();
				break;
			}
	}

#if 0
	printf ("$$$$$ BOTTOM of attributeAssemblyFunction (astNode = %s) (declaration list size = %d) (returnSynthesizedAttribute.getTransformationSourceCode() = \n%s) \n",
			astNode->sage_class_name(),
			returnSynthesizedAttribute.variableDeclarationList.size(),
			returnSynthesizedAttribute.getSourceCodeString().c_str());
#endif

	return returnSynthesizedAttribute;
}
