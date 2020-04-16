// ################################################################
// #                           Header Files                       #
// ################################################################

#include "ROSETTA_macros.h"
#include "grammar.h"
#include "AstNodeClass.h"
#include "grammarString.h"
#include <sstream>

using namespace std;
using namespace Rose;

// JJW helper macros
#define DO_ON_CHILDREN(NODE, FUNCTION) \
  do { \
    vector<AstNodeClass *>::const_iterator treeNodeIterator; \
    for(treeNodeIterator = (NODE).subclasses.begin(); \
        treeNodeIterator != (NODE).subclasses.end(); \
        treeNodeIterator++ ) \
    { \
      ROSE_ASSERT ((*treeNodeIterator) != NULL); \
      ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL); \
      FUNCTION(**treeNodeIterator,outputFile); \
    } \
  } while (0)

#define DO_ON_CHILDREN_TO_STRING(NODE, RESULT, FUNCTION) \
  do { \
    vector<AstNodeClass *>::const_iterator treeNodeIterator; \
    for(treeNodeIterator = (NODE).subclasses.begin(); \
        treeNodeIterator != (NODE).subclasses.end(); \
        treeNodeIterator++ ) \
    { \
      ROSE_ASSERT ((*treeNodeIterator) != NULL); \
      ROSE_ASSERT ((*treeNodeIterator)->getBaseClass() != NULL); \
      RESULT += FUNCTION(**treeNodeIterator); \
    } \
  } while (0)

// ################################################################
// #                   Grammar Member Functions                   #
// ################################################################

//#########################################################################################################
/* JH (11/07/2005): method that generates the soure code for following methods of the IRNode 
 * StoarageClasses:
 *   * pickOutIRNodeData : method that initializes a storage element from the data of its corresponding 
 *     IRNode object
 *   * arrangeStaticDataInOneBlock : method that is only needed before we do the file I/O. Since the 
 *     static data of the class StorageClassMemoryManagement is organized in static memory pools with 
 *     memory blocks (see also StorageClassMemeoryManagement.h) we need to copy all data in one memory
 *     block to access it with the positions that are stored. However, when the data is read from disk, 
 *     it will already be in one block.
 *   * deleteStaticData : since the pick of the data works memory-pool-wise, we can delete the static data
 *     of our StorageClassMemoryManagement after finishing every type of IRNode memory pool
 *   * writeEasyStorageDataToFile: writes the data stored within the EasyStorage classes to disk
 *   * readEasyStorageDataFromFile: reads back the data od the EasyStroage classes from file
 */
string
Grammar::buildStringForStorageClassSource ( AstNodeClass & node )
   {

     std::string pickOutIRNodeData ;
     std::string arrangeStaticDataInOneBlock;
     std::string deleteStaticData;
     std::string writeEasyStorageData ;
     std::string readEasyStorageData ;
  // building the source code of the virtual member function pickOutIRNodeData
     pickOutIRNodeData  = "\nvoid\n" ;
     pickOutIRNodeData += "$CLASSNAMEStorageClass::pickOutIRNodeData ( $CLASSNAME* pointer ) \n" ;
     pickOutIRNodeData += "   { \n" ;
     pickOutIRNodeData += node.buildStorageClassPickOutIRNodeDataSource();
     pickOutIRNodeData += "   }\n" ;
  // string declaration for the EasyStorage members in the StorageClasses
     if ( node.hasMembersThatAreStoredInEasyStorageClass() == true )
        {
       // building the source code of the static member function arrangeStaticDataInOneBlock
          arrangeStaticDataInOneBlock  = "\nvoid\n" ;
          arrangeStaticDataInOneBlock += "$CLASSNAMEStorageClass::arrangeStaticDataOfEasyStorageClassesInOneBlock ( )\n" ;
          arrangeStaticDataInOneBlock += "   { \n" ;
          arrangeStaticDataInOneBlock +=  node.buildStorageClassArrangeStaticDataInOneBlockSource();
          arrangeStaticDataInOneBlock += "   }\n" ;
       // building the source code of the static member function deleteStaticData
          deleteStaticData  = "\nvoid\n" ;
          deleteStaticData += "$CLASSNAMEStorageClass::deleteStaticDataOfEasyStorageClasses ( ) \n" ;
          deleteStaticData += "   { \n" ;
          deleteStaticData += node.buildStorageClassDeleteStaticDataSource();
          deleteStaticData += "   }\n" ;
       // building the source code of the static member function writeEasyStorageDataToFile
          writeEasyStorageData  = "\nvoid\n" ;
          writeEasyStorageData += "$CLASSNAMEStorageClass::writeEasyStorageDataToFile (std::ostream& out)\n" ;
          writeEasyStorageData += "   {\n" ;
          writeEasyStorageData += node.buildStorageClassWriteStaticDataToFileSource();
          writeEasyStorageData += "   }\n\n" ;
       // building the source code of the static member function writeEasyStorageDataToFile
          readEasyStorageData  = "\nvoid\n" ;
          readEasyStorageData += "$CLASSNAMEStorageClass::readEasyStorageDataFromFile (std::istream& in)\n   " ;
          readEasyStorageData += "   { \n" ;
          readEasyStorageData += node.buildStorageClassReadStaticDataFromFileSource();
          readEasyStorageData += "   }\n\n" ;
        }
  // computing the length of the resulting string
     string returnString = pickOutIRNodeData + arrangeStaticDataInOneBlock + deleteStaticData + writeEasyStorageData + readEasyStorageData;
     return returnString;
   }

//#########################################################################################################
/* JH (11/07/2005): build the source for one IR nodes StorageClass, while reading the macro file, 
 * calling the method above and replacing all the $CLASSNAME, etc. 
 */
void
Grammar::buildStorageClassSourceFiles( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     string sourceFileInsertionSeparator = "MEMBER_FUNCTION_DEFINITIONS";
     string fileName = "../Grammar/grammarStorageClassDefinitionMacros.macro";
     StringUtility::FileWithLineNumbers sourceFileTemplate = Grammar::readFileWithPos (fileName);
     StringUtility::FileWithLineNumbers sourceBeforeInsertion = Grammar::buildHeaderStringBeforeMarker(sourceFileInsertionSeparator, fileName);
     StringUtility::FileWithLineNumbers sourceAfterInsertion = Grammar::buildHeaderStringAfterMarker(sourceFileInsertionSeparator, fileName);
  // Edit the $CLASSNAME
     StringUtility::FileWithLineNumbers editedStringMiddle(1, StringUtility::StringWithLineNumber(buildStringForStorageClassSource(node), "" /* "<buildStringForStorageClassSource " + node.getToken().getName() + ">" */, 1));
  // Place the constructor at the top of the node specific code for this element of grammar
     StringUtility::FileWithLineNumbers editedSourceFileString = sourceBeforeInsertion + editedStringMiddle + sourceAfterInsertion;
  // Now apply the edit/subsitution specified within the grammar (by the user)
     editedSourceFileString = editSubstitution (node,editedSourceFileString);

     outputFile += editedSourceFileString;
     DO_ON_CHILDREN(node, buildStorageClassSourceFiles);
   }
      

//#########################################################################################################
/* JH (11/07/2005): build the source IR node constructors that take its corresponding StorageClass type
*/
void
Grammar::buildIRNodeConstructorOfStorageClassSource( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     string sourceFileInsertionSeparator = "CONSTRUCTOR_SOURCE";
     string fileName = "../Grammar/grammarSourceOfIRNodesAstFileIOSupport.macro";
     StringUtility::FileWithLineNumbers sourceFileTemplate = Grammar::readFileWithPos (fileName);

     StringUtility::FileWithLineNumbers sourceBeforeInsertion = buildHeaderStringBeforeMarker(sourceFileInsertionSeparator, fileName);
     StringUtility::FileWithLineNumbers sourceAfterInsertion = buildHeaderStringAfterMarker(sourceFileInsertionSeparator, fileName);

  // Edit the $CLASSNAME
     StringUtility::FileWithLineNumbers editedStringMiddle(1, StringUtility::StringWithLineNumber(node.buildSourceForIRNodeStorageClassConstructor(), "" /* "<buildSourceForIRNodeStorageClassConstructor " + node.getToken().getName() + ">" */, 1));
     StringUtility::FileWithLineNumbers editedSourceFileString = sourceBeforeInsertion + editedStringMiddle + sourceAfterInsertion;
  // Now apply the edit/subsitution specified within the grammar (by the user)
     editedSourceFileString = editSubstitution (node,editedSourceFileString);
  // JH Add the parent in position   
     AstNodeClass *term = &node;
     ROSE_ASSERT( term  != NULL );
     std::string parent = " ";
     if ( term->getBaseClass() != NULL )
        {
          term = term->getBaseClass();
          parent = " : ";
          parent += term->name;
          parent += " (storageSource)";
        }
     editedSourceFileString = GrammarString::copyEdit (editedSourceFileString,"$PARENT_CLASSNAME",parent.c_str());

     outputFile += editedSourceFileString;
     DO_ON_CHILDREN(node, buildIRNodeConstructorOfStorageClassSource);
   }

//#########################################################################################################
/* JH (11/24/2005): Method that generates the code of constructor the AstSpecificDataManagingClass, that
 * takes the root of an AST. 
 */
std::string
Grammar::buildStaticDataMemberListClassConstructor(AstNodeClass & node)
   {
     std::string classMembers = node.buildStaticDataMemberListConstructor();
     string temp = classMembers;
     classMembers = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, classMembers, buildStaticDataMemberListClassConstructor);
     return classMembers;
   }

//#########################################################################################################
/* JH (11/24/2005): Method that generates the code for the initialization of the static data of the IR node
 * classes from a AstSpecificDataManagingClass object.
 */
std::string
Grammar::buildStaticDataMemberListSetStaticDataSource(AstNodeClass & node)
   {
     std::string classMembers = node.buildStaticDataMemberListSetStaticData();
     string temp = classMembers;
     classMembers = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, classMembers, buildStaticDataMemberListSetStaticDataSource);
     return classMembers;
   }

//#########################################################################################################
/* JH (11/24/2005): Method that generates the code of the AstSpecificDataManagingClass members, i.e. the
 * static members of the IR nodes. 
 */
std::string
Grammar::buildStaticDataMemberListClassEntries(AstNodeClass & node)
   {
     std::string classMembers = node.buildStaticDataMemberList();
     string temp = classMembers;
     classMembers = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, classMembers, buildStaticDataMemberListClassEntries);
     return classMembers;
   }

//#########################################################################################################
/* JH (11/24/2005): Method that builds the data member variables of the StorageClass of the 
 * AstSpecificDataMangingClass, i.e. AstSpecificDataMangingClassStorageClass.  
 */
std::string
Grammar::buildDataMemberStorageClass(AstNodeClass & node)
   {
     std::string classMembers = node.buildStaticDataMemberListOfStorageClass();
     string temp = classMembers;
     classMembers = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, classMembers, buildDataMemberStorageClass);
     return classMembers;
   }

//#########################################################################################################
/* JH (11/24/2005): Method that builds the access functions declarations for accessing the data contained 
 * in the AstSpecificDataManagingClass.
 */
std::string
Grammar::buildAccessFunctionSources(AstNodeClass & node)
   {
     std::string functionSource = node.buildAccessFunctionsForStaticDataMemberSource();
     string temp = functionSource;
     functionSource = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, functionSource, buildAccessFunctionSources);
     return functionSource;
   }

//#########################################################################################################
/* JH (11/24/2005): Method that builds the coding of the constructor of the AstSpecificDataManagingClass
 * that takes its corresponding StorageClass (AstSpecificDataManagingClassStorageClass)
 */
std::string
Grammar::generateStaticDataConstructorSource(AstNodeClass & node)
   {
     std::string functionSource = node.buildStaticDataConstructorSource();
     string temp = functionSource;
     functionSource = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, functionSource, generateStaticDataConstructorSource);
     return functionSource;
   }

//#########################################################################################################
/* JH (11/24/2005): Method that builds the code for the method 
 * writeEasyStorageDataToFile of AstSpecificDataManagingClassStorageClass
 */
std::string
Grammar::generateStaticDataWriteEasyStorageDataToFileSource(AstNodeClass & node)
   {
     std::string functionSource = node.buildStaticDataWriteEasyStorageDataToFileSource();
     string temp = functionSource;
     functionSource = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, functionSource, Grammar::generateStaticDataWriteEasyStorageDataToFileSource);
     return functionSource;
   }

//#########################################################################################################
/* JH (11/24/2005): Method that builds the code for the method 
 * readEasyStorageDataFromFile of AstSpecificDataManagingClassStorageClass
 */
std::string
Grammar::generateStaticDataReadEasyStorageDataFromFileSource(AstNodeClass & node)
   {
     std::string functionSource = node.buildStaticDataReadEasyStorageDataFromFileSource();
     string temp = functionSource;
     functionSource = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, functionSource, Grammar::generateStaticDataReadEasyStorageDataFromFileSource);
     return functionSource;
   }

//#########################################################################################################
/* JH (11/24/2005): Method that builds the code for the method 
 * arrangeStaticDataOfEasyStorageClassesInOneBlock of AstSpecificDataManagingClassStorageClass
 */
std::string
Grammar::generateStaticDataArrangeEasyStorageInOnePoolSource(AstNodeClass & node)
   {
     std::string functionSource = node.buildStaticDataArrangeEasyStorageInOnePoolSource();
     string temp = functionSource;
     functionSource = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, functionSource, Grammar::generateStaticDataArrangeEasyStorageInOnePoolSource);
     return functionSource;
   }

//#########################################################################################################
/* JH (11/24/2005): Method that builds the code for the method 
 * deleteStaticDataOfEasyStorageClasses of AstSpecificDataManagingClassStorageClass
 */
std::string
Grammar::generateStaticDataDeleteEasyStorageMemoryPoolSource(AstNodeClass & node)
   {
     std::string functionSource = node.buildStaticDataDeleteEasyStorageMemoryPoolSource();
     string temp = functionSource;
     functionSource = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, functionSource, Grammar::generateStaticDataDeleteEasyStorageMemoryPoolSource);
     return functionSource;
   }

//#########################################################################################################
/* JH (11/24/2005): Method that builds the code for the method pickOutIRNodeData of the 
 * AstSpecificDataManagingClassStorageClass that takes the data out of a AstSpecificDataManaging object
 */
std::string
Grammar::buildStaticStorageClassPickOutSource(AstNodeClass & node)
   {
     std::string functionSource = node.buildSourceForStoringStaticMembers();
     string temp = functionSource;
     functionSource = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, functionSource, buildStaticStorageClassPickOutSource);
     return functionSource;
   }

//#########################################################################################################
/* JH (11/24/2005): Method that builds the source code for list of access 
 * functions of the data members contained in AstSpecificDataManagingClassStorageClass
 */
std::string
Grammar::buildAccessFunctionsOfClassEntries(AstNodeClass & node)
   {
     std::string accessFunctions = node.buildAccessFunctionsForStaticDataMember();
     string temp = accessFunctions;
     accessFunctions = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);
     DO_ON_CHILDREN_TO_STRING(node, accessFunctions, buildAccessFunctionsOfClassEntries);
     return accessFunctions;
   }

