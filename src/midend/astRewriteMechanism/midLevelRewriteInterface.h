#include <stack>
// Note: CODE IMPROVEMENT IDEA
//      We could specify an additional template argument here and permit the Enum values 
//      to be defined within the MidLevelRewrite class and avoid the awkward use of class 
//      specifiers (e.g. LocalScope instead of MidLevelInterfaceNodeCollection::LocalScope)

// class AbsoluteStringRewrite
template <class ASTNodeCollection>
class ROSE_DLL_API MidLevelRewrite
   {
  // Interface classification:
  //      Permits String Based Specification of Transformation: YES
  //      Permits Relative Specification of Target: NO
  //      Contains State information: NO

     public:
       // *****************************************************************
       //         Typedefs used internally and dependent upon the 
       //         typedefs defined in the template parameter
       // *****************************************************************

       // List of string transformations (length 1 for absolute interface (mid level interface), 
       // and arbitrary for relative interface (high level interface).
          typedef typename ASTNodeCollection::TransformationStringType TransformationStringType;
          typedef typename ASTNodeCollection::InputTransformationStringCollectionType InputTransformationStringCollectionType;

       // These are the lists of strings sorted by their position in scope
          typedef typename ASTNodeCollection::SortedTransformationStringCollectionType SortedTransformationStringCollectionType;

       // List of marker strings (variable names that are used to separate transformation strings 
       // in the generated intermediate file (used to convert the strings into AST nodes).
          typedef typename ASTNodeCollection::MarkerCollectionType MarkerCollectionType;

       // List of statements generated from processing of intermediate file containing 
       // transformation strings.
          typedef typename ASTNodeCollection::NodeCollectionType NodeCollectionType;

       // Enum used to classify different types of transformation strings by scope, location, action (insert, replace).
          typedef typename ASTNodeCollection::IntermediateFileStringPositionEnum IntermediateFileStringPositionEnum;

       // Enum typedef types defined in the ASTNodeCollection (relative specification of a scope)
          typedef typename ASTNodeCollection::ScopeIdentifierEnum   ScopeIdentifierEnum;

       // Enum typedef types defined in the ASTNodeCollection (location in a scope)
          typedef typename ASTNodeCollection::PlacementPositionEnum PlacementPositionEnum;

//        typedef typename ASTNodeCollection::TransformationStringType TransformationStringType;

       // Access to the template type
          typedef ASTNodeCollection CollectionType;

       // *****************************************************************

       // *****************************************************************
       // Classes supporting the generation of the prefix and suffix 
       // strings used to define all program variables and permit 
       // compilation of transformation strings.
       // *****************************************************************

       // I think that we don't need either an inherited or synthesized attribute in the 
       // PrefixTraversal but 
       // during development I have switched the location of some program code from the 
       // evaluate synthized attribute and back such that it was easier to keep both in 
       // place for now.  Future (final) work might eliminate both in favor of a visitor 
       // based traversal.
          class PrefixInheritedAttribute   {};
          class PrefixSynthesizedAttribute {};

      /*! \brief This class contains all declarations and an interface for their access

          This class is used in the Rewrite::generatePrefixAndSuffixStrings() member function. 
          It holds a stack of lists of
          pointers to SgDeclarationStatements and embedded comments.  The stack is used to record the scope (there 
          could be any number of scopes) and the list is to record the declarations (there 
          could be any number of declarations).  This class supports the generation of a string containing
          all declarations or optionally #includes (and other required CPP directives) for use in the 
          intermediate files generated to compile the transformations when input as strings.
       */
          class PrefixSuffixGenerationTraversal : 
             public AstReversePrefixInhSynProcessing<PrefixInheritedAttribute,PrefixSynthesizedAttribute>
             {
               public:
                 // build the stack of lists using data from each scope on the way back so that
                 // we can later optimize the process by depositing the list at each node on the 
                 // way down to the leaves of the AST. Later we will have a stack< list<SgStatement*> > 
                 // data structure as an attribute.  Then we only have to traverse up the tree to 
                 // the root until we find a previously deposited attribute and then push one stack 
                 // onto the other.

                    class DeclarationOrCommentListElement
                       {
                         public:
                           // We need to collect declarations AND comments (which might appear
                           // between non-declarations in non-global scopes) not attached to 
                           // declarations.
                           // SgDeclarationStatement* declaration;

                           // We define a prefix statement as any statement that is
                           // processed (except for extracting only comments) as part 
                           // of constructing the prefix.  This includes all declarations, 
                           // and also scope statements such as SgSwitchStatement so
                           // that cases can be introduced with the correct context.  
                           // It also includes SgForStatements since they can include 
                           // declarations (although these could be handled by just 
                           // producing the declarations that they contain as an alternative).
                              SgNode* prefixStatement;
                              AttachedPreprocessingInfoType* comments;
                              bool validDeclarationOrCommentListElement;

                           // For debugging it is helpful to know where the comments were attached
                              SgNode* originatingStatement;

                              DeclarationOrCommentListElement ( SgStatement* astNode, bool isInSameScope );

                              std::string generateString ( int & openingIfCounter, 
                                                           int & closingEndifCounter , 
                                                           bool generateIncludeDirectives, 
                                                           bool skipTrailingDirectives,
                                                           bool unparseAsDeclaration );

                              void display( std::string s ) const;

                          //! Many DeclarationOrCommentListElement objects are built, but they are 
                          //! only valid if they have a non NULL pointer to either a SgNode or 
                          //! AttachedPreprocessingInfoType that we want to save for the prefix 
                          //! generation.
                              bool isValidDeclarationOrCommentListElement() const;
                       };

                    typedef std::list < DeclarationOrCommentListElement > ListOfStatementsType;
                    typedef std::stack< ListOfStatementsType > StackOfListsType;
                 // Argument to evaluateSynthesizedAttribute()
                    typedef typename AstReversePrefixInhSynProcessing<PrefixInheritedAttribute,PrefixSynthesizedAttribute>::SynthesizedAttributesList SynthesizedAttributesList;

                 // Keep track of the current scope separately and push the current 
                 // scope onto the stack of scopes.
                    ListOfStatementsType currentScope;
                    StackOfListsType stackOfScopes;

                 // SgStatement* parentStatement;
                 // SgStatement* previousStatement;
                 // SgStatement* previousScope;
                    SgScopeStatement* previousScope;

                 // Record the current statment so that we can determine if the previous 
                 // statement (if it is sought) is in the same scope or not.
                 // SgStatement* initialStatement;

                 // Holds the evolving stack of scopes which changes (grows and 
                 // shrinks during the reverse traversal of the AST from where 
                 // the prefix is computed).
                 // StackOfListsType temporaryScopeStack;

                 // Control of optional generation of #include "header.h" instead of 
                 // the declarations contained in the "header.h" files.
                    bool generateIncludeDirectives;

                 // Keep track of the number of #if and #endif so that they match up
                 // (force them to match up by adding a trailing #endif if required).
                    int openingIfCounter;
                    int closingEndifCounter;

                    PrefixSuffixGenerationTraversal ( bool generateIncludeDirectives = true );

                    std::string generateEndifForPrefixString( /* int openingIfCounter, int closingEndifCounter */ );

                //! This is not a const function since it modifies the openingIfCounter 
                //! and closingEndifCounter stored internally.
                    std::string generatePrefixStringFromStatementList(
                                ListOfStatementsType & statementList,
                                bool skipStatementListTrailingDirectives, 
//                              int & openingIfCounter,
//                              int & closingEndifCounter,
                                bool globalScopeAndContainsAdditionalScopes );

                //! Used for Mid-Level Interface
                    std::string generatePrefixString();

                //! Used for High-Level Interface
                 // Not certain why we have to divide the prefix into two parts 
                 // (though this is done currently).
                 // I think it is because "{" can't appear in the global scope.
                 // These function modify the openingIfCounter and closingEndifCounter variables.
                    std::string generatePrefixStringGlobalDeclarations();
                    std::string generatePrefixStringLocalDeclarations();

                //! Used for both mid and high level interfaces
                    std::string generateSuffixString();

                    void display( std::string s ) const;

                    void display (const ListOfStatementsType & s, std::string displayString ) const;
                    void display (StackOfListsType s, std::string displayString ) const;

                 // We define the functions required to overwrite the virtual functions in the 
                 // base class AstReversePrefixInhSynProcessing::evaluateSynthesizedAttribute()
                    PrefixInheritedAttribute evaluateInheritedAttribute (
                         SgNode* astNode,
                         PrefixInheritedAttribute inputInheritedAttribute );

                    PrefixSynthesizedAttribute evaluateSynthesizedAttribute (
                         SgNode* astNode,
                         PrefixInheritedAttribute inputInheritedAttribute,
                         SynthesizedAttributesList inputSynthesizedAttributeList );
             };

       // *****************************************************************

       // *********************************************************************
       // Classes required to define the traversal of the AST generated from the 
       // intermediate file to extract the collection of AST nodes that will be 
       // inserted into the AST.
       // *********************************************************************
          class AST_FragmentIdentificationInheritedAttributeType
             {
            // Inherited attribute required for the traversal of the AST of the intermediate 
            // file passed to the front-end to compile the transformation strings.

               public:
                    bool treeFragmentFromCorrectFile;
                    std::string currentFileName;

                   ~AST_FragmentIdentificationInheritedAttributeType();
                    AST_FragmentIdentificationInheritedAttributeType();
                    AST_FragmentIdentificationInheritedAttributeType(
                         const AST_FragmentIdentificationInheritedAttributeType & X );

                    AST_FragmentIdentificationInheritedAttributeType &
                         operator=( const AST_FragmentIdentificationInheritedAttributeType & X );
             };

       // forward declaration
          class AST_FragmentIdentificationSynthesizedAttributeType;

       // Tree traversal class for seperation of tree fragments
          class AST_FragmentIdentificationTraversal
             : public SgTopDownBottomUpProcessing<AST_FragmentIdentificationInheritedAttributeType,
                                                  AST_FragmentIdentificationSynthesizedAttributeType>
             {
            // This traversal operates on the AST generated from the intermediate file used to 
            // compile the transformation strings.  It separates out from the AST all the subtrees
            // associated with the different input transformation strings.  Since multiple 
            // transformation strings can be compiled together, multiple subtrees (AST fragments) 
            // are separated out of intermediate file's AST.

               public:
                 // Argument to evaluateSynthesizedAttribute()
                    typedef typename SgTopDownBottomUpProcessing<AST_FragmentIdentificationInheritedAttributeType, AST_FragmentIdentificationSynthesizedAttributeType>::SubTreeSynthesizedAttributes SubTreeSynthesizedAttributes;

                 // I don't think we need this data member for this traversal (but I'm trying to get
                 // everything to compile and having trouble linking, so we will see it this helps).
//                  SgProject & project;
//                  AST_FragmentIdentificationTraversal( SgProject & project);

                   ~AST_FragmentIdentificationTraversal();
                    AST_FragmentIdentificationTraversal();

                 // false when not between marker strings
                    bool recordStatements;

                 // Functions required by the global tree traversal mechanism
                    AST_FragmentIdentificationInheritedAttributeType
                    evaluateInheritedAttribute (
                       SgNode* astNode,
                       AST_FragmentIdentificationInheritedAttributeType inheritedValue );

                    AST_FragmentIdentificationSynthesizedAttributeType
                         evaluateSynthesizedAttribute (
                              SgNode* astNode,
                              AST_FragmentIdentificationInheritedAttributeType inheritedValue,
                              SubTreeSynthesizedAttributes synthesizedAttributeList );
             };

          class AST_FragmentIdentificationSynthesizedAttributeType
             {
            // This class is used as the synthesized attribute in the traversal to separate out the different
            // types of AST fragments that are build from strings specificed as beloging to different
            // locations within a single scope. The value of this class is that it can be either a marker or a
            // pointer to an AST fragment.  If it is a marker then isAMarker is true and the marker string
            // holds the name of the variable used to mark the start/end of a statement for a specific
            // location.

               public:
                    bool isAMarker;
                    std::string markerString;
                    SgStatement* treeFragment;

                 // ASTFragmentRequirementStrings::IntermediateFileStringPositionEnum

                 // Array of lists of SgStatement pointers for each possible location of source code strings
                 // within scopes.
                 // list<SgStatement*> treeFragementListArray[HighLevelRewrite::LAST_INTERMEDIATE_SOURCE_CODE_PLACEMENT_TAG];
                    NodeCollectionType treeFragementListArray;

                   ~AST_FragmentIdentificationSynthesizedAttributeType();
                    AST_FragmentIdentificationSynthesizedAttributeType();
                    AST_FragmentIdentificationSynthesizedAttributeType (
                         const typename AST_FragmentIdentificationTraversal::SubTreeSynthesizedAttributes synthesizedAttributeList );

                    AST_FragmentIdentificationSynthesizedAttributeType(
                         const AST_FragmentIdentificationSynthesizedAttributeType & X );

                    AST_FragmentIdentificationSynthesizedAttributeType &
                         operator=( const AST_FragmentIdentificationSynthesizedAttributeType & X );

                    void initializeNodeListToCorrectSize ();

#if 1
                    AST_FragmentIdentificationSynthesizedAttributeType &
                         operator+=( const AST_FragmentIdentificationSynthesizedAttributeType & X );
#endif
                 // Access functions
                    void setMarker ( std::string s );
                    std::string getMarker();
                    void setTreeFragment ( SgStatement* astNode );
                    SgStatement* getTreeFragment();
                    bool isStartingMarker();
                    bool isEndingMarker();

                    static bool isStartOrEndMarker( std::string markerString, int startOrEnd );

                 // return the location if this is a marker
//                  HighLevelRewrite::IntermediateFileStringPositionEnum location();
//                  ASTNodeCollection::IntermediateFileStringPositionEnum location();
                    IntermediateFileStringPositionEnum location();

                    void display ( std::string s ) const;

                    void consistancyCheck( const std::string & label ) const;
             };

     public:
       // Supporting function for stringToStatementList() member function
       // Must take SgNode so that the prefix can be computed (position dependent)
          static typename ASTNodeCollection::NodeCollectionType 
               fileStringToNodeCollection ( SgNode* astNode, std::string transformationString );

     public:
       // We set the default to generate #include directives instead of all the 
       // declarations from all system files (which can sometimes fail to unparse).
          static void generatePrefixAndSuffix (
                           SgNode* astNode,
                           std::string & prefixString,
                           std::string & suffixString,
                        // bool generateIncludeDirectives = true,
                        // bool prefixIncludesCurrentStatement = false );
                           bool generateIncludeDirectives,
                           bool prefixIncludesCurrentStatement );

      // pmp 08JUN05
      //    added qualifiers to SurroundingScope and BeforeCuurentPosition to deal w/ changes in gcc 3.4
      //    static void insert ( 
      //                     SgStatement* target,
      //                     const string & transformationString,
      //                     ScopeIdentifierEnum inputRelativeScope = SurroundingScope,
      //                     PlacementPositionEnum locationInScope = BeforeCurrentPosition );
      
      //! relative insertion (relative to the target statement, can't be used on global scope)
      //! This function could (and maybe should) be renamed "insertRelativeToStatement()"
          static void insert ( SgStatement* target,
                               const std::string & transformationString,
                               ScopeIdentifierEnum inputRelativeScope = ASTNodeCollection::SurroundingScope,
                               PlacementPositionEnum locationInScope  = ASTNodeCollection::BeforeCurrentPosition 
                             );

      //! replace member function implemented using insert member function
          static void replace ( SgStatement* target, const std::string & transformationString );

      //! remove member function implemented using lower level remove member function
          static void remove  ( SgStatement* target );

      //! Wraps macro calls so that they are understood in the intermediate files where 
      //! they specify transformations but not expanded until the final source code is 
      //! generated.
          static std::string postponeMacroExpansion ( std::string macroCall );

     private:
      //! Returns boolean value if AST node is appropriate for generating a prefix/suffix string
          static bool acceptableTargetForPrefix ( SgNode* astNode );

      //! Filters out the few statment types that are not supported (can't be supported or
      //! requires special handling which is not yet implemented).
          static bool insertSupported ( SgStatement* astNode, 
                                        MidLevelCollectionTypedefs::ScopeIdentifierEnum inputRelativeScope );

      //! Identifies IR nodes where StatementScope may be specified within 
      //! the insert function (IR node contains a list of statements).
          static bool insertUsingStatementScopeSupported ( SgStatement* astNode );
   };















