// ROSE is a tool for building preprocessors, this file is an example preprocessor built with ROSE.

#include "rose.h"
#include "rewrite.h"
#include "roseQueryLib.h"

// DQ (1/1/2006): This is OK if not declared in a header file
using namespace std;
using namespace rose;

/*
   The purpose of this code is to generate the file "testlibGrammar.C" which
contains generated source code to recognize the abstractions found within the
test library.

   Here we consider abstractions to be language elements defined/declarated 
within the target library:
      1) functions and member functions
      2) global variables and member variables
      3) types and nested types

Below we have implemented the case of functions and member functions, first.

It should be possible to specify what collections of abstractions (represented
as what types of language elements, e.g.:
   a) function abstractions
        i. use of functions within an application from library
       ii. use of functions containing functions from the library
   b) variable use abstractions
        i. Use of variable declared in the library
       ii. Use of variables from objects whose types are defined in the library
   c) variable declaration abstractions
        i. declaration of abstractions types from the library
       ii. declaration of types derived from abstractions defined in the library
   d) 

The program analysis supplied in ROSE could be made to work on 
the attributes associated with the high-level abstractions.
Such a representation would permit a hierarchical representation of
an application's program analysis.

   This would not be well defined in the control flow unless
the library contained an abstraction represneting control flow
C++ does not make it easy to build this sort of abstraction
(though some libraries contain them, e.g. consider the A++/P++ 
"where" statement which is a library abstraction representing
a high-level abstraction for control flow (implemented using 
a macro and a for loop)).
 */

/*
One way to proceed:

    Handle overloaded operators separately from the 
    high-level abstractions (coloring).
    1) new set of enums to include overloaded operators
        a) query interface of representation of overloaded 
           functions as unary and binary operators
    2) generate functions for recognition of nodes in the AST
        a) general function
              SgNode* isNode("library",languageConstruct,astNode);
              SgNode* isLibraryNode(languageConstruct,astNode);
              SgNode* isLanguageConstructNode("library",astNode);
        b) specific functions
              isLibraryLanguageConstructNode(astNode);
   3) Introduction of ROSE node (higher level that SgNode)
        RoseNode
            RoseExpression
            RoseStatement
            RoseType
      which would contain interfaces for handling highlevel 
      details not included in the lower leve SAGE IR. Discuss 
      design with others.
 */

class AbstractionInfo
   {
  // Information for a library function or member function
     public:
          static string recognitionTemplateCode;
          string functionName;
          string typeName;

       // Keep a pointr to the SgNode to support additional queries
          SgNode* connectionToAST;

       // Use this to build a table of mapping from variants to 
       // strings containing the unparsed function prototypes
          string unparsedFunctionDeclaration;

          void   setFunctionName( string name ) { functionName = name; }
          string getFunctionName()            { return functionName; }

          void   setTypeName( string name ) { typeName = name; }
          string getTypeName()              { return typeName; }

          AbstractionInfo ( string inputTypeName, string inputFunctionName );

          string buildRecognitionCode(string libraryName);
          void   setFunctionPrototypeString ( string s );
          string getFunctionPrototypeString();
          void   normalizeNames();

          void setLinkToAST(SgNode* astNode) { connectionToAST = astNode; }
          SgNode* getLinkToAST()             { return connectionToAST; }
   };

// allocation of space for staticly defined data in AbstractionInfo class
string AbstractionInfo::recognitionTemplateCode;

void
AbstractionInfo::setFunctionPrototypeString ( string s )
   {
     unparsedFunctionDeclaration = s;
   }

string
AbstractionInfo::getFunctionPrototypeString()
   {
     return unparsedFunctionDeclaration;
   }

AbstractionInfo::AbstractionInfo ( string inputTypeName, string inputFunctionName )
   {
     typeName     = inputTypeName;
     functionName = inputFunctionName;

     if (recognitionTemplateCode.length() == 0)
        {
          recognitionTemplateCode = 
            // StringUtility::readFile("/home/dquinlan/ROSE/NEW_ROSE/Projects/HighLevelGrammars/GeneratedAbstractionRecognitionCode.code");
               StringUtility::readFile("/home/dquinlan/ROSE/NEW_ROSE/Projects/HighLevelGrammars/GeneratedAbstractionRecognitionCode.code");

#if 0
          printf ("recognitionTemplateCode = %s \n",recognitionTemplateCode.c_str());
          ROSE_ABORT();
#endif
        }
   }


