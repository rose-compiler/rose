#ifndef AST_REWRITE_H
#define AST_REWRITE_H

// #include STL_ALGO_HEADER_FILE
#include <algorithm>

#include "AstProcessing.h"
#include "AstReverseProcessing.h"
#include "AstClearVisitFlags.h"

/* Work to do:
    Convert the mixed inheritance and template model to use only templates!
A simpler interface would be to avoid forcing the user to define an inherited and 
synthesized attribute derived from the AST_Rewrite system.  The approach would be to:
     1) Template the AST_Rewrite inherited and synthesized attributes to include the user's attribute.
     2) We need a higher level function than AST_Rewrite::addSourceCodeString(,,,) that does NOT take
        the inherited and synthesized attributes but takes the otehr parameters.  Then the AST_Rewrite
        traversal can call the AST_Rewrite::addSourceCodeString(,,,) function internally.
     3) User can't call traverse() or traversInputFiles().  This permits the name of the function to start 
        the rewrite to be more specific (e.g. rewrite() or rewriteInputFiles()).  
     4) We could consider private or protected inheritance of the Rewrite traversal 
        from the AST processing traversal.
     5) Can't think of anything else ...

Advantages:
   1) Stepenov will be happy.
   2) This permits any type to be used as an inherited or synthesized attribute.
   3) Makes it easier to use the AST rewrite mechanism (makes it closer to the ASTProcessing)
   4) Allows the interface function to be a name specific to the derived class (e.g. rewrite class)
      (instead of it always being called traverse() or traverseInputFiles()).
 */

// include "projectQuery.h"
#include "transformationSupport.h"

// Require header file for AstPreorderTraversal class used to reset (clean) internal visit flags
// #include "AstPreorderTraversal.h"

#if 0
// DQ (8/19/2004): Moved to ROSE/src/util/stringSupport/string_functions.C
// DQ (9/23/2003)
// Added global function for getting the string associated 
// with an enum (which is defined in global scope)
std::string getVariantName ( VariantT v );
#endif

class MidLevelCollectionTypedefs
   {
  // Collection of typedefs to define the Mid Level Interface

     public:
          typedef enum ScopeIdentifier_Enum
             {
               unknownScope           =  0,
//             LocalScope             =  1 /*!< location of current statement being transformed */,
               StatementScope         =  1 /*!< location of current statement being transformed */,
               SurroundingScope       =  2 /*!< location of current statement being transformed */,
               Preamble               =  3 /*!< preamble ( #include<foo.h> for compilation of intermediate file */ ,
               LAST_SCOPE_TAG
             } ScopeIdentifierEnum;

       // The specification of placement in scope effects the prefix if the current 
       // position in the AST is at a declaration. This permits replacement 
       // declarations to redefine the variables in the current declaration. 
       // This detail needs to be tested later.
          typedef enum PlacementPosition_Enum
             {
               unknownPositionInScope  = 0 /*!< unknown used as a default value for error checking */ ,
               PreamblePositionInScope = 1 /*!< Source code to be placed at the top of a specified scope */ ,
               TopOfCurrentScope       = 2 /*!< Top of scope (current location must be a scope)  */ ,
               BeforeCurrentPosition   = 3 /*!< Before  */ ,
               ReplaceCurrentPosition  = 4 /*!< Replace */ ,
               AfterCurrentPosition    = 5 /*!< After   */ ,
               BottomOfCurrentScope    = 6 /*!< Bottom of scope (current location must be a scope)  */ ,
               LAST_PLACEMENT_TAG
             } PlacementPositionEnum;

       // Enum used to classify different types of transformation strings.
          typedef enum IntermediateFileStringPosition_Enum
             {
               unknownIntermediatePositionInScope  =  0 /*!< Value of undefined location within intermediate file */,
               GlobalScopePreamble                 =  1 /*!< Required declarations (and include files) */,
#if 1
               CurrentLocationTopOfScope           =  2 /*!< Top (only appies if current location is a scope)  */ ,
               CurrentLocationBefore               =  3 /*!< Source code to be placed at the current node */, 
               CurrentLocationReplace              =  4 /*!< Source code to be placed at the current node */, 
               CurrentLocationAfter                =  5 /*!< Source code to be placed at the current node */, 
               CurrentLocationBottomOfScope        =  6 /*!< Bottom (only appies if current location is a scope) */ ,
#else
               CurrentLocationBefore               =  2 /*!< Source code to be placed at the current node */, 
               CurrentLocationAfter                =  3 /*!< Source code to be placed at the current node */, 
#endif
               LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG
             } IntermediateFileStringPositionEnum;

      //! debugging support
      /*! @{
          \if documentDevelopmentVersionUsingDoxygen
               \name Debugging support
               \brief Supporting functions ... incomplete-documentation
          \endif
       */
      /*! \if documentDevelopmentVersionUsingDoxygen
               \brief useful for debugging
          \endif
       */
           static std::string stringAttribute (
               ScopeIdentifierEnum scope,
               PlacementPositionEnum positionInScope );

      /*! \if documentDevelopmentVersionUsingDoxygen
               \brief useful for debugging
          \endif
       */
          static std::string getRelativeScopeString( ScopeIdentifierEnum inputRelativeScope );

      /*! \if documentDevelopmentVersionUsingDoxygen
               \brief useful for debugging
          \endif
       */
          static std::string getRelativeLocationString( PlacementPositionEnum inputRelativeLocation );

          static std::string getIntermediateFilePositionName ( const IntermediateFileStringPositionEnum location );
      //! @}
   };

