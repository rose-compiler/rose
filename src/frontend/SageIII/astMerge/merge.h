#ifndef __ROSE_AST_MERGE_H__
#define __ROSE_AST_MERGE_H__

#include <list>
#include <string>

#include "rosedll.h"

class SgProject;

namespace Rose {
namespace AST {

ROSE_DLL_API void load(SgProject *, std::list<std::string> const &);

ROSE_DLL_API void merge(SgProject *);

ROSE_DLL_API void shareRedundantNodes(SgProject *);
ROSE_DLL_API void deleteIslands(SgProject *);
ROSE_DLL_API void link(SgProject *);

ROSE_DLL_API void consistency(SgProject *);

ROSE_DLL_API void clear();
ROSE_DLL_API void free();

}
}

#endif // __ROSE_AST_MERGE_H__