//#########################################################################################################
//JH (11/24/2005): Method that generates the code of the headers of the IR nodes StorageClasses
void
Grammar::buildStorageClassHeaderFiles( AstNodeClass & node, StringUtility::FileWithLineNumbers & outputFile )
   {
     string marker   = "DATA_MEMBER_DECLARATIONS";
     string fileName = "../Grammar/grammarStorageClassDeclatationMacros.macro";
     StringUtility::FileWithLineNumbers headerBeforeInsertion = buildHeaderStringBeforeMarker(marker,fileName);
     StringUtility::FileWithLineNumbers headerAfterInsertion  = buildHeaderStringAfterMarker (marker,fileName);
     StringUtility::FileWithLineNumbers editStringStart;
     StringUtility::FileWithLineNumbers editStringEnd;
     string className = node.getName();
     editStringStart = headerBeforeInsertion;
#if 1
  // JH Add the parent in position
     AstNodeClass *term = &(node);
     ROSE_ASSERT( term  != NULL );
     std::string parent = " ";
     if ( term->getBaseClass() != NULL )
        {
          term = term->getBaseClass();
          parent = " : public "; 
          parent += term->name;
          parent += "StorageClass"; 
        }
     editStringStart = GrammarString::copyEdit (editStringStart,"$PARENT_CLASSNAME",parent.c_str());

#endif
     editStringEnd   = GrammarString::copyEdit (headerAfterInsertion,"$CLASSNAME",className);
#if 1
     // calls to GrammarString::copyEdit() now centralized in editSubstitution()
     editStringStart = GrammarString::copyEdit (editStringStart,"$CLASSNAME",className);
     editStringEnd   = GrammarString::copyEdit (editStringEnd,"$CLASSNAME",className);
#endif
     StringUtility::FileWithLineNumbers storageClassMembers(1, StringUtility::StringWithLineNumber(node.buildStorageClassHeader(), "" /* "<buildStorageClassHeader " + node.getName() + ">" */, 1));
     StringUtility::FileWithLineNumbers editedHeaderFileStringTemp = editStringStart + storageClassMembers + editStringEnd;
     StringUtility::FileWithLineNumbers editedHeaderFileString = editSubstitution (node,editedHeaderFileStringTemp);
#if 1
     outputFile += editedHeaderFileString;
#endif
     DO_ON_CHILDREN(node, buildStorageClassHeaderFiles);
   }

//#########################################################################################################
//JH (11/24/2005): Method that generates the code of the StroageClasses.h and the StorageClasses.C files
void 
Grammar::generateStorageClassesFiles()
   {

  // Building the file StorageClasses.h
     ofstream AstSpecificDataHeaderFile ( std::string(target_directory+"/astFileIO/AstSpecificDataManagingClass.h").c_str()) ;
     if (verbose)
         std::cout << "Building StorageClasses header" << std::flush;
     StringUtility::FileWithLineNumbers readFromFile = readFileWithPos("../Grammar/grammarStaticDataManagingClassHeader.macro");
     std::string dataMembers = buildStaticDataMemberListClassEntries(*rootNode);
     std::string accessFunctions = buildAccessFunctionsOfClassEntries(*rootNode);
     std::string dataMembersStorageClass = buildDataMemberStorageClass(*rootNode);
     std::ostringstream myStream; //creates an ostringstream object
     ROSE_ASSERT (!this->astVariantToNodeMap.empty());
     size_t maxVariant = this->astVariantToNodeMap.rbegin()->first;
     myStream <<  (maxVariant + 1) << std::flush;
     std::string totalNumberOfIRNodes = myStream.str();
     readFromFile = GrammarString::copyEdit(readFromFile,"$REPLACE_NUMBEROFIRNODES", totalNumberOfIRNodes.c_str() );
     readFromFile = GrammarString::copyEdit(readFromFile,"$REPLACE_DATAMEMBERS", dataMembers.c_str() );
     readFromFile = GrammarString::copyEdit(readFromFile,"$REPLACE_ACCESSFUNCITONS", accessFunctions.c_str() );
     AstSpecificDataHeaderFile << StringUtility::toString(readFromFile);
     AstSpecificDataHeaderFile.close();
     if (verbose)
         std::cout << "... done " << std::endl;

  // Building the file StorageClasses.h
     StringUtility::FileWithLineNumbers StorageClassHeaderFile ;
     if (verbose)
         std::cout << "Building StorageClasses header" << std::flush;
     readFromFile = readFileWithPos("../Grammar/grammarStaticDataManagingClassStorageClassHeader.macro");
     dataMembers = buildStaticDataMemberListClassEntries(*rootNode);
     accessFunctions = buildAccessFunctionsOfClassEntries(*rootNode);
     dataMembersStorageClass = buildDataMemberStorageClass(*rootNode);
     readFromFile = GrammarString::copyEdit(readFromFile,"$REPLACE_NUMBEROFIRNODES", totalNumberOfIRNodes.c_str() );
     readFromFile = GrammarString::copyEdit(readFromFile,"$REPLACE_DATAMEMBERS", dataMembers.c_str() );
     readFromFile = GrammarString::copyEdit(readFromFile,"$REPLACE_ACCESSFUNCITONS", accessFunctions.c_str() );
     readFromFile = GrammarString::copyEdit(readFromFile,"$REPLACE_STORAGECLASSDATAMEMBERS", dataMembersStorageClass.c_str() );
     StorageClassHeaderFile += readFromFile;
     buildStorageClassHeaderFiles(*rootNode,StorageClassHeaderFile);
     Grammar::writeFile(StorageClassHeaderFile, target_directory, "StorageClasses", ".h");
     if (verbose)
         std::cout << "... done " << std::endl;

  // Building the file StorageClasses.C
     StringUtility::FileWithLineNumbers StorageClassSourceFile;
     if (verbose)
         std::cout << "Building StorageClasses source" << std::flush;
     readFromFile = readFileWithPos("../Grammar/grammarStaticDataManagingClassSource.macro");
     std::ostringstream myStream2; //creates an ostringstream object
     myStream2 << maxVariant + 1 << std::flush;
     totalNumberOfIRNodes = myStream2.str();
     readFromFile = GrammarString::copyEdit(readFromFile,"$REPLACE_NUMBEROFIRNODES", totalNumberOfIRNodes.c_str() );
       
     StorageClassSourceFile += readFromFile;
     StorageClassSourceFile << buildAccessFunctionSources(*rootNode);

     std::string staticConstructorSource; 
     staticConstructorSource += "AstSpecificDataManagingClass::AstSpecificDataManagingClass( SgProject* root )\n";
     staticConstructorSource += "   {\n";
#if 0
  // DQ (6/5/2010): Added more error checking!!!
     staticConstructorSource += "\n";
     staticConstructorSource += "  // Debugging tests.\n";
     staticConstructorSource += "     printf (\"At the TOP of AstSpecificDataManagingClass::AstSpecificDataManagingClass(SgProject*) \\n\"); \n";
     staticConstructorSource += "     MemoryCheckingTraversalForAstFileIO memoryCheckingTraversal_C1; \n";
     staticConstructorSource += "     memoryCheckingTraversal_C1.traverseMemoryPool(); \n";
     staticConstructorSource += "\n";
#endif
     staticConstructorSource += "     rootOfAst = root;\n";
     staticConstructorSource += "     astIndex = AST_FILE_IO::getNumberOfAsts(); \n";
     staticConstructorSource += buildStaticDataMemberListClassConstructor(*rootNode);
#if 0
  // DQ (6/5/2010): Added more error checking!!!
     staticConstructorSource += "\n";
     staticConstructorSource += "  // Debugging tests.\n";
     staticConstructorSource += "     printf (\"At the BOTTOM of AstSpecificDataManagingClass::AstSpecificDataManagingClass(SgProject*) \\n\"); \n";
     staticConstructorSource += "     MemoryCheckingTraversalForAstFileIO memoryCheckingTraversal_C2; \n";
     staticConstructorSource += "     memoryCheckingTraversal_C2.traverseMemoryPool(); \n";
     staticConstructorSource += "\n";
#endif
     staticConstructorSource += "   }\n\n";
     StorageClassSourceFile << staticConstructorSource;

     std::string staticSetSource; 
     staticSetSource += "void\n";
     staticSetSource += "AstSpecificDataManagingClass::setStaticDataMembersOfIRNodes ( ) const\n";
     staticSetSource += "   {\n";
  // DQ (2/20/2010): Remove extraneous output from AST File I/O (required to be clean for use in Autoconf tests).
  // staticSetSource += "      std::cout << \" Setting data of AST #\" << astIndex << std::endl;\n";
     staticSetSource += buildStaticDataMemberListSetStaticDataSource(*rootNode);
     staticSetSource += "   }\n\n";
     StorageClassSourceFile << staticSetSource;

     std::string staticPickOutSource; 
     staticPickOutSource += "void\n";
     staticPickOutSource += "AstSpecificDataManagingClassStorageClass::pickOutIRNodeData ( AstSpecificDataManagingClass* source )\n";
     staticPickOutSource += "   {\n";
     staticPickOutSource += "     for(int i =  0; i < " +  myStream.str() + " + 1; ++ i )\n";
     staticPickOutSource += "        {\n";
     staticPickOutSource += "          listOfAccumulatedPoolSizes[i] = source->listOfAccumulatedPoolSizes[i];\n";
     staticPickOutSource += "        }\n";
     staticPickOutSource += "     rootOfAst =  AST_FILE_IO::getGlobalIndexFromSgClassPointer (source->rootOfAst);\n";
     staticPickOutSource += buildStaticStorageClassPickOutSource(*rootNode);
     staticPickOutSource += "   }\n\n";
     StorageClassSourceFile << staticPickOutSource;

     std::string staticDataConstructorSource; 
     staticDataConstructorSource += "AstSpecificDataManagingClass::AstSpecificDataManagingClass(const AstSpecificDataManagingClassStorageClass& source)\n";
     staticDataConstructorSource += "   {\n";
#if 0
  // DQ (6/5/2010): Added more error checking!!!
     staticDataConstructorSource += "\n";
     staticDataConstructorSource += "        { \n";
     staticDataConstructorSource += "       // Debugging tests.\n";
     staticDataConstructorSource += "          printf (\"At the TOP of AstSpecificDataManagingClass::AstSpecificDataManagingClass(AstSpecificDataManagingClassStorageClass&) \\n\"); \n";
     staticDataConstructorSource += "          MemoryCheckingTraversalForAstFileIO memoryCheckingTraversal; \n";
     staticDataConstructorSource += "          memoryCheckingTraversal.traverseMemoryPool(); \n";
     staticDataConstructorSource += "        } \n";
     staticDataConstructorSource += "\n";
#endif
     staticDataConstructorSource += "     for(int i =  0; i < " +  myStream.str() + " + 1; ++ i )\n";
     staticDataConstructorSource += "        {\n";
     staticDataConstructorSource += "          listOfAccumulatedPoolSizes[i] = source.listOfAccumulatedPoolSizes[i];\n";
     staticDataConstructorSource += "        }\n";
     staticDataConstructorSource += "     astIndex = AST_FILE_IO::getNumberOfAsts();\n";
     staticDataConstructorSource += "     AST_FILE_IO::addNewAst(this);\n";
  // DQ (2/20/2010): Remove extraneous output from AST File I/O (required to be clean for use in Autoconf tests).
  // staticDataConstructorSource += "     std::cout << \"Extending memory pools ...\" << std::flush;\n";
#if 0
  // DQ (6/5/2010): Added more error checking!!!
     staticDataConstructorSource += "\n";
     staticDataConstructorSource += "        { \n";
     staticDataConstructorSource += "       // Debugging tests.\n";
     staticDataConstructorSource += "          printf (\"After AST_FILE_IO::addNewAst(this) in AstSpecificDataManagingClass::AstSpecificDataManagingClass(AstSpecificDataManagingClassStorageClass&) \\n\"); \n";
     staticDataConstructorSource += "          MemoryCheckingTraversalForAstFileIO memoryCheckingTraversal; \n";
     staticDataConstructorSource += "          memoryCheckingTraversal.traverseMemoryPool(); \n";
     staticDataConstructorSource += "        } \n";
     staticDataConstructorSource += "\n";
#endif
     staticDataConstructorSource += "\n";
     staticDataConstructorSource += "  // Note that this call is required to define the locations of IR nodes as addresses \n";
     staticDataConstructorSource += "  // which will be used to reset pointers from global IDs to pointer valuses within the \n";
     staticDataConstructorSource += "  // AST FILE I/O (reading binary ASTs) \n";
     staticDataConstructorSource += "     AST_FILE_IO::extendMemoryPoolsForRebuildingAST();\n";
     staticDataConstructorSource += "\n";
#if 0
  // DQ (6/5/2010): Added more error checking!!!
     staticDataConstructorSource += "\n";
     staticDataConstructorSource += "        { \n";
     staticDataConstructorSource += "       // Debugging tests.\n";
     staticDataConstructorSource += "          printf (\"After AST_FILE_IO::extendMemoryPoolsForRebuildingAST() in AstSpecificDataManagingClass::AstSpecificDataManagingClass(AstSpecificDataManagingClassStorageClass&) \\n\"); \n";
     staticDataConstructorSource += "          MemoryCheckingTraversalForAstFileIO memoryCheckingTraversal; \n";
     staticDataConstructorSource += "          memoryCheckingTraversal.traverseMemoryPool(); \n";
     staticDataConstructorSource += "        } \n";
     staticDataConstructorSource += "\n";
#endif
  // DQ (2/20/2010): Remove extraneous output from AST File I/O (required to be clean for use in Autoconf tests).
  // staticDataConstructorSource += "     std::cout << \" done\" << std::endl;\n";
     staticDataConstructorSource += "     rootOfAst = (SgProject*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex (source.rootOfAst));\n";
     staticDataConstructorSource += generateStaticDataConstructorSource(*rootNode);
#if 0
  // DQ (6/5/2010): Added more error checking!!!
     staticDataConstructorSource += "\n";
     staticDataConstructorSource += "        { \n";
     staticDataConstructorSource += "       // Debugging tests.\n";
     staticDataConstructorSource += "          printf (\"At the BOTTOM of AstSpecificDataManagingClass::AstSpecificDataManagingClass(AstSpecificDataManagingClassStorageClass&) \\n\"); \n";
     staticDataConstructorSource += "          MemoryCheckingTraversalForAstFileIO memoryCheckingTraversal; \n";
     staticDataConstructorSource += "          memoryCheckingTraversal.traverseMemoryPool(); \n";
     staticDataConstructorSource += "        } \n";
     staticDataConstructorSource += "\n";
#endif
     staticDataConstructorSource += "   }\n\n";
     StorageClassSourceFile << staticDataConstructorSource;

     std::string staticDataWriteEasyStorageDataToFileSource; 
     staticDataWriteEasyStorageDataToFileSource += "void\n";
     staticDataWriteEasyStorageDataToFileSource += "AstSpecificDataManagingClassStorageClass:: writeEasyStorageDataToFile (std::ostream& out)\n";
     staticDataWriteEasyStorageDataToFileSource += "   {\n";
     staticDataWriteEasyStorageDataToFileSource += generateStaticDataWriteEasyStorageDataToFileSource(*rootNode);
     staticDataWriteEasyStorageDataToFileSource += "   }\n\n";
     StorageClassSourceFile << staticDataWriteEasyStorageDataToFileSource;

     std::string staticDataReadEasyStorageDataFromFileSource; 
     staticDataReadEasyStorageDataFromFileSource += "void\n";
     staticDataReadEasyStorageDataFromFileSource += "AstSpecificDataManagingClassStorageClass:: readEasyStorageDataFromFile (std::istream& in)\n";
     staticDataReadEasyStorageDataFromFileSource += "   {\n";
     staticDataReadEasyStorageDataFromFileSource += generateStaticDataReadEasyStorageDataFromFileSource(*rootNode);
     staticDataReadEasyStorageDataFromFileSource += "   }\n\n";
     StorageClassSourceFile << staticDataReadEasyStorageDataFromFileSource;


     std::string staticDataArrangeEasyStorageInOnePoolSource; 
     staticDataArrangeEasyStorageInOnePoolSource += "void\n";
     staticDataArrangeEasyStorageInOnePoolSource += "AstSpecificDataManagingClassStorageClass:: arrangeStaticDataOfEasyStorageClassesInOneBlock()\n";
     staticDataArrangeEasyStorageInOnePoolSource += "   {\n";
     staticDataArrangeEasyStorageInOnePoolSource += generateStaticDataArrangeEasyStorageInOnePoolSource(*rootNode);
     staticDataArrangeEasyStorageInOnePoolSource += "   }\n\n";
     StorageClassSourceFile << staticDataArrangeEasyStorageInOnePoolSource;

     std::string staticDataDeleteEasyStorageSource; 
     staticDataDeleteEasyStorageSource += "void\n";
     staticDataDeleteEasyStorageSource += "AstSpecificDataManagingClassStorageClass:: deleteStaticDataOfEasyStorageClasses()\n";
     staticDataDeleteEasyStorageSource += "   {\n";
     staticDataDeleteEasyStorageSource += generateStaticDataDeleteEasyStorageMemoryPoolSource(*rootNode);
     staticDataDeleteEasyStorageSource += "   }\n\n";
     StorageClassSourceFile << staticDataDeleteEasyStorageSource;

     buildStorageClassSourceFiles(*rootNode,StorageClassSourceFile);

     StorageClassSourceFile << "\n\n";
     StorageClassSourceFile << "#endif // STORAGE_CLASSES_H\n";
     Grammar::writeFile(StorageClassSourceFile, target_directory, "StorageClasses", ".C");
     if (verbose)
         std::cout << "... done " << std::endl;
     return;
   }

