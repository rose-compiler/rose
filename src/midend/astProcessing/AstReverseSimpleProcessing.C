// tps (01/14/2010) : Switching from rose.h to sage3.
#include "sage3basic.h"

// Author: Markus Schordan
// $Id: AstReverseSimpleProcessing.C,v 1.3 2008/01/08 02:56:38 dquinlan Exp $

#ifndef ASTREVERSESIMPLEPROCESSING_C
#define ASTREVERSESIMPLEPROCESSING_C

//#include "sage3.h"
#include "AstReverseProcessing.h" 
#include "AstReverseSimpleProcessing.h"

///////////////////////////////////////////////////
//// REVERSE SIMPLE PROCESSING IMPLEMENTATIONS ////
///////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////////
// REVERSE PRFIX PROCESSING 
//////////////////////////////////////////////////////////////////////////////////////////

// GB (09/25/2007): Added constructor to set the new flag to indicate that this is not a default traversal.
AstReversePrefixSimpleProcessing::AstReversePrefixSimpleProcessing()
{
    set_useDefaultIndexBasedTraversal(false);
}

void AstReversePrefixSimpleProcessing::setNodeSuccessors(SgNode* node, SuccessorsContainer& succContainer) {
  AstSuccessorsSelectors::selectReversePrefixSuccessors(node,succContainer);
}

//////////////////////////////////////////////////////////////////////////////////////////
// REVERSE BRANCH PROCESSING 
//////////////////////////////////////////////////////////////////////////////////////////

// GB (09/25/2007): Added constructor to set the new flag to indicate that this is not a default traversal.
AstReverseBranchSimpleProcessing::AstReverseBranchSimpleProcessing()
{
    set_useDefaultIndexBasedTraversal(false);
}

void AstReverseBranchSimpleProcessing::setNodeSuccessors(SgNode* node, SuccessorsContainer& succContainer) {
  AstSuccessorsSelectors::selectReverseBranchSuccessors(node,succContainer);
}

#endif
