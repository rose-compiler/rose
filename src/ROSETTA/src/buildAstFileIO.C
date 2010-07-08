// ################################################################
// #                           Header Files                       #
// ################################################################

//#include "sage3basic.h"
#include "ROSETTA_macros.h"
#include "grammar.h"
#include "terminal.h"
#include "grammarString.h"
#include <sstream>
#include <algorithm>

using namespace std;

// ################################################################
// #                   Grammar Member Functions                   #
// ################################################################

//#############################################################################
/* JH(03/30/2006) Method for generating the code of the AST_FILE_IO.h
 * and AST_FILE_IO.C files. Within the generation the suitable building
 * functions are called!
 */
void 
Grammar::generateAST_FILE_IOFiles()
   {
  /* JH(10/26/2005): Build header for the class AST_FILE_IO providing 
   * the file I/O and corresponding pointer manipulation methods. All
   * methods and variables are static, since the memory pools, we the
   * class has to work on are static, as well.
   */
     fstream AST_FILE_IO_HeaderFile ( std::string(target_directory+"/AST_FILE_IO.h").c_str(),ios::out) ;
     std::cout << "Building AST_FILE_IO.h" << std::flush;
     string header_AST_FILE_IO_CLASS = build_header_AST_FILE_IO_CLASS () ;
     AST_FILE_IO_HeaderFile << header_AST_FILE_IO_CLASS ;
     AST_FILE_IO_HeaderFile.close();
     std::cout << " ... done " << std::endl;
  /* JH(10/25/2005): Build source for the file I/O class AST_FILE_IO.
   * Hereby, the static member functions are defined and the static 
   * data becomes allocated. 
   */
     fstream AST_FILE_IO_SourceFile ( std::string(target_directory+"/AST_FILE_IO.C").c_str(),ios::out) ;
     std::cout << "Building AST_FILE_IO.C" << std::flush;
     string source_AST_FILE_IO_CLASS = build_source_AST_FILE_IO_CLASS () ;
     AST_FILE_IO_SourceFile << source_AST_FILE_IO_CLASS ;
     AST_FILE_IO_SourceFile.close();
     std::cout << " ... done " << std::endl;
     return;
   }

//#############################################################################
/* JH (04/05/2006) Building the source of the method deleteStaticData within
 * the class AST_FILE_IO, that resets all static data of the memory pools to 
 * 0, NULL and/or deletes the data and lists behind.
 */
std::string Terminal::buildStaticDataMemberListDeleteStaticData()
   {
     std::string s;
     std::string classNameString = this->name;
     vector<GrammarString *> copyList;
     vector<GrammarString *>::const_iterator stringListIterator;
     copyList        = this->getMemberDataPrototypeList(Terminal::LOCAL_LIST,Terminal::INCLUDE_LIST);
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
                         case CHAR_POINTER:
                         case CONST_CHAR_POINTER:
                         case SGCLASS_POINTER:
                           s += "     " + classNameString + "::p_" + varNameString + " = NULL ;\n" ;
                           break;
                         case SGCLASS_POINTER_LIST_POINTER:
                           s += "     " + classNameString + "::p_" + varNameString + "->clear();\n" ;
                           s += "     delete " + classNameString + "::p_" + varNameString + ";\n" ;
                           break;
                         case STL_MAP:
                           s += "     " + classNameString + "::p_" + varNameString + ".clear();\n" ;
                           break;
                         case BASIC_DATA_TYPE:
                           s += "     " + classNameString + "::p_" + varNameString + " = ("+varTypeString+")(0);\n" ;
                           break;
                         default:
                           break;
                      }
                 }
             }
        }
     return s;
   }

//#############################################################################
/* JH (11/24/2005): This method generates the code produced for every IR node
 * for its static data by the method above!
 */