///////////////////////////////////////////////////////////////////////////////////////////////////////////
//  Fuction definition for AstNodeClass ...
///////////////////////////////////////////////////////////////////////////////////////////////////////////
//#########################################################################################################
/* JH (12/01/2005): function for splitting the Sg-class member data into their diffrent types.
     Since I need this twice or more, I decided to build an own method! A big advantage is 
     that if we have a variable, that is not handled yet, it runs into a special case TO_HANDLE!
*/
AstNodeClass::TypeEvaluation
AstNodeClass::evaluateType(std::string& varTypeString)
   {
     AstNodeClass::TypeEvaluation returnType;
     unsigned int length = varTypeString.size();
     bool typeIsStarPointer = ( varTypeString.find("*") != std::string::npos) ;
     if ( varTypeString == "const char*" )
        {
          varTypeString =  varTypeString.substr( 6,  varTypeString.size()-1 ) ;
          returnType = CONST_CHAR_POINTER;
        }
     else if ( varTypeString == "char*" )
        {
          returnType = CHAR_POINTER;
        }
  // DQ (6/14/2008): Added support for "unsigned char*"
     else if ( varTypeString == "unsigned char*" )
        {
          returnType = CHAR_POINTER;
        }
     else if ( varTypeString == "AttachedPreprocessingInfoType*" )
        {
       // varTypeString = varTypeString.substr(0,varTypeString.size()-1) ;
          returnType = ATTACHEDPREPROCESSINGINFOTYPE;
        }
  // DQ (8/19/2008): Why is there a leading space in the string literal below?
     else if ( varTypeString == " rose_hash_multimap*" )
        {
          returnType = ROSE_HASH_MULTIMAP;
        }
  // DQ (4/25/2009): Removed pointer type to this type.
  // else if ( varTypeString == "rose_graph_hash_multimap*" )
     else if ( varTypeString == "rose_graph_hash_multimap" )
        {
          returnType = ROSE_GRAPH_HASH_MULTIMAP;
        }
  // DQ (4/25/2009): Removed pointer type to this type.
  // else if ( varTypeString == "rose_directed_graph_hash_multimap*" )
     else if ( varTypeString == "rose_directed_graph_hash_multimap" )
        {
          returnType = ROSE_GRAPH_DIRECTED_EDGE_HASH_MULTIMAP;
        }
  // DQ (4/25/2009): Removed pointer type to this type.
  // else if ( varTypeString == "rose_undirected_graph_hash_multimap*" )
     else if ( varTypeString == "rose_undirected_graph_hash_multimap" )
        {
          returnType = ROSE_GRAPH_UNDIRECTED_EDGE_HASH_MULTIMAP;
        }
  // DQ (4/27/2009): Added new type...
     else if ( varTypeString == "rose_graph_node_edge_hash_multimap" )
        {
          returnType = ROSE_GRAPH_NODE_EDGE_HASH_MULTIMAP;
        }
  // DQ (5/1/2009): Added integer to node map
     else if ( varTypeString == "rose_graph_integer_node_hash_map" )
        {
          returnType = ROSE_GRAPH_INTEGER_NODE_HASH_MAP;
        }
  // DQ (5/1/2009): Added integer to edge map
     else if ( varTypeString == "rose_graph_integer_edge_hash_map" )
        {
          returnType = ROSE_GRAPH_INTEGER_EDGE_HASH_MAP;
        }
  // DQ (5/1/2009): Added string to integer map
     else if ( varTypeString == "rose_graph_string_integer_hash_multimap" )
        {
          returnType = ROSE_GRAPH_STRING_INTEGER_HASH_MULTIMAP;
        }
  // DQ (5/1/2009): Added integer pair to edge map
     else if ( varTypeString == "rose_graph_integerpair_edge_hash_multimap" )
        {
          returnType = ROSE_GRAPH_INTEGER_PAIR_EDGE_HASH_MULTIMAP;
        }
  // DQ (5/1/2009): Added integer to edge map
     else if ( varTypeString == "rose_graph_integer_edge_hash_multimap" )
        {
          returnType = ROSE_GRAPH_INTEGER_EDGE_HASH_MULTIMAP;
        }
     else if (
               (varTypeString == "SgFunctionTypeTable*" ) ||
               (varTypeString == "$CLASSNAME*" ) || 
               (varTypeString == "$CLASSNAME *" ) || 
               (
                 (
                   ( varTypeString.substr(0,15) == "$GRAMMAR_PREFIX" ) ||
                   ( varTypeString.substr(0,2) == "Sg" )
                 ) &&
                   typeIsStarPointer
               )
             )
        {
          returnType = SGCLASS_POINTER;
        }
     else if ( varTypeString == "std::ostream*" )
        {
          returnType = OSTREAM;
        }
     else if ( varTypeString == "ROSEAttributesListContainerPtr" )
        {
          returnType = ROSEATTRUBUTESLISTCONTAINER;
        }
     else if ( 10 < length && varTypeString.substr( length-10, length) == "PtrListPtr" )
        {
          varTypeString = varTypeString.substr(0,varTypeString.size()-3) ;
          returnType = SGCLASS_POINTER_LIST_POINTER;
        }
     else if ( 7 < length && varTypeString.substr( length-7, length) == "PtrList" )
        {
          returnType = SGCLASS_POINTER_LIST;
        }
  // DQ (5/22/2006): Added case of "std::list<std::string>"
     else if ( 4 < length && varTypeString.substr( length-4, length) == "List" )
        {
          returnType = STL_CONTAINER;
        }
     else if ( 9 < length && varTypeString.substr( length-9, length) == "PtrVector" )
        {
          returnType = SGCLASS_POINTER_VECTOR;
        }
     else if ( 6 < length && varTypeString.substr(0,5) == "list<" && varTypeString.rfind(">" ) == length-1 )
        {
          returnType = STL_CONTAINER;
        }
     else if ( 11 < length && varTypeString.substr(0,10) == "std::list<" && varTypeString.rfind(">" ) == length-1 )
        {
          returnType = STL_CONTAINER;
        }
  // DQ (6/12/2007): Removed "std::" prefix from first case since varTypeString.substr(0,4) == "std::set<" is ALWAYS false.
  // DQ (3/10/2007): Added extra case for "std::" prefix.
  // else if ( 5 < length && varTypeString.substr(0,4) == "set<" && varTypeString.rfind(">" ) == length-1 )
     else if ( 5 < length && varTypeString.substr(0,4) == "set<" && varTypeString.rfind(">" ) == length-1 )
        {
          returnType = STL_SET;
        }
     else if ( 10 < length && varTypeString.substr(0,9) == "std::set<" && varTypeString.rfind(">" ) == length-1 )
        {
          returnType = STL_SET;
        }
  // DQ (6/12/2007): Added case for SgNodeSet.
     else if (varTypeString == "SgNodeSet")
        {
          returnType = STL_SET;
        }
     else if ( 10 < length && varTypeString.substr(0,9) == "std::map<" && varTypeString.rfind(">" ) == length-1 )
        {
          returnType = STL_MAP;
        }
     else if (varTypeString == "ExtentMap")
        {
          returnType = STL_MAP;
        }
     else if (varTypeString == "AddressIntervalSet")
        {
          returnType = STL_SET;
        }
     else if ( 15 < length && varTypeString.substr(0,14) == "std::multimap<" && varTypeString.rfind(">" ) == length-1 )
        {
          returnType = STL_MULTIMAP;
        }
     else if ( varTypeString == "string" )
        {
          returnType = STRING;
       }
     else if ( varTypeString == "std::string" )
        {
          returnType = STRING;
        }
     else if ( ( varTypeString == "$GRAMMAR_PREFIX_Name" ) ||  ( varTypeString == "SgName" ) )
       {
          returnType = SGNAME;
       }
     else if ( (varTypeString == "$GRAMMAR_PREFIX_BitVector" ) || (varTypeString == "SgBitVector" ))
       {
          returnType = BIT_VECTOR;
       }
  // DQ (6/14/2008): Added support for vector of unsigned char
     else if ( (varTypeString == "$GRAMMAR_PREFIX_CharList" ) || (varTypeString == "SgCharList" ))
       {
       // returnType = STRING;
          returnType = STL_CONTAINER;
       }
#if 0
  // DQ (3/10/2007): Added set of SgNode* to the symbol table to support fast existence tests on symbols without names
     else if ( varTypeString == "SgNodeSet" )
       {
          returnType = STL_SET;
       }
#endif
     else  if ( 9 < length && varTypeString.substr (length-8,length) == "Modifier" )
       {
          returnType = MODIFIERCLASS;
          if (varTypeString == "SgAccessModifier" || 
              varTypeString == "SgStorageModifier" || 
              varTypeString == "SgElaboratedTypeModifier" ||
              varTypeString == "SgUPC_AccessModifier" ||
              varTypeString == "SgConstVolatileModifier")
             {
               returnType = MODIFIERCLASS_WITHOUTEASYSTORAGE;
             }
       }
     else  if ( varTypeString == "AstAttributeMechanism*" )
       {
          returnType = ASTATTRIBUTEMECHANISM;
       }
     else  if ( varTypeString == "hash_iterator" ||
                varTypeString == "const Rose::BinaryAnalysis::CallingConvention::Definition*")
       {
          returnType = SKIP_TYPE;
       }
     else if (varTypeString == "Sawyer::Container::BitVector")
       {
         returnType = BIT_VECTOR;
       }
  // This might change, as soon as the enum types have a common name style ...
     else  if (
                 ( varTypeString == "SgUnaryOp::Sgop_mode" ) ||
                 ( varTypeString == "SgThrowOp::e_throw_kind" ) ||
                 ( varTypeString == "SgCastExp::cast_type_enum" ) ||
                 ( varTypeString == "$GRAMMAR_PREFIX_UnaryOp::$GRAMMAR_PREFIX_op_mode" ) ||
                 ( varTypeString == "SgInitializedName::preinitialization_enum" ) ||
                 ( varTypeString == "SgProject::template_instantiation_enum" ) ||
                 ( varTypeString == "SgTemplateParameter::template_parameter_enum" ) ||
                 ( varTypeString == "SgTemplateArgument::template_argument_enum" ) ||
                 ( varTypeString == "SgConstVolatileModifier::cv_modifier_enum" ) ||
                 ( varTypeString == "SgStorageModifier::storage_modifier_enum" ) ||
                 ( varTypeString == "SgAccessModifier::access_modifier_enum" ) ||
                 ( varTypeString == "SgUPC_AccessModifier::upc_access_modifier_enum" ) ||
                 ( varTypeString == "SgElaboratedTypeModifier::elaborated_type_modifier_enum" ) ||
                 ( varTypeString == "SgDeclarationStatement::template_specialization_enum" ) ||
                 ( varTypeString == "SgDeclarationModifier::gnu_declaration_visability_enum" ) ||
                 ( varTypeString == "SgTemplateDeclaration::template_type_enum" ) ||
                 ( varTypeString == "SgBaseClassModifier::baseclass_modifier_enum" ) ||
                 ( varTypeString == "SgLinkageModifier::linkage_modifier_enum" ) ||
                 ( varTypeString == "SgAsmOp::asm_operand_constraint_enum" ) ||
                 ( varTypeString == "SgAsmOp::asm_operand_modifier_enum" ) ||
                 ( varTypeString == "SgInitializedName::asm_register_name_enum" ) ||
                 ( varTypeString == "SgInitializedName::excess_specifier_enum" ) ||
                 ( varTypeString == "SgJovialDirectiveStatement::directive_types" ) ||
                 ( varTypeString == "SgTypeComplex::floating_point_precision_enum" ) ||
                 ( varTypeString == "SgTypeImaginary::floating_point_precision_enum" ) ||
                 ( varTypeString == "SgClassDeclaration::class_types" ) ||
                 ( varTypeString == "SgTemplateClassDeclaration::class_types" ) ||
                 ( varTypeString == "SgStopOrPauseStatement::stop_or_pause_enum" ) ||
                 ( varTypeString == "SgIOStatement::io_statement_enum" ) ||
                 ( varTypeString == "SgForAllStatement::forall_statement_kind_enum" ) ||
                 ( varTypeString == "SgImageControlStatement::image_control_statement_enum" ) ||
                 ( varTypeString == "SgAttributeSpecificationStatement::attribute_spec_enum" ) ||
                 ( varTypeString == "SgDataStatementValue::data_statement_value_enum" ) ||
                 ( varTypeString == "SgFile::outputFormatOption_enum" ) ||

              // DQ (29/8/2017): Added generalization of language specifications so we can use 
              // a single language enum for specification of both input and output languges.
              // ( varTypeString == "SgFile::outputLanguageOption_enum" ) ||
                 ( varTypeString == "SgFile::languageOption_enum" ) ||

                 ( varTypeString == "SgOmpClause::omp_default_option_enum" ) ||
                 ( varTypeString == "SgOmpClause::omp_proc_bind_policy_enum" ) ||
                 ( varTypeString == "SgOmpClause::omp_atomic_clause_enum" ) ||
                 ( varTypeString == "SgOmpClause::omp_schedule_kind_enum" ) ||
                 ( varTypeString == "SgOmpClause::omp_reduction_operator_enum" ) ||
                 ( varTypeString == "SgOmpClause::omp_dependence_type_enum" ) ||
                 ( varTypeString == "SgOmpClause::omp_map_operator_enum" ) ||
                 ( varTypeString == "SgOmpClause::omp_map_dist_data_enum" ) ||
                 ( varTypeString == "SgProcedureHeaderStatement::subprogram_kind_enum" ) ||
                 ( varTypeString == "SgLabelSymbol::label_type_enum" ) ||
                 ( varTypeString == "SgAsmFunction::function_kind_enum" ) ||
                 ( varTypeString == "SgTypeModifier::gnu_extension_machine_mode_enum" ) ||
                 ( varTypeString == "SgDeclarationStatement::gnu_extension_visability_attribute_enum" ) ||
                 ( varTypeString == "SgVariableDeclaration::gnu_extension_declaration_attributes_enum" ) ||
                 ( varTypeString == "X86InstructionKind" ) || "Rose::BinaryAnalysis::X86InstructionKind" == varTypeString ||
                 ( varTypeString == "X86RegisterClass" ) || "Rose::BinaryAnalysis::X86RegisterClass" == varTypeString ||
                 ( varTypeString == "X86SegmentRegister" ) || "Rose::BinaryAnalysis::X86SegmentRegister" == varTypeString ||
                 ( varTypeString == "X86BranchPrediction" ) || "Rose::BinaryAnalysis::X86BranchPrediction" == varTypeString ||
                 ( varTypeString == "X86RepeatPrefix" ) || "Rose::BinaryAnalysis::X86RepeatPrefix" == varTypeString ||
                 ( varTypeString == "X86PositionInRegister" ) || "Rose::BinaryAnalysis::X86PositionInRegister" == varTypeString ||
                 ( varTypeString == "X86InstructionSize" ) || "Rose::BinaryAnalysis::X86InstructionSize" == varTypeString ||
                 ( varTypeString == "Arm64InstructionKind" ) || "Rose::BinaryAnalysis::Arm64InstructionKind" == varTypeString ||
                 ( varTypeString == "Arm64InstructionCondition" ) || "Rose::BinaryAnalysis::Arm64InstructionCondition" == varTypeString ||
                 ( varTypeString == "PowerpcInstructionKind" ) || "Rose::BinaryAnalysis::PowerpcInstructionKind" == varTypeString ||
                 ( varTypeString == "PowerpcRegisterClass" ) || "Rose::BinaryAnalysis::PowerpcRegisterClass" == varTypeString ||
                 ( varTypeString == "PowerpcConditionRegisterAccessGranularity" ) || "Rose::BinaryAnalysis::PowerpcConditionRegisterAccessGranularity" == varTypeString ||
                 ( varTypeString == "PowerpcSpecialPurposeRegister" ) || "Rose::BinaryAnalysis::PowerpcSpecialPurposeRegister" == varTypeString ||
                 ( varTypeString == "PowerpcTimeBaseRegister" ) || "Rose::BinaryAnalysis::PowerpcTimeBaseRegister" == varTypeString ||
                 ( varTypeString == "MipsInstructionKind") || "Rose::BinaryAnalysis::MipsInstructionKind" == varTypeString ||
                 ( varTypeString == "M68kInstructionKind") || "Rose::BinaryAnalysis::M68kInstructionKind" == varTypeString ||
                 ( varTypeString == "ByteOrder::Endianness" ) || "Rose::BinaryAnalysis::ByteOrder::Endianness" == varTypeString ||
              // Note that these enum names do not conform to the naming scheme used in ROSE.
                 ( varTypeString == "SgAsmGenericSection::SectionPurpose" ) ||
                 ( varTypeString == "SgAsmGenericFormat::InsSetArchitecture" ) ||
                 ( varTypeString == "SgAsmGenericFormat::ExecFamily" ) ||
                 ( varTypeString == "SgAsmGenericFormat::ExecPurpose" ) ||
                 ( varTypeString == "SgAsmGenericFormat::ExecABI" ) ||
                 ( varTypeString == "SgAsmGenericSymbol::SymbolDefState" ) ||
                 ( varTypeString == "SgAsmGenericSymbol::SymbolBinding" ) ||
                 ( varTypeString == "SgAsmGenericSymbol::SymbolType" ) ||
                 ( varTypeString == "SgAsmElfSegmentTableEntry::SegmentType" ) ||
                 ( varTypeString == "SgAsmElfSegmentTableEntry::SegmentFlags" ) ||
                 ( varTypeString == "SgAsmElfDynamicEntry::EntryType") ||
                 ( varTypeString == "SgAsmElfRelocEntry::RelocType") ||
                 ( varTypeString == "SgAsmNEEntryPoint::NEEntryFlags" ) ||
                 ( varTypeString == "SgAsmNERelocEntry::NERelocSrcType" ) ||
                 ( varTypeString == "SgAsmNERelocEntry::NERelocModifiers" ) ||
                 ( varTypeString == "SgAsmNERelocEntry::NERelocTgtType" ) ||
                 ( varTypeString == "SgAsmNERelocEntry::NERelocFlags" ) ||
                 ( varTypeString == "SgInterfaceStatement::generic_spec_enum" ) ||
                 ( varTypeString == "SgAsmElfSectionTableEntry::SectionType") ||
                 ( varTypeString == "SgAsmElfSectionTableEntry::SectionFlags") ||
                 ( varTypeString == "SgOpenclAccessModeModifier::access_mode_modifier_enum") ||
                 ( varTypeString == "SgAsmFunction::MayReturn") ||
                 ( varTypeString == "SgAsmRiscOperation::RiscOperator") ||
              // DQ (11/26/2013): Added to support use of enums from SgToken class.
                 ( varTypeString == "SgToken::ROSE_Fortran_Operators" ) ||
                 ( varTypeString == "SgToken::ROSE_Fortran_Keywords" ) ||
              // DQ (12/9/2015): Added to support use of enums from SgUntypedType class.
                 ( varTypeString == "SgUntypedType::type_enum" ) ||
                 ( varTypeString == "SgFile::standard_enum" ) ||
                 false 
              )
       {
          returnType = ENUM_TYPE;
       }
     else  if (  // basic data types
                 ( varTypeString == "bool" ) ||
                 ( varTypeString == "int" ) || 
                 ( varTypeString == "signed int" ) ||
                 ( varTypeString == "unsigned int" ) ||
                 ( varTypeString == "unsigned" ) ||  
                 ( varTypeString == "long" ) ||
                 ( varTypeString == "unsigned long" ) ||
                 ( varTypeString == "signed long" ) ||
                 ( varTypeString == "long long" ) ||
                 ( varTypeString == "double" ) ||
                 ( varTypeString == "long double" ) ||
                 ( varTypeString == "float" ) ||
                 ( varTypeString == "unsigned short" ) ||
                 ( varTypeString == "short" ) ||
                 ( varTypeString == "unsigned long int" ) ||
                 ( varTypeString == "char" ) ||
                 ( varTypeString == "unsigned char" ) ||
                 ( varTypeString == "long int" ) || 
                 ( varTypeString == "long long int" ) ||
                 ( varTypeString == "unsigned long long int" ) ||
                 ( varTypeString == "int64_t") ||
                 ( varTypeString == "rose_addr_t" ) ||
                 ( varTypeString == "rose_rva_t") ||
                 ( varTypeString == "uint64_t" ) ||
                 ( varTypeString == "uint32_t" ) ||
                 ( varTypeString == "uint16_t" ) ||
                 ( varTypeString == "uint8_t" ) ||
                 ( varTypeString == "size_t" ) ||
                 ( varTypeString == "time_t" ) ||
              // DQ (8/7/2008): Note that these are not primative types, but I will account 
              // for them as such at least temporarily so that we can be the IR in place.
                 ( varTypeString == "SgAsmNERelocEntry::iref_type" ) ||
                 ( varTypeString == "SgAsmNERelocEntry::iord_type" ) ||
                 ( varTypeString == "SgAsmNERelocEntry::iname_type" ) ||
                 ( varTypeString == "SgAsmNERelocEntry::osfixup_type" ) ||
                 ( varTypeString == "RegisterDescriptor") || ( varTypeString == "Rose::BinaryAnalysis::RegisterDescriptor" ) ||
              // DQ (8/8/2008): Added typedef for primative types (used in binary format)
                 ( varTypeString == "SgAsmGenericFormat::fileDetails" ) ||
              // DQ (8/8/2008): This is a typedef to a std::vector<ExtentPair>, this should likely be supported elsewhere.
                 ( varTypeString == "SgGraphNodeDirectedGraphEdgeMultimapPtrList" ) ||
              // TV (05/03/2010): Added support for 'SgFunctionModifier::opencl_work_group_size_t' which's  a struct of 3 longs !
                 ( varTypeString == "SgFunctionModifier::opencl_work_group_size_t") ||
                 false
              )
       {
          returnType = BASIC_DATA_TYPE;
       }
     else if (varTypeString == "SgAsmGenericStrtab::referenced_t")
       {
         return SGCLASS_POINTER_LIST;
       }
     else
       {
          returnType = TO_HANDLE;
       }
     return returnType;
   }

