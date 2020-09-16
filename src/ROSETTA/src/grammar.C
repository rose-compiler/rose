// ################################################################
// #                           Header Files                       #
// ################################################################

#include "rose_config.h"
#include "grammar.h"
#include "AstNodeClass.h"
#include "grammarString.h"
#include <cctype>
#include <sstream>
#include <fstream>
#include <map>
#include <iostream>

using namespace std;
using namespace Rose;

// Rasmussen (04/17/2019): Support for ATerms has been deprecated as it is no longer needed
// and likely never fully implemented nor tested.  Files remain in the src tree but are no
// longer built.  Macro BUILD_ATERM_SUPPORT primarily used to turn off ATerm support for Sage nodes.
// If this is going to be turned back on it should be completed and thoroughly tested.
#define BUILD_ATERM_SUPPORT 0

// MS: temporary (TODO: move to processing class)
static string RTIContainerName = "rtiContainer";  // put this into the respective processing class as private member
static string RTIreturnType    = "RTIReturnType"; // typedef in Grammar/Common.code

// Support for output of constructors as part of generated documentation
string Grammar::staticContructorPrototypeString;

// ################################################################
// #                 Grammar Static Data Members                  #
// ################################################################

vector<GrammarFile*> Grammar::fileList;

// ################################################################
// #                   Grammar Member Functions                   #
// ################################################################

#ifdef ROSE_USE_SMALLER_GENERATED_FILES
#define WRITE_SEPARATE_FILES_FOR_EACH_CLASS 1
#endif

Grammar::~Grammar ()
{
}

Grammar::Grammar ( const string& inputGrammarName,
                   const string& inputPrefixName,
                   const string& inputGrammarNameBaseClass,
                   const Grammar* inputParentGrammar,
                   const string& t_directory)
   {

  // Intialize some member data
  // By default the parent grammar is not known
     target_directory = t_directory;
     parentGrammar = NULL;

  // We want to set the parent grammar as early as possible since the specification of terminals/nonterminals is
  // dependent upon the the current grammar being a "RootGrammar" (using the isRootGrammar() member function)
  // and the value of the boolean returned from isRootGrammardepends upon the pointer to the parentGrammar being set!
     bool skipConstructionOfParseFunctions = false;
     if (inputParentGrammar == NULL)
        {
       // We want to skip the construction of parse member function for the C++ grammar
          skipConstructionOfParseFunctions = true;
        }
       else
        {
          setParentGrammar(*inputParentGrammar);
        }

  // Principle constructor
     grammarName          = inputGrammarName;
     grammarPrefixName    = inputPrefixName;
     grammarNameBaseClass = inputGrammarNameBaseClass;

     filenameForSupportClasses = "";

  // JJW 2-12-2008 Use a file for this list so the numbers will be more stable
     {

#if 1
                 // TPS (11/4/2009) : This will work now not using cygwin
           std::string astNodeListFilename = std::string(ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR) + "/src/ROSETTA/astNodeList";
#else
           // DQ (4/4/2009): MSVS is not interpreting the type correctly here...(fixed rose_paths.[hC])
           // DQ (4/11/2009): Using cygwin generated rose_paths.C files so need to map cygwin file prefix to Windows file prefix.
           std::string astNodeListFilename = ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR + "/src/ROSETTA/astNodeList";
           string prefixString = ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR;
           printf("prefix == %s\n",prefixString.c_str());
           size_t prefixLocation = astNodeListFilename.find(prefixString);
           ROSE_ASSERT(prefixLocation != string::npos);
           ROSE_ASSERT(prefixLocation == 0);
           astNodeListFilename = astNodeListFilename.substr(prefixLocation+prefixString.length());
           astNodeListFilename = "C:" + astNodeListFilename;
           int i = 0;
           while (i != astNodeListFilename.length())
              {
                        if (astNodeListFilename[i] == '/')
                           {
                          // DQ (4/11/2009): My laptop version of Windows requires '\\' but it was
                          // not a problem for the desktop version of windows to use '\'.
                                 astNodeListFilename[i] = '\\';
                           }
                        i++;
              }
           printf ("astNodeListFilename = %s \n",astNodeListFilename.c_str());
#endif
       std::ifstream astNodeList(astNodeListFilename.c_str());
       size_t c = 1;
       while (astNodeList) {
         std::string name = "hello";
         astNodeList >> name;
         if (name == "") continue;
         this->astNodeToVariantMap[name] = c;
         this->astVariantToNodeMap[c] = name;
         ++c;
       }
       if( !astNodeList.eof() )
         std::cout << "We have the path " << ROSE_AUTOMAKE_ABSOLUTE_PATH_TOP_SRCDIR << std::endl;
       ROSE_ASSERT (astNodeList.eof());
       astNodeList.close();
     }
     ROSE_ASSERT (this->astNodeToVariantMap.size() >= 10); // A reasonable count

  // Build up the terminals and nonTerminals defined within the default C++ grammar (using SAGE)
     setUpSupport();
     setUpTypes();
     setUpStatements();
     setUpExpressions();
     setUpSymbols();

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
  // DQ (3/15/2007): Added support for binaries
     setUpBinaryInstructions();
#endif

  // Setup of Node requires previous definition of types,
  // expressions, statements, symbols within the grammar
     setUpNodes();

  // Specify additional global declarations required for this grammar
     setFilenameForGlobalDeclarations ("../Grammar/Cxx_GlobalDeclarations.macro");

  // We want to skip the construction of parse member function for the C++ grammar
     if ( skipConstructionOfParseFunctions == true)
        {
          AstNodeClass & Node = *lookupTerminal(terminalList, "Node");
          Node.excludeSubTreeFunctionPrototype ( "HEADER_PARSER", "../Grammar/Node.code");
          Node.excludeSubTreeFunctionPrototype ( "SOURCE_PARSER", "../Grammar/parserSourceCode.macro");
        }

  // Check the consistency of the data that we just built
     consistencyCheck();
   }

void
Grammar::consistencyCheck() const
   {
  // Call the consistencyCheck function on the list object
     for (size_t i = 0; i < terminalList.size(); ++i) {
       terminalList[i]->consistencyCheck();
     }
   }


const Grammar*
Grammar::getParentGrammar ()
   {
  // ROSE_ASSERT (parentGrammar != NULL);
     return parentGrammar;
   }

void
Grammar::setParentGrammar ( const Grammar & GrammarPointer )
   {
     parentGrammar = &GrammarPointer;
     ROSE_ASSERT (parentGrammar != NULL);
   }

bool
Grammar::isRootGrammar ()
   {
  // Determine if this is the C++ grammar at the root of the heiarchy of grammars
  // Don't call the getParentGrammar() member function since if the
  // parentGrammar is false it will trigger an assert (which we want to keep)
     return (parentGrammar == NULL) ? true : false;
   }

void
Grammar::setRootOfGrammar ( AstNodeClass* RootNodeForGrammar )
   {
     rootNode = RootNodeForGrammar;
   }

AstNodeClass*
Grammar::getRootOfGrammar ()
   {
     return rootNode;
   }

void
Grammar::addGrammarElement ( AstNodeClass & X )
   {
     ROSE_ASSERT (this != NULL);
  // terminalList.display("START of Grammar::addGrammarElement(AstNodeClass)");
     X.setGrammar(this);
     const AstNodeClass *const &Y = &X;
     terminalList.push_back ( (AstNodeClass *const &) Y );
  // terminalList.display("END of Grammar::addGrammarElement(AstNodeClass)");
  // terminalList.consistencyCheck();
     astVariantToTerminalMap[this->getVariantForTerminal(X)] = &X;
   }

const std::string&
Grammar::getGrammarPrefixName() const
   {
  // This function returns the name of the grammar and is used to substitute for
  // "$GRAMMAR_PREFIX_" in the name of the generated classes that represent the
  // terminals and nonterminals of the grammar.
     ROSE_ASSERT(this != NULL);
     return grammarPrefixName;
   }

const std::string&
Grammar::getGrammarName() const
   {
  // This function returns the name of the grammar and is used to substitute for
  // "$GRAMMAR_PREFIX_" in the name of the generated classes that represent the
  // terminals and nonterminals of the grammar.
     ROSE_ASSERT(this != NULL);
     return grammarName;
   }

std::string
Grammar::getGrammarTagName()
   {
  // This function returns the grammar name to be substituted for "$GRAMMAR_TAG_PREFIX_"
  // so that all tags (in the global enum) are unique.  However since for SAGE the enums
  // are already set (they are used in the EDG code and I would like to avoid changing the
  // SAGE EDG interface, and generaly I would like to have the base level grammar be
  // compatable with SAGE) we want to have the "$GRAMMAR_TAG_PREFIX_" be "" in the case of the
  // SAGE grammar.  To do this we recognize if this grammar is the base level grammar (root == NULL)
  // and return "" in this case.  Within the hierarchy of grammars that ROSETTA builds
  // the root grammar (base level grammar) has a NULL pointer for it's parent grammar.
  // This is sufficient for its identification.

     ROSE_ASSERT(this != NULL);
     string returnName = "";
     if (parentGrammar != NULL)
        {
       // returnName = getGrammarName();
          returnName = getGrammarPrefixName();
        }

     return returnName;
   }

/**********************************
 * AstNodeClass/Nonterminal functions *
 **********************************/

AstNodeClass &
Grammar::terminalConstructor ( const string& lexeme, Grammar & X, const string& stringVar, const string& tagString )
   {
  // These functions build AstNodeClass and nonterminal objects to be associated with this grammar
  // Using a member function to construct these serves several purposes:
  // 1) organizes terminals and nonterminals with there respective grammar (without ambiguity)
  // 2) avoids or deferes the implementation of the envelop/letter interface mechanism so
  //    that the letter will have a scope longer than the envelope

     AstNodeClass* t = new AstNodeClass ( lexeme, X, stringVar, tagString, true );
     ROSE_ASSERT (t);
     return *(t);
   }

AstNodeClass &
Grammar::nonTerminalConstructor ( const string& lexeme, Grammar& X, const string& stringVar, const string& tagString, const SubclassListBuilder & builder, bool canHaveInstances )
   {
  // These functions build AstNodeClass and nonterminal objects to be associated with this grammar
  // Using a member function to construct these serves several purposes:
  // 1) organizes terminals and nonterminals with there respective grammar (without ambiguity)
  // 2) avoids or deferes the implementation of the envelop/letter interface mechanism so
  //    that the letter will have a scope longer than the envelope

     AstNodeClass* nt = new AstNodeClass ( lexeme, X, stringVar, tagString, canHaveInstances, builder );
     ROSE_ASSERT (nt);
     return *(nt);
   }

#define OUTPUT_TO_FILE true

StringUtility::FileWithLineNumbers
Grammar::readFileWithPos ( const string& inputFileName )
   {
  // Reads entire text file and places contents into a single string
  // We implemennt a file cache to improve the performance of this file access

     vector<GrammarFile*>::iterator i;
     for (i = fileList.begin(); i != fileList.end(); i++)
        {
          if ( (*i)->getFilename() == inputFileName )
             {
               return (*i)->getBuffer();
             }
             }

     StringUtility::FileWithLineNumbers result = StringUtility::readFileWithPos(inputFileName);

     GrammarFile *file = new GrammarFile(inputFileName,result);
     ROSE_ASSERT (file != NULL);

     fileList.push_back(file);
     return result;
   }

void
Grammar::writeFile ( const StringUtility::FileWithLineNumbers & outputString,
                     const string & directoryName,
                     const string & className,
                     const string & fileExtension )
   {
  // char* directoryName = GrammarString::stringDuplicate(directoryName);
     string outputFilename = (directoryName == "." ? "" : directoryName + "/") + className + fileExtension;


  // printf ("outputFilename = %s \n",outputFilename.c_str());
     ofstream ROSE_ShowFile(outputFilename.c_str());
     if (ROSE_ShowFile.good() == false)
        {
          printf ("outputFilename = %s could not be opened, likely the directory is missing...\n",outputFilename.c_str());
          string command = "mkdir -p " + target_directory + sourceCodeDirectoryName();

       // DQ (12/28/2009): As I recall there is a more secure way to do this...see sage_support.cpp for an example.
          if (verbose)
              printf ("Calling system(%s): making a new directory in the build tree...\n",command.c_str());
          if (system(command.c_str())) {
              std::cerr <<"command failed: \"" <<StringUtility::cEscape(command) <<"\"\n";
              exit(1);
          }

       // retry opening the file...
          ROSE_ShowFile.open(outputFilename.c_str());
        }
     ROSE_ASSERT (ROSE_ShowFile.good() == true);

  // Select an output stream for the program tree display (cout or <filename>.C.roseShow)
  // Macro OUTPUT_SHOWFILE_TO_FILE is defined in the transformation_1.h header file
     ostream & outputStream = (OUTPUT_TO_FILE ? (ROSE_ShowFile) : (cout));
     ROSE_ASSERT (outputStream.good() == true);

     outputStream << StringUtility::toString(outputString, outputFilename);
     ROSE_ASSERT (outputStream.good() == true);

     ROSE_ShowFile.close();
     ROSE_ASSERT (outputStream.good() == true);
   }


void
Grammar::appendFile ( const StringUtility::FileWithLineNumbers & outputString,
                      const string & directoryName,
                      const string & className,
                      const string & fileExtension )
   {
  // char* directoryName = GrammarString::stringDuplicate(directoryName);
     string outputFilename = (directoryName == "." ? "" : directoryName + "/") + className + fileExtension;

  // printf ("outputFilename = %s \n",outputFilename.c_str());
     ofstream ROSE_ShowFile(outputFilename.c_str(),std::ios::out | std::ios::app);
#if 0
  // At this point the file should already exist.
     if (ROSE_ShowFile.good() == false)
        {
          printf ("outputFilename = %s could not be opened, likely the directory is missing...\n",outputFilename.c_str());
          string command = "mkdir -p " + target_directory + sourceCodeDirectoryName();

       // DQ (12/28/2009): As I recall there is a more secure way to do this...see sage_support.cpp for an example.
          printf ("Calling system(%s): making a new directory in the build tree...\n",command.c_str());
          system(command.c_str());

       // retry opening the file...
          ROSE_ShowFile.open(outputFilename.c_str());
        }
#endif
     ROSE_ASSERT (ROSE_ShowFile.good() == true);

  // Select an output stream for the program tree display (cout or <filename>.C.roseShow)
  // Macro OUTPUT_SHOWFILE_TO_FILE is defined in the transformation_1.h header file
     ostream & outputStream = (OUTPUT_TO_FILE ? (ROSE_ShowFile) : (cout));
     ROSE_ASSERT (outputStream.good() == true);

     outputStream << StringUtility::toString(outputString, outputFilename);
     ROSE_ASSERT (outputStream.good() == true);

     ROSE_ShowFile.close();
     ROSE_ASSERT (outputStream.good() == true);
   }


string
Grammar::sourceCodeDirectoryName ()
   {
     return "GENERATED_CODE_DIRECTORY_" + getGrammarName();
   }