class HighLevelCollectionTypedefs
   {
  // Collection of typedefs to define the High Level Interface

     public:
       // We don't want to support the use of the StatementScope since it adds redundency to the 
       // specification of a transformation which could lead to the same declaration being inserted 
       // twice into the same position (at the top or bottom of a list of statements) which would 
       // generate bad code.  Our rewrite mechanism presently assures that if the transformation 
       // compiles then only good code is generated (from the high level interface, at least, but 
       // almost so for the mid level interface as well (redundent insertions of a declaration 
       // before any statement would pass generate bad code even though the transformation would 
       // work properly)).  The argument for the correctness of the generated code is stronger 
       // than the argument for symetry between the mid-level and high-level interfaces.
          typedef enum ScopeIdentifier_Enum
             {
               unknownScope           =  0,
//             LocalScope             =  1 /*!< location of current statement being transformed */,
//             StatementScope         =  1 /*!< scope of statement being transformed (applies only if it is a 
//                                              statement containing a list; e.g. SgGlobal, SgClassDeclaration) */,
               SurroundingScope       =  1 /*!< scope of current statement being transformed */,
               ParentScope            =  2 /*!< parent scope of the current statement being transformed */ ,
               NestedLoopScope        =  3 /*!< parent scope of set of nested for loops */ ,
               NestedConditionalScope =  4 /*!< parent scope of set of nested for loops */ ,
               FunctionScope          =  5 /*!< scope associated with current function */ ,
               FileScope              =  6 /*!< file scope (similar to global scope associated with current file) */ ,
               GlobalScope            =  7 /*!< global scope ( extern declarations in the file scope) */ ,
               Preamble               =  8 /*!< preamble ( #include<foo.h> for compilation of intermediate file */ ,
               LAST_SCOPE_TAG
             } ScopeIdentifierEnum;

       // The specification of placement in scope effects the prefix if the current 
       // position in the AST is at a declaration. This permits replacement 
       // declarations to redefine the variables in the current declaration. 
       // This detail needs to be tested later.
          typedef enum PlacementPosition_Enum
             {
               unknownPositionInScope  = 0 /*!< unknown used as a default value for error checking */ ,
               PreamblePositionInScope = 1 /*!< Source code to be placed at the top of a specified scope */ ,
               TopOfScope              = 2 /*!< Source code to be placed at the top of a specified scope */ ,
               TopOfIncludeRegion      = 3 /*!< Typically used for #include CPP declarations specified just after last #include */ ,
               BottomOfIncludeRegion   = 4 /*!< Typically used for #include CPP declarations specified just before first function definition */ ,
               BeforeCurrentPosition   = 5 /*!< Before  */ ,
               ReplaceCurrentPosition  = 6 /*!< Replace */ ,
               AfterCurrentPosition    = 7 /*!< After   */ ,
               BottomOfScope           = 8 /*!< Source code to be placed at the top of a specified scope (but before any return!) */ ,
               LAST_PLACEMENT_TAG
             } PlacementPositionEnum;

           typedef enum IntermediateFileStringPosition_Enum
             {
               unknownIntermediatePositionInScope  =  0 /*!< Value of undefined location within intermediate file scope */ ,
               GlobalScopePreamble                 =  1 /*!< Required declarations (and include files) to support transformations (added to top of file) */ ,
               GlobalScopeTopOfScope               =  2 /*!< Source code to be placed at the top of the global scope (top of file) */ ,
               GlobalScopeTopOfIncludeRegion       =  3 /*!< Used for #include CPP declarations specified just before first #include */ ,
               GlobalScopeBottomOfIncludeRegion    =  4 /*!< Used for #include CPP declarations specified just before first function definition */ ,
               GlobalScopeBeforeCurrentPosition    =  5 /*!< Global Scope Before  */ ,
               GlobalScopeReplaceCurrentPosition   =  6 /*!< Global Scope Replace */ ,
               FunctionScopePreamble               =  7 /*!< Required declarations (and include files) to support transformations (added to top of file) */ ,
               FunctionScopeTopOfScope             =  8 /*!< Function Scope Top  */ ,
               FunctionScopeBeforeCurrentPosition  =  9 /*!< Function Scope Before  */ ,
               FunctionScopeReplaceCurrentPosition = 10 /*!< Function Scope Replace */ ,
               FunctionScopeAfterCurrentPosition   = 11 /*!< Function Scope After   */ ,
               FunctionScopeBottomOfScope          = 12 /*!< Botton of function scope (but before any return!) */ ,
               GlobalScopeAfterCurrentPosition     = 13 /*!< Global Scope After   */ ,
               GlobalScopeBottomOfScope            = 14 /*!< Bottom of file */ ,
               LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG
             } IntermediateFileStringPositionEnum;

      //! debugging support
      /*! @{
          \if documentDevelopmentVersionUsingDoxygen
               \name Debugging support
               \brief Supporting functions ... incomplete-documentation
          \endif
       */
      /*! \if documentDevelopmentVersionUsingDoxygen
               \brief useful for debugging
          \endif
       */
           static std::string stringAttribute (
               ScopeIdentifierEnum scope,
               PlacementPositionEnum positionInScope );

      /*! \if documentDevelopmentVersionUsingDoxygen
               \brief useful for debugging
          \endif
       */
          static std::string getRelativeScopeString( ScopeIdentifierEnum inputRelativeScope );

      /*! \if documentDevelopmentVersionUsingDoxygen
               \brief useful for debugging
          \endif
       */
          static std::string getRelativeLocationString( PlacementPositionEnum inputRelativeLocation );

          static std::string getIntermediateFilePositionName ( const IntermediateFileStringPositionEnum location );
      //! @}
   };

