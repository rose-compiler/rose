#ifndef CHECK_ISMODIFIED_FLAG_H
#define CHECK_ISMODIFIED_FLAG_H

#if 0
// DQ (4/16/2015): Replaced with better implementations.
bool checkIsModifiedFlag( SgNode *node);
bool CheckIsModifiedFlagSupport( SgNode *node);
#endif

// DQ (4/16/2015): This functions have clearer sematics and a better implementation.
ROSE_DLL_API void reportNodesMarkedAsModified(SgNode *node);
ROSE_DLL_API void unsetNodesMarkedAsModified(SgNode *node);

// DQ (4/16/2015): This function is required because it is presently used in the binary analysis.
// Note that the semantics of this function is that it also resets the isModified flags.
// It is only used in the binary analysis and we might want to have that location use 
// a different function in the near future.
bool checkIsModifiedFlag(SgNode *node);

// endif for CHECK_ISMODIFIED_FLAG_H
#endif