//#########################################################################################################
/* JH (10/28/2005) : Build the member data for the IR node's StorageClasses. Using the switch we can decide
   if a data has to be stored and also in which manner. The static data members become static data 
   become static data members in the StorageClasses. 
*/
std::string AstNodeClass::buildStorageClassHeader ()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     string s;
     string classNameString = this-> name;
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          string varStorageNameString = "storageOf_" + varNameString;
          if ( varNameString != "freepointer" && varTypeString.substr(0,7) !=  "static " ) 
             {
               switch (evaluateType(varTypeString) )
                  {
                    case SGCLASS_POINTER:
                      s += "     unsigned long " + varStorageNameString +";\n";
                      break;
                    case ASTATTRIBUTEMECHANISM:
                    case ATTACHEDPREPROCESSINGINFOTYPE:
                    case BIT_VECTOR:
                    case CHAR_POINTER:
                    case CONST_CHAR_POINTER:
                    case ROSE_HASH_MULTIMAP:
                    case ROSE_GRAPH_HASH_MULTIMAP:
                    case ROSE_GRAPH_DIRECTED_EDGE_HASH_MULTIMAP:
                    case ROSE_GRAPH_UNDIRECTED_EDGE_HASH_MULTIMAP:
                    case ROSE_GRAPH_NODE_EDGE_HASH_MULTIMAP:

                 // DQ (5/1/2009): Added support for rose_graph_integer_node_hash_map and rose_graph_integer_edge_hash_map
                    case ROSE_GRAPH_INTEGER_NODE_HASH_MAP:
                    case ROSE_GRAPH_INTEGER_EDGE_HASH_MAP:
                    case ROSE_GRAPH_INTEGER_EDGE_HASH_MULTIMAP:

                 // DQ (5/1/2009): Added support for rose_graph_string_integer_hash_multimap
                    case ROSE_GRAPH_STRING_INTEGER_HASH_MULTIMAP:

                 // DQ (5/1/2009): Added support for rose_graph_integerpair_edge_hash_multimap
                    case ROSE_GRAPH_INTEGER_PAIR_EDGE_HASH_MULTIMAP:

                    case ROSEATTRUBUTESLISTCONTAINER:
                    case SGCLASS_POINTER_LIST:
                    case SGCLASS_POINTER_LIST_POINTER:
                    case SGCLASS_POINTER_VECTOR:
                    case STL_CONTAINER:
                    case STL_SET:
                    case STL_MAP:
                    case STL_MULTIMAP:
                    case STRING:
                      s += "       EasyStorage < " + varTypeString + " > " + varStorageNameString +";\n" ;
                      break;
                    case BASIC_DATA_TYPE:
                    case ENUM_TYPE:
                      s += "      " + varTypeString + " " + varStorageNameString +";\n";
                      break;
                    case MODIFIERCLASS:
                    case MODIFIERCLASS_WITHOUTEASYSTORAGE:
                    case SGNAME:
                      s += "      " + varTypeString + "StorageClass " + varStorageNameString +";\n";
                      break;
                    case OSTREAM:
                    case SKIP_TYPE:
                      break;
                    case TO_HANDLE:
                    default:
                   /* JH (01/29/2006) If ROSETTA stops at this assert, then a variable occurred, that is 
                    * not yet handled. However, there is only an assert at this point, nowehere else in
                    * the following methods below!
                    */
                      std::cout << " There is a class not handled in buildStorageClasses.C, Line " 
                                << __LINE__ << std::endl ;
                      std::cout << "In class " + classNameString + " caused by variable " 
                                << varTypeString + " p_" + varNameString << std::endl ;
                      assert (!"Stop immediately, since variable to build is not found ... " ) ;
                      break;
                 }
            }
       }
     return s;
   }

//#########################################################################################################
/* JH (10/28/2005) : building the source code of the initializer method of the StorageClasses. All data 
 * members of the IR node counterparts (except of static members) are visited and their contents are stored
 * in the designated variables of the StoragClass. We have to get the data members of the parents of the IR
 * node, as well ! 
 * REMARKS:
 *  * This methods operate on the data members of the parent classes, as well.
 *  * Actually, NULL pointers only are rebuilt for char*, rose_hash_multimap, AstAttributeMechanism, 
 *    AttachedPreprocessingInfoType, and pointers to STL containers containing pointers to IR nodes (and 
 *    pointers to other IR nodes, of course).
 *  * The pointer to an STL container that stores pointers to other IR nodes is still shared in my version.
 *    Thus, we copy the pointers, replace the pointers by its global indices in the container, store the 
 *    container and copy back the original ones.
 */