string 
AbstractionInfo::buildRecognitionCode(string libraryName)
   {
  // Using the template string this function generates code to be compiled with the 
  // library specific translator to recognize the use of library specific abstractions
  // within a user's application.

     string returnString = recognitionTemplateCode;

#if 0
     printf ("recognitionTemplateCode = %s \n",recognitionTemplateCode.c_str());
     ROSE_ABORT();
#endif

  // edit copy of template string
     returnString = StringUtility::copyEdit(returnString,"$FUNCTION",functionName);
     returnString = StringUtility::copyEdit(returnString,"$TYPE",typeName);

     returnString = StringUtility::copyEdit(returnString,"$DEFINED_LIBRARY_NAME",libraryName);

     return returnString;
   }

void
AbstractionInfo::normalizeNames()
   {
  // This function normalizes function names (e.g. operator() --> operator_parenthesises, etc.

     functionName = StringUtility::copyEdit(functionName,"operator()","operator_parenthesises");
     functionName = StringUtility::copyEdit(functionName,"operator==","operator_equality");
     functionName = StringUtility::copyEdit(functionName,"operator!=","operator_notequal");
     functionName = StringUtility::copyEdit(functionName,"operator++","operator_plusplus");
     functionName = StringUtility::copyEdit(functionName,"operator--","operator_minusminus");
     functionName = StringUtility::copyEdit(functionName,"operator<=","operator_lessthanorequalplus");
     functionName = StringUtility::copyEdit(functionName,"operator>=","operator_greaterthanorequal");

     functionName = StringUtility::copyEdit(functionName,"operator+=","operator_plusEqual");
     functionName = StringUtility::copyEdit(functionName,"operator-=","operator_minusEqual");
     functionName = StringUtility::copyEdit(functionName,"operator*=","operator_timesEqual");
     functionName = StringUtility::copyEdit(functionName,"operator/=","operator_divideEqual");
     functionName = StringUtility::copyEdit(functionName,"operator%=","operator_modEqual");
     functionName = StringUtility::copyEdit(functionName,"operator^=","operator_xorEqual");

     functionName = StringUtility::copyEdit(functionName,"operator&&","operator_and");
     functionName = StringUtility::copyEdit(functionName,"operator||","operator_or");

     functionName = StringUtility::copyEdit(functionName,"operator=","operator_assign");
     functionName = StringUtility::copyEdit(functionName,"operator+","operator_plus");
     functionName = StringUtility::copyEdit(functionName,"operator-","operator_minus");
     functionName = StringUtility::copyEdit(functionName,"operator*","operator_times");
     functionName = StringUtility::copyEdit(functionName,"operator/","operator_divide");
     functionName = StringUtility::copyEdit(functionName,"operator%","operator_mod");
     functionName = StringUtility::copyEdit(functionName,"operator<","operator_lessthan");
     functionName = StringUtility::copyEdit(functionName,"operator>","operator_greaterthan");
     functionName = StringUtility::copyEdit(functionName,"operator&","operator_bitwise_and");
     functionName = StringUtility::copyEdit(functionName,"operator|","operator_bitwise_or");
     functionName = StringUtility::copyEdit(functionName,"operator^","operator_xor");

     functionName = StringUtility::copyEdit(functionName,"~","destructor_");
     functionName = StringUtility::copyEdit(functionName,"operator new","operator_new");
     functionName = StringUtility::copyEdit(functionName,"operator delete","operator_delete");
   }


class Grammar
   {
  // *************************************************************************
  // This class defines the code generation for a specific library.  Since many
  // libraries could exist at the same time, multiple instances of this object
  // make sense (and is part of the design and expected use).
  // *************************************************************************
     public:
          string libraryName;
          string libraryPath;

       // Internal data used to compute enums and recognition functions
          list<AbstractionInfo> abstractionList;

          Grammar ( SgProject* project, string name, string path );

          void generateGrammarFromAST      ( SgProject *project );
          void generateGrammarFromClass    ( SgClassDeclaration *classDeclaration );
          AbstractionInfo* generateGrammarFromFunction ( SgFunctionDeclaration *functionDeclaration );
          AbstractionInfo* generateGrammarFromMemberFunction ( SgMemberFunctionDeclaration *memberFunctionDeclaration );

          void generateCode();

     private:
       // supporting member functions
          string generateClassName ( SgClassDefinition* classDefinition );
          string generateVariantEnumDeclarationCode();
          string generateLibrarySpecificCode();

          void   enumerateOverloadedFunctions();

          list<string> generateListOfOverloadedFunctions();
   };

