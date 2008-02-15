// Grammar class definition to allow us to define grammars as input to ROSE

#ifndef __ROSETTA_GRAMMAR_H__
#define __ROSETTA_GRAMMAR_H__

// DQ (3/12/2006): We want to remove config.h from being placed in every source file
#include <rose_config.h>

// DQ (11/10/2007): Include ROSE specific paths to support external tools
// These are included here, for consistancy, but are not currently used within ROSETTA.
#include "rose_paths.h"

#include <stdio.h>
#include <stdlib.h>

// We need to separate the construction of the code for ROSE from the ROSE code
// include "rose.h"
// #include FSTREAM_HEADER_FILE
#include <fstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include </usr/include/sys/file.h>
#include <unistd.h>
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

#include <list>

// using namespace std;

// DQ (1/15/2007): Removed since we only use modern C++ compilers now!
#if 0
#if 0
// Use list.h with the SUN 4.2 C++ compiler
// use the STL list classes
// include STL_LIST_HEADER_FILE
#include <list.h>
#else
// The SUN CC 5.0 and 6.0 compiler expects to see list instead of list.h
// also need to declare "using namespace std;"
// include <list>
// using namespace std;
#include <list>
// #ifndef STL_LIST_IS_BROKEN
// // #include <list>
// #include STL_LIST_HEADER_FILE
// #endif
using namespace std;
// using namespace std;
// #ifndef NAMESPACE_IS_BROKEN
// using namespace std;
// #endif
#endif
#endif

class SgProject;


#include "terminalList.h"

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
class NonTerminal;
class GrammarTreeNode;
class GrammarString;