string AstNodeClass::buildStorageClassPickOutIRNodeDataSource ()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     string classNameString = this-> name;
     string s ;
  // Maybe, this could be handled nicer by using a macro implementation ... however, I put it here!
     s += "     " + classNameString + "* source = (" + classNameString +"*) (pointer); \n" ;
     s += "#if FILE_IO_EXTRA_CHECK \n" ;
     s += "     assert ( source != NULL ) ; \n";
     s += "     assert ( source->p_freepointer != NULL) ; \n";
     s += "#endif \n" ;

     for (AstNodeClass *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
          for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
             {
               GrammarString *data = *stringListIterator;
               string varNameString = string(data->getVariableNameString());
               string varTypeString = string(data->getTypeNameString());
               string varStorageNameString = "storageOf_" + varNameString;
               string sg_string  = varTypeString;
               if (varNameString != "freepointer"  &&  varTypeString.substr(0,7) !=  "static " )
                  {
                    switch (evaluateType(varTypeString) )
                       {
                         case SGCLASS_POINTER:
                           s += "     " + varStorageNameString + " = "\
                                "AST_FILE_IO::getGlobalIndexFromSgClassPointer ( source->p_" + varNameString + " );\n" ;
                           break;

                         case ROSE_HASH_MULTIMAP:
                        // s += "     rose_hash::unordered_multimap<SgName, SgSymbol*, hash_Name, eqstr>::iterator it; \n" ;
                           // CH (4/8/2010): Use boost::unordered instead
                           //s += "#ifdef _MSCx_VER \n";
                           //s += "     rose_hash::unordered_multimap<SgName, SgSymbol*>::iterator it; \n" ;
                           //s += "#else \n";
                           //s += "     rose_hash::unordered_multimap<SgName, SgSymbol*, hash_Name, eqstr>::iterator it; \n" ;
                           s += "     rose_hash_multimap::iterator it; \n" ;
                           //s += "#endif \n";
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgSymbol** tempList" + varNameString + " = new SgSymbol* [ source->p_" + varNameString + "->size() ]; \n" ;
                           s += "     for (it = source->p_" + varNameString + "->begin(); it != source->p_" + varNameString + "->end(); ++it)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = it->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          it->second = (SgSymbol*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(it->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (it = source->p_" + varNameString + "->begin(); it != source->p_" + varNameString + "->end(); ++it) \n";
                           s += "        {\n";
                           s += "          it->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
                           break;

                         case ROSE_GRAPH_HASH_MULTIMAP:
#if 0
                           s += "     rose_hash::unordered_multimap<std::string, SgGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgGraphNode** tempList" + varNameString + " = new SgGraphNode* [ source->p_" + varNameString + "->size() ]; \n" ;
                           s += "     for (it = source->p_" + varNameString + "->begin(); it != source->p_" + varNameString + "->end(); ++it)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = it->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          it->second = (SgGraphNode*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(it->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (it = source->p_" + varNameString + "->begin(); it != source->p_" + varNameString + "->end(); ++it) \n";
                           s += "        {\n";
                           s += "          it->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
#else
                        // s += "     rose_hash::unordered_multimap<std::string, SgGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n" ;
                           // CH (4/8/2010): Use boost::unordered instead
                           //s += "#ifdef _MSCx_VER \n";
                           //s += "     rose_hash::unordered_multimap<std::string, SgGraphNode*>::iterator it; \n" ;
                           //s += "#else \n";
                           //s += "     rose_hash::unordered_multimap<std::string, SgGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n" ;
                           s += "     rose_graph_hash_multimap::iterator it; \n" ;
                           //s += "#endif \n";
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgGraphNode** tempList" + varNameString + " = new SgGraphNode* [ source->p_" + varNameString + ".size() ]; \n" ;
                           s += "     for (it = source->p_" + varNameString + ".begin(); it != source->p_" + varNameString + ".end(); ++it)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = it->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          it->second = (SgGraphNode*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(it->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (it = source->p_" + varNameString + ".begin(); it != source->p_" + varNameString + ".end(); ++it) \n";
                           s += "        {\n";
                           s += "          it->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
#endif
                           break;

#if 0
                      // DQ (5/2/2009): These are no longer used
                         case ROSE_GRAPH_DIRECTED_EDGE_HASH_MULTIMAP:
#if 0
                           s += "   {\n";
                           s += "     rose_hash::unordered_multimap<std::string, SgDirectedGraphEdge*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it_1; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgDirectedGraphEdge** tempList" + varNameString + " = new SgDirectedGraphEdge* [ source->p_" + varNameString + "->size() ]; \n" ;
                           s += "     for (it_1 = source->p_" + varNameString + "->begin(); it_1 != source->p_" + varNameString + "->end(); ++it_1)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = it_1->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          it_1->second = (SgDirectedGraphEdge*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(it_1->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (it_1 = source->p_" + varNameString + "->begin(); it_1 != source->p_" + varNameString + "->end(); ++it_1) \n";
                           s += "        {\n";
                           s += "          it_1->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
                           s += "    }\n";
#else
                        // DQ (4/25/2009): Removing pointer to type for data member to improve the interface...
                           s += "   {\n";
                           s += "     rose_hash::unordered_multimap<std::string, SgDirectedGraphEdge*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it_1; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgDirectedGraphEdge** tempList" + varNameString + " = new SgDirectedGraphEdge* [ source->p_" + varNameString + ".size() ]; \n" ;
                           s += "     for (it_1 = source->p_" + varNameString + ".begin(); it_1 != source->p_" + varNameString + ".end(); ++it_1)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = it_1->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          it_1->second = (SgDirectedGraphEdge*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(it_1->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (it_1 = source->p_" + varNameString + ".begin(); it_1 != source->p_" + varNameString + ".end(); ++it_1) \n";
                           s += "        {\n";
                           s += "          it_1->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
                           s += "    }\n";
#endif
                           break;
#endif

#if 0
                      // DQ (5/2/2009): These are no longer used
                         case ROSE_GRAPH_UNDIRECTED_EDGE_HASH_MULTIMAP:
#if 0
                           s += "     rose_hash::unordered_multimap<std::string, SgUndirectedGraphEdge*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it_2; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgUndirectedGraphEdge** tempList" + varNameString + " = new SgUndirectedGraphEdge* [ source->p_" + varNameString + "->size() ]; \n" ;
                           s += "     for (it_2 = source->p_" + varNameString + "->begin(); it_2 != source->p_" + varNameString + "->end(); ++it_2)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = it_2->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          it_2->second = (SgUndirectedGraphEdge*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(it_2->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (it_2 = source->p_" + varNameString + "->begin(); it_2 != source->p_" + varNameString + "->end(); ++it_2) \n";
                           s += "        {\n";
                           s += "          it_2->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
#else
                        // DQ (4/25/2009): Removing pointer to type for data member to improve the interface...
                           s += "     rose_hash::unordered_multimap<std::string, SgUndirectedGraphEdge*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it_2; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgUndirectedGraphEdge** tempList" + varNameString + " = new SgUndirectedGraphEdge* [ source->p_" + varNameString + ".size() ]; \n" ;
                           s += "     for (it_2 = source->p_" + varNameString + ".begin(); it_2 != source->p_" + varNameString + ".end(); ++it_2)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = it_2->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          it_2->second = (SgUndirectedGraphEdge*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(it_2->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (it_2 = source->p_" + varNameString + ".begin(); it_2 != source->p_" + varNameString + ".end(); ++it_2) \n";
                           s += "        {\n";
                           s += "          it_2->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
#endif
                           break;
#endif
                         case ROSE_GRAPH_NODE_EDGE_HASH_MULTIMAP:
                           s += "   {\n";
                           s += "     SgGraphEdgeList::local_hash_multimap_type::iterator it; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgGraphEdge** tempList" + varNameString + " = new SgGraphEdge* [ source->p_" + varNameString + ".size() ]; \n" ;
                           s += "     for (it = source->p_" + varNameString + ".begin(); it != source->p_" + varNameString + ".end(); ++it)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = it->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          it->second = (SgGraphEdge*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(it->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (it = source->p_" + varNameString + ".begin(); it != source->p_" + varNameString + ".end(); ++it) \n";
                           s += "        {\n";
                           s += "          it->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
                           s += "    }\n";
                           break;

                      // DQ (5/1/2009): Added support for rose_graph_string_integer_hash_multimap
                         case ROSE_GRAPH_STRING_INTEGER_HASH_MULTIMAP:
                           s += "   {\n";
                           s += "     rose_graph_string_integer_hash_multimap::iterator string_integer_it; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     int* tempList" + varNameString + " = new int [ source->p_" + varNameString + ".size() ]; \n" ;
                           s += "     for (string_integer_it = source->p_" + varNameString + ".begin(); string_integer_it != source->p_" + varNameString + ".end(); ++string_integer_it)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = string_integer_it->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          string_integer_it->second = 0; printf (\"Unimplemented support for rose_graph_string_integer_hash_multimap: Fixme! \\n\"); ROSE_ASSERT(false); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (string_integer_it = source->p_" + varNameString + ".begin(); string_integer_it != source->p_" + varNameString + ".end(); ++string_integer_it) \n";
                           s += "        {\n";
                           s += "          string_integer_it->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
                           s += "    }\n";
                           break;

                      // DQ (5/1/2009): Added support for rose_graph_integer_node_hash_map and rose_graph_integer_edge_hash_map
                         case ROSE_GRAPH_INTEGER_NODE_HASH_MAP:
                           s += "     rose_graph_integer_node_hash_map::iterator integer_node_it; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgGraphNode** tempList" + varNameString + " = new SgGraphNode* [ source->p_" + varNameString + ".size() ]; \n" ;
                           s += "     for (integer_node_it = source->p_" + varNameString + ".begin(); integer_node_it != source->p_" + varNameString + ".end(); ++integer_node_it)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = integer_node_it->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          integer_node_it->second = (SgGraphNode*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(integer_node_it->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (integer_node_it = source->p_" + varNameString + ".begin(); integer_node_it != source->p_" + varNameString + ".end(); ++integer_node_it) \n";
                           s += "        {\n";
                           s += "          integer_node_it->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
                           break;

                      // DQ (5/1/2009): Added support for rose_graph_integer_node_hash_map and rose_graph_integer_edge_hash_map
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MAP:
                           s += "     rose_graph_integer_edge_hash_map::iterator integer_edge_it; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgGraphEdge** tempList" + varNameString + " = new SgGraphEdge* [ source->p_" + varNameString + ".size() ]; \n" ;
                           s += "     for (integer_edge_it = source->p_" + varNameString + ".begin(); integer_edge_it != source->p_" + varNameString + ".end(); ++integer_edge_it)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = integer_edge_it->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          integer_edge_it->second = (SgGraphEdge*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(integer_edge_it->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (integer_edge_it = source->p_" + varNameString + ".begin(); integer_edge_it != source->p_" + varNameString + ".end(); ++integer_edge_it) \n";
                           s += "        {\n";
                           s += "          integer_edge_it->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
                           break;

                      // DQ (5/1/2009): Added support for rose_graph_integer_edge_hash_multimap
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MULTIMAP:
                           s += "   {\n";
                           s += "     rose_graph_integer_edge_hash_multimap::iterator integer_edge_it2; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgGraphEdge** tempList" + varNameString + " = new SgGraphEdge* [ source->p_" + varNameString + ".size() ]; \n" ;
                           s += "     for (integer_edge_it2 = source->p_" + varNameString + ".begin(); integer_edge_it2 != source->p_" + varNameString + ".end(); ++integer_edge_it2)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = integer_edge_it2->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          integer_edge_it2->second = (SgGraphEdge*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(integer_edge_it2->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (integer_edge_it2 = source->p_" + varNameString + ".begin(); integer_edge_it2 != source->p_" + varNameString + ".end(); ++integer_edge_it2) \n";
                           s += "        {\n";
                           s += "          integer_edge_it2->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
                           s += "    }\n";
                           break;

                      // DQ (5/1/2009): Added support for rose_graph_integerpair_edge_hash_multimap
                         case ROSE_GRAPH_INTEGER_PAIR_EDGE_HASH_MULTIMAP:
                           s += "     rose_graph_integerpair_edge_hash_multimap::iterator integerpair_edge_it; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     SgGraphEdge** tempList" + varNameString + " = new SgGraphEdge* [ source->p_" + varNameString + ".size() ]; \n" ;
                           s += "     for (integerpair_edge_it = source->p_" + varNameString + ".begin(); integerpair_edge_it != source->p_" + varNameString + ".end(); ++integerpair_edge_it)\n" ;
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = integerpair_edge_it->second;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          integerpair_edge_it->second = (SgGraphEdge*)(AST_FILE_IO::getGlobalIndexFromSgClassPointer(integerpair_edge_it->second) ); \n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     for (integerpair_edge_it = source->p_" + varNameString + ".begin(); integerpair_edge_it != source->p_" + varNameString + ".end(); ++integerpair_edge_it) \n";
                           s += "        {\n";
                           s += "          integerpair_edge_it->second = tempList" + varNameString + " [ tempListCount" + varNameString + " ]; \n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
                           break;

                         case SGCLASS_POINTER_VECTOR:
                         case SGCLASS_POINTER_LIST:
                           sg_string += "::value_type";
                           s += "     " + varTypeString + "::iterator i_" + varNameString + " = source->p_" + varNameString + ".begin() ; \n" ;
                           s += "     unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "     "+sg_string+" *  tempList" + varNameString + " = new "+sg_string+"[ source->p_" + varNameString + ".size() ]; \n" ;
                           s += "     for ( ; i_" + varNameString + " != source->p_" + varNameString + ".end(); ++i_" + varNameString + " ) \n";
                           s += "        {\n";
                           s += "          tempList" + varNameString + "[tempListCount" + varNameString + "] = *i_" + varNameString + ";\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "          (*i_" + varNameString + ") = "\
                                "("+sg_string+")(AST_FILE_IO::getGlobalIndexFromSgClassPointer ( *i_"+varNameString+" ) );\n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           s += "     tempListCount" + varNameString + " = 0; \n" ;
                           s += "     i_" + varNameString + " = source->p_" + varNameString + ".begin() ; \n" ;
                           s += "     for ( ; i_" + varNameString + " != source->p_" + varNameString + ".end(); ++i_" + varNameString + " ) \n";
                           s += "        {\n";
                           s += "          *i_" + varNameString + " = tempList" + varNameString + "[tempListCount" + varNameString + "] ;\n";
                           s += "          tempListCount" + varNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + varNameString + "; \n";
                           break;
                         case SGCLASS_POINTER_LIST_POINTER:
                           sg_string = varTypeString.substr(0,varTypeString.size()-7) ;
                           s += "     if ( source->p_" + varNameString + " != NULL ) \n" ;
                           s += "        { \n" ;
                           s += "          " + varTypeString + "::iterator i_" + varNameString + "; \n" ;
                           s += "          unsigned int tempListCount" + varNameString + " = 0; \n" ;
                           s += "          "+sg_string+" ** tempList" + varNameString + " = new "+sg_string+" * [ source->p_"+varNameString+"->size() ]; \n" ;
                           s += "          i_" + varNameString + " = source->p_" + varNameString + "->begin() ; \n" ;
                           s += "          for ( ; i_" + varNameString + " != source->p_" + varNameString + "->end(); ++i_" + varNameString + " ) \n";
                           s += "             {\n";
                           s += "               tempList" + varNameString + "[tempListCount" + varNameString + "] = *i_" + varNameString + " ;\n";
                           s += "               tempListCount" + varNameString + "++; \n";
                           s += "               (*i_"+varNameString+") = "\
                                "("+sg_string+"* )(AST_FILE_IO::getGlobalIndexFromSgClassPointer ( *i_"+varNameString+" ) );\n";
                           s += "             }\n";
                           s += "          " + varStorageNameString + ".storeDataInEasyStorageClass(* (source->p_" + varNameString + ") );\n" ;
                           s += "          tempListCount" + varNameString + " = 0; \n" ;
                           s += "          i_" + varNameString + " = source->p_" + varNameString + "->begin() ; \n" ;
                           s += "          for ( ; i_" + varNameString + " != source->p_" + varNameString + "->end(); ++i_" + varNameString + " ) \n";
                           s += "             {\n";
                           s += "               *i_"+varNameString+" = tempList" + varNameString + "[tempListCount" + varNameString + "] ;\n";
                           s += "               tempListCount" + varNameString + "++; \n";
                           s += "             }\n";
                           s += "           delete [] tempList" + varNameString + "; \n";
                           s += "        }\n" ;
                           break;
                         case ASTATTRIBUTEMECHANISM:
                         case ATTACHEDPREPROCESSINGINFOTYPE:
                         case BIT_VECTOR:
                         case CHAR_POINTER:
                         case CONST_CHAR_POINTER:
                         case ROSEATTRUBUTESLISTCONTAINER:
                         case STL_CONTAINER:
                         case STL_SET:
                      // DQ (10/4/2006): Added case of STL_MAP
                         case STL_MAP:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case STRING:
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->p_" + varNameString + ");\n" ;
                           break;
                         case MODIFIERCLASS:
                         case MODIFIERCLASS_WITHOUTEASYSTORAGE:
                         case SGNAME:
                           s += "     " + varStorageNameString + ".pickOutIRNodeData( &(source->p_" + varNameString + ") );\n" ;
                           break;
                         case BASIC_DATA_TYPE:
                         case ENUM_TYPE:
                           s += "     " + varStorageNameString +" =  source->p_" + varNameString + " ;\n";
                           break;
                         case OSTREAM:
                         case SKIP_TYPE:
                         case TO_HANDLE:
                           break;

                         default:
                           std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                           std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                           assert (!"Stop immediately, since variable to build is not found ... " ) ;
                           break;
                       }
                  }
             }
        }
     return s;
   }

//#########################################################################################################
/* JH (10/28/2005) : build the source code for deleting the static data caused by the EasyStorage members 
   within the StorageClasses! Since a StorageClass holds all data of its counterpart and those parents, we 
   have to call this method on all suitable members! But since we what to avoid repeatings in the call of 
   the methods, we skip dublicates (realized by the addString)!
*/
string AstNodeClass::buildStorageClassDeleteStaticDataSource ()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     string classNameString = this-> name;
     std::string s  ;
     std::string addString ;
     for (AstNodeClass *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
          for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
             {
               addString = "";
               GrammarString *data = *stringListIterator;
               string varNameString = string(data->getVariableNameString());
               string varTypeString = string(data->getTypeNameString());
               if ( varTypeString.substr(0,7) !=  "static " )
                  {
                    switch (evaluateType(varTypeString) )
                       {
                         case ASTATTRIBUTEMECHANISM:
                         case ATTACHEDPREPROCESSINGINFOTYPE:
                         case BIT_VECTOR:
                         case CHAR_POINTER:
                         case CONST_CHAR_POINTER:
                         case ROSE_HASH_MULTIMAP:
                         case ROSE_GRAPH_HASH_MULTIMAP:
                         case ROSE_GRAPH_DIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_UNDIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_NODE_EDGE_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_integer_node_hash_map and rose_graph_integer_edge_hash_map
                         case ROSE_GRAPH_INTEGER_NODE_HASH_MAP:
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MAP:
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_string_integer_hash_multimap
                         case ROSE_GRAPH_STRING_INTEGER_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_integerpair_edge_hash_multimap
                         case ROSE_GRAPH_INTEGER_PAIR_EDGE_HASH_MULTIMAP:

                         case ROSEATTRUBUTESLISTCONTAINER:
                         case SGCLASS_POINTER_LIST:
                         case SGCLASS_POINTER_LIST_POINTER:
                         case SGCLASS_POINTER_VECTOR:
                         case STL_CONTAINER:
                         case STL_SET:
                         case STL_MAP:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case STRING:
                           addString += "     EasyStorage < " + varTypeString + " > :: deleteMemoryPool() ;\n" ;
                           break;
                         case MODIFIERCLASS:
                         case SGNAME:
                           addString += "     " + varTypeString + "StorageClass :: deleteStaticDataOfEasyStorageClasses() ;\n" ;
                           break;
                         case BASIC_DATA_TYPE:
                         case ENUM_TYPE:
                         case MODIFIERCLASS_WITHOUTEASYSTORAGE:
                         case OSTREAM:
                         case SKIP_TYPE:
                         case SGCLASS_POINTER:
                           break;
                         case TO_HANDLE:

                         default:
                            std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                            std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                            assert (!"Stop immediately, since variable to build is not found ... " ) ;
                           break;
                      }
                    if ( addString != "" )
                       {
                           if ( s.find(addString) == std::string::npos )
                              {
                                 s += addString;
                               }
                         }
                  }
             }
        }
     return s;
   }