Grammar::Grammar ( SgProject* project, string name, string path )
   {
     libraryName = name;
     libraryPath = path;

     generateGrammarFromAST(project);
   }

void Grammar::enumerateOverloadedFunctions()
   {
  // This function addes numbers to the filenames so that we generate 
  // only unique filenames for use within the code generation phase.

  // Get a list of the overloaded functions
     list<string> overloadedFunctionNameList = generateListOfOverloadedFunctions();
  // ROSE_ASSERT(overloadedFunctionNameList.size() > 0);

  // We need a persistant name list to process the function 
  // and find overloaded functions and enumerate them.
     list<string> nameList;
     for (list<AbstractionInfo>::iterator i = abstractionList.begin(); i != abstractionList.end(); i++)
        {
          string targetName = (*i).typeName + "_" + (*i).functionName;

          list<string>::iterator location = find(overloadedFunctionNameList.begin(),overloadedFunctionNameList.end(),targetName);
          if (location != overloadedFunctionNameList.end())
             {
            // Found an overloaded function

            // Now count the number of previously defined versions of the overloaded function
               list<string>::iterator location = find(nameList.begin(),nameList.end(),targetName);
               list<string>::iterator nextlocation = location;

               int counter = 0;
               if (nextlocation != nameList.end())
                  {
                    counter++;
                    nextlocation++;
                  }

               while (nextlocation != nameList.end())
                  {
                    counter++;

                    list<string>::iterator tmplocation = find(nextlocation,nameList.end(),targetName);
                    nextlocation = tmplocation;
                    if (nextlocation != nameList.end())
                         nextlocation++;
                  }

            // printf ("counter = %d \n",counter);
            // ROSE_ASSERT (counter < 4);

            // found a match, so this is an overloaded function change the name
               (*i).functionName += string("_") + StringUtility::numberToString(counter);
               nameList.push_back(targetName);
             }
        }
   }

#if 0
void
Grammar::fixupOverloadedFunctions ( list<string> & overloadedFunctionNameList )
   {
  // Modify the abstractions list to correct the names of the overloaded functions that were found.
  // Now add the entries specific to each library abstraction

  // printf ("In Grammar::fixupOverloadedFunctions() abstractionList.size() = %zu \n",abstractionList.size());
  // printf ("overloadedFunctionNameList = %s \n",StringUtility::listToString(overloadedFunctionNameList).c_str());
     ROSE_ASSERT(overloadedFunctionNameList.size() > 0);

  // ROSE_ABORT();

     for (list<AbstractionInfo>::iterator i = abstractionList.begin(); i != abstractionList.end(); i++)
        {
          string targetName = (*i).functionName;
       // printf ("targetName = %s \n",targetName.c_str());
          list<string>::iterator location = find(overloadedFunctionNameList.begin(),overloadedFunctionNameList.end(),targetName);

          if (location != overloadedFunctionNameList.end())
             {
            // found a match, so this is an overloaded function change the name
            // printf ("Found a match in fixup phase: targetName = %s \n",targetName.c_str());
               (*i).functionName += "_0";
             }
        }
   }
#endif

AbstractionInfo* Grammar::generateGrammarFromFunction ( SgFunctionDeclaration *functionDeclaration )
   {
     ROSE_ASSERT (functionDeclaration != NULL);
     string functionName        = functionDeclaration->get_name().str();
     string functionNameMangled = functionDeclaration->get_mangled_name().str();
  // printf ("functionName = %s functionNameMangled = %s \n",functionName.c_str(),functionNameMangled.c_str());

     string typeName = "global";

     AbstractionInfo *abstractionData = new AbstractionInfo(typeName,functionName);
     ROSE_ASSERT (abstractionData != NULL);

  // unparseDeclarationString ( SgDeclarationStatement* declaration, bool unparseAsDeclaration )
  // string unparsedFunctionDeclaration = functionDeclaration->unparseToString();
     string unparsedFunctionDeclaration = unparseDeclarationToString(functionDeclaration);
     abstractionData->setFunctionPrototypeString(unparsedFunctionDeclaration);

  // printf ("unparsedFunctionDeclaration = %s \n",unparsedFunctionDeclaration.c_str());

     abstractionData->normalizeNames();

  // abstractionList.push_back(abstractionData);
  // printf ("In Grammar::generateGrammarFromFunction(): abstractionList.size() = %zu \n",abstractionList.size());

     return abstractionData;
   }

