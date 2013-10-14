
class ROSE_DLL_API HighLevelRewrite : public MidLevelRewrite<HighLevelInterfaceNodeCollection>
   {
  // Interface classification:
  //      Permits String Based Specification of Transformation: YES
  //      Permits Relative Specification of Target: YES
  //      Contains State information: YES

     public:
       // *****************************************************************

       // Note about Doxygen: Documentation for enums must appear in the header file.

      //! The AST Rewrite Mechanism's internal tree traversal mechanism 
      //! (makes the user's use of the rewrite mechanism as simple as possible)
          template <class RewriteInheritedAttribute, class RewriteSynthesizedAttribute>
          class RewriteTreeTraversal
             : public SgTopDownBottomUpProcessing<RewriteInheritedAttribute,RewriteSynthesizedAttribute>
             {
            //! Specialized tree traversal for use by transformations (tree traversals that 
            //! use the rewrite mechanism).

               public:
                    virtual RewriteInheritedAttribute
                         evaluateRewriteInheritedAttribute (
                            SgNode* astNode,
                            RewriteInheritedAttribute inheritedValue ) = 0;

                    virtual RewriteSynthesizedAttribute
                    evaluateRewriteSynthesizedAttribute (
                       SgNode* astNode,
                       RewriteInheritedAttribute inheritedValue,
                       typename RewriteTreeTraversal<RewriteInheritedAttribute,RewriteSynthesizedAttribute>::SubTreeSynthesizedAttributes attributList ) = 0;

               public:
                 // Functions required by the global tree traversal mechanism (must be public?)
                    RewriteInheritedAttribute
                    evaluateInheritedAttribute (
                       SgNode* astNode,
                       RewriteInheritedAttribute inheritedValue );

                    RewriteSynthesizedAttribute
                    evaluateSynthesizedAttribute (
                       SgNode* astNode,
                       RewriteInheritedAttribute inheritedValue,
                       typename RewriteTreeTraversal<RewriteInheritedAttribute,RewriteSynthesizedAttribute>::SubTreeSynthesizedAttributes attributList );
             };

      //! Class representing synthesized attribute for use in tree traversal
      //  class SynthesizedAttribute : public MiddleLevelRewrite::CollectionType
          class ROSE_DLL_API SynthesizedAttribute : public HighLevelInterfaceNodeCollection
             {
            //! This class attempts to provide an interface definition for all Synthesised attributes.  We are
            //! still searching for what would represent a base class set of abstractions for the difinition of
            //! all possible (or a reasonable subset) of abstractions supporting the specification of
            //! transformations.

            //! This class represents the string abstraction required for storing the list of strings for
            //! declarations, initializations, for insertion into different scopes. For each scope we store a
            //! list of strings so that redundant strings (or strings redundant upto an integer
            //! (psuedo-redundant strings as defined and handled by the ROSE/util/string_functions.C code)) can
            //! be identified and eliminated.  This avoids multiply defined variables when multiple
            //! transformations are applied.

               public:
                 // Enum typedef types defined in the ASTNodeCollection
                 // typedef HighLevelInterfaceNodeCollection::PlacementPositionEnum PlacementPositionEnum;

                 //! List of strings associated with each scope (ordered from current scope backward to global
                 //! scope). We use the list of strings so that we can identify uniqueness (or
                 //! pseudo-uniqueness). An STL map based on ScopeIdentifierEnum, PlacementPositionEnum,
                 //! SourceCodeStringEnum might be a better container than an STL list or vector of lists.
                 //  AST_FragmentStringContainer astFragmentStringContainer;

                 // Declarations required to compile the source code string to generate an AST fragment (which
                 // can be used to edit the original AST). We might still need this.
                 // list<string> supportingDeclarationsForSourceCodeStringList;

                 //! Strings that form declarations and initializations can be sorted and redundent strings
                 //! removed, but strings that build the main body of transformations can not be sorted else we
                 //! generate nonsense.  This variable preserves the order of the strings in the list (not
                 //! premiting the removal of redundant strings).  This is useful in the specification of the
                 //! main body of the transformation (loop structures etc.).
                    bool preserveStringOrder;

                 //! Link to associated node in the AST
                 //  SgNode* associated_AST_Node;

               public:
                 //! Constructors and destructors
                    virtual ~SynthesizedAttribute();

                 //! This is required by the tree traversal mechanism (only used by the tree traversal mechanism)
                    SynthesizedAttribute();

                 //! This is the constructor that we use
                    SynthesizedAttribute ( SgNode* inputASTNode );

                    SynthesizedAttribute ( const SynthesizedAttribute & X );
                    SynthesizedAttribute & operator= ( const SynthesizedAttribute & X );

                 // *********************************************************
                 //  Public (Published) Interface for Synthesized Attribute
                 // *********************************************************

                //! These must be defined to take a SgNode and not a SgStatement since 
                //! they can be called from the insert function on any expression within 
                //! a statement under the High-Level Rewrite Interface.
                    void insert  ( SgNode* astNode,
                         const std::string & transformation,
                         ScopeIdentifierEnum inputRelativeScope,
                         PlacementPositionEnum inputRelativeLocation );
                    void replace ( SgNode* astNode,
                         const std::string & transformation,
                         ScopeIdentifierEnum inputRelativeScope = SurroundingScope );

                 // This should not be specified as part of this interface 
                 // (it is defined in the HighLevelRewrite class)
                 // void remove  ( SgStatement* target );

                 // *********************************************************

                    virtual bool containsStringsForThisNode ( SgNode* astNode );

                 //! The details of the aggrigation of attributes is abstracted away in to an overloaded
                 //! operator+= member function
                    SynthesizedAttribute & operator+= ( const SynthesizedAttribute & X );

                 //! generate a display string of the information in the attribute (useful for debugging)
                    bool isEmpty() const;

                 //! Return all the strings associated with a particular scope or node in the AST and
                 //! for a specific position in the scoep or relative to the AST node.
                    std::list<std::string> getStringsSpecificToScopeAndPositionInScope(
                         SgNode* astNode, HighLevelInterfaceNodeCollection::PlacementPositionEnum positionInScope ) const;

                 //! Output a single string representing all the string data concatinated together
                 //! (with "[" and "]" separating different types of data.
                    std::string displayString() const;

                 //! Memeber functions called by derived classes to simplify the implementation 
                 //! of the evaluateSynthesizedAttribute() member function.
                    template <class ListOfChildSynthesizedAttributes>
                    void mergeChildSynthesizedAttributes( ListOfChildSynthesizedAttributes & list );

                    void rewriteAST( SgNode* astNode );
             };

     private:

          static void permitedLocationToInsertNewCode (
               SgNode* astNode, 
               bool & insertIntoGlobalScope,
               bool & insertIntoFunctionScope );

          static void insertContainerOfListsOfStatements ( 
               SgNode* astNode, 
               const HighLevelInterfaceNodeCollection & X );

          static void insertStatementList (
                           const SgStatementPtrList & X,
                           SgNode* targetNode,
                           IntermediateFileStringPositionEnum positionInScope );

     public:

       // We set the default to generate #include directives instead of all the 
       // declarations from all system files (which can sometimes fail to unparse).
       // Note that the HighLevelInterface::generatePrefixAndSuffix() differs from 
       // the MidLevelInterface::generatePrefixAndSuffix().
          static void generatePrefixAndSuffix (
                           SgNode* astNode,
                           std::string & globalPrefixString, 
                           std::string & localPrefixString, 
                           std::string & suffixString, 
                        // bool generateIncludeDirectives = true,
                        // bool prefixIncludesCurrentStatement = false );
                           bool generateIncludeDirectives,
                           bool prefixIncludesCurrentStatement );

       // public (and published) interface member functions (can be used from synthesized attributes as well)
          static void remove  ( SgStatement* target );
   };

