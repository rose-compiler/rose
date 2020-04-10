#ifndef __MERGE_H__
#define __MERGE_H__

#include <list>
#include <string>

class SgProject;

namespace Rose {
namespace AST {

// Load
void load(SgProject *, std::list<std::string> const &);

// Remove redundant (shareable) nodes from AST. Used when building multiple
// translation units from the same command lines, and when serialized ASTs
// are loaded from files.
void merge(SgProject *);

}
}

#endif // __MERGE_H__

