#ifndef __ROSETTA_GRAMMAR_H__
#define __ROSETTA_GRAMMAR_H__

// DQ (3/12/2006): We want to remove config.h from being placed in every source file
#include <rose_config.h>
// DQ (3/22/2009): Added MSVS support for ROSE.
#include "rose_msvc.h"
#include "rose_paths.h"

#include <stdio.h>
#include <stdlib.h>

#include <assert.h>

#include <sstream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <set>
#include "ROSETTA_macros.h"
#include "FileUtility.h"
#include "GrammarFile.h"


#define COMPLETERTI 1

// DQ (3/19/2007): Added the IR nodes for FORTRAN
// DQ (2/2/2006): Control use of Fortran specific IR nodes within ROSETTA
// We will trun this on later once more of the implementation is in place.
#define USE_FORTRAN_IR_NODES 1

// DQ and Liao (6/10/2008): Added IR node support for UPC.
#define USE_UPC_IR_NODES 1

//Liao 5/29/2009: Add IR node support for OpenMP
#define USE_OMP_IR_NODES 1

// DQ (4/12/2011): Added Java specific IR nodes.
#define USE_JAVA_IR_NODES 1

// MH (6/11/2014): Added X10 specific IR nodes.
#define USE_X10_IR_NODES 1

#include <vector>

// using namespace std;

// Switch to control whether CxxGrammarMetaProgram is noisy on standard output.
extern bool verbose;

class SgProject;

/*
 ROSETTA substitution variables: used in *.code files:
 $GRAMMAR_NAME
 $GRAMMAR_PREFIX_
 $PARENT_GRAMMARS_PREFIX_
 $CLASSNAME
 $BASECLASS
 $BASE_CLASS_CONSTRUCTOR_CALL
 $BASE_CLASS_CONSTRUCTOR_PARAMETER
 $CONSTRUCTOR_PARAMETER_LIST
 $CONSTRUCTOR_BODY
 $CLASSTAG
 $CLASS_BASE_NAME
 $PURE_VIRTUAL_MARKER
 $MARKER // see enum $MARKERVariants 
 $MARKER_UNKNOWN_GRAMMAR
 $MARKER_LAST_TAG
 $LIST_LENGTH // see TerminalNamesType $MARKERTerminalNames[$LIST_LENGTH] =
 $MEMBER_FUNCTION_DEFINITIONS
 $CODE_STRING
 $ASSOCIATED_MEMORY_POOL_TEST

 $IFDEF_MARKER_H
 $IFDEF_MARKER

 // memory pool support
 $CLASS_SPECIFIC_STATIC_MEMBERS_MEMORY_USED
 $CLASS_SPECIFIC_STATIC_MEMBERS_USING_ROSE_VISIT
 $CLASS_SPECIFIC_STATIC_MEMBERS_USING_VISITOR_PATTERN
 $CLASS_SPECIFIC_STATIC_MEMBERS_MEMORY_USED
*/

// MS 2015: rewrote ROSETTA to use enums instead of wrapper classes around enums.
// enums: ConstructParamEnum, BuildAccessEnum, CopyConfigEnum
enum ConstructParamEnum { 
  NO_CONSTRUCTOR_PARAMETER,
  CONSTRUCTOR_PARAMETER    
};

enum BuildAccessEnum { 
  NO_ACCESS_FUNCTIONS,
  BUILD_ACCESS_FUNCTIONS,
  // Just like TAG_BUILD_ACCESS_FUNCTIONS except doesn't set p_isModified
  BUILD_FLAG_ACCESS_FUNCTIONS,
  BUILD_LIST_ACCESS_FUNCTIONS
};

enum CopyConfigEnum {
  // Note that CLONE_TREE is the default if nothing is specified in the setDataPrototype() member function.

  /* This skips the generation of any code to copy the 
     pointer (deep or shallow)
  */
  NO_COPY_DATA,
  /* This copies the data (if a pointer this copies the 
     pointer, else calls the operator= for any object)
  */
  COPY_DATA,
  /* This copies the data by building a new object using 
     the copy constructor
  */
  CLONE_PTR,
  /* This builds a new object dependent on the use of the 
     SgCopyHelp input object (deep or shallow) using the object's copy member function
  */
  CLONE_TREE    
};

enum TraversalEnum {
  DEF_TRAVERSAL,
  NO_TRAVERSAL,
};

