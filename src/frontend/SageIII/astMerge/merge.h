#ifndef __ROSE_AST_MERGE_H__
#define __ROSE_AST_MERGE_H__

#include <list>
#include <string>

class SgProject;

namespace Rose {
namespace AST {

void load(SgProject *, std::list<std::string> const &);

void merge(SgProject *);

void shareRedundantNodes(SgProject *);
void deleteIslands(SgProject *);
void deleteAll(SgProject *);
void link(SgProject *);

void consistency(SgProject *);

}
}

#endif // __ROSE_AST_MERGE_H__

