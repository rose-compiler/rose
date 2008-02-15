// Author: Markus Schordan
// $Id: AstReverseProcessing.h,v 1.3 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTREVERSEPROCESSING_H
#define ASTREVERSEPROCESSING_H

#include "AstProcessing.h"

//////////////////////////////////////////////////////////////////////////////////////////
// REVERSE PREFIX PROCESSING 
//////////////////////////////////////////////////////////////////////////////////////////

template<typename InheritedAttributeType>
class AstReversePrefixInhProcessing : public AstTopDownProcessing<InheritedAttributeType> {
 public:
  AstReversePrefixInhProcessing();
 protected:
  virtual void setNodeSuccessors(SgNode* node, typename AstReversePrefixInhProcessing<InheritedAttributeType>::SuccessorsContainer& succContainer);
};

template<class SynthesizedAttributeType>
class AstReversePrefixSynProcessing : public AstBottomUpProcessing<SynthesizedAttributeType> {
 public:
  AstReversePrefixSynProcessing();
 protected:
  virtual void setNodeSuccessors(SgNode* node, typename AstReversePrefixSynProcessing<SynthesizedAttributeType>::SuccessorsContainer & succContainer);
};

template<class InheritedAttributeType, class SynthesizedAttributeType>
class AstReversePrefixInhSynProcessing : public AstTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType> {
 public:
  AstReversePrefixInhSynProcessing();
 protected:
  virtual void setNodeSuccessors(SgNode* node, typename AstReversePrefixInhSynProcessing<InheritedAttributeType,SynthesizedAttributeType>::SuccessorsContainer & succContainer);
};

//////////////////////////////////////////////////////////////////////////////////////////
// REVERSE BRANCH PROCESSING 
//////////////////////////////////////////////////////////////////////////////////////////

template<class InheritedAttributeType, class SynthesizedAttributeType>
class AstReverseBranchInhSynProcessing : public AstTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType> {
 public:
  AstReverseBranchInhSynProcessing();
 protected:
  virtual void setNodeSuccessors(SgNode* node, typename AstReverseBranchInhSynProcessing<InheritedAttributeType,SynthesizedAttributeType>::SuccessorsContainer & succContainer);
};

template<class InheritedAttributeType>
class AstReverseBranchInhProcessing : public AstTopDownProcessing<InheritedAttributeType> {
 public:
  AstReverseBranchInhProcessing();
 protected:
  virtual void setNodeSuccessors(SgNode* node, typename AstReverseBranchInhProcessing<InheritedAttributeType>::SuccessorsContainer & succContainer);
};

template<class SynthesizedAttributeType>
class AstReverseBranchSynProcessing : public AstBottomUpProcessing<SynthesizedAttributeType> {
 public:
  AstReverseBranchSynProcessing();
 protected:
  virtual void setNodeSuccessors(SgNode* node, typename AstReverseBranchSynProcessing<SynthesizedAttributeType>::SuccessorsContainer & succContainer);
};

// #ifdef HAVE_EXPLICIT_TEMPLATE_INSTANTIATION 
  #include "AstReverseProcessing.C" 
// #endif 

#include "AstReverseSimpleProcessing.h"

#endif
