#ifndef __ROSE_AST_MERGE_H__
#define __ROSE_AST_MERGE_H__

#include <list>
#include <string>

class SgProject;

namespace Rose {
namespace AST {

// Remove redundant (shareable) nodes from AST. Used when building multiple
// translation units from the same command lines, and when serialized ASTs
// are loaded from files.
void merge(SgProject *);

}
}

#endif // __ROSE_AST_MERGE_H__