//#########################################################################################################
/* JH (10/28/2005) : Similar to the method above, but it arranges the static data of the EasyStorage to be 
   contained in one block!
*/
string AstNodeClass::buildStorageClassArrangeStaticDataInOneBlockSource ()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     string classNameString = this-> name;
     std::string s;
     std::string addString;
     for (AstNodeClass *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
          for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
             {
               addString = "";
               GrammarString *data = *stringListIterator;
               string varNameString = string(data->getVariableNameString());
               string varTypeString = string(data->getTypeNameString());
               if ( varTypeString.substr(0,7) !=  "static " )
                  {
                    switch (evaluateType(varTypeString) )
                       {
                         case ASTATTRIBUTEMECHANISM:
                         case ATTACHEDPREPROCESSINGINFOTYPE:
                         case BIT_VECTOR:
                         case CHAR_POINTER:
                         case CONST_CHAR_POINTER:
                         case ROSE_HASH_MULTIMAP:
                         case ROSE_GRAPH_HASH_MULTIMAP:
                         case ROSE_GRAPH_DIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_UNDIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_NODE_EDGE_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_integer_node_hash_map and rose_graph_integer_edge_hash_map
                         case ROSE_GRAPH_INTEGER_NODE_HASH_MAP:
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MAP:
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_string_integer_hash_multimap
                         case ROSE_GRAPH_STRING_INTEGER_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_integerpair_edge_hash_multimap
                         case ROSE_GRAPH_INTEGER_PAIR_EDGE_HASH_MULTIMAP:

                         case ROSEATTRUBUTESLISTCONTAINER:
                         case SGCLASS_POINTER_LIST:
                         case SGCLASS_POINTER_LIST_POINTER:
                         case SGCLASS_POINTER_VECTOR:
                         case STL_CONTAINER:
                         case STL_SET:
                         case STL_MAP:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case STRING:
                           addString += "     EasyStorage < " + varTypeString + " > :: arrangeMemoryPoolInOneBlock() ;\n" ;
                           break;
                         case MODIFIERCLASS:
                         case SGNAME:
                           addString += "     " + varTypeString + "StorageClass :: arrangeStaticDataOfEasyStorageClassesInOneBlock() ;\n" ;
                           break;
                         case BASIC_DATA_TYPE:
                         case ENUM_TYPE:
                         case MODIFIERCLASS_WITHOUTEASYSTORAGE:
                         case OSTREAM:
                         case SGCLASS_POINTER:
                         case SKIP_TYPE:
                           break;
                         case TO_HANDLE:

                         default:
                            std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                            std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                            assert (!"Stop immediately, since variable to build is not found ... " ) ;
                           break;
                      }
                    if ( addString != "" )
                       {
                           if ( s.find(addString) == std::string::npos )
                              {
                                 s += addString;
                              }
                       }
                  }
             }
        }
     return s;
   }

//#########################################################################################################

/* JH (10/28/2005) : This method creates the source code for the IR node constructor, that has as 
   its corresponding StorageClass as parameter! Since we call the initialization for the parents, 
   we only need to handle the data members of the IR node itself!
*/
string AstNodeClass::buildSourceForIRNodeStorageClassConstructor ()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;

     string classNameString = this-> name;
     string s  ;
     if ( classNameString == "SgNode" || classNameString == "$GRAMMAR_PREFIX_Node" )
        {
          s += "     p_freepointer = AST_FileIO::IS_VALID_POINTER() ; \n";
        }
     s += "     assert ( p_freepointer == AST_FileIO::IS_VALID_POINTER() ) ; \n";
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          string varStorageNameString = "storageOf_" + varNameString;
          string sg_string ( "" );
          string modified_type ( "" );

          if (varNameString != "freepointer"  && varTypeString.substr(0,7) !=  "static " )
             {
               sg_string = varTypeString;
               switch (evaluateType(varTypeString) )
                  {
                    case SGCLASS_POINTER:
                      s += "     p_" + varNameString+" =  (" + varTypeString +
                           ")( AST_FILE_IO::getSgClassPointerFromGlobalIndex ( storageSource."+varStorageNameString+") );\n" ;
                      break;
                    case CHAR_POINTER:
                    case CONST_CHAR_POINTER:
                      s += "     p_" + varNameString + " = const_cast<" + varTypeString +" >( storageSource." + 
                           varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ) ;\n" ;
                      break;
                    case ROSE_HASH_MULTIMAP:
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                   // s += "     rose_hash::unordered_multimap<SgName, SgSymbol*, hash_Name, eqstr>::iterator it; \n " ;
                      // CH (4/8/2010): Use boost::unordered instead
                      //s += "#ifdef _MSCx_VER \n";
                      //s += "     rose_hash::unordered_multimap<SgName, SgSymbol*>::iterator it; \n " ;
                      //s += "#else \n";
                      //s += "     rose_hash::unordered_multimap<SgName, SgSymbol*, hash_Name, eqstr>::iterator it; \n " ;
                      s += "     rose_hash_multimap::iterator it; \n " ;
                      //s += "#endif \n";
                      s += "     for (it = p_" + varNameString + "->begin(); it != p_" + varNameString + "->end(); ++it)\n " ;
                      s += "        {\n";
                      s += "          it->second = (SgSymbol*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex( (unsigned long)(it->second) ) ); \n";
                      s += "        }\n";
                      break;

                 // DQ (8/19/2008): Added new case to support IR nodes for arbitrary graphs
                    case ROSE_GRAPH_HASH_MULTIMAP:
#if 0
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                      s += "     rose_hash::unordered_multimap<std::string, SgGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n " ;
                      s += "     for (it = p_" + varNameString + "->begin(); it != p_" + varNameString + "->end(); ++it)\n " ;
                      s += "        {\n";
                      s += "          it->second = (SgGraphNode*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex( (unsigned long)(it->second) ) ); \n";
                      s += "        }\n";
#else
                   // DQ (4/25/2009): Removing pointer to type for data member to improve the interface...
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                   // s += "     rose_hash::unordered_multimap<std::string, SgGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n " ;
                      // CH (4/8/2010): Use boost::unordered instead
                      //s += "#ifdef _MSCx_VER \n";
                      //s += "     rose_hash::unordered_multimap<std::string, SgGraphNode*>::iterator it; \n " ;
                      //s += "#else \n";
                      //s += "     rose_hash::unordered_multimap<std::string, SgGraphNode*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n " ;
                      s += "     rose_graph_hash_multimap::iterator it; \n " ;
                      //s += "#endif \n";
                      s += "     for (it = p_" + varNameString + ".begin(); it != p_" + varNameString + ".end(); ++it)\n " ;
                      s += "        {\n";
                      s += "          it->second = (SgGraphNode*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex( (unsigned long)(it->second) ) ); \n";
                      s += "        }\n";
#endif
                      break;

                 // DQ (5/2/2009): This is not used!
                    case ROSE_GRAPH_DIRECTED_EDGE_HASH_MULTIMAP:
#if 0
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                      s += "     rose_hash::unordered_multimap<std::string, SgDirectedGraphEdge*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n " ;
                      s += "     for (it = p_" + varNameString + "->begin(); it != p_" + varNameString + "->end(); ++it)\n " ;
                      s += "        {\n";
                      s += "          it->second = (SgDirectedGraphEdge*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex( (unsigned long)(it->second) ) ); \n";
                      s += "        }\n";
#else
                   // DQ (4/25/2009): Removing pointer to type for data member to improve the interface...
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                   // s += "     rose_hash::unordered_multimap<std::string, SgDirectedGraphEdge*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n " ;
                      // CH (4/8/2010): Use boost::unordered instead
                      //s += "#ifdef _MSCx_VER \n";
                      //s += "     rose_hash::unordered_multimap<std::string, SgDirectedGraphEdge*>::iterator it; \n " ;
                      //s += "#else \n";
                      //s += "     rose_hash::unordered_multimap<std::string, SgDirectedGraphEdge*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n " ;
                      s += "     rose_graph_directed_edge_hash_multimap::iterator it; \n " ;
                      //s += "#endif \n";
                      s += "     for (it = p_" + varNameString + ".begin(); it != p_" + varNameString + ".end(); ++it)\n " ;
                      s += "        {\n";
                      s += "          it->second = (SgDirectedGraphEdge*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex( (unsigned long)(it->second) ) ); \n";
                      s += "        }\n";
#endif
                      break;

                 // DQ (5/2/2009): This is not used!
                    case ROSE_GRAPH_UNDIRECTED_EDGE_HASH_MULTIMAP:
#if 0
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                      s += "     rose_hash::unordered_multimap<std::string, SgUndirectedGraphEdge*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n " ;
                      s += "     for (it = p_" + varNameString + "->begin(); it != p_" + varNameString + "->end(); ++it)\n " ;
                      s += "        {\n";
                      s += "          it->second = (SgUndirectedGraphEdge*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex( (unsigned long)(it->second) ) ); \n";
                      s += "        }\n";
#else
                   // DQ (4/25/2009): Removing pointer to type for data member to improve the interface...
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                   // s += "     rose_hash::unordered_multimap<std::string, SgUndirectedGraphEdge*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n " ;
                      // CH (4/8/2010): Use boost::unordered instead
                      //s += "#ifdef _MSCx_VER \n";
                      //s += "     rose_hash::unordered_multimap<std::string, SgUndirectedGraphEdge*>::iterator it; \n " ;
                      //s += "#else \n";
                      //s += "     rose_hash::unordered_multimap<std::string, SgUndirectedGraphEdge*,rose_hash::hash_string,rose_hash::eqstr_string>::iterator it; \n " ;
                      s += "     rose_graph_undirected_edge_hash_multimap::iterator it; \n " ;
                      //s += "#endif \n";
                      s += "     for (it = p_" + varNameString + ".begin(); it != p_" + varNameString + ".end(); ++it)\n " ;
                      s += "        {\n";
                      s += "          it->second = (SgUndirectedGraphEdge*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex( (unsigned long)(it->second) ) ); \n";
                      s += "        }\n";
#endif
                      break;

                 // DQ (4/27/2009): Added case for new type
                    case ROSE_GRAPH_NODE_EDGE_HASH_MULTIMAP:
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                      s += "   { rose_graph_node_edge_hash_multimap::iterator it; \n " ;
                      s += "     for (it = p_" + varNameString + ".begin(); it != p_" + varNameString + ".end(); ++it)\n " ;
                      s += "        {\n";
                      s += "          it->second = (SgGraphEdge*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex( (unsigned long)(it->second) ) ); \n";
                      s += "        }\n";
                      s += "   }\n";
                      break;

                 // DQ (5/1/2009): Added support for rose_graph_string_integer_hash_multimap
                    case ROSE_GRAPH_STRING_INTEGER_HASH_MULTIMAP:
                      s += "   {\n";
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                      s += "     rose_graph_string_integer_hash_multimap::iterator string_integer_it; \n " ;
                      s += "     for (string_integer_it = p_" + varNameString + ".begin(); string_integer_it != p_" + varNameString + ".end(); ++string_integer_it)\n " ;
                      s += "        {\n";
                      s += "          string_integer_it->second = 0; printf (\"Unimplemented support for rose_graph_string_integer_hash_multimap: Fixme! \\n\"); ROSE_ASSERT(false); \n";
                      s += "        }\n";
                      s += "   }\n";
                      break;

                 // DQ (5/1/2009): Added support for rose_graph_integer_node_hash_map
                    case ROSE_GRAPH_INTEGER_NODE_HASH_MAP:
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                      s += "     rose_graph_integer_node_hash_map::iterator integer_node_it; \n " ;
                      s += "     for (integer_node_it = p_" + varNameString + ".begin(); integer_node_it != p_" + varNameString + ".end(); ++integer_node_it)\n " ;
                      s += "        {\n";
                      s += "          integer_node_it->second = (SgGraphNode*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex( (unsigned long)(integer_node_it->second) ) ); \n";
                      s += "        }\n";
                      break;

                 // DQ (5/1/2009): Added support for rose_graph_integer_edge_hash_map
                    case ROSE_GRAPH_INTEGER_EDGE_HASH_MAP:
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                      s += "     rose_graph_integer_edge_hash_map::iterator integer_edge_it; \n " ;
                      s += "     for (integer_edge_it = p_" + varNameString + ".begin(); integer_edge_it != p_" + varNameString + ".end(); ++integer_edge_it)\n " ;
                      s += "        {\n";
                      s += "          integer_edge_it->second = (SgGraphEdge*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex( (unsigned long)(integer_edge_it->second) ) ); \n";
                      s += "        }\n";
                      break;

                 // DQ (5/1/2009): Added support for rose_graph_integer_edge_hash_multimap
                    case ROSE_GRAPH_INTEGER_EDGE_HASH_MULTIMAP:
                      s += "   {\n";
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                      s += "     rose_graph_integer_edge_hash_multimap::iterator integer_edge_it2; \n " ;
                      s += "     for (integer_edge_it2 = p_" + varNameString + ".begin(); integer_edge_it2 != p_" + varNameString + ".end(); ++integer_edge_it2)\n " ;
                      s += "        {\n";
                      s += "          integer_edge_it2->second = (SgGraphEdge*)(AST_FILE_IO::getSgClassPointerFromGlobalIndex( (unsigned long)(integer_edge_it2->second) ) ); \n";
                      s += "        }\n";
                      s += "   }\n";
                      break;

                 // DQ (5/1/2009): Added support for rose_graph_integerpair_edge_hash_multimap
                    case ROSE_GRAPH_INTEGER_PAIR_EDGE_HASH_MULTIMAP:
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                      s += "     rose_graph_integerpair_edge_hash_multimap::iterator integerpair_edge_it; \n " ;
                      s += "     for (integerpair_edge_it = p_" + varNameString + ".begin(); integerpair_edge_it != p_" + varNameString + ".end(); ++integerpair_edge_it)\n " ;
                      s += "        {\n";
                      s += "          integerpair_edge_it->second = NULL; printf (\"Error: not implemented support for rose_graph_integerpair_edge_hash_multimap \\n\"); ROSE_ASSERT(false); \n";
                      s += "        }\n";
                      break;

                    case SGCLASS_POINTER_VECTOR:
                    case SGCLASS_POINTER_LIST:
                      sg_string += "::value_type";
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                      s += "     " + varTypeString + "::iterator i_" + varNameString + " = p_" + varNameString + ".begin() ; \n" ;
                      s += "     for ( ; i_" + varNameString + " != p_" + varNameString + ".end(); ++i_" + varNameString + " ) \n";
                      s += "        {\n";
                      s += "          (*i_" + varNameString + ") = (" + sg_string + ")(AST_FILE_IO::getSgClassPointerFromGlobalIndex ( (unsigned long)(*i_" + varNameString + ") ) );\n";
                      s += "        }\n";
                      break;

                    case SGCLASS_POINTER_LIST_POINTER:
                      sg_string = varTypeString.substr(0,varTypeString.size()-7) ;
                      s += "     p_" + varNameString + " = new " + varTypeString + " ( storageSource." + 
                           varStorageNameString + ".rebuildDataStoredInEasyStorageClass() );\n" ;
                      s += "     " + varTypeString + "::iterator i_" + varNameString + " = p_" + varNameString + "->begin() ; \n" ;
                      s += "     for ( ; i_" + varNameString + " != p_" + varNameString + "->end(); ++i_" + varNameString + " ) \n";
                      s += "        {\n";
                      s += "          (*i_" + varNameString + ") = (" + sg_string +"*)"\
                            "(AST_FILE_IO::getSgClassPointerFromGlobalIndex ( (unsigned long)(*i_" + varNameString + ") ) );\n";
                      s += "        }\n";
                      break;

                    case ASTATTRIBUTEMECHANISM:
                    case ATTACHEDPREPROCESSINGINFOTYPE:
                    case BIT_VECTOR:
                    case ROSEATTRUBUTESLISTCONTAINER:
                    case STL_CONTAINER:
                    case STL_SET:
                 // DQ (10/4/2006): Added case of STL_MAP
                    case STL_MAP:
                 // DQ (4/30/2009): Added case of STL_MULTIMAP
                    case STL_MULTIMAP:
                    case STRING:
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString + ".rebuildDataStoredInEasyStorageClass() ;\n" ;
                      break;
                    case MODIFIERCLASS:
                    case MODIFIERCLASS_WITHOUTEASYSTORAGE:
                    case SGNAME:
                      s += "     p_" + varNameString + " = " + varTypeString +  " ( storageSource." + varStorageNameString + " ) ;\n" ;
                      break;
                    case ENUM_TYPE:
                    case BASIC_DATA_TYPE:
                      s += "     p_" + varNameString + " = storageSource." + varStorageNameString +" ;\n";
                      break;
                    case OSTREAM:
                    case SKIP_TYPE:
                      break;

                    case TO_HANDLE:
                    default:
                       std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                       std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                       /* Does the type need to be added to one of the lists above?
                        * See AstNodeClass::evaluateType(std::string& varTypeString) */
                       assert (!"Stop immediately, since variable to build is not found ... " ) ;
                      break;
                  }
             }
        }
     return s;
   }
