

#include "sage3basic.h"

#define DEBUG__ROSE_AST_CONSISTENCY 0

namespace Rose {
namespace AST {

void consistency(SgProject * project) {
#if DEBUG__ROSE_AST_CONSISTENCY
  printf("Rose::AST::consistency: project = %x\n", project);
#endif
  // TODO
}

} }

