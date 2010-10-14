#ifndef RW_ACCESS_LABELER_H
#define RW_ACCESS_LABELER_H

#include <string>

namespace rwAccessLabeler {
	
typedef enum {readAccess=0, writeAccess=1, rwAccess=2} accessType;

// labels the portion of the AST rooted at root with annotations that identify the various portions
// the read/write sides of SgAssignOp nodes (if they are)
void addRWAnnotations(SgNode* root);

// returns the type of access being performed at the given node
accessType getAccessType(SgNode* n);

// copies the access type annotation from src to tgt
void cloneAccessType(SgNode* tgt, SgNode* src);

// returns a string representation of the given accessType
std::string accessTypeToStr(accessType t);

}

#endif
