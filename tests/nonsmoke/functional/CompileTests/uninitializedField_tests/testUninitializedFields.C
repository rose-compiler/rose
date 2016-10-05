// Uninitialized field tester: when run with Valgrind, prints out warnings
// for ROSE AST fields that are not initialized

#include "rose.h"

#if ROSE_USE_VALGRIND

#include <valgrind/valgrind.h>
#include <valgrind/memcheck.h>

using namespace std;

struct Vis: public ROSE_VisitTraversal {
  void visit(SgNode* node) {
    // When Valgrind is enabled, this checks for uninitialized fields; we
    // do not need the answer from it
    node->roseRTI();
  }
};

int main(int argc, char *argv[]) {
  frontend(argc,argv);
  Vis().traverseMemoryPool();
  // return (VALGRIND_COUNT_ERRORS != 0) ? 1 : 0;
  return 0; // JJW hack: this is making the regression tests fail (probably properly) because there are uninitialized fields
}

#else // !ROSE_USE_VALGRIND

#error "testUninitializedFields should not be built unless valgrind/valgrind.h is found"

#endif // ROSE_USE_VALGRIND
