#ifndef __MERGE_SUPPORT_H__
#define __MERGE_SUPPORT_H__

#include <set>
#include <string>

class SgNode;

namespace Rose {
namespace AST {

ROSE_DLL_API std::set<SgNode*> getFrontendSpecificNodes();

}
}

#endif /* __MERGE_SUPPORT_H__ */