std::string
Grammar::buildStaticDataMemberListDeleteStaticDataSource(Terminal & node)
   {
     std::string classMembers = node.buildStaticDataMemberListDeleteStaticData();
     string temp = classMembers;
     classMembers = GrammarString::copyEdit(temp, "$CLASSNAME",  node.name);

     vector<Terminal *>::const_iterator treeListIterator;
     for( treeListIterator = node.subclasses.begin();
          treeListIterator != node.subclasses.end();
          treeListIterator++ )
       {
         ROSE_ASSERT ((*treeListIterator) != NULL);
         ROSE_ASSERT ((*treeListIterator)->getBaseClass() != NULL);
         classMembers += buildStaticDataMemberListDeleteStaticDataSource(**treeListIterator);
        }
      return classMembers;
   }

//#############################################################################
/* JH (02/02/2006) List containing the IR node types that are just non
 * teminals, means they serve as abstract classes. The memory pools of
 * those IR nodes will be always empty, since there will never be a pure 
 * object of those classes, only of derived classes. Therefore, we keep
 * this list, in order to skip the handling of these memory pools!  
 */
std::vector<std::string>& 
Grammar::getListOfAbstractClasses()
   {
  /* JH (03/30/2006) The class types are managed in a static STL list, 
   * containing the class names of the abstract classes as string.
   */
     static std::vector <std::string> listOfAbstractClasses;
     if ( listOfAbstractClasses.empty() )
        {
          listOfAbstractClasses.push_back ("SgSymbol");
        }
     return listOfAbstractClasses;
   }

//#############################################################################
/* JH (10/25/2005): This covers the forward decalration of the 
 * StorageClasses placed in the beginning of the file "Cxx_Grammar.h".
*/
string 
Grammar::buildStorageClassDeclarations()
   { 
     std::string nodeNameString;
     std::string declareStorageClasses;
     declareStorageClasses += "// JH (01/18/2006) Forward declarations for the StorageClasses,"\
                              " needed by the ast file i/o\n\n";
     for ( unsigned int i = 0 ; i < terminalList.size() ; ++i )
        {
          nodeNameString = terminalList[i]->name;
          declareStorageClasses += "  class " + nodeNameString + "StorageClass; \n";
        }
     declareStorageClasses += "\n  template <class A>\n  class EasyStorage;";
     declareStorageClasses += "\n  class AstSpecificDataManagingClass;\n";
     declareStorageClasses += "\n// ***** end of storage classes declarations **********\n\n";
     return declareStorageClasses;
   }

//#############################################################################
/* JH (10/25/2005): Method that builds the AST_FILE_IO.h contents. However, 
 * since the most of this header file is not IR node-dependent, we just 
 * replace the parts that correspond with the IR nodes, i.e. the total number
 * of IR nodes occurring within the grammer. 
 */
string 
Grammar::build_header_AST_FILE_IO_CLASS()
   {
  // DQ (2/27/2010): this file needs to be generated so that the "$PLACE_ENUM_SIZE" can be replaced.
  // There might be an alternative way in the future to get just this numerical value
  // (e.g. using the enum values: Cxx_Grammar_LAST_TAG or V_SgNumVariants).
     StringUtility::FileWithLineNumbers readFromFile = Grammar::readFileWithPos("../Grammar/grammarAST_FileIoHeader.code");

     ROSE_ASSERT (!this->astVariantToNodeMap.empty());
     size_t maxVariant = this->astVariantToNodeMap.rbegin()->first;
     std::string replacement = "enum { totalNumberOfIRNodes = " + StringUtility::numberToString ( maxVariant + 1 ) + "}; ";
    
     string returnString = StringUtility::toString(StringUtility::copyEdit(readFromFile,"$PLACE_ENUM_SIZE", replacement )); 
     return returnString;
   }

//#############################################################################
/* JH (03/30/2006) Method for generating the coding of the file 
 * "AST_FILE_IO.C", using the following working mechansim:
 *   1. Get the list of the abstract classes those that should not
 *      be addressed by any further iteration over the IR node (non)
 *      terminals.
 *   2. Read the file "../Grammar/grammarAST_FileIoSource.code" that 
 *      contains the IR node indepentend parts of the methods to be 
 *      generated.
 *   3. Generate the IR node dependent parts of the methods to be 
 *      built and replace all markers of the read code file.
 */