enum DeleteEnum {
  DEF_DELETE,
  NO_DELETE
};


class AstNodeClass;
class SubclassListBuilder;
class GrammarString;

AstNodeClass* lookupTerminal(const std::vector<AstNodeClass*>& tl, const std::string& name);

class Grammar
   {
  // This class defines a language grammar and pemits it to be input in a form similar to BNF notation.
  // Later we will template this class to permit it to refer to an arbitrary lower level grammar.
     public:


       // Pointer to member function
       // typedef List<GrammarString> & (AstNodeClass::*FunctionPointerType)(int i,int j) const;
          typedef std::vector<GrammarString *> & (AstNodeClass::* FunctionPointerType)(int i,int j) const;

       // Pointer to member function
          typedef std::string (GrammarString::* StringGeneratorFunctionPointerType)() const;

          Grammar ( const Grammar & X );

          /**! The constructor builds a grammar.  The inputs are strings:
               inputGrammarName          -- The name of the grammar (used in the construction of filenames etc.)
               inputPrefixName           -- The name used for the prefix to all classes generated to implement 
                                            the grammar.  This permits each class to have a unique name.  The 
                                            prefix for the base level grammar should be "Sg" to match SAGE!
               inputGrammarNameBaseClass -- The name of the class representing the grammar!  There can
                                            be many grammars defined within a preprocessor.
               inputParentGrammar        -- A pointer to the parent grammar (object) representing the base grammar.
           */
          Grammar ( const std::string& inputGrammarName, 
                    const std::string& inputPrefixName,
                    const std::string& inputGrammarNameBaseClass = "ROSE_BaseGrammar",
                    const Grammar* parentGrammar = NULL,
                    const std::string& t_directory = "");
         ~Grammar ();

     public:
          std::vector<AstNodeClass*>        terminalList;
          std::map<std::string, size_t> astNodeToVariantMap;
          std::map<size_t, std::string> astVariantToNodeMap;
          std::map<size_t, AstNodeClass*>   astVariantToTerminalMap;

          //The directory name we should generate the files to
          std::string target_directory;

          AstNodeClass* rootNode;

       // This is a pointer to the lower level grammar (every grammar has a lower level grammar except the C++ grammar)
          const Grammar* parentGrammar;

       // filename of file containing declaration and source code for a grammar's supporting classes
          std::string filenameForSupportClasses;

       // filename of file containing declaration and source code for a grammar's supporting classes
          std::string filenameForGlobalDeclarations;

       // Support for output of constructors as part of generated documentation
          static std::string staticContructorPrototypeString;

       // List processing mechanisms
          void generateStringListsFromSubtreeLists ( AstNodeClass & node,
                                                     std::vector<GrammarString *> & ,
                                                     std::vector<GrammarString *> & excludeList,
                                                     FunctionPointerType listFunction );

          void generateStringListsFromLocalLists ( AstNodeClass & node,
                                                   std::vector<GrammarString *> & ,
                                                   std::vector<GrammarString *> & excludeList,
                                                   FunctionPointerType listFunction );

          void generateStringListsFromAllLists ( AstNodeClass & node,
                                                 std::vector<GrammarString *> & includeList,
                                                 std::vector<GrammarString *> & excludeList,
                                                 FunctionPointerType listFunction );

          static void editStringList ( std::vector<GrammarString *>& origList, 
                                       const std::vector<GrammarString *> & excludeList );

          std::string buildStringFromLists ( AstNodeClass & node,
                                       FunctionPointerType listFunction,
                                       StringGeneratorFunctionPointerType stringGeneratorFunction );

          std::vector<GrammarString *> buildListFromLists ( AstNodeClass & node, FunctionPointerType listFunction );

          void setRootOfGrammar ( AstNodeClass* RootNodeForGrammar );
          AstNodeClass* getRootOfGrammar ();

     public:

       // Associative AstNodeClass/nonterminal access function
          AstNodeClass    & getTerminal    ( const std::string& name ) const;

          void consistencyCheck() const;

       // builds the header files and source code
          void buildCode ();

          std::string getFilenameForGlobalDeclarations ();
          void  setFilenameForGlobalDeclarations ( const std::string& filename );

          Rose::StringUtility::FileWithLineNumbers buildMiscSupportDeclarations ();

          std::string grammarName;
          std::string grammarPrefixName;
          std::string grammarNameBaseClass;

       // Return the name of the grammar (two cases are handled since the
       // names are different for the "$GRAMMAR_PREFIX_" and the "$GRAMMAR_TAG_PREFIX_")
          const std::string& getGrammarName() const;
          const std::string& getGrammarPrefixName() const;
          std::string getGrammarTagName();

       // Build main class declaration for this grammar
          void buildGrammarClassDeclaration ( Rose::StringUtility::FileWithLineNumbers & outputFile );

          void addGrammarElement    ( AstNodeClass    & X );

       // Check if the AstNodeClass or nonterminal (identified by a string) is a AstNodeClass or nonterminal
          bool isTerminal    ( const std::string& terminalName ) const;

       // This is the mechanism for adding new types to an existing grammar. This function
       // will more generally show how new terminals are added to an existing grammar.  It 
       // will be generalized later to the addition of nonterminals (implying the addition 
       // of subtrees of grammar being added to an existing grammar).  The later use of this 
       // will be important for the addition of the A++/P++ type system (and then Overture and Kull)
       //   -->  (realArray:doubleArray,floatArray; intArray, Indexing: Index,Range).
       // The SgProject contains the declarations associated with the AstNodeClass.  In the 
       // case where this this is a class, member functions of the class will define
       // additional terminals to be added to the grammar.
          void addNewTerminal   ( SgProject & project, AstNodeClass    & X );

       // Get the variant for a node name
          size_t getVariantForNode(const std::string& name) const;
          size_t getVariantForTerminal(const AstNodeClass& name) const;
       // Get the node name for a variant, asserting if not found
          std::string getNodeForVariant(size_t var) const;
          AstNodeClass& getTerminalForVariant(size_t var);

     protected:
       // mechanism for internal use only
          void internalRemove ( const std::string& terminalName );

     public:
          void removeReferenceFromLists ( AstNodeClass & X );

          void removeExclusionsFromTree();

          void traverseTreeToSetupAllNodeList ();
          void traverseTreeToSetupAllNodeList ( AstNodeClass & node );

          std::string getDerivedClassDeclaration ( AstNodeClass & node );

          Rose::StringUtility::FileWithLineNumbers buildHeaderStringAfterMarker  ( const std::string& marker, const std::string& fileName );
          Rose::StringUtility::FileWithLineNumbers buildHeaderStringBeforeMarker ( const std::string& marker, const std::string& fileName );

          std::string sourceCodeDirectoryName();

          static Rose::StringUtility::FileWithLineNumbers readFileWithPos ( const std::string& inputFileName );

       // DQ (12/28/2009): I don't think we want this to be static, since I want to call sourceCodeDirectoryName().
       // static void writeFile ( const StringUtility::FileWithLineNumbers& outputString, const std::string& directoryName, 
       //                         const std::string& className, const std::string& fileExtension );
          void writeFile  ( const Rose::StringUtility::FileWithLineNumbers& outputString, const std::string& directoryName, 
                            const std::string& className, const std::string& fileExtension );
       // DQ (12/31/2009): Added mechanism to append generated text to files.
          void appendFile ( const Rose::StringUtility::FileWithLineNumbers& outputString, const std::string& directoryName, 
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
          void buildRTIFile(AstNodeClass* rootNode, Rose::StringUtility::FileWithLineNumbers& rttiFile);
          Rose::StringUtility::FileWithLineNumbers buildVariants ();
          Rose::StringUtility::FileWithLineNumbers buildForwardDeclarations ();

       // DQ (12/28/2009): Added to support optionally smaller (but more numerous header files for ROSE).
       // StringUtility::FileWithLineNumbers buildIncludesForSeparateHeaderFiles();
          void buildIncludesForSeparateHeaderFiles( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );

       // DQ (10/26/2007): Add the protytype for the Cxx_GrammarTerminalNames
          void buildVariantsStringPrototype ( Rose::StringUtility::FileWithLineNumbers & outputFile );
          void buildVariantsStringDataBase ( Rose::StringUtility::FileWithLineNumbers & outputFile );

          void buildHeaderFiles ( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );

          std::string buildStringForPrototypes( AstNodeClass & node );
          Rose::StringUtility::FileWithLineNumbers buildStringForDataDeclaration                     ( AstNodeClass & node );
          Rose::StringUtility::FileWithLineNumbers buildStringForDataAccessFunctionDeclaration      ( AstNodeClass & node );
          Rose::StringUtility::FileWithLineNumbers buildStringForSource                              ( AstNodeClass & node );

       // Builds the "CLASSNAME::variant()" member function
          Rose::StringUtility::FileWithLineNumbers buildStringForVariantFunctionSource               ( AstNodeClass & node );

       // Builds the "CLASSNAME::isCLASSNAME()" friend function
          Rose::StringUtility::FileWithLineNumbers supportForBuildStringForIsClassNameFunctionSource ( AstNodeClass & node, const Rose::StringUtility::FileWithLineNumbers& accumulationString );
          Rose::StringUtility::FileWithLineNumbers buildStringForIsClassNameFunctionSource           ( AstNodeClass & node );

       // DQ (9/21/2005): Added support for memory pools
          Rose::StringUtility::FileWithLineNumbers buildStringForNewAndDeleteOperatorSource          ( AstNodeClass & node );

       // DQ (12/24/2005): Support for memory pool traversal
          Rose::StringUtility::FileWithLineNumbers buildStringForTraverseMemoryPoolSource            ( AstNodeClass & node );

       // DQ & JH (1/17/2006): Added support for building code to check pointers to IR nodes
          Rose::StringUtility::FileWithLineNumbers buildStringForCheckingIfDataMembersAreInMemoryPoolSource ( AstNodeClass & node );
          Rose::StringUtility::FileWithLineNumbers buildStringToTestPointerForContainmentInMemoryPoolSource ( AstNodeClass & node );

       // AS (2/14/06): Added support for building code to return data member pointers to IR nodes
          Rose::StringUtility::FileWithLineNumbers buildStringForReturnDataMemberPointersSource ( AstNodeClass & node );

          Rose::StringUtility::FileWithLineNumbers buildStringForProcessDataMemberReferenceToPointersSource ( AstNodeClass & node );

       // DQ (3/7/2007): support for getChildIndex member function
          Rose::StringUtility::FileWithLineNumbers buildStringForGetChildIndexSource ( AstNodeClass & node );

          bool buildConstructorParameterList ( AstNodeClass & node, 
                                               std::vector<GrammarString *> & constructorParameterList,
                                               ConstructParamEnum config );
          std::string buildConstructorParameterListString ( AstNodeClass & node, bool withInitializers, bool withTypes, ConstructParamEnum config, bool *complete = 0 );
          std::string buildConstructorParameterListStringForEssentialDataMembers(AstNodeClass& node, bool withInitializers);

       // DQ 11/6/2006): Support for building newer from of constructors (withouth source position information).
          void markNodeForConstructorWithoutSourcePositionInformation ( AstNodeClass & node );
          void markNodeForConstructorWithoutSourcePositionInformationSupport( AstNodeClass & node );
          Rose::StringUtility::FileWithLineNumbers buildConstructorWithoutSourcePositionInformation ( AstNodeClass & node );
          void buildConstructorWithoutSourcePositionInformationSupport( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );
          void constructorLoopBody(const ConstructParamEnum& config, bool& complete, const Rose::StringUtility::FileWithLineNumbers& constructorSourceCodeTemplate, AstNodeClass& node, Rose::StringUtility::FileWithLineNumbers& returnString);

       // DQ (11/7/2006): Get a specific node from the tree.
          AstNodeClass* getNamedNode ( AstNodeClass & node, const std::string & name );
          GrammarString* getNamedDataMember ( AstNodeClass & node, const std::string & name );

       // DQ (3/24/2006): Separated these functions so that we could position the data member 
       // variable declaration at the base of the class and the generated access functions at 
       // the top.  This permist us to present the documentation better using Doxygen.
       // string buildDataPrototypesAndAccessFunctionPrototypesAndConstuctorPrototype ( GrammarTreeNode & node );
          Rose::StringUtility::FileWithLineNumbers buildDataMemberVariableDeclarations ( AstNodeClass & node );
          Rose::StringUtility::FileWithLineNumbers buildMemberAccessFunctionPrototypesAndConstuctorPrototype ( AstNodeClass & node );

          Rose::StringUtility::FileWithLineNumbers buildConstructor ( AstNodeClass & node );
          Rose::StringUtility::FileWithLineNumbers buildConstructorForEssentialDataMembers ( AstNodeClass & node);

          Rose::StringUtility::FileWithLineNumbers buildCopyMemberFunctionSource ( AstNodeClass & node );

          void buildSourceFiles ( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );

       // DQ (12/23/2005): Added to support moving the new and elete operators to a 
       // different file (mostly for clarity to support users who read the source code).
          void buildNewAndDeleteOperators( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );

       // DQ (12/24/2005): Support for memory pool traversal
          void buildTraverseMemoryPoolSupport( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );

       // DQ & JH (1/17/2006): Added support for building code to check pointers to IR nodes
          void buildStringForCheckingIfDataMembersAreInMemoryPoolSupport( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );

       // AS(2/14/2006): Added support for building code to return pointers to IR nodes
          void buildStringForReturnDataMemberPointersSupport ( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );

       // DQ (4/30/2006): Support for buildStringForReturnDataMemberReferenceToPointersSource
       // JJW (11/1/2008): Changed to process rather than return the references
          void buildStringForProcessDataMemberReferenceToPointersSupport( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );

       // DQ (12/23/2005): Relocated copy function to a separate file to imrove readability of source code
          void buildCopyMemberFunctions( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );

       // DQ (3/7/2007): support for getChildIndex member function
          void buildStringForGetChildIndexSupport( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );

       // Uses list of targets and sources stored within each node to drive substitutions
          Rose::StringUtility::FileWithLineNumbers editSubstitution ( AstNodeClass & node, const Rose::StringUtility::FileWithLineNumbers& editString );

       // This calls the functions to build the parsers (one for each child of the root node)
          void buildGrammarClassSourceCode ( Rose::StringUtility::FileWithLineNumbers & outputFile );

          static Rose::StringUtility::FileWithLineNumbers extractStringFromFile ( const std::string& startMarker, const std::string& endMarker,
                                                     const std::string& filename, const std::string& directory );

          void printTreeNodeNames ( const AstNodeClass & node ) const;
       // void setUpParentNames ();
       // void setUpParentNames ( AstNodeClass & X );

       // MK: The following member function is no longer needed:
       // void traverseTree ( AstNodeClass & X );

       // These functions build AstNodeClass and nonterminal objects to be associated with this grammar
       // Using a member function to construct these serves several purposes:
       // 1) organizes terminals and nonterminals with there respective grammar (without ambiguity)
       // 2) avoids or deferes the implementation of the envelop/letter interface mechanism so
       //    that the letter will have a scope longer than the envelope
          AstNodeClass & nonTerminalConstructor ( const std::string& lexeme, Grammar& X, const std::string& stringVar, const std::string& tagString, const SubclassListBuilder & builder, bool canHaveInstances = false );
          AstNodeClass    & terminalConstructor ( const std::string& lexeme, Grammar & X, const std::string& stringVar = "", const std::string& tagString = "" );

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
          std::string buildParserPrototype ( AstNodeClass & node ); 
          std::string buildParserSource    ( AstNodeClass & node ); 

       // a more general way to traverse the Grammar and build strings for source code generation

       // MS: Grammar Traversal defs
          class GrammarSynthesizedAttribute
             {
               public:
                    AstNodeClass* grammarnode;
                    std::string text;
                    std::string nodetext;
                    std::string terminalname;
                    std::string nonterminalsbunch;
                    std::string terminalsbunch;
                    std::string problematicnodes;
                    bool isTerminal;
             };

          typedef std::string (Grammar::*evaluateStringAttributeFunctionType)(AstNodeClass&, std::string);
          std::string naiveTraverseGrammar(AstNodeClass&, evaluateStringAttributeFunctionType);
          typedef GrammarSynthesizedAttribute (Grammar::*evaluateGAttributeFunctionType)(AstNodeClass*, 
                                                                                         std::vector<GrammarSynthesizedAttribute>);
          // filtered member variables are all those that are not required to create a vailid AST (or can computed by other means)
          // this function is used by the ATerm grammar generation and when generating constructors for all data members (excluding filtered vars)
          bool isFilteredMemberVariable(std::string varName);
          // MS: type hierarchy traversal
          GrammarSynthesizedAttribute BottomUpProcessing(AstNodeClass* node, evaluateGAttributeFunctionType evaluateGAttributeFunction);
          // MS: build a Latex output that shows the abstract Cpp grammar
          GrammarSynthesizedAttribute CreateGrammarDotString(AstNodeClass* grammarnode,
                                                               std::vector<GrammarSynthesizedAttribute> v);
          GrammarSynthesizedAttribute CreateAbstractTreeGrammarString(AstNodeClass* grammarnode,
                                                               std::vector<GrammarSynthesizedAttribute> v);
          void buildGrammarDotFile(AstNodeClass* rootNode, std::ostream& GrammarDotFile);
          void buildAbstractTreeGrammarFile(AstNodeClass* rootNode, std::ostream& AbstractTreeGrammarFile);
          void buildSDFTreeGrammarFile(AstNodeClass* rootNode, std::ostream& RoseTreeGrammarFile);

          // MS: generate source for implementation of the RTI interface
          GrammarSynthesizedAttribute generateRTIImplementation(AstNodeClass* grammarnode,
                                                                 std::vector<GrammarSynthesizedAttribute> v);

          // JJW (10/16/2008): Factor this code into a separate header file
          void buildStringForMemoryPoolSupport(AstNodeClass* rootNode, Rose::StringUtility::FileWithLineNumbers& file);
          void buildStringForMemoryPoolSupportSource(AstNodeClass* rootNode, Rose::StringUtility::FileWithLineNumbers& file);
          GrammarSynthesizedAttribute generateMemoryPoolSupportImplementation(AstNodeClass* grammarnode, std::vector<GrammarSynthesizedAttribute> v);
          GrammarSynthesizedAttribute generateMemoryPoolSupportImplementationSource(AstNodeClass* grammarnode, std::vector<GrammarSynthesizedAttribute> v);

          // MS: auxiliary function
          std::string typeStringOfGrammarString(GrammarString* gs);

          // temporary: used as return type by getGrammarNodeInfo
          class GrammarNodeInfo 
             {
               public:
                    GrammarNodeInfo():numContainerMembers(0), numSingleDataMembers(0) {}
                    unsigned int numContainerMembers;
                    unsigned int numSingleDataMembers;
             };

       // MS: auxiliary function to make tests on GrammarNodes more compact. (should be made a set of member
       //     functions of GrammarNode at some point (requires many other functions to be moved as well)
          GrammarNodeInfo getGrammarNodeInfo(AstNodeClass* grammarnode);

       // MS: generates the code to implement the creation of the treeTraversalSuccessorContainer in Sage
          void buildTreeTraversalFunctions(AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile);

       // DQ (10/4/2014): Adding ATerm support to be automatically generated via ROSETTA.
       // Rasmussen (04/17/2019): Support for ATerms has been deprecated.
#define BUILD_ATERM_SUPPORT 0
#if BUILD_ATERM_SUPPORT
          void buildAtermSupportFunctions(AstNodeClass& node, Rose::StringUtility::FileWithLineNumbers& outputFile);
          void buildAtermGenerationSupportFunctions(AstNodeClass& node, Rose::StringUtility::FileWithLineNumbers& outputFile);
          void buildAtermConsumerSupportFunctions(AstNodeClass& node, Rose::StringUtility::FileWithLineNumbers& outputFile);
          void buildDataMember(AstNodeClass & node, GrammarString* grammarString, bool & firstAterm, bool & firstConstructorParameter, 
                               bool & lastDataMemberWasConstructorParameter, bool & isInConstructorParameterList, 
                               std::string & constructorArgumentsString, std::string & atermArgumentsSubstring, std::string & atermPatternSubstring, 
                               std::string & dataMemberString, std::string & dataMemberString_post, int integer_counter );
          Rose::StringUtility::FileWithLineNumbers buildAtermConstructor ( AstNodeClass & node );
          void buildAtermBuildFunctionsSourceFile( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );
          bool isIntegerKind(const std::string & typenameString);
       // void generateAtermSupport(GrammarString* gs, StringUtility::FileWithLineNumbers & outputFile);
#endif // BUILD_ATERM_SUPPORT

       // MS: create source code for enums used in traversal functions to access synthesized attribute values
          void buildEnumForNode(AstNodeClass& node, std::string& enumString);
          std::string EnumStringForNode(AstNodeClass& node,std::string s);

       // MS: build Variants for use in tree traversal (access with getVariantT())
          std::string buildVariantEnums();

       // Build functions for boost::serialization support
          void buildSerializationSupport(std::ostream &decls, std::ostream &defns, const std::string &headerName);

       // PC: build ReferenceToPointerHandler and related classes
          std::string buildReferenceToPointerHandlerCode();

       // Milind Chabbi(8/28/2013): Performance refactoring.
       // Support for table-driven SgXXX node castability.

       // Generates a table (classHierarchyCatTable) populated with information
       // about whether a given SgXXX node can be casted to a SgYYY node.
       // The technique has constant lookup time.
          std::string generateClassHierarchyCastTable();

       // Populates the classHierarchyCastTable with all the types that can be casted to AstNodeClass type
          void buildClassHierarchyCastTable(AstNodeClass * astNodeClass, std::vector<AstNodeClass*> & myParentsDescendents);

       // Gets the number of rows in classHierarchyCastTable
          size_t getRowsInClassHierarchyCastTable();

       // Gets the number of columns in classHierarchyCastTable
          size_t getColumnsInClassHierarchyCastTable();

       // AS: build the function to get the class hierarchy subtree 
          std::string buildClassHierarchySubTreeFunction();

       // AS: build the funtion to automatically generate the memory pool based traversal on VariantVectors
          std::string buildMemoryPoolBasedVariantVectorTraversalSupport();

       // MS: build VariantEnumnames 
          std::string buildVariantEnumNames();

       // DQ (11/27/2005): Support for generation of code to be used for automating name changes and interface fixes in ROSE.
          std::string buildTransformationSupport();

       // MS: creates Include list (used by buildImplementationForTerminal and travMemberAccessEnums)
          std::vector<GrammarString*> classMemberIncludeList(AstNodeClass& node);

       // MK: This member function is used by the member function buildTreeTraversalFunctions()
       // (see above) in order to determine if the current node of the grammar corresponds
       // to a grammar class whose objects may actually occur in an AST.
          bool isAstObject(AstNodeClass& node);

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
          static std::vector<GrammarFile*> fileList;
          std::string restrictedTypeStringOfGrammarString(GrammarString* gs, AstNodeClass* grammarnode, std::string grammarSymListOpPrefix, std::string grammarSymListOpPostfix);
          std::set<std::string> traversedTerminals;
          GrammarSynthesizedAttribute CreateMinimalTraversedGrammarSymbolsSet(AstNodeClass* grammarnode, std::vector<GrammarSynthesizedAttribute> v);
          bool isAbstractTreeGrammarSymbol(AstNodeClass*);
          bool isAbstractTreeGrammarSymbol(std::string);
   private:
          bool generateSDFTreeGrammar;
          std::set<std::string> sdfTreeGrammarContainerTypes;
   // JH (01/13/2006) Added to build code for ast file IO
     public:
       // JH (11/04/2005): declaration of methods, needed to build the code for the
       // StorageClasses, after generation of the code located in SageIII/astFileIO/StorageClasses.C
          std::string myBuildHeaderStringAfterMarker  ( const std::string& marker, const std::string& fileName );

          void buildStorageClassSourceFiles ( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );
          void buildStorageClassHeaderFiles ( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );
          void buildIRNodeConstructorOfStorageClassSource ( AstNodeClass & node, Rose::StringUtility::FileWithLineNumbers & outputFile );
          std::string buildStringForStorageClassSource ( AstNodeClass & node );
          std::string buildStorageClassDeclarations ( );

       // DQ (4/6/2006): Added in Jochen's new version
          std::string buildStaticDataMemberListClassEntries( AstNodeClass & node);
          std::string buildAccessFunctionsOfClassEntries( AstNodeClass & node);
          std::string buildAccessFunctionSources( AstNodeClass & node);
          std::string buildDataMemberStorageClass( AstNodeClass & node);
          std::string buildStaticStorageClassPickOutSource( AstNodeClass & node);
          std::string generateStaticDataConstructorSource(AstNodeClass & node);
          std::string generateStaticDataWriteEasyStorageDataToFileSource(AstNodeClass & node);
          std::string generateStaticDataReadEasyStorageDataFromFileSource(AstNodeClass & node);
          std::string generateStaticDataArrangeEasyStorageInOnePoolSource(AstNodeClass & node);
          std::string generateStaticDataDeleteEasyStorageMemoryPoolSource(AstNodeClass & node);
          std::string buildStaticDataMemberListSetStaticDataSource(AstNodeClass & node);
          std::string buildStaticDataMemberListDeleteStaticDataSource(AstNodeClass & node);
          std::string buildStaticDataMemberListClassConstructor(AstNodeClass & node);

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
          std::string outputClassesAndFields ( AstNodeClass & node );
   private:
          bool nameHasPrefix(std::string name, std::string prefix);

   };

#endif // endif for ROSETTA_GRAMMAR_H
