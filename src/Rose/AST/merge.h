#ifndef __ROSE_AST_MERGE_H__
#define __ROSE_AST_MERGE_H__

#include <list>
#include <string>
#include <iostream>

#include "sage3basic.hhh"

class SgProject;

namespace Rose {
namespace AST {

ROSE_DLL_API std::map<SgNode*, std::map<std::string, std::tuple<SgNode*,VariantT, SgNode*>>> consistency_details();
ROSE_DLL_API bool consistency(std::ostream & out = std::cerr);

}
}

#endif // __ROSE_AST_MERGE_H__