void
Grammar::generateStringListsFromSubtreeLists ( AstNodeClass & node,
                                               vector<GrammarString *> & includeList,
                                               vector<GrammarString *> & excludeList,
                                               FunctionPointerType listFunction )
{
  // This function traverses back through the grammar tree to collect the elements in the
  // SUBTREE_LISTs (including the SUBTREE_LISTs of the current node).
  // Since we want the parent node list elements listed first we
  // perform a postorder traversal.

  vector<GrammarString *>::const_iterator grammarStringIterator;
  vector<GrammarString *> &listOfIncludes = (node.*listFunction)(AstNodeClass::SUBTREE_LIST,AstNodeClass::INCLUDE_LIST);
  vector<GrammarString *> &listOfExcludes = (node.*listFunction)(AstNodeClass::SUBTREE_LIST,AstNodeClass::EXCLUDE_LIST);

#define PREORDER_TRAVERSAL 0

#if PREORDER_TRAVERSAL

  for( grammarStringIterator = listOfIncludes.begin();
       grammarStringIterator != listOfIncludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfExcludes.begin();
       grammarStringIterator != listOfExcludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (excludeList, **grammarStringIterator );

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#endif
//#endif

  if (node.getBaseClass() != NULL) {
    // Recursive function call
    generateStringListsFromSubtreeLists (*(node.getBaseClass()), includeList, excludeList, listFunction );
  }
  else {
  }

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#if !PREORDER_TRAVERSAL

  for( grammarStringIterator = listOfIncludes.begin();
       grammarStringIterator != listOfIncludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfExcludes.begin();
       grammarStringIterator != listOfExcludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (excludeList, **grammarStringIterator );

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#endif
//#endif
   }


void
Grammar::generateStringListsFromLocalLists ( AstNodeClass & node,
                                             vector<GrammarString *> & includeList,
                                             vector<GrammarString *> & excludeList,
                                             FunctionPointerType listFunction )
{
  // This function traverses back through the grammar tree to collect the elements in the
  // LOCAL_LISTs (including the LOCAL_LISTs of the current node).
  // Since we want the parent node list elements listed first we
  // perform a postorder traversal.

  vector<GrammarString *>::const_iterator grammarStringIterator;
  vector<GrammarString *> &listOfIncludes = (node.*listFunction)(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
  vector<GrammarString *> &listOfExcludes = (node.*listFunction)(AstNodeClass::LOCAL_LIST,AstNodeClass::EXCLUDE_LIST);

#define PREORDER_TRAVERSAL 0

#if PREORDER_TRAVERSAL

  for( grammarStringIterator = listOfIncludes.begin();
       grammarStringIterator != listOfIncludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfExcludes.begin();
       grammarStringIterator != listOfExcludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (excludeList, **grammarStringIterator );

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#endif
//#endif

  if (node.getBaseClass() != NULL) {
    // Recursive function call
    generateStringListsFromLocalLists (*(node.getBaseClass()), includeList, excludeList, listFunction );
  }
  else {
  }

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#if !PREORDER_TRAVERSAL

  for( grammarStringIterator = listOfIncludes.begin();
       grammarStringIterator != listOfIncludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfExcludes.begin();
       grammarStringIterator != listOfExcludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (excludeList, **grammarStringIterator );

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

#endif
//#endif
}


void
Grammar::generateStringListsFromAllLists ( AstNodeClass & node,
                                           vector<GrammarString *> & includeList,
                                           vector<GrammarString *> & excludeList,
                                           FunctionPointerType listFunction )
{
  // This function traverses back through the grammar tree to collect the elements in the
  // SUBTREE_LISTs and in the LOCAL_LISTs (including the lists of the current node).
  // Since we want the parent node list elements listed first we perform a postorder traversal.

  vector<GrammarString *>::const_iterator grammarStringIterator;

  vector<GrammarString *> &listOfSubTreeIncludes = (node.*listFunction)(AstNodeClass::SUBTREE_LIST,AstNodeClass::INCLUDE_LIST);
  vector<GrammarString *> &listOfSubTreeExcludes = (node.*listFunction)(AstNodeClass::SUBTREE_LIST,AstNodeClass::EXCLUDE_LIST);
  vector<GrammarString *> &listOfLocalIncludes = (node.*listFunction)(AstNodeClass::LOCAL_LIST,  AstNodeClass::INCLUDE_LIST);
  vector<GrammarString *> &listOfLocalExcludes = (node.*listFunction)(AstNodeClass::LOCAL_LIST,  AstNodeClass::EXCLUDE_LIST);

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

  if (node.getBaseClass() != NULL)
    {
      // Recursive function call
      generateStringListsFromAllLists (*(node.getBaseClass()), includeList, excludeList, listFunction );
    }

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

  // Traverse both include lists & both exclude lists
  for( grammarStringIterator = listOfSubTreeIncludes.begin();
       grammarStringIterator != listOfSubTreeIncludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfSubTreeExcludes.begin();
       grammarStringIterator != listOfSubTreeExcludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (excludeList, **grammarStringIterator );

  for( grammarStringIterator = listOfLocalIncludes.begin();
       grammarStringIterator != listOfLocalIncludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (includeList, **grammarStringIterator );

  for( grammarStringIterator = listOfLocalExcludes.begin();
       grammarStringIterator != listOfLocalExcludes.end();
       grammarStringIterator++)
    AstNodeClass::addElementToList (excludeList, **grammarStringIterator );

#if CHECK_LISTS
  checkListOfGrammarStrings(includeList);
  checkListOfGrammarStrings(excludeList);
#endif

}

void Grammar::editStringList ( vector<GrammarString *> & targetList, const vector<GrammarString *> & excludeList )
{
  // Remove the elements in the excludeList from the elements in targetList
  // The match is determined by the use of the operator= on the elements!
  // list is modified!
  vector<GrammarString*> newList;
  for (vector<GrammarString*>::const_iterator i = targetList.begin(); i != targetList.end(); ++i) {
    for (vector<GrammarString*>::const_iterator j = i + 1; j != targetList.end(); ++j) {
      if (**i == **j)
        goto skipThisElement;
    }
    for (vector<GrammarString*>::const_iterator j = excludeList.begin(); j != excludeList.end(); ++j) {
      if (**i == **j)
        goto skipThisElement;
    }
    newList.push_back(*i);
  skipThisElement:;
  }
  targetList.swap(newList);
}

string
Grammar::buildStringFromLists ( AstNodeClass & node,
                                FunctionPointerType listFunction,
                                StringGeneratorFunctionPointerType stringGeneratorFunction )
{
  // This method builds a string (according to the parameter stringGeneratorFunction)
  // from the local lists of the current node, from all of its parents' subtree lists,
  // and from its own subtree lists

  // We use the method defined below which basically corresponds to the
  // code above (which is commented out)
  vector<GrammarString *> sourceList= buildListFromLists(node, listFunction);
  vector<GrammarString *>::iterator sourceListIterator;

  // ROSE_ASSERT (node.token != NULL);

  string editStringMiddle;

  for( sourceListIterator = sourceList.begin();
       sourceListIterator != sourceList.end();
       sourceListIterator++ )
    {
      GrammarString & memberFunction = **sourceListIterator;
      string memberFunctionString = "// Start of memberFunctionString\n" + (memberFunction.*stringGeneratorFunction)() + "\n// End of memberFunctionString\n";
      editStringMiddle += memberFunctionString;
    }

  return editStringMiddle;
}


vector<GrammarString *>
Grammar::buildListFromLists ( AstNodeClass & node,
                              FunctionPointerType listFunction )
  // This method builds a list from the local lists of the current node,
  // from all of its parents' subtree lists, and from its own subtree lists
{
  vector<GrammarString *> includeList;
  vector<GrammarString *> excludeList;
  ROSE_ASSERT (includeList.size() == 0);
  ROSE_ASSERT (excludeList.size() == 0);

  // Initialize with local node data
  includeList = (node.*listFunction)(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
  excludeList = (node.*listFunction)(AstNodeClass::LOCAL_LIST,AstNodeClass::EXCLUDE_LIST);

  // Now generate the additions to the lists from the parent node subtree lists
  // and the subtree lists of the current node
  if (node.getBaseClass())
    generateStringListsFromSubtreeLists ( *(node.getBaseClass()), includeList, excludeList, listFunction );

  // Now edit the list to remove elements appearing within the exclude list
  editStringList ( includeList, excludeList );

  return includeList;
}


string
Grammar::buildStringForPrototypes ( AstNodeClass & node )
   {
  // This function adds in the source code specific to a node in the
  // tree that represents the hierachy of the grammer's implementation.

     // ROSE_ASSERT (node.token != NULL);
     // BP : 10/09/2001 modified to provide addresses
     return buildStringFromLists ( node,
                                   &AstNodeClass::getMemberFunctionPrototypeList,
                                   &GrammarString::getFunctionPrototypeString );
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForVariantFunctionSource         ( AstNodeClass & node )
   {
  // Every node in the grammar has a function that identifies it with a numerical value
  // (e.g. SCOPE_STMT).

  // JJW (10/16/2008): Remove this (variant() is now a single function in
  // Node.code)
#if 0
     string variantFunctionTemplateFileName   = "../Grammar/grammarVariantFunctionDefinitionMacros.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (variantFunctionTemplateFileName);

     returnString = GrammarString::copyEdit (returnString,"$MARKER",node.getTagName());

     return returnString;
#endif
     return StringUtility::FileWithLineNumbers();
   }

StringUtility::FileWithLineNumbers
Grammar::supportForBuildStringForIsClassNameFunctionSource     ( AstNodeClass & node, const StringUtility::FileWithLineNumbers& accumulationStringOrig )
   {
  // This function forms support for the Grammar::buildStringForIsClassNameFunctionSource function.
  // If a node is a part of the subtree represented by this grammar then is is by definition
  // of the type represented by the root of the subtree.

     StringUtility::FileWithLineNumbers accumulationString = accumulationStringOrig;

     string tempString = "               case ";

     tempString += node.getTagName();
     tempString += ":\n";
     accumulationString.push_back(StringUtility::StringWithLineNumber(tempString, "" /* "<supportForBuildStringForIsClassNameFunctionSource on " + node.getToken().getTagName() + ">" */, 1));

     vector<AstNodeClass *>::iterator nodeListIterator;
     // Loop through the children
     for( nodeListIterator = node.subclasses.begin();
          nodeListIterator != node.subclasses.end();
          nodeListIterator++)
        {
          ROSE_ASSERT ((*nodeListIterator) != NULL);
          ROSE_ASSERT ((*nodeListIterator)->getBaseClass() == &node);

          accumulationString =
               supportForBuildStringForIsClassNameFunctionSource( **nodeListIterator, accumulationString);
        }

     return accumulationString;
   }

StringUtility::FileWithLineNumbers
Grammar::buildStringForIsClassNameFunctionSource     ( AstNodeClass & node )
   {
  // This function builds the source code for a friend function, each class has
  // a member function that casts a pointer to any derived class to type represented by
  // this level of the subtree  (e.g. at the Statement level of the grammar the function would
  // cast a pointer to any type of function to the Statement base class type).

     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarIsClassNameFunctionDefinitionMacros.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     StringUtility::FileWithLineNumbers accumulationString = supportForBuildStringForIsClassNameFunctionSource(node,StringUtility::FileWithLineNumbers());
     returnString = GrammarString::copyEdit(returnString,"$ROOT_NODE_OF_GRAMMAR",getRootOfGrammar()->getName());
     returnString = GrammarString::copyEdit(returnString,"$ACCUMULATION_STRING",accumulationString);
     return returnString;
   }

StringUtility::FileWithLineNumbers
Grammar::buildStringForNewAndDeleteOperatorSource ( AstNodeClass & node )
   {
     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarNewDeleteOperatorMacros.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());
  // returnString = GrammarString::copyEdit(returnString,"$ROOT_NODE_OF_GRAMMAR",getRootOfGrammar().getName());
  // returnString = GrammarString::copyEdit(returnString,"$ACCUMULATION_STRING",accumulationString);

     return returnString;
   }

void
Grammar::buildNewAndDeleteOperators( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
  // printf ("At TOP of Grammar::buildNewAndDeleteOperators() \n");

  // printf ("Exiting at TOP of Grammar::buildSourceFiles() \n");
  // ROSE_ABORT();

  // printf ("At TOP of Grammar::buildNewAndDeleteOperators(): node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName(),node.nodeList.size());

     StringUtility::FileWithLineNumbers editString = buildStringForNewAndDeleteOperatorSource(node);

  // printf ("editString = %s \n",editString.c_str());

  // outputFile += editString;

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
  // Now write out the file (each class in its own file)!
     string fileExtension = ".C";
     string directoryName = target_directory + sourceCodeDirectoryName();
  // printf ("In buildNewAndDeleteOperators(): directoryName = %s \n",directoryName.c_str());
  // This should append the string to the target file.

     // tps (01/04/2010) Debugging output
       //   printf ("GRAMMAR Grammar::buildNewAndDeleteOperators : target_directory : %s  directoryName %s \n",target_directory.c_str(),directoryName.c_str());
     appendFile ( editString, directoryName, node.getName(), fileExtension );
#else
     outputFile += editString;
#endif




#if 1
  // Call this function recursively on the children of this node in the tree
     vector<AstNodeClass *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin();
          treeNodeIterator != node.subclasses.end();
          treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          buildNewAndDeleteOperators(**treeNodeIterator,outputFile);
        }
#endif
   }

StringUtility::FileWithLineNumbers
Grammar::buildStringForTraverseMemoryPoolSource ( AstNodeClass & node )
   {
     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarTraverseMemoryPool.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());


     string classSpecificString;
     string classSpecificVisitorPatternString;
     string classSpecificMemoryUsageString;

     string className = node.getName();

  // printf ("node.getName() = %s classSpecificString = %s \n",node.getName(),classSpecificString.c_str());
     returnString = GrammarString::copyEdit(returnString,"$CLASS_SPECIFIC_STATIC_MEMBERS_USING_ROSE_VISIT",classSpecificString);
     returnString = GrammarString::copyEdit(returnString,"$CLASS_SPECIFIC_STATIC_MEMBERS_USING_VISITOR_PATTERN",classSpecificVisitorPatternString);
     returnString = GrammarString::copyEdit(returnString,"$CLASS_SPECIFIC_STATIC_MEMBERS_MEMORY_USED",classSpecificMemoryUsageString);

     return returnString;
   }

void
Grammar::buildTraverseMemoryPoolSupport( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
  // printf ("At TOP of Grammar::buildNewAndDeleteOperators() \n");

  // printf ("Exiting at TOP of Grammar::buildSourceFiles() \n");
  // ROSE_ABORT();

  // printf ("At TOP of Grammar::buildNewAndDeleteOperators(): node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName(),node.nodeList.size());

     StringUtility::FileWithLineNumbers editString = buildStringForTraverseMemoryPoolSource(node);

  // printf ("editString = %s \n",editString.c_str());

  // outputFile += editString;

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
  // Now write out the file (each class in its own file)!
     string fileExtension = ".C";
     string directoryName = target_directory + sourceCodeDirectoryName();
  // printf ("In buildTraverseMemoryPoolSupport(): directoryName = %s \n",directoryName.c_str());

  // This should append the string to the target file.
     appendFile ( editString, directoryName, node.getName(), fileExtension );
#else
     outputFile += editString;
#endif

#if 1
  // Call this function recursively on the children of this node in the tree
     vector<AstNodeClass *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin();
          treeNodeIterator != node.subclasses.end();
          treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          buildTraverseMemoryPoolSupport(**treeNodeIterator,outputFile);
        }
#endif
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringToTestPointerForContainmentInMemoryPoolSource ( AstNodeClass & node )
   {
     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarTestPointerForContainmentInMemoryPool.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());

     string classSpecificString;
     string classSpecificVisitorPatternString;
     string classSpecificMemoryUsageString;

     string className = node.getName();

  // printf ("node.getName() = %s classSpecificString = %s \n",node.getName(),classSpecificString.c_str());
     returnString = GrammarString::copyEdit(returnString,"$CLASS_SPECIFIC_STATIC_MEMBERS_MEMORY_USED",classSpecificMemoryUsageString);

     return returnString;
   }

StringUtility::FileWithLineNumbers
Grammar::buildStringForCheckingIfDataMembersAreInMemoryPoolSource ( AstNodeClass & node )
   {
  // DQ & JH (1/17/2006): Added support for testing data members pointers if they point to IR nodes

     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarCheckingIfDataMembersAreInMemoryPool.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     string dataMemberSpecificString = node.buildPointerInMemoryPoolCheck();

     returnString = GrammarString::copyEdit(returnString,"$CODE_STRING",dataMemberSpecificString.c_str());

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());

     returnString = GrammarString::copyEdit(returnString,"$GRAMMAR_PREFIX_","Sg");

  // Add the associated virtual function to test of a pointer is pointing at an IR node located in the memory pool
     StringUtility::FileWithLineNumbers isInMemoryPoolTestString = buildStringToTestPointerForContainmentInMemoryPoolSource(node);
     returnString = GrammarString::copyEdit(returnString,"$ASSOCIATED_MEMORY_POOL_TEST",isInMemoryPoolTestString);

     return returnString;
   }

void
Grammar::buildStringForCheckingIfDataMembersAreInMemoryPoolSupport( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     StringUtility::FileWithLineNumbers editString = buildStringForCheckingIfDataMembersAreInMemoryPoolSource(node);

  // printf ("editString = %s \n",editString.c_str());

  // outputFile += editString;

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
  // Now write out the file (each class in its own file)!
     string fileExtension = ".C";
     string directoryName = target_directory + sourceCodeDirectoryName();
  // printf ("In buildStringForCheckingIfDataMembersAreInMemoryPoolSupport(): directoryName = %s \n",directoryName.c_str());

  // This should append the string to the target file.
     appendFile ( editString, directoryName, node.getName(), fileExtension );
#else
     outputFile += editString;
#endif

#if 1
  // Call this function recursively on the children of this node in the tree
     vector<AstNodeClass *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin();
          treeNodeIterator != node.subclasses.end();
          treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          buildStringForCheckingIfDataMembersAreInMemoryPoolSupport(**treeNodeIterator,outputFile);
        }
#endif
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForReturnDataMemberPointersSource ( AstNodeClass & node )
   {
  // DQ & JH (1/17/2006): Added support for testing data members pointers if they point to IR nodes

     //AS Look at this one to see how the code in buildStringToTestPointerForContainmentInMemoryPoolSource is called
     //to generate code for checking the memory pool.
     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarReturnDataMemberPointers.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     string dataMemberSpecificString = node.buildReturnDataMemberPointers();

     returnString = GrammarString::copyEdit(returnString,"$CODE_STRING",dataMemberSpecificString.c_str());

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());

     returnString = GrammarString::copyEdit(returnString,"$GRAMMAR_PREFIX_","Sg");

  // Add the associated virtual function to test of a pointer is pointing at an IR node located in the memory pool
  // Commented out because it is not relevant to the returning of data member pointers to IR nodes
  //   string isInMemoryPoolTestString = buildStringToReturnDataMemberPointersSource(node);
   //  returnString = GrammarString::copyEdit(returnString,"$ASSOCIATED_MEMORY_POOL_TEST",isInMemoryPoolTestString.c_str());

     return returnString;
   }

void
Grammar::buildStringForReturnDataMemberPointersSupport( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     StringUtility::FileWithLineNumbers editString = buildStringForReturnDataMemberPointersSource(node);

  // printf ("editString = %s \n",editString.c_str());

     outputFile += editString;

#if 1
  // Call this function recursively on the children of this node in the tree
     vector<AstNodeClass *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin();
          treeNodeIterator != node.subclasses.end();
          treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          buildStringForReturnDataMemberPointersSupport(**treeNodeIterator,outputFile);
        }
#endif
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForProcessDataMemberReferenceToPointersSource ( AstNodeClass & node )
   {
  // DQ & JH (1/17/2006): Added support for testing data members pointers if they point to IR nodes

     //AS Look at this one to see how the code in buildStringToTestPointerForContainmentInMemoryPoolSource is called
     //to generate code for checking the memory pool.
     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarProcessDataMemberReferenceToPointers.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     string dataMemberSpecificString = node.buildProcessDataMemberReferenceToPointers();

     returnString = GrammarString::copyEdit(returnString,"$CODE_STRING",dataMemberSpecificString.c_str());

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());

     returnString = GrammarString::copyEdit(returnString,"$GRAMMAR_PREFIX_","Sg");

  // Add the associated virtual function to test of a pointer is pointing at an IR node located in the memory pool
  // Commented out because it is not relevant to the returning of data member pointers to IR nodes
  //   string isInMemoryPoolTestString = buildStringToReturnDataMemberPointersSource(node);
   //  returnString = GrammarString::copyEdit(returnString,"$ASSOCIATED_MEMORY_POOL_TEST",isInMemoryPoolTestString.c_str());

     return returnString;
   }

void
Grammar::buildStringForProcessDataMemberReferenceToPointersSupport( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     StringUtility::FileWithLineNumbers editString = buildStringForProcessDataMemberReferenceToPointersSource(node);

  // printf ("editString = %s \n",editString.c_str());

     outputFile += editString;

#if 1
  // Call this function recursively on the children of this node in the tree
     vector<AstNodeClass *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin();
          treeNodeIterator != node.subclasses.end();
          treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          buildStringForProcessDataMemberReferenceToPointersSupport(**treeNodeIterator,outputFile);
        }
#endif
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForGetChildIndexSource ( AstNodeClass & node )
   {
  // DQ (3/7/2007): Added support for getting the index position associated with the list of IR nodes children in any IR node.

     string isClassNameFunctionTemplateFileName   = "../Grammar/grammarGetChildIndex.macro";
     StringUtility::FileWithLineNumbers returnString = readFileWithPos (isClassNameFunctionTemplateFileName);
  // printf ("returnString = %s \n",returnString);

     string dataMemberSpecificString = node.buildChildIndex();

     returnString = GrammarString::copyEdit(returnString,"$CODE_STRING",dataMemberSpecificString.c_str());

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());

     returnString = GrammarString::copyEdit(returnString,"$GRAMMAR_PREFIX_","Sg");

     return returnString;
   }

void
Grammar::buildStringForGetChildIndexSupport( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     StringUtility::FileWithLineNumbers editString = buildStringForGetChildIndexSource(node);

  // printf ("editString = %s \n",editString.c_str());

     outputFile += editString;

#if 1
  // Call this function recursively on the children of this node in the tree
     vector<AstNodeClass *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin();
          treeNodeIterator != node.subclasses.end();
          treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          buildStringForGetChildIndexSupport(**treeNodeIterator,outputFile);
        }
#endif
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForSource ( AstNodeClass & node )
   {
  // This function adds in the source code specific to a node in the
  // tree that represents the hierachy of the grammer's implementation.

  // BP : 10/09/2001, modified to provide addresses
     string beginString = buildStringFromLists ( node,
                                       &AstNodeClass::getMemberFunctionSourceList,
                                       &GrammarString::getFunctionPrototypeString );

     StringUtility::FileWithLineNumbers variantFunctionDefinition     = buildStringForVariantFunctionSource      (node);
     StringUtility::FileWithLineNumbers isClassnameFunctionDefinition = buildStringForIsClassNameFunctionSource  (node);

  // DQ (12/23/2005): Move this generated code to separate source file
  // char* copyMemberFunction            = buildCopyMemberFunctionSource            (node);

  // DQ (12/23/2005): Move this generated code to separate source file
  // DQ (9/21/2005): Added support for new and delete operators
  // char* newAndDeleteOperatorSource    = buildStringForNewAndDeleteOperatorSource (node);

     StringUtility::FileWithLineNumbers returnString = StringUtility::FileWithLineNumbers(1, StringUtility::StringWithLineNumber(beginString, "" /* "<buildStringForSource " + node.getToken().getName() + ">" */, 1)) + variantFunctionDefinition + isClassnameFunctionDefinition;

  // printf ("In Grammar::buildStringForSource(node): returnString = \n %s \n",returnString);

     return returnString;
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForDataDeclaration ( AstNodeClass & node )
{
  // This function builds the string representing the declaration
  // of data variables (all of them) in a class.
  // BP : 10/09/2001, modified to provide addresses
  string returnString = buildStringFromLists ( node,
                                              &AstNodeClass::getMemberDataPrototypeList,
                                              &GrammarString::getDataPrototypeString );
  return StringUtility::FileWithLineNumbers(1, StringUtility::StringWithLineNumber(returnString, "" /* "<buildStringForDataDeclaration>" */, 1));
}


// DQ (3/22/2017): Added to support output of "override" keyword to reduce Clang warnings.
// bool Grammar::generate_override_keyword( string variableNameString )
bool
generate_override_keyword( AstNodeClass & node, GrammarString & data )
   {
     bool returnResult = true;

     string variableNameString = string(data.variableNameString);
     string nodeName = node.baseName;

#if 0
     printf ("In generate_override_keyword(): nodeName = %s variableNameString = %s \n",nodeName.c_str(),variableNameString.c_str());
#endif

  // Rasmussen (8/16-17/2017): Added UntypedSubmoduleDeclaration and UntypedBlockDataDeclaration
  // Rasmussen (10/01/2017): Added SgUntypedPackageDeclaration,SgUntypedStructureDeclaration,SgUntypedTaskDeclaration,SgUntypedUnitDeclaration
  // Rasmussen (12/20/2017): Added SgUntypedExprListExpression
  // Rasmussen (11/20/2018): Added SgUntypedArrayReferenceExpression, SgUntypedForAllStatement, and SgJovialCompoolStatement
  // Rasmussen (05/22/2019): Moved scope from SgUntypedStructureDeclaration to SgUntypedStructureDefinition
  // Rasmussen (09/30/2019): Added SgUntypedTypedefDeclaration
  // PP (06/03/20): Added Ada nodes

  // Except in the root class for the virtual access function.
     if ( (nodeName == "XXXPragma"                  && variableNameString == "startOfConstruct")  ||
          (nodeName == "XXXPragma"                  && variableNameString == "endOfConstruct")    ||
          (nodeName == "IfStmt"                     && variableNameString == "end_numeric_label") ||
          (nodeName == "WhileStmt"                  && variableNameString == "end_numeric_label") ||
          (nodeName == "SwitchStatement"            && variableNameString == "end_numeric_label") ||
          (nodeName == "FortranDo"                  && variableNameString == "end_numeric_label") ||
          (nodeName == "ForAllStatement"            && variableNameString == "end_numeric_label") ||
          (nodeName == "InterfaceStatement"         && variableNameString == "end_numeric_label") ||
          (nodeName == "DerivedTypeStatement"       && variableNameString == "end_numeric_label") ||
          (nodeName == "ModuleStatement"            && variableNameString == "end_numeric_label") ||
          (nodeName == "ProgramHeaderStatement"     && variableNameString == "end_numeric_label") ||
          (nodeName == "ProcedureHeaderStatement"   && variableNameString == "end_numeric_label") ||
          (nodeName == "WhereStatement"             && variableNameString == "end_numeric_label") ||
          (nodeName == "QualifiedName"              && variableNameString == "scope") ||
          (nodeName == "InitializedName"            && variableNameString == "scope") ||
          (nodeName == "UntypedFunctionDeclaration" && variableNameString == "scope") ||
          (nodeName == "UntypedModuleDeclaration"   && variableNameString == "scope") ||
          (nodeName == "UntypedSubmoduleDeclaration"&& variableNameString == "scope") ||
          (nodeName == "UntypedBlockDataDeclaration"&& variableNameString == "scope") ||
          (nodeName == "UntypedPackageDeclaration"  && variableNameString == "scope") ||
          (nodeName == "UntypedStructureDefinition" && variableNameString == "scope") ||
          (nodeName == "UntypedTaskDeclaration"     && variableNameString == "scope") ||
          (nodeName == "UntypedUnitDeclaration"     && variableNameString == "scope") ||
          (nodeName == "UntypedBlockStatement"      && variableNameString == "scope") ||
          (nodeName == "UntypedFile"                && variableNameString == "scope") ||
          (nodeName == "TemplateParameter"          && variableNameString == "type")  ||
          (nodeName == "TemplateArgument"           && variableNameString == "type")  ||
          (nodeName == "JavaQualifiedType"          && variableNameString == "type")  ||
          (nodeName == "UntypedExprListExpression"  && variableNameString == "type")  ||
          (nodeName == "UntypedForAllStatement"     && variableNameString == "type")  ||
          (nodeName == "UntypedValueExpression"     && variableNameString == "type")  ||
          (nodeName == "UntypedVariableDeclaration" && variableNameString == "type")  ||
          (nodeName == "UntypedTypedefDeclaration"  && variableNameString == "type")  ||
          (nodeName == "UntypedFunctionDeclaration" && variableNameString == "type")  ||
          (nodeName == "UntypedInitializedName"     && variableNameString == "type")  ||
          (nodeName == "EnumDeclaration"            && variableNameString == "type")  ||
          (nodeName == "TypedefDeclaration"         && variableNameString == "type")  ||
          (nodeName == "ClassDeclaration"           && variableNameString == "type")  ||
          (nodeName == "FunctionDeclaration"        && variableNameString == "type")  ||
          (nodeName == "AsmExpression"              && variableNameString == "type")  ||
          (nodeName == "AsmGenericSymbol"           && variableNameString == "type")  ||
          (nodeName == "AsmElfSegmentTableEntry"    && variableNameString == "type")  ||
          (nodeName == "AsmElfRelocEntry"           && variableNameString == "type")  ||
          (nodeName == "AsmElfNoteEntry"            && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "SymbolTable"                && variableNameString == "name")  ||
          (nodeName == "Attribute"                  && variableNameString == "name")  ||
          (nodeName == "Directory"                  && variableNameString == "name")  ||
          (nodeName == "Graph"                      && variableNameString == "name")  ||
          (nodeName == "GraphNode"                  && variableNameString == "name")  ||
          (nodeName == "GraphEdge"                  && variableNameString == "name")  ||
          (nodeName == "TemplateType"               && variableNameString == "name")  ||
          (nodeName == "TypeDefault"                && variableNameString == "name")  ||
          (nodeName == "TypeLabel"                  && variableNameString == "name")  ||
          (nodeName == "InitializedName"            && variableNameString == "name")  ||
          (nodeName == "JavaMemberValuePair"        && variableNameString == "name")  ||
          (nodeName == "JovialCompoolStatement"     && variableNameString == "name")  ||
          (nodeName == "UntypedArrayReferenceExpression" && variableNameString == "name")  ||
          (nodeName == "UntypedReferenceExpression" && variableNameString == "name")  ||
          (nodeName == "UntypedFunctionDeclaration" && variableNameString == "name")  ||
          (nodeName == "UntypedModuleDeclaration"   && variableNameString == "name")  ||
          (nodeName == "UntypedSubmoduleDeclaration"&& variableNameString == "name")  ||
          (nodeName == "UntypedBlockDataDeclaration"&& variableNameString == "name")  ||
          (nodeName == "UntypedPackageDeclaration"  && variableNameString == "name")  ||
          (nodeName == "UntypedStructureDeclaration"&& variableNameString == "name")  ||
          (nodeName == "UntypedTypedefDeclaration"  && variableNameString == "name")  ||
          (nodeName == "UntypedTaskDeclaration"     && variableNameString == "name")  ||
          (nodeName == "UntypedUnitDeclaration"     && variableNameString == "name")  ||
          (nodeName == "UntypedInitializedName"     && variableNameString == "name")  ||
          (nodeName == "UntypedName"                && variableNameString == "name")  ||
          (nodeName == "EnumDeclaration"            && variableNameString == "name")  ||
          (nodeName == "TemplateDeclaration"        && variableNameString == "name")  ||
          (nodeName == "UseStatement"               && variableNameString == "name")  ||
          (nodeName == "NamespaceDeclarationStatement" && variableNameString == "name")  ||
          (nodeName == "InterfaceStatement"         && variableNameString == "name")  ||
          (nodeName == "NamespaceAliasDeclarationStatement" && variableNameString == "name")  ||
          (nodeName == "TypedefDeclaration"         && variableNameString == "name")  ||
          (nodeName == "ClassDeclaration"           && variableNameString == "name")  ||
          (nodeName == "FunctionDeclaration"        && variableNameString == "name")  ||
          (nodeName == "JavaPackageStatement"       && variableNameString == "name")  ||
          (nodeName == "InquireStatement"           && variableNameString == "name")  ||
          (nodeName == "OmpCriticalStatement"       && variableNameString == "name")  ||
          (nodeName == "EnumVal"                    && variableNameString == "name")  ||
          (nodeName == "IOItemExpression"           && variableNameString == "name")  ||
          (nodeName == "AsmOp"                      && variableNameString == "name")  ||
          (nodeName == "UnknownArrayOrFunctionReference" && variableNameString == "name")  ||
          (nodeName == "TypeTraitBuiltinOperator"   && variableNameString == "name")  ||
          (nodeName == "AdaPackageBodyDecl"         && variableNameString == "name")  ||
          (nodeName == "AdaPackageSpecDecl"         && variableNameString == "name")  ||
          (nodeName == "AdaTaskBodyDecl"            && variableNameString == "name")  ||
          (nodeName == "AdaTaskSpecDecl"            && variableNameString == "name")  ||
          (nodeName == "AdaTaskTypeDecl"            && variableNameString == "name")  ||
          (nodeName == "AdaRenamingDecl"            && variableNameString == "name")  ||
          (nodeName == "AsmFunction"                && variableNameString == "name")  ||
          (nodeName == "AsmSynthesizedFieldDeclaration" && variableNameString == "name")  ||
          (nodeName == "AsmGenericFile"             && variableNameString == "name")  ||
          (nodeName == "AsmElfSymverNeededAux"      && variableNameString == "name")  ||
          (nodeName == "AsmPESectionTableEntry"     && variableNameString == "name")  ||
          (nodeName == "AsmElfSymverDefinedAux"     && variableNameString == "name")  ||
          (nodeName == "AsmPEExportDirectory"       && variableNameString == "name")  ||
          (nodeName == "AsmDwarfConstruct"          && variableNameString == "name")  ||
          (nodeName == "AsmPEImportItem"            && variableNameString == "name")  )
       {
         returnResult = false;
#if 0
         printf ("Exiting as a test! \n");
         ROSE_ASSERT(false);
#endif
       }

     return returnResult;
   }

// DQ (3/22/2017): Added to support output of "override" keyword to reduce Clang warnings.
// bool Grammar::generate_override_keyword( string variableNameString )
bool
generate_override_keyword_for_set_functions( AstNodeClass & node, GrammarString & data )
   {
     bool returnResult = true;

     string variableNameString = string(data.variableNameString);
     string nodeName = node.baseName;

#if 0
     printf ("In generate_override_keyword(): nodeName = %s variableNameString = %s \n",nodeName.c_str(),variableNameString.c_str());
#endif

  // Rasmussen (8/16-17/2017): Added UntypedSubmoduleDeclaration and UntypedBlockDataDeclaration
  // Rasmussen (9/01/2017): Added SgUntypedPackageDeclaration,SgUntypedStructureDeclaration,SgUntypedTaskDeclaration,SgUntypedUnitDeclaration
  // Rasmussen (12/20/2017): Added SgUntypedExprListExpression
  // Rasmussen (11/20/2018): Added SgUntypedArrayReferenceExpression, SgUntypedForAllStatement, and SgJovialCompoolStatement
  // Rasmussen (05/22/2019): Moved scope from SgUntypedStructureDeclaration to SgUntypedStructureDefinition

  // Except in the root class for the virtual access function.
     if ( (nodeName == "Pragma"                     && variableNameString == "startOfConstruct")   ||
          (nodeName == "Pragma"                     && variableNameString == "endOfConstruct")     ||
          (nodeName == "File"                       && variableNameString == "startOfConstruct")   ||
          (nodeName == "File"                       && variableNameString == "endOfConstruct")     ||
          (nodeName == "LocatedNode"                && variableNameString == "startOfConstruct")   ||
          (nodeName == "LocatedNode"                && variableNameString == "endOfConstruct")     ||
          (nodeName == "QualifiedName"              && variableNameString == "scope") ||
          (nodeName == "InitializedName"            && variableNameString == "scope") ||
          (nodeName == "UntypedFunctionDeclaration" && variableNameString == "scope") ||
          (nodeName == "UntypedModuleDeclaration"   && variableNameString == "scope") ||
          (nodeName == "UntypedSubmoduleDeclaration"&& variableNameString == "scope") ||
          (nodeName == "UntypedBlockDataDeclaration"&& variableNameString == "scope") ||
          (nodeName == "UntypedPackageDeclaration"  && variableNameString == "scope") ||
          (nodeName == "UntypedStructureDefinition" && variableNameString == "scope") ||
          (nodeName == "UntypedTaskDeclaration"     && variableNameString == "scope") ||
          (nodeName == "UntypedUnitDeclaration"     && variableNameString == "scope") ||
          (nodeName == "UntypedBlockStatement"      && variableNameString == "scope") ||
          (nodeName == "UntypedFile"                && variableNameString == "scope") ||
          (nodeName == "TemplateParameter"          && variableNameString == "type")  ||
          (nodeName == "TemplateArgument"           && variableNameString == "type")  ||
          (nodeName == "JavaQualifiedType"          && variableNameString == "type")  ||
          (nodeName == "UntypedExprListExpression"  && variableNameString == "type")  ||
          (nodeName == "UntypedForAllStatement"     && variableNameString == "type")  ||
          (nodeName == "UntypedValueExpression"     && variableNameString == "type")  ||
          (nodeName == "UntypedVariableDeclaration" && variableNameString == "type")  ||
          (nodeName == "UntypedTypedefDeclaration"  && variableNameString == "type")  ||
          (nodeName == "UntypedFunctionDeclaration" && variableNameString == "type")  ||
          (nodeName == "UntypedInitializedName"     && variableNameString == "type")  ||
          (nodeName == "EnumDeclaration"            && variableNameString == "type")  ||
          (nodeName == "TypedefDeclaration"         && variableNameString == "type")  ||
          (nodeName == "ClassDeclaration"           && variableNameString == "type")  ||
          (nodeName == "FunctionDeclaration"        && variableNameString == "type")  ||
          (nodeName == "FunctionTypeSymbol"         && variableNameString == "type")  ||
          (nodeName == "DefaultSymbol"              && variableNameString == "type")  ||
          (nodeName == "AsmExpression"              && variableNameString == "type")  ||
          (nodeName == "AsmGenericSymbol"           && variableNameString == "type")  ||
          (nodeName == "AsmElfSegmentTableEntry"    && variableNameString == "type")  ||
          (nodeName == "JavaTypeExpression"         && variableNameString == "type")  ||
          (nodeName == "TypeExpression"             && variableNameString == "type")  ||
          (nodeName == "AsmElfRelocEntry"           && variableNameString == "type")  ||
          (nodeName == "AsmElfNoteEntry"            && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "xxx"                        && variableNameString == "type")  ||
          (nodeName == "SymbolTable"                && variableNameString == "name")  ||
          (nodeName == "Attribute"                  && variableNameString == "name")  ||
          (nodeName == "Directory"                  && variableNameString == "name")  ||
          (nodeName == "Graph"                      && variableNameString == "name")  ||
          (nodeName == "GraphNode"                  && variableNameString == "name")  ||
          (nodeName == "GraphEdge"                  && variableNameString == "name")  ||
          (nodeName == "TemplateType"               && variableNameString == "name")  ||
          (nodeName == "TypeDefault"                && variableNameString == "name")  ||
          (nodeName == "TypeLabel"                  && variableNameString == "name")  ||
          (nodeName == "InitializedName"            && variableNameString == "name")  ||
          (nodeName == "JavaMemberValuePair"        && variableNameString == "name")  ||
          (nodeName == "JovialCompoolStatement"     && variableNameString == "name")  ||
          (nodeName == "UntypedArrayReferenceExpression" && variableNameString == "name")  ||
          (nodeName == "UntypedReferenceExpression" && variableNameString == "name")  ||
          (nodeName == "UntypedFunctionDeclaration" && variableNameString == "name")  ||
          (nodeName == "UntypedModuleDeclaration"   && variableNameString == "name")  ||
          (nodeName == "UntypedSubmoduleDeclaration"&& variableNameString == "name")  ||
          (nodeName == "UntypedBlockDataDeclaration"&& variableNameString == "name")  ||
          (nodeName == "UntypedPackageDeclaration"  && variableNameString == "name")  ||
          (nodeName == "UntypedStructureDeclaration"&& variableNameString == "name")  ||
          (nodeName == "UntypedTypedefDeclaration"  && variableNameString == "name")  ||
          (nodeName == "UntypedTaskDeclaration"     && variableNameString == "name")  ||
          (nodeName == "UntypedUnitDeclaration"     && variableNameString == "name")  ||
          (nodeName == "UntypedInitializedName"     && variableNameString == "name")  ||
          (nodeName == "UntypedName"                && variableNameString == "name")  ||
          (nodeName == "EnumDeclaration"            && variableNameString == "name")  ||
          (nodeName == "TemplateDeclaration"        && variableNameString == "name")  ||
          (nodeName == "UseStatement"               && variableNameString == "name")  ||
          (nodeName == "NamespaceDeclarationStatement" && variableNameString == "name")  ||
          (nodeName == "InterfaceStatement"         && variableNameString == "name")  ||
          (nodeName == "NamespaceAliasDeclarationStatement" && variableNameString == "name")  ||
          (nodeName == "TypedefDeclaration"         && variableNameString == "name")  ||
          (nodeName == "ClassDeclaration"           && variableNameString == "name")  ||
          (nodeName == "FunctionDeclaration"        && variableNameString == "name")  ||
          (nodeName == "JavaPackageStatement"       && variableNameString == "name")  ||
          (nodeName == "InquireStatement"           && variableNameString == "name")  ||
          (nodeName == "OmpCriticalStatement"       && variableNameString == "name")  ||
          (nodeName == "EnumVal"                    && variableNameString == "name")  ||
          (nodeName == "IOItemExpression"           && variableNameString == "name")  ||
          (nodeName == "AsmOp"                      && variableNameString == "name")  ||
          (nodeName == "UnknownArrayOrFunctionReference" && variableNameString == "name")  ||
          (nodeName == "TypeTraitBuiltinOperator"   && variableNameString == "name")  ||
          (nodeName == "FunctionTypeSymbol"         && variableNameString == "name")  ||
          (nodeName == "AdaPackageBodyDecl"         && variableNameString == "name")  ||
          (nodeName == "AdaPackageSpecDecl"         && variableNameString == "name")  ||
          (nodeName == "AdaTaskBodyDecl"            && variableNameString == "name")  ||
          (nodeName == "AdaTaskSpecDecl"            && variableNameString == "name")  ||
          (nodeName == "AdaTaskTypeDecl"            && variableNameString == "name")  ||
          (nodeName == "AdaRenamingDecl"            && variableNameString == "name")  ||
          (nodeName == "AsmFunction"                && variableNameString == "name")  ||
          (nodeName == "AsmSynthesizedFieldDeclaration" && variableNameString == "name")  ||
          (nodeName == "AsmGenericFile"             && variableNameString == "name")  ||
          (nodeName == "AsmElfSymverNeededAux"      && variableNameString == "name")  ||
          (nodeName == "AsmPESectionTableEntry"     && variableNameString == "name")  ||
          (nodeName == "AsmElfSymverDefinedAux"     && variableNameString == "name")  ||
          (nodeName == "AsmPEExportDirectory"       && variableNameString == "name")  ||
          (nodeName == "AsmDwarfConstruct"          && variableNameString == "name")  ||
          (nodeName == "AsmPEImportItem"            && variableNameString == "name")  )
       {
         returnResult = false;
#if 0
         printf ("Exiting as a test! \n");
         ROSE_ASSERT(false);
#endif
       }

     return returnResult;
   }


StringUtility::FileWithLineNumbers
Grammar::buildStringForDataAccessFunctionDeclaration ( AstNodeClass & node )
   {
  // This function builds the strings representing the data access function prototypes

  // Save the original setting
  // bool originalSetting = node.getIncludeInitializerInDataStrings();

  // Mark that the formation of data strings should include their initializers
  // (e.g.  int x = 0; where the " = 0" is the initializer).  Sometimes we need these
  // and sometimes it would be an error (in C++) to generate code that included them.
  // node.setIncludeInitializerInDataStrings (includeInitializer);

  // BP : 10/09/2001, modified to provide adddress
     vector<GrammarString *> dataMemberList = buildListFromLists ( node, &AstNodeClass::getMemberDataPrototypeList );

     StringUtility::FileWithLineNumbers returnString;
     vector<GrammarString *>::iterator dataMemberIterator;

     for( dataMemberIterator = dataMemberList.begin();
          dataMemberIterator != dataMemberList.end();
          dataMemberIterator++ )
        {
          GrammarString & data = **dataMemberIterator;

          string codeString = data.getDataAccessFunctionPrototypeString();
#if 0
          printf ("codeString = %s \n",codeString.c_str());
#endif
       // DQ (3/22/2017): Do the edits for the data member access function protytypes to add "override" keyword.
          bool use_override_keyword = generate_override_keyword(node,data);
#if 0
          printf ("In Grammar::buildStringForDataAccessFunctionDeclaration(): use_override_keyword = %s \n",use_override_keyword ? "true" : "false");
#endif
          if (use_override_keyword == false)
             {
               codeString = GrammarString::copyEdit(codeString, " $ROSE_OVERRIDE_GET", "");
#if 0
               printf ("Modified (get) codeString = %s \n",codeString.c_str());
#endif
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

          bool use_override_keyword_for_set_functions = generate_override_keyword_for_set_functions(node,data);
          if (use_override_keyword_for_set_functions == false)
             {
               codeString = GrammarString::copyEdit(codeString, " $ROSE_OVERRIDE_SET", "");
#if 0
               printf ("Modified (set) codeString = %s \n",codeString.c_str());
#endif
#if 0
               printf ("Exiting as a test! \n");
               ROSE_ASSERT(false);
#endif
             }

       // And surviving references to $ROSE_OVERRIDE_GET and $ROSE_OVERRIDE_SET should be edited to be ROSE_OVERRIDE.
          codeString = GrammarString::copyEdit(codeString, " $ROSE_OVERRIDE_GET", " ROSE_OVERRIDE");
          codeString = GrammarString::copyEdit(codeString, " $ROSE_OVERRIDE_SET", " ROSE_OVERRIDE");

       // StringUtility::FileWithLineNumbers tempString(1, StringUtility::StringWithLineNumber(data.getDataAccessFunctionPrototypeString(), "" /* "<getDataAccessFunctionPrototypeString>" */, 1));
          StringUtility::FileWithLineNumbers tempString(1, StringUtility::StringWithLineNumber(codeString, "" /* "<getDataAccessFunctionPrototypeString>" */, 1));
          returnString += tempString;
        }

     return returnString;
   }


bool
Grammar::buildConstructorParameterList ( AstNodeClass & node, vector<GrammarString *> & constructorParameterList, ConstructParamEnum config )
   {
  // This function is called by the buildConstructorParameterListString(node) function
  // and builds the list of parameters that are used by a constructor.
  // The data exclusiion mechanism implies that excluded data within parents will not appear
  // in the constructor parameters of child grammar elements.

  // The input list shoud be empty
     ROSE_ASSERT(constructorParameterList.size() == 0);

  // Now build the constructor using the data specified at the node and its parents!
  // Use an exclusion mechanism to exclude data before being used within the
  // constructor parameter list (use flag in GrammarString).

     vector<GrammarString *> includeList;
     vector<GrammarString *> excludeList;

  // now generate the additions to the lists from the parent node subtree lists
  // BP : 10/09/2001, modified to provide address
     generateStringListsFromLocalLists ( node, includeList, excludeList, &AstNodeClass::getMemberDataPrototypeList );

  // Now edit the list to remove elements appearing within the exclude list
     editStringList ( includeList, excludeList );

     vector<GrammarString *>::iterator gIt;

     bool complete = true;

     for( gIt = includeList.begin(); gIt != includeList.end(); gIt++)
        {
       // BP : 10/26/2001, tried running with Sun CC and gave the correct results (i.e. generated source correctly)
          GrammarString *memberFunctionCopy= *gIt;
          ROSE_ASSERT (memberFunctionCopy != NULL);

          if (memberFunctionCopy->getIsInConstructorParameterList() == CONSTRUCTOR_PARAMETER)
             {
                constructorParameterList.push_back(memberFunctionCopy);
             }
        }

     return complete;
   }

// NEW CONSTRUCTORS
// this function is used when creating the prototype for the all-data-members constructor.
// it creates the string that is inserted into classname::classname(<string-goes-here>)
string
Grammar::buildConstructorParameterListStringForEssentialDataMembers(AstNodeClass& node, bool withInitializers) {
  string result;
  vector<GrammarString *> includeList;
  vector<GrammarString *> excludeList;
  // now generate the additions to the lists from the parent node subtree lists
  generateStringListsFromLocalLists ( node, includeList, excludeList, &AstNodeClass::getMemberDataPrototypeList );

  int generatedParam=0;
  //cout<<"DEBUG: includeList.size()="<<includeList.size()<<" :: ";
  for(vector<GrammarString *>::iterator gIt = includeList.begin(); gIt != includeList.end(); gIt++) {
#ifndef NDEBUG
    GrammarString *memberFunctionCopy= *gIt;
    ROSE_ASSERT (memberFunctionCopy != NULL);
#endif
    GrammarString& dataMember = **gIt;

    string dataMemberParameter;
    if(withInitializers)
      dataMemberParameter=dataMember.getConstructorPrototypeParameterString();
    else
      dataMemberParameter=dataMember.getConstructorSourceParameterString();

    //string filter="static";
    //bool dataMemberIsStatic=dataMemberParameter.substr(0,filter.size())!=filter;
    if(!isFilteredMemberVariable(dataMember.variableNameString)/*&&!dataMemberIsStatic*/) {
      if(generatedParam>0)
        result+=", ";
      result+=dataMemberParameter;
      generatedParam++;
    }
  }
  //cout<<"RESULT:"<<result<<endl;
  return result;
}

string
Grammar::buildConstructorParameterListString ( AstNodeClass & node, bool withInitializers, bool withTypes, ConstructParamEnum config, bool* complete )
   {
  // This function returns the string used to build the parameters within the constructor.
     int i = 0;
     vector<GrammarString *> constructorParameterList;
     vector<GrammarString *>::iterator stringListIterator;

     bool r = buildConstructorParameterList (node,constructorParameterList, config);
     if (complete != 0)
          *complete = r;

#if ROSE_DEBUG > 0
     for( stringListIterator = constructorParameterList.begin();
          stringListIterator != constructorParameterList.end();
          stringListIterator++ )
        {
          GrammarString & constructorParameter = **stringListIterator;
          string tempConstructorParameterString = constructorParameter.getFunctionNameString();
          printf ("tempConstructorParameterString = %s \n",tempConstructorParameterString.c_str());
        }
#endif

  // Build the parameter string for the constructor
     string constructorParameterString;

     int listSize = constructorParameterList.size();
  // Put the constructor paramteres into the parameter string
     for( stringListIterator = constructorParameterList.begin();
          stringListIterator != constructorParameterList.end();
          stringListIterator++ )
        {
          GrammarString & constructorParameter = **stringListIterator;
       // char* tempConstructorParameterString = constructorParameter.getConstructorParameterString();
          string tempConstructorParameterString = "";
          if (withTypes == true)
             {
               if (withInitializers == true)
                    tempConstructorParameterString = constructorParameter.getConstructorPrototypeParameterString();
                 else
                    tempConstructorParameterString = constructorParameter.getConstructorSourceParameterString();
             }
            else
             {
               ROSE_ASSERT (withInitializers == false);
               tempConstructorParameterString = constructorParameter.getBaseClassConstructorSourceParameterString();
             }

       // printf ("building constructorParameterString tempConstructorParameterString = %s \n",tempConstructorParameterString);

          constructorParameterString += tempConstructorParameterString;

       // If there is another parameter to add in then separate them with a ","
          if (i < listSize-1)
             {
                 // BP : 10/24/2001, delete extra memory
               constructorParameterString += ", ";
             }
          i++;
        }

     return constructorParameterString;
   }


StringUtility::FileWithLineNumbers
Grammar::buildDataMemberVariableDeclarations ( AstNodeClass & node )
   {
  // This function builds a single string containing:
  //    1) Data prototype  (e.g. "int data; $Data* someSageData;")

  // This builds the data declaration (the easy part) e.g. "int dataField; \n char* charField; \n"
  // Spaces and CR's have been added to simplify the final formatting
     StringUtility::FileWithLineNumbers result;
     result.push_back(StringUtility::StringWithLineNumber("    protected:", "" /* "<buildDataMemberVariableDeclarations on " + node.getToken().getName() + ">" */, 1));
     result += buildStringForDataDeclaration(node);

     return result;
   }

StringUtility::FileWithLineNumbers
Grammar::buildMemberAccessFunctionPrototypesAndConstuctorPrototype ( AstNodeClass & node )
   {

  // This function builds a single string containing:
  //    1) Data Access function prototypes (e.g. "void set_data( int data ); int get_data(void); ..." )
  //    2) Constructor prototype (e.g. "$CLASSNAME ( data = 0, $Data* someSageData = NULL );" )
     StringUtility::FileWithLineNumbers dataAccessFunctionPrototypeString = buildStringForDataAccessFunctionDeclaration(node);

     if (node.baseName == "IfStmt")
        {
            if (verbose)
                printf ("In buildMemberAccessFunctionPrototypesAndConstuctorPrototype(): node.name = %s \n",node.name.c_str());
#if 0
          for (size_t i = 0; i < dataAccessFunctionPrototypeString.size(); i++)
             {
               printf ("In buildMemberAccessFunctionPrototypesAndConstuctorPrototype(): dataAccessFunctionPrototypeString[%zu] = \n%s\n",i,dataAccessFunctionPrototypeString[i].toString().c_str());
             }
#endif

#if 0
          printf ("Exiting as a test! \n");
          ROSE_ASSERT(false);
#endif
        }

     string className = node.getName();

  // Build the constructor prototype and then edit the names!
     string destructorPrototype  = "\n     public: \n         virtual ~" + string(className) +  "();\n";

     if (node.generateDestructor() == true)
         dataAccessFunctionPrototypeString.push_back(StringUtility::StringWithLineNumber(destructorPrototype, "" /* "<destructor>" */, 1));

  // Now build the constructor and put in the constructorParameterString
     if (node.generateConstructor() == true)
        {
          bool complete = false;
          ConstructParamEnum cur = CONSTRUCTOR_PARAMETER;
          string constructorPrototype = "\n     public: \n";
#if 1
          bool withInitializers = true;
#else
       // DQ (11/7/2006): Let's try to force use of initializers! So that we
       // can also define constructors that don't take a Sg_File_Info object.
          bool withInitializers = false;
#endif
          bool withTypes        = true;

       // Get the SgLocatedNode so that we can set the data member as not being a constructor
       // parameter so that we can reuse the same code generation source code.
          AstNodeClass* parentNode = getNamedNode ( node, "SgLocatedNode" );
          if (parentNode != NULL)
             {
               GrammarString* returnValue = getNamedDataMember ( *parentNode, "startOfConstruct" );
               ROSE_ASSERT(returnValue != NULL);

            // DQ (11/7/2006): Mark it temporarily as NOT a constructor parameter.
               string defaultInitializer = returnValue->getDefaultInitializerString();
               returnValue->defaultInitializerString = "";

               string constructorParameterString_1 = buildConstructorParameterListString(node,withInitializers,withTypes, cur, &complete);
               constructorPrototype = constructorPrototype + "         " + string(className) + "(" + constructorParameterString_1 + "); \n";

            // Reset "withInitializers" to false and generate a new string for the constructor parameters.
               withInitializers = false;

            // DQ (11/7/2006): Mark it temporarily as NOT a constructor parameter.
               returnValue->setIsInConstructorParameterList(NO_CONSTRUCTOR_PARAMETER);

               string constructorParameterString_2 = buildConstructorParameterListString(node,withInitializers,withTypes, cur, &complete);
               constructorPrototype = constructorPrototype + "         " + string(className) + "(" + constructorParameterString_2 + "); \n";

               /* ESSENTIAL DATA MEMBERS CONSTRUCTOR: generate prototype for all data members constructor */
               if(nameHasPrefix(className,"SgUntyped")) {
                 string constructorParameterString_3 = buildConstructorParameterListStringForEssentialDataMembers(node,false);
                 // ensure that the already generated constructor is not generated again
                 if(constructorParameterString_2!="") {
                   constructorPrototype+=string(className) + "();\n";
                   node.setGenerateEnforcedDefaultConstructorImplementation(true);
                 } else {
                   node.setGenerateEnforcedDefaultConstructorImplementation(false);
                 }
                 if(constructorParameterString_3!=constructorParameterString_2) {
                   constructorPrototype += string(className) + "(" + constructorParameterString_3 + ");\n";
                   node.setGenerateEssentialDataMembersConstructorImplementation(true);
                 } else {
                   node.setGenerateEssentialDataMembersConstructorImplementation(false);
                 }
               }

            // DQ (11/7/2006): Turn it back on as a constructor parameter (and reset the defaultInitializerString)
               returnValue->setIsInConstructorParameterList(CONSTRUCTOR_PARAMETER);
               returnValue->defaultInitializerString = defaultInitializer;
             }
            else
             {
            // If not a SgLocatedNode then output the normal constructor prototype (with all the default arguments).
               string constructorParameterString = buildConstructorParameterListString(node,withInitializers,withTypes, cur, &complete);
               constructorPrototype = constructorPrototype + "         " + string(className) + "(" + constructorParameterString + "); \n";
               withInitializers = false;
             }

          dataAccessFunctionPrototypeString.push_back(StringUtility::StringWithLineNumber(constructorPrototype, "" /* "<constructor>" */, 1));
        } else {
       // no constructor (only generate default constructor)
          if(nameHasPrefix(className,"SgUntyped")) {
            string constructorPrototype=className+"::"+className+"() {}\n";
            //dataAccessFunctionPrototypeString.push_back(StringUtility::StringWithLineNumber(constructorPrototype, "", 1));
          }
        }

#if BUILD_ATERM_SUPPORT
  // DQ (10/7/2014): Adding support for Aterm specific function to build ROSE IR nodes (we want it generated independent of if (node.generateConstructor() == true)).
  // if (node.generateConstructor() == true)
        {
          bool complete = false;
          ConstructParamEnum cur = CONSTRUCTOR_PARAMETER;
          bool withInitializers = true;
          bool withTypes        = true;
          string constructorPrototype = "\n     public: \n";

          string constructorParameterString = buildConstructorParameterListString(node,withInitializers,withTypes, cur, &complete);

          constructorPrototype = constructorPrototype + "         static " + string(className) + "* build_node_from_nonlist_children(" + constructorParameterString + "); \n";

          dataAccessFunctionPrototypeString.push_back(StringUtility::StringWithLineNumber(constructorPrototype, "" /* "<aterm support>" */, 1));
        }
#endif

     return dataAccessFunctionPrototypeString;
   }

void Grammar::constructorLoopBody(const ConstructParamEnum& config, bool& complete, const StringUtility::FileWithLineNumbers& constructorSourceCodeTemplate, AstNodeClass& node, StringUtility::FileWithLineNumbers& returnString) {
  StringUtility::FileWithLineNumbers constructorSource = constructorSourceCodeTemplate;
  if (node.getBaseClass() != NULL) {
    string parentClassName = node.getBaseClass()->getName();
    // printf ("In Grammar::buildConstructor(): parentClassName = %s \n",parentClassName);
    // printf ("Calling base class default constructor (should call paramtererized version) \n");

    string baseClassParameterString;
    bool withInitializers = false;
    bool withTypes        = false;
    baseClassParameterString = buildConstructorParameterListString (*node.getBaseClass(),withInitializers,withTypes, config);
    string preInitializationString = parentClassName + "($BASECLASS_PARAMETERS)";
    preInitializationString = ": " + preInitializationString;
    preInitializationString = GrammarString::copyEdit (preInitializationString,"$BASECLASS_PARAMETERS",baseClassParameterString);
    constructorSource = GrammarString::copyEdit (constructorSource,"$PRE_INITIALIZATION_LIST",preInitializationString);
  } else {
    constructorSource = GrammarString::copyEdit (constructorSource,"$PRE_INITIALIZATION_LIST","");
  }

  bool withInitializers         = false;
  bool withTypes                = true;
  string constructorParameterString = buildConstructorParameterListString (node,withInitializers,withTypes,config,&complete);
  constructorSource = GrammarString::copyEdit (constructorSource,"$CONSTRUCTOR_PARAMETER_LIST",constructorParameterString);
  constructorSource = GrammarString::copyEdit (constructorSource,"$CLASSNAME",node.getName());

  if (config == NO_CONSTRUCTOR_PARAMETER) {
    constructorSource = GrammarString::copyEdit (constructorSource,"$CONSTRUCTOR_BODY","");
  } else {
    string constructorFunctionBody = node.buildConstructorBody(withInitializers, config);
    constructorSource = GrammarString::copyEdit (constructorSource,"$CONSTRUCTOR_BODY",constructorFunctionBody);
  }

  // NEW CONSTRUCTOR: generate IMPLEMENTATION
  string constructorEssentialDataMembers;
  // generate new constructor only for Untyped nodes.
  if(node.baseName.substr(0,7)=="Untyped") {
    string className=node.getName();
    string constructorClassName=className+"::"+className;
    if(node.getGenerateEnforcedDefaultConstructorImplementation()) {
      constructorEssentialDataMembers+=constructorClassName+" () /* ESSENTIAL DATA MEMBERS ENFORCED DEFAULT CONSTRUCTOR */ {}\n";
    }
    if(node.getGenerateEssentialDataMembersConstructorImplementation()) {
      //cout<<"Generating constructor implementation for "<<node.baseName<<endl;
      string constructorParameters
        =buildConstructorParameterListStringForEssentialDataMembers(node,false);
      // check whether constructor already exists
      if(constructorParameters!="") {
        string constructorImpl=node.buildConstructorBodyForEssentialDataMembers();
        constructorEssentialDataMembers+=
           constructorClassName
           +"("+constructorParameters+")"
           +" /* ESSENTIAL DATA MEMBERS CONSTRUCTOR */ "
           +"{\n"+constructorImpl+"}\n";
      }
    }
  }
  constructorSource = GrammarString::copyEdit (constructorSource,"$CONSTRUCTOR_ESSENTIAL_DATA_MEMBERS",constructorEssentialDataMembers);

  returnString.insert(returnString.end(), constructorSource.begin(), constructorSource.end());
}

StringUtility::FileWithLineNumbers
Grammar::buildConstructor ( AstNodeClass & node )
   {
  // Build the constructors for each class
  // Example:
  // /* this is the generated constructor */
  // ClassDeclaration::ClassDeclaration
  //    ( File_Info* info , Name name, int class_type, ClassType* type, ClassDefinition* definition)
  //    : DeclarationStatement(info)
  //    {
  //      p_name = name;
  //      p_class_type = class_type;
  //      p_type = type;
  //      p_definition = definition;
  //   /* now a call to the user defined intialization function */
  //      post_construction_initialization();
  //    }

     string className = node.getName();

     StringUtility::FileWithLineNumbers returnString;

     if (node.generateDestructor() == true)
        {
       // Build the string representing the constructor text (with macro variables)
          string destructorTemplateFileName  = "../Grammar/grammarDestructorDefinitionMacros.macro";
          StringUtility::FileWithLineNumbers destructorSourceCodeTemplate = readFileWithPos (destructorTemplateFileName);

       // edit the string to customize it for this node in the grammar!
          StringUtility::FileWithLineNumbers destructorSource = GrammarString::copyEdit (destructorSourceCodeTemplate,"$CLASSNAME",className);

       // For now make the descructor function body empty
       // AJ (10/27/2004) - Added the destructor body generation
       // char* destructorFunctionBody = "";
          string destructorFunctionBody = node.buildDestructorBody();
          destructorSource = GrammarString::copyEdit (destructorSource,"$DESTRUCTOR_BODY",destructorFunctionBody);

       // printf ("destructorSource = \n%s\n",destructorSource);

          returnString.insert(returnString.end(), destructorSource.begin(), destructorSource.end());
        }

     if (node.generateConstructor() == true)
        {
          string constructorTemplateFileName = "../Grammar/grammarConstructorDefinitionMacros.macro";
          StringUtility::FileWithLineNumbers constructorSourceCodeTemplate = readFileWithPos (constructorTemplateFileName);

          bool complete  = false;
          ConstructParamEnum config = CONSTRUCTOR_PARAMETER;
          if  (node.getBuildDefaultConstructor())
             {
               config = NO_CONSTRUCTOR_PARAMETER;
             }

          if (config == NO_CONSTRUCTOR_PARAMETER)
             {
              constructorLoopBody(NO_CONSTRUCTOR_PARAMETER, complete, constructorSourceCodeTemplate, node, returnString);
             }
            else
             {
               constructorLoopBody(CONSTRUCTOR_PARAMETER, complete, constructorSourceCodeTemplate, node, returnString);
             }
        }

     return returnString;
   }


#if BUILD_ATERM_SUPPORT
StringUtility::FileWithLineNumbers
Grammar::buildAtermConstructor ( AstNodeClass & node )
   {
  // DQ (10/10/2014): This function is only called to generate the skeleton for a
  // small part of the API to translate Aterms to ROSE IR nodes.

  // DQ (10/7/2014): Build the Aterm support static member function (constructor).

     StringUtility::FileWithLineNumbers returnString;

     string constructorTemplateFileName = "../Grammar/grammarAtermConstructorDefinitionMacros.macro";
     StringUtility::FileWithLineNumbers constructorSourceCodeTemplate = readFileWithPos (constructorTemplateFileName);

     bool complete  = false;
     constructorLoopBody(CONSTRUCTOR_PARAMETER, complete, constructorSourceCodeTemplate, node, returnString);

     return returnString;
   }
#endif // BUILD_ATERM_SUPPORT


StringUtility::FileWithLineNumbers
Grammar::buildCopyMemberFunctionSource ( AstNodeClass & node )
   {
  // This function builds the copy function within each class defined by the grammar
  // return node.getToken().buildCopyMemberFunctionSource();

  // char* returnString = node.getToken().buildCopyMemberFunctionSource().c_str();
  // char* returnString = GrammarString::stringDuplicate(node.getToken().buildCopyMemberFunctionSource().c_str());
     StringUtility::FileWithLineNumbers returnString = node.buildCopyMemberFunctionSource();

  // printf ("In Grammar::buildCopyMemberFunctionSource(): returnCppString length = %ld \n",returnCppString.length());

     returnString = GrammarString::copyEdit(returnString,"$CLASSNAME",node.getName());
     returnString = GrammarString::copyEdit(returnString,"$GRAMMAR_PREFIX_",getGrammarPrefixName());

  // printf ("In Grammar::buildCopyMemberFunctionSource(node): returnString = \n%s \n",returnString);

     return returnString;
   }

void
Grammar::buildCopyMemberFunctions ( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
  // printf ("At TOP of Grammar::buildCopyMemberFunctions() \n");
  // printf ("At TOP of Grammar::buildCopyMemberFunctions(): node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName(),node.nodeList.size());

     StringUtility::FileWithLineNumbers editString = buildCopyMemberFunctionSource(node);

  // printf ("editString = %s \n",editString.c_str());

  // outputFile += editString;

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
  // Now write out the file (each class in its own file)!
     string fileExtension = ".C";
     string directoryName = target_directory + sourceCodeDirectoryName();
  // printf ("In buildCopyMemberFunctions(): directoryName = %s \n",directoryName.c_str());

  // This should append the string to the target file.
     appendFile ( editString, directoryName, node.getName(), fileExtension );
#else
     outputFile += editString;
#endif

#if 1
  // Call this function recursively on the children of this node in the tree
     vector<AstNodeClass *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin();
          treeNodeIterator != node.subclasses.end();
          treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          buildCopyMemberFunctions(**treeNodeIterator,outputFile);
        }
#endif
   }

void
Grammar::buildGrammarClassSourceCode ( StringUtility::FileWithLineNumbers & outputFile )
   {
     printf ("This should not be called! \n");
     ROSE_ASSERT(false);

     string fileName  = "../Grammar/grammarMainClassSourceCodeMacros.macro";

  // grammarMainClassParseFunctionSourceCode
     string parseFunctionFileName = "../Grammar/grammarMainClassParseFunctionSourceCode.macro";

     StringUtility::FileWithLineNumbers sourceCodeTemplate = readFileWithPos (fileName);
     StringUtility::FileWithLineNumbers parseFunctionSourceCodeTemplate = readFileWithPos (parseFunctionFileName);

     // BP : 10/25/2001, rewrote to do only one alloc
     StringUtility::FileWithLineNumbers finalOutputString = sourceCodeTemplate;
     if(!isRootGrammar()==true)
       finalOutputString.insert(finalOutputString.end(), parseFunctionSourceCodeTemplate.begin(), parseFunctionSourceCodeTemplate.end());

     finalOutputString = GrammarString::copyEdit (finalOutputString,"$CLASSNAME",getGrammarName());
     finalOutputString = GrammarString::copyEdit (finalOutputString,"$GRAMMAR_BASECLASS",grammarNameBaseClass);
     finalOutputString = GrammarString::copyEdit (finalOutputString,"$GRAMMAR_PREFIX_",getGrammarPrefixName());

     outputFile += finalOutputString;
   }

string
Grammar::getDerivedClassDeclaration ( AstNodeClass & node )
   {
     string derivedClassString;

  // printf ("EDIT className (%s) during copy \n",className);
     if (node.getBaseClass() != NULL)
       derivedClassString = string(": public ") + node.getBaseClass()->getName();

  // printf ("EDIT parentClassName (%s) durring copy \n",parentClassName);

     return derivedClassString;
   }


StringUtility::FileWithLineNumbers
Grammar::buildHeaderStringBeforeMarker( const string& marker, const string& fileName )
   {
     string headerFileInsertionSeparator = marker;
     StringUtility::FileWithLineNumbers headerFileTemplate = readFileWithPos (fileName);

     for (unsigned int i = 0; i < headerFileTemplate.size(); ++i) {
       std::string::size_type pos = headerFileTemplate[i].str.find(headerFileInsertionSeparator);
       if (pos != string::npos) {
         headerFileTemplate.erase(headerFileTemplate.begin() + i + 1, headerFileTemplate.end());
         headerFileTemplate[i].str = headerFileTemplate[i].str.substr(0, pos);
  // headerFileTemplate[i].filename += " before marker " + marker;
         headerFileTemplate.insert(headerFileTemplate.begin(), StringUtility::StringWithLineNumber("", "" /* "<before output of buildHeaderStringBeforeMarker " + marker + " " + fileName + ">" */, 1));
         headerFileTemplate.insert(headerFileTemplate.end(), StringUtility::StringWithLineNumber("", "" /* "<after output of buildHeaderStringBeforeMarker " + marker + " " + fileName + ">" */, 1));
         return headerFileTemplate;
       }
     }
     ROSE_ASSERT (!"Marker not found");

  // DQ (11/28/2009): MSVC warns that this function should return a value from all paths.
     return headerFileTemplate;
   }

StringUtility::FileWithLineNumbers
Grammar::buildHeaderStringAfterMarker( const string& marker, const string& fileName )
   {
     string headerFileInsertionSeparator = marker;
  // char* headerFileTemplate = readFile ("../Grammar/grammarClassDeclatationMacros.macro");
     StringUtility::FileWithLineNumbers headerFileTemplate = readFileWithPos (fileName);

     for (unsigned int i = 0; i < headerFileTemplate.size(); ++i) {
       std::string::size_type pos = headerFileTemplate[i].str.find(headerFileInsertionSeparator);
       if (pos != string::npos) {
         headerFileTemplate.erase(headerFileTemplate.begin(), headerFileTemplate.begin() + i);
         headerFileTemplate[0].str = headerFileTemplate[0].str.substr(pos + headerFileInsertionSeparator.size());
 // headerFileTemplate[0].filename += " after marker " + marker;
         headerFileTemplate.insert(headerFileTemplate.begin(), StringUtility::StringWithLineNumber("", "" /* "<before output of buildHeaderStringAfterMarker " + marker + " " + fileName + ">" */, 1));
         headerFileTemplate.insert(headerFileTemplate.end(), StringUtility::StringWithLineNumber("", "" /* "<after output of buildHeaderStringAfterMarker " + marker + " " + fileName + ">" */, 1));
         return headerFileTemplate;
       }
     }
     ROSE_ASSERT (!"Marker not found");

  // DQ (11/28/2009): MSVC warns that this function should return a value from all paths.
     return headerFileTemplate;
   }

void
Grammar::buildHeaderFiles( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     string marker   = "MEMBER_FUNCTION_DECLARATIONS";
     string fileName = "../Grammar/grammarClassDeclarationMacros.macro";

     StringUtility::FileWithLineNumbers headerBeforeInsertion = buildHeaderStringBeforeMarker(marker,fileName);
     StringUtility::FileWithLineNumbers headerAfterInsertion  = buildHeaderStringAfterMarker (marker,fileName);

#if 1
  // DQ (3/24/2006): Have this be generated from the CommonCode.code file
  // so that we can better control how the documentation is done.
  // Here is where the virtual copy function is added to the header file!
     StringUtility::FileWithLineNumbers copyString = node.buildCopyMemberFunctionHeader();

  // printf ("TEMP String Value: copyString = \n%s\n",copyString);
  // ROSE_ASSERT(false);

     headerBeforeInsertion += copyString;
#endif

  // Edit the $CLASSNAME
     string className = node.getName();

     string derivedClassString = getDerivedClassDeclaration(node);

  // Likely this must happen here since the substitution for BASECLASS is different
  // within the calls to GrammarString::copyEdit() now centralized in editSubstitution()
  // This should be fixed!
     StringUtility::FileWithLineNumbers editStringStart = GrammarString::copyEdit (headerBeforeInsertion,"$BASECLASS",derivedClassString);

  // C preprocessor condition wrapping this entire class declaration.
     string cppCondition = node.getCppCondition();
     if (cppCondition.empty())
         cppCondition = "1";
     editStringStart = GrammarString::copyEdit(editStringStart, "$CPP_CONDITION", cppCondition);

  // calls to GrammarString::copyEdit() now centralized in editSubstitution()
  // BP : 10/24/2001, keep track of memory being freed
     editStringStart = GrammarString::copyEdit (editStringStart,"$CLASSNAME",className);
     StringUtility::FileWithLineNumbers editStringEnd   = GrammarString::copyEdit (headerAfterInsertion,"$CLASSNAME",className);

  // int editedStringMiddleLength = 0;
     StringUtility::FileWithLineNumbers editedStringMiddle;

  // Each of these functions should return a null terminated string
  // (even if there are no code strings associated with this node).

     StringUtility::FileWithLineNumbers editStringMiddleNodeMemberFunctions(1, StringUtility::StringWithLineNumber(buildStringForPrototypes(node), "" /* "<buildStringForPrototypes " + node.getToken().getName() + ">" */, 1));

     editedStringMiddle += editStringMiddleNodeMemberFunctions;

  // DQ (3/24/2006): Separated these functions so that we could position the data member
  // variable declaration at the base of the class and the generated access functions at
  // the top.  This permist us to present the documentation better using Doxygen.
  // string buildDataPrototypesAndAccessFunctionPrototypesAndConstuctorPrototype ( GrammarTreeNode & node );

  // Using the data prototypes we also want to build the constructor
  // call (with the data types and variables as prototypes)
  // string editStringMiddleNodeData = buildDataPrototypesAndAccessFunctionPrototypesAndConstuctorPrototype (node);
     StringUtility::FileWithLineNumbers editStringMiddleNodeMemberFunctionsPrototypes = buildMemberAccessFunctionPrototypesAndConstuctorPrototype(node);

  // DQ (3/25/2006): Change the order back because the *.code files often define enum and typdefs that are
  // required (even though I thought that the order of appearance of such things was unimporant in C++ classes).
  // DQ (3/24/2006): Output the generated code before the code in the *.code files that ROSETTA uses.
  // editedStringMiddle = GrammarString::stringConcatenate (editedStringMiddle,editStringMiddleNodeData.c_str());
  // editedStringMiddle = GrammarString::stringConcatenate (editStringMiddleNodeMemberFunctionsPrototypes.c_str(),editedStringMiddle);
     editedStringMiddle += editStringMiddleNodeMemberFunctionsPrototypes;

  // DQ (3/24/2006): Add the data members to the end of the class in the generated code.
     StringUtility::FileWithLineNumbers editStringMiddleNodeData = buildDataMemberVariableDeclarations(node);
     editedStringMiddle += editStringMiddleNodeData;

  // printf ("editStringMiddleNodeMemberFunctions = %s \n",editStringMiddleNodeMemberFunctions);
  // char* editStringForParserPrototype = buildParserPrototype (node);
  // ROSE_ASSERT (editStringForParserPrototype != NULL);

  // char *tmpString = NULL;
  // printf ("editStringMiddleNodeMemberFunctions = %s \n",editStringMiddleNodeMemberFunctions);
  // printf ("editStringMiddleSubTreeCodeMemberFunctions = %s \n",editStringMiddleSubTreeCodeMemberFunctions);
  // printf ("editStringMiddleNodeData = %s \n",editStringMiddleNodeData);

  // increment the final string with the node specific string
     StringUtility::FileWithLineNumbers editedHeaderFileStringTemp = editStringStart + editedStringMiddle + editStringEnd;

  // Specification of declarations that will appear before or after the node associated class
  // (this issue comes up in SAGE where the Name class must have the postdeclaration string
  // "extern Name defaultName;" so that other classes which follow it can provide default
  // initialization of function parameters).
     StringUtility::FileWithLineNumbers predeclarationString(1, StringUtility::StringWithLineNumber(node.getPredeclarationString (), "" /* "<getPredeclarationString " + node.getToken().getName() + ">" */, 1));

     StringUtility::FileWithLineNumbers editedHeaderFileString = GrammarString::copyEdit (editedHeaderFileStringTemp,"$PREDECLARATIONS" ,predeclarationString);

     StringUtility::FileWithLineNumbers postdeclarationString(1, StringUtility::StringWithLineNumber(node.getPostdeclarationString(), "" /* "<getPostdeclarationString " + node.getToken().getName() + ">" */, 1));
     editedHeaderFileString = GrammarString::copyEdit (editedHeaderFileString,"$POSTDECLARATIONS",postdeclarationString);

#if 0
     printf ("In Grammar::buildHeaderFiles(): className = %s \n",className.c_str());
#endif

  // DQ (3/21/2017): Modify code generation to eliminate Clang C++11 override warning.
     if (className == "SgNode")
        {
          editedHeaderFileString = GrammarString::copyEdit(editedHeaderFileString, " $ROSE_OVERRIDE", "");
        }
       else
        {
          editedHeaderFileString = GrammarString::copyEdit(editedHeaderFileString, " $ROSE_OVERRIDE", " ROSE_OVERRIDE");
        }

     editedHeaderFileString = editSubstitution (node,editedHeaderFileString);

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
  // Now write out the file (each class in its own file)!
     string fileExtension = ".h";
     string directoryName = target_directory + sourceCodeDirectoryName();
  // printf ("In buildHeaderFiles(): directoryName = %s \n",directoryName.c_str());
     writeFile ( editedHeaderFileString, directoryName, node.getName(), fileExtension );
#endif

  // Also output strings to single file (this outputs everything to a single file)
     outputFile += editedHeaderFileString;

     vector<AstNodeClass *>::iterator treeListIterator;
     for( treeListIterator = node.subclasses.begin(); treeListIterator != node.subclasses.end(); treeListIterator++ )
        {
          ROSE_ASSERT ((*treeListIterator) != NULL);
          ROSE_ASSERT ((*treeListIterator)->getBaseClass() != NULL);
          buildHeaderFiles(**treeListIterator,outputFile);
        }
   }

StringUtility::FileWithLineNumbers
Grammar::editSubstitution ( AstNodeClass & node, const StringUtility::FileWithLineNumbers& editStringOrig )
   {
  // Setup default edit variables (locate them here to centralize the process)
     string className          = node.getName();
     string derivedClassString = getDerivedClassDeclaration(node);
     string parentClassName    = (node.getBaseClass() != NULL) ?
                                 node.getBaseClass()->getName() :
                                   "//"; //"NO PARENT AVAILABLE";
     string baseClassConstructorParameterString = "";
     string constructorParameterListString      = "";
     string constructorBodyString               = "";

  // printf ("In editSubstitution: className = %s \n",className);

     StringUtility::FileWithLineNumbers editString = editStringOrig;
     // fprintf(stderr, "Original editString: %s\n", editString.c_str());
     editString = GrammarString::copyEdit (editString,"$CLASSNAME",className);
     editString = GrammarString::copyEdit (editString,"$GRAMMAR_NAME",getGrammarName());  // grammarName string defined in Grammar class
     editString = GrammarString::copyEdit (editString,"$BASECLASS",parentClassName);
     editString = GrammarString::copyEdit (editString,"$BASE_CLASS_CONSTRUCTOR_CALL",derivedClassString);
  // Set these to NULL strings if they are still present within the string
     editString = GrammarString::copyEdit (editString,"$BASE_CLASS_CONSTRUCTOR_PARAMETER",baseClassConstructorParameterString);
     editString = GrammarString::copyEdit (editString,"$CONSTRUCTOR_PARAMETER_LIST",constructorParameterListString);
     editString = GrammarString::copyEdit (editString,"$CONSTRUCTOR_BODY",constructorBodyString);
     editString = GrammarString::copyEdit (editString,"$CLASSTAG",node.getTagName());
     editString = GrammarString::copyEdit (editString,"$CONSTRUCTOR_ESSENTIAL_DATA_MEMBERS","");

  // edit the suffix of the $CLASSNAME (separate from the $GRAMMAR_PREFIX_)
  // printf ("node.getToken().getName() = %s \n",node.getToken().getBaseName());
  // printf ("node.getToken().getName() = %s (%s) \n",node.getToken().getName(),node.getToken().getBaseName());

     editString = GrammarString::copyEdit (editString,"$CLASS_BASE_NAME",node.getBaseName());

  // Fixup the declaration of pure virtual functions (so that they are defined properly at the leaves)
     std::string emptyString       = "";
     std::string pureVirtualMarker = " = 0";

     if (isAstObject(node))
        {
          editString = GrammarString::copyEdit (editString,"$PURE_VIRTUAL_MARKER",emptyString);
        }
       else
        {
          editString = GrammarString::copyEdit (editString,"$PURE_VIRTUAL_MARKER",pureVirtualMarker);
        }

  // Now do final editing/substitution as specified by the user
     ROSE_ASSERT (node.getEditSubstituteTargetList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST).size() ==
                  node.getEditSubstituteSourceList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST).size());
     ROSE_ASSERT (node.getEditSubstituteTargetList(AstNodeClass::SUBTREE_LIST,AstNodeClass::INCLUDE_LIST).size() ==
                  node.getEditSubstituteSourceList(AstNodeClass::SUBTREE_LIST,AstNodeClass::INCLUDE_LIST).size());
     ROSE_ASSERT (node.getEditSubstituteTargetList(AstNodeClass::LOCAL_LIST,AstNodeClass::EXCLUDE_LIST).size() ==
                  node.getEditSubstituteSourceList(AstNodeClass::LOCAL_LIST,AstNodeClass::EXCLUDE_LIST).size());
     ROSE_ASSERT (node.getEditSubstituteTargetList(AstNodeClass::SUBTREE_LIST,AstNodeClass::EXCLUDE_LIST).size() ==
                  node.getEditSubstituteSourceList(AstNodeClass::SUBTREE_LIST,AstNodeClass::EXCLUDE_LIST).size());

  // Local lists that we will accumulate elements into
  // (traversing up through the parents in the grammar tree)
     vector<GrammarString *> targetList;
     vector<GrammarString *> targetExcludeList;
     vector<GrammarString *> sourceList;
     vector<GrammarString *> sourceExcludeList;

  // Initialize with local node data
     targetList        = node.getEditSubstituteTargetList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     targetExcludeList = node.getEditSubstituteTargetList(AstNodeClass::LOCAL_LIST,AstNodeClass::EXCLUDE_LIST);
     sourceList        = node.getEditSubstituteSourceList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     sourceExcludeList = node.getEditSubstituteSourceList(AstNodeClass::LOCAL_LIST,AstNodeClass::EXCLUDE_LIST);

  // now generate the additions to the lists from the parent node subtree lists
     generateStringListsFromSubtreeLists ( node, targetList, targetExcludeList, &AstNodeClass::getEditSubstituteTargetList );
     generateStringListsFromSubtreeLists ( node, sourceList, sourceExcludeList, &AstNodeClass::getEditSubstituteSourceList );

     ROSE_ASSERT (sourceList.size()        == targetList.size());
     ROSE_ASSERT (sourceExcludeList.size() == targetExcludeList.size());

     vector<GrammarString *>::iterator sourceListIterator, targetListIterator;
     for ( sourceListIterator = sourceList.begin(), targetListIterator = targetList.begin();
           sourceListIterator != sourceList.end() || targetListIterator != targetList.end();
           sourceListIterator++, targetListIterator++ )
        {
          // MS 11/22/2015: changed above loop test to check on both iterators (not just check one iterator as before).
          // therefore the following inariant must hold or something is wrong
          ROSE_ASSERT(sourceListIterator!=sourceList.end() && targetListIterator != targetList.end());

          // These are done in the order in which the user specified them!
          // fprintf (stderr, "targetList[index].getFunctionNameString() = %s \n",(*targetListIterator)->getFunctionPrototypeString().c_str());
          // fprintf (stderr, "sourceList[index].getFunctionNameString() = %s \n",(*sourceListIterator)->getFunctionPrototypeString().c_str());

          editString = GrammarString::copyEdit ( editString,
                                  (*targetListIterator)->getFunctionPrototypeString(),
                                  (*sourceListIterator)->getFunctionPrototypeString() );
          // fprintf(stderr, "After edit: %s\n", editString.c_str());
        }

     // Finally, Edit into place the name of the grammar
     // printf ("In editSubstitution node name = %s \n",node.getName());
     editString = GrammarString::copyEdit (editString,"$GRAMMAR_PREFIX_",node.getGrammar()->getGrammarPrefixName());
     editString = GrammarString::copyEdit (editString,"$GRAMMAR_TAG_PREFIX_",node.getGrammar()->getGrammarTagName());

     string parentGrammarPrefix = "";
     if (isRootGrammar() == true)
        {
          // In the case of a root grammar there is no parent
          parentGrammarPrefix = node.getGrammar()->getGrammarPrefixName();
        }
       else
        {
       // Some subsitutions are dependent upon the prefix of the lower level grammar
          ROSE_ASSERT(node.getGrammar() != NULL);
          ROSE_ASSERT(node.getGrammar()->getParentGrammar() != NULL);
          parentGrammarPrefix = node.getGrammar()->getParentGrammar()->getGrammarPrefixName();
        }

     editString = GrammarString::copyEdit (editString,"$PARENT_GRAMMARS_PREFIX_",parentGrammarPrefix);
     editString = GrammarString::copyEdit (editString,"$GRAMMAR_BASECLASS",grammarNameBaseClass);

  // We need to be able to substitute the "X" into some variable names etc.
  // So the following helps to support this feature

     editString = GrammarString::copyEdit (editString,"$CLASSNAME",className);

     return editString;
   }