//#########################################################################################################
/* JH (10/28/2005) : build method for writing the static data members of the EasyStorage classes to disk.
   this looks up all members and build the call for the suitable ones!
*/
string AstNodeClass::buildStorageClassWriteStaticDataToFileSource ()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     string classNameString = this-> name;
     std::string s;
     std::string addString;
     for (AstNodeClass *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
          for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
             {
               addString = "";
               GrammarString *data = *stringListIterator;
               string varNameString = string(data->getVariableNameString());
               string varTypeString = string(data->getTypeNameString());
               if ( varTypeString.substr(0,7) !=  "static " )
                  {
                    switch (evaluateType(varTypeString) )
                       {
                         case ASTATTRIBUTEMECHANISM:
                         case ATTACHEDPREPROCESSINGINFOTYPE:
                         case CHAR_POINTER:
                         case CONST_CHAR_POINTER:
                         case STRING:
                         case ROSE_HASH_MULTIMAP:
                         case ROSE_GRAPH_HASH_MULTIMAP:
                         case ROSE_GRAPH_DIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_UNDIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_NODE_EDGE_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_integer_node_hash_map and rose_graph_integer_edge_hash_map
                         case ROSE_GRAPH_INTEGER_NODE_HASH_MAP:
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MAP:
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_string_integer_hash_multimap
                         case ROSE_GRAPH_STRING_INTEGER_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_integerpair_edge_hash_multimap
                         case ROSE_GRAPH_INTEGER_PAIR_EDGE_HASH_MULTIMAP:

                         case ROSEATTRUBUTESLISTCONTAINER:
                         case SGCLASS_POINTER_LIST:
                         case SGCLASS_POINTER_LIST_POINTER:
                         case SGCLASS_POINTER_VECTOR:
                         case STL_CONTAINER:
                         case STL_SET:
                         case STL_MAP:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case BIT_VECTOR:
                           addString += "     EasyStorage < " + varTypeString + " > :: writeToFile(out) ;\n" ;
                           break;
                         case MODIFIERCLASS:
                         case SGNAME:
                           addString += "     " + varTypeString + "StorageClass :: writeEasyStorageDataToFile(out) ;\n" ;
                           break;
                         case BASIC_DATA_TYPE:
                         case ENUM_TYPE:
                         case MODIFIERCLASS_WITHOUTEASYSTORAGE:
                         case OSTREAM:
                         case SGCLASS_POINTER:
                         case SKIP_TYPE:
                           break;
                         case TO_HANDLE:
                         default:
                            std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                            std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                            assert (!"Stop immediately, since variable to build is not found ... " ) ;
                           break;
                      }
                    if ( addString != "" )
                       {
                           if ( s.find(addString) == std::string::npos )
                              {
                                 s += addString;
                              }
                       }
                  }
             }
        }
     return s;
   }

//#########################################################################################################
/* JH (10/28/2005) : This methods builds the source for reading the static data members of the EasyStorage
   classes, caused by the StorageClasses to disk! 
*/
string AstNodeClass::buildStorageClassReadStaticDataFromFileSource()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     string classNameString = this-> name;
     std::string s;
     std::string addString;
     for (AstNodeClass *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
          for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
             {
               addString = "";
               GrammarString *data = *stringListIterator;
               string varNameString = string(data->getVariableNameString());
               string varTypeString = string(data->getTypeNameString());
               if ( varTypeString.substr(0,7) !=  "static " )
                  {
                    switch (evaluateType(varTypeString) )
                       {
                         case ASTATTRIBUTEMECHANISM:
                         case ATTACHEDPREPROCESSINGINFOTYPE:
                         case BIT_VECTOR:
                         case CHAR_POINTER:
                         case CONST_CHAR_POINTER:
                         case ROSEATTRUBUTESLISTCONTAINER:
                         case ROSE_HASH_MULTIMAP:
                         case ROSE_GRAPH_HASH_MULTIMAP:
                         case ROSE_GRAPH_DIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_UNDIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_NODE_EDGE_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_integer_node_hash_map and rose_graph_integer_edge_hash_map
                         case ROSE_GRAPH_INTEGER_NODE_HASH_MAP:
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MAP:
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_string_integer_hash_multimap
                         case ROSE_GRAPH_STRING_INTEGER_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_integerpair_edge_hash_multimap
                         case ROSE_GRAPH_INTEGER_PAIR_EDGE_HASH_MULTIMAP:

                         case SGCLASS_POINTER_LIST:
                         case SGCLASS_POINTER_LIST_POINTER:
                         case SGCLASS_POINTER_VECTOR:
                         case STL_CONTAINER:
                         case STL_SET:
                         case STL_MAP:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case STRING:
                           addString += "     EasyStorage < " + varTypeString + " > :: readFromFile(in) ;\n" ;
                           break;
                         case MODIFIERCLASS:
                         case SGNAME:
                           addString += "     " + varTypeString + "StorageClass :: readEasyStorageDataFromFile(in) ;\n" ;
                           break;
                         case BASIC_DATA_TYPE:
                         case ENUM_TYPE:
                         case MODIFIERCLASS_WITHOUTEASYSTORAGE:
                         case OSTREAM:
                         case SGCLASS_POINTER:
                         case SKIP_TYPE:
                           break;
                         case TO_HANDLE:
                         default:
                            std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                            std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                            assert (!"Stop immediately, since variable to build is not found ... " ) ;
                           break;
                      }
                    if ( addString != "" )
                       {
                           if ( s.find(addString) == std::string::npos )
                              {
                                 s += addString;
                              }
                       }
                  }
             }
        }
     return s;
   }

//#########################################################################################################
/* JH (10/28/2005) : Checking, wheather a teminal or nonterminal has data members, that will be stored in 
   in EasyStorage classes. 
   REMARK: This checking of the data members INCLUDES the checking of the data memebers of the parent! 
*/
bool AstNodeClass::hasMembersThatAreStoredInEasyStorageClass()
   {
     bool hasMembersThatWillBeStoredInEasyStorage = false;
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     for (AstNodeClass *t = this; t != NULL; t = t->getBaseClass())
        {
          copyList        = t->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
          for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
             {
               GrammarString *data = *stringListIterator;
               string varNameString = string(data->getVariableNameString());
               string varTypeString = string(data->getTypeNameString());
               if ( varTypeString.substr(0,7) !=  "static " )
                  {
                    switch (evaluateType(varTypeString) )
                       {
                         case ASTATTRIBUTEMECHANISM:
                         case ATTACHEDPREPROCESSINGINFOTYPE:
                         case BIT_VECTOR:
                         case CHAR_POINTER:
                         case CONST_CHAR_POINTER:
                         case MODIFIERCLASS:
                         case MODIFIERCLASS_WITHOUTEASYSTORAGE:
                         case ROSE_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_integer_node_hash_map and rose_graph_integer_edge_hash_map
                         case ROSE_GRAPH_INTEGER_NODE_HASH_MAP:
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MAP:
                         case ROSE_GRAPH_INTEGER_EDGE_HASH_MULTIMAP:

                      // DQ (8/19/2008): Added new case to support IR nodes for arbitrary graphs
                         case ROSE_GRAPH_HASH_MULTIMAP:

                         case ROSE_GRAPH_DIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_UNDIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_NODE_EDGE_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_string_integer_hash_multimap
                         case ROSE_GRAPH_STRING_INTEGER_HASH_MULTIMAP:

                      // DQ (5/1/2009): Added support for rose_graph_integerpair_edge_hash_multimap
                         case ROSE_GRAPH_INTEGER_PAIR_EDGE_HASH_MULTIMAP:

                         case ROSEATTRUBUTESLISTCONTAINER:
                         case SGCLASS_POINTER_LIST:
                         case SGCLASS_POINTER_LIST_POINTER:
                         case SGCLASS_POINTER_VECTOR:
                         case SGNAME:
                         case STL_CONTAINER:
                         case STL_SET:
                         case STL_MAP:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case STRING:
                           hasMembersThatWillBeStoredInEasyStorage = true;
                           break;
                         case SGCLASS_POINTER:
                         case ENUM_TYPE:
                         case BASIC_DATA_TYPE:
                         case OSTREAM:
                         case SKIP_TYPE:
                         case TO_HANDLE:
                           break;

                         default:
                           assert (!"ERROR");
                           break;
                      }
                 }
            }
       }
     return hasMembersThatWillBeStoredInEasyStorage;
   }

//#########################################################################################################
/* JH (10/28/2005) : Checking, wheather a teminal or nonterminal has static data members
*/
bool AstNodeClass::hasStaticMembers()
   {
     bool hasStaticDataMembers = false;
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          if ( varTypeString.substr(0,7) ==  "static " )
             {
               hasStaticDataMembers = true;
             }
       }
     return hasStaticDataMembers;
   }

//#########################################################################################################
/* JH (10/28/2005) : Checking, wheather a teminal or nonterminal has data members, that will be stored in 
   in EasyStorage classes. 
   REMARK: This checking of the data members INCLUDES the checking of the static ones. This is done, because 
   we want to call the storing and reading of the EasyStorage classes caused by the static data members not
   explicitly!
*/
std::string AstNodeClass::buildStaticDataMemberList()
   {
     std::string s;
     std::string classNameString = this->name;
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          if ( varTypeString.substr(0,7) ==  "static " )
             {
               varTypeString =  varTypeString.substr( 7,  varTypeString.size()-1 ) ;
               s += "     " + varTypeString + "  " + classNameString + "_" + varNameString + ";\n" ;
             }
        }
     return s;
   }

//#########################################################################################################
/* JH (04/04/2006) Method for generating the constructor source contributed by an IR node to the 
 * AstSpecificDataManagingClass
 */
std::string AstNodeClass::buildStaticDataMemberListConstructor()
   {
     std::string s;
     std::string classNameString = this->name;
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          if ( varTypeString.substr(0,7) ==  "static " )
             {
               varTypeString =  varTypeString.substr( 7,  varTypeString.size()-1 ) ;
               s += "     " + classNameString + "_" + varNameString + " = " + classNameString + "::p_" + varNameString + ";\n" ;
             }
        }
     return s;
   }

//#########################################################################################################
/* JH (04/05/2006) Method for generating the data member list within the AstSpecificDataManaginClass which
 * are added by an IR node. 
 */
std::string AstNodeClass::buildStaticDataMemberListSetStaticData()
   {
     std::string s;
     std::string classNameString = this->name;
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
      
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          if ( varTypeString.substr(0,7) ==  "static " )
             {
               varTypeString =  varTypeString.substr( 7,  varTypeString.size()-1 ) ;
               if (evaluateType(varTypeString) == SGCLASS_POINTER )
                  {
                    s += "     if ( " + classNameString + "_" + varNameString + " != NULL ) \n" ;
                    s += "        {\n" ;
                 // DQ (6/21/2010): Only reset the pointer to the static member (e.g. p_builtin_type) if it is NULL.
                 //                 This is important when reading multiple files from the binary AST file format.
                 // s += "           " + classNameString + "::p_" + varNameString + " = " + classNameString + "_" + varNameString + ";\n" ;
                 // s += " ;\n";
                    s += "          if (" + classNameString + "::p_" + varNameString + " == NULL)\n" ;
                    s += "             {\n" ;
                    s += "               " + classNameString + "::p_" + varNameString + " = " + classNameString + "_" + varNameString + ";\n" ;
                    s += "             }\n" ;
                    s += "        }\n" ;
                  }
               else 
                  {
                    s += "     " + classNameString + "::p_" + varNameString + " = " + classNameString + "_" + varNameString + ";\n" ;
                  }
             }
        }
     return s;
   }

//#########################################################################################################
/* JH (04/05/2006) Method for generating the data member list within the 
 * AstSpecificDataManagingClassStorageClass which are added by an IR node. 
 */
std::string AstNodeClass::buildStaticDataMemberListOfStorageClass()
   {
     std::string s;
     std::string classNameString = this->name;
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;

     copyList = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);

     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          if ( varTypeString.substr(0,7) ==  "static " )
             {
               string varStorageNameString =  "storageOf_" + classNameString + "_"  + varNameString;
               varTypeString = varTypeString.substr(7,varTypeString.size()-1 ) ;
            
               if (varNameString != "freepointer" )
                  {
                    switch (evaluateType(varTypeString) )
                       {
                         case SGCLASS_POINTER:
                           s += "     unsigned long " + varStorageNameString +";\n";
                           break;
                         case ASTATTRIBUTEMECHANISM:
                         case ATTACHEDPREPROCESSINGINFOTYPE:
                         case BIT_VECTOR:
                         case CHAR_POINTER:
                         case CONST_CHAR_POINTER:
                         case ROSE_HASH_MULTIMAP:
                         case ROSE_GRAPH_HASH_MULTIMAP:
                         case ROSE_GRAPH_DIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_UNDIRECTED_EDGE_HASH_MULTIMAP:
                         case ROSE_GRAPH_NODE_EDGE_HASH_MULTIMAP:

                         case ROSEATTRUBUTESLISTCONTAINER:
                         case SGCLASS_POINTER_LIST:
                         case SGCLASS_POINTER_LIST_POINTER:
                         case SGCLASS_POINTER_VECTOR:
                         case STL_CONTAINER:
                         case STL_MAP:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case STL_SET:
                         case STRING:
                           s += "     EasyStorage < " + varTypeString + " > " + varStorageNameString +";\n" ;
                           break;
                         case MODIFIERCLASS:
                         case MODIFIERCLASS_WITHOUTEASYSTORAGE:
                         case SGNAME:
                           s += "     " + varTypeString + "StorageClass " + varStorageNameString +";\n";
                           break;
                         case BASIC_DATA_TYPE:
                         case ENUM_TYPE:
                           s += "     " + varTypeString + " " + varStorageNameString +";\n";
                           break;
                         case OSTREAM:
                         case SKIP_TYPE:
                           break;

                         case TO_HANDLE:
                         default:
                           std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                           std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                           assert (!"Stop immediately, since variable to build is not found ... " ) ;
                           break;
                      }
                 }
             }
        }

     return s;
   }

