
#ifndef FRONTIER_DETECTION_HEADER
#define FRONTIER_DETECTION_HEADER

// DQ (12/1/2013): Added switch to control testing mode for token unparsing.
// Test codes in the tests/nonsmoke/functional/roseTests/astTokenStreamTests directory turn on this 
// variable so that all regression tests can be processed to mix the unparsing of 
// the token stream with unparsing from the AST.
extern ROSE_DLL_API bool tokenUnparsingTestingMode;

// The support for unparsing from the token stream is a feature in 
// ROSE to provide a new level of portability for the generated code.

// This support for frontier detection on the AST distinguishes 
// the subtrees that must be unparsed from the AST vs. those that
// may be unparsed from the token stream.  The solution is not to
// simplipy unparse the whole file from the token stream starting
// at the SgSourceFile, since this would not reflect transformations
// and might not reflect parts of the AST for which there is some
// lack of precise enough information in the mapping of the token 
// stream to the AST  (to support this feature of unparsing from 
// the token stream).

class FrontierDetectionForTokenStreamMapping_InheritedAttribute
   {
     public:
       // Save a reference to the associated source file so that we can get the filename to compare against.
          SgSourceFile* sourceFile;

       // Detect when to stop processing deeper into the AST.
          bool processChildNodes;

          bool isFrontier;

          bool unparseUsingTokenStream;
          bool unparseFromTheAST;
       // bool containsNodesToBeUnparsedFromTheAST;

       // DQ (5/11/2021): Added to support header file unparsing.
          bool isInCurrentFile;
          SgNode* node;

       // DQ (5/23/2021): Added to support C++.
          bool isPartOfTemplateInstantiation;

       // DQ (5/12/2021): Added to support header file unparsing.
       // int subtree_color_index;
       // std::string subtree_color;

       // DQ (12/1/2013): Support specific restrictions in where frontiers can be placed.
       // For now: avoid class declarations in typedefs using a mixture of unparsing from tokens and unparsing from the AST.
       // bool isPartOfTypedefDeclaration;
       // For now: avoid unparing the SgIfStmt from the AST and the conditional expression/statement from the token stream.
       // bool isPartOfConditionalStatement;

       // Specific constructors are required
          FrontierDetectionForTokenStreamMapping_InheritedAttribute();
       // FrontierDetectionForTokenStreamMapping_InheritedAttribute( SgSourceFile* file, SgNode* n, int color_index );
          FrontierDetectionForTokenStreamMapping_InheritedAttribute( SgSourceFile* file, SgNode* n );

       // DQ (5/11/2021): This is used to start the traversal.
          FrontierDetectionForTokenStreamMapping_InheritedAttribute( SgSourceFile* file );

#if 0
       // DQ (5/11/2021): I don't think this is used.
          FrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end,bool processed);
#endif

          FrontierDetectionForTokenStreamMapping_InheritedAttribute ( const FrontierDetectionForTokenStreamMapping_InheritedAttribute & X );

       // DQ (5/15/2021): Added operator=() support.
          FrontierDetectionForTokenStreamMapping_InheritedAttribute operator= ( const FrontierDetectionForTokenStreamMapping_InheritedAttribute & X );


       // DQ (5/11/2021): More appropriate function to determine when statements are in the source file.
          bool isNodeFromCurrentFile ( SgStatement* statement );
   };


class FrontierNode
   {
  // These objects represent the frontier in the AST of where we have to unparse using either the token stream or the AST.
     public:
          SgStatement* node;

          bool unparseUsingTokenStream;
          bool unparseFromTheAST;

       // DQ (5/16/2021): This data member is not used.
          bool redundant_token_subsequence;

       // FrontierNode(SgStatement* n,bool unparseUsingTokenStream,bool unparseFromTheAST) : node(node), unparseUsingTokenStream(unparseUsingTokenStream), unparseFromTheAST(unparseFromTheAST)
          FrontierNode(SgStatement* n,bool unparseUsingTokenStream,bool unparseFromTheAST);

         std::string display();

         FrontierNode(const FrontierNode & X);
         FrontierNode operator=(const FrontierNode & X);
   };
   

