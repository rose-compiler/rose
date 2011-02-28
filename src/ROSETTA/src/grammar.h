// Grammar class definition to allow us to define grammars as input to ROSE

#ifndef __ROSETTA_GRAMMAR_H__
#define __ROSETTA_GRAMMAR_H__

// DQ (3/12/2006): We want to remove config.h from being placed in every source file
#include <rose_config.h>
#include "fileoffsetbits.h"


// DQ (3/22/2009): Added MSVS support for ROSE.
#include "rose_msvc.h"

#include "rose_paths.h"

#include <stdio.h>
#include <stdlib.h>

// We need to separate the construction of the code for ROSE from the ROSE code
// #include FSTREAM_HEADER_FILE
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#if !ROSE_MICROSOFT_OS
#include <dirent.h>
#include </usr/include/sys/file.h>
#include <unistd.h>
#endif
#include <assert.h>

#include "ROSETTA_macros.h"
#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>

#include "string_functions.h"

#define COMPLETERTI 1

// DQ (3/19/2007): Added the IR nodes for FORTRAN
// DQ (2/2/2006): Control use of Fortran specific IR nodes within ROSETTA
// We will trun this on later once more of the implementation is in place.
#define USE_FORTRAN_IR_NODES 1

// DQ and Liao (6/10/2008): Added IR node support for UPC.
#define USE_UPC_IR_NODES 1

//Liao 5/29/2009: Add IR node support for OpenMP
#define USE_OMP_IR_NODES 1

#include <vector>

// using namespace std;

class SgProject;

// Things to add:
//   1) not operator for nodes within a subtree
//   2) mechanism for adding code to the base of a tree to a specified depth
//      (for example, if the depth == 1 then code is added only to the leaves)
//   3) addition of code to lists of nodes (or subtrees)
//   4) more general editing mechanism for substitution of names into code 
//      inserted at each node.

// Bobby's list of things to do:
// 1. Capability to have some nodes in a grammar subtree be excluded from an operation
//         - eg some functions are overloaded in some but not all derived classes
// 2. Capability to have operations on all subtrees of a node, but exclude the node itself 
//    from performing the operation
//         - eg initialisation routines might be present only in derived classes
// 3. Generate get/set functions based on private data members
//         - eg obvious
// 4. Capability to perform operations on a list of nodes
//         - eg when a function is present in an arbitrary number of classes which have no tree structure
// 5. Ability to create and to replace arbitrary keywords rather than only CLASSNAME etc
//         - eg SgFunctionDeclarationStatement has function getFunctionDeclarationSymbol - would be 
//              desirable to replace FunctionDeclaration 

/*
   We are searching for a minimal set of substitution variable that would allow us
   to build all source code (of the type that would implement grammars).
   Current editing substitution variables:
       CLASSNAME
       BASECLASS
       BASE_CLASS_CONSTRUCTOR_CALL
       BASE_CLASS_CONSTRUCTOR_PARAMETER

 */

class grammarFile
   {
  // This object is used to cache files read in as part of the processing of a grammar

     private:
       std::string filename;
       StringUtility::FileWithLineNumbers buffer;

     public:
          grammarFile ( std::string inputFilename = "", const StringUtility::FileWithLineNumbers inputBuffer = StringUtility::FileWithLineNumbers() )
            {
              setFilename (inputFilename);
              setBuffer(inputBuffer);
            }

          grammarFile ( const grammarFile & X )
            {
              *this = X;
            }

          grammarFile & operator= ( const grammarFile & X )
            {
              setFilename (X.getFilename());
              setBuffer(X.getBuffer());
              return *this;
            }

         ~grammarFile ()
            {
            }

          void setFilename ( const std::string& inputFilename )
             {
               filename = inputFilename;
             }

          std::string getFilename() const
             {
               return filename;
             }

          void setBuffer ( const StringUtility::FileWithLineNumbers& inputBuffer )
             {
               buffer = inputBuffer;
             }

          const StringUtility::FileWithLineNumbers& getBuffer() const
             {
               return buffer;
             }
    };

class Terminal;
class SubclassListBuilder;
class GrammarString;

Terminal* lookupTerminal(const std::vector<Terminal*>& tl, const std::string& name);

