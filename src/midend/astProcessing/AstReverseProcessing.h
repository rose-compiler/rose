// Author: Markus Schordan
// $Id: AstReverseProcessing.h,v 1.3 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTREVERSEPROCESSING_H
#define ASTREVERSEPROCESSING_H

// tps (01/08/2010) Added sage3basic since this doesnt compile under gcc4.1.2
//#include "sage3basic.h"
//#include "sage3.h"

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

// Author: Markus Schordan
// $Id: AstReverseProcessing.C,v 1.3 2008/01/08 02:56:38 dquinlan Exp $

// DQ (3/12/2006): This is now not required, the file name is 
// changed to rose_config.h and is included directly by rose.h.
// #ifdef HAVE_CONFIG_H
// #include <config.h>
// #endif

//////////////////////////////////////////////////////////////////////////////////////////
// REVERSE PRFIX PROCESSING 
//////////////////////////////////////////////////////////////////////////////////////////

// GB (09/25/2007): Added constructor to set flag to indicate that this is not a default traversal.
template<class InheritedAttributeType>
AstReversePrefixInhProcessing<InheritedAttributeType>::AstReversePrefixInhProcessing()
{
    AstTopDownProcessing<InheritedAttributeType>::set_useDefaultIndexBasedTraversal(false);
}

template<class InheritedAttributeType>
void 
AstReversePrefixInhProcessing<InheritedAttributeType>
::setNodeSuccessors(SgNode* node, typename AstReversePrefixInhProcessing<InheritedAttributeType>::SuccessorsContainer& succContainer) {
  AstSuccessorsSelectors::selectReversePrefixSuccessors(node,succContainer);
}

// GB (09/25/2007): Added constructor to set flag to indicate that this is not a default traversal.
template<class SynthesizedAttributeType>
AstReversePrefixSynProcessing<SynthesizedAttributeType>::AstReversePrefixSynProcessing()
{
    AstBottomUpProcessing<SynthesizedAttributeType>::set_useDefaultIndexBasedTraversal(false);
}

template<class SynthesizedAttributeType>
void 
AstReversePrefixSynProcessing<SynthesizedAttributeType>
::setNodeSuccessors(SgNode* node, typename AstReversePrefixSynProcessing<SynthesizedAttributeType>::SuccessorsContainer& succContainer) {
  AstSuccessorsSelectors::selectReversePrefixSuccessors(node,succContainer);
}

// GB (09/25/2007): Added constructor to set flag to indicate that this is not a default traversal.
template<class InheritedAttributeType, class SynthesizedAttributeType>
AstReversePrefixInhSynProcessing<InheritedAttributeType, SynthesizedAttributeType>::AstReversePrefixInhSynProcessing()
{
    AstTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType>::set_useDefaultIndexBasedTraversal(false);
}

template<class InheritedAttributeType, class SynthesizedAttributeType>
void 
AstReversePrefixInhSynProcessing<InheritedAttributeType, SynthesizedAttributeType>
::setNodeSuccessors(SgNode* node, typename AstReversePrefixInhSynProcessing<InheritedAttributeType, SynthesizedAttributeType>::SuccessorsContainer& succContainer) {
  AstSuccessorsSelectors::selectReversePrefixSuccessors(node,succContainer);
}

//////////////////////////////////////////////////////////////////////////////////////////
// REVERSE BRANCH PROCESSING 
//////////////////////////////////////////////////////////////////////////////////////////

// GB (09/25/2007): Added constructor to set flag to indicate that this is not a default traversal.
template<class InheritedAttributeType, class SynthesizedAttributeType>
AstReverseBranchInhSynProcessing<InheritedAttributeType, SynthesizedAttributeType>::AstReverseBranchInhSynProcessing()
{
    AstTopDownBottomUpProcessing<InheritedAttributeType, SynthesizedAttributeType>::set_useDefaultIndexBasedTraversal(false);
}

template<class InheritedAttributeType, class SynthesizedAttributeType>
void 
AstReverseBranchInhSynProcessing<InheritedAttributeType, SynthesizedAttributeType>
::setNodeSuccessors(SgNode* node, typename AstReverseBranchInhSynProcessing<InheritedAttributeType, SynthesizedAttributeType>::SuccessorsContainer& succContainer) {
  AstSuccessorsSelectors::selectReverseBranchSuccessors(node,succContainer);
}

// GB (09/25/2007): Added constructor to set flag to indicate that this is not a default traversal.
template<class InheritedAttributeType>
AstReverseBranchInhProcessing<InheritedAttributeType>::AstReverseBranchInhProcessing()
{
    AstTopDownProcessing<InheritedAttributeType>::set_useDefaultIndexBasedTraversal(false);
}

template<class InheritedAttributeType>
void 
AstReverseBranchInhProcessing<InheritedAttributeType>
::setNodeSuccessors(SgNode* node, typename AstReverseBranchInhProcessing<InheritedAttributeType>::SuccessorsContainer& succContainer) {
  AstSuccessorsSelectors::selectReverseBranchSuccessors(node,succContainer);
}

// GB (09/25/2007): Added constructor to set flag to indicate that this is not a default traversal.
template<class SynthesizedAttributeType>
AstReverseBranchSynProcessing<SynthesizedAttributeType>::AstReverseBranchSynProcessing()
{
    AstBottomUpProcessing<SynthesizedAttributeType>::set_useDefaultIndexBasedTraversal(false);
}

template<class SynthesizedAttributeType>
void 
AstReverseBranchSynProcessing<SynthesizedAttributeType>
::setNodeSuccessors(SgNode* node, typename AstReverseBranchSynProcessing<SynthesizedAttributeType>::SuccessorsContainer& succContainer) {
  AstSuccessorsSelectors::selectReverseBranchSuccessors(node,succContainer);
}

#include "AstReverseSimpleProcessing.h"

#endif