string Grammar::generateClassName ( SgClassDefinition* classDefinition )
   {
  // SgClassDefinition*  classDefinition  = memberFunctionDeclaration->get_scope();
     ROSE_ASSERT (classDefinition != NULL);

  // SgClassDeclaration* classDeclaration = classDefinition->get_declaration();
  // classDeclaration = classDefinition->get_declaration();
  // typeName = classDeclaration->get_name().str();
     string className = classDefinition->get_qualified_name().str();
  // printf ("In generateClassName: className = %s \n",className.c_str());

     string baseClassNameString;

#if 0
     SgBaseClassList & baseClassList = classDefinition->get_inheritances();
     for (list<SgBaseClass>::iterator i = baseClassList.begin(); i != baseClassList.end(); i++)
        {
          ROSE_ASSERT ((*i).get_base_class() != NULL);
          SgClassDeclaration* classDeclaration = (*i).get_base_class();
          SgClassDefinition*  classDefinition  = classDeclaration->get_definition();

          printf ("In for loop over base classes: baseClassNameString = %s \n",baseClassNameString.c_str());

          baseClassNameString += generateClassName( classDefinition );
        }
#endif

     string returnString = baseClassNameString + className;

     return returnString;
   }

AbstractionInfo* Grammar::generateGrammarFromMemberFunction ( SgMemberFunctionDeclaration *memberFunctionDeclaration )
   {
     ROSE_ASSERT (memberFunctionDeclaration != NULL);
     string functionName        = memberFunctionDeclaration->get_name().str();
     string functionNameMangled = memberFunctionDeclaration->get_mangled_name().str();
  // printf ("memberFunctionName = %s functionNameMangled = %s \n",functionName.c_str(),functionNameMangled.c_str());

     SgClassDefinition*  classDefinition  = memberFunctionDeclaration->get_class_scope();
     string typeName = generateClassName(classDefinition);

     typeName = StringUtility::copyEdit(typeName,"::","_");
  // printf ("typeName = %s functionName = %s \n",typeName.c_str(),functionName.c_str());
  // ROSE_ABORT();

     AbstractionInfo* abstractionData = new AbstractionInfo(typeName,functionName);
     ROSE_ASSERT (abstractionData != NULL);

  // string unparsedMemberFunctionDeclaration = memberFunctionDeclaration->unparseToString();
     string unparsedMemberFunctionDeclaration = unparseDeclarationToString(memberFunctionDeclaration);
     abstractionData->setFunctionPrototypeString(unparsedMemberFunctionDeclaration);

  // printf ("unparsedMemberFunctionDeclaration = %s \n",unparsedMemberFunctionDeclaration.c_str());

     abstractionData->normalizeNames();

  // abstractionList.push_back(abstractionData);
  // printf ("In Grammar::generateGrammarFromMemberFunction(): abstractionList.size() = %zu \n",abstractionList.size());

     return abstractionData;
   }

