#ifndef ROSE_checkPhysicalSourcePosition_H
#define ROSE_checkPhysicalSourcePosition_H

/** Checks whether the physical source position information is consistant in the Sg_File_Info object
 *
 *  New to the EDG 4x work, we now record the logical and physical source position information.
 *  */
size_t checkPhysicalSourcePosition(SgNode *ast);

#endif

