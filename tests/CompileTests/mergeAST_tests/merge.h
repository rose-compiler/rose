

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


// Global variable that functions can use to make sure that there IR nodes were not deleted!
extern std::set<SgNode*> finalDeleteSet;

// std::set<SgNode*> getSetOfFrontendSpecificNodes( const std::set<SgNode*> & requiredNodesTest );
// void deleteSubtrees ( std::set<SgNode*> & listToDelete );
std::set<SgNode*> getSetOfFrontendSpecificNodes();

void deleteNodes ( std::set<SgNode*> & listToDelete );

void deleteSetErrorCheck( SgProject* project, const std::set<SgNode*> & listToDelete );

