// Author: Markus Schordan
// $Id: AstSuccessorsSelectors.h,v 1.2 2008/01/08 02:56:39 dquinlan Exp $

#ifndef ASTSUCCESSORSSELECTORS_H
#define ASTSUCCESSORSSELECTORS_H

class ROSE_DLL_API AstSuccessorsSelectors {
 public:
  typedef std::vector<SgNode*> SuccessorsContainer; // type is used in all AstProcessing classes

  static void selectDefaultSuccessors(SgNode* node, SuccessorsContainer& succContainer);
  static void selectReversePrefixSuccessors(SgNode* node, SuccessorsContainer& succContainer);
  static void selectReverseBranchSuccessors(SgNode* node, SuccessorsContainer& succContainer);
 private:
  static SgNode* leftSibling(SgNode* node);
};

//#ifdef HAVE_EXPLICIT_TEMPLATE_INSTANTIATION 
//  #include "AstSuccessorsSelectors.C" 
//#endif 

#endif