// Class representing container for string and relative position for string in the AST
template < class InterfaceCollectionTypedefs>
class TransformationStringTemplatedType // : public string
   {
 //! Design Point: This class could be derived from the string class (is this
 //! worthwhile?). No! Derivation from the C++ string class obscures a better
 //! implementation later that would include additional lists of strings containing types
 //! (which would be used to support construction of header files to compile the
 //! intermediate source strings).

     public:
          typedef typename InterfaceCollectionTypedefs::ScopeIdentifierEnum   ScopeIdentifierEnum;
          typedef typename InterfaceCollectionTypedefs::PlacementPositionEnum PlacementPositionEnum;

      //! Member Data indicating relative position
          ScopeIdentifierEnum   relativeScope;
          PlacementPositionEnum relativeLocation;

       // SourceCodeStringClassificationEnum sourceCodeClassification;

      //! data string to insert into AST
          std::string sourceCode;

#if 0
      /*! \brief Supporting type to allow automated generation of new types used in a 
                 transformation.

          This class supports the declaration of new types and member function prototypes
          for us in the automated construction of header files to support the compilation
          of the intermediate strings associated with the specification of
          transformations.
       */
          class TypeDeclaration
             {
           //! contains class name and list of member functions
               public:
                    std::string className;
                    std::list<std::string> memberFunctionList;

                   ~TypeDeclaration ();
                    TypeDeclaration ( std::string className );
                    void addMemberFunction ( std::string memberFunction );
             };

      //! Support for automated construction of header files to compile intermediate
      //! source strings.
          std::list<TypeDeclaration> typeList;
          std::list<std::string> globalFunctionList;
#endif

      //! Handle declaration of strings for inclusion in their own scope, an option to the
      //! positioning of new strings via (ScopeIdentifierEnum X PlacementPositionEnum).  Strings
      //! defined with (buildInNewScope == true) will be introduced into the AST into a new scope (with
      //! surrounding "{ }").
          bool buildInNewScope;

      //! location in AST with which to associate the string 
      //! (computed using current position and relative offset)
      //! Note: NULL value indicates current position (permits simpler interface for mid level interface)
          SgNode* associatedASTNode;

      //! Permits mid level interface to specific a string only and not an 
      //! associated SgNode pointer while permiting a strings inforcement 
      //! of non-NULL pointers for the high level interface.
          bool currentPosition;

      //! Member Functions
         ~TransformationStringTemplatedType();
          TransformationStringTemplatedType( const TransformationStringTemplatedType & X );
          TransformationStringTemplatedType & operator= ( const TransformationStringTemplatedType & X );

      //! Constructor permitting specification of string with scope, position in scope, and
      //! classification using a string plus defaults
          TransformationStringTemplatedType (
               SgNode*               astNode,
               const std::string &   X,
               ScopeIdentifierEnum   relativeScope,
               PlacementPositionEnum relativeLocation,
               bool                  buildInNewScope = false );

      /*! \brief Function to enforce preconditions (constraints) on the 
                 values used within the constructor's interface.

          Note that we automatically change some of the input specifications to permit the 
          simplified automatic testing of all the permutations associated with new strings 
          added to all locations in a scope and for all scopes. Some combinations of 
          relative scope and relative location simply do not make sense!.  This function
          modifies the scope and location so that they make sense.
       */
          void preconditions(
               ScopeIdentifierEnum & relativeScope,
               PlacementPositionEnum & relativeLocation,
               bool & inputBuildInNewScope );

          TransformationStringTemplatedType operator+= ( const TransformationStringTemplatedType & X );
          TransformationStringTemplatedType operator+  ( const TransformationStringTemplatedType & X );

          ScopeIdentifierEnum getRelativeScope() const;
          PlacementPositionEnum getRelativePosition() const;
          std::string getSourceCodeString() const;
          std::string getRelativeScopeString() const;
          std::string getRelativeLocationString() const;

          bool matchingAttributes ( const TransformationStringTemplatedType & X ) const;

      //! check for same scope (matches objects with the nodes where their strings will be inserted)
          bool isSameScope( SgNode* targetAstNode ) const;
          bool isTargetedForHigherLevelScope( SgNode* inputAstNode ) const;

      //! general display dump of data for debugging
          void display( std::string label = "" ) const;

          bool   getBuildInNewScope() const;

      //! allows STL sort operator to be called
          bool operator== ( const TransformationStringTemplatedType & X ) const;

#if 1
      //! Get the requested scope of the input node
      //! This must be defined to take a SgNode and not a SgStatement since 
      //! they can be called from the insert function on any expression within 
      //! a statement under the High-Level Rewrite Interface.
          SgScopeStatement* getScope ( SgNode* astNode, ScopeIdentifierEnum relativeScope );

      //! Get the position (a statement) in the AST required to trigger the transformation
      //! This must be defined to take a SgNode and not a SgStatement since 
      //! they can be called from the insert function on any expression within 
      //! a statement under the High-Level Rewrite Interface.
          SgStatement* getAssociatedStatement ( 
               SgNode* astNode,
               ScopeIdentifierEnum relativeScope,
               PlacementPositionEnum relativeLocation );
#else
      //! Get the requested scope of the input node
          SgScopeStatement* getScope ( SgStatement* statement, ScopeIdentifierEnum relativeScope );

      //! Get the position (a statement) in the AST required to trigger the transformation
          SgStatement* getAssociatedStatement ( 
               SgStatement* statement,
               ScopeIdentifierEnum relativeScope,
               PlacementPositionEnum relativeLocation );
#endif
   };

