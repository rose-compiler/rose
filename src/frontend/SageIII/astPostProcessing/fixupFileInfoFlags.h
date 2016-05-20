#ifndef ROSE_fixupFileInfoFlags_H
#define ROSE_fixupFileInfoFlags_H

/** Checks and fixes up inconsistanties in the settings of Sg_File_Info flags (e.g. isTransformation flag) in the Sg_File_Info object
 *
 *  We are trying to move the API for setting this into the SgLocatedNode 
 *  (since there are multiple Sg_File_Info objects (two for each SgStatement, 
 *  and two to three for each SgExpression).
 *  */
size_t fixupFileInfoInconsistanties(SgNode *ast);

#endif

