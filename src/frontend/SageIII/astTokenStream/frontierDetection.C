#include "rose.h"

// #include "general_token_defs.h"

#include "frontierDetection.h"

FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute()
   {
   }

FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute(SgSourceFile* input_sourceFile, int start, int end,bool processed)
   {
   }


FrontierDetectionForTokenStreamMapping_InheritedAttribute::
FrontierDetectionForTokenStreamMapping_InheritedAttribute ( const FrontierDetectionForTokenStreamMapping_InheritedAttribute & X )
   {
   }




FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute()
   {
   }

FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(SgNode* n)
   {
   }

FrontierDetectionForTokenStreamMapping_SynthesizedAttribute::
FrontierDetectionForTokenStreamMapping_SynthesizedAttribute(const FrontierDetectionForTokenStreamMapping_SynthesizedAttribute & X)
   {
   }


FrontierDetectionForTokenStreamMapping::
FrontierDetectionForTokenStreamMapping( SgSourceFile* sourceFile)
   {
   }


FrontierDetectionForTokenStreamMapping_InheritedAttribute
FrontierDetectionForTokenStreamMapping::evaluateInheritedAttribute(SgNode* n, FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute)
   {
     return FrontierDetectionForTokenStreamMapping_InheritedAttribute();
   }


FrontierDetectionForTokenStreamMapping_SynthesizedAttribute 
FrontierDetectionForTokenStreamMapping::FrontierDetectionForTokenStreamMapping::evaluateSynthesizedAttribute (SgNode* n, 
     FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute, 
     SubTreeSynthesizedAttributes synthesizedAttributeList )
   {
     return FrontierDetectionForTokenStreamMapping_SynthesizedAttribute();
   }


void
frontierDetectionForTokenStreamMapping ( SgSourceFile* sourceFile )
   {
     FrontierDetectionForTokenStreamMapping_InheritedAttribute inheritedAttribute;
     FrontierDetectionForTokenStreamMapping fdTraversal(sourceFile);

     fdTraversal.traverse(sourceFile,inheritedAttribute);
   }

