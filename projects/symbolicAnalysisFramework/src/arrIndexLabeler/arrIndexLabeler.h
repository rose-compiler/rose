#ifndef ARR_INDEX_LABELER_H
#define ARR_INDEX_LABELER_H

#include "rose.h"
#include <list>
using namespace std;
namespace arrIndexLabeler {
	
// labels the portion of the AST rooted at root with annotations that identify the various portions
// the read/write sides of SgAssignOp nodes (if they are)
void addArrayIndexAnnotations(SgNode* root);

// returns true if the given node is part of an array index expression and false otherwise
bool isArrayIndex(SgNode* n);

// returns true the given SgPntrArrRefExp node this is a top-level SgPntrArrRefExp that is not part 
// of a larger SgPntrArrRefExp
// (i.e. given a[b[i][j]][k][l], it is either a[b[i][j]][k][l] or b[i][j])
bool isTopArrayRefExp(SgNode* n);

// returns the SgExpression node that contains the name of the array in the given SgPntrArrRefExp or 
// NULL if the node is not a SgPntrArrRefExp
SgExpression* getArrayNameExp(SgNode* n);

// returns the dimensionality of the array reference in the given SgPntrArrRefExp
int getArrayDim(SgPntrArrRefExp* n);

// returns the list of index expressionf in the given SgPntrArrRefExp
list<SgExpression*>& getArrayIndexExprs(SgPntrArrRefExp* n);
}
#endif