template < class InterfaceTypedefs >
class AbstractInterfaceNodeCollection : public InterfaceTypedefs
   {
  // This class contains the lists of input transformation strings, sorted strings, 
  // and associated AST statements associated with the sorted strings.
  // The order of processing is as follows:
  //    input transformation strings (relative) 
  //         --> sorted strings (absolute and sorted by position in scope) 
  //         --> AST statement nodes (sorted by position inscope)

  // DQ (9/4/2005): When compiling with g++ 3.4.x this can't be protected (I think)
  // protected:
     public:
       InterfaceTypedefs::Preamble;
     public:
        InterfaceTypedefs::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG;

       // Used by MidLevel and High Level Interface as a base class to make enum values 
       // easly available without the class specifiers. (E.g. LocalScope instead of 
       // MidLevelInterfaceNodeCollection::LocalScope) requires derivation from 
       // MidLevelInterfaceNodeCollection::InterfaceTypedefEnums -- CAN'T MAKE THIS WORK!!!
       // typedef InterfaceTypedefEnums InterfaceTypedefs;

       // The marker strings in the middle level interface are different from 
       // those in the high level interface (this can't be const since it brings out a bug with g++)
       // typedef const string MarkerCollectionType [LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG][2];
       // pmp 08JUN05
       //   changed to cope with changes in template processing in gcc 3.4
       //   was: typedef string MarkerCollectionType [LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG][2];
          typedef std::string MarkerCollectionType [InterfaceTypedefs::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG][2];
          static MarkerCollectionType markerStrings;

       // List of string transformations (length 1 for absolute interface (mid level interface), 
       // and arbitrary for relative interface (high level interface).
       // typedef list<string> StringCollectionType [LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG];
       // typedef vector< list<string> > StringCollectionType;
       // typedef vector< list<TransformationStringType> > StringCollectionType;
//        typedef list<TransformationStringType> StringCollectionType;

          typedef TransformationStringTemplatedType<InterfaceTypedefs> TransformationStringType;
          typedef std::vector<TransformationStringType> InputTransformationStringCollectionType;
          InputTransformationStringCollectionType inputTransformationStrings;

       // Sorted strings are the separated collections of strings organized 
       // by position in current scope and the global scope.
          typedef std::vector< std::list<std::string> > SortedTransformationStringCollectionType;
          SortedTransformationStringCollectionType sortedTransformationStrings;

       // List of statements generated from processing of intermediate file containing
       // transformation strings.
       // typedef SgStatementPtrList NodeCollectionType [LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG];
          typedef std::vector<SgStatementPtrList> NodeCollectionType;

       // Transformation strings (more than one can be specified at one time)
//        StringCollectionType transformationStrings;

       // AST subtrees generated from transformation strings (position in list communicates 
       // scope, location, and possible removal of current subtree being traversed).
//        NodeCollectionType sortedStatementNodes;
          NodeCollectionType treeFragement;

       // Mapping of IntermediateFileStringPositionEnum baack to the AST for the current 
       // position in the AST. At any position of the AST, statement lists generated within 
       // the specification of a transformation can be inserted at the top or bottom of a 
       // scope, or ahead or behind the current position.   This vector would be set when 
       // the transformation strings are sorted. An alternative would be a reverse 
       // mapping function from IntermediateFileStringPositionEnum back to a PlacementPositionEnum 
       // (though the parent scope might have to be found directly to support insertion at 
       // top or bottom of scope).  Not sure which is best???
       // pmp 08JUN05
       //   changed to cope with changes in template processing in gcc 3.4
       //   was: typedef SgNode* PositionInASTVectorType [LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG];
          typedef SgNode* PositionInASTVectorType [InterfaceTypedefs::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG];
          PositionInASTVectorType positionInASTVector;

          AbstractInterfaceNodeCollection();
          virtual ~AbstractInterfaceNodeCollection();

          AbstractInterfaceNodeCollection ( const AbstractInterfaceNodeCollection & X );
          AbstractInterfaceNodeCollection & operator =( const AbstractInterfaceNodeCollection & X );
//        virtual AbstractInterfaceNodeCollection & operator+=( const AbstractInterfaceNodeCollection & X ) = 0;


//        void addString ( const string & s );
//        void addString ( SgNode*, const TransformationStringType & s );
          void addString ( SgNode* astNode, const TransformationStringType & s );

       // Sum of sizes of all lists (higher level support)
          virtual int numberOfTransformationStrings();
          virtual int numberOfSortedTransformationStrings();
          virtual int numberOfStatements();

       // Lower level support for individual lists
          virtual int numberOfSortedTransformationStrings( int i );
          virtual int numberOfStatements( int i );

//        static string positionName ( int i );

       // Access function for vector of lists of AST statements nodes
          SgStatementPtrList getStatementList ( typename InterfaceTypedefs::IntermediateFileStringPositionEnum i );

          void display ( const char* s ) const;
          void display ( const std::string & s ) const;

      //! Preamble strings are often required to define variables used in the 
      //! compilation of intermediate files (e.g. global variables that might 
      //! be required by a transformation).  Strings not specific to the 
      //! transformation specified at the current scope.
          std::list<std::string> getPreambleStrings(
                    SgNode* astNode, 
                    typename InterfaceTypedefs::ScopeIdentifierEnum lowerBoundScope, 
                    typename InterfaceTypedefs::ScopeIdentifierEnum upperBoundScope ) const;

          static bool isInGlobalScope ( SgNode* astNode );

      //! Virtual functions that define the interface for operations on strings to produce AST transformations
          virtual void sortInputStrings ( SgNode* astNode ) = 0;
          virtual std::string generateIntermediateFileString ( SgNode* astNode, bool prefixIncludesCurrentStatement ) = 0;
          virtual void compileSortedStringsToGenerateNodeCollection ( SgNode* astNode, bool prefixIncludesCurrentStatement ) = 0;

#if 0
          virtual void insertStatementList ( 
                            const SgStatementPtrList & X,
                            SgNode* targetNode,
                            typename InterfaceTypedefs::IntermediateFileStringPositionEnum positionInScope ) = 0;
#endif

#if 0
      //! Inserts the list minus markers whos attachments are moved to the surrounding nodes of the target
          void insertStatementListWithMarkers ( 
                            SgStatement* target, 
                            SgStatementPtrList statementList, 
                            typename InterfaceTypedefs::IntermediateFileStringPositionEnum filePosition );
#endif

          static AttachedPreprocessingInfoType getPreprocessingInfoList ( 
                    SgStatement* statement, 
                    PreprocessingInfo::RelativePositionType position );

       // DQ (1/11/2006): Added support for marking the statement list as a transformation
          static void markStatementListWithMarkersAsTransformation ( SgStatementPtrList & statementList );

          static AttachedPreprocessingInfoType processCommentsAndDirectives ( SgStatementPtrList & statementList );

          static void stripOffMarkerDeclarations ( SgStatementPtrList & statementList );

          void consistancyCheck( const std::string & label ) const;
   };

