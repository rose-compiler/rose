#ifndef __ROSE_AST_LOAD_H__
#define __ROSE_AST_LOAD_H__

class SgProject;

namespace Rose {
namespace AST {

// Load
void load(SgProject *, std::list<std::string> const &);

}
}

#endif // __ROSE_AST_LOAD_H__