class Grammar
   {
  // This class defines a language grammar and pemits it to be input in a form similar to BNF notation.
  // Later we will template this class to permit it to refer to an arbitrary lower level grammar.
     public:


       // Pointer to member function
       // typedef List<GrammarString> & (Terminal::*FunctionPointerType)(int i,int j) const;
          typedef std::vector<GrammarString *> & (Terminal::* FunctionPointerType)(int i,int j) const;

       // Pointer to member function
          typedef std::string (GrammarString::* StringGeneratorFunctionPointerType)() const;





     public:
          std::vector<Terminal*>        terminalList;
          std::map<std::string, size_t> astNodeToVariantMap;
          std::map<size_t, std::string> astVariantToNodeMap;
          std::map<size_t, Terminal*>   astVariantToTerminalMap;

          //The directory name we should generate the files to
          std::string target_directory;

          Terminal* rootNode;

       // This is a pointer to the lower level grammar (every grammar has a lower level grammar except the C++ grammar)
          const Grammar* parentGrammar;

       // filename of file containing declaration and source code for a grammar's supporting classes
          std::string filenameForSupportClasses;

       // filename of file containing declaration and source code for a grammar's supporting classes
          std::string filenameForGlobalDeclarations;

       // Support for output of constructors as part of generated documentation
          static std::string staticContructorPrototypeString;

       // List processing mechanisms
          void generateStringListsFromSubtreeLists ( Terminal & node,
                                                     std::vector<GrammarString *> & ,
                                                     std::vector<GrammarString *> & excludeList,
                                                     FunctionPointerType listFunction );

          void generateStringListsFromLocalLists ( Terminal & node,
                                                   std::vector<GrammarString *> & ,
                                                   std::vector<GrammarString *> & excludeList,
                                                   FunctionPointerType listFunction );

          void generateStringListsFromAllLists ( Terminal & node,
                                                 std::vector<GrammarString *> & includeList,
                                                 std::vector<GrammarString *> & excludeList,
                                                 FunctionPointerType listFunction );

          static void editStringList ( std::vector<GrammarString *>& origList, 
                                       const std::vector<GrammarString *> & excludeList );

          std::string buildStringFromLists ( Terminal & node,
                                       FunctionPointerType listFunction,
                                       StringGeneratorFunctionPointerType stringGeneratorFunction );

          std::vector<GrammarString *> buildListFromLists ( Terminal & node, FunctionPointerType listFunction );

          void setRootOfGrammar ( Terminal* RootNodeForGrammar );
          Terminal* getRootOfGrammar ();

         ~Grammar ();
     private:
          Grammar ();
     public:
          Grammar ( const Grammar & X );

       // Associative terminal/nonterminal access function
          Terminal    & getTerminal    ( const std::string& name ) const;

          void consistencyCheck() const;

       // builds the header files and source code
          void buildCode ();

          std::string getFilenameForGlobalDeclarations ();
          void  setFilenameForGlobalDeclarations ( const std::string& filename );

          StringUtility::FileWithLineNumbers buildMiscSupportDeclarations ();

       // Principle constructor
          Grammar ( const std::string& inputGrammarName, 
                    const std::string& inputPrefixName,
                    const std::string& inputGrammarNameBaseClass = "ROSE_BaseGrammar",
                    const Grammar* parentGrammar = NULL,
                    const std::string& t_directory = "");

          std::string grammarName;
          std::string grammarPrefixName;
          std::string grammarNameBaseClass;

       // Return the name of the grammar (two cases are handled since the
       // names are different for the "$GRAMMAR_PREFIX_" and the "$GRAMMAR_TAG_PREFIX_")
          const std::string& getGrammarName() const;
          const std::string& getGrammarPrefixName() const;
          std::string getGrammarTagName();

       // Build main class declaration for this grammar
          void buildGrammarClassDeclaration ( StringUtility::FileWithLineNumbers & outputFile );

          void addGrammarElement    ( Terminal    & X );

       // Check if the terminal or nonterminal (identified by a string) is a terminal or nonterminal
          bool isTerminal    ( const std::string& terminalName ) const;

       // This is the mechanism for adding new types to an existing grammar. This function
       // will more generally show how new terminals are added to an existing grammar.  It 
       // will be generalized later to the addition of nonterminals (implying the addition 
       // of subtrees of grammar being added to an existing grammar).  The later use of this 
       // will be important for the addition of the A++/P++ type system (and then Overture and Kull)
       //   -->  (realArray:doubleArray,floatArray; intArray, Indexing: Index,Range).
       // The SgProject contains the declarations associated with the terminal.  In the 
       // case where this this is a class, member functions of the class will define
       // additional terminals to be added to the grammar.
          void addNewTerminal   ( SgProject & project, Terminal    & X );

       // Get the variant for a node name
          size_t getVariantForNode(const std::string& name) const;
          size_t getVariantForTerminal(const Terminal& name) const;
       // Get the node name for a variant, asserting if not found
          std::string getNodeForVariant(size_t var) const;
          Terminal& getTerminalForVariant(size_t var);

     protected:
       // mechanism for internal use only
          void internalRemove ( const std::string& terminalName );

     public:
          void removeReferenceFromLists ( Terminal & X );

          void removeExclusionsFromTree();

          void traverseTreeToSetupAllNodeList ();
          void traverseTreeToSetupAllNodeList ( Terminal & node );

          std::string getDerivedClassDeclaration ( Terminal & node );

          StringUtility::FileWithLineNumbers buildHeaderStringAfterMarker  ( const std::string& marker, const std::string& fileName );
          StringUtility::FileWithLineNumbers buildHeaderStringBeforeMarker ( const std::string& marker, const std::string& fileName );

          std::string sourceCodeDirectoryName();

          static StringUtility::FileWithLineNumbers readFileWithPos ( const std::string& inputFileName );

       // DQ (12/28/2009): I don't think we want this to be static, since I want to call sourceCodeDirectoryName().
       // static void writeFile ( const StringUtility::FileWithLineNumbers& outputString, const std::string& directoryName, 
       //                         const std::string& className, const std::string& fileExtension );
          void writeFile  ( const StringUtility::FileWithLineNumbers& outputString, const std::string& directoryName, 
                            const std::string& className, const std::string& fileExtension );
       // DQ (12/31/2009): Added mechanism to append generated text to files.
          void appendFile ( const StringUtility::FileWithLineNumbers& outputString, const std::string& directoryName, 
                            const std::string& className, const std::string& fileExtension );

          std::string generateTraverseSuccessorForLoopSource(std::string typeString, 
                                                        std::string memberVariableName, 
                                                        std::string successorContainerName,
                                                        std::string successorContainerAccessOperator);
      // GB (8/1/2007)
          std::string generateNumberOfSuccessorsComputation(std::vector<GrammarString*>& traverseDataMemberList,
                               std::string successorContainerName);
          std::string generateTraverseSuccessor(GrammarString* gs, std::string successorContainerName);
          std::string generateTraverseSuccessorNamesForLoopSource(std::string typeString, 
                                                       std::string memberVariableName, 
                                                       std::string successorContainerName,
                                                             std::string successorContainerAccessOperator);
          std::string generateTraverseSuccessorNames(GrammarString* gs, std::string successorContainerName);

          std::string generateRTICode(GrammarString* gs, std::string successorContainerName, std::string className, size_t index);
          void buildRTIFile(Terminal* rootNode, StringUtility::FileWithLineNumbers& rttiFile);
          StringUtility::FileWithLineNumbers buildVariants ();
          StringUtility::FileWithLineNumbers buildForwardDeclarations ();

       // DQ (12/28/2009): Added to support optionally smaller (but more numerous header files for ROSE).
       // StringUtility::FileWithLineNumbers buildIncludesForSeparateHeaderFiles();
          void buildIncludesForSeparateHeaderFiles( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (10/26/2007): Add the protytype for the Cxx_GrammarTerminalNames
          void buildVariantsStringPrototype ( StringUtility::FileWithLineNumbers & outputFile );
          void buildVariantsStringDataBase ( StringUtility::FileWithLineNumbers & outputFile );

          void buildHeaderFiles ( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );

          std::string buildStringForPrototypes( Terminal & node );
          StringUtility::FileWithLineNumbers buildStringForDataDeclaration                     ( Terminal & node );
          StringUtility::FileWithLineNumbers buildStringForDataAccessFunctionDeclaration      ( Terminal & node );
          StringUtility::FileWithLineNumbers buildStringForSource                              ( Terminal & node );

       // Builds the "CLASSNAME::variant()" member function
          StringUtility::FileWithLineNumbers buildStringForVariantFunctionSource               ( Terminal & node );

       // Builds the "CLASSNAME::isCLASSNAME()" friend function
          StringUtility::FileWithLineNumbers supportForBuildStringForIsClassNameFunctionSource ( Terminal & node, const StringUtility::FileWithLineNumbers& accumulationString );
          StringUtility::FileWithLineNumbers buildStringForIsClassNameFunctionSource           ( Terminal & node );

       // DQ (9/21/2005): Added support for memory pools
          StringUtility::FileWithLineNumbers buildStringForNewAndDeleteOperatorSource          ( Terminal & node );

       // DQ (12/24/2005): Support for memory pool traversal
          StringUtility::FileWithLineNumbers buildStringForTraverseMemoryPoolSource            ( Terminal & node );

       // DQ & JH (1/17/2006): Added support for building code to check pointers to IR nodes
          StringUtility::FileWithLineNumbers buildStringForCheckingIfDataMembersAreInMemoryPoolSource ( Terminal & node );
          StringUtility::FileWithLineNumbers buildStringToTestPointerForContainmentInMemoryPoolSource ( Terminal & node );

       // AS (2/14/06): Added support for building code to return data member pointers to IR nodes
          StringUtility::FileWithLineNumbers buildStringForReturnDataMemberPointersSource ( Terminal & node );

          StringUtility::FileWithLineNumbers buildStringForProcessDataMemberReferenceToPointersSource ( Terminal & node );

       // DQ (3/7/2007): support for getChildIndex member function
          StringUtility::FileWithLineNumbers buildStringForGetChildIndexSource ( Terminal & node );

          bool buildConstructorParameterList ( Terminal & node, 
                                               std::vector<GrammarString *> & constructorParameterList,
                                               ConstructParamEnum config );
          std::string buildConstructorParameterListString ( Terminal & node, bool withInitializers, bool withTypes, ConstructParamEnum config, bool *complete = 0 );


       // DQ 11/6/2006): Support for building newer from of constructors (withouth source position information).
          void markNodeForConstructorWithoutSourcePositionInformation ( Terminal & node );
          void markNodeForConstructorWithoutSourcePositionInformationSupport( Terminal & node );
          StringUtility::FileWithLineNumbers buildConstructorWithoutSourcePositionInformation ( Terminal & node );
          void buildConstructorWithoutSourcePositionInformationSupport( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );
          void constructorLoopBody(const ConstructParamEnum& config, bool& complete, const StringUtility::FileWithLineNumbers& constructorSourceCodeTemplate, Terminal& node, StringUtility::FileWithLineNumbers& returnString);

       // DQ (11/7/2006): Get a specific node from the tree.
          Terminal* getNamedNode ( Terminal & node, const std::string & name );
          GrammarString* getNamedDataMember ( Terminal & node, const std::string & name );

       // DQ (3/24/2006): Separated these functions so that we could position the data member 
       // variable declaration at the base of the class and the generated access functions at 
       // the top.  This permist us to present the documentation better using Doxygen.
       // string buildDataPrototypesAndAccessFunctionPrototypesAndConstuctorPrototype ( GrammarTreeNode & node );
          StringUtility::FileWithLineNumbers buildDataMemberVariableDeclarations ( Terminal & node );
          StringUtility::FileWithLineNumbers buildMemberAccessFunctionPrototypesAndConstuctorPrototype ( Terminal & node );

          StringUtility::FileWithLineNumbers buildConstructor ( Terminal & node );

          StringUtility::FileWithLineNumbers buildCopyMemberFunctionSource ( Terminal & node );

          void buildSourceFiles ( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (12/23/2005): Added to support moving the new and elete operators to a 
       // different file (mostly for clarity to support users who read the source code).
          void buildNewAndDeleteOperators( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (12/24/2005): Support for memory pool traversal
          void buildTraverseMemoryPoolSupport( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ & JH (1/17/2006): Added support for building code to check pointers to IR nodes
          void buildStringForCheckingIfDataMembersAreInMemoryPoolSupport( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );

       // AS(2/14/2006): Added support for building code to return pointers to IR nodes
          void buildStringForReturnDataMemberPointersSupport ( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (4/30/2006): Support for buildStringForReturnDataMemberReferenceToPointersSource
       // JJW (11/1/2008): Changed to process rather than return the references
          void buildStringForProcessDataMemberReferenceToPointersSupport( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (12/23/2005): Relocated copy function to a separate file to imrove readability of source code
          void buildCopyMemberFunctions( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (3/7/2007): support for getChildIndex member function
          void buildStringForGetChildIndexSupport( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );

       // Uses list of targets and sources stored within each node to drive substitutions
          StringUtility::FileWithLineNumbers editSubstitution ( Terminal & node, const StringUtility::FileWithLineNumbers& editString );

       // This calls the functions to build the parsers (one for each child of the root node)
          void buildGrammarClassSourceCode ( StringUtility::FileWithLineNumbers & outputFile );

          static StringUtility::FileWithLineNumbers extractStringFromFile ( const std::string& startMarker, const std::string& endMarker,
                                                     const std::string& filename, const std::string& directory );

          void printTreeNodeNames ( const Terminal & node ) const;
       // void setUpParentNames ();
       // void setUpParentNames ( Terminal & X );

       // MK: The following member function is no longer needed:
       // void traverseTree ( Terminal & X );

       // These functions build terminal and nonterminal objects to be associated with this grammar
       // Using a member function to construct these serves several purposes:
       // 1) organizes terminals and nonterminals with there respective grammar (without ambiguity)
       // 2) avoids or deferes the implementation of the envelop/letter interface mechanism so
       //    that the letter will have a scope longer than the envelope
          Terminal & nonTerminalConstructor ( const std::string& lexeme, Grammar& X, const std::string& stringVar, const std::string& tagString, const SubclassListBuilder & builder, bool canHaveInstances = false );
          Terminal    & terminalConstructor ( const std::string& lexeme, Grammar & X, const std::string& stringVar = "", const std::string& tagString = "" );

       // These functions build the C++ grammar
          void setUpTypes ();
          void setUpSymbols ();
          void setUpExpressions ();
          void setUpStatements ();
          void setUpSupport ();
          void setUpNodes ();

       // DQ (3/15/2007): Added support for binaries
          void setUpBinaryInstructions ();

       // setup connection to parent grammar (part of building hierarchy of grammars)
          const Grammar* getParentGrammar ();
          void setParentGrammar ( const Grammar & GrammarPointer );

       // Functions that manage the hierarchy of grammars
          bool isRootGrammar ();

       // functions for building the parsers for translation between grammars
          std::string buildParserPrototype ( Terminal & node ); 
          std::string buildParserSource    ( Terminal & node ); 

       // a more general way to traverse the Grammar and build strings for source code generation

       // MS: Grammar Traversal defs
          class GrammarSynthesizedAttribute
             {
               public:
                    Terminal* grammarnode;
                    std::string text;
                    std::string nodetext;
                    std::string nonterminalsbunch;
                    std::string terminalsbunch;
                    std::string problematicnodes;
             };

          typedef std::string (Grammar::*evaluateStringAttributeFunctionType)(Terminal&, std::string);
          std::string naiveTraverseGrammar(Terminal&, evaluateStringAttributeFunctionType);
          typedef GrammarSynthesizedAttribute (Grammar::*evaluateGAttributeFunctionType)(Terminal*, 
                                                                                         std::vector<GrammarSynthesizedAttribute>);
          // MS: type hierarchy traversal
          GrammarSynthesizedAttribute BottomUpProcessing(Terminal* node, evaluateGAttributeFunctionType evaluateGAttributeFunction);
          // MS: build a Latex output that shows the abstract Cpp grammar
          GrammarSynthesizedAttribute CreateGrammarDotString(Terminal* grammarnode,
                                                               std::vector<GrammarSynthesizedAttribute> v);
          GrammarSynthesizedAttribute CreateGrammarLatexString(Terminal* grammarnode,
                                                               std::vector<GrammarSynthesizedAttribute> v);
          void buildGrammarDotFile(Terminal* rootNode, std::ostream& GrammarDotFile);
          void buildGrammarLatexFile(Terminal* rootNode, std::ostream& GrammarLatexFile);

          // MS: generate source for implementation of the RTI interface
          GrammarSynthesizedAttribute generateRTIImplementation(Terminal* grammarnode,
                                                                 std::vector<GrammarSynthesizedAttribute> v);

          // JJW (10/16/2008): Factor this code into a separate header file
          void buildStringForMemoryPoolSupport(Terminal* rootNode, StringUtility::FileWithLineNumbers& file);
          void buildStringForMemoryPoolSupportSource(Terminal* rootNode, StringUtility::FileWithLineNumbers& file);
          GrammarSynthesizedAttribute generateMemoryPoolSupportImplementation(Terminal* grammarnode, std::vector<GrammarSynthesizedAttribute> v);
          GrammarSynthesizedAttribute generateMemoryPoolSupportImplementationSource(Terminal* grammarnode, std::vector<GrammarSynthesizedAttribute> v);

          // MS: auxiliary function
          std::string typeStringOfGrammarString(GrammarString* gs);

          // temporary: used as return type by getGrammarNodeInfo
          class GrammarNodeInfo {
          public:
            GrammarNodeInfo():numContainerMembers(0), numSingleDataMembers(0) {}
            unsigned int numContainerMembers;
            unsigned int numSingleDataMembers;
          };

          // MS: auxiliary function to make tests on GrammarNodes more compact. (should be made a set of member
          //     functions of GrammarNode at some point (requires many other functions to be moved as well)
     GrammarNodeInfo getGrammarNodeInfo(Terminal* grammarnode);

     // MS: generates the code to implement the creation of the treeTraversalSuccessorContainer in Sage
     void buildTreeTraversalFunctions(Terminal & node, StringUtility::FileWithLineNumbers & outputFile);

     // MS: create source code for enums used in traversal functions to access synthesized attribute values
     void buildEnumForNode(Terminal& node, std::string& enumString);
     std::string EnumStringForNode(Terminal& node,std::string s);

     // MS: build Variants for use in tree traversal (access with getVariantT())
     std::string buildVariantEnums();

     // PC: build ReferenceToPointerHandler and related classes
     std::string buildReferenceToPointerHandlerCode();

         //AS: build the function to get the class hierarchy subtree 
     std::string buildClassHierarchySubTreeFunction();

         //AS: build the funtion to automatically generate the memory pool based traversal on VariantVectors
     std::string buildMemoryPoolBasedVariantVectorTraversalSupport();

     // MS: build VariantEnumnames 
     std::string buildVariantEnumNames();

  // DQ (11/27/2005): Support for generation of code to be used for automating name changes and interface fixes in ROSE.
     std::string buildTransformationSupport();

     // MS: creates Include list (used by buildImplementationForTerminal and travMemberAccessEnums)
     std::vector<GrammarString*> classMemberIncludeList(Terminal& node);

     // MK: This member function is used by the member function buildTreeTraversalFunctions()
     // (see above) in order to determine if the current node of the grammar corresponds
     // to a grammar class whose objects may actually occur in an AST.
     bool isAstObject(Terminal& node);

     // MK: We need this function to determine if the object is a pointer to an STL container
     bool isSTLContainerPtr(const std::string& typeString);

     // MK: We need this function to determine if the object itself is an STL container
     bool isSTLContainer(const std::string& typeString);

  // MK: Method to build the iterator declaration for traversing an STL container
     std::string getIteratorString(const std::string& typeString);

  // DQ (5/24/2005): Added support to output sizes of IR nodes 
     std::string buildMemoryStorageEvaluationSupport();

  // DQ (11/26/2005): Support for visitor patterns (experimental)
     std::string buildVisitorBaseClass();

  // DQ (12/23/2005): Support for building the code to use the visitor 
  // pattern on the IR nodes in memory pools.
     std::string buildMemoryPoolBasedTraversalSupport();

     private:
       // file cache for reading files
       static std::vector<grammarFile*> fileList;
       std::string restrictedTypeStringOfGrammarString(GrammarString* gs, Terminal* grammarnode, std::string grammarSymListOpPrefix, std::string grammarSymListOpPostfix);
       std::set<std::string> traversedTerminals;
       GrammarSynthesizedAttribute CreateMinimalTraversedGrammarSymbolsSet(Terminal* grammarnode, std::vector<GrammarSynthesizedAttribute> v);
       bool isAbstractGrammarSymbol(std::string);

   // JH (01/13/2006) Added to build code for ast file IO
      public:
   // JH (11/04/2005): declaration of methods, needed to build the code for the
   //StorageClasses, after generation of the code located in SageIII/astFileIO/StorageClasses.C
       std::string myBuildHeaderStringAfterMarker  ( const std::string& marker, const std::string& fileName );

      void buildStorageClassSourceFiles ( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );
      void buildStorageClassHeaderFiles ( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );
      void buildIRNodeConstructorOfStorageClassSource ( Terminal & node, StringUtility::FileWithLineNumbers & outputFile );
      std::string buildStringForStorageClassSource ( Terminal & node );
      std::string buildStorageClassDeclarations ( );

   // DQ (4/6/2006): Added in Jochen's new version
      std::string buildStaticDataMemberListClassEntries( Terminal & node);
      std::string buildAccessFunctionsOfClassEntries( Terminal & node);
      std::string buildAccessFunctionSources( Terminal & node);
      std::string buildDataMemberStorageClass( Terminal & node);
      std::string buildStaticStorageClassPickOutSource( Terminal & node);
      std::string generateStaticDataConstructorSource(Terminal & node);
      std::string generateStaticDataWriteEasyStorageDataToFileSource(Terminal & node);
      std::string generateStaticDataReadEasyStorageDataFromFileSource(Terminal & node);
      std::string generateStaticDataArrangeEasyStorageInOnePoolSource(Terminal & node);
      std::string generateStaticDataDeleteEasyStorageMemoryPoolSource(Terminal & node);
      std::string buildStaticDataMemberListSetStaticDataSource(Terminal & node);
      std::string buildStaticDataMemberListDeleteStaticDataSource(Terminal & node);
      std::string buildStaticDataMemberListClassConstructor(Terminal & node);

      void  generateAST_FILE_IOFiles ( );
      void  generateStorageClassesFiles ( );
      std::vector<std::string>& getListOfBuiltinTypes();

   // DQ (4/6/2006): Added in Jochen's new version
      std::vector<std::string>& getListOfAbstractClasses();

   // JH(10/25/2005): declaration of the grammar functions that build the header and the source of the
   // AstFileIO class, loacated after the generation in SageIII/astFileIO/AstFileIO.(hC)
     std::string build_header_AST_FILE_IO_CLASS();
     std::string build_source_AST_FILE_IO_CLASS();

  // DQ (5/18/2007): support for documentation to handle mapping to KDM
     std::string outputClassesAndFields ( Terminal & node );
   };

// We need a defintion of an arbitrary identifier
// This is defined and initialized in the grammar.C file
// extern Terminal identifier;

// We need to have all the strdup use the C++ new/delete
// mechanism instead of the C malloc/free mechanism
// #define strdup Grammar::stringDuplicate

// Macro used to define terminals we want to object to be built on the heap so that
// it will not go out of scope, but we want a reference to the object instead of
// a pointer to the object to preserve the interface with existing code!
#define NEW_TERMINAL_MACRO(TERMINAL_NAME,TERMINAL_NAME_STRING,TAG_NAME_STRING)                \
     Terminal & TERMINAL_NAME = terminalConstructor (TERMINAL_NAME_STRING, *this,             \
                                                     TERMINAL_NAME_STRING, TAG_NAME_STRING ); \
     ROSE_ASSERT (TERMINAL_NAME.associatedGrammar != NULL);


// A new nonterminal should not be born a parent of any child
#define NEW_NONTERMINAL_MACRO(NONTERMINAL_NAME, NONTERMINAL_EXPRESSION, NONTERMINAL_NAME_STRING, NONTERMINAL_TAG_STRING, NONTERMINAL_CAN_HAVE_INSTANCES) \
     Terminal & NONTERMINAL_NAME = nonTerminalConstructor ( NONTERMINAL_NAME_STRING, *this, NONTERMINAL_NAME_STRING, NONTERMINAL_TAG_STRING, (SubclassListBuilder() | NONTERMINAL_EXPRESSION), NONTERMINAL_CAN_HAVE_INSTANCES ); \
     ROSE_ASSERT (NONTERMINAL_NAME.associatedGrammar != NULL);
     // /*printf ("%s ---> ",NONTERMINAL_NAME_STRING);*/ NONTERMINAL_NAME.show(); //printf ("\n"); //MS

#endif // endif for ROSETTA_GRAMMAR_H