void
Grammar::buildVariantsStringPrototype ( StringUtility::FileWithLineNumbers & outputFile )
   {
  // DQ (10/26/2007): Add the protytype for the Cxx_GrammarTerminalNames
  // This has been changed to use the newer V_SgNode form of the IR node names.

     string startString = "typedef struct \n" \
                         "   { \n" \
                         "     VariantT variant; \n" \
                         "     std::string name; \n" \
                         "   } TerminalNamesType; \n\n" \
                         "extern TerminalNamesType $MARKERTerminalNames[$LIST_LENGTH]; \n\n";

  // Set the type name using the grammarName variable contained within the grammar
  // startString = GrammarString::copyEdit (startString,"$MARKER",getGrammarPrefixName());
     startString = GrammarString::copyEdit (startString,"$MARKER",getGrammarName());

     size_t maxVariant = this->astVariantToNodeMap.rbegin()->first;

     string listLengthString = StringUtility::numberToString(maxVariant + 2);

  // COPY the length into the string at "LIST_LENGTH"
     startString = GrammarString::copyEdit (startString,"$LIST_LENGTH",listLengthString);

     string finalString = startString;

     outputFile.push_back(StringUtility::StringWithLineNumber(finalString, "", 1));
   }


void
Grammar::buildVariantsStringDataBase ( StringUtility::FileWithLineNumbers & outputFile )
   {
     string startString = "TerminalNamesType $MARKERTerminalNames[$LIST_LENGTH] = {  \n";

  // Set the type name using the grammarName variable contained within the grammar
  // startString = GrammarString::copyEdit (startString,"$MARKER",getGrammarPrefixName());
     startString = GrammarString::copyEdit (startString,"$MARKER",getGrammarName());

     size_t maxVariant = this->astVariantToNodeMap.rbegin()->first;

     string listLengthString = StringUtility::numberToString(maxVariant + 2U);

  // COPY the length into the string at "LIST_LENGTH"
     startString = GrammarString::copyEdit (startString,"$LIST_LENGTH",listLengthString);

     // BP : 10/25/2001, this new version of the code only performs one new memory allocation,
     // uses list iterators instead of the operator[], and should be a lot faster
     string openString      = "          {";
     string separatorString = ", \"";
     string closeString     = "\"}, \n";
     string middleString;

     vector<string> variantNames;
     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
       if (i->first + 1 > variantNames.size()) {
         variantNames.resize(i->first + 1, "<ERROR: unknown VariantT>");
       }
       variantNames[i->first] = i->second;
     }
     for (size_t i=0; i < variantNames.size(); i++) {
       middleString += openString + "(VariantT)" + StringUtility::numberToString(i) + separatorString + variantNames[i] + closeString;
     }

  // string endString = "          {$MARKER_LAST_TAG, \"last tag\" } \n   }; \n\n\n";
     string endString = "          {V_SgNumVariants, \"last tag\" } \n   }; \n\n\n";

     endString = GrammarString::copyEdit (endString,"$MARKER",getGrammarName());

     string finalString = startString + middleString + endString;

     outputFile.push_back(StringUtility::StringWithLineNumber(finalString, "", 1));
   }