void Grammar::generateGrammarFromClass ( SgClassDeclaration *classDeclaration )
   {
     ROSE_ASSERT (classDeclaration != NULL);

     string className          = classDeclaration->get_qualified_name().str();
     string qualifiedClassName = classDeclaration->get_qualified_name().str();

  // Not sure why this function requires an integer reference as a parameter 
  // (I think it is to avoid havin it called more than once internally when 
  // computing the mangled name for a member function)
     int counter = 0;
     string mangledQualifiedClassName   = classDeclaration->get_mangled_qualified_name(counter).str();

  // printf ("className = %s qualifiedClassName = %s mangledClassName = %s \n",
  //      className.c_str(),qualifiedClassName.c_str(),mangledQualifiedClassName.c_str());
     printf ("className = %s \n",className.c_str());
     printf ("qualifiedClassName = %s \n",qualifiedClassName.c_str());
     printf ("mangledQualifiedClassName = %s \n",mangledQualifiedClassName.c_str());

#if 0
  // Now get any nested class declarations
     SgClassDefinition* classDefinition = classDeclaration->get_definition();
     ROSE_ASSERT(classDefinition != NULL);
     list<SgNode*> classDeclarationList    = NodeQuery::querySubTree(classDefinition,NodeQuery::ClassDeclarations);
     printf ("In generateGrammarFromClass: classDeclarationList.size() = %zu \n",classDeclarationList.size());
     for (list<SgNode*>::iterator i = classDeclarationList.begin(); i != classDeclarationList.end(); i++)
        {
          SgClassDeclaration* localClassDeclaration = isSgClassDeclaration(*i);
          ROSE_ASSERT (localClassDeclaration != NULL);
          generateGrammarFromClass (localClassDeclaration);
        }
#endif

  // Process all member functions
     Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree(classDeclaration,NodeQuery::FunctionDeclarations);
     printf ("In generateGrammarFromClass: functionDeclarationList.size() = %zu \n",functionDeclarationList.size());
     for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++)
        {
          SgFunctionDeclaration* localFunctionDeclaration = isSgFunctionDeclaration(*i);
          ROSE_ASSERT (localFunctionDeclaration != NULL);

       // generateGrammarFromFunction (localFunctionDeclaration);

       // Look for member function declarations and tree them differently
          SgMemberFunctionDeclaration* localMemberFunctionDeclaration = isSgMemberFunctionDeclaration(*i);
          if (localMemberFunctionDeclaration != NULL)
             {
               generateGrammarFromMemberFunction (localMemberFunctionDeclaration);
             }
            else
             {
               generateGrammarFromFunction (localFunctionDeclaration);
             }
        }

#if 0
  // We don't need non member functions (at least not yet)
     Rose_STL_Container<SgNode*> classMemberList    = NodeQuery::querySubTree(classDeclaration,NodeQuery::ClassFields);
     printf ("classMemberList.size() = %zu \n",classMemberList.size());
     for (Rose_STL_Container<SgNode*>::iterator i = classMemberList.begin(); i != classMemberList.end(); i++)
        {
        }
#endif
   }

list<string> 
Grammar::generateListOfOverloadedFunctions()
{
  // We need a persistant name list to process the function 
  // and find overloaded functions and enumerate them.
     list<string> nameList;
     list<string> returnList;

     for (list<AbstractionInfo>::iterator i = abstractionList.begin(); i != abstractionList.end(); i++)
        {
          string targetName = (*i).typeName + "_" + (*i).functionName;
       // printf ("targetName = %s \n",targetName.c_str());
          list<string>::iterator location = find(nameList.begin(),nameList.end(),targetName);

          if (location != nameList.end())
             {
            // found a match, so this is an overloaded function change the name
            // printf ("Found a match in Grammar::generateListOfOverloadedFunctions: targetName = %s \n",targetName.c_str());
            // Put the overloaded function into the list upon the location of the name in the nameList
               returnList.push_back(targetName);
             }
            else
             {
               nameList.push_back(targetName);
             }
        }

     returnList.sort();
     returnList.unique();

     return returnList;
   }