class FrontierDetectionForTokenStreamMapping_SynthesizedAttribute
   {
     public:
       // DQ (5/14/2021): I think that we need the sourceFile information to support header file unparsing.
          SgSourceFile* sourceFile;

          SgStatement* node;

          bool isFrontier;

          bool unparseUsingTokenStream;
          bool unparseFromTheAST;
          bool containsNodesToBeUnparsedFromTheAST;
          bool containsNodesToBeUnparsedFromTheTokenStream;

       // std::vector<SgStatement*> frontierNodes;
       // std::vector<FrontierNode*> frontierNodes;

          FrontierDetectionForTokenStreamMapping_SynthesizedAttribute();

       // FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n);
          FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n, SgSourceFile* file);

          FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(const FrontierDetectionForTokenStreamMapping_SynthesizedAttribute & X);

       // DQ (5/15/2021): Added operator=() support.
          FrontierDetectionForTokenStreamMapping_SynthesizedAttribute operator= (const FrontierDetectionForTokenStreamMapping_SynthesizedAttribute & X);
   };


class FrontierDetectionForTokenStreamMapping : public SgTopDownBottomUpProcessing<FrontierDetectionForTokenStreamMapping_InheritedAttribute,FrontierDetectionForTokenStreamMapping_SynthesizedAttribute>
   {
     public:
          int numberOfNodes;

       // DQ (5/13/2021): Adding accumulator attribute.
       // int filenameToColorCodeMap;

       // DQ (5/13/2021): Using a vector is not sufficient to support the most general cases (need a map).
       // DQ (5/13/2021): Save the set of supported filenames so that we can use the size of the set to 
       // distinquish the number of files supported (and the color code for different header files).
       // std::vector<std::string> supportedFilesList;
       // std::map<std::string,int> filenameToColorCodeMap;

       // DQ (5/16/2021): Moved the collection of FrontierNode to here from the FrontierDetectionForTokenStreamMapping_SynthesizedAttribute.
       // std::vector<FrontierNode*> frontierNodes;
       // std::map<int,std::vector<FrontierNode*> > frontierNodes;
       // std::map<int,std::map<SgStatement*,FrontierNode*> > frontierNodes;
          std::map<int,std::map<SgStatement*,FrontierNode*>* > frontierNodes;

          void addFrontierNode (SgStatement* statement, FrontierNode* frontierNode );
          FrontierNode* getFrontierNode (SgStatement* statement );
          void outputFrontierNodes();

          FrontierDetectionForTokenStreamMapping( SgSourceFile* sourceFile);

       // virtual function must be defined
          FrontierDetectionForTokenStreamMapping_InheritedAttribute evaluateInheritedAttribute(SgNode* n, FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute);

       // virtual function must be defined
          FrontierDetectionForTokenStreamMapping_SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, 
               FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute, 
               SubTreeSynthesizedAttributes synthesizedAttributeList );

       // This is used to test the random association of AST node to either token unparsing or AST unparsing.
          int numberOfNodesInSubtree(SgSourceFile* sourceFile);

       // DQ (5/11/2021): Added to better organize code (and support for unparsing headers).
          bool isChildNodeFromSameFileAsCurrentNode (SgStatement* statement, SgStatement* child_statement);
   };


#if 0
void frontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile );
#else
// DQ (5/10/2021): Activate this code.
void frontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile, bool traverseHeaderFiles );
#endif

class FrontierDetectionForTokenStreamMappingAttribute : public AstAttribute
   {
  // This class supports marking the AST in the normal ROSE AST graph generation.
  // We use this ROSE feature to mark the frontier of where the token stream will 
  // be unparsed vs. unparsing directly from the AST (e.g. for transformations).

     private:
       // DQ (3/25/2017): Remove to avoid Clang warning about unused private variable.
       // SgNode* node;

          std::string name;
          std::string options;

     public:

          FrontierDetectionForTokenStreamMappingAttribute(SgNode* n, std::string name, std::string options);

          FrontierDetectionForTokenStreamMappingAttribute(const FrontierDetectionForTokenStreamMappingAttribute & X);

       // Support for graphics output of IR nodes using attributes (see the DOT graph of the AST)
          virtual std::string additionalNodeOptions() override;
          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo() override;
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo() override;

       // Support for the coping of AST and associated attributes on each IR node (required for attributes 
       // derived from AstAttribute, else just the base class AstAttribute will be copied).
          virtual AstAttribute* copy() const override;

       // DQ (6/11/2017): Added virtual function now required to eliminate warning at runtime.
          virtual AstAttribute::OwnershipPolicy getOwnershipPolicy() const override; // { return CONTAINER_OWNERSHIP; }
   };

#endif