void
Grammar::buildSourceFiles( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
  // printf ("At TOP of Grammar::buildSourceFiles() \n");
  // printf ("Exiting at TOP of Grammar::buildSourceFiles() \n");
  // ROSE_ABORT();

     string sourceFileInsertionSeparator = "MEMBER_FUNCTION_DEFINITIONS";
     string fileName = "../Grammar/grammarClassDefinitionMacros.macro";
     StringUtility::FileWithLineNumbers sourceFileTemplate = readFileWithPos (fileName);

  // place this string into a separate locations (isolate it from side effects)
     // BP : 10/25/2001, no need to duplicate here

     StringUtility::FileWithLineNumbers sourceBeforeInsertion;
#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
  // DQ (12/29/2009): Add this to the top of each file.
        // tps (01/06/2010) : If we include sage3.h instead of rose.h on Windows these files are
                // currently only 7MB instead of 17MB - still to large though
         string sourceHeader = "#include \"sage3basic.h\"   // sage3 from grammar.C \nusing namespace std;\n\n";
     sourceBeforeInsertion.push_back(StringUtility::StringWithLineNumber(sourceHeader, "", 1));
#else
  // StringUtility::FileWithLineNumbers sourceBeforeInsertion = buildHeaderStringBeforeMarker(sourceFileInsertionSeparator, fileName);
     sourceBeforeInsertion += buildHeaderStringBeforeMarker(sourceFileInsertionSeparator, fileName);
#endif

     StringUtility::FileWithLineNumbers sourceAfterInsertion = buildHeaderStringAfterMarker(sourceFileInsertionSeparator, fileName);

     string derivedClassString;

  // printf ("Exiting to test editing ... \n");

  // int editedStringMiddleLength = 0;
     StringUtility::FileWithLineNumbers editedStringMiddle;
  // Each of these functions should return a null terminated string
  // (even if there are no code strings associated with this node).
  // char* editStringMiddleNodeMemberFunctions = computeNodeSpecificMemberFunctionsSource (node);

  // At this point data access functions have already been built and placed into the source code lists
  // all that is left is the construction of the code specific to the constructor
  // to build the constructor we require all the data variables
  // (which is why we could not have build it with the access functions)

     StringUtility::FileWithLineNumbers editStringMiddleNodeDataMemberFunctions = buildConstructor (node);

  // printf ("editStringMiddleNodeDataMemberFunctions = %s \n",editStringMiddleNodeDataMemberFunctions);

     StringUtility::FileWithLineNumbers editStringMiddleNodeMemberFunctions = buildStringForSource(node);

  // Place the constructor at the top of the node specific code for this element of grammar

  // BP : 10/24/2001, keep track of memory
     editedStringMiddle += editStringMiddleNodeMemberFunctions;
     editedStringMiddle += editStringMiddleNodeDataMemberFunctions;

  // printf ("editStringMiddleNodeMemberFunctions = %s \n",editStringMiddleNodeMemberFunctions);
  // printf ("editedStringMiddle = %s \n",editedStringMiddle);

  // increment the final string with the node specific string

     StringUtility::FileWithLineNumbers editedSourceFileString = sourceBeforeInsertion + editedStringMiddle;
     editedSourceFileString += sourceAfterInsertion;
  // Now apply the edit/subsitution specified within the grammar (by the user)
     editedSourceFileString = editSubstitution (node,editedSourceFileString);

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
  // Now write out the file!
     string fileExtension = ".C";
     string directoryName = target_directory + sourceCodeDirectoryName();

     writeFile ( editedSourceFileString, directoryName, node.getName(), fileExtension );
#endif

#if 1
// Also output strings to single file
     outputFile += editedSourceFileString;
#endif

  // printf ("node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName(),node.nodeList.size());

#if 1
  // Call this function recursively on the children of this node in the tree
     vector<AstNodeClass *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin(); treeNodeIterator != node.subclasses.end(); treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          buildSourceFiles(**treeNodeIterator,outputFile);
        }
#endif
   }


#if BUILD_ATERM_SUPPORT
void
Grammar::buildAtermBuildFunctionsSourceFile( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
  // DQ (10/10/2014): This function is only called to generate the skeleton for a
  // small part of the API to translate Aterms to ROSE IR nodes.

     printf ("At TOP of Grammar::buildAtermBuildFunctionsSourceFile() \n");

#if 0
     printf ("Exiting at TOP of Grammar::buildAtermBuildFunctionsSourceFile() \n");
     ROSE_ASSERT(false);
#endif

     StringUtility::FileWithLineNumbers editStringMiddleNodeDataMemberFunctions = buildAtermConstructor (node);

  // Also output strings to single file
     outputFile += editStringMiddleNodeDataMemberFunctions;

  // printf ("node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName(),node.nodeList.size());

  // Call this function recursively on the children of this node in the tree
     vector<AstNodeClass *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin(); treeNodeIterator != node.subclasses.end(); treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          buildAtermBuildFunctionsSourceFile(**treeNodeIterator,outputFile);
        }
   }
#endif // BUILD_ATERM_SUPPORT


void
Grammar::printTreeNodeNames ( const AstNodeClass & node ) const
{
  vector<AstNodeClass *>::const_iterator treeNodeIterator;
  int i=0;
  if (node.subclasses.size() > 0)
    {
      printf ("\n");
      printf ("node.name = %s  (# of subtrees/leaves = %zu) \n",node.getName().c_str(),(size_t) node.subclasses.size());
      for( treeNodeIterator = node.subclasses.begin();
           treeNodeIterator != node.subclasses.end();
           treeNodeIterator++ )
        {
          printf ("     node.subclasses[%d] = %s (%s) \n",
                  i, (*treeNodeIterator)->getName().c_str(),
                  ((*treeNodeIterator)->subclasses.size() == 0) ? "IS A LEAF" : "IS NOT A LEAF");
          i++;
        }

      printf ("\n");

      for( treeNodeIterator = node.subclasses.begin();
           treeNodeIterator != node.subclasses.end();
           treeNodeIterator++ )
        {
          printTreeNodeNames(**treeNodeIterator);
        }
    }
}