void Grammar::generateGrammarFromAST ( SgProject *project )
   {
     ROSE_ASSERT (project != NULL);

  // This actually generates a list of all function declarations in the whole program, 
  // not just in global scope but this is sufficent for initial testing.
     Rose_STL_Container<SgNode*> functionDeclarationList = NodeQuery::querySubTree(project,NodeQuery::FunctionDeclarations);

     printf ("In main: functionDeclarationList.size() = %zu \n",functionDeclarationList.size());
  // ROSE_ABORT();

     int counter = 1;
     for (Rose_STL_Container<SgNode*>::iterator i = functionDeclarationList.begin(); i != functionDeclarationList.end(); i++)
        {
          SgFunctionDeclaration* localFunctionDeclaration = isSgFunctionDeclaration(*i);
          ROSE_ASSERT (localFunctionDeclaration != NULL);

       // Check the path so that we only include declarations defined in the library source tree
       // string functionDeclarationPath = rose::getFileName(localFunctionDeclaration);
          string functionDeclarationPath = localFunctionDeclaration->get_file_info()->get_filename();
          functionDeclarationPath = rose::getPathFromFileName(functionDeclarationPath.c_str());
       // printf ("functionDeclarationPath = %s \n",functionDeclarationPath.c_str());
       // printf ("Target library path = %s \n",libraryPath.c_str());

          if ( StringUtility::isContainedIn(functionDeclarationPath,libraryPath) == true)
             {
            // Look for member function declarations and tree them differently
               SgMemberFunctionDeclaration* localMemberFunctionDeclaration = isSgMemberFunctionDeclaration(*i);
               AbstractionInfo* abstractionData = NULL;
               if (localMemberFunctionDeclaration != NULL)
                  {
                    abstractionData = generateGrammarFromMemberFunction (localMemberFunctionDeclaration);
                  }
                 else
                  {
                    abstractionData = generateGrammarFromFunction (localFunctionDeclaration);
                  }

               ROSE_ASSERT (abstractionData != NULL);

               abstractionData->setLinkToAST(*i);

            // printf ("In Grammar::generateGrammarFromAST processing %d of %zu \n",counter,functionDeclarationList.size());
               abstractionList.push_back(*abstractionData);
            // printf ("In Grammar::generateGrammarFromAST(): abstractionList.size() = %zu \n",abstractionList.size());
             }

          counter++;
        }

  // The first function of each overloaded function should be lables consistantly 
  // with the others. It was not until we saw the function twice that we realized 
  // it was an overloaded function, so now go back and fixup the first function 
  // to have a consistant name as the others.  Now we modify the elements in the 
  // abstractionList directly.
  // ROSE_ASSERT(overloadedFunctionNameList.size() > 0);
  // fixupOverloadedFunctions(overloadedFunctionNameList);

#if 0
  // Get a list of the overloaded functions
     list<string> overloadedFunctionNameList = generateListOfOverloadedFunctions();
     ROSE_ASSERT(overloadedFunctionNameList.size() > 0);

  // We need a persistant name list to process the function 
  // and find overloaded functions and enumerate them.
     list<string> persistantNamesList;
     for (list<AbstractionInfo>::iterator i = abstractionList.begin(); i != abstractionList.end(); i++)
        {
          enumerateOverloadedFunctions(persistantNamesList,overloadedFunctionNameList,*i);
        }
#else
  // Number the overloaded statements
     enumerateOverloadedFunctions();
#endif

#if 0
     printf ("Exiting in Grammar::generateGrammarFromAST \n");
     ROSE_ABORT();
#endif

#if 0
     list<SgNode*> classDeclarationList    = NodeQuery::querySubTree(project,NodeQuery::ClassDeclarations);

     printf ("In main: classDeclarationList.size() = %zu \n",classDeclarationList.size());

     for (list<SgNode*>::iterator i = classDeclarationList.begin(); i != classDeclarationList.end(); i++)
        {
          SgClassDeclaration* localClassDeclaration = isSgClassDeclaration(*i);
          ROSE_ASSERT (localClassDeclaration != NULL);
          generateGrammarFromClass (localClassDeclaration);
        }
#endif
   }

string Grammar::generateVariantEnumDeclarationCode()
   {
     printf ("Generate enum declaration ... \n");

     string codeString;
     string indentString = "          ";

  // codeString += indentString;
     codeString += "       // enum declaration code \n";

     codeString += indentString;
     codeString += string("enum ") + libraryName + "VariantEnum \n";
     codeString += indentString;
     codeString += "   {\n";

     int numberOfSageVariants = (int)V_SgNumVariants;
     int maxStringLength = 0;
     for (int i=0; i < numberOfSageVariants; i++)
        {
       // build a new variant list
          string librarySpecificLanguageAbstraction = getVariantName(VariantT(i));
          int tempLength = librarySpecificLanguageAbstraction.length();
          maxStringLength = (maxStringLength < tempLength) ? tempLength : maxStringLength;
        }

  // Add a value to represent unknown values
     codeString += indentString;
     codeString += "     V_unknownValue,\n";

     for (int i=0; i < numberOfSageVariants; i++)
        {
       // build a new variant list
          string librarySpecificLanguageAbstraction = getVariantName(VariantT(i));

#if 0
       // Comment out to preserve the same names of the values as in the base language (C++ via SAGE III)
          librarySpecificLanguageAbstraction = 
               StringUtility::copyEdit(librarySpecificLanguageAbstraction,"Sg",libraryName);
#endif

          codeString += indentString;

       // Make the assignments line up nicely for fromatting of generated code
          string lineupAssignments;
          for (unsigned j=0; j < maxStringLength-librarySpecificLanguageAbstraction.length(); j++)
          lineupAssignments += " ";

          codeString += string("     V_") + librarySpecificLanguageAbstraction + lineupAssignments + string(" = ::V_") + getVariantName(VariantT(i)) + ",\n";
        }

#if 1
  // Now add the entries specific to each library abstraction
     for (list<AbstractionInfo>::iterator i = abstractionList.begin(); i != abstractionList.end(); i++)
        {
          string librarySpecificLanguageAbstraction = string("     V_") + libraryName + "_" + 
                                                      (*i).typeName + "_" + (*i).functionName;

          codeString += indentString;
          codeString += librarySpecificLanguageAbstraction + ",\n";
        }
#endif

     codeString += indentString;
     codeString += "     V_SgNumVariants\n";
     codeString += indentString;
     codeString += "   };\n";

     codeString += string("   };\n");

     return codeString;
   }

