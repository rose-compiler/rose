#ifndef __ROSE_AST_LINK_H__
#define __ROSE_AST_LINK_H__

class SgProject;

namespace Rose {
namespace AST {

void link_variable(SgProject *);
void link_function(SgProject *);
void link_class(SgProject *);
void link_namespace(SgProject *);

}
}

#endif // __ROSE_AST_LINK_H__

