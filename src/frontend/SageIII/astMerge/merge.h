

// #include "colorTraversal.h"

#include "test_support.h"
#include "merge_support.h"
#include "nullifyAST.h"
#include "buildMangledNameMap.h"
#include "deleteOrphanNodes.h"
#include "buildReplacementMap.h"
#include "fixupTraversal.h"
#include "collectAssociateNodes.h"
#include "requiredNodes.h"

// DQ (5/27/2007): Commented out since this header file has been removed.
// #include "sageSupport.h"

// Global variable that functions can use to make sure that there IR nodes were not deleted!
extern std::set<SgNode*> finalDeleteSet;

// std::set<SgNode*> getSetOfFrontendSpecificNodes( const std::set<SgNode*> & requiredNodesTest );
// void deleteSubtrees ( std::set<SgNode*> & listToDelete );
std::set<SgNode*> getSetOfFrontendSpecificNodes();

void deleteNodes ( std::set<SgNode*> & listToDelete );

void deleteSetErrorCheck( SgProject* project, const std::set<SgNode*> & listToDelete );

void mergeAST ( SgProject* project, bool skipFrontendSpecificIRnodes = false );


// DQ (7/3/2010): Implementation of alternative appraoch to define the list 
// of redundant nodes to delete based on the detection of nodes disconnected 
// from the modified AST after the merge.
std::set<SgNode*> buildDeleteSet( SgProject* project );