//#########################################################################################################
/* JH (04/05/2006) Method for generating the access functions headers of the AstSpecificDataManagingClass, 
 * that are yielded by an IR node. 
 */
std::string AstNodeClass::buildAccessFunctionsForStaticDataMember()
   {
     std::string s;
     std::string classNameString = this->name;
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          if ( varTypeString.substr(0,7) ==  "static " )
             {
               varTypeString =  varTypeString.substr( 7,  varTypeString.size()-1 ) ;
               s += "     " + varTypeString + "  get_" + classNameString + "_" + varNameString + "() const;\n"; 
             }
        }
     return s;
   }

//#########################################################################################################
/* JH (04/05/2006) Method for generating the access functions source of the AstSpecificDataManagingClass, 
 * that are yielded by an IR node. 
 */
std::string AstNodeClass::buildAccessFunctionsForStaticDataMemberSource()
   {
     std::string s;
     std::string classNameString = this->name;
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          if ( varTypeString.substr(0,7) ==  "static " )
             {
               varTypeString =  varTypeString.substr( 7,  varTypeString.size()-1 ) ;
               s += varTypeString + "\n";
               s += "AstSpecificDataManagingClass::get_" + classNameString + "_" + varNameString + "() const\n";
               s += "  {\n";
               s += "    return " + classNameString+ "_" + varNameString + ";\n";
               s += "  }\n\n" ;
             }
        }
     return s;
   }

//#########################################################################################################
/* JH (10/28/2005) : The pickOutData generation of the AstSpecificDataMangaingClassStroageClass
 */
string AstNodeClass::buildSourceForStoringStaticMembers ()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     string classNameString = this-> name;
     string s;
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          string varStorageNameString =  "storageOf_" + classNameString + "_"  + varNameString;
          if (varTypeString.substr(0,7) ==  "static " )
             {
                varTypeString = varTypeString.substr(7,varTypeString.size()-1 ) ;
                std::string sg_string = varTypeString;
                switch (evaluateType(varTypeString) )
                  {
                    case SGCLASS_POINTER:
                      s += "     " + varStorageNameString + " = "\
                           "AST_FILE_IO::getGlobalIndexFromSgClassPointer ( source->" + classNameString + "_" + varNameString + " );\n" ;
                      break;
                    case CHAR_POINTER:
                      s += "     " + varStorageNameString + ".storeDataInEasyStorageClass( source->" + classNameString + "_" + varNameString + " );\n" ;
                      break;
                    case STRING:
                      s += "     " + varStorageNameString + ".storeDataInEasyStorageClass( source->" + classNameString + "_" + varNameString + " );\n" ;
                      break;
                    case SGCLASS_POINTER_LIST:
                           sg_string += "::value_type";
                           s += "     " + varTypeString + "::iterator i_" + classNameString + " = source->" + classNameString + "_" + varNameString +".begin() ; \n" ;
                           s += "     unsigned int tempListCount" + classNameString + " = 0; \n" ;
                           s += "     "+sg_string+" *  tempList" + classNameString + " = new "+sg_string+"[ source->" + classNameString + "_" + varNameString +".size() ]; \n" ;
                           s += "     for ( ; i_" + classNameString + " != source->" + classNameString + "_" + varNameString +".end(); ++i_" + classNameString + " ) \n";
                           s += "        {\n";
                           s += "          tempList" + classNameString + "[tempListCount" + classNameString + "] = *i_" + classNameString + ";\n";
                           s += "          tempListCount" + classNameString + "++; \n";
                           s += "          (*i_" + classNameString + ") = "\
                                "("+sg_string+")(AST_FILE_IO::getGlobalIndexFromSgClassPointer ( *i_"+classNameString+" ) );\n";
                           s += "        }\n";
                           s += "     " + varStorageNameString + ".storeDataInEasyStorageClass(source->" + classNameString + "_" + varNameString +");\n" ;
                           s += "     tempListCount" + classNameString + " = 0; \n" ;
                           s += "     i_" + classNameString + " = source->" + classNameString + "_" + varNameString +".begin() ; \n" ;
                           s += "     for ( ; i_" + classNameString + " != source->" + classNameString + "_" + varNameString +".end(); ++i_" + classNameString + " ) \n";
                           s += "        {\n";
                           s += "          *i_" + classNameString + " = tempList" + classNameString + "[tempListCount" + classNameString + "] ;\n";
                           s += "          tempListCount" + classNameString + "++; \n";
                           s += "        }\n";
                           s += "      delete [] tempList" + classNameString + "; \n";
                      break;

                 // DQ (7/12/2014): Adding support for STL_SET.
                    case STL_SET:
                 // DQ (4/30/2009): Added case of STL_MULTIMAP
                    case STL_MULTIMAP:
                    case STL_MAP:
                      s += "     " + varStorageNameString + ".storeDataInEasyStorageClass( source->" + classNameString + "_" + varNameString + ");\n" ;
                      break;
                    case BASIC_DATA_TYPE:
                      s += "     " + varStorageNameString +" =  source->" + classNameString + "_" + varNameString + " ;\n";
                      break;
                    default:
                       std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                       std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                       assert (!"Stop immediately, since variable to build is not found ... " ) ;
                      break;
                 }
            }
        }
     return s;
   }

//#########################################################################################################
/* JH (10/28/2005) : Building the contribute of an IR node to the constructor of 
 * AstSpecificDataManagingClass that takes its corresponding StorageClass as parameter. 
 */
string AstNodeClass::buildStaticDataConstructorSource ()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;

     string classNameString = this-> name;
     string s  ;
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          string varStorageNameString =  "source.storageOf_" + classNameString + "_"  + varNameString;
          if (varTypeString.substr(0,7) ==  "static " )
             {
                varTypeString = varTypeString.substr(7,varTypeString.size()-1 ) ;
                std::string sg_string = varTypeString;
                switch (evaluateType(varTypeString) )
                  {
                    case SGCLASS_POINTER:
                      s += "     " + classNameString + "_" + varNameString + " = "\
                           "(" +varTypeString + " ) (AST_FILE_IO::getSgClassPointerFromGlobalIndex (" + varStorageNameString + " ) );\n" ;
                      break;
                    case CHAR_POINTER:
                      s += "     " + classNameString + "_" + varNameString + " = " + varStorageNameString + ".rebuildDataStoredInEasyStorageClass();\n" ;
                      break;
                    case STRING:
                      s += "     " + classNameString + "_" + varNameString + " = " + varStorageNameString + ".rebuildDataStoredInEasyStorageClass();\n" ;
                      break;
                    case SGCLASS_POINTER_LIST:
                      sg_string += "::value_type";
                      s += "     " + classNameString + "_" + varNameString + " = " + varStorageNameString + ".rebuildDataStoredInEasyStorageClass();\n" ;
                      s += "     " + varTypeString + "::iterator i_" + classNameString + " = " + classNameString + "_" + varNameString + ".begin() ; \n" ;
                      s += "     for ( ; i_" + classNameString + " != " + classNameString + "_" + varNameString + ".end(); ++i_" + classNameString + " ) \n";
                      s += "        {\n";
                      s += "          (*i_" + classNameString + ") = "\
                           "(" + sg_string + ")(AST_FILE_IO::getSgClassPointerFromGlobalIndex ( (unsigned long) (*i_" + classNameString + " )  ) );\n";
                      s += "        }\n";
                      break;

                 // DQ (7/12/2014): Adding support for STL_SET.
                    case STL_SET:
                 // DQ (4/30/2009): Added case of STL_MULTIMAP
                    case STL_MULTIMAP:
                    case STL_MAP:
                      s += "     " + classNameString + "_" + varNameString + " = " + varStorageNameString + ".rebuildDataStoredInEasyStorageClass();\n" ;
                      break;
                    case BASIC_DATA_TYPE:
                      s += "     " + classNameString + "_" + varNameString + " = " + varStorageNameString + ";\n";
                      break;
                    default:
                       std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                       std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                       assert (!"Stop immediately, since variable to build is not found ... " ) ;
                      break;
                 }
            }
        }
     return s;
   }

//#########################################################################################################
/* JH (10/28/2005) : Writing the static data members of the AstSpecificDataManagingClassStorageClasses to 
 * file.
 */
string AstNodeClass::buildStaticDataWriteEasyStorageDataToFileSource()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     string classNameString = this-> name;
     string s  ;
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          std::string addString;
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          if ( varTypeString.substr(0,7) ==  "static " ) 
             {
               string varStorageNameString =  "storageOf_" + classNameString + "_"  + varNameString;
               varTypeString = varTypeString.substr(7,varTypeString.size()-1 ) ;
               if (varNameString != "freepointer" )
                  {
                    switch (evaluateType(varTypeString) )
                       {
                         case BASIC_DATA_TYPE:
                         case SGCLASS_POINTER:
                           break;
                         case CONST_CHAR_POINTER:
                         case CHAR_POINTER:
                         case STRING:
                           addString += "     EasyStorage<" + varTypeString + "> :: writeToFile(out) ;\n";
                           break;
                         case SGCLASS_POINTER_LIST:

                      // DQ (7/12/2014): Adding support for STL_SET.
                         case STL_SET:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case STL_MAP:
                           addString += "     EasyStorage<" + varTypeString + " > :: writeToFile(out);\n";
                           break;
                         default:
                           std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                           std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                           assert (!"Stop immediately, since variable to build is not found ... " ) ;
                           break;
                      }
                 }
               if ( addString != "" )
                  {
                      if ( s.find(addString) == std::string::npos )
                         {
                            s += addString;
                         }
                  }
            }
        }
     return s;
   }

//#########################################################################################################
/* JH (10/28/2005) : This methods builds the source for reading the static data members of an IR node 
   from disk and rebuilds the EasyStorage data of them! Do not mistake this method with the 
   buildStorageClassReadStaticDataFromFileSource, that soes the same for the StorageClasses!!!!!!!!!
*/
 
string AstNodeClass::buildStaticDataReadEasyStorageDataFromFileSource()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;

     string classNameString = this-> name;
     string s  ( "" );
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          if ( varTypeString.substr(0,7) ==  "static " ) 
             {
               std::string addString;
               string varStorageNameString =  "storageOf_" + classNameString + "_"  + varNameString;
                varTypeString = varTypeString.substr(7,varTypeString.size()-1 ) ;
                if (varNameString != "freepointer" )
                   {
                    switch (evaluateType(varTypeString) )
                       {
                         case BASIC_DATA_TYPE:
                         case SGCLASS_POINTER:
                           break;
                         case CONST_CHAR_POINTER:
                         case CHAR_POINTER:
                         case STRING:
                           addString += "     EasyStorage<" + varTypeString + "> :: readFromFile(in) ;\n";
                           break;
                         case SGCLASS_POINTER_LIST:

                      // DQ (7/12/2014): Adding support for STL_SET.
                         case STL_SET:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case STL_MAP:
                           addString += "     EasyStorage<" + varTypeString + " > :: readFromFile(in) ;\n";
                           break;

                         default:
                           std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                           std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                           assert (!"Stop immediately, since variable to build is not found ... " ) ;
                           break;
                      }
                 }
               if ( addString != "" )
                  {
                      if ( s.find(addString) == std::string::npos )
                         {
                            s += addString;
                         }
                  }
            }
        }
     return s;
   }

//#########################################################################################################
/* JH (10/28/2005) : This methods builds the source for reading the static data members of an IR node 
   from disk and rebuilds the EasyStorage data of them! Do not mistake this method with the 
   buildStorageClassReadStaticDataFromFileSource, that soes the same for the StorageClasses!!!!!!!!!
*/
string AstNodeClass::buildStaticDataArrangeEasyStorageInOnePoolSource()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     string classNameString = this-> name;
     string s;
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          if ( varTypeString.substr(0,7) ==  "static " ) 
             {
               std::string addString;
               string varStorageNameString =  "storageOf_" + classNameString + "_"  + varNameString;
                varTypeString = varTypeString.substr(7,varTypeString.size()-1 ) ;
                if (varNameString != "freepointer" )
                   {
                    switch (evaluateType(varTypeString) )
                       {
                         case SGCLASS_POINTER:
                         case BASIC_DATA_TYPE:
                           break;
                         case CONST_CHAR_POINTER:
                         case CHAR_POINTER:
                         case STRING:
                         case SGCLASS_POINTER_LIST:

                      // DQ (7/12/2014): Adding support for STL_SET.
                         case STL_SET:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case STL_MAP:
                           addString += "     EasyStorage<" + varTypeString + " > :: arrangeMemoryPoolInOneBlock();\n";
                           break;
                         default:
                           std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                           std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                           assert (!"Stop immediately, since variable to build is not found ... " ) ;
                           break;
                      }
                 }
               if ( addString != "" )
                  {
                      if ( s.find(addString) == std::string::npos )
                         {
                            s += addString;
                         }
                  }
            }
        }
     return s;
   }

//#########################################################################################################
/* JH (10/28/2005) : This methods builds the source for deleting the EasyStorage memory pools
   for the static data members of the IR nodes! Do not mistake this method with the 
   buildStorageClassReadStaticDataFromFileSource, that does the same for the StorageClasses!!!!!!!!!
*/
string AstNodeClass::buildStaticDataDeleteEasyStorageMemoryPoolSource()
   {
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     string classNameString = this-> name;
     string s;
     copyList        = this->getMemberDataPrototypeList(AstNodeClass::LOCAL_LIST,AstNodeClass::INCLUDE_LIST);
     for ( stringListIterator = copyList.begin(); stringListIterator != copyList.end(); stringListIterator++ )
        {
          GrammarString *data = *stringListIterator;
          string varNameString = string(data->getVariableNameString());
          string varTypeString = string(data->getTypeNameString());
          if ( varTypeString.substr(0,7) ==  "static " ) 
             {
               std::string addString;
               string varStorageNameString =  "storageOf_" + classNameString + "_"  + varNameString;
                varTypeString = varTypeString.substr(7,varTypeString.size()-1 ) ;
                if (varNameString != "freepointer" )
                   {
                    switch (evaluateType(varTypeString) )
                       {
                         case SGCLASS_POINTER:
                         case BASIC_DATA_TYPE:
                           break;
                         case CONST_CHAR_POINTER:
                         case CHAR_POINTER:
                         case STRING:
                         case SGCLASS_POINTER_LIST:

                      // DQ (7/12/2014): Adding support for STL_SET.
                         case STL_SET:
                      // DQ (4/30/2009): Added case of STL_MULTIMAP
                         case STL_MULTIMAP:
                         case STL_MAP:
                           addString += "     EasyStorage<" + varTypeString + " > :: deleteMemoryPool();\n";
                           break;

                         default:
                           std::cout << " There is a class not handled in buildStorageClasses.C, Line " << __LINE__ << endl ;
                           std::cout << "In class " + classNameString + " caused by variable " + varTypeString + " p_" + varNameString << endl ;
                           assert (!"Stop immediately, since variable to build is not found ... " ) ;
                           break;
                      }
                 }
            /* We only want to add the deletion of the memory pool, if this is not 
             * already in the string.
             */
               if ( addString != "" )
                  {
                      if ( s.find(addString) == std::string::npos )
                         {
                            s += addString;
                         }
                  }
            }
        }
     return s;
   }
//#########################################################################################################
