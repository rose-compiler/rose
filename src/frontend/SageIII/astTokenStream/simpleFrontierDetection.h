

#ifndef SIMPLE_FRONTIER_DETECTION_HEADER
#define SIMPLE_FRONTIER_DETECTION_HEADER

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
// simply unparse the whole file from the token stream starting
// at the SgSourceFile, since this would not reflect transformations
// and might not reflect parts of the AST for which there is some
// lack of precise enough information in the mapping of the token 
// stream to the AST  (to support this feature of unparsing from 
// the token stream).

// This SIMPLE version is an attempt to build a more precise unparsing
// which is simpler and which will divide the unparsing from the 
// token stream into pieces that surround the subtree that must be
// unparsed from the AST (because it was transformed).

class SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute
   {
     public:
       // Save a reference to the associated source file so that we can get the filename to compare against.
          SgSourceFile* sourceFile;

       // Detect when to stop processing deeper into the AST.
          bool processChildNodes;

          bool isFrontier;

          bool unparseUsingTokenStream;
          bool unparseFromTheAST;
          bool containsNodesToBeUnparsedFromTheAST;

       // DQ (12/1/2013): Support specific restrictions in where frontiers can be placed.
       // For now: avoid class declarations in typedefs using a mixture of unparsing from tokens and unparsing from the AST.
       // bool isPartOfTypedefDeclaration;
       // For now: avoid unparing the SgIfStmt from the AST and the conditional expression/statement from the token stream.
       // bool isPartOfConditionalStatement;

       // Specific constructors are required
          SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute();

       // DQ (11/13/2018): Added constructor.
          SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile);

          SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end,bool processed);

          SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute ( const SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute & X ); // : processChildNodes(X.processChildNodes) {};

   };

   
class SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute
   {
     public:
          SgStatement* node;

          bool isFrontier;

          bool unparseUsingTokenStream;
          bool unparseFromTheAST;
          bool containsNodesToBeUnparsedFromTheAST;
          bool containsNodesToBeUnparsedFromTheTokenStream;

       // std::vector<SgStatement*> frontierNodes;
       // std::vector<FrontierNode*> frontierNodes;

          SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute();
          SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n);

          SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute(const SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute & X);
   };


class SimpleFrontierDetectionForTokenStreamMapping : public SgTopDownBottomUpProcessing<SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute,SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute>
   {
     public:
          int numberOfNodes;

          SimpleFrontierDetectionForTokenStreamMapping( SgSourceFile* sourceFile);

       // virtual function must be defined
          SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute evaluateInheritedAttribute(SgNode* n, SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute);

       // virtual function must be defined
          SimpleFrontierDetectionForTokenStreamMapping_SynthesizedAttribute evaluateSynthesizedAttribute (SgNode* n, 
               SimpleFrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute, 
               SubTreeSynthesizedAttributes synthesizedAttributeList );

       // This is used to test the random association of AST node to either token unparsing or AST unparsing.
          int numberOfNodesInSubtree(SgSourceFile* sourceFile);
   };


void simpleFrontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile );


#endif



