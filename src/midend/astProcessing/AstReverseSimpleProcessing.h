// Author: Markus Schordan
// $Id: AstReverseSimpleProcessing.h,v 1.2 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTSIMPLEREVERSEPROCESSING_H
#define ASTSIMPLEREVERSEPROCESSING_H

#include "AstReverseProcessing.h"

// these functions have to be put in a different file than the other 3 templated versions to compile&link with gcc

//////////////////////////////////////////////////////////////////////////////////////////
// REVERSE PRFIX PROCESSING 
//////////////////////////////////////////////////////////////////////////////////////////

class AstReversePrefixSimpleProcessing : public AstSimpleProcessing {
 public:
  AstReversePrefixSimpleProcessing();
 protected:
  virtual void setNodeSuccessors(SgNode* node, SuccessorsContainer& succContainer);
};

//////////////////////////////////////////////////////////////////////////////////////////
// REVERSE BRANCH PROCESSING 
//////////////////////////////////////////////////////////////////////////////////////////

class AstReverseBranchSimpleProcessing : public AstSimpleProcessing {
 public:
  AstReverseBranchSimpleProcessing();
 protected:
  virtual void setNodeSuccessors(SgNode* node, SuccessorsContainer& succContainer);
};

#endif