string 
Grammar::generateLibrarySpecificCode()
   {
     string returnString = 
          StringUtility::readFile("/home/dquinlan/ROSE/NEW_ROSE/Projects/HighLevelGrammars/GeneratedLibraryCode.code");

  // edit copy of template string
     returnString = StringUtility::copyEdit(returnString,"$DEFINED_LIBRARY_NAME",libraryName);

#if 0
     printf ("returnString = %s \n",returnString.c_str());
     ROSE_ABORT();
#endif

     return returnString;
   }


void Grammar::generateCode()
   {
     printf ("Generate code ... \n");

     string indentString = "          ";
     string headerFileCodeString = "// header file (declarations and class definitions) \n";

     headerFileCodeString += string("class ") + libraryName + "Grammar \n   {\n     public:\n";
  // headerFileCodeString += indentString;

     headerFileCodeString += generateVariantEnumDeclarationCode();

  // printf ("headerFileCodeString = %s \n",headerFileCodeString.c_str());
  // ROSE_ABORT();

  // write out arrayAbstractionGeneratedSource.h file
     string outputFileName       = libraryName + "AbstractionGeneratedSource";

  // write out the header file
     string outputHeaderFileName = outputFileName + ".h";
     StringUtility::writeFile ( headerFileCodeString.c_str() , outputHeaderFileName.c_str() , "" );

  // Now generate the source code which recognizes the abstractions within the AST

     string sourceFileCodeString = "// source code file \n";

#if 0
  // Now add the entries specific to each library abstraction (function and member function)
     for (list<AbstractionInfo>::iterator i = abstractionList.begin(); i != abstractionList.end(); i++)
        {
          sourceFileCodeString += (*i).buildRecognitionCode(libraryName);
        }
#else
     printf ("SKIPPING ABSTRACTION SPECIFIC CODE GENERATION \n");
#endif

#if 0
  // Code generated once for each specific library
     sourceFileCodeString += generateLibrarySpecificCode();
#else
     printf ("SKIPPING LIBRARY SPECIFIC CODE GENERATION \n");
#endif

     printf ("sourceFileCodeString = %s \n",sourceFileCodeString.c_str());

  // write out the source file
     string outputSourceFileName = outputFileName + ".C";
     StringUtility::writeFile ( sourceFileCodeString.c_str() , outputSourceFileName.c_str() , "" );
   }

int
main ( int argc, char** argv )
   {
  // Main Function for default example ROSE Preprocessor
  // This is an example of a preprocessor that can be built with ROSE
  // This example can be used to test the ROSE infrastructure

     SgProject* project = frontend(argc,argv);

  // printf ("Generate the pdf output of the SAGE III AST \n");
  // generatePDF ( *project );

  // Generate list of classes in global scope
     string libraryName = "Aplusplus";
  // string libraryPath = ".";
     string libraryPath = "/home/dquinlan/ROSE/NEW_ROSE/TESTS/CompileTests/A++Code";

     Grammar X ( project, libraryName, libraryPath );

#if 0
     printf ("Exiting after processing grammar constructor \n");
     ROSE_ABORT();
#endif

     X.generateCode();

     return 0;

  // return backend(project);
  // return backend(frontend(argc,argv));
   }