class MidLevelInterfaceNodeCollection : public AbstractInterfaceNodeCollection<MidLevelCollectionTypedefs>
   {
     public:
          MidLevelInterfaceNodeCollection();
          virtual ~MidLevelInterfaceNodeCollection();

          MidLevelInterfaceNodeCollection ( const MidLevelInterfaceNodeCollection & X );
          MidLevelInterfaceNodeCollection & operator =( const MidLevelInterfaceNodeCollection & X );
          MidLevelInterfaceNodeCollection & operator+=( const MidLevelInterfaceNodeCollection & X );

          void sortInputStrings ( SgNode* astNode );

          void compileSortedStringsToGenerateNodeCollection ( SgNode* astNode, bool prefixIncludesCurrentStatement );

       // Supporting function for stringToStatementList() member function
       // Must take SgNode so that the prefix can be computed (position dependent)
          static NodeCollectionType stringToNodeCollection ( SgNode* astNode, std::string transformationString );

          std::string generateIntermediateFileString ( SgNode* astNode, bool prefixIncludesCurrentStatement );

          void insertContainerOfListsOfStatements ( SgStatement* astNode );

          static void insertStatementList ( 
                           SgStatement* target, 
                           const SgStatementPtrList & X,
                           AttachedPreprocessingInfoType & commentsAndDirectives,
                           IntermediateFileStringPositionEnum filePosition );

#if 1
          static void insertStatementListWithMarkers ( 
                           SgStatement* target, 
                           SgStatementPtrList statementList, 
                           IntermediateFileStringPositionEnum filePosition );
#endif
          static void removeCommentsFromMarkerDeclarationsAndReattachToTarget ( 
                           SgVariableDeclaration* varDeclaration, 
                           SgStatement* target,
                           IntermediateFileStringPositionEnum filePosition );

          static bool isAMarkerString ( std::string s );

          void writeAllChangeRequests ( SgStatement* target, bool prefixIncludesCurrentStatement );

     private:
//        static string positionName ( IntermediateFileStringPositionEnum i );

       // Mapping defined in the mid-level interface is simpler than in the high-level interface
          static IntermediateFileStringPositionEnum map ( ScopeIdentifierEnum scope, PlacementPositionEnum location );

          static std::string wrapMarkersAroundTransformationString ( 
                           std::string transformationString, 
                           IntermediateFileStringPositionEnum filePosition );
   };