size_t Grammar::getVariantForNode(const std::string& name) const {
  std::map<std::string, size_t>::const_iterator it = this->astNodeToVariantMap.find(name);
  if (it == this->astNodeToVariantMap.end()) {
    it = this->astNodeToVariantMap.find(this->grammarPrefixName + name);
  }
  if (it == this->astNodeToVariantMap.end()) {
    std::cerr << "Could not find variant number for " << name << std::endl;
    std::cerr << "This node name must be added to the list in $(top_srcdir)/src/ROSETTA/astNodeList" << std::endl;
    abort();
  }
  return it->second;
}

size_t Grammar::getVariantForTerminal(const AstNodeClass& t) const {
  return this->getVariantForNode(t.getName());
}

string Grammar::getNodeForVariant(size_t var) const {
  std::map<size_t, std::string>::const_iterator it = this->astVariantToNodeMap.find(var);
  ROSE_ASSERT (it != this->astVariantToNodeMap.end());
  return it->second;
}

AstNodeClass& Grammar::getTerminalForVariant(size_t var)
   {
     std::map<size_t, AstNodeClass*>::const_iterator it = this->astVariantToTerminalMap.find(var);

  // Note that when this assertion fails it can be because the IR nodes
  // name is listed more than once in the "astNodeList" file.
     ROSE_ASSERT (it != this->astVariantToTerminalMap.end());

     ROSE_ASSERT (it->second);
     return *(it->second);
   }

StringUtility::FileWithLineNumbers
Grammar::buildVariants()
   {
     string header = "//! Variants used to identify elements of the grammar used in ROSE \n" \
                          "/*! Each element is assigned a unique value defined by this enumerated type \n" \
                          "    the values are used to generate the casts from one type toanother were permitted. \n" \
                          "    This is a technique borrowed from the design of SAGE II. \n" \
                          "*/ \n" \
                          "enum $MARKERVariants \n" \
                          "   { \n";

     string footer = "     $MARKER_UNKNOWN_GRAMMAR = " + StringUtility::numberToString(this->astNodeToVariantMap.size() + 1) + ",\n" \
                          "     $MARKER_LAST_TAG \n" \
                          "   }; \n";

     string separatorString = "     ";
     string newlineString   = ",\n";

     unsigned int i=0;

     //     cout << "Grammar::buildVariants (): The current string length is " << stringLength << endl;

     // now allocate the necessary memory
     StringUtility::FileWithLineNumbers returnString;
     returnString.push_back(StringUtility::StringWithLineNumber(header, "" /* "<buildVariants header>" */, 1));

     for (i=0; i < terminalList.size(); i++)
       {
         returnString.push_back(StringUtility::StringWithLineNumber(separatorString + terminalList[i]->getTagName() + " = " + StringUtility::numberToString(this->getVariantForTerminal(*terminalList[i])) + ", ", "" /* "<variant for node type " + terminalList[i].getTagName() + ">" */, 1));
       }

     returnString.push_back(StringUtility::StringWithLineNumber(footer, "" /* "<buildVariants footer>" */, 1));
  // printf ("In Grammar::buildVariants (): returnString = \n%s\n",returnString);

     return returnString;
   }


void
Grammar::buildIncludesForSeparateHeaderFiles( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
  // DQ (12/28/2009): New function to support generation of includes for separate header files.
  // This work is optionally included as an alternative to the generation of huge 300K line files.
  // This is only a performance issue and perhaps an optimization issue for compiling ROSE on
  // machines without large memories (or for 32bit machines).  It is also an attempt to address
  // the complexity of handling Windows and the MSVC compiler (which does not appear to like
  // large single files).

  // printf ("At TOP of Grammar::buildIncludesForSeparateHeaderFiles() \n");

     string includeDerictive = string("#include \"") + sourceCodeDirectoryName() + "/" + node.name + ".h\"";
  // printf ("In Grammar::buildIncludesForSeparateHeaderFiles(): includeDerictive = %s \n",includeDerictive.c_str());
     outputFile.push_back(StringUtility::StringWithLineNumber(includeDerictive,"",1));

  // Call this function recursively on the children of this node in the tree
     vector<AstNodeClass *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin();
          treeNodeIterator != node.subclasses.end();
          treeNodeIterator++ )
        {
          ROSE_ASSERT ((*treeNodeIterator) != NULL);
          ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL);

          buildIncludesForSeparateHeaderFiles(**treeNodeIterator,outputFile);
        }
   }




StringUtility::FileWithLineNumbers
Grammar::buildForwardDeclarations ()
   {
  // DQ (4/23/2006): Need to add forward declarations of "Sg[CLASSNAME]* isSg[CLASSNAME](SgNode*)" friend functions

     string header = "\n\n\n//! Forward Declarations used to represent the grammar used in ROSE \n";

     StringUtility::FileWithLineNumbers returnString;
     returnString.push_back(StringUtility::StringWithLineNumber(header, "" /* "<buildForwardDeclarations header>" */, 1));

     for (unsigned int i=0; i < terminalList.size(); i++)
        {
          returnString.push_back(StringUtility::StringWithLineNumber("class " + terminalList[i]->name + ";", "" /* "<forward decl for " + terminalList[i].name + ">" */, 1));
        }

     returnString.push_back(StringUtility::StringWithLineNumber("\n\n// Forward declaration of \"<classname> is<classname> (SgNode* n)\" friend functions.\n", "" /* "<unknown>" */, 1));
     returnString.push_back(StringUtility::StringWithLineNumber("// GNU g++ 4.1.0 requires these be declared outside of the class (because the friend declaration in the class is not enough).\n\n", "" /* "<unknown>" */, 2));

     returnString.push_back(StringUtility::StringWithLineNumber("\n\n#include \"rosedll.h\"\n", "" /* "<unknown>" */, 1));
     // Milind Chabbi (8/28/2013): Performance refactoring: make rose_ClassHierarchyCastTable accessible for IS_SgXXX_FAST_MACRO()s
     size_t maxRows= getRowsInClassHierarchyCastTable();
     size_t maxCols = getColumnsInClassHierarchyCastTable();
     string externDeclarationForClassHierarchyCastTable = "\nextern const uint8_t rose_ClassHierarchyCastTable[" + StringUtility::numberToString(maxRows) + "][" + StringUtility::numberToString(maxCols) + "] ;\n";
     returnString.push_back(StringUtility::StringWithLineNumber(externDeclarationForClassHierarchyCastTable, "", 1));
     for (unsigned int i=0; i < terminalList.size(); i++)
        {
          string className = terminalList[i]->name;
          returnString.push_back(StringUtility::StringWithLineNumber("ROSE_DLL_API "+className + "* is" + className + "(SgNode* node);", "" /* "<downcast function for " + className + ">" */, 1));
          returnString.push_back(StringUtility::StringWithLineNumber("ROSE_DLL_API const " + className + "* is" + className + "(const SgNode* node);", "" /* "<downcast function for " + className + ">" */, 2));
          // Milind Chabbi (8/28/2013): Performance refactoring.
          // Providing additional MACRO for each isSgXXX() function.
          // One can substitue each isSgXXX() function with IS_SgXXX_FAST_MACRO() function.
          // Being a macro, IS_SgXXX_FAST_MACRO() is unsafe and should be used with care.
          // Using IS_SgXXX_FAST_MACRO() with side effect expressions. e.g., IS_SgXXX_FAST_MACRO(*it++) will cause undefined effects.
          // However, it can be used safely for side effect free expressions e.g., IS_SgXXX_FAST_MACRO(node) and this will improve performance.
          // A good use case of using IS_SgXXX_FAST_MACRO() is in places where isSgXXX() is very heavily used.
          // Substituting all isSgXXX() with IS_SgXXX_FAST_MACRO() worked fine for entire of rose but failed in unsafe uses in tests e.g. src/optimizer/programAnalysis/StencilAnalysis.C
          string fromVariantString = "(node)->variantT()";
          string toVariantString = className +"::static_variant";
          string toVariantBytePositionString = toVariantString + " >> 3";
          string toVariantbitMaskPositionString =  "(1 << (" +  toVariantString + " & 7))";
          string rose_ClassHierarchyCastTableAccessString = " (rose_ClassHierarchyCastTable[" + fromVariantString + "][" + toVariantBytePositionString + "] & " + toVariantbitMaskPositionString   + ")";
          returnString.push_back(StringUtility::StringWithLineNumber("#define IS_" + className + "_FAST_MACRO(node) ( (node) ? ((" + rose_ClassHierarchyCastTableAccessString + ") ? ((" + className + "*) (node)) : NULL) : NULL)", "" /* "<downcast MACRO for " + className + ">" */, 1));
          // One can replace all isSgXXX() with IS_SgXXX_FAST_MACRO() by enabling the line below. This exists for possible future use.
          //returnString.push_back(StringUtility::StringWithLineNumber("#define is" + className + "(node) IS_" + className + "_FAST_MACRO(node)", "" /* "<MACRO replacement for " + className + ">" */, 1));
        }

  // printf ("In Grammar::buildForwardDeclarations (): returnString = \n%s\n",returnString.c_str());
  // ROSE_ASSERT(false);

     return returnString;
   }

string
Grammar::buildTransformationSupport()
   {
  // DQ (11/27/2005): This function builds support text for transformations
  // that change the names of interface and objects as part of a pre-release
  // effort to fixup many details of ROSE.  The goal is to do it at one time
  // and provide the automate mechanism to ROSE users as well.

  // Goal is to generate: "pair<string,string> array[2] = { pair<string,string>("a1","a2"), pair<string,string>("b1","b2") };"

     const string header = "Text to be use in the development of automated translation of interfaces. \n" \
                           "string arrayOfStrings[] \n" \
                           "   { \n";
     const string footer = "   }; \n";

     const string separatorString = "          pair<string,string>(";
     const string newlineString   = "),\n";

     unsigned int i=0;

  // now allocate the necessary memory
     string returnString = header;

     for (i=0; i < terminalList.size(); i++)
        {
          returnString += separatorString;
          returnString += string("\"") + terminalList[i]->getTagName() + string("\"");
          returnString += string(", \"V_") + terminalList[i]->name + string("\"");
          returnString += newlineString;
        }

     returnString += footer;

     return returnString;
   }

StringUtility::FileWithLineNumbers
Grammar::extractStringFromFile (
   const string& startMarker, const string& endMarker,
   const string& filename, const string& directory )
   {
  // Open file
     StringUtility::FileWithLineNumbers fileString = Grammar::readFileWithPos (filename);

  // search for starting marker string
     bool found = false;
     for (unsigned int i = 0; i < fileString.size(); ++i)
        {
          std::string::size_type pos = fileString[i].str.find(startMarker);
          if (pos != string::npos)
             {
               fileString.erase(fileString.begin(), fileString.begin() + i);
               fileString[0].str = fileString[0].str.substr(pos + startMarker.size());
               found = true;
               break;
             }
        }

  // If this is false then the MARKER_*_START strings were not located in the file
     if (found == false)
         throw std::runtime_error("Error: could not locate startMarker = " + startMarker + " in file = " + filename);

     found = false;
     for (unsigned int i = 0; i < fileString.size(); ++i)
        {
          std::string::size_type pos = fileString[i].str.find(endMarker);
          if (pos != string::npos)
             {
               fileString.erase(fileString.begin() + i + 1, fileString.end());
               fileString[i].str = fileString[i].str.substr(0, pos);
               found = true;
               break;
             }
        }

  // If this is false then the MARKER_*_END strings were not located in the file
     if (found == false)
        {
          printf ("Error: could not locate endMarker = %s in file = %s \n",endMarker.c_str(),filename.c_str());
        }
     ROSE_ASSERT (found);

     return fileString;
   }


string
Grammar::getFilenameForGlobalDeclarations()
   {
     return filenameForGlobalDeclarations;
   }

void
Grammar::setFilenameForGlobalDeclarations( const string& filename )
   {
     filenameForGlobalDeclarations = filename;
   }


StringUtility::FileWithLineNumbers
Grammar::buildMiscSupportDeclarations()
   {
  // This function allows the introduction of support classes for the grammar
     StringUtility::FileWithLineNumbers returnString;

  // char* fileName  = "../Grammar/grammarConstructorDeclarationMacros.macro";
  // char* fileName  = getFilenameForSupportClasses();
     string fileName  = getFilenameForGlobalDeclarations();
     returnString = readFileWithPos (fileName);

  // printf ("In (BEFORE EDITING) Grammar::buildMiscSupportDeclarations(): returnString = \n %s \n",returnString);

  // Finally, Edit into place the name of the grammar
     returnString = GrammarString::copyEdit (returnString,"$GRAMMAR_PREFIX_",getGrammarPrefixName());

  // printf ("In (AFTER EDITING) Grammar::buildMiscSupportDeclarations(): returnString = \n %s \n",returnString);

     return returnString;
   }

// MS: new automatically generated variant. Replaces variant().
// used in variantT()
string
Grammar::buildVariantEnums() {
  string s=string("enum VariantT \n{\n");
  unsigned int i;
  bool notFirst=false;
  for (i=0; i < terminalList.size(); i++) {
    if(notFirst) {
      s+=string(",\n");
    }
    notFirst=true;
    size_t varNum = this->getVariantForNode(terminalList[i]->name);
    s+=(string("V_")+terminalList[i]->name+" = "+StringUtility::numberToString(varNum));
  }
  // add an ENUM to get the number of enums declared.
  s+=string(", V_SgNumVariants = ")+StringUtility::numberToString(this->astNodeToVariantMap.size() + 1);
  s+="};\n";
  return s;
}

// Milind Chabbi (8/28/2013): Performance refactoring
// classHierarchyCastTable is a table where each row represents a SgXXX node and each column represents
// if the node can be dynamically casted to the other SgXXX node.
// Sample classHierarchyCastTable
//                   | SgNode | SgStatement | SgBreakStmt |
// --------------------------------------------------------
// SgNode            | true   |  false      | false       |
// SgStatement       | true   |  true       | false       |
// SgBreakStmt       | true   |  true       | true        |
// --------------------------------------------------------
// The table, however, instead of storing booleans, is represented using bits in a byte.


static uint8_t ** classHierarchyCastTable;

// Set the correct bit in classHierarchyCastTable correspinding to the given row and column
static void SetBitInClassHierarchyCastTable(size_t row, size_t col){
    size_t bytePosition = col  >> 3;
    uint8_t bitPosition = col & 7;
    classHierarchyCastTable[row][bytePosition] |=  (1 << bitPosition);
}

#if 0 // [Robb P Matzke 2016-11-06]: unused, so commenting out to avoid compiler warning
// Get the correct bit value from classHierarchyCastTable correspinding to the given row and column
static bool GetBitInClassHierarchyCastTable(size_t row, size_t col){
    size_t bytePosition = col  >> 3;
    size_t bitMask = 1 << (col & 7);
    bool val = classHierarchyCastTable[row][bytePosition] &  bitMask;
    return val;
}
#endif

// Gets the number of rows in classHierarchyCastTable
size_t Grammar::getRowsInClassHierarchyCastTable(){
    return this->astVariantToNodeMap.rbegin()->first;
}

// Gets the number of columns in classHierarchyCastTable
size_t Grammar::getColumnsInClassHierarchyCastTable(){
    return (this->astVariantToNodeMap.rbegin()->first / 8) + 1;
}


// Generates a table (classHierarchyCatTable) populated with information
// about whether a given SgXXX node can be casted to other SgYYY node.
// The technique has constant lookup time.
string
Grammar::generateClassHierarchyCastTable() {
    // First allocate the classHierarchyCastTable table
    size_t maxRows = getRowsInClassHierarchyCastTable();
    size_t maxCols = getColumnsInClassHierarchyCastTable();
    classHierarchyCastTable = new uint8_t* [maxRows];
    for(size_t i = 0 ; i < maxRows; i++){
        classHierarchyCastTable[i] = new uint8_t[maxCols]();
    }

    // Populate classHierarchyCastTable by visiting the Sg node tree
    vector<AstNodeClass*> myParentsDescendents;
    buildClassHierarchyCastTable(getRootOfGrammar(), myParentsDescendents);

    // Output the table as a constant in the generated code
    string s="\nconst uint8_t rose_ClassHierarchyCastTable[" + StringUtility::numberToString(maxRows) + "][" + StringUtility::numberToString(maxCols) + "] = {";
    bool outerLoopFirst = true;
    for(size_t i = 0 ; i < maxRows; i++) {
        if(!outerLoopFirst) {
            s += ",";
        } else {
            outerLoopFirst = false;
        }
        bool innerLoopFirst = true;
        for(size_t j = 0 ; j < maxCols; j++) {
            if (innerLoopFirst){
                s +=  "{";
                innerLoopFirst = false;
            } else {
                s += ",";
            }
            s += StringUtility::numberToString(classHierarchyCastTable[i][j]);
        }
        s += "}\n";
    }
    s += "};\n";

    for(size_t i = 0 ; i < maxRows; i++){
        delete [] classHierarchyCastTable[i];
    }
    delete []classHierarchyCastTable;

    return s;
}

// Populates the classHierarchyCastTable with all the types that can be casted to AstNodeClass type
void Grammar::buildClassHierarchyCastTable(AstNodeClass * astNodeClass, vector<AstNodeClass*> & myParentsDescendents) {
    // obtain the immediate derived classes of the given AstNodeClass.
    vector<AstNodeClass*> myImmediateDescendents = astNodeClass->subclasses;
    vector<AstNodeClass*> myDescendents ;

    // recur on the immediate derived classes to obtain the entire class hierarchy rooted at the given node.
    for(vector<AstNodeClass*>::iterator it = myImmediateDescendents.begin(), e = myImmediateDescendents.end(); it != e; it++){
        buildClassHierarchyCastTable(*it, myDescendents);
    }

    // add self to the vector
    myDescendents.push_back(astNodeClass);

    size_t toVariant= getVariantForTerminal(*astNodeClass);

    // Set the bits in classHierarchyCastTable indicating all derived types in this subtree can be casted to the type of the AstNodeClass.
    for(vector<AstNodeClass*>::iterator it = myDescendents.begin(), e = myDescendents.end(); it != e; it++){
        size_t fromVariant= getVariantForTerminal(*(*it));
        SetBitInClassHierarchyCastTable(fromVariant, toVariant);
    }
    // return the list of all types rooted at this subtree to the caller
    myParentsDescendents.insert(myParentsDescendents.end(), myDescendents.begin(), myDescendents.end());
}

// AS: new automatically generated variant. Replaces variant().
// used in variantT()
string
Grammar::buildClassHierarchySubTreeFunction() {

  //The first function which takes a vector reference which is used to
  //return the result.
  string s="void SgNode::getClassHierarchySubTreeFunction( VariantT v, std::vector<VariantT>& subTreeVariants){\n";

  //s+=string("std::vector<VariantT> subTreeVariants;\n");
  s+="switch(v){\n ";
  unsigned int i;
  for (i=0; i < terminalList.size(); i++) {
    // Chabbi & TV : This generated function is causing a lot of ICACHE misses
    // This optimization generates case labels iff needed and rest all fall into "default"
    // TODO: A better way to generate switch case statements is to put common case in the top
    // and put less common into a nested switch inside the default.
    if (terminalList[i]->subclasses.empty())
        continue;

    s+="case " + string("V_")+string(terminalList[i]->name)+":\n";

        s+="{\n";

        for(vector<AstNodeClass*>::iterator iItr = terminalList[i]->subclasses.begin();
            iItr != terminalList[i]->subclasses.end(); ++iItr)
        {
        s+= "subTreeVariants.push_back(V_"+ string((*iItr)->getName()) + ");\n";

        }
        s+="break;\n";
        s+="}\n";
  }
  //Add default case
  s+="default:\n{ }\n";
  s+="}\n\n";

  s+="};\n";

  //Building second function which return a vector. This is a slower call
  //due to creation and destruction of vectords
  s+="\n\n\n\n";

  s+="std::vector<VariantT> SgNode::getClassHierarchySubTreeFunction( VariantT v){\n";
  s+="std::vector<VariantT> subTreeVariants;\n";
  s+="getClassHierarchySubTreeFunction(v, subTreeVariants);\n";
  s+="return subTreeVariants;\n";
  s+="}\n";

  return s;
}

// AS: new automatically generated variant. Replaces variant().
// used in variantT()
string
Grammar::buildMemoryPoolBasedVariantVectorTraversalSupport() {

  //The first function which takes a vector reference which is used to
  //return the result.
  string s="template <class FunctionalType>\n";
             s+="void AstQueryNamespace::queryMemoryPool(AstQuery<ROSE_VisitTraversal,FunctionalType>& astQuery,";
                 s+=" VariantVector* variantsToTraverse)\n";
                 s+="  {\n";

  //s+=string("std::vector<VariantT> subTreeVariants;\n");
   s+="for (VariantVector::iterator it = variantsToTraverse->begin(); it != variantsToTraverse->end(); ++it)\n";
   s+="  {\n";
   s+="switch(*it){\n ";

  unsigned int i;

  for (i=0; i < terminalList.size(); i++) {
    s+="case " + string("V_")+terminalList[i]->name+": {\n";
    s+="  " + terminalList[i]->name+"::traverseMemoryPoolNodes(astQuery);\n";
    s+="  break;\n";
    s+="}\n";
  }

  //Add default case
  s+="default:\n{\n";
  s+="  // This is a common error after adding a new IR node (because this function should have been automatically generated).\n";
  s+="  std::cout << \"Case not implemented in queryMemoryPool(..). Exiting.\" << std::endl;\n";
  s+="  ROSE_ASSERT(false);\n";
  s+="  break;\n";
  s+="}\n";
  s+="}\n";
  s+="}\n\n";
  s+="};\n";

  return s;
}

// MS: new automatically generated variantnames as variantEnum->string mapping
string
Grammar::buildVariantEnumNames() {
  vector<string> variantNames;
  for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
    if (i->first + 1 > variantNames.size()) {
      variantNames.resize(i->first + 1, "<ERROR: unknown VariantT>");
    }
    variantNames[i->first] = i->second;
  }
  bool first = true;
  string s = "";
  for (size_t i=0; i < variantNames.size(); i++) {
    s+=(first ? "" : string(",\n"))+"\"" + variantNames[i]+string("\"");
    first = false;
  }
  return s;
}

// Build the file for supporting boost::serialization
void
Grammar::buildSerializationSupport(std::ostream &declarations, std::ostream &definitions, const std::string &headerName) {

    // Header prologue
    std::string includeOnce = "ROSE_" + headerName;               // no boost
    for (size_t i=0; i<includeOnce.size(); ++i) {
        if (!isalnum(includeOnce[i]))
            includeOnce[i] = '_';
    }
    if (includeOnce.size()>2 && includeOnce.substr(includeOnce.size()-2)=="_h")
        includeOnce[includeOnce.size()-1] = 'H';
    declarations <<"// Declarations and templates for supporting boost::serialization of Sage IR nodes.\n"
                 <<"#ifndef " <<includeOnce <<"\n"
                 <<"#define " <<includeOnce <<"\n"
                 <<"\n"
                 <<"#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB\n"
                 <<"\n"
                 <<"// sage3basic.h or rose.h must be inlucded first from a .C file (don't do it here!)\n"
                 <<"#include <boost/serialization/export.hpp>\n\n";

    // BOOST_CLASS_EXPORT_KEY
    declarations <<"// The declaration half of exporting polymorphic classes.\n";
    for (size_t i=0; i<terminalList.size(); ++i) {
        if (terminalList[i]->isBoostSerializable())
            declarations <<"BOOST_CLASS_EXPORT_KEY(" <<terminalList[i]->name <<");\n";
    }
    declarations <<"\n\n";

    // roseAstSerializationRegistration
    declarations <<"/** Register all Sage IR node types for serialization.\n"
                 <<" *\n"
                 <<" *  This function should be called before any Sage IR nodes are serialized or deserialized. It\n"
                 <<" *  registers all SgNode subclasses that might be serialized through a base pointer. Note that\n"
                 <<" *  registration is required but not sufficient for serialization: the \"serialize\" member\n"
                 <<" *  function template must also be defined. */\n"
                 <<"template<class Archive>\n"
                 <<"void roseAstSerializationRegistration(Archive &archive) {\n";
    for (size_t i=0; i<terminalList.size(); ++i) {
        if (terminalList[i]->isBoostSerializable())
            declarations <<"    archive.template register_type<" <<terminalList[i]->name <<">();\n";
    }
    declarations <<"}\n";

    // BOOST_CLASS_EXPORT_IMPLEMENT
    definitions <<"#include <sage3basic.h>\n"
                <<"#include <" <<headerName <<">\n"
                <<"\n"
                <<"#ifdef ROSE_HAVE_BOOST_SERIALIZATION_LIB\n"
                <<"\n"
                <<"#include <boost/serialization/export.hpp>\n"
                <<"\n\n"
                <<"// Register SgNode and all subclasses so that subclasses can be serialized through a base class pointer.\n";
    for (size_t i=0; i<terminalList.size(); ++i) {
        if (terminalList[i]->isBoostSerializable())
            definitions <<"BOOST_CLASS_EXPORT_IMPLEMENT(" <<terminalList[i]->name <<");\n";
    }
    definitions <<"\n"
                <<"#endif\n";

    // Header epilogue
    declarations <<"\n"
                 <<"#endif\n"
                 <<"#endif\n";
}

// PC: new implementation of ReferenceToPointerHandler.  This implementation
// allows you to use a subclass to override the template function
// ReferenceToPointerHandler::apply, which is not usually possible.
// This is done by defining an overloaded set of virtual functions each
// corresponding to a particular node.  The SimpleReferenceToPointerHandler
// class provides ReferenceToPointerHandler's old behaviour.
// SimpleReferenceToPointerHandler also acts as an example of how to use
// ReferenceToPointerHandlerImpl to write a "virtual template function".
string
Grammar::buildReferenceToPointerHandlerCode()
   {
  // DQ (3/9/2013): Adding support to exclude some code from SWIG.
     string s;
  // s += string("#ifndef ROSE_USE_SWIG_SUPPORT \n\n");
     s += string("#ifndef SWIG \n\n");
     s += "#ifndef REFERENCETOPOINTERHANDLER_DEFINED\n"
                "#define REFERENCETOPOINTERHANDLER_DEFINED\n\n"

                "struct ReferenceToPointerHandler\n"
                "   {\n";

     for (size_t i=0; i < terminalList.size(); i++)
        {
          s +=  "     virtual void apply(" + terminalList[i]->name + " *&r, const SgName &n, bool traverse) = 0;\n";
        }

     s +=     "\n     virtual ~ReferenceToPointerHandler() {}\n"
                "   };\n\n";

     s += "template <class ImplClass>\n"
          "struct ReferenceToPointerHandlerImpl : ReferenceToPointerHandler\n"
          "   {\n";

     for (size_t i=0; i < terminalList.size(); i++)
        {
          s +=  "     void apply(" + terminalList[i]->name + " *&r, const SgName &n, bool traverse)\n"
                "        {\n"
                "          ROSE_ASSERT(this != NULL);\n"
             // "          ROSE_ASSERT(r != NULL);\n"
                "          static_cast<ImplClass *>(this)->genericApply(r, n, traverse);\n"
                "        }\n\n";
        }

     s += "};\n\n"

          "struct SimpleReferenceToPointerHandler : ReferenceToPointerHandlerImpl<SimpleReferenceToPointerHandler>\n"
          "   {\n"
          "     template <typename NodeSubclass>\n"
          "     void genericApply(NodeSubclass*& r, const SgName& n, bool traverse)\n"
          "        {\n"
          "          SgNode* sgn = r;\n"
          "          (*this)(sgn, n, traverse);\n\n"
          "       // DQ (9/7/2016): Make this more specific.\n"
          "       // ROSE_ASSERT (sgn == NULL || dynamic_cast<NodeSubclass*>(sgn));\n"
       // "          printf (\"sgn = %p \\n\",sgn);\n"
       // "          ROSE_ASSERT (sgn != NULL);\n"
       // DQ (9/8/2016): This is not alowed since in the header file the template where
       // this code lives comes before the definitions of the ROSE IR node classes.
       // "          printf (\"sgn->class_name() = %s \\n\",sgn->class_name().c_str());\n"
       // "          ROSE_ASSERT (dynamic_cast<NodeSubclass*>(sgn) != NULL);\n\n"
       // "          r = dynamic_cast<NodeSubclass*>(sgn);\n"
          "          if (r != NULL) \n"
          "             {\n"
          "               r = dynamic_cast<NodeSubclass*>(sgn);\n"
          "             }\n"
       // "            else\n"
       // "             {\n"
       // "               printf (\"sgn = %p \\n\",sgn);\n"
       // "               printf (\"dynamic_cast<NodeSubclass*>(sgn) = %p \\n\",dynamic_cast<NodeSubclass*>(sgn));\n"
       // "             }\n"
          "        }\n\n"

          "     virtual void operator()(SgNode*&, const SgName&, bool) = 0;\n"
          "   };\n\n"

          "#endif // REFERENCETOPOINTERHANDLER_DEFINED\n\n";

  // DQ (3/9/2013): Adding support to exclude some code from SWIG.
     s += string("#endif // endif for ifndef SWIG \n\n");

     return s;
   }


