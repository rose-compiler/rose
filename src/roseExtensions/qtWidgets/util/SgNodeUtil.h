
#ifndef SG_NODE_UTIL_H
#define SG_NODE_UTIL_H

#include <vector>

class SgNode;
class SgProject;
class SgFile;

SgFile *getSgFile( SgNode *node );

bool isBinaryNode( SgNode *node );
bool isSourceNode( SgNode *node );

typedef std::vector<SgNode *> SgNodeVector;

std::vector<SgNode *> getLinkedBinaryNodes( SgNode *node );
std::vector<SgNode *> getLinkedSourceNodes( SgNode *node );

bool isAncestor( SgNode *parent, SgNode *child );

/// Removes an SgFile from a project and deletes the file
/// file may be NULL, then nothing is done
void deleteFileFromProject(SgFile * file, SgProject * proj);

/// tries to find the target of a call or jmp
/// if successful it returns true, and the address is written to out
bool findNextInstruction(SgNode * node, rose_addr_t & out);

SgProject * getProjectOf(SgNode * node);

#endif