class HighLevelInterfaceNodeCollection : public AbstractInterfaceNodeCollection<HighLevelCollectionTypedefs>
   {
     public:
       // Constants used to dimension arrays (read only constants) and simplify the code
          static const int numberOfScopes;
          static const int numberOfPositionsInScope;
          static const int numberOfIntermediatFileSourceCodePositions;

          HighLevelInterfaceNodeCollection();
          virtual ~HighLevelInterfaceNodeCollection();

          HighLevelInterfaceNodeCollection ( const HighLevelInterfaceNodeCollection & X );
          HighLevelInterfaceNodeCollection & operator =( const HighLevelInterfaceNodeCollection & X );
          HighLevelInterfaceNodeCollection & operator+=( const HighLevelInterfaceNodeCollection & X );

          void sortInputStrings ( SgNode* astNode );

          void compileSortedStringsToGenerateNodeCollection ( SgNode* astNode, bool prefixIncludesCurrentStatement );

          std::string generateIntermediateFileString ( SgNode* astNode, bool prefixIncludesCurrentStatement );

          static void insertStatementList ( 
                           SgStatement* target, 
                           const SgStatementPtrList & X,
                           AttachedPreprocessingInfoType & commentsAndDirectives,
                           IntermediateFileStringPositionEnum positionInScope );

          static void insertStatementListWithMarkers ( 
                           SgStatement* target, 
                           SgStatementPtrList statementList, 
                           IntermediateFileStringPositionEnum filePosition );

          void insertContainerOfListsOfStatements ( SgStatement* astNode );

          static bool isAMarkerString ( std::string s );

          void writeAllChangeRequests ( SgStatement* target, bool prefixIncludesCurrentStatement );

     private:
//        static string positionName ( IntermediateFileStringPositionEnum i);

       // Need a map function to translate the relative location into a position in the intermediate file
          static IntermediateFileStringPositionEnum map ( 
                      SgNode* astNode, 
                      PlacementPositionEnum location );
#if 0
          static PlacementPositionEnum reverseMap ( 
                      SgStatement* target, 
                      IntermediateFileStringPositionEnum filePosition );
#endif
          void permitedLocationToInsertNewCode ( 
                    SgNode* astNode, 
                    bool & insertIntoGlobalScope,
                    bool & insertIntoFunctionScope );
#if 0
      //! Remove comments and CPP directives from marker variables 
      //! and reassociate them with the statments between markers 
      //! or the target statement directly.
          static void removeCommentsFromMarkerDeclarationsAndReattachToTarget ( 
                            SgVariableDeclaration* varDeclaration, 
                            SgStatement* target,
                            IntermediateFileStringPositionEnum filePosition );
#endif
   };