void
Grammar::buildCode ()
   {
  // Build tree representing the type hierarchy
  // buildTree();
  // Get the root node (the only one without a parent)
  // Also, add the grammar prefix to each node
     this->setRootOfGrammar(NULL);
     for (vector<AstNodeClass*>::const_iterator i = terminalList.begin(); i != terminalList.end(); ++i)
        {
          (*i)->addGrammarPrefixToName();
          if ((*i)->getBaseClass() == NULL)
             {
               this->setRootOfGrammar(*i);
               break;
             }
        }
     ROSE_ASSERT (this->getRootOfGrammar());
     ROSE_ASSERT (rootNode != NULL);
  // ROSE_ASSERT (rootNode->parentTreeNode == NULL);
  // printTreeNodeNames(*rootNode);

  // **************************************************************
  //                 AST HEADER FILE GENERATION
  // **************************************************************

     StringUtility::FileWithLineNumbers ROSE_ArrayGrammarHeaderFile;

  // Put in comment block for Doxygen (so that autogenerated
  // grammars can be automatically documented).

  // DQ (12/28/2009): Removed references to files that should be elsewhere to simplify splitting large files generated by ROSETTA..
     string headerString = "// MACHINE GENERATED HEADER FILE --- DO NOT MODIFY! \n\n\n" \
                           "//! AST implementation generated by ROSETTA \n" \
                           "//  (in many this is an object oriented IR based upon Sage II's implementation (Gannon et. al.). \n\n\n" \
                           "#ifndef $IFDEF_MARKER_H \n" \
                           "#define $IFDEF_MARKER_H \n\n";

     string footerString = "\n\n\n#endif // ifndef IFDEF_MARKER_H \n\n\n";

  // Get the strings onto the heap so that copy edit can process it (is this poor design? MS: yes)
     headerString = GrammarString::copyEdit (headerString,"$IFDEF_MARKER",getGrammarName());
     footerString = GrammarString::copyEdit (footerString,"$IFDEF_MARKER",getGrammarName());
     ROSE_ArrayGrammarHeaderFile << headerString;

  // Build (old) ast node variants and write them to the header file ...
  // BP 10/19/2001, bugfix to prevent memory leak
  // OLD variants
     StringUtility::FileWithLineNumbers variantsString = buildVariants();
     variantsString = GrammarString::copyEdit ( variantsString,"$MARKER",getGrammarName());
     ROSE_ArrayGrammarHeaderFile += variantsString;

  // MS: NEW variants
  // build new variants (access with variantT())
     string variantEnumString = buildVariantEnums();
     ROSE_ArrayGrammarHeaderFile << variantEnumString;

  // DQ (10/26/2007): Add the protytype for the Cxx_GrammarTerminalNames
     buildVariantsStringPrototype ( ROSE_ArrayGrammarHeaderFile );

  // char *forwardDeclString = buildForwardDeclarations ();
     StringUtility::FileWithLineNumbers forwardDeclString = buildForwardDeclarations();
  // forwardDeclString = GrammarString::copyEdit (forwardDeclString,string("$MARKER"),string(getGrammarName()));
     forwardDeclString = StringUtility::copyEdit (forwardDeclString,"$MARKER",getGrammarName());
     ROSE_ArrayGrammarHeaderFile += forwardDeclString;
  // delete [] forwardDeclString;

  // JH (01/09/2006) : Adding the declaration of the ParentStorageClass: above!
     ROSE_ArrayGrammarHeaderFile << buildStorageClassDeclarations();

     ROSE_ArrayGrammarHeaderFile << "\n\n";
     ROSE_ArrayGrammarHeaderFile << "ROSE_DLL_API std::ostream& operator<<(std::ostream&, const SgName&);\n\n";

  // DQ (12/6/2003): Added output function for SgBitVector objects
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const std::vector<bool>&);\n\n";

  // DQ (8/3/2005): Added output function for STL set objects
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const std::set<int>&);\n\n";

  // DQ (10/4/2006): Added output function for STL map objects
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const std::map<SgNode*,int>&);\n\n";

  // DQ (10/6/2006): Added output function for STL map objects
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const std::map<SgNode*,std::string>&);\n\n";

  // DQ (3/31/2007): Modified to be a list instead of a set (and added case for list of SgAsmExpression),
  //                 though I am not certain these are even required to be specified.
  // DQ (3/15/2007): Added output function for STL list objects
  // ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const std::set<SgAsmStatement*>&);\n\n";

#ifdef ROSE_BUILD_BINARY_ANALYSIS_SUPPORT
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const Rose_STL_Container<SgAsmStatement*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const Rose_STL_Container<SgAsmExpression*>&);\n\n";
#endif

  // DQ (11/20/2007): Part of support for the Fortran data statement
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const Rose_STL_Container<SgDataStatementObject*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const Rose_STL_Container<SgDataStatementValue*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const Rose_STL_Container<SgCommonBlockObject*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const Rose_STL_Container<SgDimensionObject*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const Rose_STL_Container<SgLabelSymbol*>&);\n\n";
     ROSE_ArrayGrammarHeaderFile << "std::ostream& operator<<(std::ostream&, const Rose_STL_Container<SgFormatItem*>&);\n\n";

  // traversal order
  // ROSE_ArrayGrammarHeaderFile << "typedef enum {preorder, postorder} t_traverseOrder;\n\n";
  // GB (7/6/2007): Changed these values so we can build bitmasks. This makes
  // it possible to define somewhat more general traversals that have both pre
  // and post order components. The user doesn't notice this change.
     ROSE_ArrayGrammarHeaderFile << "typedef enum \n{preorder = 1, postorder = 2, preandpostorder = preorder | postorder} t_traverseOrder;\n\n";

#if 1
  // DQ (12/28/2009): Make this a configure option to use the separate, dramatically smaller but more numerous header files for each IR node.
     StringUtility::FileWithLineNumbers includesForSeparateHeaderFilesString;

  // Note that this macro is defined in sage3.h.
     includesForSeparateHeaderFilesString.push_back(StringUtility::StringWithLineNumber(string("#if ROSE_USING_SMALL_GENERATED_HEADER_FILES"),string(""),1));

  // Build the includes for each of the files (later build in groups of N IR nodes at a time; rather than based on file length).
  // Note that these files must exist so we can't have the include list be dependent upon file length triggered elsewhere in ROSETTA.
  // includesForSeparateHeaderFilesString += buildIncludesForSeparateHeaderFiles();
     buildIncludesForSeparateHeaderFiles(*rootNode,includesForSeparateHeaderFilesString);

  // Close off the list with "#else".
     includesForSeparateHeaderFilesString.push_back(StringUtility::StringWithLineNumber(string("#else // else for ifdef ROSE_USING_SMALL_GENERATED_HEADER_FILES"),string(""),1));

  // includesForSeparateHeaderFilesString.push_back(StringUtility::StringWithLineNumber(string("#endif // endif for ifdef ROSE_USING_SMALL_GENERATED_HEADER_FILES"),string(""),1));
  // includesForSeparateHeaderFilesString = GrammarString::copyEdit ( includesForSeparateHeaderFilesString,"$MARKER",getGrammarName());
     ROSE_ArrayGrammarHeaderFile += includesForSeparateHeaderFilesString;
#endif

  // Now declare the classes representing the terminals and nonterminals within the grammar
     ROSE_ASSERT (rootNode != NULL);
#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
     StringUtility::FileWithLineNumbers ROSE_ArrayGrammarEmptyHeaderFile;
     buildHeaderFiles(*rootNode,ROSE_ArrayGrammarEmptyHeaderFile);
#else
     buildHeaderFiles(*rootNode,ROSE_ArrayGrammarHeaderFile);
#endif

  // DQ (12/29/2009): Attach endif associated with optional separate (smaller) header files.
     ROSE_ArrayGrammarHeaderFile.push_back(StringUtility::StringWithLineNumber(string("#endif // endif for ifdef ROSE_USING_SMALL_GENERATED_HEADER_FILES"),string(""),1));

  // DQ (11/26/2005): Support for visitor pattern.
     string visitorSupport = buildVisitorBaseClass();
     ROSE_ArrayGrammarHeaderFile.push_back(StringUtility::StringWithLineNumber(visitorSupport, "", 1));

     ROSE_ArrayGrammarHeaderFile.push_back(StringUtility::StringWithLineNumber(footerString, "", 1));

     ROSE_ArrayGrammarHeaderFile << buildReferenceToPointerHandlerCode();

  // Now place all global declarations at the base of the
  // header file after all classes have been defined
     StringUtility::FileWithLineNumbers miscSupport = buildMiscSupportDeclarations ();
     ROSE_ArrayGrammarHeaderFile += miscSupport;

     Grammar::writeFile(ROSE_ArrayGrammarHeaderFile, target_directory, getGrammarName(), ".h");


#if 1
  // **************************************************************
  //                 AST SOURCE FILE GENERATION
  // **************************************************************

     StringUtility::FileWithLineNumbers ROSE_ArrayGrammarSourceFile;
  // Now build the source files

     // tps (Feb 23 2009): added rose.h since I had to remove it from the .h header files
     // tps (01/09/2010) : removed it again to optimize binary sizes
     //string includeHeaderFileNameROSE = "rose.h";
     //     string includeHeaderStringROSE =
     //  "// MACHINE GENERATED ROSE SOURCE FILE --- DO NOT MODIFY!\n\n #include \"" + includeHeaderFileNameROSE + "\"\n\n";

     string includeHeaderFileName = "sage3basic.h";
     //     string includeHeaderString = includeHeaderStringROSE+
     string includeHeaderString =
       "// MACHINE GENERATED SOURCE FILE WITH ROSE (Grammar.h)--- DO NOT MODIFY!\n\n#include \"" + includeHeaderFileName + "\"\n\n";
     string includeHeaderStringWithoutROSE =
       "// MACHINE GENERATED SOURCE FILE --- DO NOT MODIFY! (Grammar.C) \n\n";
     //#include \"" + includeHeaderFileName + "\"\n\n";
     //tps (2/23/2010)
     // we cannot add this because in this way we would include sage3basic.h in a header file, which is not allowed for precompiled headers

  // DQ (10/18/2007): These have been moved to the src/frontend/SageIII directory
  // to provde greater parallelism to the make -jn parallel make feature.
  // JH (01/09/2006)
  // string includeHeaderAstFileIO ="#include \"astFileIO/AST_FILE_IO.h\"\n\n";
   //  string includeSage3 ="#include \"Cxx_Grammar.h\"\n\n";
   //  includeHeaderString += includeSage3;

  // string includeHeaderAstFileIO = "#include \"AST_FILE_IO.h\"\n\n";
     string includeHeaderAstFileIO = "#ifndef ROSE_USE_INTERNAL_FRONTEND_DEVELOPMENT\n   #include \"AST_FILE_IO.h\"\n#endif \n";
     includeHeaderString += includeHeaderAstFileIO;

  // DQ (10/14/2010):  This should only be included by source files that require it.
  // This fixed a reported bug which caused conflicts with autoconf macros (e.g. PACKAGE_BUGREPORT).
  // Interestingly it must be at the top of the list of include files.
     includeHeaderString += "// The header file (\"rose_config.h\") should only be included by source files that require it.\n";
     string includeHeader_rose_config ="#include \"rose_config.h\"\n\n";
     includeHeaderString += includeHeader_rose_config;

     string defines1 = "#if _MSC_VER\n";
     string defines2 = "#define USE_CPP_NEW_DELETE_OPERATORS 0\n";
     string defines3 = "#endif\n\n";
     includeHeaderString += defines1;
     includeHeaderString += defines2;
     includeHeaderString += defines3;

     includeHeaderString += "#define ROSE_ALLOC_MEMSET 0\n";
     includeHeaderString += "#define ROSE_ALLOC_TRACE 0\n";
     includeHeaderString += "#define ROSE_ALLOC_AUTH_ALT_SIZE 1\n";
     includeHeaderString += "#define ROSE_DEALLOC_ASSERT 0\n\n";

  // DQ (3/5/2017): Add message stream support for diagnostic messge from the ROSE IR nodes.
  // This allows us to easily convert printf() functions to mprintf() functions that contain
  // the more sophisticated Saywer support for diagnostic messages.
  // Insert:
  //    #undef mprintf
  //    #define mprintf Rose::Diagnostics::mfprintf(Rose::mlog[Rose::Diagnostics::DEBUG])

     string defines4 = "#undef mprintf\n";
     includeHeaderString += defines4;
     string defines5 = "#define mprintf Rose::Diagnostics::mfprintf(Rose::ir_node_mlog[Rose::Diagnostics::DEBUG])\n\n";
     includeHeaderString += defines5;


     includeHeaderString += "\nusing namespace std;\n";

     ROSE_ArrayGrammarSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));

  // Setup the data base of names (linking name strings to grammar element tags)
     buildVariantsStringDataBase(ROSE_ArrayGrammarSourceFile);

  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);
#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
     printf ("Skipping output of source files into Cxx_Grammar.C ...\n");
     StringUtility::FileWithLineNumbers ROSE_ArrayGrammarEmptySourceFile;

  // Output all the source for each IR node into a single smaller file (one for each IR node).
     buildSourceFiles(*rootNode,ROSE_ArrayGrammarEmptySourceFile);
     buildNewAndDeleteOperators(*rootNode,ROSE_ArrayGrammarEmptySourceFile);
     buildCopyMemberFunctions(*rootNode,ROSE_ArrayGrammarEmptySourceFile);
     buildTraverseMemoryPoolSupport(*rootNode,ROSE_ArrayGrammarEmptySourceFile);
     buildStringForCheckingIfDataMembersAreInMemoryPoolSupport(*rootNode,ROSE_ArrayGrammarEmptySourceFile);
#else
     buildSourceFiles(*rootNode,ROSE_ArrayGrammarSourceFile);
#endif
     if (verbose)
         cout << "DONE: buildSourceFiles()" << endl;

  // DQ (5/24/2005): Support for evaluation of memory sizes of IR nodes
     string memoryStorageEvaluationSupport = buildMemoryStorageEvaluationSupport();
     ROSE_ArrayGrammarSourceFile.push_back(StringUtility::StringWithLineNumber(memoryStorageEvaluationSupport, "", 1));

  // DQ (12/23/2005): Build the visitor pattern traversal code (to call the traveral
  // of the memory pools for each IR node)
     string memoryPoolTraversalSupport = buildMemoryPoolBasedTraversalSupport();
     ROSE_ArrayGrammarSourceFile.push_back(StringUtility::StringWithLineNumber(memoryPoolTraversalSupport, "", 1));

     Grammar::writeFile(ROSE_ArrayGrammarSourceFile, target_directory, getGrammarName(), ".C");
#endif