class Grammar
   {
  // This class defines a language grammar and pemits it to be input in a form similar to BNF notation.
  // Later we will template this class to permit it to refer to an arbitrary lower level grammar.
     public:


       // Pointer to member function
       // typedef List<GrammarString> & (Terminal::*FunctionPointerType)(int i,int j) const;
          typedef std::list<GrammarString *> & (Terminal::* FunctionPointerType)(int i,int j) const;

       // Pointer to member function
          typedef std::string (GrammarString::* StringGeneratorFunctionPointerType)() const;





     public:
          TerminalList    terminalList;
          NonTerminalList nonTerminalList;
          std::map<std::string, size_t> astNodeToVariantMap;
          std::map<size_t, std::string> astVariantToNodeMap;
          std::map<size_t, Terminal*> astVariantToTerminalMap;

          GrammarTreeNode* rootNode;
          TerminalList leafNodeList;
          TerminalList allNodeList;

#if 0
       // Not clear if this is worthwhile (not specific enough to map between grammars)
          List<Terminal> elementsMappingToType;
          List<Terminal> elementsMappingToExpression;
          List<Terminal> elementsMappingToStatement;
          List<Terminal> elementsMappingToSymbol;
#endif

       // This is a pointer to the lower level grammar (every grammar has a lower level grammar except the C++ grammar)
          const Grammar* parentGrammar;

       // filename of file containing declaration and source code for a grammar's supporting classes
          std::string filenameForSupportClasses;

       // filename of file containing declaration and source code for a grammar's supporting classes
	  std::string filenameForGlobalDeclarations;

       // Support for output of constructors as part of generated documentation
          static std::string staticContructorPrototypeString;

       // List processing mechanisms
          void generateStringListsFromSubtreeLists ( GrammarTreeNode & node,
						     std::list<GrammarString *> & ,
						     std::list<GrammarString *> & excludeList,
						     FunctionPointerType listFunction );

          void generateStringListsFromLocalLists ( GrammarTreeNode & node,
						   std::list<GrammarString *> & ,
						   std::list<GrammarString *> & excludeList,
						   FunctionPointerType listFunction );

          void generateStringListsFromAllLists ( GrammarTreeNode & node,
						 std::list<GrammarString *> & includeList,
						 std::list<GrammarString *> & excludeList,
						 FunctionPointerType listFunction );

          static void editStringList ( std::list<GrammarString *> & , 
                                       std::list<GrammarString *> & excludeList );

	  std::string buildStringFromLists ( GrammarTreeNode & node,
                                       FunctionPointerType listFunction,
                                       StringGeneratorFunctionPointerType stringGeneratorFunction );

	  std::list<GrammarString *> buildListFromLists ( GrammarTreeNode & node,
								      FunctionPointerType listFunction );

          void setRootOfGrammar ( GrammarTreeNode* RootNodeForGrammar );
          GrammarTreeNode & getRootOfGrammar ();
          NonTerminal & getRootToken ();

          TerminalList & getLeafNodeList() { return leafNodeList; };
          void setupLeafNodeList ();
          void traverseTreeToSetupLeafNodeList ( GrammarTreeNode & node );

         ~Grammar ();
          Grammar ();
          Grammar ( const Grammar & X );

       // Associative terminal/nonterminal access function (cute but does
       // can not be overloaded to return terminals and nonterminals!
       // Terminal & operator[] ( char* name ) const;
          Terminal    & getTerminal    ( const std::string& name ) const;
          NonTerminal & getNonTerminal ( const std::string& name ) const;

          void consistencyCheck() const;

       // builds the header files and source code
          void buildCode ();

       // char* getFilenameForSupportClasses ();
       // void  setFilenameForSupportClasses ( char* filename );
	  std::string getFilenameForGlobalDeclarations ();
          void  setFilenameForGlobalDeclarations ( const std::string& filename );

          StringUtility::FileWithLineNumbers buildMiscSupportDeclarations ();

       // Principle constructor
          Grammar ( const std::string& inputGrammarName, 
                    const std::string& inputPrefixName,
                    const std::string& inputGrammarNameBaseClass = "ROSE_BaseGrammar",
                    const Grammar* parentGrammar = NULL );

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
          void addGrammarElement    ( NonTerminal & X );
          void deleteGrammarElement ( Terminal    & X );
          void deleteGrammarElement ( NonTerminal & X );

       // Check if the terminal or nonterminal (identified by a string) is a terminal or nonterminal
          bool isTerminal    ( const std::string& terminalName ) const;
          bool isNonTerminal ( const std::string& nonTerminalName ) const;

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
          void addNewNonTerminal( SgProject & project, NonTerminal & X );

       // This is the mechanism for restricting a grammars through the
       // removal of new terminals and non-terminals added as a result of
       // their addition using the addNewTerminal member function (which adds
       // many expression terminals for any added new type terminal and adds
       // many new statement terminals if an expression terminal is added).
          void removeNewTerminal   ( Terminal    & X );
          void removeNewNonTerminal( NonTerminal & X );

       // Add a new terminal to the existing grammar.  This function automatically
       // locates the terminal/nonterminal within the grammar's heirarchy
       // void addTerminal    ( SgProject & project, Terminal    & X );
       // void addNonTerminal ( SgProject & project, NonTerminal & X );

          Terminal* copy ( const std::string& terminalName );

       // Removes a terminal (which we look up from the string) from the grammar
          void remove ( const std::string& terminalName );

       // Get the variant for a node name
          size_t getVariantForNode(const std::string& name) const;
          size_t getVariantForTerminal(const Terminal& name) const;
          size_t getVariantForNonterminal(const NonTerminal& name) const;
       // Get the node name for a variant, asserting if not found
          std::string getNodeForVariant(size_t var) const;
          Terminal& getTerminalForVariant(size_t var);

     protected:
       // mechanism for internal use only
          void internalRemove ( const std::string& terminalName );

     public:
          void removeReferenceFromLists ( Terminal & X );

       // Removes header and source files specific to this grammar
       // void  clearGeneratedFiles ();

#if 0   // BP: 10/10/2001, moving to GrammarString class
          static char* copyEdit ( char* inputString, const char* oldToken, const char* newToken );
          static int isSameName ( const char* s1, const char* s2 );
          static bool isContainedIn ( const char* longString, const char* shortString );
          static char* stringDuplicate ( const char* tempString );
#endif

          void buildTree ();
          void removeExclusionsFromTree();

          void traverseTreeToSetupAllNodeList ();
          void traverseTreeToSetupAllNodeList ( GrammarTreeNode & node );

	  std::string getDerivedClassDeclaration ( GrammarTreeNode & node );

	  StringUtility::FileWithLineNumbers buildHeaderStringAfterMarker  ( const std::string& marker, const std::string& fileName );
	  StringUtility::FileWithLineNumbers buildHeaderStringBeforeMarker ( const std::string& marker, const std::string& fileName );

#if 0   // BP : 10/10/2001, moved to GrammarString class

       // A smarter string concatination function (corrects for space in the target automatically)
          static char* stringConcatinate ( const char* target, const char* endingString );
#endif
	  std::string sourceCodeDirectoryName ();

          // static std::string readFile ( const std::string& inputFileName );
          static StringUtility::FileWithLineNumbers readFileWithPos ( const std::string& inputFileName );
          static void writeFile ( const StringUtility::FileWithLineNumbers& outputString, const std::string& directoryName, 
                                  const std::string& className, const std::string& fileExtension );

	  std::string generateTraverseSuccessorForLoopSource(std::string typeString, 
							std::string memberVariableName, 
							std::string successorContainerName,
							std::string successorContainerAccessOperator);
      // GB (8/1/2007)
	  std::string generateNumberOfSuccessorsComputation(std::list<GrammarString*>& traverseDataMemberList,
                               std::string successorContainerName);
	  std::string generateTraverseSuccessor(GrammarString* gs, std::string successorContainerName);
	  std::string generateTraverseSuccessorNamesForLoopSource(std::string typeString, 
						       std::string memberVariableName, 
						       std::string successorContainerName,
							     std::string successorContainerAccessOperator);
	  std::string generateTraverseSuccessorNames(GrammarString* gs, std::string successorContainerName);

	  std::string generateRTICode(GrammarString* gs, std::string successorContainerName);
	  void buildRTIFile(GrammarTreeNode* rootNode, StringUtility::FileWithLineNumbers& rttiFile);
	  StringUtility::FileWithLineNumbers buildVariants ();
	  StringUtility::FileWithLineNumbers buildForwardDeclarations ();

       // DQ (10/26/2007): Add the protytype for the Cxx_GrammarTerminalNames
          void buildVariantsStringPrototype ( StringUtility::FileWithLineNumbers & outputFile );
          void buildVariantsStringDataBase ( StringUtility::FileWithLineNumbers & outputFile );

          void buildHeaderFiles ( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );

	  std::string buildStringForPrototypes( GrammarTreeNode & node );
	  StringUtility::FileWithLineNumbers buildStringForDataDeclaration                     ( GrammarTreeNode & node );
	  StringUtility::FileWithLineNumbers buildStringForDataAccessFunctionDeclaration      ( GrammarTreeNode & node );
	  StringUtility::FileWithLineNumbers buildStringForSource                              ( GrammarTreeNode & node );

       // Builds the "CLASSNAME::variant()" member function
	  StringUtility::FileWithLineNumbers buildStringForVariantFunctionSource               ( GrammarTreeNode & node );

       // Builds the "CLASSNAME::isCLASSNAME()" friend function
	  StringUtility::FileWithLineNumbers supportForBuildStringForIsClassNameFunctionSource ( GrammarTreeNode & node, const StringUtility::FileWithLineNumbers& accumulationString );
	  StringUtility::FileWithLineNumbers buildStringForIsClassNameFunctionSource           ( GrammarTreeNode & node );

       // DQ (9/21/2005): Added support for memory pools
	  StringUtility::FileWithLineNumbers buildStringForNewAndDeleteOperatorSource          ( GrammarTreeNode & node );

       // DQ (12/24/2005): Support for memory pool traversal
	  StringUtility::FileWithLineNumbers buildStringForTraverseMemoryPoolSource            ( GrammarTreeNode & node );

       // DQ & JH (1/17/2006): Added support for building code to check pointers to IR nodes
	  StringUtility::FileWithLineNumbers buildStringForCheckingIfDataMembersAreInMemoryPoolSource ( GrammarTreeNode & node );
	  StringUtility::FileWithLineNumbers buildStringToTestPointerForContainmentInMemoryPoolSource ( GrammarTreeNode & node );

       // AS (2/14/06): Added support for building code to return data member pointers to IR nodes
	  StringUtility::FileWithLineNumbers buildStringForReturnDataMemberPointersSource ( GrammarTreeNode & node );

	  StringUtility::FileWithLineNumbers buildStringForReturnDataMemberReferenceToPointersSource ( GrammarTreeNode & node );

       // DQ (3/7/2007): support for getChildIndex member function
	  StringUtility::FileWithLineNumbers buildStringForGetChildIndexSource ( GrammarTreeNode & node );


          bool buildConstructorParameterList ( GrammarTreeNode & node, 
                                               std::list<GrammarString *> & constructorParameterList,
                                               ConstructParamEnum config );
	  std::string buildConstructorParameterListString ( GrammarTreeNode & node, bool withInitializers, bool withTypes, ConstructParamEnum config, bool *complete = 0 );


       // DQ 11/6/2006): Support for building newer from of constructors (withouth source position information).
          void markNodeForConstructorWithoutSourcePositionInformation ( GrammarTreeNode & node );
          void markNodeForConstructorWithoutSourcePositionInformationSupport( GrammarTreeNode & node );
	  StringUtility::FileWithLineNumbers buildConstructorWithoutSourcePositionInformation       ( GrammarTreeNode & node );
          void buildConstructorWithoutSourcePositionInformationSupport( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (11/7/2006): Get a specific node from the tree.
          GrammarTreeNode* getNamedNode ( GrammarTreeNode & node, const std::string & name );
          GrammarString* getNamedDataMember ( GrammarTreeNode & node, const std::string & name );

       // DQ (3/24/2006): Separated these functions so that we could position the data member 
       // variable declaration at the base of the class and the generated access functions at 
       // the top.  This permist us to present the documentation better using Doxygen.
       // string buildDataPrototypesAndAccessFunctionPrototypesAndConstuctorPrototype ( GrammarTreeNode & node );
	  StringUtility::FileWithLineNumbers buildDataMemberVariableDeclarations ( GrammarTreeNode & node );
	  StringUtility::FileWithLineNumbers buildMemberAccessFunctionPrototypesAndConstuctorPrototype ( GrammarTreeNode & node );

	  StringUtility::FileWithLineNumbers buildConstructor ( GrammarTreeNode & node );

	  StringUtility::FileWithLineNumbers buildCopyMemberFunctionSource ( GrammarTreeNode & node );

          void buildSourceFiles ( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (12/23/2005): Added to support moving the new and elete operators to a 
       // different file (mostly for clarity to support users who read the source code).
          void buildNewAndDeleteOperators( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (12/24/2005): Support for memory pool traversal
          void buildTraverseMemoryPoolSupport( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ & JH (1/17/2006): Added support for building code to check pointers to IR nodes
          void buildStringForCheckingIfDataMembersAreInMemoryPoolSupport( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );

       // AS(2/14/2006): Added support for building code to return pointers to IR nodes
          void buildStringForReturnDataMemberPointersSupport ( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (4/30/2006): Support for buildStringForReturnDataMemberReferenceToPointersSource
          void buildStringForReturnDataMemberReferenceToPointersSupport( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (12/23/2005): Relocated copy function to a seperate file to imrove readability of source code
          void buildCopyMemberFunctions( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );

       // DQ (3/7/2007): support for getChildIndex member function
          void buildStringForGetChildIndexSupport( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );

       // Uses list of targets and sources stored within each node to drive substitutions
	  StringUtility::FileWithLineNumbers editSubstitution ( GrammarTreeNode & node, const StringUtility::FileWithLineNumbers& editString );

       // This calls the functions to build the parsers (one for each child of the root node)
          void buildGrammarClassSourceCode ( StringUtility::FileWithLineNumbers & outputFile );

          static StringUtility::FileWithLineNumbers extractStringFromFile ( const std::string& startMarker, const std::string& endMarker,
                                                     const std::string& filename, const std::string& directory );

          void printTreeNodeNames ( const GrammarTreeNode & node ) const;
       // void setUpParentNames ();
       // void setUpParentNames ( GrammarTreeNode & X );

       // MK: The following member function is no longer needed:
       // void traverseTree ( GrammarTreeNode & X );

       // These functions build terminal and nonterminal objects to be associated with this grammar
       // Using a member function to construct these serves several purposes:
       // 1) organizes terminals and nonterminals with there respective grammar (without ambiguity)
       // 2) avoids or deferes the implementation of the envelop/letter interface mechanism so
       //    that the letter will have a scope longer than the envelope
          NonTerminal & nonTerminalConstructor ( const Terminal & token );
          NonTerminal & nonTerminalConstructor ( const NonTerminal & X );
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
	  std::string buildParserPrototype ( GrammarTreeNode & node ); 
	  std::string buildParserSource    ( GrammarTreeNode & node ); 

       // a more general way to traverse the Grammar and build strings for source code generation

       // MS: Grammar Traversal defs
          class GrammarSynthesizedAttribute
             {
               public:
                    GrammarTreeNode* grammarnode;
		    std::string text;
		    std::string nodetext;
		    std::string nonterminalsbunch;
		    std::string terminalsbunch;
		    std::string problematicnodes;
             };

	  typedef std::string (Grammar::*evaluateStringAttributeFunctionType)(GrammarTreeNode&, std::string);
	  std::string naiveTraverseGrammar(GrammarTreeNode&, evaluateStringAttributeFunctionType);
	  typedef GrammarSynthesizedAttribute (Grammar::*evaluateGAttributeFunctionType)(GrammarTreeNode*, 
											 std::vector<GrammarSynthesizedAttribute>);
	  // MS: type hierarchy traversal
	  GrammarSynthesizedAttribute BottomUpProcessing(GrammarTreeNode* node, evaluateGAttributeFunctionType evaluateGAttributeFunction);
	  // MS: build a Latex output that shows the abstract Cpp grammar
	  GrammarSynthesizedAttribute CreateGrammarDotString(GrammarTreeNode* grammarnode,
							       std::vector<GrammarSynthesizedAttribute> v);
	  GrammarSynthesizedAttribute CreateGrammarLatexString(GrammarTreeNode* grammarnode,
							       std::vector<GrammarSynthesizedAttribute> v);
	  void buildGrammarDotFile(GrammarTreeNode* rootNode, std::ostream& GrammarDotFile);
	  void buildGrammarLatexFile(GrammarTreeNode* rootNode, std::ostream& GrammarLatexFile);

	  // MS: generate source for implementation of the RTI interface
	  GrammarSynthesizedAttribute generateRTIImplementation(GrammarTreeNode* grammarnode,
								 std::vector<GrammarSynthesizedAttribute> v);
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
     GrammarNodeInfo getGrammarNodeInfo(GrammarTreeNode* grammarnode);

     // MS: generates the code to implement the creation of the treeTraversalSuccessorContainer in Sage
     void buildTreeTraversalFunctions(GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile);

     // MS: create source code for enums used in traversal functions to access synthesized attribute values
     void buildEnumForNode(GrammarTreeNode& node, std::string& enumString);
     std::string EnumStringForNode(GrammarTreeNode& node,std::string s);

     // MS: build Variants for use in tree traversal (access with getVariantT())
     std::string buildVariantEnums();

	 //AS: build the function to get the class hierarchy subtree 
     std::string buildClassHierarchySubTreeFunction();

	 //AS: build the funtion to automatically generate the memory pool based traversal on VariantVectors
     std::string buildMemoryPoolBasedVariantVectorTraversalSupport();

     // MS: build VariantEnumnames 
     std::string buildVariantEnumNames();

  // DQ (11/27/2005): Support for generation of code to be used for automating name changes and interface fixes in ROSE.
     std::string buildTransformationSupport();

     // MS: creates Include list (used by buildImplementationForTerminal and travMemberAccessEnums)
     std::list<GrammarString*> classMemberIncludeList(GrammarTreeNode& node);

     // MK: This member function is used by the member function buildTreeTraversalFunctions()
     // (see above) in order to determine if the current node of the grammar corresponds
     // to a grammar class whose objects may actually occur in an AST.
     bool isAstObject(GrammarTreeNode& node);

     // MK: We need this function to determine if the object is a pointer to an STL container
     bool isSTLContainerPtr(const std::string& typeString);

     // MK: We need this function to determine if the object itself is an STL container
     bool isSTLContainer(const std::string& typeString);

     // MK: Method to build the iterator declaration for traversing an STL container
     std::string getIteratorString(const std::string& typeString);

     // MS
     std::string getContainerElementTypeString(const std::string& typeString);

  // DQ (5/24/2005): Added support to output sizes of IR nodes 
     std::string buildMemoryStorageEvaluationSupport();

  // DQ (11/26/2005): Support for visitor patterns (experimental)
     std::string buildVisitorBaseClass();

  // DQ (12/23/2005): Support for building the code to use the visitor 
  // pattern on the IR nodes in memory pools.
     std::string buildMemoryPoolBasedTraversalSupport();

     private:
       // char* computeNodeMemberFunctionString ( GrammarTreeNode & node, const List<GrammarString> & list );
       // file cache for reading files
       static std::vector<grammarFile*> fileList;
       std::string restrictedTypeStringOfGrammarString(GrammarString* gs, GrammarTreeNode* grammarnode, std::string grammarSymListOpPrefix, std::string grammarSymListOpPostfix);
       std::set<std::string> traversedTerminals;
       GrammarSynthesizedAttribute CreateMinimalTraversedGrammarSymbolsSet(GrammarTreeNode* grammarnode, std::vector<GrammarSynthesizedAttribute> v);
       bool isAbstractGrammarSymbol(std::string);
       std::string cocoScannerDefineName(std::string s);
       std::string cocoScannerDefines();

   // JH (01/13/2006) Added to build code for ast file IO
      public:
   // JH (11/04/2005): declaration of methods, needed to build the code for the
   //StorageClasses, after generation of the code located in SageIII/astFileIO/StorageClasses.C
       std::string myBuildHeaderStringAfterMarker  ( const std::string& marker, const std::string& fileName );

   // DQ (4/6/2006): Removed from Jochen's new version
   // void buildParentStorageClassHeaderFiles (ostream & outputFile );

      void buildStorageClassSourceFiles ( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );
      void buildStorageClassHeaderFiles ( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );
      void buildIRNodeConstructorOfStorageClassSource ( GrammarTreeNode & node, StringUtility::FileWithLineNumbers & outputFile );
      std::string buildStringForStorageClassSource ( GrammarTreeNode & node );
      std::string buildStorageClassDeclarations ( );

   // DQ (4/6/2006): Added in Jochen's new version
      std::string buildStaticDataMemberListClassEntries( GrammarTreeNode & node);
      std::string buildAccessFunctionsOfClassEntries( GrammarTreeNode & node);
      std::string buildAccessFunctionSources( GrammarTreeNode & node);
      std::string buildDataMemberStorageClass( GrammarTreeNode & node);
      std::string buildStaticStorageClassPickOutSource( GrammarTreeNode & node);
      std::string generateStaticDataConstructorSource(GrammarTreeNode & node);
      std::string generateStaticDataWriteEasyStorageDataToFileSource(GrammarTreeNode & node);
      std::string generateStaticDataReadEasyStorageDataFromFileSource(GrammarTreeNode & node);
      std::string generateStaticDataArrangeEasyStorageInOnePoolSource(GrammarTreeNode & node);
      std::string generateStaticDataDeleteEasyStorageMemoryPoolSource(GrammarTreeNode & node);
      std::string buildStaticDataMemberListSetStaticDataSource(GrammarTreeNode & node);
      std::string buildStaticDataMemberListDeleteStaticDataSource(GrammarTreeNode & node);
      std::string buildStaticDataMemberListClassConstructor(GrammarTreeNode & node);

      void  generateAST_FILE_IOFiles ( );
      void  generateStorageClassesFiles ( );
      std::list<std::string>& getListOfBuiltinTypes();

   // DQ (4/6/2006): Added in Jochen's new version
      std::list<std::string>& getListOfAbstractClasses();

   // DQ (4/6/2006): Removed from Jochen's new version
   // unsigned int getNumberOfWorkingPointers();

   // JH(10/25/2005): declaration of the grammar functions that build the header and the source of the
   // AstFileIO class, loacated after the generation in SageIII/astFileIO/AstFileIO.(hC)
     std::string build_header_AST_FILE_IO_CLASS();
     std::string build_source_AST_FILE_IO_CLASS();

  // DQ (5/18/2007): support for documentation to handle mapping to KDM
  // std::string outputClassesAndFields ( GrammarTreeNode & node, std::ostream & outputFile );
     std::string outputClassesAndFields ( GrammarTreeNode & node );
   };

// We need a defintion of an arbitrary identifier
// This is defined and initialized in the grammar.C file
extern Terminal identifier;

// We need to have all the strdup use the C++ new/delete
// mechanism instead of the C malloc/free mechanism
#define strdup Grammar::stringDuplicate

// Macro used to define terminals we want to object to be built on the heap so that
// it will not go out of scope, but we want a reference to the object instead of
// a pointer to the object to preserve the interface with existing code!
#define NEW_TERMINAL_MACRO(TERMINAL_NAME,TERMINAL_NAME_STRING,TAG_NAME_STRING)                \
     Terminal & TERMINAL_NAME = terminalConstructor (TERMINAL_NAME_STRING, *this,             \
                                                     TERMINAL_NAME_STRING, TAG_NAME_STRING ); \
     ROSE_ASSERT (TERMINAL_NAME.parentTerminal == NULL); \
     ROSE_ASSERT (TERMINAL_NAME.associatedGrammar != NULL);


// A new nonterminal should not be born a parent of any child
#define NEW_NONTERMINAL_MACRO(NONTERMINAL_NAME, NONTERMINAL_EXPRESSION, NONTERMINAL_NAME_STRING,NONTERMINAL_TAG_STRING) \
     NonTerminal & NONTERMINAL_NAME = nonTerminalConstructor ( NONTERMINAL_EXPRESSION ); \
     ROSE_ASSERT (NONTERMINAL_NAME.parentTerminal == NULL); \
     NONTERMINAL_NAME.setGrammar (this);  \
     NONTERMINAL_NAME.setName(NONTERMINAL_NAME_STRING,NONTERMINAL_TAG_STRING); \
     ROSE_ASSERT (NONTERMINAL_NAME.associatedGrammar != NULL); \
     addGrammarElement(NONTERMINAL_NAME); \
     /*printf ("%s ---> ",NONTERMINAL_NAME_STRING);*/ NONTERMINAL_NAME.show(); //printf ("\n"); //MS

// NONTERMINAL_NAME.display("In MACRO NEW_NONTERMINAL MACRO");

// Macro used in Grammar::NonTerminal::operator |= ( const Terminal & Y )
// The use of macros here is only temporary!
// Notes:
//    1) The copy function builds a new terminal and sets up the parent child relationship
//    2) The automate generation of the parse functions are suppressed for these child 
//       terminals/nonterminals.

#define BUILD_CHILD_TERMINAL_MACRO(TERMINAL_NAME,ORIGINAL_TERMINAL_NAME_STRING,NEW_TERMINAL_NAME_STRING,NEW_TERMINAL_TAG_STRING) \
     Terminal & TERMINAL_NAME = getTerminal(ORIGINAL_TERMINAL_NAME_STRING) \
                                     .copy(NEW_TERMINAL_NAME_STRING,NEW_TERMINAL_TAG_STRING); \
     TERMINAL_NAME.excludeFunctionPrototype ( "HEADER_PARSER", "Grammar/Node.code"); \
     TERMINAL_NAME.excludeFunctionSource ( "SOURCE_PARSER", "Grammar/parserSourceCode.macro" ); \
     ROSE_ASSERT(TERMINAL_NAME.associatedGrammar != NULL); \
     ROSE_ASSERT(TERMINAL_NAME.isChild() == TRUE); \
     ROSE_ASSERT(TERMINAL_NAME.associatedGrammar->isTerminal(NEW_TERMINAL_NAME_STRING) == TRUE);

//   TERMINAL_NAME.excludeSubTreeFunctionPrototype ( "HEADER_PARSER", "Grammar/Node.code");

// TERMINAL_NAME.associatedGrammar->addGrammarElement(TERMINAL_NAME);
// orTokenPointerArray.addElement (TERMINAL_NAME);

// Note that the assignment of the expression forces the parentTerminal to NULL so it is reset
#define BUILD_CHILD_NONTERMINAL_MACRO(NON_TERMINAL_NAME,NONTERMINAL_EXPRESSION,ORIGINAL_NON_TERMINAL_NAME_STRING,NEW_NON_TERMINAL_NAME_STRING,NEW_NON_TERMINAL_TAG_STRING) \
     NonTerminal & NON_TERMINAL_NAME = getNonTerminal(GrammarString::stringDuplicate(ORIGINAL_NON_TERMINAL_NAME_STRING)) \
               .copy(GrammarString::stringDuplicate(NEW_NON_TERMINAL_NAME_STRING),GrammarString::stringDuplicate(NEW_NON_TERMINAL_TAG_STRING));  \
     ROSE_ASSERT(NON_TERMINAL_NAME.isChild() == TRUE);                             \
     ROSE_ASSERT(NON_TERMINAL_NAME.automaticGenerationOfDestructor          ==     \
                 NON_TERMINAL_NAME.getParentTerminal()->automaticGenerationOfDestructor);  \
     ROSE_ASSERT(NON_TERMINAL_NAME.associatedGrammar != NULL);                     \
     NON_TERMINAL_NAME.setExpression ( NONTERMINAL_EXPRESSION );                   \
     NON_TERMINAL_NAME.setParentTerminal(&(getNonTerminal(GrammarString::stringDuplicate(ORIGINAL_NON_TERMINAL_NAME_STRING)))); \
     ROSE_ASSERT(NON_TERMINAL_NAME.isTemporary() == FALSE);                        \
     ROSE_ASSERT(NON_TERMINAL_NAME.isChild() == TRUE);                             \
     NON_TERMINAL_NAME.setName(GrammarString::stringDuplicate(NEW_NON_TERMINAL_NAME_STRING));              \
     NON_TERMINAL_NAME.setLexeme(GrammarString::stringDuplicate(NEW_NON_TERMINAL_NAME_STRING));            \
     NON_TERMINAL_NAME.setTagName(GrammarString::stringDuplicate(NEW_NON_TERMINAL_TAG_STRING));            \
     NON_TERMINAL_NAME.excludeFunctionPrototype ( "HEADER_PARSER", "Grammar/Node.code");       \
     NON_TERMINAL_NAME.excludeFunctionSource    ( "SOURCE_PARSER", "Grammar/parserSourceCode.macro" ); \
     ROSE_ASSERT(NON_TERMINAL_NAME.associatedGrammar != NULL);                     \
     ROSE_ASSERT(NON_TERMINAL_NAME.automaticGenerationOfConstructor         ==     \
                 NON_TERMINAL_NAME.getParentTerminal()->automaticGenerationOfConstructor); \
     ROSE_ASSERT(NON_TERMINAL_NAME.automaticGenerationOfDestructor          ==     \
                 NON_TERMINAL_NAME.getParentTerminal()->automaticGenerationOfDestructor);  \
     ROSE_ASSERT(NON_TERMINAL_NAME.automaticGenerationOfDataAccessFunctions ==     \
                 NON_TERMINAL_NAME.getParentTerminal()->automaticGenerationOfDataAccessFunctions); \
     ROSE_ASSERT(NON_TERMINAL_NAME.automaticGenerationOfCopyFunction        ==             \
                 NON_TERMINAL_NAME.getParentTerminal()->automaticGenerationOfCopyFunction);        \
     ROSE_ASSERT(NON_TERMINAL_NAME.isChild() == TRUE);                                     \
     ROSE_ASSERT((NEW_NON_TERMINAL_NAME_STRING) == (NON_TERMINAL_NAME.name)); \
     ROSE_ASSERT(NON_TERMINAL_NAME.associatedGrammar->isNonTerminal(GrammarString::stringDuplicate(NEW_NON_TERMINAL_NAME_STRING)) == TRUE);

#endif // endif for ROSETTA_GRAMMAR_H