#include "lowLevelRewriteInterface.h"
#include "midLevelRewriteInterface.h"

typedef MidLevelRewrite<MidLevelInterfaceNodeCollection> MiddleLevelRewrite;

#include "highLevelRewriteInterface.h"

#if 0
class SAGE_Rewrite
   {
  // Not sure if we need this class since this stands for the SAGE interface 
  // for insert/replace/remove of children from any parent node.  For now this
  // just helps us think about the whole interface (even if this part of it is 
  // implemented separately in SAGE.

  // Interface classification:
  //      Permits String Based Specification of Transformation: NO
  //      Permits Relative Specification of Target: NO
  //      Contains State information: NO

  // Note: replace alway has the semantics of 
  //      insert(currentNode,newNode); 
  //      remove(currentNode); 
  // but can be implemented more efficently.

     public:
          static void insert  ( SgStatement* targetStatement, SgNode* newStatement, bool insertBeforeNode = true );
          static void replace ( SgStatement* targetStatement, SgNode* newStatement );
          static void remove  ( SgStatement* targetStatement );

          static void insert  ( SgStatement* targetStatement, SgStatementPtrList newStatementList, bool insertBeforeNode = true );
          static void replace ( SgStatement* targetStatement, SgStatementPtrList newStatmentList );
          static void remove  ( SgStatementPtrList targetStatementList );
   };
#endif

// endif for AST_REWRITE_H
#endif


























