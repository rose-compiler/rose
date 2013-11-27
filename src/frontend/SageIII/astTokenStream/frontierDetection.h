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
       // Same a reference to the associated source file so that we can get the filename to compare against.
          SgSourceFile* sourceFile;

       // Detect when to stop processing deeper into the AST.
          bool processChildNodes;

          bool isFrontier;

          bool unparseUsingTokenStream;
          bool unparseFromTheAST;
          bool containsNodesToBeUnparsedFromTheAST;

       // Specific constructors are required
          FrontierDetectionForTokenStreamMapping_InheritedAttribute();
          FrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end,bool processed);

          FrontierDetectionForTokenStreamMapping_InheritedAttribute ( const FrontierDetectionForTokenStreamMapping_InheritedAttribute & X ); // : processChildNodes(X.processChildNodes) {};
   };


class FrontierNode
   {
  // These objects represent the frontier in the AST of where we have to unparse using either the token stream or the AST.
     public:
          SgStatement* node;

          bool unparseUsingTokenStream;
          bool unparseFromTheAST;

       // FrontierNode(SgStatement* n,bool unparseUsingTokenStream,bool unparseFromTheAST) : node(node), unparseUsingTokenStream(unparseUsingTokenStream), unparseFromTheAST(unparseFromTheAST)
          FrontierNode(SgStatement* n,bool unparseUsingTokenStream,bool unparseFromTheAST);

         std::string display();
   };
   

class FrontierDetectionForTokenStreamMapping_SynthesizedAttribute
   {
     public:
          SgStatement* node;

          bool isFrontier;

          bool unparseUsingTokenStream;
          bool unparseFromTheAST;
          bool containsNodesToBeUnparsedFromTheAST;
          bool containsNodesToBeUnparsedFromTheTokenStream;

       // std::vector<SgStatement*> frontierNodes;
          std::vector<FrontierNode*> frontierNodes;

          FrontierDetectionForTokenStreamMapping_SynthesizedAttribute();
          FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n);

          FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(const FrontierDetectionForTokenStreamMapping_SynthesizedAttribute & X);
   };


class FrontierDetectionForTokenStreamMapping : public SgTopDownBottomUpProcessing<FrontierDetectionForTokenStreamMapping_InheritedAttribute,FrontierDetectionForTokenStreamMapping_SynthesizedAttribute>
   {
     public:
          FrontierDetectionForTokenStreamMapping( SgSourceFile* sourceFile);

       // virtual function must be defined
          FrontierDetectionForTokenStreamMapping_InheritedAttribute evaluateInheritedAttribute(SgNode* n, FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute);

       // virtual function must be defined
          FrontierDetectionForTokenStreamMapping_SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, 
               FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute, 
               SubTreeSynthesizedAttributes synthesizedAttributeList );

   };


void frontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile );


class FrontierDetectionForTokenStreamMappingAttribute : public AstAttribute
   {
  // This class supports marking the AST in the normal ROSE AST graph generation.
  // We use this ROSE feature to mark the frontier of where the token stream will 
  // be unparsed vs. unparsing directly from the AST (e.g. for transformations).

     private:
          SgNode* node;
          std::string name;
          std::string options;

     public:

          FrontierDetectionForTokenStreamMappingAttribute(SgNode* n, std::string name, std::string options);

          FrontierDetectionForTokenStreamMappingAttribute(const FrontierDetectionForTokenStreamMappingAttribute & X);

       // Support for graphics output of IR nodes using attributes (see the DOT graph of the AST)
          virtual std::string additionalNodeOptions();
          virtual std::vector<AstAttribute::AttributeEdgeInfo> additionalEdgeInfo();
          virtual std::vector<AstAttribute::AttributeNodeInfo> additionalNodeInfo();

       // Support for the coping of AST and associated attributes on each IR node (required for attributes 
       // derived from AstAttribute, else just the base class AstAttribute will be copied).
          virtual AstAttribute* copy();
   };