#if 1
   //-----------------------------------------------
   // generate code for the new and delete operators
   //-----------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_NewAndDeleteOperatorSourceFile;
     ROSE_NewAndDeleteOperatorSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));

     ROSE_NewAndDeleteOperatorSourceFile.push_back(StringUtility::StringWithLineNumber("#include \"Cxx_GrammarMemoryPoolSupport.h\"\n", "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
     printf ("When generating small files we combine the New and Delete oporators into the source files above. \n");
#else
     buildNewAndDeleteOperators(*rootNode,ROSE_NewAndDeleteOperatorSourceFile);
#endif
     if (verbose)
         cout << "DONE: buildNewAndDeletOperators()" << endl;

  // printf ("Exiting after building new and delete operators \n");
  // ROSE_ASSERT(false);

  // DQ(10/22/2007): fixed missed variable renaming.
  // Grammar::writeFile(ROSE_ArrayGrammarSourceFile, target_directory, getGrammarName() + "NewAndDeleteOperators", ".C");
     Grammar::writeFile(ROSE_NewAndDeleteOperatorSourceFile, target_directory, getGrammarName() + "NewAndDeleteOperators", ".C");
#endif


#if 1
   //--------------------------------------------
   // generate code for the memory pool traversal
   //--------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_TraverseMemoryPoolSourceFile;

     ROSE_TraverseMemoryPoolSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
     ROSE_TraverseMemoryPoolSourceFile.push_back(StringUtility::StringWithLineNumber("#include \"Cxx_GrammarMemoryPoolSupport.h\"\n", "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
     printf ("When generating small files we combine the Traversal Memory Pool support into the source files above. \n");
#else
     buildTraverseMemoryPoolSupport(*rootNode,ROSE_TraverseMemoryPoolSourceFile);
#endif
     if (verbose)
         cout << "DONE: buildTraverseMemoryPoolSupport()" << endl;

  // printf ("Exiting after building traverse memory pool functions \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_TraverseMemoryPoolSourceFile, target_directory, getGrammarName() + "TraverseMemoryPool", ".C");
#endif


#if 1
  // --------------------------------------------
  // generate code for the memory pool traversal
  // --------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_CheckingIfDataMembersAreInMemoryPoolSourceFile;

     ROSE_CheckingIfDataMembersAreInMemoryPoolSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
     ROSE_CheckingIfDataMembersAreInMemoryPoolSourceFile.push_back(StringUtility::StringWithLineNumber("#include \"Cxx_GrammarMemoryPoolSupport.h\"\n", "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
     printf ("When generating small files we combine the CheckingIfDataMembersAreInMemoryPool support into the source files above. \n");
#else
     buildStringForCheckingIfDataMembersAreInMemoryPoolSupport(*rootNode,ROSE_CheckingIfDataMembersAreInMemoryPoolSourceFile);
#endif
     if (verbose)
         cout << "DONE: buildStringForCheckingIfDataMembersAreInMemoryPoolSupport()" << endl;

  // printf ("Exiting after building code to check data members which are pointers to IR nodes \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_CheckingIfDataMembersAreInMemoryPoolSourceFile, target_directory, getGrammarName() + "CheckingIfDataMembersAreInMemoryPool", ".C");
#endif

#if 1
         // AS (10/22/07)
  // --------------------------------------------
  // generate code for return a list of variants in the class hierarchy subtree
  // --------------------------------------------
     string returnClassHierarchySubTreeFileName = string(getGrammarName()) + "ReturnClassHierarchySubTree.C";
     fstream ROSE_returnClassHierarchySubTreeSourceFile(std::string(target_directory+"/"+returnClassHierarchySubTreeFileName).c_str(),ios::out);
     ROSE_ASSERT (ROSE_returnClassHierarchySubTreeSourceFile.good() == true);

     ROSE_returnClassHierarchySubTreeSourceFile << includeHeaderString;
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     ROSE_returnClassHierarchySubTreeSourceFile << buildClassHierarchySubTreeFunction();
     // Include the classHierarchyCastTable in the file for fast casting between compatible types
     ROSE_returnClassHierarchySubTreeSourceFile << generateClassHierarchyCastTable();
     if (verbose)
         cout << "DONE: buildClassHierarchySubTreeFunction()" << endl;
     ROSE_returnClassHierarchySubTreeSourceFile.close();

  // printf ("Exiting after building code to return data members which are pointers to IR nodes \n");
  // ROSE_ASSERT(false);
#endif

#if 1
         // AS (10/22/07)
  // --------------------------------------------
  // generate code for return a list of variants in the class hierarchy subtree
  // --------------------------------------------
     string memoryPoolTraversalFileName = "AstQueryMemoryPool.h";
     fstream ROSE_memoryPoolTraversalSourceFile(std::string(target_directory+"/"+ memoryPoolTraversalFileName).c_str(),ios::out);
     ROSE_ASSERT (ROSE_memoryPoolTraversalSourceFile.good() == true);

  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     ROSE_memoryPoolTraversalSourceFile << buildMemoryPoolBasedVariantVectorTraversalSupport();
     if (verbose)
         cout << "DONE: buildMemoryPoolBasedVariantVectorTraversalSupport()" << endl;
     ROSE_memoryPoolTraversalSourceFile.close();

  // printf ("Exiting after building code to return data members which are pointers to IR nodes \n");
  // ROSE_ASSERT(false);
#endif



#if 1
  // --------------------------------------------
  // generate code for returning data member pointers to IR nodes
  // --------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_ReturnDataMemberPointersSourceFile;

     ROSE_ReturnDataMemberPointersSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     buildStringForReturnDataMemberPointersSupport(*rootNode,ROSE_ReturnDataMemberPointersSourceFile);
     if (verbose)
         cout << "DONE: buildStringForReturnDataMemberPointersSupport()" << endl;

  // printf ("Exiting after building code to return data members which are pointers to IR nodes \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_ReturnDataMemberPointersSourceFile, target_directory, getGrammarName() + "ReturnDataMemberPointers", ".C");
#endif

#if 1
  // --------------------------------------------
  // generate code for returning data member pointers to IR nodes
  // --------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_ProcessDataMemberReferenceToPointersSourceFile;

     ROSE_ProcessDataMemberReferenceToPointersSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     buildStringForProcessDataMemberReferenceToPointersSupport(*rootNode,ROSE_ProcessDataMemberReferenceToPointersSourceFile);
     if (verbose)
         cout << "DONE: buildStringForProcessDataMemberReferenceToPointersSupport()" << endl;

  // printf ("Exiting after building code to return references data members which are pointers to IR nodes \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_ProcessDataMemberReferenceToPointersSourceFile, target_directory, getGrammarName() + "ProcessDataMemberReferenceToPointers", ".C");
#endif

#if 1
  // --------------------------------------------
  // generate code for getChildIndex at IR nodes
  // --------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_GetChildIndexSourceFile;

     ROSE_GetChildIndexSourceFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

  // DQ(10/22/2007): fixed missed variable renaming.
  // buildStringForGetChildIndexSupport(*rootNode,ROSE_ReturnDataMemberReferenceToPointersSourceFile);
     buildStringForGetChildIndexSupport(*rootNode,ROSE_GetChildIndexSourceFile);
     if (verbose)
         cout << "DONE: buildStringForGetChildIndexSupport()" << endl;

  // printf ("Exiting after building code to get the child index from any IR node \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_GetChildIndexSourceFile, target_directory, getGrammarName() + "GetChildIndex", ".C");
#endif

#if 1
  // --------------------------------------------
  // generate code for the copy member functions
  // --------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_CopyMemberFunctionsSourceFile;

     ROSE_CopyMemberFunctionsSourceFile << includeHeaderString;
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

#if WRITE_SEPARATE_FILES_FOR_EACH_CLASS
     printf ("When generating small files we combine the Copy Member functions into the source files above. \n");
#else
     buildCopyMemberFunctions(*rootNode,ROSE_CopyMemberFunctionsSourceFile);
#endif
     if (verbose)
         cout << "DONE: buildCopyMemberFunctions()" << endl;

  // printf ("Exiting after copy member functions \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_CopyMemberFunctionsSourceFile, target_directory, getGrammarName() + "CopyMemberFunctions", ".C");
#endif


  // ---------------------------------------------------------------------------------------------
  // generate a function for each node in the AST to return the node's successors of the traversal
  // ---------------------------------------------------------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_treeTraversalFunctionsSourceFile;
     if (verbose)
         cout << "Calling buildTreeTraversalFunctions() ..." << endl;
  // Write header string to file (it's the same string as above, we just reuse it)
     ROSE_treeTraversalFunctionsSourceFile << includeHeaderString;

  // DQ (12/31/2005): Insert "using namespace std;" into the source file (but never into the header files!)
     ROSE_treeTraversalFunctionsSourceFile << "\n// Simplify code by using std namespace (never put into header files since it effects users) \nusing namespace std;\n\n";

  // Generate the implementations of the tree traversal functions
     buildTreeTraversalFunctions(*rootNode, ROSE_treeTraversalFunctionsSourceFile);
     if (verbose)
         cout << "DONE: buildTreeTraversalFunctions()" << endl;
     Grammar::writeFile(ROSE_treeTraversalFunctionsSourceFile, target_directory, getGrammarName() + "TreeTraversalSuccessorContainer", ".C");

#if BUILD_ATERM_SUPPORT
  // DQ (10/4/2014): Adding ATerm support via ROSETTA.
  // ---------------------------------------------------------------------------------------------
  // generate a function for each node in the AST to support ATerm read and write operations.
  // ---------------------------------------------------------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_ATermSupportSourceFile;
     if (verbose)
         cout << "Calling buildAtermSupportFunctions() ..." << endl;
  // Write header string to file (it's the same string as above, we just reuse it)
     ROSE_ATermSupportSourceFile << includeHeaderString;

  // DQ (10/4/2014): Insert "using namespace std;" into the source file (but never into the header files!)
     ROSE_ATermSupportSourceFile << "\n// Simplify code by using AtermSupport namespace (never put into header files since it effects users) \nusing namespace AtermSupport;\n\n";

  // Generate the implementations of the ATerm support functions
     buildAtermSupportFunctions(*rootNode, ROSE_ATermSupportSourceFile);
     if (verbose)
         cout << "DONE: buildAtermSupportFunctions()" << endl;
     Grammar::writeFile(ROSE_ATermSupportSourceFile, target_directory, getGrammarName() + "AtermSupport", ".C");

#if 0
     printf ("Exiting as a test in ROSETTA generation of ATerm support! \n");
     ROSE_ASSERT(false);
#endif
#endif // BUILD_ATERM_SUPPORT

  // ---------------------------------------------------------------------------------------------
  // generate what is necessary for SAGE support in AstProcessing classes
  // ---------------------------------------------------------------------------------------------
     if (verbose)
         cout << "building TreeTraversalAccessEnums ... ";
     string treeTraversalClassHeaderFileName = getGrammarName();
     treeTraversalClassHeaderFileName += "TreeTraversalAccessEnums.h";
     ofstream ROSE_treeTraversalClassHeaderFile(string(target_directory+"/"+treeTraversalClassHeaderFileName).c_str());
     ROSE_ASSERT (ROSE_treeTraversalClassHeaderFile.good() == true);
     ROSE_treeTraversalClassHeaderFile << "// GENERATED HEADER FILE --- DO NOT MODIFY!"
                                       << endl << endl;
     ROSE_treeTraversalClassHeaderFile <<  naiveTraverseGrammar(*rootNode, &Grammar::EnumStringForNode);
     if (verbose)
         cout << "finished." << endl;

  // --------------------------------------------
  // generate code for variantT enum names
  // --------------------------------------------
     string variantEnumNamesFileName = string(getGrammarName())+"VariantEnumNames.C";
     ofstream variantEnumNamesFile(string(target_directory+"/"+variantEnumNamesFileName).c_str());
     ROSE_ASSERT(variantEnumNamesFile.good() == true);
     string  variantEnumNames=buildVariantEnumNames();

  // DQ (4/8/2004): Maybe we need a more obscure name to prevent global name space pollution?
     variantEnumNamesFile << "\n#include \"rosedll.h\"\n ROSE_DLL_API const char* roseGlobalVariantNameList[] = { \n" << variantEnumNames << "\n};\n\n";

  // --------------------------------------------
  // generate code for boost::serialization support
  // --------------------------------------------
     {
         string declarationsFileName = string(getGrammarName()) + "Serialization.h";
         string definitionsFileName = string(getGrammarName()) + "Serialization.C";
         ofstream declarationsFile(string(target_directory+"/"+declarationsFileName).c_str());
         ofstream definitionsFile(string(target_directory+"/"+definitionsFileName).c_str());
         ROSE_ASSERT(declarationsFile.good());
         ROSE_ASSERT(definitionsFile.good());
         buildSerializationSupport(declarationsFile, definitionsFile, declarationsFileName);
     }

  // --------------------------------------------
  // generate code for RTI support
  // --------------------------------------------
     string rtiFunctionsSourceFileName = string(getGrammarName())+"RTI.C";
     StringUtility::FileWithLineNumbers rtiFile;
     rtiFile << includeHeaderString;

  // DQ (12/31/2005): Insert "using namespace std;" into the source file (but never into the header files!)
     rtiFile << "\n// Simplify code by using std namespace (never put into header files since it effects users) \nusing namespace std;\n\n";

     buildRTIFile(rootNode, rtiFile);
     if (verbose)
         cout << "DONE: buildRTIFile" << endl;
     Grammar::writeFile(rtiFile, target_directory, getGrammarName() + "RTI", ".C");

#if 0
  // DQ (11/27/2005): Support for renaming transformations for ROSE project
  // part of pre-release work to fixup interface and names of objects within ROSE.
     string transformationSupportFileName = "translationSupport.code";
     ofstream ROSE_TransformationSupportFile(transformationSupportFileName.c_str());
     ROSE_ASSERT(ROSE_TransformationSupportFile.good() == true);
     string transformationSupportString = buildTransformationSupport();
     ROSE_TransformationSupportFile << transformationSupportString;
     ROSE_TransformationSupportFile.close();
#endif

  // ---------------------------------------------------------------------------
  // generate grammar representations (from class hierarchy and node attributes)
  // ---------------------------------------------------------------------------
     // MS: 2002: Generate the grammar that defines the set of all ASTs as dot and latex file
     ofstream GrammarDotFile("grammar.dot");
     ROSE_ASSERT (GrammarDotFile.good());
     buildGrammarDotFile(rootNode, GrammarDotFile);
     if (verbose)
         cout << "DONE: buildGrammarDotFile" << endl;
     ofstream AbstractTreeGrammarFile("generated_abstractcppgrammar.atg");
     ROSE_ASSERT (AbstractTreeGrammarFile.good());
     buildAbstractTreeGrammarFile(rootNode, AbstractTreeGrammarFile);
     if (verbose)
         cout << "DONE: buildAbstractTreeGrammarFile" << endl;
     ofstream sdfTreeGrammarFile("generated_sdf_tree_grammar.rtg");
     ROSE_ASSERT (sdfTreeGrammarFile.good());
     buildSDFTreeGrammarFile(rootNode, sdfTreeGrammarFile);
     if (verbose)
         cout << "DONE: buildSDFTreeGrammarFile" << endl;

#if 1
   // JH (01/18/2006)
   //--------------------------------------------
   // generate IR node constructor that takes a
   // storage class object
   //--------------------------------------------

     StringUtility::FileWithLineNumbers ROSE_ConstructorTakingStorageClassSourceFile;

     ROSE_ConstructorTakingStorageClassSourceFile << includeHeaderString;
     ROSE_ConstructorTakingStorageClassSourceFile << "#include \"Cxx_GrammarMemoryPoolSupport.h\"\n";
  // JH (01/18/2006) Adding additionally the header of StorageClasses
  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

     buildIRNodeConstructorOfStorageClassSource(*rootNode,ROSE_ConstructorTakingStorageClassSourceFile);
     if (verbose)
         cout << "DONE: buildConstructorTakingStorageClass()" << endl;

  // printf ("Exiting after building traverse memory pool functions \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_ConstructorTakingStorageClassSourceFile, target_directory+"/astFileIO/", "SourcesOfIRNodesAstFileIOSupport", ".C");
#endif
#if 1
  // --------------------------------------------
  // generate code for memory pool support header
  // --------------------------------------------
     StringUtility::FileWithLineNumbers ROSE_MemoryPoolSupportFile;
     ROSE_MemoryPoolSupportFile.push_back(StringUtility::StringWithLineNumber(includeHeaderStringWithoutROSE, "", 1));
     ROSE_ASSERT (rootNode != NULL);
     buildStringForMemoryPoolSupport(rootNode,ROSE_MemoryPoolSupportFile);
     if (verbose)
         cout << "DONE: buildStringForMemoryPoolSupport()" << endl;
     Grammar::writeFile(ROSE_MemoryPoolSupportFile, target_directory, getGrammarName() + "MemoryPoolSupport", ".h");
  // --------------------------------------------
  // generate code for memory pool support source
  // --------------------------------------------
     ROSE_MemoryPoolSupportFile.clear();
     ROSE_MemoryPoolSupportFile.push_back(StringUtility::StringWithLineNumber(includeHeaderString, "", 1));
     ROSE_ASSERT (rootNode != NULL);
     buildStringForMemoryPoolSupportSource(rootNode,ROSE_MemoryPoolSupportFile);
     if (verbose)
         cout << "DONE: buildStringForMemoryPoolSupportSource()" << endl;
     Grammar::writeFile(ROSE_MemoryPoolSupportFile, target_directory, getGrammarName() + "MemoryPoolSupport", ".C");
#endif

  /////////////////////////////////////////////////////////////////////////////////////////////
  // JH(10/26/2005): Build files for ast file io
  //   * AST_FILE_IO.h
  //   * AST_FILE_IO.C
  //   * StorageClasses.h
  //   * StorageClasses.C

     Grammar::generateAST_FILE_IOFiles();
     Grammar::generateStorageClassesFiles();
  /////////////////////////////////////////////////////////////////////////////////////////////



#if 1
  // -----------------------------------------------------------------------------------------------------------------------
  // generate code for new form of constructor without source position information (this code generation must be done LAST!)
  // -----------------------------------------------------------------------------------------------------------------------

     StringUtility::FileWithLineNumbers ROSE_NewConstructorsSourceFile;

     ROSE_NewConstructorsSourceFile << includeHeaderString;

  // Now build the source code for the terminals and non-terminals in the grammar
     ROSE_ASSERT (rootNode != NULL);

  // Modify the tree to mark the Sg_File_Info* in the SgLocatedNode to NOT
  // be a constructor parameter. Then regenerate the code.
     markNodeForConstructorWithoutSourcePositionInformationSupport(*rootNode);

     buildConstructorWithoutSourcePositionInformationSupport (*rootNode,ROSE_NewConstructorsSourceFile);
  // cout << "DONE: buildConstructorWithoutSourcePositionInformationSupport()" << endl;

  // printf ("Exiting after building code for new constructors without source position information \n");
  // ROSE_ASSERT(false);
     Grammar::writeFile(ROSE_NewConstructorsSourceFile, target_directory, getGrammarName() + "NewConstructors", ".C");
#endif

#if 1
     string outputClassesAndFieldsSourceFileName = string(getGrammarName()) + "ClassesAndFields.txt";
     ofstream ROSE_outputClassesAndFieldsSourceFile(string(target_directory+"/"+outputClassesAndFieldsSourceFileName).c_str());
     ROSE_ASSERT (ROSE_outputClassesAndFieldsSourceFile.good() == true);

     if (verbose)
         printf ("Building OutputClassesAndFields() \n");
  // outputClassesAndFields ( *rootNode, ROSE_outputClassesAndFieldsSourceFile);
     ROSE_outputClassesAndFieldsSourceFile << outputClassesAndFields ( *rootNode );
#endif

     return;
   }

// MS:2002 (should be member function of GrammarString)
string Grammar::typeStringOfGrammarString(GrammarString* gs)
   {
     string type = gs->getTypeNameString();

     type=GrammarString::copyEdit (type,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
     return type;
   }

// MS: this function should be a member function of GrammarNode (but this requires several
// other functions to be moved there as well. If we need more functions this will be done.
Grammar::GrammarNodeInfo Grammar::getGrammarNodeInfo(AstNodeClass* grammarnode) {
  GrammarNodeInfo info;
  vector<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
  for(vector<GrammarString*>::iterator stringListIterator = includeList.begin();
      stringListIterator != includeList.end();
      stringListIterator++) {
    if ( (*stringListIterator)->getToBeTraversed() == DEF_TRAVERSAL) {
      string stype=typeStringOfGrammarString(*stringListIterator);
      if (isSTLContainer(stype.c_str())) {
        info.numContainerMembers++;
      } else {
        info.numSingleDataMembers++;
     // GB (8/16/2007): added sanity check
        if (info.numContainerMembers > 0) {
          cout << "Error: in grammar tree node " << grammarnode->getName()
              << ": single member " << (*stringListIterator)->variableNameString
              << " marked for traversal follows a container also marked "
              << "for traversal, that's not allowed" << endl;
          ROSE_ASSERT((info.numSingleDataMembers > 0 ?  info.numContainerMembers == 0 : true));
        }
      }
    }
  }
  if (info.numSingleDataMembers > 0 && info.numContainerMembers > 0) {
 // GB (9/11/2007): After a discussion with Dan and Markus we decided that
 // having both single and container members will be allowed temporarily, but
 // only for SgVariableDeclaration. (SgTypedefDeclaration was also involved
 // in the traversal island issue, but it does not have a container member,
 // so we need not mention it in this code.)
    std::string nodeName = grammarnode->getName();

 // DQ (2/7/2011): Added message to report which nodes are in violation of ROSETTA rules.

 /* MS (10/8/2015): turned off the warning. This doesn't break
  anything except the enums for synthesized attributes access. But those
  will be removed. */
 //   printf ("Warning: Detected node violating ROSETTA rules (some exceptions are allowed): nodeName = %s, num-trav-datam:%d, num-trav-container:%d\n",nodeName.c_str(),info.numSingleDataMembers,info.numContainerMembers);

 // DQ (2/7/2011): Added SgExprListExp to the list so that we can support originalExpressionTree data member in SgExpression.
 // Liao I made more exceptions for some OpenMP specific nodes for now
 // The traversal generator has already been changed accordingly.
 //    std::cout << "both single and container members in node " << nodeName << std::endl;
    ROSE_ASSERT(
          nodeName == "SgVariableDeclaration"
     // DQ (12/21/2011): Added exception for SgTemplateVariableDeclaration derived from SgVariableDeclaration.
        ||nodeName == "SgTemplateVariableDeclaration"
        ||nodeName == "SgOmpClauseBodyStatement"
        ||nodeName == "SgOmpParallelStatement"
        ||nodeName == "SgOmpSectionsStatement"
        ||nodeName == "SgOmpTargetStatement"
        ||nodeName == "SgOmpTargetDataStatement"
        ||nodeName == "SgOmpSingleStatement"
        ||nodeName == "SgOmpSimdStatement"
        ||nodeName == "SgOmpTaskStatement"
        ||nodeName == "SgOmpForStatement"
        ||nodeName == "SgOmpForSimdStatement"
        ||nodeName == "SgOmpForSimdStatement"
        ||nodeName == "SgOmpDoStatement"
        ||nodeName == "SgOmpAtomicStatement"
        ||nodeName == "SgExprListExp"
        ||nodeName == "SgAdaTaskSpec" /* \todo \revisit PP */);
  }
  return info;
}

/////////////////////////
// RTI CODE GENERATION //
/////////////////////////
//MS: 2002
void Grammar::buildRTIFile(AstNodeClass* rootNode, StringUtility::FileWithLineNumbers& rtiFile) {
  GrammarSynthesizedAttribute a=BottomUpProcessing(rootNode, &Grammar::generateRTIImplementation);
  string result;
  result += "// generated file\n";
  result += "#include \"rtiHelpers.h\"\n";
  // container in file scope to avoid multiple (200) template instantiation
  // result += "static " + RTIreturnType + " " + RTIContainerName + ";\n\n";
  result += a.text; // synthesized attribute
  rtiFile.push_back(StringUtility::StringWithLineNumber(result, "", 1));
}

Grammar::GrammarSynthesizedAttribute
Grammar::generateRTIImplementation(AstNodeClass* grammarnode, vector<GrammarSynthesizedAttribute> v)
   {
     GrammarSynthesizedAttribute sa;

  // MS: 2002
  // simply traverse includeList and generate the same code as for traversalSuccessorContainer
  // start: generate roseRTI() method
     vector<GrammarString*> includeList=classMemberIncludeList(*grammarnode);
     ostringstream ss;
     ss << RTIreturnType << endl
        << grammarnode->getName() << "::roseRTI() {" << endl;
     ss << RTIreturnType << " " << RTIContainerName << "(" << includeList.size() << ");\n\n";
     // << RTIreturnType << " " << RTIContainerName << ";" << endl
  // ss << RTIContainerName << ".clear();" << endl;
  // if(grammarnode->isNonTerminal()) {
  // (traversed) data member information for current grammar node
     for(vector<GrammarString*>::iterator stringListIterator = includeList.begin(); stringListIterator != includeList.end(); stringListIterator++)
        {
       // do it for all data members
          string type = (*stringListIterator)->getTypeNameString();
          type=GrammarString::copyEdit (type,"$GRAMMAR_PREFIX_",getGrammarPrefixName());
          type=GrammarString::copyEdit (type,"*","");
       // s += string(grammarnode->getName())+" -> "+type
       //   +" [label="+(*stringListIterator)->getVariableNameString()+"];\n";
#if COMPLETERTI
          ss << generateRTICode(*stringListIterator, RTIContainerName, grammarnode->getName(), stringListIterator - includeList.begin());
#endif
        }

     ss << "return "<< RTIContainerName << ";\n}" << endl; // end of function
  // end: roseRTI generation

     string s = string(ss.str());
  // union data of subtree nodes
     for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++)
        {
          s+=(*viter).text;
        }

     sa.grammarnode = grammarnode;
     sa.text = s;

     return sa;
   }

// MS: 2002: generate source for adding RTI information to node (more detailed than C++ RTI info!)
// this info is used in PDF and dot output
string Grammar::generateRTICode(GrammarString* gs, string dataMemberContainerName, string className, size_t index) {
  string memberVariableName=gs->getVariableNameString();
  string typeString=string(gs->getTypeNameString());
  {
    // REPLACE $GRAMMAR_PREFIX_ in typeString by getGrammarPrefixName() (returns char* const)
    StringUtility::copyEdit(typeString, "$GRAMMAR_PREFIX_", getGrammarPrefixName());
  }
  ostringstream ss;

  ss << "doRTI(\"" << memberVariableName << "\", (void*)(&p_" << memberVariableName << "), sizeof(p_" << memberVariableName << "), (void*)this, \"" << className << "\", \"" << typeString << "\", \"p_" << memberVariableName << "\", toStringForRTI(p_" << memberVariableName << "), " << dataMemberContainerName << "[" << index << "]);\n";
#if 0
  ss << "#if ROSE_USE_VALGRIND\n";
  ss << "doUninitializedFieldCheck(\"" << memberVariableName << "\", (void*)(&p_" << memberVariableName << "), sizeof(p_" << memberVariableName << "), (void*)this, \"" << className << "\");\n";
  ss << "#endif\n";
  ss << dataMemberContainerName << ".push_back(RTIMemberData("
  << "\"" << typeString << "\"" << ", "
  << "\"p_" << memberVariableName << "\"" << ", "
  << "toStringForRTI(p_" << memberVariableName << ")"
  << "));" << endl;
#endif
  return ss.str();
}


/////////////////////////////////////////
// MEMORY POOL SUPPORT CODE GENERATION //
/////////////////////////////////////////
// JJW 10/16/2008 -- This just plugs in each class name into a bunch of
// function and data definitions
void Grammar::buildStringForMemoryPoolSupport(AstNodeClass* rootNode, StringUtility::FileWithLineNumbers& file) {
  GrammarSynthesizedAttribute a=BottomUpProcessing(rootNode, &Grammar::generateMemoryPoolSupportImplementation);
  string result;
  result += "// generated file\n";
  result += a.text; // synthesized attribute
  file.push_back(StringUtility::StringWithLineNumber(result, "", 1));
}

void Grammar::buildStringForMemoryPoolSupportSource(AstNodeClass* rootNode, StringUtility::FileWithLineNumbers& file) {
  GrammarSynthesizedAttribute a=BottomUpProcessing(rootNode, &Grammar::generateMemoryPoolSupportImplementationSource);
  string result;
  result += "// generated file\n";
  result += a.text; // synthesized attribute
  file.push_back(StringUtility::StringWithLineNumber(result, "", 1));
}

Grammar::GrammarSynthesizedAttribute
Grammar::generateMemoryPoolSupportImplementation(AstNodeClass* grammarnode, vector<GrammarSynthesizedAttribute> v)
   {
     GrammarSynthesizedAttribute sa;
     StringUtility::FileWithLineNumbers file = extractStringFromFile("HEADER_MEMORY_POOL_SUPPORT_START", "HEADER_MEMORY_POOL_SUPPORT_END", "../Grammar/grammarMemoryPoolSupport.macro", "");
     file = GrammarString::copyEdit (file,"$CLASSNAME",grammarnode->name);
     string s = toString(file);
  // union data of subtree nodes
     for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {s+=(*viter).text;}
     sa.grammarnode = grammarnode;
     sa.text = s;
     return sa;
   }

Grammar::GrammarSynthesizedAttribute
Grammar::generateMemoryPoolSupportImplementationSource(AstNodeClass* grammarnode, vector<GrammarSynthesizedAttribute> v)
   {
     GrammarSynthesizedAttribute sa;
     StringUtility::FileWithLineNumbers file = extractStringFromFile("SOURCE_MEMORY_POOL_SUPPORT_START", "SOURCE_MEMORY_POOL_SUPPORT_END", "../Grammar/grammarMemoryPoolSupport.macro", "");
     file = GrammarString::copyEdit (file,"$CLASSNAME",grammarnode->name);
     string s = toString(file);
  // union data of subtree nodes
     for(vector<GrammarSynthesizedAttribute>::iterator viter=v.begin(); viter!=v.end(); viter++) {s+=(*viter).text;}
     sa.grammarnode = grammarnode;
     sa.text = s;
     return sa;
   }


//======================================================================
// BUILD TRAVERSAL SUCCESSOR CONTAINER CREATION CODE
//======================================================================
// MS: This method is used to write the individual tree traversal functions to
// the specified output file. It only generates the code for creating a
// container of successors (of AST nodes) at run time.
// The C++ inheritance mechanism allows us to only use the local lists for
// introducing data members. Consequently this function only calls
// generateStringListsFromLocalLists() since this is enough.
void
Grammar::buildTreeTraversalFunctions(AstNodeClass& node, StringUtility::FileWithLineNumbers& outputFile)
   {
     string successorContainerName="traversalSuccessorContainer";

     if (isAstObject(node))
        {
       // Determine the data members to be investigated (starting at the root of the grammar)
          vector<GrammarString *> includeList;
          vector<GrammarString *> excludeList;
          vector<GrammarString *>::iterator stringListIterator;

          ROSE_ASSERT(includeList.size() == 0);
          ROSE_ASSERT(excludeList.size() == 0);
       // See the note at the beginning of this member function!
       // BP : 10/09/2001, modified to give address of function
          generateStringListsFromLocalLists(node,includeList,excludeList, &AstNodeClass::getMemberDataPrototypeList);

       // Now edit the lists to remove elements appearing within the exclude list
          editStringList(includeList,excludeList);

       // MS: generate the reduced list of traversed data members
          vector<GrammarString*> traverseDataMemberList;
          for(stringListIterator = includeList.begin(); stringListIterator != includeList.end(); stringListIterator++)
             {
               if ((*stringListIterator)->getToBeTraversed() == DEF_TRAVERSAL)
                  {
                    traverseDataMemberList.push_back(*stringListIterator);
                  }
             }
       // start: generate get_traversalSuccessorContainer() method
          outputFile << "vector<" << grammarPrefixName << "Node*>\n"
                     << node.getName() << "::get_traversalSuccessorContainer() {\n"
                     << "  vector<" << grammarPrefixName << "Node*> " << successorContainerName << ";\n";
       // GB (8/1/2007): Preallocating the memory needed for the traversal successors to avoid frequent reallocations on
       // push_back. This makes things a little more efficient.
          if (traverseDataMemberList.size() > 0)
             {
               outputFile <<"  " <<successorContainerName << ".reserve("
                          << generateNumberOfSuccessorsComputation(traverseDataMemberList, successorContainerName)
                          << ");\n";
             }
          for(vector<GrammarString*>::iterator iter=traverseDataMemberList.begin(); iter!=traverseDataMemberList.end(); iter++)
             {
            // GB (8/13/2007): When generating traversal successors, the right thing is almost always a call to
            // generateTraverseSuccessor(), but there are a few cases where we need extra logic. At the moment
            // these are the type definitions that may occur in typedef or variable declarations.
               GrammarString *gs = *iter;
               string nodeName = node.getName();
               string memberVariableName = gs->getVariableNameString();
               if (nodeName == "SgTypedefDeclaration" && memberVariableName == "declaration")
                  {
                    outputFile << successorContainerName << ".push_back(compute_baseTypeDefiningDeclaration());\n";
                  }
            // else if (nodeName == "SgVariableDeclaration" && memberVariableName == "baseTypeDefiningDeclaration")
               else if ((nodeName == "SgVariableDeclaration" || nodeName == "SgTemplateVariableDeclaration") && memberVariableName == "baseTypeDefiningDeclaration")
                  {
                    outputFile << successorContainerName << ".push_back(compute_baseTypeDefiningDeclaration());\n";
                  }
            // GB (09/26/2007): This case used to be handled by AstSuccessorsSelectors, but that's no good with the
            // index based traversals. Only traverse a class declaration's definition member if the isForward flag is
            // false.
               else if ((nodeName == "SgClassDeclaration" || nodeName == "SgTemplateInstantiationDecl") && memberVariableName == "definition")
                  {
                    outputFile << successorContainerName << ".push_back(compute_classDefinition());\n";
                  }
            // DQ (10/12/2014): Added case to supress handling of the builtin types in the ROSE SgType IR nodes.
               else if ((gs->getTypeNameString() == "static $CLASSNAME*") && memberVariableName == "builtin_type")
                  {
                    outputFile << "  // suppress handling of builtin_type date members \n";
                  }
               else
                  {
                 // normal case
                    outputFile << generateTraverseSuccessor(*iter, successorContainerName);
                  }
             }
          outputFile << "return "<< successorContainerName << ";\n}\n";
       // end: generate get_traversalSuccessorContainer() method


       // start: generate get_traversalSuccessorNamesContainer() method
          outputFile << "vector<string>\n"
                     << node.getName() << "::get_traversalSuccessorNamesContainer() {\n"
                     << "vector<string> " << successorContainerName << ";\n";
       // GB (8/16/2007): Moved the variable i here. It is initialized to the number of single traversal successors and
       // will be the starting index for the numbering of container successors (if any).
          GrammarNodeInfo info = getGrammarNodeInfo(&node);
          if (info.numContainerMembers)
             {
               outputFile << "int i = " << StringUtility::numberToString(info.numSingleDataMembers) << ";\n";
             }
          for(vector<GrammarString*>::iterator iter=traverseDataMemberList.begin(); iter!=traverseDataMemberList.end(); iter++)
             {
               outputFile << generateTraverseSuccessorNames(*iter, successorContainerName);
             }
          outputFile << "return "<< successorContainerName << ";\n}\n"; // end of function
       // end: generate get_traversalSuccessorNamesContainer() method


       // start: generate get_numberOfTraversalSuccessors() method
       // GB (09/25/2007): Added this method.
          outputFile << "size_t\n"
                     << node.getName() << "::get_numberOfTraversalSuccessors() {\n";
          if (traverseDataMemberList.size() > 0)
             {
               outputFile << "return "
                          << generateNumberOfSuccessorsComputation(traverseDataMemberList, successorContainerName)
                          << ";\n";
             }
          else
             {
               outputFile << "return 0;\n";
             }
          outputFile << "}\n";
       // end: generate get_numberOfTraversalSuccessors() method


       // start: generate get_traversalSuccessorByIndex() method
       // GB (09/25/2007): Added this method.
          outputFile << "SgNode *\n"
                     << node.getName() << "::get_traversalSuccessorByIndex(size_t idx) {\n";
          if (traverseDataMemberList.size() > 0)
             {
               GrammarString *gs = traverseDataMemberList.front();
               string typeString = gs->getTypeNameString();
            // Exceptional case first: SgVariableDeclaration, which has a fixed member (that we compute using a special
            // function) followed by a container.
            // if (string(node.getName()) == "SgVariableDeclaration")
               if (string(node.getName()) == "SgVariableDeclaration" || string(node.getName()) == "SgTemplateVariableDeclaration")
                  {
                    outputFile << "if (idx == 0) return compute_baseTypeDefiningDeclaration();\n"
                               << "else return p_variables[idx-1];\n";
                  }
                // Liao, 5/30/2009
               // More exceptional cases for SgOmpClauseBodyStatement and its derived classes
              // We allow them to have mixed members (simple member and container member)
               else if (string(node.getName()) == "SgOmpClauseBodyStatement"
                 ||string(node.getName()) == "SgOmpParallelStatement"
                 ||string(node.getName()) == "SgOmpSingleStatement"
                 ||string(node.getName()) == "SgOmpSimdStatement"
                 ||string(node.getName()) == "SgOmpTaskStatement"
                 ||string(node.getName()) == "SgOmpSectionsStatement"
                 ||string(node.getName()) == "SgOmpTargetStatement"
                 ||string(node.getName()) == "SgOmpTargetDataStatement"
                 ||string(node.getName()) == "SgOmpForStatement"
                 ||string(node.getName()) == "SgOmpForSimdStatement"
                 ||string(node.getName()) == "SgOmpForSimdStatement"
                 ||string(node.getName()) == "SgOmpDoStatement"
                 ||string(node.getName()) == "SgOmpAtomicStatement"
                 )
                  {
                    outputFile << "if (idx == 0) return p_body;\n"
                               << "else return p_clauses[idx-1];\n";
                  }
//             // SgOmpSectionsStatement has two containers: p_sections, p_clauses
//               else if (string(node.getName()) == "SgOmpSectionsStatement")
//                  {
//                    outputFile << "if (idx < p_sections.size()) return p_sections[idx];\n"
//                               << "else return p_clauses[idx - p_sections.size()];\n";
//                  }
               else if (isSTLContainerPtr(typeString.c_str()))
                  {
                    outputFile << "ROSE_ASSERT(idx < p_" << gs->getVariableNameString() << "->size());\n";
                    outputFile << "return (*p_" << gs->getVariableNameString() << ")[idx];\n";
                  }
               else if (isSTLContainer(typeString.c_str()))
                  {
                    outputFile << "ROSE_ASSERT(idx < p_" << gs->getVariableNameString() << ".size());\n";
                    outputFile << "return p_" << gs->getVariableNameString() << "[idx];\n";
                  }
               else
                  {
                 // Fixed members, generate a switch.
                    outputFile << "switch (idx) {\n";
                    vector<GrammarString*>::iterator iter;
                    size_t counter = 0;
                    for (iter = traverseDataMemberList.begin(); iter != traverseDataMemberList.end(); ++iter)
                       {
                         string memberVariableName = (*iter)->getVariableNameString();
                      // Special case: SgTypedefDeclaration has a member that is computed using a special function.
                         if (string(node.getName()) == "SgTypedefDeclaration" && memberVariableName == "declaration")
                            {
                              outputFile << "case " << StringUtility::numberToString(counter++) << ": "
                                         << "return compute_baseTypeDefiningDeclaration();\n";
                            }
                      // Special case: SgClassDeclaration has a member that is computed using a special function. That
                      // member is inherited by SgTemplateInstantiationDecl!
                         else if ((string(node.getName()) == "SgClassDeclaration" || string(node.getName()) == "SgTemplateInstantiationDecl") && memberVariableName == "definition")
                            {
                              outputFile << "case " << StringUtility::numberToString(counter++) << ": "
                                         << "return compute_classDefinition();\n";
                            }
                           else
                            {
                           // DQ (4/22/2014): Added code to allow valgrind to detect unitialized variables.
                           // outputFile << "case " << StringUtility::numberToString(counter++) << ": " << "return p_" << memberVariableName << ";\n";
                              outputFile << "case " << StringUtility::numberToString(counter++) << ": "
                                         << "ROSE_ASSERT(p_" << memberVariableName << " == NULL || p_" << memberVariableName << " != NULL); return p_" << memberVariableName << ";\n";
                            }
                       }
                 // Reaching the default case is an error.
                    outputFile << "default: cout << \"invalid index \" << idx << "
                        << "\" in get_traversalSuccessorByIndex()\" << endl;\n"
                        << "ROSE_ASSERT(false);\n"
                 // DQ (8/31/2009): Added return statement to avoid EDG warning when compiling ROSE with ROSE.
                        << "return NULL;\n";
                 // Close the switch.
                    outputFile << "}\n";
                  }
             }
          else
             {
            // There are no successors, so calling this function was an error. Complain.
               outputFile << "cout << \"error: get_traversalSuccessorByIndex called on node of type \" << \""
                          << node.getName() << "\" << \" that has no successors!\" << endl;\n"
                          << "ROSE_ASSERT(false);\n"
            // DQ (8/31/2009): Added return statement to avoid EDG warning when compiling ROSE with ROSE.
                          << "return NULL;\n";
             }
          outputFile << "}\n";
       // end: generate get_traversalSuccessorByIndex() method


       // start: generate get_childIndex() method
       // GB (09/25/2007): Added this method.
          outputFile << "size_t\n"
                     << node.getName() << "::get_childIndex(SgNode *child) {\n";
          if (traverseDataMemberList.size() > 0)
             {
               GrammarString *gs = traverseDataMemberList.front();
               string typeString = gs->getTypeNameString();
            // Exceptional case first: SgVariableDeclaration, which has a fixed member (that we compute using a special
            // function) followed by a container.
            // if (string(node.getName()) == "SgVariableDeclaration")
               if (string(node.getName()) == "SgVariableDeclaration" || string(node.getName()) == "SgTemplateVariableDeclaration")
                  {
                    outputFile << "if (child == compute_baseTypeDefiningDeclaration()) return 0;\n"
                               << "else {\n"
                               << "SgInitializedNamePtrList::iterator itr = find(p_variables.begin(), p_variables.end(), child);\n"
                               << "if (itr != p_variables.end()) return (itr - p_variables.begin()) + 1;\n"
                               << "else return (size_t) -1;\n"
                               << "}\n";
                  }
               // More exceptional cases for SgOmpClauseBodyStatement and its derived classes
              // We allow them to have mixed members
               else if (string(node.getName()) == "SgOmpClauseBodyStatement"
                 ||string(node.getName()) == "SgOmpParallelStatement"
                 ||string(node.getName()) == "SgOmpSingleStatement"
                 ||string(node.getName()) == "SgOmpSimdStatement"
                 ||string(node.getName()) == "SgOmpTaskStatement"
                 ||string(node.getName()) == "SgOmpSectionsStatement"
                 ||string(node.getName()) == "SgOmpTargetStatement"
                 ||string(node.getName()) == "SgOmpTargetDataStatement"
                 ||string(node.getName()) == "SgOmpForStatement"
                 ||string(node.getName()) == "SgOmpForSimdStatement"
                 ||string(node.getName()) == "SgOmpDoStatement"
                 ||string(node.getName()) == "SgOmpAtomicStatement"
                 )
                  {
                     outputFile << "if (child == p_body) return 0;\n"
                               << "else {\n"
                               << "SgOmpClausePtrList::iterator itr = find(p_clauses.begin(), p_clauses.end(), child);\n"
                               << "if (itr != p_clauses.end()) return (itr - p_clauses.begin()) + 1;\n"
                               << "else return (size_t) -1;\n"
                               << "}\n";
                  }
//               // SgOmpSectionsStatement has two containter members: p_sections, p_clauses
//             else if(string(node.getName()) == "SgOmpSectionsStatement")
//                  {
//                     outputFile<< "SgOmpSectionStatementPtrList::iterator itr1 = find(p_sections.begin(), p_sections.end(), child);\n"
//                               << "if (itr1 != p_sections.end()) return (itr1 - p_sections.begin());\n"
//                               << "else \n "
//                               << "{\n"
//                               << "SgOmpClausePtrList::iterator itr = find(p_clauses.begin(), p_clauses.end(), child);\n"
//                               << "if (itr != p_clauses.end()) return (itr - p_clauses.begin()) + p_sections.size();\n"
//                               << "else return (size_t) -1;\n"
//                               << "}\n";
//                  }
               else if (isSTLContainerPtr(typeString.c_str()))
                  {
                    string memberVariableName = gs->getVariableNameString();
                    string begin = "p_" + memberVariableName + "->begin()";
                    string end = "p_" + memberVariableName + "->end()";
                    outputFile << getIteratorString(typeString.c_str()) << " itr = find(" << begin << ", " << end << ", child);\n"
                               << "if (itr != " << end << ") return itr - " << begin << ";\n"
                               << "else return (size_t) -1;\n";
                  }
               else if (isSTLContainer(typeString.c_str()))
                  {
                    string memberVariableName = gs->getVariableNameString();
                    string begin = "p_" + memberVariableName + ".begin()";
                    string end = "p_" + memberVariableName + ".end()";
                    outputFile << getIteratorString(typeString.c_str()) << " itr = find(" << begin << ", " << end << ", child);\n"
                               << "if (itr != " << end << ") return itr - " << begin << ";\n"
                               << "else return (size_t) -1;\n";
                  }
               else
                  {
                 // Fixed members, generate an if-else ladder.
                    vector<GrammarString*>::iterator iter;
                    size_t counter = 0;
                    for (iter = traverseDataMemberList.begin(); iter != traverseDataMemberList.end(); ++iter)
                       {
                         string memberVariableName = (*iter)->getVariableNameString();
                      // Special case: SgTypedefDeclaration has a member that is computed using a special function.
                         if (string(node.getName()) == "SgTypedefDeclaration" && memberVariableName == "declaration")
                            {
                              outputFile << "if (child == compute_baseTypeDefiningDeclaration()) return " << StringUtility::numberToString(counter++) << ";\n"
                                         << "else ";
                            }
                      // Special case: SgClassDeclaration has a member that is computed using a special function.
                         if ((string(node.getName()) == "SgClassDeclaration" || string(node.getName()) == "SgTemplateInstantiationDecl") && memberVariableName == "definition")
                            {
                              outputFile << "if (child == compute_classDefinition()) return " << StringUtility::numberToString(counter++) << ";\n"
                                         << "else ";
                            }
                         else
                            {
                              outputFile << "if (child == p_" << memberVariableName << ") return " << StringUtility::numberToString(counter++) << ";\n"
                                         << "else ";
                            }
                       }
                 // If execution reaches this point, it's not my child.
                    outputFile << "return (size_t) -1;\n";
                  }
             }
            else
             {
            // There are no successors, so calling this function was an error. Complain.
               outputFile << "cout << \"error: get_childIndex called on node of type \" << \""
                          << node.getName() << "\" << \" that has no successors!\" << endl;\n"
                          << "ROSE_ASSERT(false);\n"
            // DQ (8/30/2009): Added return statement to avoid EDG warning when compiling ROSE with ROSE.
                          << "return 0; \n";
             }
          outputFile << "}\n";
       // end: generate get_childIndex() method
        }
       else
        {
       // *** The tree traversal function for the current class issues an error message
       // *** and causes the program to abort. Build this code string and write it to
       // *** the outputFile

          outputFile << "vector<" << grammarPrefixName << "Node*>\n" << node.getName() << "::get_traversalSuccessorContainer() {\n"
                     << "vector<" << grammarPrefixName << "Node*> " << successorContainerName << ";\n";
          outputFile << "   cerr << \"Internal error(!): called tree traversal mechanism for illegal object: \" << endl\n"
                     << "<< \"static: " << node.getName() << "\" << endl << \"dynamic:  \" << this->sage_class_name() << endl;\n"
                     << "cerr << \"Aborting ...\" << endl;\n"
                     << "ROSE_ASSERT(false);\n"
                     << "return " << successorContainerName << ";\n }\n\n";

          outputFile << "vector<string>\n" << node.getName() << "::get_traversalSuccessorNamesContainer() {\n"
                     << "vector<string> " << successorContainerName << ";\n";
          outputFile << "   cerr << \"Internal error(!): called tree traversal mechanism for illegal object: \" << endl\n"
                     << "<< \"static: " << node.getName() << "\" << endl << \"dynamic:  \" << this->sage_class_name() << endl;\n"
                     << "cerr << \"Aborting ...\" << endl;\n"
                     << "ROSE_ASSERT(false);\n"
                     << "return " << successorContainerName << ";\n }\n\n";

       // GB (09/25/2007): Added implementations for the new methods get_numberOfTraversalSuccessors, get_traversalSuccessorByIndex, and get_childIndex.
          outputFile << "size_t\n" << node.getName() << "::get_numberOfTraversalSuccessors() {\n";
          outputFile << "   cerr << \"Internal error(!): called tree traversal mechanism for illegal object: \" << endl\n"
                     << "<< \"static: " << node.getName() << "\" << endl << \"dynamic:  this = \" << this << \" = \" << this->sage_class_name() << endl;\n"
                     << "cerr << \"Aborting ...\" << endl;\n"
                     << "ROSE_ASSERT(false);\n"
                     << "return 42;\n }\n\n";

          outputFile << "SgNode*\n" << node.getName() << "::get_traversalSuccessorByIndex(size_t) {\n";
          outputFile << "   cerr << \"Internal error(!): called tree traversal mechanism for illegal object: \" << endl\n"
                     << "<< \"static: " << node.getName() << "\" << endl << \"dynamic:  \" << this->sage_class_name() << endl;\n"
                     << "cerr << \"Aborting ...\" << endl;\n"
                     << "ROSE_ASSERT(false);\n"
                     << "return NULL;\n }\n\n";

          outputFile << "size_t\n" << node.getName() << "::get_childIndex(SgNode *) {\n";
          outputFile << "   cerr << \"Internal error(!): called tree traversal mechanism for illegal object: \" << endl\n"
                     << "<< \"static: " << node.getName() << "\" << endl << \"dynamic:  \" << this->sage_class_name() << endl;\n"
                     << "cerr << \"Aborting ...\" << endl;\n"
                     << "ROSE_ASSERT(false);\n"
                     << "return 42;\n }\n\n";
        }

  // Traverse all nodes of the grammar recursively and build the tree traversal function
  // for each of them
     vector<AstNodeClass *>::iterator treeNodeIterator;
     for( treeNodeIterator = node.subclasses.begin(); treeNodeIterator != node.subclasses.end(); treeNodeIterator++ )
        {
          ROSE_ASSERT((*treeNodeIterator) != NULL);
          ROSE_ASSERT((*treeNodeIterator)->getBaseClass() != NULL);
          buildTreeTraversalFunctions(**treeNodeIterator, outputFile);
        }

  // return;
   }


/////////////////////////////////////////////////
// traversalSuccessorContainer Code Generation //
/////////////////////////////////////////////////

// MS: 06/28/02: factored out the loop code generation
string Grammar::generateTraverseSuccessorForLoopSource(string typeString,
                                                       string memberVariableName,
                                                       string successorContainerName,
                                                       string successorContainerAccessOperator)
   {
     string travSuccSource="";
     travSuccSource += "   {\n";
  // Build the declaration of the STL iterator
     travSuccSource += "     " + string(getIteratorString(typeString.c_str())) + " iter;\n";
  // Build the loop for iterating on the container

  // DQ (3/22/2007): Added error checking on pointer data members.
     if (successorContainerAccessOperator == "->")
          travSuccSource += "     ROSE_ASSERT(p_" + string(memberVariableName) + " != NULL);\n";
     travSuccSource += "     for (iter = p_" + string(memberVariableName) + successorContainerAccessOperator+"begin();"
                    +  " iter != p_" + string(memberVariableName) + successorContainerAccessOperator+"end(); iter++)\n";

  // Check whether the STL container contains pointers or not
     if (typeString.find("PtrList") != string::npos || typeString.find("PtrVector") != string::npos)
          travSuccSource += "          " + successorContainerName + ".push_back(*iter);\n"; // It contains pointers to AST objects
       else
          travSuccSource += "          " + successorContainerName + ".push_back(&(*iter));\n";  // It contains AST objects
     travSuccSource += "        }\n";
     return travSuccSource;
   }

// GB (8/16/2007): Generate the pre-allocation of the traversal successor
// container. We know the size beforehand, so calling reserve saves some time
// as it avoids repeated reallocations on push_back. The size of the container
// is the sum of the number of single members and the size of the optional
// container member.
string Grammar::generateNumberOfSuccessorsComputation( vector<GrammarString*>& traverseDataMemberList, string successorContainerName)
{
    stringstream travSuccSource;
    if (!traverseDataMemberList.empty())
    {
        vector<GrammarString*>::iterator iter;
        int singleSuccessors = 0, containerSuccessors = 0;
        for (iter = traverseDataMemberList.begin(); iter != traverseDataMemberList.end(); ++iter)
        {
            string typeString = (*iter)->getTypeNameString();
            string memberVariableName = (*iter)->getVariableNameString();
            if (isSTLContainerPtr(typeString))
            {
                containerSuccessors++;
                travSuccSource << "p_" << memberVariableName << "->size() + ";
            }
            else if (isSTLContainer(typeString))
            {
                containerSuccessors++;
                travSuccSource << "p_" << memberVariableName << ".size() + ";
            }
            else
            {
                singleSuccessors++;
                // If this is a single successor, no container may come before
                // it as that would break the traversal successor enums.
                if (containerSuccessors > 0)
                {
                    cout << "Error: traversal successor " << memberVariableName
                        << " is preceded by a container that is also "
                        << "traversed; this is not allowed";
                    ROSE_ASSERT((singleSuccessors > 0 ? containerSuccessors == 0 : true));
                }
            }
//#if 1  // Liao, 5/30/2009, allow multiple container-type members for SgOmpSectionsStatement

            //if ((containerSuccessors > 1) &&(memberVariableName!="clauses"))
            if (containerSuccessors > 1)
            {
                cout << "Error: traversal successor (" << memberVariableName
                    << ") is a container preceded by another container that is "
                    << "also traversed; this is not allowed";
                ROSE_ASSERT(containerSuccessors <= 1);
            }
//#endif
        }

        // In general, the result of this function will be something like 'p_foo.size()+42' or '+23'.
        // The + is unary or binary depending on context, no need to worry about it. It is forbidden to have more than
        // one container.
        travSuccSource << singleSuccessors;
        //travSuccSource << "+" << singleSuccessors;
    }
    return travSuccSource.str();
}

// MS: 03/11/02: new TraversalMechanism
// generate source for adding successors of a node to the successors container.
string Grammar::generateTraverseSuccessor(GrammarString* gs, string successorContainerName)
   {
     string memberVariableName=gs->getVariableNameString();
     string typeString=gs->getTypeNameString();

  // MS: sstream should be used here in future
     string travSuccSource="";

#if 0
  // DQ (5/8/2005): Debugging code introduced to debug enclusion of SgDirectory concept!
     travSuccSource += string("/* typeString = ") + string(typeString) + string(" */\n");
     travSuccSource += string("/* isSTLContainer   (typeString) = ") +
                       string((isSTLContainer   (typeString) == true ? "true" : "false")) + string(" */\n");
     travSuccSource += string("/* isSTLContainerPtr(typeString) = ") +
                       string((isSTLContainerPtr(typeString) == true ? "true" : "false")) + string(" */\n");
#endif

     if (isSTLContainerPtr(typeString))
        {
          travSuccSource += generateTraverseSuccessorForLoopSource(typeString,memberVariableName,successorContainerName,"->");
        }
       else
        {
          if (isSTLContainer(typeString))
             {
               travSuccSource += generateTraverseSuccessorForLoopSource(typeString,memberVariableName,successorContainerName,".");
             }
            else
             {
            // ***********************************************************************
            // The data member to be visited is not a container (it is a single object)
            // ***********************************************************************
            // Check if the data member has a pointer type in which case
            // we need the "->" operator. Otherwise we need the "." operator
               if (typeString.find('*') != string::npos)
                  {
                    travSuccSource += successorContainerName + ".push_back(p_"+ memberVariableName + ");\n"; // It is a pointer to an AST object
                  }
                 else
                  {
                 // Does this ever occur?
                    travSuccSource += successorContainerName + ".push_back(&p_" + memberVariableName + ");\n"; // It is an AST object
                  }
             }
        }

     return travSuccSource;
   }

// -------------------------------------------------------------------------------------
// generate a container with Names of the traversed members for a better output (DOT/PDF)
// -------------------------------------------------------------------------------------
// MS: 06/28/02
string Grammar::generateTraverseSuccessorNamesForLoopSource(string typeString,
                                                       string memberVariableName,
                                                       string successorContainerName,
                                                       string successorContainerAccessOperator)
   {
     string travSuccSource="";
     travSuccSource += "   {\n";
  // Build the declaration of the STL iterator
     travSuccSource += "     " + string(getIteratorString(typeString.c_str())) + "  iter;\n";
  // GB (8/16/2007): Moved this declaration up to the beginning of the
  // function because we do not necessarily want to count from 0, depending on
  // the other members.
  // travSuccSource += "  int i=0;\n";

  // Build the loop for iterating on the container
  // DQ (3/22/2007): Added error checking on pointer data members.
     if (successorContainerAccessOperator == "->")
          travSuccSource += "     ROSE_ASSERT(p_" + string(memberVariableName) + " != NULL);\n";
     travSuccSource += "     for (iter = p_" + string(memberVariableName) + successorContainerAccessOperator+"begin();"
                    +  " iter != p_" + string(memberVariableName) + successorContainerAccessOperator+"end(); (iter++,i++)) {\n";

  // Check whether the STL container contains pointers or not
     travSuccSource+="char buf[20];\n";
     if (typeString.find("PtrList") != string::npos || typeString.find("PtrVector") != string::npos)
        {
          travSuccSource+="sprintf(buf,\"*[%d]\",i);\n"; // pointers are represented as '*'
          travSuccSource += successorContainerName + ".push_back(buf);\n"; // It contains pointers to AST objects
        }
       else
        {
          travSuccSource+="sprintf(buf,\"[%d]\",i);\n";
          travSuccSource += successorContainerName + ".push_back(buf);\n";  // It contains AST objects
        }

     travSuccSource += "        }\n   }\n";
     return travSuccSource;
   }

// MS: 06/28/02:
// generate source for adding successor names of a node to the successornames container.
string Grammar::generateTraverseSuccessorNames(GrammarString* gs, string successorContainerName) {
  string memberVariableName=gs->getVariableNameString();
  string typeString=gs->getTypeNameString();

  // MS: sstream should be used here in future
  string travSuccSource="";
  if (isSTLContainerPtr(typeString)) {
    travSuccSource=generateTraverseSuccessorNamesForLoopSource(typeString,memberVariableName,successorContainerName,"->");
  } else if (isSTLContainer(typeString)) {
    travSuccSource=generateTraverseSuccessorNamesForLoopSource(typeString,memberVariableName,successorContainerName,".");
  } else {
    // ***********************************************************************
    // The data member to be visited is not a container (it is a single object)
    // ***********************************************************************
    // Check if the data member has a pointer type in which case
    // we need the "->" operator. Otherwise we need the "." operator
    if (typeString.find('*') != string::npos) {
      travSuccSource = successorContainerName + ".push_back(\"p_"+ memberVariableName + "\");\n"; // It is a pointer to an AST object
    } else {
      // Does this ever occur?
      travSuccSource = successorContainerName + ".push_back(\"&p_" + memberVariableName + "\");\n"; // It is an AST object
    }
  }
  return travSuccSource;
}

void
Grammar::buildEnumForNode(AstNodeClass& node, string& allEnumsString) {
  GrammarNodeInfo info=getGrammarNodeInfo(&node);
// GB (8/16/2007): The distinction between container and non-container nodes
// has been dropped, and so has this code. Instead, we now generate enums
// even for nodes that contain containers; the enum for the container member
// is then the index of the first element of that container, which is neat!
// It also means that we can only allow at most one container per node,
// since the enums for further containers would not correspond to their
// first elements.
//#if 1  // allow multiple container-style members for SgOmpSectionsStatement: sections, clauses
  //if ((info.numContainerMembers > 1) &&(node.getName()!="SgOmpSectionsStatement"))
  if (info.numContainerMembers > 1)
  {
    cout << "Error: grammar node (" << node.getName() << ") has more than one container member" << endl;
    ROSE_ASSERT(info.numContainerMembers <= 1);
  }
//#endif
  vector<GrammarString*> includeList=classMemberIncludeList(node);
  vector<GrammarString*>::iterator stringListIterator;
  if (!includeList.empty()) {
    bool isFirst=true;
    for(stringListIterator = includeList.begin();
        stringListIterator != includeList.end();
        stringListIterator++) {
      if ( (*stringListIterator)->getToBeTraversed() == DEF_TRAVERSAL) {
        if (isFirst) {
          allEnumsString += string("enum E_") + node.getName() + " \n{\n";
        } else {
          allEnumsString += ", ";
        }
        isFirst=false;
        allEnumsString += string(node.getName()) + "_" + (*stringListIterator)->getVariableNameString();
      }
    }
    if(!isFirst) {
      allEnumsString += "};\n";
    }
  }
}

string Grammar::EnumStringForNode(AstNodeClass& node, string s) {
  // let's reuse the old function for now
  string source;
  buildEnumForNode(node,source);
  return s + source;
}

//////////////////////////////////////////////////////////////////////////////////////////
// GRAMMAR TRAVERSAL
// MS: build a vector of synth attributes for each node of the grammar
// this is a (strongly) simplified version of the BottomUpProcessing class functionality
// Arguments: 1. GrammarNode,
//            2. a function like evaluateSynthesizedAttribute,
//               with string being the synthesized attribute type
// (can be replaced by MSTL/DSProcessing.C (when finished))
//////////////////////////////////////////////////////////////////////////////////////////
Grammar::GrammarSynthesizedAttribute
Grammar::BottomUpProcessing(AstNodeClass* node,
                            evaluateGAttributeFunctionType evaluateGAttributeFunction) {
  // Traverse all nodes of the grammar recursively and build the synthesized attribute
  // for each of them
  vector<AstNodeClass *>::iterator treeNodeIterator;
  vector<GrammarSynthesizedAttribute> v;
  for( treeNodeIterator = node->subclasses.begin();
       treeNodeIterator != node->subclasses.end();
       treeNodeIterator++ ) {
    ROSE_ASSERT((*treeNodeIterator)!=NULL);
    ROSE_ASSERT((*treeNodeIterator)->getBaseClass()!=NULL);
    v.push_back(BottomUpProcessing(*treeNodeIterator, evaluateGAttributeFunction));
  }
  return (this->*evaluateGAttributeFunction)(node, v);
}

// MS: build a string for each node of the grammar (and concatenate these strings)
// this is a (strongly) simplified version of the BottomUpProcessing class functionality
// Arguments: 1. GrammarNode,
//            2. a function like evaluateSynthesizedAttribute,
//               with string being the synthesized attribute type
string
Grammar::naiveTraverseGrammar(AstNodeClass &node,
                              evaluateStringAttributeFunctionType evaluateStringAttributeFunction) {
  // Traverse all nodes of the grammar recursively and build the synthesized string attribute
  // for each of them
  vector<AstNodeClass *>::iterator treeNodeIterator;
  string s;
  for( treeNodeIterator = node.subclasses.begin();
       treeNodeIterator != node.subclasses.end();
       treeNodeIterator++ ) {
    ROSE_ASSERT((*treeNodeIterator)!=NULL);
    ROSE_ASSERT((*treeNodeIterator)->getBaseClass()!=NULL);
    s+=naiveTraverseGrammar(**treeNodeIterator, evaluateStringAttributeFunction);
  }
  return (this->*evaluateStringAttributeFunction)(node, s);
}

/////////////////////////////////
// GRAMMAR AUXILIARY FUNCTIONS //
/////////////////////////////////
vector<GrammarString*>
Grammar::classMemberIncludeList(AstNodeClass& node) {
  // Determine the data members to be investigated (starting at the root of the grammar)
  vector<GrammarString *> includeList;
  vector<GrammarString *> excludeList;

  ROSE_ASSERT(includeList.size() == 0);
  ROSE_ASSERT(excludeList.size() == 0);

  // Generate include and exclude list, see function buildTreeTraversalFunctions() which
  // belongs to the 1. implementation of a tree traversal mechnism
  // BP : 10/09/2001, modified to provide address of function
  generateStringListsFromLocalLists(node,includeList,excludeList, &AstNodeClass::getMemberDataPrototypeList); //TODO:This pointer is unsafe (used for NonTerminal objects as well! (MS)

  // Now edit the lists to remove elements appearing within the exclude list
  editStringList(includeList,excludeList);
  return includeList;
}

// MK: This member function is used by the member function buildTreeTraversalFunctions()
// in order to determine if the current node of the grammar corresponds to a grammar
// class whose objects may actually occur in an AST. In a symmetric implementation
// these would exactly be the AstNodeClass objects. For the moment we have to be a little
// more careful and treat several classes as special cases ...
bool
Grammar::isAstObject(AstNodeClass& node)
{
  return node.getCanHaveInstances();
}

// MK: We need this function to determine if the object
// is a pointer to an STL container
bool
Grammar::isSTLContainerPtr(const string& typeString)
{
  return typeString.size() >= 3 &&
         typeString.substr(typeString.size() - 3) == "Ptr" &&
         isSTLContainer(typeString.substr(0, typeString.size() - 3));
}

// MK: We need this function to determine if the object
// is an STL container
bool
Grammar::isSTLContainer(const string& typeString)
{
  if (typeString.size() >= 4 && typeString.substr(typeString.size() - 4) == "List") return true;
  if (typeString.size() >= 9 && typeString.substr(typeString.size() - 9) == "BitVector") return false;
  if (typeString.size() >= 6 && typeString.substr(typeString.size() - 6) == "Vector") return true;
  return false;
}

string
Grammar::getIteratorString(const string& typeString)
   {
     string ts = typeString;
     if (ts.size() >= 3 && ts.substr(ts.size() - 3) == "Ptr") {
       ts = ts.substr(0, ts.size() - 3);
     }
     return ts + "::iterator";
   }

AstNodeClass* lookupTerminal(const vector<AstNodeClass*>& tl, const std::string& name) {
  for (vector<AstNodeClass*>::const_iterator it = tl.begin();
       it != tl.end(); ++it) {
    if ((*it)->getName() == name) {
      return *it;
    }
  }
  cerr << "Reached end of AstNodeClass list in search for '" << name << "'" << endl;
  ROSE_ASSERT (false);

// DQ (11/28/2009): MSVC warns that this function should return a value from all paths.
  return NULL;
}

bool Grammar::nameHasPrefix(string name, string prefix) {
  return name.substr(0,prefix.size())==prefix;
}
