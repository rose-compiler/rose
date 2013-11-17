
class FrontierDetectionForTokenStreamMapping_InheritedAttribute
   {
     public:
       // Same a reference to the associated source file so that we can get the filename to compare against.
          SgSourceFile* sourceFile;

       // Detect when to stop processing deeper into the AST.
          bool processChildNodes;

       // Specific constructors are required
          FrontierDetectionForTokenStreamMapping_InheritedAttribute();
          FrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end,bool processed);

          FrontierDetectionForTokenStreamMapping_InheritedAttribute ( const FrontierDetectionForTokenStreamMapping_InheritedAttribute & X ); // : processChildNodes(X.processChildNodes) {};
   };


class FrontierDetectionForTokenStreamMapping_SynthesizedAttribute
   {
     public:
         SgNode* node;

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