string  
Grammar::build_source_AST_FILE_IO_CLASS()
   { 
  /* JH (03/30/2006) get the list of the abstract classes and 
   * name the corresponding iterators, to have a nicer use within
   * the following code generation!
   */
     std::vector<std::string> listOfAbstractClasses = getListOfAbstractClasses();
     assert ( listOfAbstractClasses.empty() == false );
     std::vector<std::string>::const_iterator abstractClassesListStart = listOfAbstractClasses.begin() ;
     std::vector<std::string>::const_iterator abstractClassesListEnd = listOfAbstractClasses.end();

  /* JH (03/30/2006) Some working strings. nodeNameString denotes the name 
   * of the IR node, nbr holds the string cast of an integer.
   */
     std::string nodeNameString ;
     std::string nbr ;

  /* JH (03/30/2006) Read the code file "../Grammar/grammarAST_FileIoSource.code"
   * that contains all the IR node independent parts of the methods to be 
   * generated for the AST_FILE_IO class.
   */
     StringUtility::FileWithLineNumbers generatedCode = readFileWithPos("../Grammar/grammarAST_FileIoSource.code");

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /* JH (03/30/2006) Generating the code for the startUp method. In 
   * this method we generate the contents of the listOfMemoryPoolSizes. 
   * This list contains the the accumulated pool sizes. Therefore, the 
   * method getNumberOfValidNodesAndSetGlobalIndexInFreepointer counts 
   * every valid content to globalIndexCounter and returns the result. 
   * Additionally, this methods stores the corresponding global index 
   * of a valid object in its p_freepointer. The freepointer of the 
   * invalid objects is set to NULL. Thus, this counting of the pool
   * contents is not side-effect free, but we spare an additional looping
   * over the memory pools. 
   * In order to be complete, we build the code even for the abstract 
   * classes. However, the getNumberOfValidNodesAndSetGlobalIndexInFreepointer
   * will always return a not increaded globalIndexCounter. This serves us 
   * to receive a complete list of the acumulated pool contents, where we 
   * always can compute the size of the acutal pool by subtracting the 
   * i-th-1 listOfMemoryPool entry from the i-th entry.
   */
     std::string startUp;

     set<string> presentNames;
     for ( unsigned int i = 0 ; i < terminalList.size() ; ++i ) {
       presentNames.insert(terminalList[i]->name);
     }

     startUp += "     listOfMemoryPoolSizes [ 0 ] =  globalIndexCounter;\n"  ;

     ROSE_ASSERT (!this->astVariantToNodeMap.empty());
     size_t maxVariant = this->astVariantToNodeMap.rbegin()->first;
     for (size_t i = 0; i <= maxVariant; ++i) {
       map<size_t, string>::const_iterator it = this->astVariantToNodeMap.find(i);
       if (it != this->astVariantToNodeMap.end()) {
          nodeNameString = it->second;
          if (presentNames.find(nodeNameString) != presentNames.end()) {
            startUp +=  "     globalIndexCounter = " + nodeNameString + 
                        "_getNumberOfValidNodesAndSetGlobalIndexInFreepointer ( globalIndexCounter ) ; \n" ;
          }
       }
       startUp +=  "     listOfMemoryPoolSizes [ " +  StringUtility::numberToString ( i + 1) + " ] "\
                    "=  globalIndexCounter; \n"  ;
     }
     generatedCode = GrammarString::copyEdit(generatedCode,"$REPLACE_STARTUP", startUp.c_str() );
  
  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /* JH (03/30/2006) Building the IR node dependent source of compressAst. First, 
   * a object is generated in order to hold the static AST data. Second, we iterate
   * over the memory pools, store the data into the corresponding StorageClasses. 
   * Thereby the abstract classes are omitted. 
   * Third, we clear all the memory pools, but do not free the memory, since we will
   * reuse this allocated memory again.
   * Forth, we build a ASTSpecificDataManagingClass object and set the static data
   * Last, we iterate over the memory pools and reconstruct the data contained within
   * the StorageClasses.
   */
     std::string compressAst;
   /* 1. Generate a AstSpecificDataManagingClassStorageClass object, in order to hold
    *    the static data of an AST.
    */
     compressAst += "     AstSpecificDataManagingClassStorageClass staticTemp; \n" ;
     compressAst += "     staticTemp.pickOutIRNodeData(actualRebuildAst); \n" ;

   /* 2. Iterate over the memory pools and store the data into the corresponding 
    *    StorageClasses. Thereby, the abstract classes are omitted!
    */
     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
          nodeNameString = i->second  ;
          if (presentNames.find(nodeNameString) == presentNames.end())
            continue;
          if ( find (abstractClassesListStart,abstractClassesListEnd,nodeNameString) == abstractClassesListEnd )
             {
               compressAst += "     sizeOfActualPool = getSizeOfMemoryPool (V_" + nodeNameString + " ) ; \n" ;
               compressAst += "     std::cout << \" " + nodeNameString + " has size \" << sizeOfActualPool << std::endl;\n" ;
               compressAst += "     " + nodeNameString + "StorageClass* " + nodeNameString + "StorageArray = NULL; \n" ;
               compressAst += "     if ( 0 < sizeOfActualPool ) \n" ;
               compressAst += "        {  \n" ;
               compressAst += "          " + nodeNameString + "StorageArray = "\
                              "new " + nodeNameString + "StorageClass[sizeOfActualPool] ; \n" ;
               compressAst += "          unsigned long storageClassIndex = " + nodeNameString + 
                              "_initializeStorageClassArray (" + nodeNameString + "StorageArray); ;\n" ;
               compressAst += "          assert ( storageClassIndex == sizeOfActualPool ); \n" ;
               compressAst += "        }  \n" ;
             }
        }
   /* 3. Clear all the memory pools!
    */
     compressAst += "     clearAllMemoryPools();\n" ;
   /* 4. Arrange the static data in on pool (EasyStorage) and initialize a 
    *    new AstSpecificMannagingClass with that data.
    */
     compressAst += "     AstSpecificDataManagingClassStorageClass :: arrangeStaticDataOfEasyStorageClassesInOneBlock (); \n" ;
     compressAst += "     actualRebuildAst = new AstSpecificDataManagingClass(staticTemp); \n" ;
     compressAst += "     actualRebuildAst->setStaticDataMembersOfIRNodes(); \n" ;
   /* 5. Iterate over the IR node types and store the data contained within the 
    *    StorageClassen into the memorypools.
    */
     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
          nodeNameString = i->second  ;
          if (presentNames.find(nodeNameString) == presentNames.end())
            continue;
          if ( find (abstractClassesListStart,abstractClassesListEnd,nodeNameString) == abstractClassesListEnd )
             {
               compressAst += "     sizeOfActualPool =  getPoolSizeOfNewAst( V_" + nodeNameString + " ) ;\n" ;
               compressAst += "     std::cout << \" " + nodeNameString + " has size \" << sizeOfActualPool << std::endl;\n" ;
               compressAst += "     if ( 0 < sizeOfActualPool )\n" ;
               compressAst += "        { \n" ;
               if (this->getTerminalForVariant(i->first).hasMembersThatAreStoredInEasyStorageClass() == true )
                  {
                    compressAst += "          " + nodeNameString + 
                                   "StorageClass :: arrangeStaticDataOfEasyStorageClassesInOneBlock () ; \n" ;
                  }
               compressAst += "          for ( unsigned int i = 0;  i < sizeOfActualPool; ++i )\n" ;
               compressAst += "              {\n" ;
               compressAst += "               new " + nodeNameString + " ( " + nodeNameString + "StorageArray[i] ) ; \n" ;
               compressAst += "              }\n" ;
               compressAst += "          delete [] " + nodeNameString + "StorageArray;\n" ;
               compressAst += "        } \n" ;
            }
        }
     generatedCode = GrammarString::copyEdit(generatedCode,"$REPLACE_COMPRESSASTINMEMEORYPOOL", compressAst.c_str() );

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /* JH (04/01/2006) Generating code for resetValidAstAfterWriting. Since the
   * p_freepointers are set to the global id for valid entries and NULL for 
   * invalid IR nodes within the memory pools, we provide this method, in order
   * to reset the p_freepointers to theit original value again. 
   * However, this method dies acually not work ...
   */
     std::string resetValidAstAfterWriting;

     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
          nodeNameString = i->second  ;
          if (presentNames.find(nodeNameString) == presentNames.end()) continue;
          resetValidAstAfterWriting += "     " + nodeNameString + "_resetValidFreepointers( );\n" ;
        }
     generatedCode = GrammarString::copyEdit(generatedCode,"$REPLACE_RESETVALIDASTAFTERWRITING",resetValidAstAfterWriting.c_str() );

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /* JH (04/01/2006) Part for generating the code for printListOfPoolSizes. 
   * The produced method does print a list of the actual memory pool sizes.
   */
     std::string printListOfPoolSizes;
     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
          nodeNameString = i->second  ;
          if (presentNames.find(nodeNameString) == presentNames.end()) continue;
          printListOfPoolSizes += "     std::cout << \"Memory pool size of " + nodeNameString + ": \"" ;
          printListOfPoolSizes += " <<  listOfMemoryPoolSizes[V_" + nodeNameString + "] << std::endl; \n";
        }
     generatedCode = GrammarString::copyEdit(generatedCode,"$REPLACE_PRINTLISTOFPOOLSIZES",printListOfPoolSizes.c_str() );

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /* JH (04/01/2006) Part for generating the code for printListOfPoolSizes. 
   * The produced method does print a list of the actual memory pool sizes.
   */
     std::string printListOfPoolSizesOfAst;
     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
          nodeNameString = i->second  ;
          if (presentNames.find(nodeNameString) == presentNames.end()) continue;
          printListOfPoolSizesOfAst += "     std::cout << \"Memory pool size of " + nodeNameString + ": \"" ;
          printListOfPoolSizesOfAst += " <<  vectorOfASTs[index]->getMemoryPoolSize(V_"+nodeNameString+") << std::endl; \n";
        }
     generatedCode = GrammarString::copyEdit(generatedCode,"$REPLACE_PRINTSPECIFICLISTOFPOOLSIZESOFAST",printListOfPoolSizesOfAst.c_str() );

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /* JH (04/01/2006) Generating getSgClassPointerFromGlobalIndex. This method
   * returns the memory address within the suitable memory pool and 
   * corresponding to the global index.
   */
     std::string getSgClassPointerFromGlobalIndex;
     getSgClassPointerFromGlobalIndex += "    if ( 0 < globalIndex )\n";
     getSgClassPointerFromGlobalIndex += "       { \n";
     getSgClassPointerFromGlobalIndex += "         unsigned long sgVariantOfIRNodeTypeOfIndex = binarySearch ( globalIndex );\n";
     getSgClassPointerFromGlobalIndex += "#if FILE_IO_EXTRA_CHECK\n";
     getSgClassPointerFromGlobalIndex += "         // Check, wheather the index is really within the range of this memory pool\n";
     getSgClassPointerFromGlobalIndex += "         assert ( getAccumulatedPoolSizeOfNewAst(sgVariantOfIRNodeTypeOfIndex) <= globalIndex );\n";
     getSgClassPointerFromGlobalIndex += "         assert ( globalIndex < AST_FILE_IO::getAccumulatedPoolSizeOfNewAst(sgVariantOfIRNodeTypeOfIndex+1) );\n";
     getSgClassPointerFromGlobalIndex += "#endif\n";
     getSgClassPointerFromGlobalIndex += "         switch ( sgVariantOfIRNodeTypeOfIndex )\n";
     getSgClassPointerFromGlobalIndex += "            {\n";
     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
          nodeNameString = i->second  ;
          if (presentNames.find(nodeNameString) == presentNames.end()) continue;
          getSgClassPointerFromGlobalIndex += "             case V_" + nodeNameString + ": \n " ;
          getSgClassPointerFromGlobalIndex += "                returnPointer =  " + nodeNameString + "_getPointerFromGlobalIndex( globalIndex )  ;\n";
          getSgClassPointerFromGlobalIndex += "                break ; \n" ;
        }
     getSgClassPointerFromGlobalIndex += "              default:\n";
     getSgClassPointerFromGlobalIndex += "                 assert ( !\" Index error in getPointerFromGlobalIndex !\" ) ;\n";
     getSgClassPointerFromGlobalIndex += "                 break ;\n";
     getSgClassPointerFromGlobalIndex += "            }\n";
     getSgClassPointerFromGlobalIndex += "#if FILE_IO_EXTRA_CHECK\n";
     getSgClassPointerFromGlobalIndex += "         assert ( returnPointer != NULL );\n" ;
     getSgClassPointerFromGlobalIndex += "#endif\n";
     getSgClassPointerFromGlobalIndex += "       }\n" ;
     generatedCode = GrammarString::copyEdit(generatedCode,"$REPLACE_GETSGPOINTER", getSgClassPointerFromGlobalIndex.c_str() );

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /* JH (04/01/2006) : Generating code for clearMemoryPools of the IR node 
   * specific parts. 
   */
     std::string clearMemoryPools;
     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
          nodeNameString = i->second  ;
          if (presentNames.find(nodeNameString) == presentNames.end()) continue;
          clearMemoryPools += "     " + nodeNameString + "_clearMemoryPool( );\n" ;
        }
     generatedCode = GrammarString::copyEdit(generatedCode,"$REPLACE_CLEARMEMORYPOOLS",clearMemoryPools.c_str() );

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /* JH (04/01/2006) : Generating code for deleteStaticData, the method that 
   * resets the static data of the IR nodes.
   */
     std::string deleteStaticData; 
     deleteStaticData += buildStaticDataMemberListDeleteStaticDataSource(*rootNode);
     generatedCode = GrammarString::copyEdit(generatedCode,"$REPLACE_DELETESTATICDATA",deleteStaticData.c_str() );

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  /* JH (04/01/2006) Generating code for extendMemoryPoolsForRebuildingAST
     * the suitable extendMemoryPoolForFileIO on every IR node type is called.
  */
     std::string extendMemoryPoolsForRebuildingAST;
     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
          nodeNameString = i->second  ;
          if (presentNames.find(nodeNameString) == presentNames.end()) continue;
          extendMemoryPoolsForRebuildingAST += "     " + nodeNameString + "_extendMemoryPoolForFileIO( );\n" ;
        }
     generatedCode = GrammarString::copyEdit(generatedCode,"$REPLACE_EXTENDMEMORYPOOLS",extendMemoryPoolsForRebuildingAST.c_str() );

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // JH (04/05/2006) generate code for writeASTToFile
     std::string writeASTToFile;
     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
          nodeNameString = i->second  ;
          if (presentNames.find(nodeNameString) == presentNames.end()) continue;
          if ( find (abstractClassesListStart,abstractClassesListEnd,nodeNameString) == abstractClassesListEnd )
             {
               writeASTToFile += "     sizeOfActualPool = getSizeOfMemoryPool(V_" + nodeNameString + " ); \n" ;
               writeASTToFile += "     storageClassIndex = 0 ;\n" ;
               writeASTToFile += "     if ( 0 < sizeOfActualPool ) \n" ;
               writeASTToFile += "        {  \n" ;
            // Initializing the StorageClasses 
               writeASTToFile += "          " + nodeNameString + "StorageClass* storageArray = "\
                                 "new " + nodeNameString + "StorageClass[sizeOfActualPool] ;\n" ;
               writeASTToFile += "           storageClassIndex = " + nodeNameString + "_initializeStorageClassArray (storageArray); ;\n" ;
               writeASTToFile += "           assert ( storageClassIndex == sizeOfActualPool ); \n" ;
             
            // Writing StorageClass array to disk
               writeASTToFile += "           out.write ( (char*) (storageArray) , sizeof ( " + nodeNameString + "StorageClass ) * sizeOfActualPool) ;\n" ;
            // delete array 
               writeASTToFile += "           delete [] storageArray;  \n" ;
            // Writing EasyStorage stuff 
               if (this->getTerminalForVariant(i->first).hasMembersThatAreStoredInEasyStorageClass() == true )
                  {
                    writeASTToFile += "           " + nodeNameString + "StorageClass :: writeEasyStorageDataToFile(out) ;\n" ;
                  }
               writeASTToFile += "        }  \n\n" ;
             }
        }
     generatedCode = GrammarString::copyEdit(generatedCode,"$REPLACE_WRITEASTTOFILE", writeASTToFile.c_str() );

  //+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
  // JH (04/05/2006) generate code for readASTFromFile
     std::string readASTFromFile;
     for (map<size_t, string>::const_iterator i = this->astVariantToNodeMap.begin(); i != this->astVariantToNodeMap.end(); ++i) {
          nodeNameString = i->second  ;
          if (presentNames.find(nodeNameString) == presentNames.end()) continue;
          if ( find (abstractClassesListStart,abstractClassesListEnd,nodeNameString) == abstractClassesListEnd )
             {
            // readASTFromFile += "     std::cout << \" reading " + nodeNameString + " \" << std::endl; \n" ;
               readASTFromFile += "     sizeOfActualPool = getPoolSizeOfNewAst(V_" + nodeNameString + " ); \n" ;
               readASTFromFile += "     storageClassIndex = 0 ;\n" ;
               readASTFromFile += "     " + nodeNameString + "StorageClass* storageArray" + nodeNameString + " = NULL;\n" ;
               readASTFromFile += "     if ( 0 < sizeOfActualPool ) \n" ;
               readASTFromFile += "        {  \n" ;
            // Reading StorageClass array
               readASTFromFile += "          storageArray" + nodeNameString + " = new " + nodeNameString + "StorageClass[sizeOfActualPool] ;\n" ;
               readASTFromFile += "          inFile.read ( (char*) (storageArray" + nodeNameString + ") , "\
                                                           "sizeof ( " + nodeNameString + "StorageClass ) * sizeOfActualPool) ;\n" ;
            // Reading EasyStorage stuff 
               if (this->getTerminalForVariant(i->first).hasMembersThatAreStoredInEasyStorageClass() == true )
                  {
                    readASTFromFile += "        " + nodeNameString + "StorageClass :: readEasyStorageDataFromFile(inFile) ;\n" ;
                  }
               readASTFromFile += "          " + nodeNameString + "StorageClass* storageArray = storageArray" + nodeNameString + ";\n" ;
               readASTFromFile += "          for ( unsigned int i = 0;  i < sizeOfActualPool; ++i )\n" ;
               readASTFromFile += "             {\n" ;
            // readASTFromFile += "               new " + nodeNameString + " ( *storageArray ) ; \n" ;
               readASTFromFile += "               " + nodeNameString + "* tmp = new " + nodeNameString + " ( *storageArray ) ; \n" ;
               readASTFromFile += "               ROSE_ASSERT(tmp->p_freepointer == AST_FileIO::IS_VALID_POINTER() ); \n" ;
               readASTFromFile += "               storageArray++ ; \n" ;
               readASTFromFile += "             }\n" ;
               readASTFromFile += "        }  \n" ;
            // delete array 
               readASTFromFile += "      delete [] storageArray" + nodeNameString + ";  \n" ;
            // delete EasyStorage stuff 
               if (this->getTerminalForVariant(i->first).hasMembersThatAreStoredInEasyStorageClass() == true )
                  {
                    readASTFromFile += "      " + nodeNameString + "StorageClass :: deleteStaticDataOfEasyStorageClasses();\n" ;
                  }
               readASTFromFile += "\n\n" ;
             }
        }
     generatedCode = GrammarString::copyEdit(generatedCode,"$REPLACE_READASTFROMFILE", readASTFromFile.c_str() );
     std::string returnCode = StringUtility::toString(generatedCode);

     return returnCode;
   }

